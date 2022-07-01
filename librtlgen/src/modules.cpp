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
#include "minstance.h"
#include "files.h"
#include "operations.h"

namespace sysvc {

ModuleObject::ModuleObject(GenObject *parent, const char *name) :
    GenObject(parent, ID_MODULE, name) {
    SCV_register_module(this);
    Operation::start(this);     // set operation parent to this module
}

std::string ModuleObject::getType(EGenerateType v) {
    std::string out = "";
    if (v == SYSC_ALL) {
        out += "SC_MODULE";
    } else if (v == SV_ALL) {
        out += "module";
    } else {
    }
    return out;
}

MInstanceObject *ModuleObject::createInstance(GenObject *parent, const char *name) {
    MInstanceObject *inst = new MInstanceObject(this, parent, name);
    instances_.push_back(inst);

    GenObject *p = parent;
    while (p) {
        if (p->getId() != ID_FILE) {
            p = p->getParent();
            continue;
        }
        // to build include dependencies
        std::string fulpath = getFullPath();
        static_cast<FileObject *>(p)->notifyAccess(fulpath);
        break;
    }
    return inst;
}

bool ModuleObject::isAsyncReset() {
    for (auto &e: entries_) {
        if (e->getId() == ID_MINSTANCE) {
            if (static_cast<MInstanceObject *>(e)->isAsyncReset()) {
                return true;
            }
        } else if (e->getId() == ID_REG) {
            return true;
        }
    }
    return false;
}

bool ModuleObject::isRegProcess() {
    for (auto &e: entries_) {
        if (e->getId() == ID_REG) {
            return true;
        }
    }
    return false;
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



std::string ModuleObject::generate(EGenerateType v) {
    std::string out = "";
    Operation::set_space(0);
    if (v == SYSC_H) {
        out += generate_sysc_h();
    } else if (v == SYSC_CPP) {
        out += generate_sysc_cpp();
    } else if (v == SV_PKG) {
        out += generate_sv_pkg();
    } else if (v == SV_MOD) {
        out += generate_sv_mod();
    }
    return out;
}

}
