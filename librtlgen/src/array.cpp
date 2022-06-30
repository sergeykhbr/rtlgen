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

#include "array.h"
#include "utils.h"

namespace sysvc {

ArrayObject::ArrayObject(GenObject *parent,
                         const char *type,
                         const char *name,
                         int size,
                         const char *comment)
    : GenObject(parent, ID_ARRAY, name, comment), size_(size) {
    type_ = std::string(type);
}

std::string ArrayObject::generate(EGenerateType v) {
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

std::string ArrayObject::generate_sysc() {
    std::string ret = "";
    std::string ln;
    // sub-structure
    for (auto &p: entries_) {
        if (p->getId() != ID_STRUCT_DEF) {
            continue;
        }
        ret += p->generate(SYSC_ALL);
    }

    // FIXME: sub structures created as a child of current struct (it is wrong)
    if (getComment().size()) {
        ret += "    // " + getComment() + "\n";
    }
    ret += "    struct " + getType(SYSC_ALL) + " {\n";
    for (auto &p: entries_) {
        if (p->getId() == ID_STRUCT_DEF) {
            // FIXME: remove this condition
            continue;
        }
        ln = "        " + p->getType(SYSC_ALL) + " " + p->getName() + ";";
        if (p->getComment().size()) {
            while (ln.size() < 60) {
                ln += " ";
            }
            ln += "// " + p->getComment();
        }
        ret += ln + "\n";
    }
    ret += "    };\n";
    ret += "\n";
    return ret;
}

std::string ArrayObject::generate_sysv() {
    std::string ret = "";
    ret += "    struct " + getType(SYSC_ALL) + " {\n";
    for (auto &p: entries_) {
        ret += "        " + p->getType(SV_ALL) + " " + p->getName() + ";\n";
    }
    ret += "    };\n";
    ret += "\n";
    return ret;
}

std::string ArrayObject::generate_vhdl() {
    std::string ret = "";
    return ret;
}


}
