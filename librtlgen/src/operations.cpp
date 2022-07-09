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

std::string Operation::addtext(EGenerateType v, GenObject *obj, size_t curpos) {
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
        curname = obj->getValue(SYSC_ALL);
        curname += name;
    } else if (obj->getId() == ID_OPERATION) {
        curname = obj->generate(SYSC_ALL);
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

std::string Operation::obj2varname(GenObject *obj, const char *prefix) {
    std::string ret = "";
    if (!obj) {
        return ret;
    }
    ret = fullname(prefix, ret, obj);

    if (obj->getId() == ID_INPUT) {
        if (SCV_is_sysc()) {
            ret += ".read()";
        }
    }
    return ret;
}


std::string Operation::reset(std::string prefix, ModuleObject *m, std::string xrst) {
    std::string ret = "";
    std::string ln;
    if (SCV_is_sysc()) {
        ret += Operation::addspaces();
        if (prefix.c_str()[0] == 'v') {
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
        if (!m->is2DimReg()) {
            // reset using function
            ret += Operation::addspaces();
            ret += m->getName() + "_r_reset(" + prefix + ");\n";
        } else {
            // reset each register separatly
            for (auto &p: m->getEntries()) {
                if (!p->isReg()) {
                    continue;
                }
                if (p->getId() == ID_ARRAY_DEF) {
                    ret += Operation::addspaces();
                    ret += "for (int i = 0; i < " + p->getDepth(SYSC_ALL) + "; i++) {\n";
                    spaces_++;
                    std::list<GenObject *>::iterator it = p->getEntries().begin();  // element[0]
                    ln = Operation::addspaces() + prefix + "." + p->getName() + "[i]";
                    for (auto &s: (*it)->getEntries()) {
                        ret += ln + "." + s->getName() + " = " + s->getValue(SYSC_ALL) + ";\n";
                    }
                    if ((*it)->getEntries().size() == 0) {
                        ret += ln + " = " + p->getValue(SYSC_ALL);
                    }
                    spaces_--;
                    ret += Operation::addspaces();
                    ret += "}\n";
                } else {
                    ret += Operation::addspaces();
                    ret += prefix + "." + p->getName() + " = " + p->getValue(SYSC_ALL) + ";\n";
                }
            }
        }
        spaces_--;
        ret += Operation::addspaces() + "}";
    } else if (SCV_is_sv()) {
    } else {
    }
    return ret;
}


// TEXT
std::string TEXT_gen(EGenerateType v, GenObject **args) {
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
std::string ALLZEROS_gen(EGenerateType v, GenObject **args) {
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
std::string ALLONES_gen(EGenerateType v, GenObject **args) {
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
std::string BIT_gen(EGenerateType v, GenObject **args) {
    std::string ret = "";
    ret += Operation::obj2varname(args[1]);
    if (SCV_is_sysc()) {
         ret += "[";
         ret += Operation::obj2varname(args[2]);
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
    p->igen_ = BIT_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}

Operation &BIT(GenObject &a, int b, const char *comment) {
    Operation *p = new Operation(0, comment);
    char tstr[64];
    p->igen_ = BIT_gen;
    p->add_arg(p);
    p->add_arg(&a);
    RISCV_sprintf(tstr, sizeof(tstr), "%d", b);
    p->add_arg(new I32D(tstr));
    return *p;
}

// BITS
std::string BITS_gen(EGenerateType v, GenObject **args) {
    std::string ret = "";
    ret += Operation::obj2varname(args[1]);
    if (SCV_is_sysc()) {
         ret += "(";
         ret += Operation::obj2varname(args[2]);
         ret += ", ";
         ret += Operation::obj2varname(args[3]);
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
    p->igen_ = BITS_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&h);
    p->add_arg(&l);
    return *p;
}

Operation &BITS(GenObject &a, int h, int l, const char *comment) {
    Operation *p = new Operation(0, comment);
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
std::string CONST_gen(EGenerateType v, GenObject **args) {
    std::string ret = Operation::obj2varname(args[1]);
    return ret;
}

Operation &CONST(const char *val) {
    Operation *p = new Operation(0, "");
    p->igen_ = CONST_gen;
    p->add_arg(p);
    p->add_arg(new I32D(val));
    return *p;
}

Operation &CONST(const char *val, int width) {
    Operation *p = new Operation(0, "");
    char tstr[64];
    p->igen_ = CONST_gen;
    p->add_arg(p);
    RISCV_sprintf(tstr, sizeof(tstr), "%d", width);
    p->add_arg(new Logic(tstr, "", val));
    return *p;
}

// SETZERO
std::string SETZERO_gen(EGenerateType v, GenObject **args) {
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
    ret += Operation::addtext(v, args[0], ret.size());
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
std::string SETONE_gen(EGenerateType v, GenObject **args) {
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
    ret += Operation::addtext(v, args[0], ret.size());
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
std::string SETBIT_gen(EGenerateType v, GenObject **args) {
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
    ret += Operation::addtext(v, args[0], ret.size());
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
    Operation *p = new Operation(comment);
    char tstr[64];
    p->igen_ = SETBIT_gen;
    p->add_arg(p);
    p->add_arg(&a);
    RISCV_sprintf(tstr, sizeof(tstr), "%d", b);
    p->add_arg(new I32D(tstr));
    p->add_arg(&val);
    return *p;
}

// SETBITS
std::string SETBITS_gen(EGenerateType v, GenObject **args) {
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
    ret += Operation::addtext(v, args[0], ret.size());
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
std::string SETVAL_gen(EGenerateType v, GenObject **args) {
    std::string ret = Operation::addspaces();
    ret += Operation::obj2varname(args[1], "v") + " = ";
    if (args[2]->getId() == ID_CONST) {
        ret += args[2]->getValue(v);
    } else if (args[2]->getId() == ID_VALUE
            || args[2]->getId() == ID_INPUT
            || args[2]->getId() == ID_SIGNAL
            || args[2]->getId() == ID_PARAM) {
        ret += Operation::obj2varname(args[2]);
    } else {
        ret += args[2]->generate(v);
    }
    ret += ";";
    ret += Operation::addtext(v, args[0], ret.size());
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

// TO_INT
std::string TO_INT_gen(EGenerateType v, GenObject **args) {
    std::string A = Operation::obj2varname(args[1]);
    A = A + ".to_int()";
    return A;
}

Operation &TO_INT(GenObject &a, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = TO_INT_gen;
    p->add_arg(p);
    p->add_arg(&a);
    return *p;
}

// EQ
std::string EQ_gen(EGenerateType v, GenObject **args) {
    std::string A = Operation::obj2varname(args[1]);
    std::string B = Operation::obj2varname(args[2]);
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


// EZ
std::string EZ_gen(EGenerateType v, GenObject **args) {
    std::string A = Operation::obj2varname(args[1]);
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
    p->add_arg(p);
    p->add_arg(&a);
    return *p;
}

// NZ
std::string NZ_gen(EGenerateType v, GenObject **args) {
    std::string A = Operation::obj2varname(args[1]);
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
    p->add_arg(p);
    p->add_arg(&a);
    return *p;
}

// INV
std::string INV_gen(EGenerateType v, GenObject **args) {
    std::string A = Operation::obj2varname(args[1]);
    A = "(~" + A + ")";
    return A;
}

Operation &INV(GenObject &a, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = INV_gen;
    p->add_arg(p);
    p->add_arg(&a);
    return *p;
}

// OR2
std::string OR2_gen(EGenerateType v, GenObject **args) {
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
std::string OR3_gen(EGenerateType v, GenObject **args) {
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
std::string OR4_gen(EGenerateType v, GenObject **args) {
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

// ADD2
std::string ADD2_gen(EGenerateType v, GenObject **args) {
    std::string A = Operation::obj2varname(args[1]);
    std::string B = Operation::obj2varname(args[2]);
    A = "(" + A + " + " + B + ")";
    return A;
}

Operation &ADD2(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = ADD2_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}

// AND_REDCE
std::string AND_REDUCE_gen(EGenerateType v, GenObject **args) {
    std::string A = Operation::obj2varname(args[1]);
    A += ".and_reduce()";
    return A;
}

Operation &AND_REDUCE(GenObject &a, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = AND_REDUCE_gen;
    p->add_arg(p);
    p->add_arg(&a);
    return *p;
}


// AND2
std::string AND2_gen(EGenerateType v, GenObject **args) {
    std::string A = Operation::obj2varname(args[1]);
    std::string B = Operation::obj2varname(args[2]);
    A = "(" + A + " && " + B + ")";
    return A;
}

Operation &AND2(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = AND2_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}

// AND3
std::string AND3_gen(EGenerateType v, GenObject **args) {
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
std::string AND4_gen(EGenerateType v, GenObject **args) {
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
std::string DEC_gen(EGenerateType v, GenObject **args) {
    std::string A = Operation::obj2varname(args[1]);
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
std::string INC_gen(EGenerateType v, GenObject **args) {
    std::string A = Operation::obj2varname(args[1]);
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
std::string MUL2_gen(EGenerateType v, GenObject **args) {
    std::string A = Operation::obj2varname(args[1]);
    std::string B = Operation::obj2varname(args[2]);
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


// Select item in 2-dimensional array
std::string SELECTARRITEM_gen(EGenerateType v, GenObject **args) {
    std::string ret = "";
    ArrayObject *arr = static_cast<ArrayObject *>(args[1]);
    arr->setSelector(args[2]);
    return ret;
}

Operation &SELECTARRITEM(GenObject &arr, GenObject &mux, const char *comment) {
    Operation *p = new Operation(comment);
    p->igen_ = SELECTARRITEM_gen;
    p->add_arg(p);
    p->add_arg(&arr);
    p->add_arg(&mux);
    return *p;
}

// ARRITEM
std::string ARRITEM_gen(EGenerateType v, GenObject **args) {
    ArrayObject *arr = static_cast<ArrayObject *>(args[1]);
    arr->setSelector(args[2]);
    std::string ret = Operation::obj2varname(args[3]);
    return ret;
}

Operation &ARRITEM(GenObject &arr, GenObject &idx, GenObject &item, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = ARRITEM_gen;
    p->add_arg(p);
    p->add_arg(&arr);
    p->add_arg(&idx);
    p->add_arg(&item);
    return *p;
}

// SETARRITEM
std::string SETARRITEM_gen(EGenerateType v, GenObject **args) {
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
std::string IF_gen(EGenerateType v, GenObject **args) {
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
std::string ELSIF_gen(EGenerateType v, GenObject **args) {
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
std::string ELSE_gen(EGenerateType v, GenObject **args) {
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
std::string ENDIF_gen(EGenerateType v, GenObject **args) {
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


// FOR
std::string FOR_gen(EGenerateType v, GenObject **args) {
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
std::string ENDFOR_gen(EGenerateType v, GenObject **args) {
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
std::string SYNC_RESET_gen(EGenerateType v, GenObject **args) {
    ModuleObject *m = static_cast<ModuleObject *>(args[1]);
    std::string xrst = Operation::obj2varname(args[2]);
    std::string prefix = "v";
    std::string ret = Operation::reset(prefix, m, xrst);
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

}
