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

#include "proc.h"
#include "operations.h"

namespace sysvc {

ProcObject::ProcObject(GenObject *parent,
                       const char *name,
                       const char *comment)
    : GenObject(parent, ID_PROCESS, name, comment) {
    Operation::start(this);
}

std::string ProcObject::generate(EGenerateType v) {
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

std::string ProcObject::generate_sysc() {
    std::string ret = "";
    for (auto &e: entries_) {
        if (e->getId() != ID_OPERATION) {
            continue;
        }
        ret += e->generate(SYSC_CPP) + "\n";
    }
    return ret;
}


std::string ProcObject::generate_sysv() {
    std::string ret = "";
    return ret;
}


std::string ProcObject::generate_vhdl() {
    std::string ret = "";
    return ret;
}

}
