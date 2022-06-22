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

#include "structs.h"
#include "utils.h"

namespace sysvc {

StructObject::StructObject(GenObject *parent,
                           const char *name)
    : GenObject(parent, ID_STRUCT, name) {
}

std::string StructObject::generate(EGenerateType v) {
    std::string ret = "";
    if (v == SYSC_ALL || v == SYSC_H || v == SYSC_CPP) {
        ret += generate_sysc();
    } else if (v == SV_ALL || v == SV_PKG || v == SV_MOD) {
        ret += generate_sysv();
    } else {
        ret += generate_vhdl();
    }
    return ret;
}

std::string StructObject::generate_sysc() {
    std::string ret = "";
    ret += "struct " + getName() + " {\n";
    for (auto &p: entries_) {
        ret += "    " + p->getName() + " = ";
        ret += static_cast<I32D *>(p)->generate(SYSC_ALL);
        if (&p != &entries_.back()) {
            ret += ",";
        }
        ret += "\n";
    }
    ret += "};\n";
    return ret;
}

std::string StructObject::generate_sysv() {
    std::string ret = "";
    for (auto &p: entries_) {
        ret += "     " + p->getType(SV_ALL) + " = ";
        ret += static_cast<I32D *>(p)->generate(SV_ALL);
        ret += ";\n";
    }
    return ret;
}

std::string StructObject::generate_vhdl() {
    std::string ret = "";
    return ret;
}


}
