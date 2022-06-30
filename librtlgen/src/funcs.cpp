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
    if (getpReturn()) {
        ret += getpReturn()->getType(SYSC_ALL);
    } else {
        ret += "void";
    }
    ret += " ";
    ret += getName();
    ret += "(";

    // Compute total number of arguments
    int argtotal = 0;
    int argcnt = 0;
    for (auto &a : entries_) {
        if (a->getId() == ID_INPUT || a->getId() == ID_OUTPUT) {
            argtotal++;
        }
    }
    // Generate list of arguments
    for (auto &a : entries_) {
        if (a->getId() == ID_INPUT || a->getId() == ID_OUTPUT) {
            ret += "\n    " + a->generate(SYSC_ALL);
            if (++argcnt < argtotal) {
                ret += ",";
            }
        }
    }
    ret += ")";

    if (isStatic()) {
        ret += " {\n";
        for (auto &e: entries_) {
            if (e->getId() != ID_VALUE) {
                continue;
            }
            ret += "    " + e->getType(SYSC_ALL) + " " + e->getName() + ";\n";
        }
        ret += "\n";
        for (auto &e: entries_) {
            if (e->getId() != ID_OPERATION) {
                continue;
            }
            ret += e->generate(SYSC_ALL);
        }
        if (getpReturn()) {
            ret += "    return " + getpReturn()->getName() + ";\n";
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
    if (getpReturn()) {
        ret += getpReturn()->getType(SV_ALL);
    }
    ret += " ";
    ret += getName();
    // Compute total number of arguments
    int argtotal = 0;
    int argcnt = 0;
    for (auto &a : entries_) {
        if (a->getId() == ID_INPUT || a->getId() == ID_OUTPUT) {
            argtotal++;
        }
    }
    if (argtotal) {
        ret += "(";
    }
    // Generate list of arguments
    for (auto &a : entries_) {
        if (a->getId() == ID_INPUT || a->getId() == ID_OUTPUT) {
            ret += "\n    " + a->generate(SV_ALL);
            if (++argcnt < argtotal) {
                ret += ",";
            }
        }
    }
    if (argtotal) {
        ret += ")";
    }
    ret += ";\n";
    for (auto &e: entries_) {
        if (e->getId() != ID_VALUE) {
            continue;
        }
        ret += "    " + e->getType(SV_ALL) + " " + e->getName() +";\n";
    }

    ret += "\n";
    for (auto &e: entries_) {
        if (e->getId() != ID_OPERATION) {
            continue;
        }
        ret += e->generate(SV_ALL);
    }
    if (getpReturn()) {
        ret += "    return " + getpReturn()->getName() + ";\n";
    }
    ret += "endfunction\n";
    return ret;
}


std::string FunctionObject::generate_vhdl() {
    std::string ret = "";
    return ret;
}
}
