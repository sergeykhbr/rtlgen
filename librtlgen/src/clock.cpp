// 
//  Copyright 2023 Sergey Khabarov, sergeykhbr@gmail.com
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

#include "clock.h"
#include "operations.h"
#include "utils.h"

namespace sysvc {

Clock::Clock(GenObject *parent,
             const char *name,
             const char *period,
             const char *comment)
    : GenValue(parent, name, period, comment) {
}


std::string Clock::getType() {
    std::string ret = "";

    if (SCV_is_sysc()) {
         ret += "sc_clock";
    } else if (SCV_is_sv()) {
        ret = std::string("logic");
    }
    return ret;
}

std::string Clock::getStrValue() {
    return objValue_->getName();
}

}
