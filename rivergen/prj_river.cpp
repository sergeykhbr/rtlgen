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

#include "prj_river.h"

RiverProject::RiverProject(const char *rootpath) :
    ProjectObject("river_v3", rootpath, "River v3.0 CPU generator"),
    folder_rtl_(this, "rtl"),
    folder_ambalib_(&folder_rtl_, "ambalib"),
    file_types_amba_(&folder_ambalib_),
    folder_riverlib_(&folder_rtl_, "riverlib"),
    file_river_cfg_(&folder_riverlib_),
    file_river_top_(&folder_riverlib_)
{
}
