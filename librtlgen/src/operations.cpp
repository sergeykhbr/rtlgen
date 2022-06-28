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

namespace sysvc {

Operation::Operation(GenObject *parent,
                     const char *comment)
    : GenObject(0, ID_OPERATION, "", comment), argcnt_(0) {
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
    std::string ret = "";
    std::string prefix = "";
    if (args[0u]->getId() == ID_REG) {
        prefix += "v.";
    }
    ret += prefix + args[0]->getName();
    if (v == SYSC_ALL || v == SYSC_H || v == SYSC_CPP) {
        ret += " = 0;";
    } else if (v == SV_ALL || v == SV_PKG || v == SV_MOD) {
        ret += " = '0;";
    } else  {
        ret += " = (others => '0');";
    }
    return ret;
}


std::string EQ::generate(EGenerateType v) {
    std::string ret = "";
    ret += args[0]->getName() + " = " + args[1]->getValue(v) + ";";
    return ret;
}

std::string SETBIT::generate(EGenerateType v) {
    std::string ret = "";
    ret += args[0]->getName();
    if (v == SYSC_ALL || v == SYSC_H || v == SYSC_CPP) {
         ret += "[" + args[1]->getValue(v) + "] = 1;";
    } else if (v == SV_ALL || v == SV_PKG || v == SV_MOD) {
        ret += "[" + args[1]->getValue(v) + "] = 1'b1;";
    } else {
        ret += "(" + args[1]->getValue(v) + ") := '1';";
    }
    return ret;
}

std::string NOT::generate(EGenerateType v) {
    std::string A = obj2varname(v, args[0]);
    name_ = "(!" + A + ")";
    return name_;
}

std::string OR2::generate(EGenerateType v) {
    std::string A = obj2varname(v, args[0]);
    std::string B = obj2varname(v, args[1]);
    name_ = "(" + A + " || " + B + ")";
    return name_;
}

std::string AND2::generate(EGenerateType v) {
    std::string A = obj2varname(v, args[0]);
    std::string B = obj2varname(v, args[1]);
    name_ += "(" + A + " && " + B + ")";
    return name_;
}

std::string IF::generate(EGenerateType v) {
    std::string A = obj2varname(v, args[0]);
    name_ += "if " + A + " {\n";
    name_ += "}\n";
    return name_;
}

}
