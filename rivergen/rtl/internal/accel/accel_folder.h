// 
//  Copyright 2025 Sergey Khabarov, sergeykhbr@gmail.com
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

#include <api_rtlgen.h>
#include "types_accel_bus0.h"
#include "types_accel_bus1.h"
#include "accel_axi2apb_bus1.h"
#include "accel_axictrl_bus0.h"
#include "accel_soc.h"

class accel_folder : public FolderObject {
 public:
    accel_folder(GenObject *parent) :
        FolderObject(parent, "accel"),
        types_accel_bus0_(this),
        types_accel_bus1_(this),
        accel_axictrl_bus0_file_(this),
        accel_axi2apb_bus1_file_(this),
        accel_soc_file_(this) {}

    virtual std::string getLibName() override { return getName(); }

 protected:
    // subfolders:
    // files
    types_accel_bus0 types_accel_bus0_;
    types_accel_bus1 types_accel_bus1_;
    accel_axictrl_bus0_file accel_axictrl_bus0_file_;
    accel_axi2apb_bus1_file accel_axi2apb_bus1_file_;
    accel_soc_file accel_soc_file_;
};
