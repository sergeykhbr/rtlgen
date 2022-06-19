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

#include "funcs.h"

namespace sysvc {

FunctionObject::FunctionObject(GenObject *parent,
                                 const char *name,
                                 const char *comment)
    : GenObject(parent, ID_FUNCTION, name, comment) {
    retval_ = 0;
}

std::string FunctionObject::generate(EGenerateType v) {
    std::string ret = "";
    if (isStatic()) {
        ret += "static ";
    }
    if (retval_) {
        ret += retval_->getType(v);
    } else {
        ret += "void";
    }
    ret += " ";
    ret += getName();
    ret += "(";
    for (auto a : args_) {
        if (a->getId() == ID_INPUT || a->getId() == ID_OUTPUT) {
            ret += "\n    " + a->generate(v);
            if (&a != &args_.back()) {
                ret += ",";
            }
        }
    }
    ret += ")";

    if (isStatic()) {
        ret += " {\n";
        if (retval_) {
            ret += "    " + retval_->getType(v) + " " + retval_->getName() +";\n";
        }
        for (auto e: entries_) {
            ret += "    " + e->generate(v) + "\n";
        }
        if (retval_) {
            ret += "    return " + retval_->getName() + ";\n";
        }
        ret += "}\n";
    } else {
        ret += ";\n";
    }
    return ret;
}

}
