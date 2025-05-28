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
#include "types_gencpu64_bus0.h"
#include "types_gencpu64_bus1.h"
#include "gencpu64_axi2apb_bus1.h"
#include "gencpu64_axictrl_bus0.h"
#include "gencpu64_soc.h"

class gencpu64_folder : public FolderObject {
 public:
    gencpu64_folder(GenObject *parent) :
        FolderObject(parent, "gencpu64"),
        types_gencpu64_bus0_(this),
        types_gencpu64_bus1_(this),
        types_gencpu64_buspnp_(this),
        gencpu64_axictrl_bus0_file_(this),
        gencpu64_axi2apb_bus1_file_(this),
        gencpu64_soc_file_(this) {}

    virtual std::string getLibName() override { return getName(); }

 protected:
    // subfolders:
    // files
    types_gencpu64_bus0 types_gencpu64_bus0_;
    types_gencpu64_bus1 types_gencpu64_bus1_;
    gencpu64_axictrl_bus0_file gencpu64_axictrl_bus0_file_;
    gencpu64_axi2apb_bus1_file gencpu64_axi2apb_bus1_file_;
    gencpu64_soc_file gencpu64_soc_file_;
};
