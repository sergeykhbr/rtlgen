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

#include "ports.h"

namespace sysvc {

InPort::InPort(GenObject *parent, const char *name, const char *width,
    const char *comment) : Logic(width, name, "", parent, comment) {
    id_ = ID_INPUT;
}

InPort::InPort(GenObject *parent, const char *name, GenValue *width, 
    const char *comment) : Logic(width, name, "", parent, comment) {
    id_ = ID_INPUT;
}


OutPort::OutPort(GenObject *parent, const char *name, const char *width,
    const char *comment) : Logic(width, name, "", parent, comment) {
    id_ = ID_OUTPUT;
}

OutPort::OutPort(GenObject *parent, const char *name, GenValue *width, 
    const char *comment) : Logic(width, name, "", parent, comment) {
    id_ = ID_OUTPUT;
}


std::string InPort::getType(EGenerateType v) {
    std::string out = "";
    if (v == SYSC_ALL || v == SYSC_H || v == SYSC_CPP) {
        out += "sc_in<" + Logic::getType(v) + ">";
    } else if (v == SV_ALL || v == SV_PKG || v == SV_MOD) {
        out += "input " + Logic::getType(v);
    } else {
    }
    return out;
}

std::string OutPort::getType(EGenerateType v) {
    std::string out = "";
    if (v == SYSC_ALL || v == SYSC_H || v == SYSC_CPP) {
        out += "sc_out<" + Logic::getType(v) + ">";
    } else if (v == SV_ALL || v == SV_PKG || v == SV_MOD) {
        out += "output " + Logic::getType(v);
    } else {
    }
    return out;
}

}
