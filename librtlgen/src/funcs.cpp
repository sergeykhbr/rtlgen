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
#include "operations.h"

namespace sysvc {

FunctionObject::FunctionObject(GenObject *parent,
                                 const char *name,
                                 const char *comment)
    : GenObject(parent, ID_FUNCTION, name, comment) {
    retval_ = 0;
    Operation::start(this);
}

std::string FunctionObject::generate(EGenerateType v) {
    std::string ret = "";
    Operation::set_space(1);
    if (v == SYSC_ALL || v == SYSC_H || v == SYSC_CPP) {
        ret += generate_sysc();
    } else if (v == SV_ALL || v == SV_PKG || v == SV_MOD) {
        ret += generate_sysv();
    } else {
        ret += generate_vhdl();
    }
    return ret;
}

std::string FunctionObject::generate_sysc() {
    std::string ret = "";
    if (isStatic()) {
        ret += "static ";
    }
    if (retval_) {
        ret += retval_->getType(SYSC_ALL);
    } else {
        ret += "void";
    }
    ret += " ";
    ret += getName();
    ret += "(";
    for (auto &a : args_) {
        if (a->getId() == ID_INPUT || a->getId() == ID_OUTPUT) {
            ret += "\n    " + a->generate(SYSC_ALL);
            if (&a != &args_.back()) {
                ret += ",";
            }
        }
    }
    ret += ")";

    if (isStatic()) {
        ret += " {\n";
        if (retval_) {
            ret += "    " + retval_->getType(SYSC_ALL) + " " + retval_->getName() +";\n";
        }
        ret += "\n";
        for (auto &e: entries_) {
            if (e->getId() != ID_OPERATION) {
                continue;
            }
            ret += "    " + e->generate(SYSC_ALL) + "\n";
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


std::string FunctionObject::generate_sysv() {
    std::string ret = "";
    ret += "function automatic ";
    if (retval_) {
        ret += retval_->getType(SV_ALL);
    }
    ret += " ";
    ret += getName();
    if (args_.size()) {
        ret += "(";
    }
    for (auto &a : args_) {
        if (a->getId() == ID_INPUT || a->getId() == ID_OUTPUT) {
            ret += "\n    " + a->generate(SV_ALL);
            if (&a != &args_.back()) {
                ret += ",";
            }
        }
    }
    if (args_.size()) {
        ret += ")";
    }
    ret += ";\n";
    if (retval_) {
        ret += "    " + retval_->getType(SV_ALL) + " " + retval_->getName() +";\n";
    }
    ret += "\n";
    for (auto &e: entries_) {
        if (e->getId() != ID_OPERATION) {
            continue;
        }
        ret += "    " + e->generate(SV_ALL) + "\n";
    }
    if (retval_) {
        ret += "    return " + retval_->getName() + ";\n";
    }
    ret += "endfunction\n";
    return ret;
}


std::string FunctionObject::generate_vhdl() {
    std::string ret = "";
    return ret;
}
}
