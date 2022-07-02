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
                         GenObject *type,
                         const char *name,
                         const char *depth,
                         const char *comment)
    : GenObject(parent, type ? ID_ARRAY_INST : ID_ARRAY_DEF, name, comment), depth_(depth) {
    type_ = std::string("");
    if (type) {
        type_ = type->getName();

        arr_ = new GenObject *[depth_.getValue()];
        for (int i = 0; i < static_cast<int>(depth_.getValue()); i++) {
            arr_[i] = new ArrayItem(this, type, i);
            //for (auto &e : type->getEntries()) {
            //    entries_.push_back(e);
            //}
        }
    }
}

std::string ArrayItem::getName() {
    char tstr[64];
    RISCV_sprintf(tstr, sizeof(tstr), "%d", idx_);
    std::string ret = std::string(tstr);
    return ret;
}

std::string ArrayObject::generate(EGenerateType v) {
    std::string ret = "";
    if (getId() == ID_ARRAY_DEF && v == VHDL_ALL) {
        ret += generate_vhdl();
    } else {
        return ret;
    }
    return ret;
}

std::string ArrayObject::generate_sysc() {
    std::string ret = "";
    return ret;
}

std::string ArrayObject::generate_sysv() {
    std::string ret = "";
    return ret;
}

std::string ArrayObject::generate_vhdl() {
    std::string ret = "";
    return ret;
}


}
