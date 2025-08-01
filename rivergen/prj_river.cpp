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
    ProjectObject(rootpath, "River v3.0 CPU generator"),
    target_folder_(this),
    rtl_folder_(this),
    prj_folder_(this),
    ambalib_tb_folder_(&prj_folder_)
#if GENCFG_PCIE_ENABLE
    ,pcie_dma_tb_folder_(&prj_folder_)  // pull all TBs into /prj/tb
#endif
#if CONFIG_GPU3D
    ,accel_tb_folder_(&prj_folder_)
#endif
    ,misclib_tb_folder_(&prj_folder_)   // pull all TBs into /prj/tb
    ,cdc_tb_folder_(&prj_folder_)       // pull all TBs into /prj/tb
#if GENCFG_HDMI_ENABLE
    ,hdmi_tb_folder_(&prj_folder_)      // pull all TBs into /prj/tb
    ,mathlib_tb_folder_(&prj_folder_)   // pull all TBs into /prj/tb
#endif
{
}
