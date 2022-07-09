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

#include "minstance.h"
#include "modules.h"
#include "files.h"
#include "operations.h"
#include "utils.h"

namespace sysvc {

MInstanceObject::MInstanceObject(GenObject *owner, GenObject *parent, const char *name) :
    GenObject(parent, ID_MINSTANCE, name), owner_(owner) {
}

bool MInstanceObject::isAsyncReset() {
    return static_cast<ModuleObject *>(owner_)->isAsyncReset();
}

void MInstanceObject::connect_param(const char *ioname, GenObject *v) {
    param_[std::string(ioname)] = v;
}

void MInstanceObject::connect_io(const char *ioname, GenObject *v) {
    io_[std::string(ioname)] = v;
}

std::string MInstanceObject::generate(EGenerateType v) {
    std::string ret = "";
    if (v == SYSC_ALL || v == SYSC_CPP || v == SYSC_H) {
        return generate_sysc();
    } else if (v == SV_ALL || v == SV_PKG || v == SV_MOD) {
        return generate_sv();
    } else {
        return generate_vhdl();
    }
    return ret;
}

std::string MInstanceObject::generate_sysc() {
    std::string ret = "";
    std::string ln;
    ret += "    " + getName() + " = new " + getType(SYSC_ALL);
    ret += "(\"" + getName() + "\"";
    ModuleObject *mod = static_cast<ModuleObject *>(owner_);
    std::list<GenObject *> paramlist;
    std::list<GenObject *> iolist;

    // Generic paramater list
    mod->getParamList(paramlist);
    if (mod->isAsyncReset()) {
        ret += ", async_reset";
    }
    for (auto &p : paramlist) {
        ret += ", ";
        if (param_.find(p->getName()) == param_.end()) {
            SHOW_ERROR("param '%s' not connected", p->getName());
        } else {
            ret += p->getName();
        }
    }
    ret += ");\n";

    // IO port assignments
    mod->getIoList(iolist);
    for (auto &io: iolist) {
        ret += "    " + getName() + "->" + io->getName() + "(";
        if (io_.find(io->getName()) == io_.end()) {
            SHOW_ERROR("io '%s' not connected", io->getName());
        } else {
            GenObject *port = io_[io->getName()];
            std::string sname = "";
            sname = Operation::fullname("r", sname, port);
            ret += sname;
        }
        ret += ");\n";
    }
    return ret;
}

std::string MInstanceObject::generate_sv() {
    std::string ret = "";
    ModuleObject *mod = static_cast<ModuleObject *>(owner_);
    std::list<GenObject *> paramlist;
    std::list<GenObject *> iolist;
    int cnt;

    ret += getType(SV_ALL);

    // Generic parameter list
    cnt = 0;
    mod->getParamList(paramlist);
    if (mod->isAsyncReset() || paramlist.size()) {
        ret += " #(\n";
        if (mod->isAsyncReset()) {
            ret += "    .async_reset(async_reset)";
            if (paramlist.size()) {
                ret += ",";
            }
            ret += "\n";
        }
        for (auto &p : paramlist) {
            ret += "    ." + p->getName() + "(";
            if (param_.find(p->getName()) == param_.end()) {
                SHOW_ERROR("param '%s' not connected", p->getName());
            } else {
                ret += p->getName();
            }
            ret += ")";
            if (++cnt < paramlist.size()) {
                ret += ",";
            }
            ret += "\n";
        }
    }
    // Instance name
    ret += ") " + getName() + "\n";
    ret += "(\n";

    // IO port assignments
    cnt = 0;
    mod->getIoList(iolist);
    for (auto &io: iolist) {
        ret += "    ." + io->getName() + "(";
        if (io_.find(io->getName()) == io_.end()) {
            SHOW_ERROR("io '%s' not connected", io->getName());
        } else {
            GenObject *port = io_[io->getName()];
            ret += port->getName();
        }
        ret += ")";
        if (++cnt < iolist.size()) {
            ret += ",";
        }
        ret += "\n";
    }
    ret += ");\n";
    ret += "\n";
    return ret;
}

std::string MInstanceObject::generate_vhdl() {
    std::string ret = "";
    return ret;
}


}
