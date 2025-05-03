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

#include "projects.h"
#include "utils.h"

namespace sysvc {

ProjectObject::ProjectObject(const char *name,
                         const char *comment)
    : GenObject(0, comment) {
    name_ = std::string(name);
}

void ProjectObject::configureGenerator(ECfgGenType cfg) {
    if (cfg == CFG_GEN_SYSC) {
        SCV_set_generator(SYSC_ALL);
    } else if (cfg == CFG_GEN_SV) {
        SCV_set_generator(SV_ALL);
    } else if (cfg == CFG_GEN_VHDL) {
        SCV_set_generator(VHDL_ALL);
    }
    GenObject::configureGenerator(cfg);
}


std::string ProjectObject::generate() {
    if (!SCV_is_dir_exists(name_.c_str())) {
        SCV_create_dir(name_.c_str());
    }

    for (auto &p: entries_) {
        p->generate();
    }

    return GenObject::generate();
}

}  // namespace sysvc

