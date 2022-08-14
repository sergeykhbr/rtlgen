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
#include "operations.h"

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
    std::string ln;
    if (getId() == ID_STRUCT_INST) {
        return ret;
    }

    if (getComment().size()) {
        ret += "    // " + getComment() + "\n";
    }
    ret += Operation::addspaces();
    if (SCV_is_sysc()) {
        ret += "struct " + getType() + " {\n";
    } else {
        ret += "typedef struct {\n";
    }
    Operation::set_space(Operation::get_space() + 1);
    for (auto &p: entries_) {
        ln = Operation::addspaces();
        ln += p->getType() + " " + p->getName();
        if (p->getDepth()) {
            ln += "[";
            if (SCV_is_sysc()) {
                ln += p->getStrDepth();
            } else {
                ln += "0: " + p->getStrDepth() + " - 1";
            }
            ln += "]";
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
    Operation::set_space(Operation::get_space() - 1);
    ret += Operation::addspaces();
    if (SCV_is_sysc()) {
        ret += "};\n";
    } else  {
        ret += "} " + getType() +";\n";
    }
    ret += "\n";
    return ret;
}

}
