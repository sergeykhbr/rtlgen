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

#include "params.h"
#include "utils.h"

namespace sysvc {


ParamObject::ParamObject(GenObject *parent,
                         const char *name,
                         GenValue *value,
                         const char *comment)
    : GenObject(parent, ID_PARAM, name, comment), value_(value) {

    SCV_set_cfg_parameter(getName(), value_->getValue());
}

std::string ParamBOOL::generate(EGenerateType v) {
    std::string ret = "";
    if (v == SYSC_DECLRATION) {
        ret += "static const bool " + getName();
        ret += " = " + value_->generate_sysc() + ";";

        // One line comment
        if (getComment().size()) {
            ret += "    // " + getComment();
        }
        ret += "\n";
    }

    return ret;
}


std::string ParamI32::generate(EGenerateType v) {
    std::string ret = "static const int " + getName();
    ret += " = " + value_->generate_sysc() + ";";

    // One line comment
    if (getComment().size()) {
        ret += "    // " + getComment();
    }
    ret += "\n";

    return ret;
}

std::string ParamUI64::generate(EGenerateType v) {
    std::string ret = "static const uint64_t " + getName();
    ret += " = " + value_->generate_sysc() + ";";

    // One line comment
    if (getComment().size()) {
        ret += "    // " + getComment();
    }
    ret += "\n";

    return ret;
}

}
