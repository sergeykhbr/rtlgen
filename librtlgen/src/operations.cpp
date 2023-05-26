// 
//  Copyright 2022 Sergey Khabarov, sergeykhbr@gmail.com
// 
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
// 
//      http://www.apache.org/licenses/LICENSE-2.0
// 
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
// 

#include "operations.h"
#include "array.h"
#include "utils.h"
#include "modules.h"
#include "comments.h"
#include <cstring>

namespace sysvc {

int spaces_ = 1;
int stackcnt_ = 0;
GenObject *stackobj_[256] = {0};

Operation::Operation(const char *comment)
    : GenObject(stackobj_[stackcnt_], "", ID_OPERATION, "", comment), igen_(0), argcnt_(0) {
}

Operation::Operation(GenObject *parent, const char *comment)
    : GenObject(parent, "", ID_OPERATION, "", comment), igen_(0), argcnt_(0) {
}

void Operation::start(GenObject *owner) {
    stackcnt_ = 0;
    stackobj_[stackcnt_] = owner;
}

void Operation::push_obj(GenObject *obj) {
    stackobj_[++stackcnt_] = obj;
}

void Operation::pop_obj() {
    stackobj_[stackcnt_--] = 0;
}

void Operation::set_space(int n) {
    spaces_ = n;
}
int Operation::get_space() {
    return spaces_;
}
std::string Operation::addspaces() {
    std::string ret = "";
    for (int i = 0; i < 4*spaces_; i++) {
        ret += " ";
    }
    return ret;
}

std::string Operation::addtext(GenObject *obj, size_t curpos) {
    std::string ret = "";
    if (obj->getComment().size()) {
        while (curpos++ < 60) {
            ret += " ";
        }
        ret += "// " + obj->getComment();
    }
    return ret;
}

std::string Operation::fullname(const char *prefix, std::string name, GenObject *obj) {
    if (!obj) {
        return name;
    }
    std::string read = "";
#if 1
    if (obj->getName() == "i_l1i") {
        bool st = true;
    }
#endif
    GenObject *p = obj->getParent();
    std::string curname = "";
    if (p && p->getId() == ID_ARRAY_DEF) {
        curname = fullname(prefix, name, p);
        // Do not add 'name' to avoid double adding
    } else if (obj->getId() == ID_CONST) {
        curname = obj->getStrValue();
        curname += name;
    } else if (obj->getId() == ID_OPERATION) {
        curname = obj->generate();
        curname += name;
    } else if (obj->getSelector()) {
        curname = "";
        curname = fullname("r", curname, obj->getSelector());
        curname = "[" + curname + "]";
        curname = obj->getName() + curname;
        if (name.size()) {
            curname += ".";
        }
        curname += name;
        obj->setSelector(0);
    } else if (obj->getId() == ID_STRUCT_INST
                || obj->getId() == ID_STRUCT_DEF) {
        curname = obj->getName();
        if (name.size()) {
            curname += ".";
        }
        curname += name;
    } else if (obj->getId() == ID_DEF_PARAM && SCV_is_sysc()) {
        curname = obj->getName() + name + "_";
    } else {
        curname = obj->getName() + name;
    }

    if (p && p->isInput()) {
        if (SCV_is_sysc()) {
            read += "read().";
        }
    }

    if (p && (p->getId() == ID_STRUCT_INST
            || p->getId() == ID_STRUCT_DEF
            || p->getId() == ID_VECTOR)) {
        curname = fullname(prefix, read + curname, obj->getParent());
    } else if (obj->isReg()) {
        curname = std::string(prefix) + "." + read + curname;
    } else if (obj->isNReg()) {
        curname = std::string("n") + std::string(prefix) + "." + read + curname;
    }
    return curname;
}

std::string Operation::obj2varname(GenObject *obj, const char *prefix, bool read) {
    std::string ret = "";
    if (!obj) {
        return ret;
    }
    ret = fullname(prefix, ret, obj);

    if (read) {
        if (obj->isInput()
            || (prefix[0] == 'r' && obj->isSignal())) {
            if (SCV_is_sysc()) {
                ret += ".read()";
            }
        }
    }
    return ret;
}

// dst = r or v
// src = r, v or 0
std::string Operation::copyreg_entry(char *idx, std::string dst, std::string src, GenObject *p) {
    std::string ret = "";
    std::string i = std::string(idx);
    if (p->getId() == ID_ARRAY_DEF) {
        ret += Operation::addspaces();
        ret += "for (int "+i+" = 0; "+i+" < " + p->getStrDepth() + "; "+i+"++) ";
        if (SCV_is_sysc()) {
            ret += "{\n";
        } else {
            ret += "begin\n";
        }
        spaces_++;
        std::list<GenObject *>::iterator it = p->getEntries().begin();  // element[0]
        if ((*it)->getEntries().size() == 0) {
            ret += Operation::addspaces();
            ret +=  std::string(dst) + "." + p->getName() + "["+i+"]";
            if (SCV_is_sysc()) {
                ret += " = ";
            } else {
                if (dst[0] == 'r' && (dst[1] == '\0' || dst[1] == '.')) {
                    ret += " <= ";
                } else {
                    ret += " = ";
                }
            }
            if (src.size() == 0) {
                // reset
                ret += p->getItem()->getStrValue();
            } else {
                // copy data
                ret += src + "." + p->getName() + "["+i+"]";
            }
            ret +=  ";\n";
        } else {
            std::string tdst = dst;
            std::string tsrc = src;
            tdst += "." + p->getName() + "["+i+"]";
            if (tsrc.size()) {
                tsrc += "." + p->getName() + "["+i+"]";
            }
            idx[0]++;
            for (auto &s: (*it)->getEntries()) {
                ret += copyreg_entry(idx, tdst, tsrc, s);
            }
            idx[0]--;
        }
        spaces_--;
        ret += Operation::addspaces();
        if (SCV_is_sysc()) {
            ret += "}\n";
        } else {
            ret += "end\n";
        }
    } else {
        ret += Operation::addspaces();
        ret += dst + "." + p->getName();
        if (SCV_is_sysc()) {
            ret += " = ";
        } else {
            if (dst[0] == 'r' && (dst[1] == '\0' || dst[1] == '.')) {
                ret += " <= ";
            } else {
                ret += " = ";
            }
        }
        if (src.size() == 0) {
            // reset
            if (SCV_is_sv() && p->getWidth() > 64 && p->getValue() == 0) {
                ret += "'0";
            } else {
                ret += p->getStrValue();
            }
        } else {
            // copy value
            ret += src + "." + p->getName();
        }
        ret += ";\n";
    }
    return ret;
}

std::string Operation::copyreg(const char *dst, const char *src, ModuleObject *m) {
    std::string ret = "";
    std::string t_src = "";
    if (src) {
        t_src = std::string(src);
    }
    if (!m->is2DimReg() && (t_src == "r" || t_src == "v" || t_src == "rin"
                        || t_src == "nr" || t_src == "nv" || t_src == "nrin")) {
        ret += Operation::addspaces();
        if (SCV_is_sysc()) {
            ret += std::string(dst) + " = " + t_src + ";\n";
        } else {
            if (dst[0] == 'r' && (dst[1] == '\0' || dst[1] == '.')) {
                ret += std::string(dst) + " <= " + t_src + ";\n";
            } else if (dst[0] == 'n' && dst[1] == 'r' && (dst[2] == '\0' || dst[2] == '.')) {
                ret += std::string(dst) + " <= " + t_src + ";\n";
            } else {
                ret += std::string(dst) + " = " + t_src + ";\n";
            }
        }
    } else {
        // reset each register separatly (warning: not implemented for negedge clock!!)
        for (auto &p: m->getEntries()) {
            if (!p->isReg()) {
                continue;
            }
            if (src == 0 && p->isResetDisabled()) {
                continue;
            }

            char idx[2] = "i";  // cycle index variable
            std::string tdst = std::string(dst);
            std::string tsrc;
            if (src) {
                tsrc = std::string(src);
            }
            ret += copyreg_entry(idx, tdst, tsrc, p);
        }
    }
    return ret;
}

std::string Operation::reset(const char *dst, const char *src, ModuleObject *m, std::string xrst) {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret += Operation::addspaces();
        if (dst[0] == 'v') {
            ret += "if ";
            if (xrst.size()) {
                ret += "(";
            }
            ret += "(!async_reset_ && ";
            ret += m->getResetPort()->getName() + ".read() == ";
            if (m->getResetActive()) {
                ret += "1";
            } else {
                ret += "0";
            }
            ret += ")";
            if (xrst.size()) {
                ret += " || " + xrst + ")";
            }
            ret += " {\n";
        } else {
            ret += "if (";
            if (m->getAsyncReset()) {
                ret += "async_reset_ && ";
            }
            ret += m->getResetPort()->getName() + ".read() == ";
            if (m->getResetActive() == 0) {
                ret += "0";
            } else {
                ret += "1";
            }
            ret += ") {\n";
        }

        spaces_++;
        if (!m->is2DimReg()) {
            if (src == 0) {
                // reset using function
                ret += Operation::addspaces();
                if (dst[0] == 'n' && dst[1] == 'r' && (dst[2] == '\0' || dst[2] == '.')) {
                    ret += m->getType() + "_nr_reset(" + std::string(dst) + ")";
                } else {
                    ret += m->getType() + "_r_reset(" + std::string(dst) + ")";
                }
            } else {
                // copy data from src into dst
                ret += Operation::addspaces();
                ret += std::string(dst) + " = " + std::string(src);
            }
            ret += ";\n";
        } else {
            ret += copyreg(dst, src, m);
        }
        spaces_--;
        ret += Operation::addspaces() + "}";
    } else if (SCV_is_sv()) {
        ret += Operation::addspaces();
        if (dst[0] == 'v') {
            ret += "if ";
            if (xrst.size()) {
                ret += "(";
            }
            ret += "(~async_reset && ";
            ret += m->getResetPort()->getName() + " == ";
            if (!m->getResetActive()) {
                ret += "1'b0";
            } else {
                ret += "1'b1";
            }
            ret += ")";
            if (xrst.size()) {
                ret += " || " + xrst + ")";
            }
            ret += " begin\n";
        } else {
            ret += "if (" + m->getResetPort()->getName() + " == ";
            if (!m->getResetActive()) {
                ret += "1'b0";
            } else {
                ret += "1'b1";
            }
            ret += ") begin\n";
        }

        spaces_++;
        if (!m->is2DimReg()) {
            if (src == 0) {
                // reset using function
                ret += Operation::addspaces();
                if (dst[0] == 'r' && (dst[1] == '\0' || dst[1] == '.')) {
                    ret += std::string(dst) + " <= " + m->getType() + "_r_reset";
                } else if (dst[0] == 'n' && dst[1] == 'r' && (dst[2] == '\0' || dst[2] == '.')) {
                    ret += std::string(dst) + " <= " + m->getType() + "_nr_reset";
                } else {
                    ret += std::string(dst) + " = " + m->getType() + "_r_reset";
                }
            } else {
                // copy data from src into dst
                ret += Operation::addspaces();
                if (dst[0] == 'r' && (dst[1] == '\0' || dst[1] == '.')) {
                    ret += std::string(dst) + " <= " + std::string(src);
                } else if (dst[0] == 'n' && dst[1] == 'r' && (dst[2] == '\0' || dst[2] == '.')) {
                    ret += std::string(dst) + " <= " + std::string(src);
                } else {
                    ret += std::string(dst) + " = " + std::string(src);
                }
            }
            ret += ";\n";
        } else {
            ret += copyreg(dst, src, m);
        }
        spaces_--;
        ret += Operation::addspaces() + "end";
    } else {
    }
    return ret;
}


// TEXT
std::string TEXT_gen(GenObject **args) {
    std::string ret = "";
    if (args[0]->getComment().size() == 0) {
        // Do nothing
    } else {
        ret = Operation::addspaces() + "// " + args[0]->getComment();
    }
    ret += "\n";
    return ret;
}
void TEXT(const char *comment) {
    Operation *p = new Operation(comment);
    p->igen_ = TEXT_gen;
    p->add_arg(p);
}

// ALLZEROS
std::string ALLZEROS_gen(GenObject **args) {
    std::string ret = "";
    if (SCV_is_sysc()) {
         ret += "0";
    } else if (SCV_is_sv()) {
         ret += "'0";
    } else {
        ret += "(others => '0')";
    }
    return ret;
}

Operation &ALLZEROS(const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = ALLZEROS_gen;
    p->add_arg(p);
    return *p;
}

// ALLONES
std::string ALLONES_gen(GenObject **args) {
    std::string ret = "";
    if (SCV_is_sysc()) {
         ret += "~0ull";
    } else if (SCV_is_sv()) {
         ret += "'1";
    } else {
        ret += "(others => '1')";
    }
    return ret;
}

Operation &ALLONES(const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = ALLONES_gen;
    p->add_arg(p);
    return *p;
}


// BIT
std::string BIT_gen(GenObject **args) {
    std::string ret = "";
    ret += Operation::obj2varname(args[1], "r", true);
    if (SCV_is_sysc()) {
         ret += "[";
         ret += Operation::obj2varname(args[2], "r", true);
         ret += "]";
    } else if (SCV_is_sv()) {
         ret += "[";
         ret += Operation::obj2varname(args[2]);
         ret += "]";
    } else {
         ret += "(";
         ret += Operation::obj2varname(args[2]);
         ret += ")";
    }
    return ret;
}

Operation &BIT(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->setWidth(1);
    p->igen_ = BIT_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}

Operation &BIT(GenObject &a, const char *b, const char *comment) {
    GenObject *t1 = new I32D(b);
    return BIT(a, *t1, comment);
}

Operation &BIT(GenObject &a, int b, const char *comment) {
    char tstr[64];
    RISCV_sprintf(tstr, sizeof(tstr), "%d", b);
    return (BIT(a, tstr, comment));
}

// BITS
std::string BITS_gen(GenObject **args) {
    std::string ret = "";
    bool use_shift = false;
    std::string t1 = args[1]->getType();
    if (SCV_is_sysc() && 
        (t1 == "uint64_t" || t1 == "int" || t1 == "uint32_t" || t1 == "uint16_t")) {
        use_shift = true;
        ret += "(";
    }
    ret += Operation::obj2varname(args[1], "r", true);
    if (use_shift) {
        ret += " >> " + Operation::obj2varname(args[3], "r", true);
        ret += ")";
    } else if (SCV_is_sysc()) {
        ret += "(";
        ret += Operation::obj2varname(args[2], "r", true);
        ret += ", ";
        ret += Operation::obj2varname(args[3], "r", true);
        ret += ")";
    } else if (SCV_is_sv()) {
        ret += "[";
        ret += Operation::obj2varname(args[2], "r", true);
        ret += ": ";
        ret += Operation::obj2varname(args[3], "r", true);
        ret += "]";
    } else {
        ret += "(";
        ret += Operation::obj2varname(args[2], "r", true);
        ret += " downto ";
        ret += Operation::obj2varname(args[3], "r", true);
        ret += ")";
    }
    return ret;
}

Operation &BITS(GenObject &a, GenObject &h, GenObject &l, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->setWidth(static_cast<int>(h.getValue() - l.getValue()) + 1);
    p->igen_ = BITS_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&h);
    p->add_arg(&l);
    return *p;
}

Operation &BITS(GenObject &a, int h, int l, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->setWidth(h - l + 1);
    char tstr[64];
    p->igen_ = BITS_gen;
    p->add_arg(p);
    p->add_arg(&a);
    RISCV_sprintf(tstr, sizeof(tstr), "%d", h);
    p->add_arg(new I32D(tstr));
    RISCV_sprintf(tstr, sizeof(tstr), "%d", l);
    p->add_arg(new I32D(tstr));
    return *p;
}

// BITSW
std::string BITSW_gen(GenObject **args) {
    std::string ret = "";
    ret += Operation::obj2varname(args[1], "r", true);
    if (SCV_is_sysc()) {
         ret += "(";
         ret += Operation::obj2varname(args[2], "r", true);
         ret += " + ";
         ret += Operation::obj2varname(args[3], "r", true);
         ret += " - 1, ";
         ret += Operation::obj2varname(args[2], "r", true);
         ret += ")";
    } else if (SCV_is_sv()) {
         ret += "[";
         ret += Operation::obj2varname(args[2], "r", true);
         ret += " +: ";
         ret += Operation::obj2varname(args[3], "r", true);
         ret += "]";
    } else {
         ret += "(";
         ret += Operation::obj2varname(args[2], "r", true);
         ret += " + ";
         ret += Operation::obj2varname(args[3], "r", true);
         ret += " - 1 downto ";
         ret += Operation::obj2varname(args[2], "r", true);
         ret += ")";
    }
    return ret;
}

Operation &BITSW(GenObject &a, GenObject &start, GenObject &width, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->setWidth(static_cast<int>(width.getValue()));
    p->igen_ = BITSW_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&start);
    p->add_arg(&width);
    return *p;
}


// CONST
GenObject &CONST(const char *val) {
    GenObject *p = new I32D(val);
    return *p;
}

GenObject &CONST(const char *val, const char *width) {
    GenObject *p = new Logic(width, "", val);
    return *p;
}

GenObject &CONST(const char *val, int width) {
    char tstr[64];
    RISCV_sprintf(tstr, sizeof(tstr), "%d", width);
    return CONST(val, tstr);
}

// SETZERO
std::string SETZERO_gen(GenObject **args) {
    std::string ret = Operation::addspaces();
    ret += Operation::obj2varname(args[1], "v");
    if (SCV_is_sysc()) {
        ret += " = 0";
    } else if (SCV_is_sv()) {
        if (args[1]->getWidth() == 1) {
            ret += " = 1'b0";
        } else {
            ret += " = '0";
        }
    } else  {
        ret += " = (others => '0')";
    }
    ret +=  + ";";
    ret += Operation::addtext(args[0], ret.size());
    ret += "\n";
    return ret;
}

Operation &SETZERO(GenObject &a, const char *comment) {
    Operation *p = new Operation(comment);
    p->igen_ = SETZERO_gen;
    p->add_arg(p);
    p->add_arg(&a);
    return *p;
}


// SETONE
std::string SETONE_gen(GenObject **args) {
    std::string ret = Operation::addspaces();
    ret += Operation::obj2varname(args[1], "v");
    if (SCV_is_sysc()) {
        ret += " = 1";
    } else if (SCV_is_sv()) {
        if (args[1]->getWidth() == 1) {
            ret += " = 1'b1";
        } else {
            char tstr[64];
            RISCV_sprintf(tstr, sizeof(tstr), "%d'd1", args[1]->getWidth());
            ret += " = " + std::string(tstr);
        }
    } else  {
        ret += " = '1'";
    }
    ret +=  + ";";
    ret += Operation::addtext(args[0], ret.size());
    ret += "\n";
    return ret;
}

Operation &SETONE(GenObject &a, const char *comment) {
    Operation *p = new Operation(comment);
    p->igen_ = SETONE_gen;
    p->add_arg(p);
    p->add_arg(&a);    // output signal
    return *p;
}

// SETBIT
std::string SETBIT_gen(GenObject **args) {
    std::string ret = Operation::addspaces();
    ret += Operation::obj2varname(args[1], "v");
    if (SCV_is_sysc() || SCV_is_sv()) {
        ret += "[";
        ret += Operation::obj2varname(args[2]);
        ret += "] = " + Operation::obj2varname(args[3], "r", true) + ";";
    } else {
        ret += "(";
        ret += Operation::obj2varname(args[2]);
        ret += ") := " + Operation::obj2varname(args[3]) + ";";
    }
    ret += Operation::addtext(args[0], ret.size());
    ret += "\n";
    return ret;
}

Operation &SETBIT(GenObject &a, GenObject &b, GenObject &val, const char *comment) {
    Operation *p = new Operation(comment);
    p->igen_ = SETBIT_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    p->add_arg(&val);
    return *p;
}

Operation &SETBIT(GenObject &a, int b, GenObject &val, const char *comment) {
    char tstr[64];
    RISCV_sprintf(tstr, sizeof(tstr), "%d", b);
    GenObject *t1 = new I32D(tstr);
    return SETBIT(a, *t1, val, comment);
}

// SETBITONE
std::string SETBITONE_gen(GenObject **args) {
    std::string ret = Operation::addspaces();
    ret += Operation::obj2varname(args[1], "v");
    if (SCV_is_sysc()) {
        ret += "[";
        ret += Operation::obj2varname(args[2]);
        ret += "] = 1;";
    } else if (SCV_is_sv()) {
        ret += "[";
        ret += Operation::obj2varname(args[2]);
        ret += "] = 1'b1;";
    } else {
        ret += "(";
        ret += Operation::obj2varname(args[2]);
        ret += ") := '1';";
    }
    ret += Operation::addtext(args[0], ret.size());
    ret += "\n";
    return ret;
}

Operation &SETBITONE(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Operation(comment);
    p->setWidth(1);
    p->igen_ = SETBITONE_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}
Operation &SETBITONE(GenObject &a, const char *b, const char *comment) {
    GenObject *t1 = new I32D(b);
    return SETBITONE(a, *t1, comment);
}

Operation &SETBITONE(GenObject &a, int b, const char *comment) {
    char tstr[64];
    RISCV_sprintf(tstr, sizeof(tstr), "%d", b);
    return SETBITONE(a, tstr, comment);
}

// SETBITZERO
std::string SETBITZERO_gen(GenObject **args) {
    std::string ret = Operation::addspaces();
    ret += Operation::obj2varname(args[1], "v");
    if (SCV_is_sysc()) {
        ret += "[";
        ret += Operation::obj2varname(args[2]);
        ret += "] = 0;";
    } else if (SCV_is_sv()) {
        ret += "[";
        ret += Operation::obj2varname(args[2]);
        ret += "] = 1'b0;";
    } else {
        ret += "(";
        ret += Operation::obj2varname(args[2]);
        ret += ") := '0';";
    }
    ret += Operation::addtext(args[0], ret.size());
    ret += "\n";
    return ret;
}

Operation &SETBITZERO(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Operation(comment);
    p->setWidth(1);
    p->igen_ = SETBITZERO_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}
Operation &SETBITZERO(GenObject &a, const char *b, const char *comment) {
    GenObject *t1 = new I32D(b);
    return SETBITZERO(a, *t1, comment);
}

Operation &SETBITZERO(GenObject &a, int b, const char *comment) {
    char tstr[64];
    RISCV_sprintf(tstr, sizeof(tstr), "%d", b);
    return SETBITZERO(a, tstr, comment);
}

// SETBITS
std::string SETBITS_gen(GenObject **args) {
    std::string ret = Operation::addspaces();
    ret += Operation::obj2varname(args[1], "v");
    if (SCV_is_sysc()) {
         ret += "(";
         ret += Operation::obj2varname(args[2]);
         ret += ", ";
         ret += Operation::obj2varname(args[3]);
         ret += ") = " + Operation::obj2varname(args[4]) + ";";
    } else if (SCV_is_sv()) {
         ret += "[";
         ret += Operation::obj2varname(args[2]);
         ret += ": ";
         ret += Operation::obj2varname(args[3]);
         ret += "] = " + Operation::obj2varname(args[4]) + ";";
    } else {
    }
    ret += Operation::addtext(args[0], ret.size());
    ret += "\n";
    return ret;
}

Operation &SETBITS(GenObject &a, GenObject &h, GenObject &l, GenObject &val, const char *comment) {
    Operation *p = new Operation(comment);
    p->igen_ = SETBITS_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&h);
    p->add_arg(&l);
    p->add_arg(&val);
    return *p;
}

Operation &SETBITS(GenObject &a, int h, int l, GenObject &val, const char *comment) {
    Operation *p = new Operation(comment);
    char tstr[64];
    p->igen_ = SETBITS_gen;
    p->add_arg(p);
    p->add_arg(&a);
    RISCV_sprintf(tstr, sizeof(tstr), "%d", h);
    p->add_arg(new I32D(tstr));
    RISCV_sprintf(tstr, sizeof(tstr), "%d", l);
    p->add_arg(new I32D(tstr));
    p->add_arg(&val);
    return *p;
}

// SETBITSW
std::string SETBITSW_gen(GenObject **args) {
    std::string ret = Operation::addspaces();
    ret += Operation::obj2varname(args[1], "v");
    if (SCV_is_sysc()) {
         ret += "(";
         ret += Operation::obj2varname(args[2]);
         ret += " + ";
         ret += Operation::obj2varname(args[3]);
         ret += "- 1, ";
         ret += Operation::obj2varname(args[2]);
         ret += ") = " + Operation::obj2varname(args[4]) + ";";
    } else if (SCV_is_sv()) {
         ret += "[";
         ret += Operation::obj2varname(args[2]);
         ret += " +: ";
         ret += Operation::obj2varname(args[3]);
         ret += "] = " + Operation::obj2varname(args[4]) + ";";
    } else {
    }
    ret += Operation::addtext(args[0], ret.size());
    ret += "\n";
    return ret;
}

Operation &SETBITSW(GenObject &a, GenObject &start, GenObject &width, GenObject &val, const char *comment) {
    Operation *p = new Operation(comment);
    p->igen_ = SETBITSW_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&start);
    p->add_arg(&width);
    p->add_arg(&val);
    return *p;
}

// SETVAL
std::string SETVAL_gen(GenObject **args) {
    std::string ret = Operation::addspaces();
    ret += Operation::obj2varname(args[1], "v") + " = ";
    if (args[2]->getId() == ID_CONST) {
        ret += args[2]->getStrValue();
    } else if (args[2]->getId() == ID_VALUE
            || args[2]->getId() == ID_PARAM
            || args[2]->getId() == ID_DEF_PARAM
            || args[2]->getId() == ID_TMPL_PARAM
            || args[2]->getId() == ID_STRUCT_INST
            || args[2]->getId() == ID_VECTOR) {
        ret += Operation::obj2varname(args[2]);
    } else {
        ret += args[2]->generate();
    }
    ret += ";";
    ret += Operation::addtext(args[0], ret.size());
    ret += "\n";
    return ret;
}

Operation &SETVAL(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Operation(comment);
    p->igen_ = SETVAL_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}

// SETSTR
Operation &SETSTR(GenObject &a, const char *str, const char *comment) {
    Operation *p = new Operation(comment);
    p->igen_ = SETVAL_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(new STRING(str, ""));
    return *p;
}

// SETSTRF
std::string SETSTRF_gen(GenObject **args) {
    std::string ret = Operation::addspaces();
    std::string fmt;
    if (SCV_is_sysc()) {
        ret += "RISCV_sprintf(tstr, sizeof(tstr), ";
    } else {
        ret += Operation::obj2varname(args[1]);
        if (args[4]) {
            ret += " += ";
        } else {
            ret += " = ";
        }
        ret += "$sformatf(";
    }
    fmt = args[2]->getStrValue();
    if (SCV_is_sv()) {
        // Remove RV_PRI64 occurences:
        const char *pos;
        while (pos = strstr(fmt.c_str(), "\" RV_PRI64 \"")) {
            fmt.erase(pos - fmt.c_str(), 12);
        }
    }
    ret += fmt;
    size_t cnt = reinterpret_cast<size_t>(args[3]);
    Operation::set_space(Operation::get_space() + 2);
    if (cnt > 1) {
        ret += ",\n" + Operation::addspaces();
    } else {
        ret += ", ";
    }
    for (size_t i = 0; i < cnt; i++) {
        if (i > 0) {
            ret += ",\n" + Operation::addspaces();
        }
        ret += Operation::obj2varname(args[5 + i]);
        if (SCV_is_sysc()
            && args[5+i]->getId() == ID_DEF_PARAM
            && strstr(args[5+i]->getType().c_str(), "string")) {
            ret += ".c_str()";
        }
    }
    Operation::set_space(Operation::get_space() - 2);
    ret += ");\n";
    if (SCV_is_sysc()) {
        ret += Operation::addspaces() + Operation::obj2varname(args[1]) + " ";
        if (args[4]) {
            ret += "+";
        }
        ret += "= std::string(tstr);\n";
    }
    return ret;
}

Operation &SETSTRF(GenObject &a, const char *fmt, size_t cnt, ...) {
    Operation *p = new Operation("");
    GenObject *obj;
    p->igen_ = SETSTRF_gen;
    p->add_arg(p);  // 0
    p->add_arg(&a); // 1
    p->add_arg(new STRING(fmt, "")); // 2
    p->add_arg(reinterpret_cast<GenObject *>(cnt)); // 3
    p->add_arg(0); // 4
    va_list arg;
    va_start(arg, cnt);
    for (int i = 0; i < cnt; i++) {
        obj = va_arg(arg, GenObject *);
        p->add_arg(obj);    // 5+
    }
    va_end(arg);
    return *p;
}

Operation &ADDSTRF(GenObject &a, const char *fmt, size_t cnt, ...) {
    Operation *p = new Operation("");
    GenObject *obj;
    p->igen_ = SETSTRF_gen;
    p->add_arg(p);  // 0
    p->add_arg(&a); // 1
    p->add_arg(new STRING(fmt, ""));    // 2
    p->add_arg(reinterpret_cast<GenObject *>(cnt)); // 3
    p->add_arg(reinterpret_cast<GenObject *>(1));   // 4
    va_list arg;
    va_start(arg, cnt);
    for (int i = 0; i < cnt; i++) {
        obj = va_arg(arg, GenObject *);
        p->add_arg(obj);    // 5+
    }
    va_end(arg);
    return *p;
}


// BIG_TO_U64: explicit conersion of biguint to uint64 (sysc only)
std::string BIG_TO_U64_gen(GenObject **args) {
    std::string A = "";
    if (SCV_is_sysc()) {
        A = Operation::obj2varname(args[1], "r", true) + ".to_uint64()";
    } else {
        A = Operation::obj2varname(args[1], "r", true);
    }
    return A;
}

Operation &BIG_TO_U64(GenObject &a, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->setWidth(a.getWidth());
    p->igen_ = BIG_TO_U64_gen;
    p->add_arg(p);
    p->add_arg(&a);
    return *p;
}

// TO_BIG
std::string TO_BIG_gen(GenObject **args) {
    std::string A = "";
    if (SCV_is_sysc()) {
        char tstr[64];
        size_t sz = reinterpret_cast<size_t>(args[1]);
        RISCV_sprintf(tstr, sizeof(tstr), "%d", sz);
        A = "sc_biguint<" + std::string(tstr) + ">(";
        A += Operation::obj2varname(args[2], "r", true) + ")";
    } else {
        A += Operation::obj2varname(args[2], "r", true);
    }
    return A;
}

Operation &TO_BIG(size_t sz, GenObject &a) {
    Operation *p = new Operation(0, "");
    p->setWidth(static_cast<int>(sz));
    p->igen_ = TO_BIG_gen;
    p->add_arg(p);
    p->add_arg(reinterpret_cast<GenObject *>(sz));
    p->add_arg(&a);
    return *p;
}

// TO_INT
std::string TO_INT_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    if (SCV_is_sysc()) {
        if (args[1]->getId() != ID_PARAM) { // params aren't use sc_uint<> tempalates
            A = A + ".to_int()";
        }
    } else {
        A = "int'(" + A + ")";
    }
    return A;
}

Operation &TO_INT(GenObject &a, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->setWidth(32);
    p->igen_ = TO_INT_gen;
    p->add_arg(p);
    p->add_arg(&a);
    return *p;
}

// TO_U32
std::string TO_U32_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    if (SCV_is_sysc()) {
        A = A + ".to_uint()";
    }
    return A;
}

Operation &TO_U32(GenObject &a, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->setWidth(32);
    p->igen_ = TO_U32_gen;
    p->add_arg(p);
    p->add_arg(&a);
    return *p;
}

// TO_U64
std::string TO_U64_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    if (SCV_is_sysc()) {
        A = A + ".to_uint64()";
    }
    return A;
}

Operation &TO_U64(GenObject &a, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->setWidth(64);
    p->igen_ = TO_U64_gen;
    p->add_arg(p);
    p->add_arg(&a);
    return *p;
}

// TO_CSTR
std::string TO_CSTR_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    if (SCV_is_sysc()) {
        A = A + ".c_str()";
    }
    return A;
}

Operation &TO_CSTR(GenObject &a, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = TO_CSTR_gen;
    p->add_arg(p);
    p->add_arg(&a);
    return *p;
}

// EQ
std::string EQ_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    std::string B = Operation::obj2varname(args[2], "r", true);
    A = "(" + A + " == " + B + ")";
    return A;
}

Operation &EQ(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->setWidth(1);
    p->igen_ = EQ_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}


// NE
std::string NE_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    std::string B = Operation::obj2varname(args[2], "r", true);
    A = "(" + A + " != " + B + ")";
    return A;
}

Operation &NE(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->setWidth(1);
    p->igen_ = NE_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}


// EZ
std::string EZ_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    if (args[1]->getWidth() > 1) {
        if (SCV_is_sysc()) {
            A += ".or_reduce()";
        } else if (SCV_is_sv()) {
            A = "(|" + A + ")";
        }
    }
    if (SCV_is_sysc()) {
        A = "(" + A + " == 0)";
    } else {
        A = "(" + A + " == 1'b0)";
    }
    return A;
}

Operation &EZ(GenObject &a, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = EZ_gen;
    p->setWidth(1);
    p->add_arg(p);
    p->add_arg(&a);
    return *p;
}

// NZ
std::string NZ_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    if (args[1]->getWidth() > 1) {
        if (SCV_is_sysc()) {
            A += ".or_reduce()";
        } else if (SCV_is_sv()) {
            A = "(|" + A + ")";
        }
    }
    if (SCV_is_sysc()) {
        A = "(" + A + " == 1)";
    } else {
        A = "(" + A + " == 1'b1)";
    }
    return A;
}

Operation &NZ(GenObject &a, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = NZ_gen;
    p->setWidth(1);
    p->add_arg(p);
    p->add_arg(&a);
    return *p;
}

// GT
std::string GT_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    std::string B = Operation::obj2varname(args[2], "r", true);
    A = "(" + A + " > " + B + ")";
    return A;
}

Operation &GT(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = GT_gen;
    p->setWidth(1);
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}

// GE
std::string GE_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    std::string B = Operation::obj2varname(args[2], "r", true);
    A = "(" + A + " >= " + B + ")";
    return A;
}

Operation &GE(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = GE_gen;
    p->setWidth(1);
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}

// LS
std::string LS_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    std::string B = Operation::obj2varname(args[2], "r", true);
    A = "(" + A + " < " + B + ")";
    return A;
}

Operation &LS(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = LS_gen;
    p->setWidth(1);
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}

// LE
std::string LE_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    std::string B = Operation::obj2varname(args[2], "r", true);
    A = "(" + A + " <= " + B + ")";
    return A;
}

Operation &LE(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = LE_gen;
    p->setWidth(1);
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}

// INV_L
std::string INV_L_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    A = "(~" + A + ")";
    return A;
}

Operation &INV_L(GenObject &a, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->setWidth(a.getWidth());
    p->igen_ = INV_L_gen;
    p->add_arg(p);
    p->add_arg(&a);
    return *p;
}

// INV
std::string INV_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1]);
    if (SCV_is_sysc()) {
        A = "(!" + A + ")";
    } else {
        A = "(~" + A + ")";
    }
    return A;
}

Operation &INV(GenObject &a, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->setWidth(a.getWidth());
    p->igen_ = INV_gen;
    p->add_arg(p);
    p->add_arg(&a);
    return *p;
}

// OR2_L
std::string OR2_L_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    std::string B = Operation::obj2varname(args[2], "r", true);
    A = "(" + A + " | " + B + ")";
    return A;
}

Operation &OR2_L(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->setWidth(a.getWidth() > b.getWidth() ? a.getWidth() : b.getWidth());
    p->igen_ = OR2_L_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}

// OR2
std::string OR2_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1]);
    std::string B = Operation::obj2varname(args[2]);
    A = "(" + A + " || " + B + ")";
    return A;
}

Operation &OR2(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->setWidth(a.getWidth() > b.getWidth() ? a.getWidth() : b.getWidth());
    p->igen_ = OR2_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}

// OR3
std::string OR3_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1]);
    std::string B = Operation::obj2varname(args[2]);
    std::string C = Operation::obj2varname(args[3]);
    A = "(" + A + " || " + B + " || " + C + ")";
    return A;
}

Operation &OR3(GenObject &a, GenObject &b, GenObject &c, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = OR3_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    p->add_arg(&c);
    return *p;
}

// OR4
std::string OR4_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1]);
    std::string B = Operation::obj2varname(args[2]);
    std::string C = Operation::obj2varname(args[3]);
    std::string D = Operation::obj2varname(args[4]);
    A = "(" + A + " || " + B + " || " + C + " || " + D + ")";
    return A;
}

Operation &OR4(GenObject &a, GenObject &b, GenObject &c, GenObject &d, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = OR4_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    p->add_arg(&c);
    p->add_arg(&d);
    return *p;
}

// ORx
std::string ORx_gen(GenObject **args) {
    std::string ret = "(";
    size_t cnt = reinterpret_cast<size_t>(args[1]);
    Operation::set_space(Operation::get_space() + 2);
    for (size_t i = 0; i < cnt; i++) {
        if (i > 0) {
            ret += "\n";
            ret += Operation::addspaces();
            ret += "|| ";
        }
        ret += Operation::obj2varname(args[2 + i]);
    }
    Operation::set_space(Operation::get_space() - 2);
    ret += ")";
    return ret;
}

Operation &ORx(size_t cnt, ...) {
    Operation *p = new Operation(0, "");
    p->setWidth(1);
    GenObject *obj;
    p->igen_ = ORx_gen;
    p->add_arg(p);
    p->add_arg(reinterpret_cast<GenObject *>(cnt));
    va_list arg;
    va_start(arg, cnt);
    for (int i = 0; i < cnt; i++) {
        obj = va_arg(arg, GenObject *);
        p->add_arg(obj);
    }
    va_end(arg);
    return *p;
}

// ORx_L
std::string ORx_L_gen(GenObject **args) {
    std::string ret = "(";
    size_t cnt = reinterpret_cast<size_t>(args[1]);
    Operation::set_space(Operation::get_space() + 2);
    for (size_t i = 0; i < cnt; i++) {
        if (i > 0) {
            ret += "\n";
            ret += Operation::addspaces();
            ret += "| ";
        }
        ret += Operation::obj2varname(args[2 + i]);
    }
    Operation::set_space(Operation::get_space() - 2);
    ret += ")";
    return ret;
}

Operation &ORx_L(size_t cnt, ...) {
    Operation *p = new Operation(0, "");
    p->setWidth(1);
    GenObject *obj;
    p->igen_ = ORx_L_gen;
    p->add_arg(p);
    p->add_arg(reinterpret_cast<GenObject *>(cnt));
    va_list arg;
    va_start(arg, cnt);
    for (int i = 0; i < cnt; i++) {
        obj = va_arg(arg, GenObject *);
        p->add_arg(obj);
    }
    va_end(arg);
    return *p;
}

// XOR2
std::string XOR2_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    std::string B = Operation::obj2varname(args[2], "r", true);
    A = "(" + A + " ^ " + B + ")";
    return A;
}

Operation &XOR2(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->setWidth(a.getWidth() > b.getWidth() ? a.getWidth() : b.getWidth());
    p->igen_ = XOR2_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}

// XORx
std::string XORx_gen(GenObject **args) {
    std::string ret = "(";
    size_t cnt = reinterpret_cast<size_t>(args[1]);
    Operation::set_space(Operation::get_space() + 2);
    for (size_t i = 0; i < cnt; i++) {
        if (i > 0) {
            ret += "\n";
            ret += Operation::addspaces();
            ret += "^ ";
        }
        ret += Operation::obj2varname(args[2 + i]);
    }
    Operation::set_space(Operation::get_space() - 2);
    ret += ")";
    return ret;
}

Operation &XORx(size_t cnt, ...) {
    Operation *p = new Operation(0, "");
    p->setWidth(1);
    GenObject *obj;
    p->igen_ = XORx_gen;
    p->add_arg(p);
    p->add_arg(reinterpret_cast<GenObject *>(cnt));
    va_list arg;
    va_start(arg, cnt);
    for (int i = 0; i < cnt; i++) {
        obj = va_arg(arg, GenObject *);
        p->add_arg(obj);
    }
    va_end(arg);
    return *p;
}


// ADD2
class OpADD2 : public Operation {
 public:
    OpADD2(const char *comment="") : Operation(0, comment) {}
    virtual std::string generate() override {
        std::string A = Operation::obj2varname(args[1], "r", true);
        std::string B = Operation::obj2varname(args[2], "r", true);
        A = "(" + A + " + " + B + ")";
        return A;
    }
    virtual uint64_t getValue() override {
        return args[1]->getValue() + args[2]->getValue();
    }
    virtual int getWidth() override {
        GenObject &a = *args[1];
        GenObject &b = *args[2];
        return a.getWidth() > b.getWidth() ? a.getWidth() : b.getWidth();
    }
};

/*std::string ADD2_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    std::string B = Operation::obj2varname(args[2], "r", true);
    A = "(" + A + " + " + B + ")";
    return A;
}*/

Operation &ADD2(GenObject &a, GenObject &b, const char *comment) {
    OpADD2 *p = new OpADD2(comment);
    //p->setWidth(a.getWidth() > b.getWidth() ? a.getWidth() : b.getWidth());
    //p->setValue(a.getValue() + b.getValue());
    //p->igen_ = ADD2_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}

// ADDx
std::string ADDx_gen(GenObject **args) {
    std::string ret = "(";
    size_t cnt = reinterpret_cast<size_t>(args[1]);
    Operation::set_space(Operation::get_space() + 2);
    for (size_t i = 0; i < cnt; i++) {
        if (i > 0) {
            ret += "\n";
            ret += Operation::addspaces();
            ret += "+ ";
        }
        ret += Operation::obj2varname(args[2 + i]);
    }
    Operation::set_space(Operation::get_space() - 2);
    ret += ")";
    return ret;
}

Operation &ADDx(size_t cnt, ...) {
    Operation *p = new Operation(0, "");
    p->setWidth(1);
    GenObject *obj;
    p->igen_ = ADDx_gen;
    p->add_arg(p);
    p->add_arg(reinterpret_cast<GenObject *>(cnt));
    va_list arg;
    va_start(arg, cnt);
    for (int i = 0; i < cnt; i++) {
        obj = va_arg(arg, GenObject *);
        p->add_arg(obj);
    }
    va_end(arg);
    return *p;
}


// CALCWIDTHx
class OpCALCWIDTHx : public Operation {
 public:
    OpCALCWIDTHx(const char *comment="") : Operation(0, comment) {}
    virtual std::string generate() override {
        int w;
        char tstr[64];
        std::string ret = "(";
        size_t cnt = reinterpret_cast<size_t>(args[1]);
        Operation::set_space(Operation::get_space() + 2);
        for (size_t i = 0; i < cnt; i++) {
            if (i > 0) {
                ret += "\n";
                ret += Operation::addspaces();
                ret += "+ ";
            }
            w = args[2 + i]->getWidth();
            RISCV_sprintf(tstr, sizeof(tstr), "%d", w);
            ret += args[2 + i]->getStrWidth() + "  // " + args[2 + i]->getName();
        }
        Operation::set_space(Operation::get_space() - 2);
        ret += "\n" + Operation::addspaces() + ")";
        return ret;
    }
};

/*std::string CALCWIDTHx_gen(GenObject **args) {
    int w;
    char tstr[64];
    std::string ret = "(";
    size_t cnt = reinterpret_cast<size_t>(args[1]);
    Operation::set_space(Operation::get_space() + 2);
    for (size_t i = 0; i < cnt; i++) {
        if (i > 0) {
            ret += "\n";
            ret += Operation::addspaces();
            ret += "+ ";
        }
        w = args[2 + i]->getWidth();
        RISCV_sprintf(tstr, sizeof(tstr), "%d", w);
        ret += args[2 + i]->getStrWidth() + "  // " + args[2 + i]->getName();
    }
    Operation::set_space(Operation::get_space() - 2);
    ret += "\n" + Operation::addspaces() + ")";
    return ret;
}*/

Operation &CALCWIDTHx(size_t cnt, ...) {
    OpCALCWIDTHx *p = new OpCALCWIDTHx("");
    uint64_t w = 0;
    GenObject *obj;
//    p->igen_ = CALCWIDTHx_gen;
    p->add_arg(p);
    p->add_arg(reinterpret_cast<GenObject *>(cnt));
    va_list arg;
    va_start(arg, cnt);
    for (int i = 0; i < cnt; i++) {
        obj = va_arg(arg, GenObject *);
        p->add_arg(obj);
        w += obj->getWidth();
    }
    va_end(arg);
    p->setWidth(32);
    p->setValue(w);
    return *p;
}


// SUB2
class OpSUB2 : public Operation {
 public:
    OpSUB2(const char *comment="") : Operation(0, comment) {}
    virtual std::string generate() override {
        std::string A = Operation::obj2varname(args[1], "r", true);
        std::string B = Operation::obj2varname(args[2], "r", true);
        A = "(" + A + " - " + B + ")";
        return A;
    }
    virtual uint64_t getValue() override {
        return args[1]->getValue() - args[2]->getValue();
    }
    virtual int getWidth() override {
        GenObject &a = *args[1];
        GenObject &b = *args[2];
        return a.getWidth() > b.getWidth() ? a.getWidth() : b.getWidth();
    }
};

/*std::string SUB2_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    std::string B = Operation::obj2varname(args[2], "r", true);
    A = "(" + A + " - " + B + ")";
    return A;
}*/

Operation &SUB2(GenObject &a, GenObject &b, const char *comment) {
    OpSUB2 *p = new OpSUB2(comment);
//    p->setWidth(a.getWidth() > b.getWidth() ? a.getWidth() : b.getWidth());
//    p->setValue(a.getValue() - b.getValue());
//    p->igen_ = SUB2_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}

// AND_REDUCE
std::string AND_REDUCE_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    if (SCV_is_sysc()) {
        A += ".and_reduce()";
    } else {
        A = "(&" + A + ")";
    }
    return A;
}

Operation &AND_REDUCE(GenObject &a, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->setWidth(1);
    p->igen_ = AND_REDUCE_gen;
    p->add_arg(p);
    p->add_arg(&a);
    return *p;
}

// OR_REDUCE
std::string OR_REDUCE_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    if (SCV_is_sysc()) {
        A += ".or_reduce()";
    } else {
        A = "(|" + A + ")";
    }
    return A;
}

Operation &OR_REDUCE(GenObject &a, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->setWidth(1);
    p->igen_ = OR_REDUCE_gen;
    p->add_arg(p);
    p->add_arg(&a);
    return *p;
}

// AND2_L
std::string AND2_L_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    std::string B = Operation::obj2varname(args[2], "r", true);
    A = "(" + A + " & " + B + ")";
    return A;
}

Operation &AND2_L(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->setWidth(a.getWidth() > b.getWidth() ? a.getWidth() : b.getWidth());
    p->igen_ = AND2_L_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}

// AND3_L
std::string AND3_L_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    std::string B = Operation::obj2varname(args[2], "r", true);
    std::string C = Operation::obj2varname(args[3], "r", true);
    A = "(" + A + " & " + B + " & " + C + ")";
    return A;
}

Operation &AND3_L(GenObject &a, GenObject &b, GenObject &c, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->setWidth(a.getWidth() > b.getWidth() ? a.getWidth() : b.getWidth());
    p->igen_ = AND3_L_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    p->add_arg(&c);
    return *p;
}


// AND2
std::string AND2_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1]);
    std::string B = Operation::obj2varname(args[2]);
    A = "(" + A + " && " + B + ")";
    return A;
}

Operation &AND2(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->setWidth(a.getWidth() > b.getWidth() ? a.getWidth() : b.getWidth());
    p->igen_ = AND2_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}

// AND3
std::string AND3_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1]);
    std::string B = Operation::obj2varname(args[2]);
    std::string C = Operation::obj2varname(args[3]);
    A = "(" + A + " && " + B + " && " + C + ")";
    return A;
}

Operation &AND3(GenObject &a, GenObject &b, GenObject &c, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = AND3_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    p->add_arg(&c);
    return *p;
}

// AND4
std::string AND4_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1]);
    std::string B = Operation::obj2varname(args[2]);
    std::string C = Operation::obj2varname(args[3]);
    std::string D = Operation::obj2varname(args[4]);
    A = "(" + A + " && " + B + " && " + C + " && " + D + ")";
    return A;
}

Operation &AND4(GenObject &a, GenObject &b, GenObject &c, GenObject &d, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = AND4_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    p->add_arg(&c);
    p->add_arg(&d);
    return *p;
}

// ANDx
std::string ANDx_gen(GenObject **args) {
    std::string ret = "(";
    size_t cnt = reinterpret_cast<size_t>(args[1]);
    Operation::set_space(Operation::get_space() + 2);
    for (size_t i = 0; i < cnt; i++) {
        if (i > 0) {
            ret += "\n";
            ret += Operation::addspaces();
            ret += "&& ";
        }
        ret += Operation::obj2varname(args[2 + i]);
    }
    Operation::set_space(Operation::get_space() - 2);
    ret += ")";
    return ret;
}

Operation &ANDx(size_t cnt, ...) {
    Operation *p = new Operation(0, "");
    p->setWidth(1);
    GenObject *obj;
    p->igen_ = ANDx_gen;
    p->add_arg(p);
    p->add_arg(reinterpret_cast<GenObject *>(cnt));
    va_list arg;
    va_start(arg, cnt);
    for (int i = 0; i < cnt; i++) {
        obj = va_arg(arg, GenObject *);
        p->add_arg(obj);
    }
    va_end(arg);
    return *p;
}

// ANDx_L
std::string ANDx_L_gen(GenObject **args) {
    std::string ret = "(";
    size_t cnt = reinterpret_cast<size_t>(args[1]);
    Operation::set_space(Operation::get_space() + 2);
    for (size_t i = 0; i < cnt; i++) {
        if (i > 0) {
            ret += "\n";
            ret += Operation::addspaces();
            ret += "& ";
        }
        ret += Operation::obj2varname(args[2 + i]);
    }
    Operation::set_space(Operation::get_space() - 2);
    ret += ")";
    return ret;
}

Operation &ANDx_L(size_t cnt, ...) {
    Operation *p = new Operation(0, "");
    p->setWidth(1);
    GenObject *obj;
    p->igen_ = ANDx_L_gen;
    p->add_arg(p);
    p->add_arg(reinterpret_cast<GenObject *>(cnt));
    va_list arg;
    va_start(arg, cnt);
    for (int i = 0; i < cnt; i++) {
        obj = va_arg(arg, GenObject *);
        p->add_arg(obj);
    }
    va_end(arg);
    return *p;
}


// DEC
class OpDEC : public Operation {
 public:
    OpDEC(const char *comment="") : Operation(0, comment) {}
    virtual std::string generate() override {
        std::string A = Operation::obj2varname(args[1], "r", true);
        A = "(" + A + " - 1)";
        return A;
    }
    virtual uint64_t getValue() override { return args[1]->getValue() - 1; }
    virtual int getWidth() override { return args[1]->getWidth(); }
};

/*std::string DEC_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    A = "(" + A + " - 1)";
    return A;
}*/

Operation &DEC(GenObject &a, const char *comment) {
    OpDEC *p = new OpDEC(comment);
    /*p->igen_ = DEC_gen;
    p->setWidth(a.getWidth());
    p->setValue(a.getValue() - 1);*/
    p->add_arg(p);
    p->add_arg(&a);
    return *p;
}

// INC
std::string INC_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    A = "(" + A + " + 1)";
    return A;
}

Operation &INC(GenObject &a, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = INC_gen;
    p->add_arg(p);
    p->add_arg(&a);
    return *p;
}

// INCVAL
std::string INCVAL_gen(GenObject **args) {
    std::string ret = Operation::addspaces();
    ret += Operation::obj2varname(args[1], "v");
    ret += " += ";
    ret += Operation::obj2varname(args[2], "r", true);
    ret += ";\n";
    return ret;
}

Operation &INCVAL(GenObject &res, GenObject &inc, const char *comment) {
    Operation *p = new Operation(comment);
    p->igen_ = INCVAL_gen;
    p->add_arg(p);
    p->add_arg(&res);
    p->add_arg(&inc);
    return *p;
}

// MUL2
std::string MUL2_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    std::string B = Operation::obj2varname(args[2], "r", true);
    A = "(" + A + " * " + B + ")";
    return A;
}

Operation &MUL2(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = MUL2_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    p->setValue(a.getValue() * b.getValue());
    return *p;
}

// DIV2
std::string DIV2_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    std::string B = Operation::obj2varname(args[2], "r", true);
    A = "(" + A + " / " + B + ")";
    return A;
}

Operation &DIV2(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = DIV2_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}

// CCx
std::string CCx_gen(GenObject **args) {
    std::string ret;
    if (SCV_is_sysc()) {
        ret = "(";
    } else if (SCV_is_sv()) {
        ret = "{";
    }
    size_t cnt = reinterpret_cast<size_t>(args[1]);
    Operation::set_space(Operation::get_space() + 2);
    for (size_t i = 0; i < cnt; i++) {
        if (i > 0) {
            ret += ",\n";
            ret += Operation::addspaces();
        }
        ret += Operation::obj2varname(args[2 + i]);
    }
    Operation::set_space(Operation::get_space() - 2);
    if (SCV_is_sysc()) {
        ret += ")";
    } else if (SCV_is_sv()) {
        ret += "}";
    }
    return ret;
}

Operation &CCx(size_t cnt, ...) {
    Operation *p = new Operation(0, "");
    GenObject *obj;
    p->igen_ = CCx_gen;
    p->add_arg(p);
    p->add_arg(reinterpret_cast<GenObject *>(cnt));
    va_list arg;
    va_start(arg, cnt);
    for (int i = 0; i < cnt; i++) {
        obj = va_arg(arg, GenObject *);
        p->add_arg(obj);
    }
    va_end(arg);
    return *p;
}

// CCx
std::string SPLx_gen(GenObject **args) {
    std::string ret = "";
    size_t cnt = reinterpret_cast<size_t>(args[2]);
    int w = args[1]->getWidth();
    char tstr[64];
    for (size_t i = 0; i < cnt; i++) {
        ret += Operation::addspaces();
        ret += Operation::obj2varname(args[3 + i]);
        ret += " = ";
        ret += Operation::obj2varname(args[1], "r", true);
        if (args[3 + i]->getWidth() > 1) {
            if (SCV_is_sysc()) {
                RISCV_sprintf(tstr, sizeof(tstr), "(%d, %d)",
                                w - 1, w - args[3 + i]->getWidth());
            } else if (SCV_is_sv()) {
                RISCV_sprintf(tstr, sizeof(tstr), "[%d: %d]",
                                w - 1, w - args[3 + i]->getWidth());
            } else {
                RISCV_sprintf(tstr, sizeof(tstr), "(%d downto %d)",
                                w - 1, w - args[3 + i]->getWidth());
            }
            ret += std::string(tstr);
        } else {
            RISCV_sprintf(tstr, sizeof(tstr), "[%d]", w - 1);
            ret += std::string(tstr);
        }
        ret += ";\n";
        w -= args[3 + i]->getWidth();
    }
    return ret;
}

Operation &SPLx(GenObject &a, size_t cnt, ...) {
    Operation *p = new Operation("");
    GenObject *obj;
    p->igen_ = SPLx_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(reinterpret_cast<GenObject *>(cnt));
    va_list arg;
    va_start(arg, cnt);
    for (int i = 0; i < cnt; i++) {
        obj = va_arg(arg, GenObject *);
        p->add_arg(obj);
    }
    va_end(arg);
    return *p;
}

// CC2
std::string CC2_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    std::string B = Operation::obj2varname(args[2], "r", true);
    if (SCV_is_sysc()) {
        if (args[2]->getId() == ID_CONST) {
            int w = args[2]->getWidth();
            A = "(" + A + " << " + args[2]->getStrWidth() + ")";
            if (args[2]->getValue() != 0) {
                A = "(" + A + " | " + args[2]->getStrValue() + ")";
            }
        } else {
            A = "(" + A + ", " + B + ")";
        }
    } else {
        A = "{" + A + ", " + B + "}";
    }
    return A;
}

Operation &CC2(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->setWidth(a.getWidth() + b.getWidth());
    p->igen_ = CC2_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}

// CC3
std::string CC3_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1]);
    std::string B = Operation::obj2varname(args[2]);
    std::string C = Operation::obj2varname(args[3]);
    if (SCV_is_sysc()) {
        if (args[1]->getId() == ID_CONST && args[1]->getValue() == 0
            && args[3]->getId() == ID_CONST) {
            int w = args[2]->getWidth();
            A = "(" + B + " << " + args[3]->getStrWidth() + ")";
            if (args[3]->getValue() != 0) {
                A = "(" + A + " | " + args[3]->getStrValue() + ")";
            }
        } else {
            A = "(" + A + ", " + B + ", " + C + ")";
        }
    } else {
        A = "{" + A + ", " + B + ", " + C + "}";
    }
    return A;
}

Operation &CC3(GenObject &a, GenObject &b, GenObject &c, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->setWidth(a.getWidth() + b.getWidth() + c.getWidth());
    p->igen_ = CC3_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    p->add_arg(&c);
    return *p;
}

// CC4
std::string CC4_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1]);
    std::string B = Operation::obj2varname(args[2]);
    std::string C = Operation::obj2varname(args[3]);
    std::string D = Operation::obj2varname(args[4]);
    if (SCV_is_sysc()) {
        A = "(" + A + ", " + B + ", " + C + + ", " + D + ")";
    } else {
        A = "{" + A + ", " + B + ", " + C + + ", " + D + "}";
    }
    return A;
}

Operation &CC4(GenObject &a, GenObject &b, GenObject &c, GenObject &d, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->setWidth(a.getWidth() + b.getWidth() + c.getWidth() + d.getWidth());
    p->igen_ = CC4_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    p->add_arg(&c);
    p->add_arg(&d);
    return *p;
}

// LSH: left shift
std::string LSH_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    std::string B = Operation::obj2varname(args[2]);
    if (SCV_is_sysc()) {
        A = "(" + A + " << " + B + ")";
    } else if (SCV_is_sv()) {
        if (args[2]->getId() == ID_PARAM || args[2]->getId() == ID_CONST) {
            A = "{" + A + ", {" + B + "{1'b0}}}";
        } else {
            A = "(" + A + " << " + B + ")";
        }
    } else {
    }
    return A;
}

Operation &LSH(GenObject &a, GenObject &sz, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->setWidth(a.getWidth());
    p->igen_ = LSH_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&sz);
    return *p;
}

Operation &LSH(GenObject &a, int sz, const char *comment) {
    Operation *p = new Operation(0, comment);
    char tstr[64];
    p->setWidth(a.getWidth());
    p->igen_ = LSH_gen;
    p->add_arg(p);
    p->add_arg(&a);
    RISCV_sprintf(tstr, sizeof(tstr), "%d", sz);
    p->add_arg(new I32D(tstr));
    return *p;
}

// RSH: right shift
std::string RSH_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    std::string B = Operation::obj2varname(args[2]);
    if (SCV_is_sysc()) {
        A = "(" + A + " >> " + B + ")";
    } else if (SCV_is_sv()) {
        if (args[2]->isNumber(B)) {
            A = "{'0, " + A + "[" + args[1]->getStrWidth() + " - 1: " + B + "]}";
        } else {
            A = "(" + A + " >> " + B + ")";
        }
    } else {
    }
    return A;
}

Operation &RSH(GenObject &a, GenObject &sz, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->setWidth(a.getWidth());
    p->igen_ = RSH_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&sz);
    return *p;
}

Operation &RSH(GenObject &a, int sz, const char *comment) {
    Operation *p = new Operation(0, comment);
    char tstr[64];
    p->setWidth(a.getWidth());
    p->igen_ = RSH_gen;
    p->add_arg(p);
    p->add_arg(&a);
    RISCV_sprintf(tstr, sizeof(tstr), "%d", sz);
    p->add_arg(new I32D(tstr));
    return *p;
}

// ARRITEM
std::string ARRITEM_gen(GenObject **args) {
    std::string ret = "";
    args[1]->setSelector(args[2]);
    if (args[4]) {
        ret = Operation::obj2varname(args[3], "r", true);
    } else {
        ret = Operation::obj2varname(args[3]);
    }
    return ret;
}

Operation &ARRITEM(GenObject &arr, GenObject &idx, GenObject &item, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = ARRITEM_gen;
    p->add_arg(p);      // 0
    p->add_arg(&arr);   // 1
    p->add_arg(&idx);   // 2
    p->add_arg(&item);  // 3
    p->add_arg(0);      // [4] do not use .read()
    return *p;
}

Operation &ARRITEM(GenObject &arr, int idx, GenObject &item, const char *comment) {
    Operation *p = new Operation(0, comment);
    char tstr[256];
    RISCV_sprintf(tstr, sizeof(tstr), "%d", idx);
    p->igen_ = ARRITEM_gen;
    p->add_arg(p);      // 0
    p->add_arg(&arr);   // 1
    p->add_arg(new I32D(tstr));   // 2
    p->add_arg(&item);  // 3
    p->add_arg(0);      // [4] do not use .read()
    return *p;
}

Operation &ARRITEM(GenObject &arr, int idx) {
    Operation *p = new Operation(0, "");
    char tstr[256];
    RISCV_sprintf(tstr, sizeof(tstr), "%d", idx);
    p->igen_ = ARRITEM_gen;
    p->add_arg(p);      // 0
    p->add_arg(&arr);   // 1
    p->add_arg(new I32D(tstr));   // 2
    p->add_arg(&arr);  // 3
    p->add_arg(0);      // [4] do not use .read()
    return *p;
}

Operation &ARRITEM_B(GenObject &arr, GenObject &idx, GenObject &item, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = ARRITEM_gen;
    p->add_arg(p);      // 0
    p->add_arg(&arr);   // 1
    p->add_arg(&idx);   // 2
    p->add_arg(&item);  // 3
    p->add_arg(p);      // [4] use .read()
    return *p;
}

// SETARRIDX
std::string SETARRIDX_gen(GenObject **args) {
    GenObject *arr = args[1];
    std::string ret = "";
    arr->setSelector(args[2]);
    return ret;
}

Operation &SETARRIDX(GenObject &arr, GenObject &idx) {
    Operation *p = new Operation("");
    p->igen_ = SETARRIDX_gen;
    p->add_arg(p);      // 0
    p->add_arg(&arr);   // 1
    p->add_arg(&idx);   // 2
    return *p;
}

// SETARRITEM
std::string SETARRITEM_gen(GenObject **args) {
    args[1]->setSelector(args[2]);
    std::string ret = Operation::addspaces();
    if (args[5]) {
        if (SCV_is_sv()) {
            ret += "assign ";
        }
    }
    ret += Operation::obj2varname(args[3], "v");
    ret += " = ";
    ret += Operation::obj2varname(args[4]);
    ret += ";";
    ret += Operation::addtext(args[0], ret.size());
    ret += "\n";
    return ret;
}

Operation &SETARRITEM(GenObject &arr, GenObject &idx, GenObject &item, GenObject &var, const char *comment) {
    Operation *p = new Operation(comment);
    p->igen_ = SETARRITEM_gen;
    p->add_arg(p);
    p->add_arg(&arr);
    p->add_arg(&idx);
    p->add_arg(&item);
    p->add_arg(&var);
    p->add_arg(0);  // [5] do not use 'assign '
    return *p;
}

Operation &ASSIGNARRITEM(GenObject &arr, GenObject &idx, GenObject &item, GenObject &var, const char *comment) {
    Operation *p = new Operation(comment);
    p->igen_ = SETARRITEM_gen;
    p->add_arg(p);
    p->add_arg(&arr);
    p->add_arg(&idx);
    p->add_arg(&item);
    p->add_arg(&var);
    p->add_arg(p);  // [5] add 'assign '
    return *p;
}

// reduced formed
Operation &SETARRITEM(GenObject &arr, int idx, GenObject &val) {
    Operation *p = new Operation("");
    char tstr[256];
    RISCV_sprintf(tstr, sizeof(tstr), "%d", idx);
    p->igen_ = SETARRITEM_gen;
    p->add_arg(p);
    p->add_arg(&arr);
    p->add_arg(new I32D(tstr));
    p->add_arg(&arr);
    p->add_arg(&val);
    p->add_arg(0);  // [5] do not use 'assign '
    return *p;
}

Operation &ASSIGNARRITEM(GenObject &arr, int idx, GenObject &val) {
    Operation *p = new Operation("");
    char tstr[256];
    RISCV_sprintf(tstr, sizeof(tstr), "%d", idx);
    p->igen_ = SETARRITEM_gen;
    p->add_arg(p);
    p->add_arg(&arr);
    p->add_arg(new I32D(tstr));
    p->add_arg(&arr);
    p->add_arg(&val);
    p->add_arg(p);  // [5] use 'assign '
    return *p;
}


//IF_OTHERWISE
std::string IF_OTHERWISE_gen(GenObject **args) {
    std::string ret = "";
    ret += Operation::obj2varname(args[1]);
    ret += " ? ";
    ret += Operation::obj2varname(args[2]);
    ret += " : ";
    ret += Operation::obj2varname(args[3]);
    return ret;
}

Operation &IF_OTHERWISE(GenObject &cond, GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = IF_OTHERWISE_gen;
    p->add_arg(p);
    p->add_arg(&cond);
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}


// IF
std::string IF_gen(GenObject **args) {
    std::string ret = Operation::addspaces();
    std::string A = Operation::obj2varname(args[1]);
    spaces_++;

    if (A.c_str()[0] == '(') {
    } else {
        A = "(" + A + ")";
    }
    ret += "if " + A;
    if (SCV_is_sysc()) {
        ret += " {";
    } else {
        ret += " begin";
        if (args[2]) {
            ret += ": " + args[2]->getStrValue().substr(1, args[2]->getStrValue().size()-2);
        }
    }
    ret += Operation::addtext(args[0], ret.size());
    ret += "\n";
    return ret;
}

void IF(GenObject &a, const char *comment) {
    Operation *p = new Operation(comment);
    Operation::push_obj(p);
    p->igen_ = IF_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(0);
}

void IFGEN(GenObject &a, STRING *name, const char *comment) {
    Operation *p = new Operation(comment);
    Operation::push_obj(p);
    p->igen_ = IF_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(name);
}

// ELSE IF
std::string ELSIF_gen(GenObject **args) {
    std::string ret = "";
    std::string A = Operation::obj2varname(args[1]);

    if (A.c_str()[0] == '(') {
    } else {
        A = "(" + A + ")";
    }
    spaces_--;
    if (SCV_is_sysc()) {
        ret += Operation::addspaces() + "} else if " + A + " {";
    } else {
        ret += Operation::addspaces() + "end else if " + A + " begin";
    }
    ret += Operation::addtext(args[0], ret.size());
    ret += "\n";
    spaces_++;
    return ret;
}

void ELSIF(GenObject &a, const char *comment) {
    Operation *p = new Operation(comment);
    Operation::pop_obj();
    Operation::push_obj(p);
    p->igen_ = ELSIF_gen;
    p->add_arg(p);
    p->add_arg(&a);
}

// ELSE
std::string ELSE_gen(GenObject **args) {
    std::string ret = "";
    spaces_--;
    if (SCV_is_sysc()) {
        ret += Operation::addspaces() + "} else {\n";
    } else {
        if (args[1]) {
            ret += Operation::addspaces();
            ret += "end: " + args[1]->getStrValue().substr(1, args[1]->getStrValue().size()-2) + "_en\n";
            ret += Operation::addspaces();
            ret += "else begin: " + args[1]->getStrValue().substr(1, args[1]->getStrValue().size()-2) + "_dis";
        } else {
            ret += Operation::addspaces();
            ret += "end else begin";
        }
        ret += "\n";

    }
    spaces_++;
    return ret;
}

void ELSE(const char *comment) {
    Operation *p = new Operation(comment);
    Operation::pop_obj();
    Operation::push_obj(p);
    p->igen_ = ELSE_gen;
    p->add_arg(p);
    p->add_arg(0);
}

void ELSEGEN(STRING *name, const char *comment) {
    Operation *p = new Operation(comment);
    Operation::pop_obj();
    Operation::push_obj(p);
    p->igen_ = ELSE_gen;
    p->add_arg(p);
    p->add_arg(name);
}


// ENDIF
std::string ENDIF_gen(GenObject **args) {
    std::string ret = "";
    spaces_--;
    if (SCV_is_sysc()) {
        ret = Operation::addspaces() + "}\n";
    } else {
        ret = Operation::addspaces() + "end";
        if (args[1]) {
            ret += ": " + args[1]->getStrValue().substr(1, args[1]->getStrValue().size()-2);
        }
        ret += "\n";
    }
    return ret;
}

void ENDIF(const char *comment) {
    Operation::pop_obj();
    Operation *p = new Operation(comment);
    p->igen_ = ENDIF_gen;
    p->add_arg(p);
    p->add_arg(0);
}

void ENDIFGEN(STRING *name, const char *comment) {
    Operation::pop_obj();
    Operation *p = new Operation(comment);
    p->igen_ = ENDIF_gen;
    p->add_arg(p);
    p->add_arg(name);
}


// SWITCH
std::string SWITCH_gen(GenObject **args) {
    std::string ret = Operation::addspaces();
    std::string A = Operation::obj2varname(args[1], "r", true);
    spaces_++;

    if (A.c_str()[0] == '(') {
    } else {
        A = "(" + A + ")";
    }
    if (SCV_is_sysc()) {
        ret += "switch " + A + " {";
    } else {
        ret += "case " + A;
    }
    ret += Operation::addtext(args[0], ret.size());
    ret += "\n";
    return ret;
}

void SWITCH(GenObject &a, const char *comment) {
    Operation *p = new Operation(comment);
    Operation::push_obj(p);
    p->igen_ = SWITCH_gen;
    p->add_arg(p);
    p->add_arg(&a);
}

// CASE
std::string CASE_gen(GenObject **args) {
    std::string ret = "";
    std::string A = Operation::obj2varname(args[1]);
    spaces_--;
    if (SCV_is_sysc()) {
        ret += Operation::addspaces() + "case " + A + ":";
    } else {
        ret += Operation::addspaces() + A + ": begin";
    }
    ret += Operation::addtext(args[0], ret.size());
    ret += "\n";
    spaces_++;
    return ret;
}

void CASE(GenObject &a, const char *comment) {
    Operation *p = new Operation(comment);
    Operation::pop_obj();
    Operation::push_obj(p);
    p->igen_ = CASE_gen;
    p->add_arg(p);
    p->add_arg(&a);
}

// CASEDEF
std::string CASEDEF_gen(GenObject **args) {
    std::string ret = "";
    spaces_--;
    ret += Operation::addspaces();
    if (SCV_is_sysc()) {
        ret += "default:";
    } else {
        ret += "default: begin";
    }
    ret += "\n";
    spaces_++;
    return ret;
}

void CASEDEF(const char *comment) {
    Operation *p = new Operation(comment);
    Operation::pop_obj();
    Operation::push_obj(p);
    p->igen_ = CASEDEF_gen;
    p->add_arg(p);
}

// ENDCASE
std::string ENDCASE_gen(GenObject **args) {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret += Operation::addspaces() + "break;\n";
    } else {
        spaces_--;
        ret += Operation::addspaces() + "end\n";
        spaces_++;
    }
    return ret;
}

void ENDCASE(const char *comment) {
    Operation *p = new Operation(comment);
    p->igen_ = ENDCASE_gen;
    p->add_arg(p);
}

// ENDSWITCH
std::string ENDSWITCH_gen(GenObject **args) {
    spaces_--;
    std::string ret = Operation::addspaces();
    if (SCV_is_sysc()) {
        ret += "}\n";
    } else {
        ret += "endcase\n";
    }
    return ret;
}

void ENDSWITCH(const char *comment) {
    Operation::pop_obj();
    Operation *p = new Operation(comment);
    p->igen_ = ENDSWITCH_gen;
    p->add_arg(p);
}

// GENVAR

// FOR
std::string FOR_gen(GenObject **args) {
    std::string ret = Operation::addspaces();
    GenObject *op = args[0];
    std::string i = Operation::obj2varname(args[1]);
    spaces_++;
    std::string start = Operation::obj2varname(args[2]);
    std::string end = Operation::obj2varname(args[3]);
    std::string dir = Operation::obj2varname(args[4]);
    STRING *gename = static_cast<STRING *>(args[5]);


    if (SCV_is_sysc()) {
        ret += "for (int " + i + " = ";
    } else {
        if (args[1]->isGenVar()) {
            ret += "for (genvar " + i + " = ";
        } else {
            ret += "for (int " + i + " = ";
        }
    }
    ret += start + "; ";
    ret += i;
    if (dir == "++") {
        ret += " < ";
    } else {
        ret += " >= ";
    }
    ret += end + "; ";
    ret += i + dir;
    ret += ")";
    if (SCV_is_sysc()) {
        ret += " {\n";
    } else {
        ret += " begin";
        if (gename) {
            ret += ": " + gename->getStrValue().substr(1, gename->getStrValue().size()-2);
        }
        ret += "\n";
    }
    return ret;
}

GenObject &FOR(const char *i, GenObject &start, GenObject &end, const char *dir, const char *comment) {
    Operation *p = new Operation(comment);
    I32D *ret = new I32D("0", i);
    Operation::push_obj(p);
    p->igen_ = FOR_gen;
    p->add_arg(p);
    p->add_arg(ret);
    p->add_arg(&start);
    p->add_arg(&end);
    p->add_arg(new TextLine(0, dir));
    p->add_arg(0);
    return *ret;
}

// 'generate' for cycle used in rtl, it is the same for in systemc
GenObject &FORGEN(const char *i, GenObject &start, GenObject &end, const char *dir, STRING *name, const char *comment) {
    Operation *p = new Operation(comment);
    I32D *ret = new GenVar("0", i, 0);
    Operation::push_obj(p);
    p->igen_ = FOR_gen;
    p->add_arg(p);
    p->add_arg(ret);
    p->add_arg(&start);
    p->add_arg(&end);
    p->add_arg(new TextLine(0, dir));
    p->add_arg(name);
    return *ret;
}

// ENDFOR
std::string ENDFOR_gen(GenObject **args) {
    std::string ret = "";
    spaces_--;
    if (SCV_is_sysc()) {
        ret = Operation::addspaces() + "}\n";
    } else {
        STRING *gename = static_cast<STRING *>(args[1]);
        ret = Operation::addspaces() + "end";
        if (gename) {
            ret += ": " + gename->getStrValue().substr(1, gename->getStrValue().size()-2);
        }
        ret += "\n";
    }
    return ret;
}

void ENDFOR(const char *comment) {
    Operation::pop_obj();
    Operation *p = new Operation(comment);
    p->igen_ = ENDFOR_gen;
    p->add_arg(p);
    p->add_arg(0);
}

void ENDFORGEN(STRING *name, const char *comment) {
    Operation::pop_obj();
    Operation *p = new Operation(comment);
    p->igen_ = ENDFOR_gen;
    p->add_arg(p);
    p->add_arg(name);
}



// WHILE
std::string WHILE_gen(GenObject **args) {
    std::string ret = Operation::addspaces();
    std::string A = Operation::obj2varname(args[1], "r", true);
    spaces_++;

    if (A.c_str()[0] == '(') {
    } else {
        A = "(" + A + ")";
    }
    ret += "while " + A;
    if (SCV_is_sysc()) {
        ret += " {\n";
    } else {
        ret += " begin\n";
    }
    return ret;
}

void WHILE(GenObject &a, const char *comment) {
    Operation *p = new Operation(comment);
    Operation::push_obj(p);
    p->igen_ = WHILE_gen;
    p->add_arg(p);
    p->add_arg(&a);
}


// ENDWHILE
std::string ENDWHILE_gen(GenObject **args) {
    spaces_--;
    std::string ret = Operation::addspaces();
    if (SCV_is_sysc()) {
        ret += "}\n";
    } else {
        ret += "end\n";
    }
    return ret;
}

void ENDWHILE(const char *comment) {
    Operation::pop_obj();
    Operation *p = new Operation(comment);
    p->igen_ = ENDWHILE_gen;
    p->add_arg(p);
}


// Sync reset
std::string SYNC_RESET_gen(GenObject **args) {
    ModuleObject *m = static_cast<ModuleObject *>(args[1]);
    std::string xrst = Operation::obj2varname(args[2]);
    std::string ret = Operation::reset("v", 0, m, xrst);
    ret += "\n";
    return ret;
}

void SYNC_RESET(GenObject &a, GenObject *xrst, const char *comment) {
    Operation *p = new Operation(comment);
    p->igen_ = SYNC_RESET_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(xrst);
}

// CALLF
std::string CALLF_gen(GenObject **args) {
    std::string ret = Operation::addspaces();
    if (args[1]) {
        ret += Operation::obj2varname(args[1], "v") + " = ";
    }
    ret += args[2]->getName();
    ret += "(";
    size_t cnt = reinterpret_cast<size_t>(args[3]);
    Operation::set_space(Operation::get_space() + 2);
    for (size_t i = 0; i < cnt; i++) {
        if (i > 0) {
            ret += ",\n" + Operation::addspaces();
        }
        ret += Operation::obj2varname(args[4 + i]);
    }
    Operation::set_space(Operation::get_space() - 2);

    ret += ");\n";
    return ret;
}

void CALLF(GenObject *ret, GenObject &a, size_t argcnt, ...) {
    Operation *p = new Operation("");
    GenObject *obj;
    p->igen_ = CALLF_gen;
    p->add_arg(p);
    p->add_arg(ret);
    p->add_arg(&a);
    p->add_arg(reinterpret_cast<GenObject *>(argcnt));
    va_list arg;
    va_start(arg, argcnt);
    for (int i = 0; i < argcnt; i++) {
        obj = va_arg(arg, GenObject *);
        p->add_arg(obj);
    }
    va_end(arg);
}

// FOPEN
std::string FOPEN_gen(GenObject **args) {
    std::string ret = Operation::addspaces();
    ret += Operation::obj2varname(args[1]);
    if (SCV_is_sysc()) {
        ret += " = fopen(";
        ret += Operation::obj2varname(args[2]);
        if (args[2]->isString()) {
            ret += ".c_str()";
        }
        ret += ", \"wb\");\n";
    } else {
        ret += " = $fopen(" + Operation::obj2varname(args[2]) + ", \"w\");\n";
        ret += Operation::addspaces() + "assert (" + Operation::obj2varname(args[1]) + ")\n";
        ret += Operation::addspaces() + "else begin\n";
        ret += Operation::addspaces() + "    $warning(\"Cannot open log-file\");\n";
        ret += Operation::addspaces() + "end";
    }
    return ret;
}

void FOPEN(GenObject &f, GenObject &str) {
    Operation *p = new Operation("");
    p->igen_ = FOPEN_gen;
    p->add_arg(p);
    p->add_arg(&f);
    p->add_arg(&str);
}


// FWRITE
std::string FWRITE_gen(GenObject **args) {
    std::string ret = Operation::addspaces();
    if (SCV_is_sysc()) {
        ret += "fwrite(";
        ret += Operation::obj2varname(args[2]) + ".c_str(), 1, ";
        ret += Operation::obj2varname(args[2]) + ".size(), ";
        ret += Operation::obj2varname(args[1]);
    } else {
        ret += "$fwrite(";
        ret += Operation::obj2varname(args[1]);
        ret += ", \"%s\", ";
        ret += Operation::obj2varname(args[2]);
    }
    ret += ");\n";
    return ret;
}

void FWRITE(GenObject &f, GenObject &str) {
    Operation *p = new Operation("");
    p->igen_ = FWRITE_gen;
    p->add_arg(p);
    p->add_arg(&f);
    p->add_arg(&str);
}

// NEW module instance
std::string NEW_gen_sv(Operation *op, ModuleObject *mod, std::string name) {
    std::string ret = "";
    std::string ln = "";
    std::string idx = "";
    int tcnt = 0;

    ln = Operation::addspaces();
    ln += mod->getType() + " ";
    ret += ln;
    
    std::list<GenObject *>tmpllist;
    mod->getTmplParamList(tmpllist);
    mod->getParamList(tmpllist);    
    tcnt = 0;
    if (mod->getAsyncReset() || tmpllist.size()) {
        ret += "#(\n";
        Operation::set_space(Operation::get_space() + 1);
        if (mod->getAsyncReset() && mod->getEntryByName("async_reset") == 0) {
            ret += Operation::addspaces() + ".async_reset(async_reset)";
            if (tmpllist.size()) {
                ret += ",";
            }
            ret += "\n";
            tcnt++;
        }
        for (auto &e : tmpllist) {
            ret += Operation::addspaces();
            if (e->getId() == ID_TMPL_PARAM) {
                ret += "." + e->getName() + "(" + e->getStrValue() + ")";
            } else if (e->getObjValue()) {
                // generic parameter but with the defined string value
                ret += "." + e->getName() + "(" + e->getObjValue()->getName() + ")";
            } else {
                ret += "." + e->getName() + "(" + e->getName() + ")";
            }
            if (e != tmpllist.back()) {
                ret += ",";
            }
            ret += "\n";
            tcnt++;
        }
        Operation::set_space(Operation::get_space() - 1);
        ret += Operation::addspaces() + ") ";
    }
    
    ret += name + " (";
    Operation::set_space(Operation::get_space() + 1);

    std::list<GenObject *>iolist;
    mod->getIoList(iolist);
    ret += "\n";
    for (auto &io : iolist) {
        ret += op->gen_connection(io->getName());
        if (io != iolist.back()) {
            ret += ",";
        }
        ret += "\n";
    }
    Operation::set_space(Operation::get_space() - 1);
    ret += Operation::addspaces() + ");";
    ret += "\n";
    return ret;
}

std::string NEW_gen(GenObject **args) {
    std::string ret = "";
    std::string ln = "";
    std::string lasttmpl = "";
    std::string idx = "";
    int tcnt = 0;
    std::string name = Operation::obj2varname(args[2]);
    ModuleObject *mod = static_cast<ModuleObject *>(args[1]);

    if (SCV_is_sv()) {
        ret += NEW_gen_sv(static_cast<Operation *>(args[0]),
                          mod,
                          name);
        return ret;
    }
    ln = Operation::addspaces();
    if (args[3]) {
        idx = Operation::obj2varname(args[3]);
        ret += Operation::addspaces();
        ret += "char tstr[256];\n";
        ret += Operation::addspaces();
        ret += "RISCV_sprintf(tstr, sizeof(tstr), \"" + name + "%d\", " + idx + ");\n";
    }
    ln += name;
    if (idx.size()) {
        ln += "[" + idx + "]";
    }
    ln += " = new " + mod->getType();
    ret += ln;
    
    std::list<GenObject *>tmpllist;
    mod->getTmplParamList(tmpllist);
    tcnt = 0;
    if (tmpllist.size()) {
        ret += "<";
        for (auto &e : tmpllist) {
            if (tcnt) {
                ret += ",\n";
                for (int i = 0; i <= ln.size(); i++) {
                    ret += " ";
                }
            }
            ret += e->getStrValue();
            lasttmpl = e->getStrValue(); // need to properly compute space value
            tcnt++;
        }
        ret += ">";
        ln += lasttmpl + ">";
    }

    ret += "(";
    ln += "(";
    if (idx.size()) {
        ret += "tstr";
    } else {
        ret += "\"" + name + "\"";
    }
    if (mod->getAsyncReset() && mod->getEntryByName("async_reset") == 0) {
        ret += ", async_reset";
    }
    std::list<GenObject *>genlist;
    mod->getParamList(genlist);
    for (auto &g : genlist) {
        ret += ",\n";
        for (int i = 0; i <= ln.size(); i++) {
            ret += " ";
        }
        if (g->getObjValue()) {
            // generic parameter but with the defined string value
            ret += g->getObjValue()->getName();
        } else {
            ret += g->getName();
        }
    }
    ret += ");\n";

    std::list<GenObject *>iolist;
    mod->getIoList(iolist);
    for (auto &io : iolist) {
        ret += Operation::addspaces();
        ret += name;
        ret += static_cast<Operation *>(args[0])->gen_connection(io->getName());
        ret += ";";
        ret += "\n";
    }
    return ret;
}

void NEW(GenObject &m, const char *name, GenObject *idx, const char *comment) {
    Operation *p = new Operation(comment);
    Operation::push_obj(p);
    p->igen_ = NEW_gen;
    p->add_arg(p);
    p->add_arg(&m);
    p->add_arg(new TextLine(0, name));
    p->add_arg(idx);
}

// CONNECT
std::string CONNECT_gen(GenObject **args) {
    std::string ret = "";
    Operation *p = static_cast<Operation *>(args[0]);
    if (SCV_is_sysc()) {
        if (args[2]) {
            ret += "[" + Operation::obj2varname(args[2]) + "]";
        }
        ret += "->";
        ret += args[3]->getName();
        ret += "(";
        ret += Operation::obj2varname(args[4]);
        ret += ")";
    } else {
        ret += Operation::addspaces();
        ret += "." + args[3]->getName();
        ret += "(";
        ret += Operation::obj2varname(args[4]);
        ret += ")";
    }
    return ret;
}

void CONNECT(GenObject &inst, GenObject *idx, GenObject &port, GenObject &s, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = CONNECT_gen;
    p->add_arg(p);
    p->add_arg(&inst);
    p->add_arg(idx);
    p->add_arg(&port);
    p->add_arg(&s);
    static_cast<Operation *>(stackobj_[stackcnt_])->add_connection(port.getName(), p);
}

// ENDNEW
std::string ENDNEW_gen(GenObject **args) {
    std::string ret = "";
    return ret;
}

void ENDNEW(const char *comment) {
    Operation::pop_obj();
    Operation *p = new Operation(comment);
    p->igen_ = ENDNEW_gen;
    p->add_arg(p);
}

// DECLARE_TSTR
std::string DECLARE_TSTR_gen(GenObject **args) {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret += Operation::addspaces() + "char tstr[256];\n";
    }
    return ret;
}

void DECLARE_TSTR() {
    Operation *p = new Operation("");
    p->igen_ = DECLARE_TSTR_gen;
    p->add_arg(p);
}

// INITIAL
std::string INITIAL_gen(GenObject **args) {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret += Operation::addspaces() + "// initial\n";
    } else {
        ret += Operation::addspaces() + "initial begin\n";
        Operation::set_space(Operation::get_space() + 1);
    }
    return ret;
}

void INITIAL() {
    Operation *p = new Operation("");
    Operation::push_obj(p);
    p->igen_ = INITIAL_gen;
    p->add_arg(p);
}

// ENDINITIAL
std::string ENDINITIAL_gen(GenObject **args) {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret += Operation::addspaces() + "// end initial\n";
    } else {
        Operation::set_space(Operation::get_space() - 1);
        ret += Operation::addspaces() + "end\n";
    }
    return ret;
}

void ENDINITIAL() {
    Operation::pop_obj();
    Operation *p = new Operation("");
    p->igen_ = ENDINITIAL_gen;
    p->add_arg(p);
}


// GENERATE
std::string GENERATE_gen(GenObject **args) {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret += Operation::addspaces() + "// generate\n";
    } else {
        ret += Operation::addspaces() + "generate\n";
        Operation::set_space(Operation::get_space() + 1);
    }
    return ret;
}

void GENERATE(const char *name, const char *comment) {
    Operation *p = new Operation(comment);
    Operation::push_obj(p);
    p->igen_ = GENERATE_gen;
    p->add_arg(p);
    p->add_arg(new STRING(name));
}

// ENDGENERATE
std::string ENDGENERATE_gen(GenObject **args) {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret += Operation::addspaces() + "// endgenerate\n";
    } else {
        Operation::set_space(Operation::get_space() - 1);
        ret += Operation::addspaces() + "endgenerate\n";
    }
    return ret;
}

void ENDGENERATE(const char *name, const char *comment) {
    Operation::pop_obj();
    Operation *p = new Operation(comment);
    p->igen_ = ENDGENERATE_gen;
    p->add_arg(p);
    p->add_arg(new STRING(name));
}

// ASSIGNZERO
std::string ASSIGNZERO_gen(GenObject **args) {
    std::string ret = Operation::addspaces();
    if (SCV_is_sv()) {
        ret += "assign ";
    }
    ret += Operation::obj2varname(args[1], "v");
    if (SCV_is_sysc()) {
        ret += " = 0";
    } else if (SCV_is_sv()) {
        if (args[1]->getWidth() == 1) {
            ret += " = 1'b0";
        } else {
            ret += " = '0";
        }
    } else  {
        ret += " = (others => '0')";
    }
    ret +=  + ";";
    ret += Operation::addtext(args[0], ret.size());
    ret += "\n";
    return ret;
}

Operation &ASSIGNZERO(GenObject &a, const char *comment) {
    Operation *p = new Operation(comment);
    p->igen_ = ASSIGNZERO_gen;
    p->add_arg(p);
    p->add_arg(&a);
    return *p;
}

// ASSIGNONE
std::string ASSIGN_gen(GenObject **args) {
    std::string ret = Operation::addspaces();
    std::string b = Operation::obj2varname(args[2], "r");
    if (SCV_is_sv()) {
        ret += "assign ";
    }
    ret += Operation::obj2varname(args[1], "v");
    ret += " = ";
    ret += b;
    ret +=  ";";
    ret += Operation::addtext(args[0], ret.size());
    ret += "\n";
    return ret;
}

Operation &ASSIGN(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Operation(comment);
    p->igen_ = ASSIGN_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}

}
