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

#include "modules.h"
#include "utils.h"

namespace sysvc {

ModuleObject::ModuleObject(GenObject *parent, const char *name) :
    GenObject(parent, ID_MODULE, name) {
}


std::string ModuleObject::generate(EGenerateType v) {
    std::string out = "";
    if (v == SYSC_DECLRATION) {
        out += generate_sysc_h();
    }
    return out;
}

std::string ModuleObject::generate_sysc_h() {
    std::string out = "";

    out +=
        "SC_MODULE(" + getName() + ") {\n";

    // Input/Output signal declaration
    GenObject *io = getEntryById(ID_IO_START);
    for (auto &p: entries_) {
        out += p->generate(SYSC_DECLRATION);
    }
    
    out += 
        "};\n"
        "\n";

    return out;
}

}
