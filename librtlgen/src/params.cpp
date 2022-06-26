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


Param::Param(GenObject *parent,
                         const char *name,
                         GenValue *value,
                         const char *comment)
    : GenObject(parent, ID_PARAM, name, comment),
    value_(value) {

    std::string path = getFullPath();
    SCV_set_cfg_parameter(path,
                          getFile(),
                          getName().c_str(),
                          value_->getValue());
}

std::string Param::generate(EGenerateType v) {
    std::string ret = "";

    if (v == SYSC_ALL || v == SYSC_H || v == SYSC_CPP) {
        ret += generate_sysc();
    } else if (v == SV_ALL || v == SV_PKG || v == SV_MOD) {
        ret += generate_sysv();
    } else {
        ret += generate_vhdl();
    }

    return ret;
}

std::string Param::generate_sysc() {
    std::string ret = "";

    ret += "static const ";
    ret += value_->getType(SYSC_ALL) + " ";
    ret += getName() + " = ";
    ret += value_->getValue(SYSC_ALL) + ";";

    // One line comment
    if (getComment().size()) {
        while (ret.size() < 60) {
            ret += " ";
        }
        ret += "// " + getComment();
    }
    ret += "\n";
    return ret;
}

std::string Param::generate_sysv() {
    std::string ret = "";

    ret += "localparam ";
    ret += value_->getType(SV_ALL) + " ";
    ret += getName() + " = ";
    ret += value_->getValue(SV_ALL) + ";";

    // One line comment
    if (getComment().size()) {
        while (ret.size() < 60) {
            ret += " ";
        }
        ret += "// " + getComment();
    }
    ret += "\n";
    return ret;
}

std::string Param::generate_vhdl() {
    std::string ret = "";
    return ret;
}

}
