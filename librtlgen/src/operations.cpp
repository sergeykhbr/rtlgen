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
#include "utils.h"

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

std::string Operation::obj2varname(EGenerateType v, GenObject *obj) {
    std::string ret = obj->getName();
    if (obj->getId() == ID_REG) {
        ret = "r." + obj->getName();
    } else if (obj->getId() == ID_INPUT) {
        ret = obj->getName();// + ".read()";
    } else if (obj->getId() == ID_VALUE
            || obj->getId() == ID_SIGNAL) {
        ret = obj->getName();
        GenObject *p = obj->getParent();
        while (p->getId() == ID_STRUCT_DEF) {
            ret = p->getName() + "." + ret;
            p = p->getParent();
        }
    } else if (obj->getId() == ID_OPERATION) {
        ret = obj->generate(v);
    }
    return ret;
}

// TEXT
std::string TEXT_gen(EGenerateType v, GenObject **args) {
    std::string ret = "";
    if (args[0]->getComment().size() == 0) {
        ret += "\n";
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

// SETZERO
std::string SETZERO_gen(EGenerateType v, GenObject **args) {
    std::string ret = Operation::addspaces();
    if (args[1]->getId() == ID_REG) {
        ret += "v.";
    }
    ret += args[1]->getName();
    if (v == SYSC_ALL || v == SYSC_H || v == SYSC_CPP) {
        ret += " = 0";
    } else if (v == SV_ALL || v == SV_PKG || v == SV_MOD) {
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
    if (args[1]->getId() == ID_REG) {
        ret += "v.";
    }
    ret += args[1]->getName();
    if (v == SYSC_ALL || v == SYSC_H || v == SYSC_CPP) {
        ret += " = 1";
    } else if (v == SV_ALL || v == SV_PKG || v == SV_MOD) {
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

// SETALLONE
std::string SETALLONE_gen(EGenerateType v, GenObject **args) {
    std::string ret = Operation::addspaces();
    if (args[1]->getId() == ID_REG) {
        ret += "v.";
    }
    ret += args[1]->getName();
    if (v == SYSC_ALL || v == SYSC_H || v == SYSC_CPP) {
        if (args[1]->getWidth() <= 32) {
            ret += " = ~0ul";
        } else if (args[1]->getWidth() <= 64) {
            ret += " = ~0ull";
        } else {
            SHOW_ERROR("Not implemented all ones for w=%d", args[1]->getWidth());
        }
    } else if (v == SV_ALL || v == SV_PKG || v == SV_MOD) {
        ret += " = '1";
    } else  {
        ret += " = (others => '1')";
    }
    ret +=  + ";";
    ret += Operation::addtext(v, args[0], ret.size());
    ret += "\n";
    return ret;
}

Operation &SETALLONE(GenObject &a, const char *comment) {
    Operation *p = new Operation(comment);
    p->igen_ = SETALLONE_gen;
    p->add_arg(p);
    p->add_arg(&a);    // output signal
    return *p;
}

// SETBIT
std::string SETBIT_gen(EGenerateType v, GenObject **args) {
    std::string ret = Operation::addspaces();
    ret += args[1]->getName();
    if (v == SYSC_ALL || v == SYSC_H || v == SYSC_CPP) {
         ret += "[";
         if (args[2]->getId() == ID_PARAM || args[2]->getId() == ID_DEF_PARAM) {
             ret += args[2]->getName();
         } else {
             ret += args[2]->getValue(v);
         }
         ret += "] = 1;";
    } else if (v == SV_ALL || v == SV_PKG || v == SV_MOD) {
         ret += "[";
         if (args[2]->getId() == ID_PARAM || args[2]->getId() == ID_DEF_PARAM) {
             ret += args[2]->getName();
         } else {
             ret += args[2]->getValue(v);
         }
         ret += "] = 1'b1;";
    } else {
        ret += "(" + args[2]->getValue(v) + ") := '1';";
    }
    ret += Operation::addtext(v, args[0], ret.size());
    ret += "\n";
    return ret;
}

Operation &SETBIT(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Operation(comment);
    p->igen_ = SETBIT_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}

// SETVAL
std::string SETVAL_gen(EGenerateType v, GenObject **args) {
    std::string ret = Operation::addspaces();
    ret += Operation::obj2varname(v, args[1]) + " = ";
    if (args[2]->getId() == ID_CONST) {
        ret += args[2]->getValue(v);
    } else if (args[2]->getId() == ID_VALUE
            || args[2]->getId() == ID_SIGNAL
            || args[2]->getId() == ID_PARAM) {
        ret += Operation::obj2varname(v, args[2]);
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

// EZ
std::string EZ_gen(EGenerateType v, GenObject **args) {
    std::string A = Operation::obj2varname(v, args[0]);
    A = "(" + A + " == 0)";
    return A;
}

Operation &EZ(GenObject &a, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = EZ_gen;
    p->add_arg(&a);
    return *p;
}

// NZ
std::string NZ_gen(EGenerateType v, GenObject **args) {
    std::string A = Operation::obj2varname(v, args[0]);
    A = "(" + A + " == 1)";
    return A;
}

Operation &NZ(GenObject &a, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = NZ_gen;
    p->add_arg(&a);
    return *p;
}

// INV
std::string INV_gen(EGenerateType v, GenObject **args) {
    std::string A = Operation::obj2varname(v, args[0]);
    A = "(~" + A + ")";
    return A;
}

Operation &INV(GenObject &a, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = INV_gen;
    p->add_arg(&a);
    return *p;
}

// OR2
std::string OR2_gen(EGenerateType v, GenObject **args) {
    std::string A = Operation::obj2varname(v, args[0]);
    std::string B = Operation::obj2varname(v, args[1]);
    A = "(" + A + " || " + B + ")";
    return A;
}

Operation &OR2(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = OR2_gen;
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}

// OR3
std::string OR3_gen(EGenerateType v, GenObject **args) {
    std::string A = Operation::obj2varname(v, args[0]);
    std::string B = Operation::obj2varname(v, args[1]);
    std::string C = Operation::obj2varname(v, args[2]);
    A = "(" + A + " || " + B + " || " + C + ")";
    return A;
}

Operation &OR3(GenObject &a, GenObject &b, GenObject &c, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = OR3_gen;
    p->add_arg(&a);
    p->add_arg(&b);
    p->add_arg(&c);
    return *p;
}

// OR4
std::string OR4_gen(EGenerateType v, GenObject **args) {
    std::string A = Operation::obj2varname(v, args[0]);
    std::string B = Operation::obj2varname(v, args[1]);
    std::string C = Operation::obj2varname(v, args[2]);
    std::string D = Operation::obj2varname(v, args[3]);
    A = "(" + A + " || " + B + " || " + C + " || " + D + ")";
    return A;
}

Operation &OR4(GenObject &a, GenObject &b, GenObject &c, GenObject &d, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = OR4_gen;
    p->add_arg(&a);
    p->add_arg(&b);
    p->add_arg(&c);
    p->add_arg(&d);
    return *p;
}

// AND2
std::string AND2_gen(EGenerateType v, GenObject **args) {
    std::string A = Operation::obj2varname(v, args[0]);
    std::string B = Operation::obj2varname(v, args[1]);
    A = "(" + A + " && " + B + ")";
    return A;
}

Operation &AND2(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = AND2_gen;
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}

// AND3
std::string AND3_gen(EGenerateType v, GenObject **args) {
    std::string A = Operation::obj2varname(v, args[0]);
    std::string B = Operation::obj2varname(v, args[1]);
    std::string C = Operation::obj2varname(v, args[2]);
    A = "(" + A + " && " + B + " && " + C + ")";
    return A;
}

Operation &AND3(GenObject &a, GenObject &b, GenObject &c, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = AND3_gen;
    p->add_arg(&a);
    p->add_arg(&b);
    p->add_arg(&c);
    return *p;
}

// IF
std::string IF_gen(EGenerateType v, GenObject **args) {
    std::string ret = Operation::addspaces();
    std::string A = Operation::obj2varname(v, args[0]);
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
}

}
