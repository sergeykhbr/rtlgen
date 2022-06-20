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
    : GenObject(parent, ID_SIGNAL, name, comment), wire_(wire) {
}

std::string Signal::getType(EGenerateType v) {
    std::string out = "";
    if (v == SYSC_ALL || v == SYSC_DECLRATION) {
        out += "sc_signal<" + wire_->getType(v) + ">";
    } else if (v == SYSVERILOG_ALL) {
        out += "logic " + wire_->getType(v);
    } else {
    }
    return out;
}

}
