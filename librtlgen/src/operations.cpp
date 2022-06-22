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

#include "operations.h"

namespace sysvc {

Operation::Operation(GenObject *parent,
                     GenObject *a,
                     GenObject *b,
                     const char *comment)
    : GenObject(parent, ID_OPERATION, "", comment), a_(a), b_(b) {
}

std::string ZEROS::generate(EGenerateType v) {
    std::string ret = "";
    ret += a_->getName();
    if (v == SYSC_ALL || v == SYSC_H || v == SYSC_CPP) {
        ret += " = 0;";
    } else if (v == SV_ALL || v == SV_PKG || v == SV_MOD) {
        ret += " = '0;";
    } else  {
        ret += " = (others => '0');";
    }
    return ret;
}


std::string EQ::generate(EGenerateType v) {
    std::string ret = "";
    ret += a_->getName() + " = " + b_->generate(v) + ";";
    return ret;
}

std::string SETBIT::generate(EGenerateType v) {
    std::string ret = "";
    ret += a_->getName();
    if (v == SYSC_ALL || v == SYSC_H || v == SYSC_CPP) {
         ret += "[" + b_->generate(v) + "] = 1;";
    } else if (v == SV_ALL || v == SV_PKG || v == SV_MOD) {
        ret += "[" + b_->generate(v) + "] = 1'b1;";
    } else {
        ret += "(" + b_->generate(v) + ") := '1';";
    }
    return ret;
}

}
