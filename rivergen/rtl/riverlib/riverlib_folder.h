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
#include "cache/cache_folder.h"
#include "l2cache/l2cache_folder.h"
#include "core/core_folder.h"
#include "dmi/dmi_folder.h"
#include "river_cfg.h"
#include "types_river.h"
#include "river_top.h"
#include "river_amba.h"
#include "dummycpu.h"
#include "ic_axi4_to_l1.h"
#include "workgroup.h"

class riverlib_folder : public FolderObject {
 public:
    riverlib_folder(GenObject *parent) :
        FolderObject(parent, "riverlib"),
        river_cfg_(this),
        types_river_(this),
        cache_(this),
        core_(this),
        l2cache_(this),
        dmi_(this),
        river_top_(this),
        river_amba_(this),
        dummycpu_(this),
        ic_axi4_to_l1_(this),
        workgroup_(this) {}

    virtual std::string getLibName() override { return getName(); }

 protected:
    river_cfg river_cfg_;
    types_river types_river_;
    cache_folder cache_;
    core_folder core_;
    l2cache_folder l2cache_;
    dmi_folder dmi_;
    river_top river_top_;
    river_amba_file river_amba_;
    dummycpu_file dummycpu_;
    ic_axi4_to_l1_file ic_axi4_to_l1_;
    workgroup_file workgroup_;
};
