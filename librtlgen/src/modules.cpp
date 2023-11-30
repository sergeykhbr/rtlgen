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
                           const char *comment) :
    GenObject(parent, type, ID_MODULE, name, comment) {
    SCV_set_local_module(this);
}

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

}
