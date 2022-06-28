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

#include "signals.h"

namespace sysvc {

Signal::Signal(GenObject *parent,
               const char *name,
               Logic *wire,
               const char *comment)
    : GenValueWrapper(parent, ID_SIGNAL, name, wire, comment) {
}

std::string Signal::getType(EGenerateType v) {
    std::string out = "";
    if (v == SYSC_ALL || v == SYSC_H || v == SYSC_CPP) {
        out += "sc_signal<" + value_->getType(v) + ">";
    } else if (v == SV_ALL || v == SV_PKG || v == SV_MOD) {
        out += value_->getType(v);
    } else {
    }
    return out;
}

}
