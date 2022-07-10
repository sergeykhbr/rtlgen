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
                           const char *type,
                           const char *name,
                           int idx,
                           const char *comment)
    : GenObject(parent,
                idx != -1 ? ID_STRUCT_INST 
                          : name[0] ? ID_STRUCT_INST : ID_STRUCT_DEF, name, comment) {
    type_ = std::string(type);
    idx_ = idx;
    if (idx != -1) {
        char tstr[256];
        RISCV_sprintf(tstr, sizeof(tstr), "%d", idx);
        name_ = std::string(tstr);
    }
}

std::string StructObject::getName() {
    std::string ret = GenObject::getName();
    if (idx_ == -1) {
        return ret;
    }
    GenObject *sel = static_cast<ArrayObject *>(getParent())->getSelector();
    if (!sel) {
        return ret;
    }
    if (sel->getId() == ID_CONST) {
        ret = sel->getStrValue();
    } else if (sel->getId() == ID_OPERATION) {
        ret = sel->generate();
    } else {
        ret = sel->getName();
    }
    return ret;
}


std::string StructObject::generate() {
    std::string ret = "";
    if (getId() == ID_STRUCT_INST) {
        return ret;
    }
    if (SCV_is_sysc()) {
        ret += generate_sysc();
    } else if (SCV_is_sv()) {
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
    ret += "    struct " + getType() + " {\n";
    for (auto &p: entries_) {
        ln = "        " + p->getType() + " " + p->getName();
        if (p->getDepth()) {
            ln += "[" + p->getStrDepth() + "]";

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
    ret += "    struct " + getType() + " {\n";
    for (auto &p: entries_) {
        ret += "        " + p->getType() + " " + p->getName() + ";\n";
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
