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

#include "modules.h"
#include "defparams.h"
#include "ports.h"
#include "signals.h"
#include "utils.h"
#include "files.h"
#include "operations.h"
#include <string.h>

namespace sysvc {

ModuleObject::ModuleObject(GenObject *parent,
                           const char *type,
                           const char *name,
                           const char *comment)
    : GenObject(parent, type, ID_MODULE, name, comment) {

    SCV_add_module(this);
}

GenObject *ModuleObject::getAsyncResetParam() {
    for (auto &p: getEntries()) {
        if (p->isParam()) {
            if (p->getName() == "async_reset") {
                return p;
            }
        }
    }
    return 0;
}

GenObject *ModuleObject::getResetPort() {
    for (auto &p: getEntries()) {
        if (p->isInput()) {
            if (p->getName() == "i_nrst") {
                return p;
            }
        }
    }
    return 0;
}

GenObject *ModuleObject::getClockPort() {
    for (auto &p: getEntries()) {
        if (p->isInput()) {
            if (p->getName() == "i_clk") {
                return p;
            }
        }
    }
    return 0;
}


/*GenObject *ModuleObject::getAsyncReset() {
    GenObject *rst_port = getResetPort();
    if (!rst_port) {
        return rst_port;
    }
    for (auto &e: entries_) {
        if (e->isModule()) {
            if (e->getAsyncReset()) {
                return rst_port;
            }
        } else if (e->isReg() || e->isNReg()) {
            return rst_port;
        }
    }
    rst_port = 0;    // no registers to reset in this module
    return rst_port;
}*/


std::string ModuleObject::generate() {
    std::string ret = "";
    if (SCV_is_sysc_h()) {
        return generate_sysc_h();
    } else if (SCV_is_sysc()) {
        return generate_sysc_cpp();
    }
    return ret;
}

bool ModuleObject::isCombProcess() {
    for (auto &e: entries_) {
        if (e->getId() == ID_PROCESS
            && e->getName() != "registers"
            && e->getName() != "nregisters") {
            return true;
        }
    }
    return false;
}

bool ModuleObject::isRegs() {
    for (auto &e: entries_) {
        if (e->isReg()) {
            return true;
        }
    }
    return false;
}

bool ModuleObject::isRegProcess() {
    for (auto &e: entries_) {
        if (e->isReg()
            || (e->getId() == ID_PROCESS && e->getName() == "registers")) {
            return true;
        }
    }
    return false;
}

bool ModuleObject::isNRegs() {
    for (auto &e: entries_) {
        if (e->isNReg()) {
            return true;
        }
    }
    return false;
}

bool ModuleObject::isNRegProcess() {
    for (auto &e: entries_) {
        if (e->isNReg()
            || (e->getId() == ID_PROCESS && e->getName() == "nregisters")) {
            return true;
        }
    }
    return false;
}

bool ModuleObject::is2DimReg() {
    for (auto &p: entries_) {
        if (!p->isReg()) {
            continue;
        }
        if (p->getDepth()) {
            // two-dimensional array is presence in a register list
            return true;
        }
    }
    return false;
}

bool ModuleObject::isSubModules() {
    for (auto &p: entries_) {
        if (p->isModule()) {
            return true;
        }
    }
    return false;
}

bool ModuleObject::isFileValue() {
    for (auto &p: entries_) {
        if (p->getId() == ID_FILEVALUE) {
            return true;
        }
    }
    return false;
}

bool ModuleObject::isSignalEntries(GenObject *obj) {
    if (obj->isSignal()) {
        return true;
    }
    for (auto &p: obj->getEntries()) {
        if (p->isSignal()) {
            return true;
        }
        if (p->isStruct()) {
            if (isSignalEntries(p)) {
                return true;
            }
        }
    }
    return false;
}

void ModuleObject::getTmplParamList(std::list<GenObject *> &genlist) {
    for (auto &e : entries_) {
        if (e->isParamTemplate()) {
            genlist.push_back(e);
        }
    }
}

void ModuleObject::getParamList(std::list<GenObject *> &genlist) {
    for (auto &e : entries_) {
        if (e->isParamGeneric() && !e->isParamTemplate()) {
            genlist.push_back(e);
        }
    }
}

void ModuleObject::getIoList(std::list<GenObject *> &iolist) {
    for (auto &e : entries_) {
        if (e->isInput() || e->isOutput()) {
            iolist.push_back(e);
        }
    }
}

std::string ModuleObject::generate_all_proc_nullify(GenObject *obj,
                                                    std::string prefix,
                                                    std::string i) {
    std::string ret = "";
    if (((obj->isValue() && !obj->isConst())
        || (obj->isStruct() && !obj->isTypedef())) == 0) {
        return ret;
    }

    if (prefix.size()) {
        prefix += ".";
    }
    prefix += obj->getName();

    if (obj->getObjDepth()) {
        prefix += "[" + i + "]";
        ret += addspaces();
        ret += "for (int " + i + " = 0; " + i + " < " + obj->getStrDepth() + "; " + i + "++)";
        if (SCV_is_sysc()) {
            ret += " {";
        } else if (SCV_is_sv()) {
            ret += " begin";
        } else if (SCV_is_vhdl()) {
            ret += " begin";
        }
        ret += "\n";
        pushspaces();

        const char tidx[2] = {static_cast<char>(static_cast<int>(i.c_str()[0]) + 1), 0};
        i = std::string(tidx);
    }

    if (obj->isStruct()
        && ((SCV_is_sysc() && obj->getStrValue().c_str()[0] == '{')
            || (SCV_is_sv() && obj->getStrValue().c_str()[0] == '\''))) {
        for (auto &p : obj->getEntries()) {
            ret += generate_all_proc_nullify(p, prefix, i);
        }
    } else {
        ret += addspaces() + prefix + " = " + obj->getStrValue() + ";\n";
    }

    if (obj->getObjDepth()) {
        popspaces();
        ret += addspaces();
        if (SCV_is_sysc()) {
            ret += "}";
        } else if (SCV_is_sv()) {
            ret += "end";
        } else if (SCV_is_vhdl()) {
            ret += "end";
        }
        ret += "\n";
    }
    return ret;
}


}
