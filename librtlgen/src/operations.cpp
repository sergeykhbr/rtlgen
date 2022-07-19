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

namespace sysvc {

int spaces_ = 1;
int stackcnt_ = 0;
GenObject *stackobj_[256] = {0};

Operation::Operation(const char *comment)
    : GenObject(stackobj_[stackcnt_], ID_OPERATION, "", comment), igen_(0), argcnt_(0) {
}

Operation::Operation(GenObject *parent, const char *comment)
    : GenObject(parent, ID_OPERATION, "", comment), igen_(0), argcnt_(0) {
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
        while (++curpos < 60) {
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
    } else if (obj->getId() == ID_ARRAY_DEF) {
        curname = "";
        curname = fullname("r", curname, static_cast<ArrayObject *>(obj)->getSelector());
        curname = "[" + curname + "]";
        curname = obj->getName() + curname;
        if (name.size()) {
            curname += ".";
        }
        curname += name;
    } else if (obj->getId() == ID_STRUCT_INST
            || obj->getId() == ID_STRUCT_DEF) {
        curname = obj->getName();
        if (name.size()) {
            curname += ".";
        }
        curname += name;
    } else if (obj->getId() == ID_DEF_PARAM) {
        curname = obj->getName() + name + "_";
    } else {
        curname = obj->getName() + name;
    }

    if (p && (p->getId() == ID_STRUCT_INST
            || p->getId() == ID_STRUCT_DEF)) {
        curname = fullname(prefix, curname, obj->getParent());
    } else if (obj->isReg()) {
        curname = std::string(prefix) + "." + curname;
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
        if (obj->getId() == ID_INPUT
            || (prefix[0] == 'r' && obj->getId() == ID_SIGNAL)) {
            if (SCV_is_sysc()) {
                ret += ".read()";
            }
        }
    }
    return ret;
}


// dst = r or v
// src = r, v or 0
std::string Operation::copyreg(const char *dst, const char *src, ModuleObject *m) {
    std::string ret = "";
    // reset dst
    if (!m->is2DimReg()) {
        if (src == 0) {
            // reset using function
            ret += Operation::addspaces();
            ret += m->getType() + "_r_reset(" + std::string(dst) + ")";
        } else {
            // copy data from src into dst
            ret += Operation::addspaces();
            ret += std::string(dst) + " = " + std::string(src);
        }
        ret += ";\n";
    } else {
        // reset each register separatly
        for (auto &p: m->getEntries()) {
            if (!p->isReg()) {
                continue;
            }
            if (src == 0 && p->isResetDisabled()) {
                continue;
            }
            if (p->getId() == ID_ARRAY_DEF) {
                ret += Operation::addspaces();
                ret += "for (int i = 0; i < " + p->getStrDepth() + "; i++) {\n";
                spaces_++;
                std::list<GenObject *>::iterator it = p->getEntries().begin();  // element[0]
                for (auto &s: (*it)->getEntries()) {
                    ret += Operation::addspaces();
                    ret +=  std::string(dst) + "." + p->getName() + "[i]." + s->getName() + " = ";
                    if (src == 0) {
                        // reset
                        ret += s->getStrValue();
                    } else {
                        // copy data
                        ret += std::string(src) + "." + p->getName() + "[i]." + s->getName();
                    }
                    ret +=  ";\n";
                }
                if ((*it)->getEntries().size() == 0) {
                    ret += Operation::addspaces();
                    ret +=  std::string(dst) + "." + p->getName() + "[i]" + " = ";
                    if (src == 0) {
                        // reset
                        ret += p->getStrValue();
                    } else {
                        // copy data
                        ret += std::string(src) + "." + p->getName() + "[i]";
                    }
                    ret +=  ";\n";
                }
                spaces_--;
                ret += Operation::addspaces();
                ret += "}\n";
            } else {
                ret += Operation::addspaces();
                ret += std::string(dst) + "." + p->getName() + " = ";
                if (src == 0) {
                    // reset
                    ret += p->getStrValue();
                } else {
                    // copy value
                    ret += std::string(src) + "." + p->getName();
                }
                ret += ";\n";
            }
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
            ret += "(!async_reset_ && i_nrst.read() == 0)";
            if (xrst.size()) {
                ret += " || " + xrst + ")";
            }
            ret += " {\n";
        } else {
            ret += "if (async_reset_ && i_nrst.read() == 0) {\n";
        }
        spaces_++;
        ret += copyreg(dst, src, m);
        spaces_--;
        ret += Operation::addspaces() + "}";
    } else if (SCV_is_sv()) {
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
    ret += Operation::obj2varname(args[1], "r", true);
    if (SCV_is_sysc()) {
         ret += "(";
         ret += Operation::obj2varname(args[2], "r", true);
         ret += ", ";
         ret += Operation::obj2varname(args[3], "r", true);
         ret += ")";
    } else if (SCV_is_sv()) {
         ret += "[";
         ret += Operation::obj2varname(args[2]);
         ret += ": ";
         ret += Operation::obj2varname(args[3]);
         ret += "]";
    } else {
         ret += "(";
         ret += Operation::obj2varname(args[2]);
         ret += " downto ";
         ret += Operation::obj2varname(args[3]);
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

// CONST
GenObject &CONST(const char *val) {
    GenObject *p = new I32D(val);
    return *p;
}

GenObject &CONST(const char *val, int width) {
    char tstr[64];
    RISCV_sprintf(tstr, sizeof(tstr), "%d", width);
    GenObject *p = new Logic(tstr, "", val);
    return *p;
}

// SETZERO
std::string SETZERO_gen(GenObject **args) {
    std::string ret = Operation::addspaces();
    ret += Operation::obj2varname(args[1], "v");
    if (SCV_is_sysc()) {
        ret += " = 0";
    } else if (SCV_is_sv()) {
        ret += " = '0";
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
        char tstr[64];
        RISCV_sprintf(tstr, sizeof(tstr), "%dd'1", args[1]->getWidth());
        ret += " = " + std::string(tstr);
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
        ret += "] = " + Operation::obj2varname(args[3]) + ";";
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
        ret += "] = 1'b01;";
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
    return SETBITONE(a, *t1, comment);
}

Operation &SETBITZERO(GenObject &a, int b, const char *comment) {
    char tstr[64];
    RISCV_sprintf(tstr, sizeof(tstr), "%d", b);
    return SETBITONE(a, tstr, comment);
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
         ret += Operation::obj2varname(args[2]);
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

// SETVAL
std::string SETVAL_gen(GenObject **args) {
    std::string ret = Operation::addspaces();
    ret += Operation::obj2varname(args[1], "v") + " = ";
    if (args[2]->getId() == ID_CONST) {
        ret += args[2]->getStrValue();
    } else if (args[2]->getId() == ID_VALUE
            || args[2]->getId() == ID_INPUT
            || args[2]->getId() == ID_SIGNAL
            || args[2]->getId() == ID_PARAM) {
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

// BIG_TO_U64: explicit conersion of biguint to uint64 (sysc only)
std::string BIG_TO_U64_gen(GenObject **args) {
    std::string A = "";
    if (SCV_is_sysc()) {
        A = Operation::obj2varname(args[1]) + ".to_uint64()";
    }
    return A;
}

Operation &BIG_TO_U64(GenObject &a, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = BIG_TO_U64_gen;
    p->add_arg(p);
    p->add_arg(&a);
    return *p;
}


// TO_INT
std::string TO_INT_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    A = A + ".to_int()";
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
    A = "(" + A + " == 0)";
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
    A = "(" + A + " == 1)";
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

// INV
std::string INV_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1]);
    A = "(!" + A + ")";
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

/*Operation &ORx(size_t cnt, ...) {
    Operation *p = new Operation(0, "");
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
}*/

// ADD2
std::string ADD2_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    std::string B = Operation::obj2varname(args[2], "r", true);
    A = "(" + A + " + " + B + ")";
    return A;
}

Operation &ADD2(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->setWidth(a.getWidth() > b.getWidth() ? a.getWidth() : b.getWidth());
    p->setValue(a.getValue() + b.getValue());
    p->igen_ = ADD2_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}

// SUB2
std::string SUB2_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    std::string B = Operation::obj2varname(args[2], "r", true);
    A = "(" + A + " - " + B + ")";
    return A;
}

Operation &SUB2(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->setWidth(a.getWidth() > b.getWidth() ? a.getWidth() : b.getWidth());
    p->setValue(a.getValue() - b.getValue());
    p->igen_ = SUB2_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}

// AND_REDUCE
std::string AND_REDUCE_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1]);
    A += ".and_reduce()";
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

// DECC
std::string DEC_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    A = "(" + A + " - 1)";
    return A;
}

Operation &DEC(GenObject &a, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = DEC_gen;
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
    return *p;
}

// CC2
std::string CC2_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1]);
    std::string B = Operation::obj2varname(args[2]);
    if (args[2]->getId() == ID_CONST) {
        int w = args[2]->getWidth();
        A = "(" + A + " << " + args[2]->getStrWidth() + ")";
        if (args[2]->getValue() != 0) {
            A = "(" + A + " | " + args[2]->getStrValue() + ")";
        }
    } else {
        A = "(" + A + ", " + B + ")";
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
    A = "(" + A + ", " + B + ", " + C + ")";
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

// LSH: left shift
std::string LSH_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    std::string B = Operation::obj2varname(args[2]);
    if (SCV_is_sysc()) {
        A = "(" + A + " << " + B + ")";
    } else if (SCV_is_sv()) {
        A = "{" + A + "," + B + "'d0}";
    } else {
    }
    return A;
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

// ARRITEM
std::string ARRITEM_gen(GenObject **args) {
    ArrayObject *arr = static_cast<ArrayObject *>(args[1]);
    std::string ret = "";
    arr->setSelector(args[2]);
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

// SETARRITEM
std::string SETARRITEM_gen(GenObject **args) {
    ArrayObject *arr = static_cast<ArrayObject *>(args[1]);
#if 1
    if (arr->getName() == "dbg_npc") {
        bool st = true;
    }
#endif
    arr->setSelector(args[2]);
    std::string ret = Operation::addspaces();
    ret += Operation::obj2varname(args[3], "v");
    ret += " = ";
    ret += Operation::obj2varname(args[4]);
    ret += ";\n";
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
    ret += "if " + A + " {\n";
    return ret;
}

void IF(GenObject &a, const char *comment) {
    Operation *p = new Operation(comment);
    Operation::push_obj(p);
    p->igen_ = IF_gen;
    p->add_arg(p);
    p->add_arg(&a);
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
    ret += Operation::addspaces() + "} else if " + A + " {\n";
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
    ret += Operation::addspaces() + "} else {\n";
    spaces_++;
    return ret;
}

void ELSE(const char *comment) {
    Operation *p = new Operation(comment);
    Operation::pop_obj();
    Operation::push_obj(p);
    p->igen_ = ELSE_gen;
    p->add_arg(p);
}


// ENDIF
std::string ENDIF_gen(GenObject **args) {
    spaces_--;
    std::string ret = Operation::addspaces() + "}\n";
    return ret;
}

void ENDIF(const char *comment) {
    Operation::pop_obj();
    Operation *p = new Operation(comment);
    p->igen_ = ENDIF_gen;
    p->add_arg(p);
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
    ret += "switch " + A + " {\n";
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
    ret += Operation::addspaces() + "case " + A + ":";
    Operation *p = static_cast<Operation *>(args[0]);
    if (p->getComment().size()) {
        while (ret.size() < 60) {
            ret += " ";
        }
        ret += "// " + p->getComment();
    }
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
    ret += Operation::addspaces() + "default:";

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
    std::string ret = Operation::addspaces() + "break;\n";
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
    std::string ret = Operation::addspaces() + "}\n";
    return ret;
}

void ENDSWITCH(const char *comment) {
    Operation::pop_obj();
    Operation *p = new Operation(comment);
    p->igen_ = ENDSWITCH_gen;
    p->add_arg(p);
}


// FOR
std::string FOR_gen(GenObject **args) {
    std::string ret = Operation::addspaces();
    std::string i = Operation::obj2varname(args[1]);
    spaces_++;
    std::string start = Operation::obj2varname(args[2]);
    std::string end = Operation::obj2varname(args[3]);
    std::string dir = Operation::obj2varname(args[4]);

    ret += "for (int " + i + " = ";
    ret += start + "; ";
    ret += i;
    if (dir == "++") {
        ret += " < ";
    } else {
        ret += " >= ";
    }
    ret += end + "; ";
    ret += i + dir;
    ret += ") {\n";
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
    return *ret;
}

// ENDFOR
std::string ENDFOR_gen(GenObject **args) {
    spaces_--;
    std::string ret = Operation::addspaces() + "}\n";
    return ret;
}

void ENDFOR(const char *comment) {
    Operation::pop_obj();
    Operation *p = new Operation(comment);
    p->igen_ = ENDFOR_gen;
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


// NEW module instance
std::string NEW_gen(GenObject **args) {
    std::string ret = "";
    std::string ln = "";
    std::string idx = "";
    int tcnt = 0;
    std::string name = Operation::obj2varname(args[2]);
    ModuleObject *mod = static_cast<ModuleObject *>(args[1]);
    if (args[3]) {
        idx = Operation::obj2varname(args[3]);
        ret += Operation::addspaces();
        ret += "char tstr[256];\n";
        ret += Operation::addspaces();
        ret += "RISCV_sprintf(tstr, sizeof(tstr), \"" + name + "%d\", " + idx + ");\n";
    }
    ln = Operation::addspaces();
    ln += name;
    if (idx.size()) {
        ln += "[" + idx + "]";
    }
    ln += " = new " + args[1]->getType();
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
            tcnt++;
        }
        ret += ">";
    }

    ret += "(";
    if (idx.size()) {
        ret += "tstr";
    } else {
        ret += "\"" + name + "\"";
    }
    if (mod->isAsyncReset()) {
        ret += ", async_reset";
    }
    std::list<GenObject *>genlist;
    mod->getParamList(genlist);
    for (auto &g : genlist) {
        ret += ", " + g->getName();
    }

    ret += ");\n";

    std::list<GenObject *>iolist;
    mod->getIoList(iolist);
    for (auto &io : iolist) {
        ret += Operation::addspaces();
        ret += name;
        ret += static_cast<Operation *>(args[0])->gen_connection(io->getName());
        ret += ";";
        // todo comments
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
    if (args[2]) {
        ret += "[" + Operation::obj2varname(args[2]) + "]";
    }
    ret += "->";
    ret += args[3]->getName();
    ret += "(";
    ret += Operation::obj2varname(args[4]);
    ret += ")";
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

}
