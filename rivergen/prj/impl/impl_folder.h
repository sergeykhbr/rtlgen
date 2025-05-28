// 
//  Copyright 2023 Sergey Khabarov, sergeykhbr@gmail.com
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
#include <genconfig.h>
#if CONFIG_RISCV_VHDL
    #include "asic_gencpu64/asic_gencpu64_folder.h"
    #include "asic_gencpu64/asic_gencpu64_sim_folder.h"
#endif
#if CONFIG_GPU3D
    #include "asic_accel/asic_accel_folder.h"
    #include "asic_accel_sim/asic_accel_sim_folder.h"
#endif


class impl_folder : public FolderObject {
  public:
    impl_folder(GenObject *parent) :
        FolderObject(parent, "impl"),
        asic_accel_folder_(this),
        asic_accel_sim_folder_(this) {}

 protected:
    // subfolders:
    asic_accel_folder asic_accel_folder_;
    asic_accel_sim_folder asic_accel_sim_folder_;
    // files
};
