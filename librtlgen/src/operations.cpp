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

int stackcnt_ = 0;
GenObject *stackobj_[256] = {0};

Operation::Operation(const char *comment)
    : GenObject(top_obj(), "", ID_OPERATION, "", comment), igen_(0), argcnt_(0) {
}

Operation::Operation(GenObject *parent, const char *comment)
    : GenObject(parent, "", ID_OPERATION, "", comment), igen_(0), argcnt_(0) {
}

void Operation::start(GenObject *owner) {
    stackcnt_ = 0;
    stackobj_[stackcnt_] = owner;
    SCV_set_local_module(owner);
}

void Operation::push_obj(GenObject *obj) {
    stackobj_[++stackcnt_] = obj;
}

void Operation::pop_obj() {
    stackobj_[stackcnt_--] = 0;
}

GenObject *Operation::top_obj() {
    return stackobj_[stackcnt_];
}

std::string Operation::addtext(GenObject *obj, size_t curpos) {
    std::string ret = "";
    if (obj->getComment().size()) {
        while (curpos++ < 60) {
            ret += " ";
        }
        ret += obj->addComment();
    }
    return ret;
}

std::string Operation::v_name(std::string v) {
    std::string ret = generate();
    if (v.size()) {
        ret += "." + v;
    }
    return ret;
}


std::string Operation::fullname(const char *prefix, std::string name, GenObject *obj) {
    if (!obj) {
        return name;
    }
    std::string read = "";
    GenObject *p = obj->getParent();
    std::string curname = "";
    if (obj->isConst()) {
        curname = obj->getStrValue();
    } else if (obj->isOperation()) {
        curname = obj->generate();
    } else if (obj->getSelector()) {
        curname = "";
        curname = fullname("r", curname, obj->getSelector());
        if (SCV_is_vhdl()) {
            curname = "(" + curname + ")";
        } else {
            curname = "[" + curname + "]";
        }
        if ((obj->isInput() && !obj->isVector()) && SCV_is_sysc()) {
            curname = obj->getName() + ".read()" + curname;
        } else {
            curname = obj->getName() + curname;
        }
        if (name.size()) {
            curname += ".";
        }
        curname += name;
        obj->setSelector(0);
    } else if (obj->isStruct()) {
        curname = obj->getName();
        if (name.size()) {
            curname += ".";
        }
        curname += name;
    } else if (SCV_is_sysc() && obj->isParamGeneric() && !obj->isParamTemplate()) {
        curname = obj->getName() + "_";
    } else if (SCV_is_sv_pkg() && obj->isParam() && !obj->isParamGeneric()) {
        GenObject *pfile = obj->getParent();
        while (pfile && !pfile->isFile()) {
            pfile = pfile->getParent();
        }
        if (pfile) {
            curname += pfile->getName() + "_pkg::";
        }
        curname += obj->getName();
    } else {
        curname = obj->getName();
    }

    if (p && p->isInput()) {
        if (SCV_is_sysc()) {
            read += "read().";
        }
    }

    if (p && p->isStruct()) {
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

    // IT IS JUST A BEGINNING OF THE fullname() FUNCTION REMOVING. IN PROGRESS!!!
    std::string tr = obj->r_name("");
    std::string tv = obj->v_name("");
    return ret;
}

// dst = r or v
// src = r, v or 0
std::string Operation::copyreg_entry(char *idx, std::string dst, std::string src, GenObject *p) {
    std::string ret = "";
    std::string i = std::string(idx);

    if (p->getObjDepth()) {
        ret += addspaces();
        ret += "for (int " + i + " = 0; " + i + " < " + p->getStrDepth() + "; " + i + "++) ";
        if (SCV_is_sysc()) {
            ret += "{\n";
        } else {
            ret += "begin\n";
        }
        pushspaces();
        std::string tdst = dst;
        std::string tsrc = src;
        tdst += "." + p->getName() + "[" + i + "]";
        if (tsrc.size()) {
            tsrc += "." + p->getName() + "[" + i + "]";
        }
        if (p->isStruct()) {
            idx[0]++;
            for (auto &s: p->getEntries()) {
                ret += copyreg_entry(idx, tdst, tsrc, s);
            }
            idx[0]--;
        } else {
            ret += addspaces() + tdst;
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
                ret += p->getStrValue();
            } else {
                // copy value
                ret += tsrc;
            }
            ret += ";\n";
        }
        popspaces();
        ret += addspaces();
        if (SCV_is_sysc()) {
            ret += "}\n";
        } else {
            ret += "end\n";
        }
    } else {
        ret += addspaces();
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
        ret += addspaces();
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
        ret += addspaces();
        if (dst[0] == 'v') {
            ret += "if ";
            if (xrst.size()) {
                ret += "(";
            }
            ret += "(";
            if (m->isAsyncResetParam()) {
                ret += "!async_reset_ && ";
            }
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
            if (m->getResetPort() && m->isAsyncResetParam()) {
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

        pushspaces();
        if (!m->is2DimReg()) {
            if (src == 0) {
                // reset using function
                ret += addspaces();
                if (dst[0] == 'n' && dst[1] == 'r' && (dst[2] == '\0' || dst[2] == '.')) {
                    ret += m->getType() + "_nr_reset(" + std::string(dst) + ")";
                } else {
                    ret += m->getType() + "_r_reset(" + std::string(dst) + ")";
                }
            } else {
                // copy data from src into dst
                ret += addspaces();
                ret += std::string(dst) + " = " + std::string(src);
            }
            ret += ";\n";
        } else {
            ret += copyreg(dst, src, m);
        }
        popspaces();
        ret += addspaces() + "}";
    } else if (SCV_is_sv()) {
        ret += addspaces();
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

        pushspaces();
        if (!m->is2DimReg()) {
            if (src == 0) {
                // reset using function
                ret += addspaces();
                if (dst[0] == 'r' && (dst[1] == '\0' || dst[1] == '.')) {
                    ret += std::string(dst) + " <= " + m->getType() + "_r_reset";
                } else if (dst[0] == 'n' && dst[1] == 'r' && (dst[2] == '\0' || dst[2] == '.')) {
                    ret += std::string(dst) + " <= " + m->getType() + "_nr_reset";
                } else {
                    ret += std::string(dst) + " = " + m->getType() + "_r_reset";
                }
            } else {
                // copy data from src into dst
                ret += addspaces();
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
        popspaces();
        ret += addspaces() + "end";
    } else {
    }
    return ret;
}

/** Standard class for standard 2-operands functions:
        (A + B), (A - B), (A & B), (A / B) etc
 */
uint64_t TwoStandardOperandsOperation::getWidth() {
    return a_->getWidth() >= b_->getWidth() ? a_->getWidth(): b_->getWidth();
}

std::string TwoStandardOperandsOperation::getStrValue() {
    std::string A = obj2varname(a_, "r", true);
    std::string B = obj2varname(b_, "r", true);
    A = "(" + A + getOperand() + B + ")";
    return A;
}

/** Operation with multiple arguments
 */
std::string NStandardOperandsOperation::getStrValue() {
    std::string ret = "";
    ret += getOpeningBrace();
    if (!oneline_) {
        pushspaces();
        pushspaces();
    }

    for (auto &p: getEntries()) {
        ret += obj2varname(p, "r", true);
        if (p != getEntries().back()) {
            ret += getOperand();
            if (oneline_) {
                ret += " ";
            } else {
                ret += "\n" + addspaces();
            }
        }
    }

    if (!oneline_) {
        popspaces();
        popspaces();
    }
    ret += getClosingBrace();
    return ret;
}


/**
    Generate commenting string:
        // text
 */
std::string TextOperation::generate() {
    std::string ret = "";
    if (getComment().size() == 0) {
        // Do nothing
    } else {
        ret += addspaces();
        if (SCV_is_vhdl()) {
            ret += "-- ";
        } else {
            ret += "// ";
        }
        ret += getComment();
    }
    ret += "\n";
    return ret;
}

/**
    Assign constant value to all bits. No parent.
 */
std::string AllConstOperation::getStrValue() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        if (v_) {
            ret += "~0ull";
        } else {
            ret += "0";
        }
    } else if (SCV_is_sv()) {
        if (v_) {
            ret += "'1";
        } else {
            ret += "'0";
        }
    } else {
        if (v_) {
            ret += "(others => '1')";
        } else {
            ret += "(others => '0')";
        }
    }
    return ret;
}

std::string BitOperation::generate() {
    std::string ret = "";
    ret += Operation::obj2varname(a_, "r", true);
    if (SCV_is_sysc()) {
         ret += "[";
         ret += Operation::obj2varname(idx_, "r", true);
         ret += "]";
    } else if (SCV_is_sv()) {
         ret += "[";
         ret += Operation::obj2varname(idx_);
         ret += "]";
    } else {
         ret += "(";
         ret += Operation::obj2varname(idx_);
         ret += ")";
    }
    return ret;
}

// BITS
std::string BitsOperation::generate() {
    std::string ret = "";
    bool use_shift = false;
    std::string t1 = a_->getType();
    if (SCV_is_sysc() && 
        (t1 == "uint64_t" || t1 == "int" || t1 == "uint32_t" || t1 == "uint16_t")) {
        use_shift = true;
        ret += "(";
    }
    ret += Operation::obj2varname(a_, "r", true);
    if (use_shift) {
        ret += " >> " + Operation::obj2varname(l_, "r", true);
        ret += ")";
    } else if (SCV_is_sysc()) {
        ret += "(";
        ret += Operation::obj2varname(h_, "r", true);
        ret += ", ";
        ret += Operation::obj2varname(l_, "r", true);
        ret += ")";
    } else if (SCV_is_sv()) {
        ret += "[";
        ret += Operation::obj2varname(h_, "r", true);
        ret += ": ";
        ret += Operation::obj2varname(l_, "r", true);
        ret += "]";
    } else {
        ret += "(";
        ret += Operation::obj2varname(h_, "r", true);
        ret += " downto ";
        ret += Operation::obj2varname(l_, "r", true);
        ret += ")";
    }
    return ret;
}

// BITSW
std::string BitswOperation::generate() {
    std::string ret = "";
    ret += Operation::obj2varname(a_, "r", true);
    if (SCV_is_sysc()) {
         ret += "(";
         ret += Operation::obj2varname(start_, "r", true);
         ret += " + ";
         ret += Operation::obj2varname(width_, "r", true);
         ret += " - 1, ";
         ret += Operation::obj2varname(start_, "r", true);
         ret += ")";
    } else if (SCV_is_sv()) {
         ret += "[";
         ret += Operation::obj2varname(start_, "r", true);
         ret += " +: ";
         ret += Operation::obj2varname(width_, "r", true);
         ret += "]";
    } else {
         ret += "(";
         ret += Operation::obj2varname(start_, "r", true);
         ret += " + ";
         ret += Operation::obj2varname(width_, "r", true);
         ret += " - 1 downto ";
         ret += Operation::obj2varname(start_, "r", true);
         ret += ")";
    }
    return ret;
}


/**
    Used to set 0 or 1, not ued with other values (but possible)
    todo: rename to SetDec..
 */
SetConstOperation::SetConstOperation(GenObject &a, uint64_t v, const char *comment)
    : Operation(top_obj(), comment), a_(&a) {
    if (a_->isLogic()) {
        b_ = new DecLogicConst(a_->getObjWidth(), v);
    } else {
        b_ = new DecConst(v);
    }
}

std::string SetConstOperation::generate() {
    std::string ret = addspaces();
    if (SCV_is_sysc() && a_->isClock()) {
        ret += "// ";   // do not clear clock module
    }
    ret += obj2varname(a_, "v");
    if (SCV_is_vhdl()) {
        ret += " := ";
    } else {
        ret += " = ";
    }
    ret += b_->getStrValue() + ";";
    ret += addtext(this, ret.size());
    ret += "\n";
    return ret;
}

// SETBIT
std::string SETBIT_gen(GenObject **args) {
    std::string ret = addspaces();
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
    std::string ret = addspaces();
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
    std::string ret = addspaces();
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
    std::string ret = addspaces();
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
    std::string ret = addspaces();
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
    std::string ret = addspaces();
    ret += Operation::obj2varname(args[1], "v") + " = ";
    if (SCV_is_sv() && args[2]->isString()) {
        ret += "{";
    }
    if (args[2]->isConst()) {
        ret += args[2]->getStrValue();
    } else if (args[2]->isValue()
            || args[2]->isClock()
            || args[2]->isParam()
            || args[2]->isStruct()) {
        ret += Operation::obj2varname(args[2]);
    } else {
        ret += args[2]->generate();
    }
    if (SCV_is_sv() && args[2]->isString()) {
        ret += "}";
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

// SETZ
std::string SETZ_gen(GenObject **args) {
    std::string ret = addspaces();

    if (SCV_is_sysc()) {
        // No systemc assigning Z    
        ret += "// ";
    }
    ret += Operation::obj2varname(args[1], "v");
    ret += " = 1'bz;";
    ret += Operation::addtext(args[0], ret.size());
    ret += "\n";
    return ret;
}

Operation &SETZ(GenObject &a, const char *comment) {
    Operation *p = new Operation(comment);
    p->igen_ = SETZ_gen;
    p->add_arg(p);
    p->add_arg(&a);
    return *p;
}


// SETSTR
Operation &SETSTR(GenObject &a, const char *str, const char *comment) {
    Operation *p = new Operation(comment);
    p->igen_ = SETVAL_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(new StringConst(str));
    return *p;
}

// SETSTRF
std::string SETSTRF_gen(GenObject **args) {
    std::string ret = addspaces();
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
    size_t cnt = args[3]->getValue();
    pushspaces();
    pushspaces();
    if (cnt > 1) {
        ret += ",\n" + addspaces();
    } else {
        ret += ", ";
    }
    for (size_t i = 0; i < cnt; i++) {
        if (i > 0) {
            ret += ",\n" + addspaces();
        }
        ret += Operation::obj2varname(args[5 + i]);
        if (SCV_is_sysc()
            && args[5+i]->isParamGeneric()
            && strstr(args[5+i]->getType().c_str(), "string")) {
            ret += ".c_str()";
        }
    }
    popspaces();
    popspaces();
    ret += ");\n";
    if (SCV_is_sysc()) {
        ret += addspaces() + Operation::obj2varname(args[1]) + " ";
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
    p->add_arg(new StringConst(fmt)); // 2
    p->add_arg(new DecConst(cnt)); // 3
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
    p->add_arg(new StringConst(fmt));    // 2
    p->add_arg(new DecConst(cnt)); // 3
    p->add_arg(new DecConst(1));   // 4
    va_list arg;
    va_start(arg, cnt);
    for (int i = 0; i < cnt; i++) {
        obj = va_arg(arg, GenObject *);
        p->add_arg(obj);    // 5+
    }
    va_end(arg);
    return *p;
}

// ADDSTRU8
std::string ADDSTRU8_gen(GenObject **args) {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret += addspaces() + "int tsz = RISCV_sprintf(tstr, sizeof(tstr), \"%s\", ";
        ret += Operation::obj2varname(args[2]) + ".c_str());\n";
        ret += addspaces() + "tstr[tsz++] = static_cast<char>(";
        ret += Operation::obj2varname(args[3], "r", true) + ".to_uint());\n";
        ret += addspaces() + "tstr[tsz] = 0;\n";
        ret += addspaces() + Operation::obj2varname(args[1]);
        ret += " = tstr;\n";
    } else {
        ret += addspaces();
        ret += Operation::obj2varname(args[1]) + " = {";
        ret += Operation::obj2varname(args[2]) + ", ";
        ret += Operation::obj2varname(args[3]);
        ret += "};\n";
    }
    return ret;
}

Operation &ADDSTRU8(GenObject &strout, GenObject &strin, GenObject &val) {
    Operation *p = new Operation("");
    p->igen_ = ADDSTRU8_gen;
    p->add_arg(p);  // 0
    p->add_arg(&strout); // 1
    p->add_arg(&strin); // 2
    p->add_arg(&val); // 3
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
    p->igen_ = BIG_TO_U64_gen;
    p->add_arg(p);
    p->add_arg(&a);
    return *p;
}

// TO_BIG
std::string ToBigOperation::generate() {
    std::string A = "";
    if (SCV_is_sysc()) {
        A = "sc_biguint<" + objWidth_->getStrValue() + ">(";
        A += Operation::obj2varname(a_, "r", true) + ")";
    } else {
        A += Operation::obj2varname(a_, "r", true);
    }
    return A;
}

// TO_INT
std::string TO_INT_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    if (SCV_is_sysc()) {
        if (!args[1]->isParam()) {      // params aren't use sc_uint<> templates
            A = A + ".to_int()";
        }
    } else {
        A = "int'(" + A + ")";
    }
    return A;
}

Operation &TO_INT(GenObject &a, const char *comment) {
    Operation *p = new Operation(0, comment);
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
    p->add_arg(p);
    p->add_arg(&a);
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
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}

// INV (arithemtic, logical)
std::string InvOperation::generate() {
    std::string A = obj2varname(a_, "r", true);
    if (SCV_is_sysc()) {
        if (logical_) {
            A = "(~" + A + ")";
        } else {
            A = "(!" + A + ")";
        }
    } else if (SCV_is_sv()) {
        A = "(~" + A + ")";
    } else if (SCV_is_vhdl()) {
        A = "(not " + A + ")";
    }

    return A;
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
    pushspaces();
    pushspaces();
    for (size_t i = 0; i < cnt; i++) {
        if (i > 0) {
            ret += "\n";
            ret += addspaces();
            ret += "|| ";
        }
        ret += Operation::obj2varname(args[2 + i]);
    }
    popspaces();
    popspaces();
    ret += ")";
    return ret;
}

Operation &ORx(size_t cnt, ...) {
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
}

// ORx_L
std::string ORx_L_gen(GenObject **args) {
    std::string ret = "(";
    size_t cnt = reinterpret_cast<size_t>(args[1]);
    pushspaces();
    pushspaces();
    for (size_t i = 0; i < cnt; i++) {
        if (i > 0) {
            ret += "\n";
            ret += addspaces();
            ret += "| ";
        }
        ret += Operation::obj2varname(args[2 + i]);
    }
    popspaces();
    popspaces();
    ret += ")";
    return ret;
}

Operation &ORx_L(size_t cnt, ...) {
    Operation *p = new Operation(0, "");
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
    pushspaces();
    pushspaces();
    for (size_t i = 0; i < cnt; i++) {
        if (i > 0) {
            ret += "\n";
            ret += addspaces();
            ret += "^ ";
        }
        ret += Operation::obj2varname(args[2 + i]);
    }
    popspaces();
    popspaces();
    ret += ")";
    return ret;
}

Operation &XORx(size_t cnt, ...) {
    Operation *p = new Operation(0, "");
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



// ADDx
std::string ADDx_gen(GenObject **args) {
    std::string ret = "(";
    size_t cnt = reinterpret_cast<size_t>(args[1]);
    pushspaces();
    pushspaces();
    for (size_t i = 0; i < cnt; i++) {
        if (i > 0) {
            ret += "\n";
            ret += addspaces();
            ret += "+ ";
        }
        ret += Operation::obj2varname(args[2 + i]);
    }
    popspaces();
    popspaces();
    ret += ")";
    return ret;
}

Operation &ADDx(size_t cnt, ...) {
    Operation *p = new Operation(0, "");
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
uint64_t CalcWidthOperation::getValue() {
    uint64_t ret = 0;
    for (auto &p: getEntries()) {
        ret += p->getWidth();
    }
    return ret;
}

std::string CalcWidthOperation::getStrValue() {
    std::string ret = "(";
    pushspaces();
    pushspaces();
    for (auto &p: getEntries()) {
        ret += p->getStrWidth() + "  // " + p->getName();
        if (p != getEntries().back()) {
            ret += "\n";
            ret += addspaces();
            ret += "+ ";
        }
    }
    popspaces();
    popspaces();
    ret += "\n" + addspaces() + ")";
    return ret;
}

// AND_REDUCE
std::string AndReduceOperation::getStrValue() {
    std::string A = obj2varname(a_, "r", true);
    if (SCV_is_sysc()) {
        A += ".and_reduce()";
    } else if (SCV_is_sv()) {
        A = "(&" + A + ")";
    } else if (SCV_is_vhdl()) {
        A = "and_reduce(" + A + ")";
    }
    return A;
}

// OR_REDUCE
std::string OrReduceOperation::getStrValue() {
    std::string A = obj2varname(a_, "r", true);
    if (SCV_is_sysc()) {
        A += ".or_reduce()";
    } else if (SCV_is_sv()) {
        A = "(|" + A + ")";
    } else if (SCV_is_vhdl()) {
        A = "or_reduce(" + A + ")";
    }
    return A;
}


// AND2_L
std::string And2Operation::getOperand() {
    if (SCV_is_vhdl()) {
        return std::string(" and ");
    }
    if (logical_) {
        return std::string(" & ");
    }
    return std::string(" && ");
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
    p->igen_ = AND3_L_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    p->add_arg(&c);
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
    pushspaces();
    pushspaces();
    for (size_t i = 0; i < cnt; i++) {
        if (i > 0) {
            ret += "\n";
            ret += addspaces();
            ret += "&& ";
        }
        ret += Operation::obj2varname(args[2 + i]);
    }
    popspaces();
    popspaces();
    ret += ")";
    return ret;
}

Operation &ANDx(size_t cnt, ...) {
    Operation *p = new Operation(0, "");
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
    pushspaces();
    pushspaces();
    for (size_t i = 0; i < cnt; i++) {
        if (i > 0) {
            ret += "\n";
            ret += addspaces();
            ret += "& ";
        }
        ret += Operation::obj2varname(args[2 + i]);
    }
    popspaces();
    popspaces();
    ret += ")";
    return ret;
}

Operation &ANDx_L(size_t cnt, ...) {
    Operation *p = new Operation(0, "");
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


// INCVAL
std::string INCVAL_gen(GenObject **args) {
    std::string ret = addspaces();
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

// SPLx
std::string SplitOperation::generate() {
    std::string ret = "";
    char tstr[64];
    int w = static_cast<int>(a_->getWidth());
    std::string busname = Operation::obj2varname(a_, "r", true);

    for (auto &p: getEntries()) {
        ret += addspaces();
        ret += obj2varname(p);
        ret += " = ";
        ret += busname;
        if (p->getWidth() > 1) {
            if (SCV_is_sysc()) {
                RISCV_sprintf(tstr, sizeof(tstr), "(%d, %d)",
                                w - 1, w - static_cast<int>(p->getWidth()));
            } else if (SCV_is_sv()) {
                RISCV_sprintf(tstr, sizeof(tstr), "[%d: %d]",
                                w - 1, w - static_cast<int>(p->getWidth()));
            } else {
                RISCV_sprintf(tstr, sizeof(tstr), "(%d downto %d)",
                                w - 1, w - static_cast<int>(p->getWidth()));
            }
            ret += std::string(tstr);
        } else {
            RISCV_sprintf(tstr, sizeof(tstr), "[%d]", w - 1);
            ret += std::string(tstr);
        }
        ret += ";\n";
        w -= static_cast<int>(p->getWidth());
    }
    return ret;
}

// CCx
std::string CCxOperation::getOpeningBrace() {
    if (SCV_is_sv()) {
        return std::string("{");
    }
    return NStandardOperandsOperation::getOpeningBrace();
}
std::string CCxOperation::getClosingBrace() {
    if (SCV_is_sv()) {
        return std::string("}");
    }
    return NStandardOperandsOperation::getClosingBrace();
}

std::string CCxOperation::getOperand() {
    if (SCV_is_vhdl()) {
        return std::string(" &");
    }
    return std::string(",");
}

uint64_t CCxOperation::getWidth() {
    uint64_t ret = 0;
    for (auto &p: getEntries()) {
        ret += p->getWidth();
    }
    return ret;
}

std::string CCxOperation::getStrValue() {
    std::string ret = "";
    std::list<GenObject *>::iterator it = getEntries().begin();
    GenObject *A = *it;
    it++;
    GenObject *B = *it;
    it++;
    if (SCV_is_sysc() && getEntries().size() == 2 && B->isConst()) {
        ret = getOpeningBrace();
        if (B->getValue() != 0) {
            ret += "(";
        }
        ret += obj2varname(A, "r", true) + " << " + B->getStrWidth();
        if (B->getValue() != 0) {
            ret += ") | " + B->getStrValue();
        }
        ret += getClosingBrace();
    } else if (SCV_is_sysc() && getEntries().size() == 3
        && A->isConst() && A->getValue() == 0 && (*it)->isConst()) {
        GenObject *C = (*it);
        ret = getOpeningBrace();
        if (C->getValue() != 0) {
            ret += "(";
        }
        ret += obj2varname(B, "r", true) + " << " + C->getStrWidth();
        if (C->getValue() != 0) {
            ret += ") | " + C->getStrValue();
        } else {
            ret += ")";
        }
        getClosingBrace();
    } else {
        return NStandardOperandsOperation::getStrValue();
    }

    return ret;
}

#if 0
std::string CC2_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1], "r", true);
    std::string B = Operation::obj2varname(args[2], "r", true);
    if (SCV_is_sysc()) {
        if (args[2]->isConst()) {
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


// CC3
std::string CC3_gen(GenObject **args) {
    std::string A = Operation::obj2varname(args[1]);
    std::string B = Operation::obj2varname(args[2]);
    std::string C = Operation::obj2varname(args[3]);
    if (SCV_is_sysc()) {
        if (args[1]->isConst() && args[1]->getValue() == 0
            && args[3]->isConst()) {
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

// CCx
std::string CCx_gen(GenObject **args) {
    std::string ret;
    if (SCV_is_sysc()) {
        ret = "(";
    } else if (SCV_is_sv()) {
        ret = "{";
    }
    size_t cnt = reinterpret_cast<size_t>(args[1]);
    pushspaces();
    pushspaces();
    for (size_t i = 0; i < cnt; i++) {
        if (i > 0) {
            if (SCV_is_vhdl()) {
                ret += "&\n";
            } else {
                ret += ",\n";
            }
            ret += addspaces();
        }
        ret += Operation::obj2varname(args[2 + i]);
    }
    popspaces();
    popspaces();
    if (SCV_is_sysc()) {
        ret += ")";
    } else if (SCV_is_sv()) {
        ret += "}";
    }
    return ret;
}
#endif


// LSH: left shift
std::string LshOperation::getStrValue() {
    std::string A = obj2varname(a_, "r", true);
    std::string B = obj2varname(sz_);
    if (SCV_is_sysc()) {
        A = "(" + A + " << " + B + ")";
    } else if (SCV_is_sv()) {
        if (sz_->isParam() || sz_->isConst()) {
            A = "{" + A + ", {" + B + "{1'b0}}}";
        } else {
            A = "(" + A + " << " + B + ")";
        }
    } else if (SCV_is_vhdl()) {
        A = "(" + A + " sll " + B + ")";
    }
    return A;
}

// POW2: 1 << sz
std::string Pow2Operation::getStrValue() {
    std::string A = "";
    std::string sz = obj2varname(sz_);
    if (SCV_is_sysc()) {
        A = "(1 << " + sz + ")";
    } else if (SCV_is_sv()) {
        A = "(2**" + sz + ")";
    } else if (SCV_is_vhdl()) {
        A = "(2**" + sz + ")";
    }
    return A;
}

// RSH: right shift
std::string RshOperation::generate() {
    std::string A = obj2varname(a_, "r", true);
    std::string B = obj2varname(sz_);
    if (SCV_is_sysc()) {
        A = "(" + A + " >> " + B + ")";
    } else if (SCV_is_sv()) {
        if (sz_->isConst()) {
            A = "{'0, " + A + "[" + a_->getStrWidth() + " - 1: " + B + "]}";
        } else {
            A = "(" + A + " >> " + B + ")";
        }
    } else if (SCV_is_vhdl()) {
        A = "(" + A + " srl " + B + ")";
    }
    return A;
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
    std::string ret = addspaces();
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

// Set bits value in array element
// SETARRITEMBIT
std::string SETARRITEMBIT_gen(GenObject **args) {
    args[1]->setSelector(args[2]);
    std::string ret = addspaces();
    ret += Operation::obj2varname(args[3], "v");
    if (SCV_is_sysc()) {
        ret += "[";
    } else if (SCV_is_sv()) {
        ret += "[";
    } else {
    }

    ret += Operation::obj2varname(args[4]);

    if (SCV_is_sysc()) {
        ret += "]";
    } else if (SCV_is_sv()) {
        ret += "]";
    } else {
    }
    ret += " = ";
    ret += Operation::obj2varname(args[5]);
    ret += ";";
    ret += Operation::addtext(args[0], ret.size());
    ret += "\n";
    return ret;
}

Operation &SETARRITEMBIT(GenObject &arr, GenObject &idx, GenObject &item, 
                           GenObject &bitidx, GenObject &val, const char *comment) {
    Operation *p = new Operation(comment);
    p->igen_ = SETARRITEMBIT_gen;
    p->add_arg(p);      // 0
    p->add_arg(&arr);   // 1
    p->add_arg(&idx);   // 2
    p->add_arg(&item);  // 3
    p->add_arg(&bitidx); // 4
    p->add_arg(&val);   // 5
    return *p;
}

// SETARRITEMBITS
std::string SETARRITEMBITSW_gen(GenObject **args) {
    args[1]->setSelector(args[2]);
    std::string ret = addspaces();
    ret += Operation::obj2varname(args[3], "v");
    if (SCV_is_sysc()) {
        ret += "(";
        ret += Operation::obj2varname(args[4]);
        ret += " + ";
        ret += Operation::obj2varname(args[5]);
        ret += "- 1, ";
        ret += Operation::obj2varname(args[4]);
        ret += ")";
    } else if (SCV_is_sv()) {
        ret += "[";
        ret += Operation::obj2varname(args[4]);
        ret += " +: ";
        ret += Operation::obj2varname(args[5]);
        ret += "]";
    } else {
    }
    ret += " = ";
    ret += Operation::obj2varname(args[6]);
    ret += ";";
    ret += Operation::addtext(args[0], ret.size());
    ret += "\n";
    return ret;
}

Operation &SETARRITEMBITSW(GenObject &arr, GenObject &idx, GenObject &item, 
                           GenObject &start, GenObject &width, GenObject &val, const char *comment) {
    Operation *p = new Operation(comment);
    p->igen_ = SETARRITEMBITSW_gen;
    p->add_arg(p);      // 0
    p->add_arg(&arr);   // 1
    p->add_arg(&idx);   // 2
    p->add_arg(&item);  // 3
    p->add_arg(&start); // 4
    p->add_arg(&width); // 5
    p->add_arg(&val);   // 6
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
    std::string ret = addspaces();
    std::string A = Operation::obj2varname(args[1]);
    pushspaces();

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
    popspaces();
    if (SCV_is_sysc()) {
        ret += addspaces() + "} else if " + A + " {";
    } else {
        ret += addspaces() + "end else if " + A + " begin";
    }
    ret += Operation::addtext(args[0], ret.size());
    ret += "\n";
    pushspaces();
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
    popspaces();
    if (SCV_is_sysc()) {
        ret += addspaces() + "} else {\n";
    } else {
        if (args[1]) {
            ret += addspaces();
            ret += "end: " + args[1]->getStrValue().substr(1, args[1]->getStrValue().size()-2) + "_en\n";
            ret += addspaces();
            ret += "else begin: " + args[1]->getStrValue().substr(1, args[1]->getStrValue().size()-2) + "_dis";
        } else {
            ret += addspaces();
            ret += "end else begin";
        }
        ret += "\n";

    }
    pushspaces();
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
    popspaces();
    if (SCV_is_sysc()) {
        ret = addspaces() + "}\n";
    } else {
        ret = addspaces() + "end";
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
    std::string ret = addspaces();
    std::string A = Operation::obj2varname(args[1], "r", true);
    pushspaces();

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
    popspaces();
    if (SCV_is_sysc()) {
        ret += addspaces() + "case " + A + ":";
    } else {
        ret += addspaces() + A + ": begin";
    }
    ret += Operation::addtext(args[0], ret.size());
    ret += "\n";
    pushspaces();
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
    popspaces();
    ret += addspaces();
    if (SCV_is_sysc()) {
        ret += "default:";
    } else {
        ret += "default: begin";
    }
    ret += "\n";
    pushspaces();
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
        ret += addspaces() + "break;\n";
    } else {
        popspaces();
        ret += addspaces() + "end\n";
        pushspaces();
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
    popspaces();
    std::string ret = addspaces();
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
    std::string ret = addspaces();
    GenObject *op = args[0];
    std::string i = Operation::obj2varname(args[1]);
    pushspaces();
    std::string start = Operation::obj2varname(args[2]);
    std::string end = Operation::obj2varname(args[3]);
    std::string dir = Operation::obj2varname(args[4]);
    GenObject *gename = args[5];


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
    I32D *ret = new I32D(&start, i, 0);
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
    popspaces();
    if (SCV_is_sysc()) {
        ret = addspaces() + "}\n";
    } else {
        GenObject *gename = args[1];
        ret = addspaces() + "end";
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
    std::string ret = addspaces();
    std::string A = Operation::obj2varname(args[1], "r", true);
    pushspaces();

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
    popspaces();
    std::string ret = addspaces();
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
    std::string ret = addspaces();
    if (args[1]) {
        ret += Operation::obj2varname(args[1], "v") + " = ";
    }
    ret += args[2]->getName();
    ret += "(";
    size_t cnt = reinterpret_cast<size_t>(args[3]);
    pushspaces();
    pushspaces();
    for (size_t i = 0; i < cnt; i++) {
        if (i > 0) {
            ret += ",\n" + addspaces();
        }
        ret += Operation::obj2varname(args[4 + i]);
    }
    popspaces();
    popspaces();

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
    std::string ret = addspaces();
    ret += Operation::obj2varname(args[1]);
    if (SCV_is_sysc()) {
        ret += " = fopen(";
        ret += Operation::obj2varname(args[2]);
        if (args[2]->isString()) {
            ret += ".c_str()";
        }
        ret += ", \"wb\");\n";
    } else {
        ret += " = $fopen(" + Operation::obj2varname(args[2]) + ", \"wb\");\n";
        ret += addspaces() + "assert (" + Operation::obj2varname(args[1]) + ")\n";
        ret += addspaces() + "else begin\n";
        ret += addspaces() + "    $warning(\"Cannot open log-file\");\n";
        ret += addspaces() + "end\n";
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
    std::string ret = addspaces();
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

// FWRITE
std::string FWRITECHAR_gen(GenObject **args) {
    std::string ret = addspaces();
    if (SCV_is_sysc()) {
        ret += "fwrite(";
        ret += Operation::obj2varname(args[2]) + "to_uint(), 1, 1, ";
        ret += Operation::obj2varname(args[1]);
    } else {
        ret += "$fwrite(";
        ret += Operation::obj2varname(args[1]);
        ret += ", \"%c\", ";
        ret += Operation::obj2varname(args[2]);
    }
    ret += ");\n";
    return ret;
}

void FWRITECHAR(GenObject &f, GenObject &str) {
    Operation *p = new Operation("");
    p->igen_ = FWRITECHAR_gen;
    p->add_arg(p);
    p->add_arg(&f);
    p->add_arg(&str);
}

// FFLUSH
std::string FFLUSH_gen(GenObject **args) {
    std::string ret = addspaces();
    if (SCV_is_sv()) {
        ret += "$";
    }
    ret += "fflush(";
    ret += Operation::obj2varname(args[1]);
    ret += ");\n";
    return ret;
}

void FFLUSH(GenObject &f) {
    Operation *p = new Operation("");
    p->igen_ = FFLUSH_gen;
    p->add_arg(p);
    p->add_arg(&f);
}

// READMEMH
std::string READMEMH_gen(GenObject **args) {
    std::string ret = addspaces();
    if (SCV_is_sysc()) {
        ret += "SV_readmemh(";
        ret += Operation::obj2varname(args[1]) + ".c_str(), ";
        ret += Operation::obj2varname(args[2]);
        ret += ");\n";
    } else if (SCV_is_sv()) {
        ret += "$readmemh(";
        ret += Operation::obj2varname(args[1]) + ", ";
        ret += Operation::obj2varname(args[2]);
        ret += ");\n";
    } else if (SCV_is_vhdl()) {
        ret += Operation::obj2varname(args[2]);
        ret += " = init_rom(";
        ret += Operation::obj2varname(args[1]) + ");\n";
    }
    return ret;
}

void READMEMH(GenObject &fname, GenObject &mem) {
    Operation *p = new Operation("");
    p->igen_ = READMEMH_gen;
    p->add_arg(p);
    p->add_arg(&fname);
    p->add_arg(&mem);
    mem.getParentFile()->setSvApiUsed();
}

// DISPLAYSTR
std::string DISPLAYSTR_gen(GenObject **args) {
    std::string ret = addspaces();
    if (SCV_is_sysc()) {
        ret += "SV_display(";
        ret += Operation::obj2varname(args[1]) + ".c_str()";
    } else {
        ret += "$display(\"%s\", ";
        ret += Operation::obj2varname(args[1]);
    }
    ret += ");\n";
    return ret;
}

void DISPLAYSTR(GenObject &str) {
    Operation *p = new Operation("");
    p->igen_ = DISPLAYSTR_gen;
    p->add_arg(p);
    p->add_arg(&str);
    str.getParentFile()->setSvApiUsed();
}


// NEW module instance
std::string NewOperation::generate() {
    if (SCV_is_sysc()) {
        return generate_sc();
    } else if (SCV_is_sv()) {
        return generate_sv();
    }
    return generate_vhdl();
}

std::string NewOperation::generate_sc() {
    std::string ret = "";
    std::string ln = "";
    std::string lasttmpl = "";
    std::string idx = "";
    int tcnt = 0;

    ln = addspaces();
    if (idx_) {
        idx = obj2varname(idx_);
        ret += addspaces();
        ret += "char tstr[256];\n";
        ret += addspaces();
        ret += "RISCV_sprintf(tstr, sizeof(tstr), \"" + instname_ + "%d\", " + idx + ");\n";
    }
    ln += instname_;
    if (idx.size()) {
        ln += "[" + idx + "]";
    }
    ln += " = new " + m_->getType();
    ret += ln;
    
    std::list<GenObject *>tmpllist;
    m_->getTmplParamList(tmpllist);
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
            ret += e->generate();
            lasttmpl = e->generate(); // need to properly compute space value
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
        ret += "\"" + instname_ + "\"";
    }
    if (m_->isAsyncResetParam() && m_->getAsyncResetParam() == 0) {
        ret += ", async_reset";
    }
    std::list<GenObject *>genlist;
    m_->getParamList(genlist);
    for (auto &g : genlist) {
        ret += ",\n";
        for (int i = 0; i <= ln.size(); i++) {
            ret += " ";
        }
        ret += g->getStrValue();
    }
    ret += ");\n";

    std::list<GenObject *>iolist;
    m_->getIoList(iolist);
    for (auto &io : iolist) {
        ret += addspaces();
        ret += instname_;
        ret += gen_connection(io->getName());
        ret += ";";
        ret += "\n";
    }
    return ret;
}

std::string NewOperation::generate_sv() {
    std::string ret = "";
    std::string ln = "";
    std::string idx = "";
    int tcnt = 0;

    ln = addspaces();
    ln += m_->getType() + " ";
    ret += ln;
    
    std::list<GenObject *>genlist;
    m_->getTmplParamList(genlist);
    m_->getParamList(genlist);    
    tcnt = 0;
    if ((m_->isAsyncResetParam() && m_->getAsyncResetParam() == 0) || genlist.size()) {
        ret += "#(\n";
        pushspaces();
        if (m_->isAsyncResetParam() && m_->getAsyncResetParam() == 0) {
            ret += addspaces() + ".async_reset(async_reset)";
            if (genlist.size()) {
                ret += ",";
            }
            ret += "\n";
            tcnt++;
        }
        for (auto &e : genlist) {
            ret += addspaces() + "." + e->getName();
#if 1
            ret += "(" + e->getStrValue() + ")";
#else
            if (e->getObjValue()) {
                if (e->getObjValue()->getName() == "") {
                    ret += "(" + e->getObjValue()->getStrValue() + ")";
                } else {
                    // parameter is connected to some config parameter:
                    ret += "(" + e->getObjValue()->getName() + ")";
                }
            } else if (e->isParamTemplate()) {
                ret += "(" + e->generate() + ")";
            } else {
                ret += "(" + e->getName() + ")";
            }
#endif
            if (e != genlist.back()) {
                ret += ",";
            }
            ret += "\n";
            tcnt++;
        }
        popspaces();
        ret += addspaces() + ") ";
    }
    
    ret += instname_ + " (";
    pushspaces();

    std::list<GenObject *>iolist;
    m_->getIoList(iolist);
    ret += "\n";
    for (auto &io : iolist) {
        ret += gen_connection(io->getName());
        if (io != iolist.back()) {
            ret += ",";
        }
        ret += "\n";
    }
    popspaces();
    ret += addspaces() + ");";
    ret += "\n";
    return ret;
}

std::string NewOperation::generate_vhdl() {
    std::string ret = "";
    std::string ln = "";
    std::string idx = "";
    int tcnt = 0;

    ln = addspaces();
    ln += instname_ + " : " + m_->getType() + " ";
    ret += ln;
    
    std::list<GenObject *>tmpllist;
    m_->getTmplParamList(tmpllist);
    m_->getParamList(tmpllist);    
    tcnt = 0;
    if ((m_->isAsyncResetParam() && m_->getAsyncResetParam() == 0) || tmpllist.size()) {
        ret += "generic map (\n";
        pushspaces();
        if (m_->isAsyncResetParam() && m_->getAsyncResetParam() == 0) {
            ret += addspaces() + "async_reset => async_reset";
            if (tmpllist.size()) {
                ret += ",";
            }
            ret += "\n";
            tcnt++;
        }
        for (auto &e : tmpllist) {
            ret += addspaces();
#if 1
            ret += e->getName() + " => " + e->getStrValue();
#else
            if (e->getObjValue()) {
                // generic parameter but with the defined string value
                ret += e->getName() + " => " + e->getObjValue()->getName();
            } else if (e->isParamTemplate()) {
                ret += e->getName() + " => " + e->generate();
            } else {
                ret += e->getName() + " => " + e->getName();
            }
#endif
            if (e != tmpllist.back()) {
                ret += ",";
            }
            ret += "\n";
            tcnt++;
        }
        popspaces();
        ret += addspaces() + ") ";
    }
    
    ret += "port map (";
    pushspaces();

    std::list<GenObject *>iolist;
    m_->getIoList(iolist);
    ret += "\n";
    for (auto &io : iolist) {
        ret += gen_connection(io->getName());
        if (io != iolist.back()) {
            ret += ",";
        }
        ret += "\n";
    }
    popspaces();
    ret += addspaces() + ");";
    ret += "\n";
    return ret;
}


// CONNECT
std::string ConnectOperation::generate() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        if (idx_) {
            ret += "[" + obj2varname(idx_) + "]";
        }
        ret += "->";
        ret += port_->getName() + "(" + obj2varname(s_) + ")";
    } else if (SCV_is_sv()) {
        ret += addspaces() + ".";
        ret += port_->getName() + "(" + obj2varname(s_) + ")";
    } else {
        ret += addspaces();
        ret += port_->getName() + " => " + obj2varname(s_);
    }
    return ret;
}

// DECLARE_TSTR
std::string DECLARE_TSTR_gen(GenObject **args) {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret += addspaces() + "char tstr[256];\n";
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
        ret += addspaces() + "// initial\n";
    } else {
        ret += addspaces() + "initial begin\n";
        pushspaces();
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
        ret += addspaces() + "// end initial\n";
    } else {
        popspaces();
        ret += addspaces() + "end\n";
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
        ret += addspaces() + "// generate\n";
    } else {
        ret += addspaces() + "generate\n";
        pushspaces();
    }
    return ret;
}

void GENERATE(const char *name, const char *comment) {
    Operation *p = new Operation(comment);
    Operation::push_obj(p);
    p->igen_ = GENERATE_gen;
    p->add_arg(p);
    p->add_arg(new StringConst(name));
}

// ENDGENERATE
std::string ENDGENERATE_gen(GenObject **args) {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret += addspaces() + "// endgenerate\n";
    } else {
        popspaces();
        ret += addspaces() + "endgenerate\n";
    }
    return ret;
}

void ENDGENERATE(const char *name, const char *comment) {
    Operation::pop_obj();
    Operation *p = new Operation(comment);
    p->igen_ = ENDGENERATE_gen;
    p->add_arg(p);
    p->add_arg(new StringConst(name));
}

/**
    Assigning value in the out-of-combination method:
        a = 0
        a = b
 */
std::string AssignOperation::generate() {
    std::string ret = addspaces();
    if (SCV_is_sv()) {
        ret += "assign ";
    }
    ret += obj2varname(a_, "v");
    if (SCV_is_vhdl()) {
        ret += " <= ";
    } else {
        ret += " = ";
    }
    if (b_) {
        ret += obj2varname(b_, "r");;
    } else if (SCV_is_sysc()) {
        ret += "0";
    } else if (SCV_is_sv()) {
        if (a_->getWidth() == 1) {
            ret += "1'b0";
        } else {
            ret += "'0";
        }
    } else if (SCV_is_vhdl()) {
        if (a_->getWidth() == 1) {
            ret += "'0'";
        } else {
            ret += "(others => '0')";
        }
    }
    ret +=  ";";
    ret += addtext(this, ret.size());
    ret += "\n";
    return ret;
}

}
