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

#pragma once

#include <api.h>
#include "rtl/ambalib/types_amba.h"
#include "rtl/riverlib/river_cfg.h"
#include "rtl/riverlib/river_top.h"

using namespace sysvc;

class RiverProject : public ProjectObject {
 public:
    RiverProject(const char *rootpath);

 protected:
    FolderObject folder_rtl_;
    FolderObject folder_ambalib_;
    types_amba file_types_amba_;
    FolderObject folder_riverlib_;
    river_cfg file_river_cfg_;
    river_top file_river_top_;
};


