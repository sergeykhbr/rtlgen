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
#include "files.h"
#include "utils.h"
#include "array.h"

namespace sysvc {

StructObject::StructObject(GenObject *parent,
                           StructObject *type,
                           const char *name,
                           const char *comment)
    : GenObject(parent, type ? ID_STRUCT_INST : ID_STRUCT_DEF, name, comment) {
    type_ = std::string("");
    idx_ = -1;
    if (type) {
        type_ = type->getName();
    }
}

StructObject::StructObject(GenObject *parent,
                           const char *name,
                           int idx,
                           const char *comment)
    : GenObject(parent, ID_ARRAY_ITEM, name, comment) {
    idx_ = idx;
    type_ = parent_->getName();
}


std::string StructObject::getName() {
    std::string ret = GenObject::getName();
    if (idx_ != -1) {
        GenObject *sel = static_cast<ArrayObject *>(getParent())->getSelector();
        if (sel) {
            ret = sel->getName();
        }
    }
    return ret;
}


std::string StructObject::generate(EGenerateType v) {
    std::string ret = "";
    if (getId() == ID_STRUCT_INST || getId() == ID_ARRAY_ITEM) {
        return ret;
    }
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
    std::string ln;

    if (getComment().size()) {
        ret += "    // " + getComment() + "\n";
    }
    ret += "    struct " + getName() + " {\n";
    for (auto &p: entries_) {
        ln = "        " + p->getType(SYSC_ALL) + " " + p->getName();
        if (p->getDepth()) {
            ln += "[" + p->getDepth(SYSC_ALL) + "]";

        }
        ln += ";";
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

std::string StructObject::generate_sysv() {
    std::string ret = "";
    ret += "    struct " + getName() + " {\n";
    for (auto &p: entries_) {
        ret += "        " + p->getType(SV_ALL) + " " + p->getName() + ";\n";
    }
    ret += "    };\n";
    ret += "\n";
    return ret;
}

std::string StructObject::generate_vhdl() {
    std::string ret = "";
    return ret;
}


}
