// 
//  Copyright 2025 Sergey Khabarov, sergeykhbr@gmail.com
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

#include "refobj.h"

namespace sysvc {

std::string RefObject::nameInModule(EPorts portid, bool no_sc_read) {
    std::string ret;
    GenObject *saveParent = ref_->getParent();
    ref_->setParent(getParent());
    ret = ref_->nameInModule(portid, no_sc_read);
    ref_->setParent(saveParent);
    return ret;
}
std::string RefResetObject::nameInModule(EPorts portid, bool no_sc_read) {
    std::string ret;
    GenObject *saveParent = ref_->getParent();
    ref_->setParent(getParent());
    ret = ref_->nameInModule(portid, NO_SC_READ);
    ref_->setParent(saveParent);
    return ret;
}

}  // namespace sysvc
