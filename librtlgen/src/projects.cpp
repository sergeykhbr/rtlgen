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
                         const char *rootpath,
                         const char *comment)
    : GenObject(0, ID_PROJECT, name, comment) {
    rootpath_ = std::string(rootpath);
}


std::string ProjectObject::generate() {
    if (!SCV_is_dir_exists(rootpath_.c_str())) {
        SCV_create_dir(rootpath_.c_str());
    }

    for (auto &p: entries_) {
        p->generate();
    }

    return GenObject::generate();
}

}  // namespace sysvc

