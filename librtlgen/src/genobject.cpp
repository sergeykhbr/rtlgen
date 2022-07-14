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

#include "genobjects.h"

namespace sysvc {

GenObject::GenObject(GenObject *parent, EIdType id,
                     const char *name, const char *comment) {
    id_ = id;
    parent_ = parent;
    value_ = 0;
    width_ = 0;
    reg_ = false;
    reset_disabled_ = false;
    vcd_enabled_ = true;
    name_ = std::string(name);
    comment_ = std::string(comment);
    if (parent_) {
        parent_->add_entry(this);
    }
}

std::string GenObject::getFullPath() {
    std::string ret = "";
    if (parent_) {
        ret = parent_->getFullPath();
    }
    return ret;
}

std::string GenObject::getFile() {
    std::string ret = "";
    if (getId() == ID_FILE) {
        return getName();
    } else if (parent_) {
        return parent_->getFile();
    }
    return ret;
}

void GenObject::add_entry(GenObject *p) {
    entries_.push_back(p);
}

}
