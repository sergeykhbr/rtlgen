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

#include "enums.h"
#include "utils.h"

namespace sysvc {

EnumObject::EnumObject(GenObject *parent,
                       const char *name)
    : GenObject(parent, ID_ENUM, name) {
}

void EnumObject::add_value(const char *name, const char *comment) {
    char tstr[64];
    int total = static_cast<int>(entries_.size());
    RISCV_sprintf(tstr, sizeof(tstr), "%d", total);
    new I32D(tstr, name, this, comment);

    std::string path = getFullPath();
    std::string file = getFile();
    SCV_set_cfg_parameter(path,
                          file,
                          name,
                          total);
}

std::string EnumObject::generate() {
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

std::string EnumObject::generate_sysc() {
    std::string ret = "";
    std::string ln = "";
    ret += "enum " + getName() + " {\n";
    for (auto &p: entries_) {
        ln = "    " + p->getName() + " = ";
        ln += static_cast<I32D *>(p)->getStrValue();
        if (&p != &entries_.back()) {
            ln += ",";
        }
        if (p->getComment().size()) {
            while (ln.size() < 60) {
                ln += " ";
            }
            ln += "// " + p->getComment();
        }
        ret += ln + "\n";
    }
    ret += "};\n";
    return ret;
}

std::string EnumObject::generate_sysv() {
    std::string ret = "";
    std::string ln = "";
    for (auto &p: entries_) {
        ln = "localparam int " + p->getName() + " = ";
        ln += static_cast<I32D *>(p)->getStrValue() + ";";
        if (p->getComment().size()) {
            while (ln.size() < 60) {
                ln += " ";
            }
            ln += "// " + p->getComment();
        }
        ret += ln + "\n";
    }
    return ret;
}

std::string EnumObject::generate_vhdl() {
    std::string ret = "";
    return ret;
}


}
