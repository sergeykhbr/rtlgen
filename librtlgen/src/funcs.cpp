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
    : GenObject(parent, "", ID_FUNCTION, name, comment) {
    Operation::start(this);
}

std::string FunctionObject::generate() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret += generate_sysc();
    } else if (SCV_is_sv()) {
        ret += generate_sysv();
    } else {
        ret += generate_vhdl();
    }
    return ret;
}

std::string FunctionObject::generate_sysc() {
    std::string ret = "";
    std::list<GenObject *> argslist;
    int tcnt = 0;
    
    getArgsList(argslist);

    ret += getName() + "(";
    if (argslist.size() == 1) {
        for (auto &e: argslist) {
            ret += e->getType() + " " + e->getName();
        }
    } else if (argslist.size() > 1) {
        pushspaces();
        pushspaces();
        ret += "\n" + addspaces();
        for (auto &e: argslist) {
            ret += e->getType() + " " + e->getName();
            if (e != argslist.back()) {
                ret += ",\n" + addspaces();
            }
        }
        popspaces();
        popspaces();
    }
    ret += ") {\n";
    
    // process variables declaration
    if (getType() == "std::string" || getType() == "string") {
        // return value is string
        ret += "    char tstr[256];\n";
    }
    tcnt = 0;
    bool skiparg;
    for (auto &e: getEntries()) {
        skiparg = false;
        for (auto &arg: argslist) {
            if (e->getName() == arg->getName()) {
                skiparg = true;
                break;
            }
        }
        if (skiparg) {
            continue;
        }

        if (e->getId() == ID_VALUE) {
            ret += "    " + e->getType() + " " + e->getName();
            tcnt++;
        } else if (e->getId() == ID_ARRAY_DEF) {
            ret += "    " + e->getType() + " " + e->getName();
            ret += "[";
            ret += e->getStrDepth();
            ret += "]";
        } else if (e->getId() == ID_STRUCT_INST) {
            ret += "    " + e->getType() + " " + e->getName();
        } else {
            continue;
        }
        tcnt++;
        ret += ";\n";
    }
    if (tcnt) {
        ret += "\n";
        tcnt = 0;
    }

    // Generate operations:
    pushspaces();
    for (auto &e: getEntries()) {
        if (e->getId() != ID_OPERATION) {
            continue;
        }
        ret += e->generate();
    }

    // return value
    if (getpReturn()) {
        ret += addspaces() + "return " + getpReturn()->getName() + ";\n";
    }
    popspaces();

    ret += addspaces() + "}\n";
    return ret;
}


std::string FunctionObject::generate_sysv() {
    std::string ret = "";
    std::list<GenObject *> argslist;
    int tcnt = 0;
    
    getArgsList(argslist);

    ret += getType() + " " + getName() + "(";
    if (argslist.size() == 1) {
        for (auto &e: argslist) {
            ret += "input " + e->getType() + " " + e->getName();
        }
    } else if (argslist.size() > 1) {
        pushspaces();
        pushspaces();
        ret += "\n" + addspaces();
        for (auto &e: argslist) {
            ret += "input " + e->getType() + " " + e->getName();
            if (e != argslist.back()) {
                ret += ",\n" + addspaces();
            }
        }
        popspaces();
        popspaces();
    }
    ret += ");\n";
    
    // process variables declaration
    if (getpReturn()) {
        ret += getpReturn()->getType() + " "
            + getpReturn()->getName() + ";\n";
    }
    ret += "begin\n";
    tcnt = 0;
    bool skiparg;
    for (auto &e: getEntries()) {
        skiparg = false;
        for (auto &arg: argslist) {
            if (e->getName() == arg->getName()) {
                skiparg = true;
                break;
            }
        }
        if (e->getName() == getpReturn()->getName()) {
            skiparg = true;
        }
        if (skiparg) {
            continue;
        }

        if (e->getId() == ID_VALUE) {
            ret += "    " + e->getType() + " " + e->getName();
            tcnt++;
        } else if (e->getId() == ID_ARRAY_DEF) {
            ret += "    " + e->getType() + " " + e->getName();
            ret += "[";
            ret += e->getStrDepth();
            ret += "]";
        } else if (e->getId() == ID_STRUCT_INST) {
            ret += "    " + e->getType() + " " + e->getName();
        } else {
            continue;
        }
        tcnt++;
        ret += ";\n";
    }
    if (tcnt) {
        ret += "\n";
        tcnt = 0;
    }

    // Generate operations:
    pushspaces();
    for (auto &e: getEntries()) {
        if (e->getId() != ID_OPERATION) {
            continue;
        }
        ret += e->generate();
    }

    // return value
    if (getpReturn()) {
        ret += addspaces() + "return " + getpReturn()->getName() + ";\n";
    }
    popspaces();

    ret += addspaces() + "end\n";
    ret += addspaces() + "endfunction: " + getName() + "\n";
    ret += "\n";
    return ret;
}


std::string FunctionObject::generate_vhdl() {
    std::string ret = "";
    return ret;
}
}
