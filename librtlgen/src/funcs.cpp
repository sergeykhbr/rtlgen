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
    int tcnt = 0;
    if (SCV_is_sysc_h()) {
        // module header
        if (getParent()->isModule()) {
            std::list<GenObject *> argslist;
            ret += addspaces() + getType() + " " + getName() + "(";
            argslist.clear();
            getArgsList(argslist);
            for (auto &io: argslist) {
                if (tcnt++) {
                    ret += ", ";
                }
                ret += io->getType() + " " + io->getName();
            }
            ret += ");\n";
        } else {
            ret += addspaces() + "static " + getType() + " ";
            ret += generate_sysc();
        }
    } else if (SCV_is_sysc()) {
        ret += generate_sysc();
    } else if (SCV_is_sv()) {
        if (getParent()->isFile()) {
            ret += addspaces() + "function automatic ";
        }
        ret += generate_sysv();
    } else {
        ret += "function ";
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
    pushspaces();

    // process variables declaration
    if (getType() == "std::string" || getType() == "string") {
        if (!isString()) {
            bool st = true;
        }
    }

    //if (isString()) {  // doesnot work
    if (getType() == "std::string" || getType() == "string") {
        // return value is string
        ret += addspaces() + "char tstr[256];\n";
    }
    tcnt = 0;
    bool skiparg;
    for (auto &e: getEntries()) {
        if (!e->isValue() && !e->isStruct()) { // no need to check typedef inside of function
            continue;
        }

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

        if (e->isConst()) {
            SHOW_ERROR("constant defined inside of function: %s", getName());
        }

        ret += addspaces() + e->getType() + " " + e->getName();
        if (e->getObjDepth()) {
            ret += "[";
            ret += e->getStrDepth();
            ret += "]";
        }
        tcnt++;
        ret += ";\n";
    }
    if (tcnt) {
        ret += "\n";
        tcnt = 0;
    }

    // Generate operations:
    for (auto &e: getEntries()) {
        if (e->isOperation()) {
            ret += e->generate();
        }
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
    pushspaces();

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
            ret += addspaces() + e->getType() + " " + e->getName();
            tcnt++;
        } else if (e->getObjDepth()) {
            ret += addspaces() + e->getType() + " " + e->getName();
            ret += "[";
            ret += e->getStrDepth();
            ret += "]";
        } else if (e->isStruct()) { // no need to check typedef inside function
            ret += addspaces() + e->getType() + " " + e->getName();
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
    for (auto &e: getEntries()) {
        if (e->isOperation()) {
            ret += e->generate();
        }
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
