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
                         const char *name,
                         const char *depth,
                         const char *comment)
    : GenObject(parent, ID_ARRAY_DEF, name, comment), depth_(depth) {
    sel_ = 0;
}

std::string ArrayObject::getType(EGenerateType v) {
    std::string ret = "";
    if (getEntries().size()) {
        return (*getEntries().begin())->getType(v);
    }
    return ret;
}

}
