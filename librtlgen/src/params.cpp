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

ParamGeneric::ParamGeneric(GenValue *parent) {

    std::string path = parent->getFullPath();
    std::string file = parent->getFile();
    SCV_set_cfg_parameter(path,
                          file,
                          parent->getName().c_str(),
                          parent->getValue());
}

std::string ParamGeneric::genparam(EGenerateType v, GenValue *p) {
    std::string ret = "";

    if (v == SYSC_ALL || v == SYSC_H || v == SYSC_CPP) {
        ret += "static const ";
    } else if (v == SV_ALL || v == SV_PKG || v == SV_MOD) {
        ret += "localparam ";
        v = SV_ALL;     // no need to print package_name::param_name
    } else {
        ret += "const ";
    }
    ret += p->getType(v) + " ";
    ret += p->getName() + " = ";
    ret += p->getValue(v) + ";";

    // One line comment
    if (p->getComment().size()) {
        while (ret.size() < 60) {
            ret += " ";
        }
        ret += "// " + p->getComment();
    }
    ret += "\n";
    return ret;
}

}
