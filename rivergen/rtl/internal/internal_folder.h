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

#include <api_rtlgen.h>
#include <genconfig.h>
#include "ambalib/ambalib_folder.h"
#include "cdc/cdc_folder.h"
#include "mem/mem_folder.h"
#include "riverlib/riverlib_folder.h"
#include "misclib/misclib_folder.h"
#if GENCFG_SD_CTRL_ENABLE
    #include "sdctrl/sdctrl_folder.h"
#endif
#if GENCFG_PCIE_ENABLE
    #include "pcie_dma/pcie_dma_folder.h"
#endif
#if GENCFG_HDMI_ENABLE
    #include "hdmilib/hdmilib_folder.h"
#endif
#if CONFIG_RISCV_VHDL
    #include "gencpu64/gencpu64_folder.h"        // riscv_soc: generic 64-bits CPU
#endif
#if CONFIG_GPU3D
    #include "accel/accel_folder.h"              // gpu3d project: khbr_accel device
#endif
//#include "mincpu32/mincpu32_folder.h"        // minimal 32-bits CPU


class internal_folder : public FolderObject {
  public:
    internal_folder(GenObject *parent) :
        FolderObject(parent, "internal"),
        ambalib_folder_(this),
        cdc_folder_(this),
        mem_folder_(this),
        riverlib_folder_(this),
        misclib_folder_(this)
#if GENCFG_SD_CTRL_ENABLE
        ,sdctrl_folder_(this)
#endif
#if GENCFG_PCIE_ENABLE
        ,pcie_dma_folder_(this)
#endif
#if GENCFG_HDMI_ENABLE
        ,hdmilib_folder_(this)
#endif
        //mincpu32_folder_(this),
#if CONFIG_RISCV_VHDL
        ,gencpu64_folder_(this)
#endif
#if CONFIG_GPU3D
        ,accel_folder_(this)
#endif
    {}

 protected:
    // subfolders:
    ambalib_folder ambalib_folder_;
    cdc_folder cdc_folder_;
    mem_folder mem_folder_;
    riverlib_folder riverlib_folder_;
    misclib_folder misclib_folder_;
#if GENCFG_SD_CTRL_ENABLE
    sdctrl_folder sdctrl_folder_;
#endif
#if GENCFG_PCIE_ENABLE
    pcie_dma_folder pcie_dma_folder_;
#endif
#if GENCFG_HDMI_ENABLE
    hdmilib_folder hdmilib_folder_;
#endif
    // files
#if CONFIG_RISCV_VHDL
    gencpu64_folder gencpu64_folder_;
#endif
#if CONFIG_GPU3D
    accel_folder accel_folder_;
#endif
};
