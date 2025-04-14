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
    : GenObject(parent, comment) {
    name_ = std::string(name);

    Operation::start(this);
}

std::string ProcObject::getPostAssign() {
    std::string ret = "";
    if (!SCV_is_sv()) {
        return ret;
    }

    if (listPostAssign_.size()) {
        ret += "\n";
    }

    for (auto &p: listPostAssign_) {
        ret += p->generate();
    }
    return ret;
}

}
