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
                         const char *depth,
                         const char *comment)
    : GenObject(parent, type, ID_ARRAY_DEF, name, comment), depth_(depth) {
}

std::string ArrayObject::getType() {
    if (type_.size()) {
        return type_;
    }
    return getItem()->getType();
}

std::string ArrayObject::generate() {
    std::string ret = "";
    if (SCV_is_sv()) {
        ret += "typedef " + getType() + " " + getName();
        ret += "[0: " + getStrDepth() + "-1];\n";
    } else if (SCV_is_sysc()) {
        ret += "typedef " + getType() + " " + getName();
        ret += "[" + getStrDepth() + "];\n";
    }
    return ret;
}

}
