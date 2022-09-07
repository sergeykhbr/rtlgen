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

namespace sysvc {

ModuleObject::ModuleObject(GenObject *parent, const char *type, const char *name) :
    GenObject(parent, type, name[0] ? ID_MODULE_INST : ID_MODULE, name) {
    if (getId() == ID_MODULE) {
        SCV_register_module(this);
    }
}


bool ModuleObject::isAsyncReset() {
    bool nrst_exist = false;
    for (auto &e: entries_) {
        if (e->getName() == "i_nrst") {
            nrst_exist = true;
            break;
        }
    }
    if (!nrst_exist) {
        return false;
    }
    for (auto &e: entries_) {
        if (e->getId() == ID_MODULE_INST) {
            if (static_cast<ModuleObject *>(e)->isAsyncReset()) {
                return true;
            }
        } else if (e->isReg()) {
            return true;
        }
    }
    return false;
}

bool ModuleObject::isCombProcess() {
    for (auto &e: entries_) {
        if (e->getId() == ID_PROCESS && e->getName() != "registers") {
            return true;
        }
    }
    return false;
}

bool ModuleObject::isRegProcess() {
    for (auto &e: entries_) {
        if (e->isReg() || (e->getId() == ID_PROCESS && e->getName() == "registers")) {
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
        if (p->getId() == ID_MODULE_INST) {
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


void ModuleObject::getTmplParamList(std::list<GenObject *> &genlist) {
    for (auto &e : entries_) {
        if (e->getId() != ID_TMPL_PARAM) {
            continue;
        }
        genlist.push_back(e);
    }
}

void ModuleObject::changeTmplParameter(const char *name, const char *val) {
    std::string tname = std::string(name);
    std::list<GenObject *> genlist;
    getTmplParamList(genlist);
    for (auto &e : genlist) {
        if (e->getName() != tname) {
            continue;
        }
        static_cast<TmplParamI32D *>(e)->changeValue(val);
    }
}


void ModuleObject::getParamList(std::list<GenObject *> &genlist) {
    for (auto &e : entries_) {
        if (e->getId() != ID_DEF_PARAM) {
            continue;
        }
        genlist.push_back(e);
    }
}

void ModuleObject::getIoList(std::list<GenObject *> &iolist) {
    for (auto &e : entries_) {
        if (e->getId() != ID_INPUT && e->getId() != ID_OUTPUT) {
            continue;
        }
        iolist.push_back(e);
    }
}

}
