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
    : GenObject(stackobj_[stackcnt_], ID_OPERATION, "", comment), argcnt_(0) {
}

Operation::Operation(GenObject *parent, const char *comment)
    : GenObject(parent, ID_OPERATION, "", comment), argcnt_(0) {
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

std::string Operation::obj2varname(EGenerateType v, GenObject *obj) {
    std::string ret = obj->getName();
    if (obj->getId() == ID_REG) {
        ret = "r." + obj->getName();
    } else if (obj->getId() == ID_INPUT) {
        ret = obj->getName() + ".read()";
    } else if (obj->getId() == ID_OPERATION) {
        ret = obj->generate(v);
    }
    return ret;
}

std::string ZEROS::generate(EGenerateType v) {
    std::string ret = addspaces();
    if (args[0u].obj->getId() == ID_REG) {
        ret += "v.";
    }
    ret += args[0].obj->getName();
    if (v == SYSC_ALL || v == SYSC_H || v == SYSC_CPP) {
        ret += " = 0";
    } else if (v == SV_ALL || v == SV_PKG || v == SV_MOD) {
        ret += " = '0";
    } else  {
        ret += " = (others => '0')";
    }
    ret +=  + ";\n";
    return ret;
}

std::string ONE::generate(EGenerateType v) {
    std::string ret = addspaces();
    if (args[0u].obj->getId() == ID_REG) {
        ret += "v.";
    }
    ret += args[0].obj->getName();
    if (v == SYSC_ALL || v == SYSC_H || v == SYSC_CPP) {
        ret += " = 1";
    } else if (v == SV_ALL || v == SV_PKG || v == SV_MOD) {
        char tstr[64];
        RISCV_sprintf(tstr, sizeof(tstr), "%dd'1", args[0].obj->getWidth());
        ret += " = " + std::string(tstr);
    } else  {
        ret += " = '1'";
    }
    ret +=  + ";\n";
    return ret;
}

std::string EQ::generate(EGenerateType v) {
    std::string ret = addspaces();
    ret += args[0].obj->getName() + " = " + args[1].obj->getValue(v) + ";\n";
    return ret;
}

std::string SETBIT::generate(EGenerateType v) {
    std::string ret = addspaces();
    ret += args[0].obj->getName();
    if (v == SYSC_ALL || v == SYSC_H || v == SYSC_CPP) {
         ret += "[" + args[1].obj->getValue(v) + "] = 1;\n";
    } else if (v == SV_ALL || v == SV_PKG || v == SV_MOD) {
        ret += "[" + args[1].obj->getValue(v) + "] = 1'b1;\n";
    } else {
        ret += "(" + args[1].obj->getValue(v) + ") := '1';\n";
    }
    return ret;
}

std::string NOT::generate(EGenerateType v) {
    std::string A = obj2varname(v, args[0].obj);
    name_ = "(!" + A + ")";
    return name_;
}

std::string OR2::generate(EGenerateType v) {
    std::string A = obj2varname(v, args[0].obj);
    std::string B = obj2varname(v, args[1].obj);
    name_ = "(" + A + " || " + B + ")";
    return name_;
}

std::string AND2::generate(EGenerateType v) {
    std::string A = obj2varname(v, args[0].obj);
    std::string B = obj2varname(v, args[1].obj);
    name_ += "(" + A + " && " + B + ")";
    return name_;
}

std::string IF::generate(EGenerateType v) {
    std::string A = obj2varname(v, args[0].obj);
    name_ += addspaces() + "if ";
    spaces_++;

    if (A.c_str()[0] == '(') {
        name_ += A;
    } else {
        name_ += "(" + A + ")";
    }
    name_ += " {\n";
    for (auto &e: entries_) {
        if (e->getId() != ID_OPERATION) {
            continue;
        }
        name_ += e->generate(v);
    }
    spaces_--;
    name_ += addspaces() + "}\n";
    return name_;
}

std::string ELSE::generate(EGenerateType v) {
    spaces_--;
    name_ += addspaces() + "} else {\n";
    spaces_++;
    for (auto &e: entries_) {
        if (e->getId() != ID_OPERATION) {
            continue;
        }
        name_ += e->generate(v);
    }
    spaces_--;
    name_ += addspaces() + "}\n";
    return name_;
}

}
