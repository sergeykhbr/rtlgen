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
#if CONFIG_RISCV_VHDL
    #include "prj/impl/asic_gencpu64/target_gencpu64_cfg.h"
    #define TARGET_IMPL_FOLDER "asic_gencpu64"
#endif
#if CONFIG_GPU3D
    #include "prj/impl/asic_accel/target_accel_cfg.h"
    #define TARGET_IMPL_FOLDER "asic_accel"
#endif
#include "prj/prj_folder.h"
#include "rtl/rtl_folder.h"
#include "rtl/internal/ambalib/tb/ambalib_tb_folder.h"
#include "rtl/internal/pcie_dma/tb/pcie_dma_tb_folder.h"
#include "rtl/internal/misclib/tb/misclib_tb_folder.h"
#include "rtl/internal/cdc/tb/cdc_tb_folder.h"
#include "rtl/internal/hdmilib/tb/hdmi_tb_folder.h"
#include "rtl/internal/mathlib/tb/mathlib_tb_folder.h"
#include "rtl/internal/accel/tb/accel_tb_folder.h"

using namespace sysvc;

class RiverProject : public ProjectObject {
 public:
    RiverProject(const char *rootpath);

 protected:
     /**
        Create folder "prj/impl/asic" before generating target_cfg
    */
    class target_folder : public FolderObject {
      public:
        target_folder(GenObject *parent)
            : FolderObject(parent, "prj"), impl_folder_(this) {}
      protected:
         class impl_folder : public FolderObject {
          public:
            impl_folder(GenObject *parent) : FolderObject(parent, "impl"), asic_folder_(this) {}
          protected:
            class asic_folder : public FolderObject {
              public:
                asic_folder(GenObject *parent) : FolderObject(parent, TARGET_IMPL_FOLDER), target_cfg_(this) {}
              protected:
#if CONFIG_RISCV_VHDL
                target_gencpu64_cfg target_cfg_;
#endif
#if CONFIG_GPU3D
                target_accel_cfg target_cfg_;
#endif
            } asic_folder_;
        } impl_folder_;
    };

    /**
        Create Target configuration files before RTL
    */
    target_folder target_folder_;
    rtl_folder rtl_folder_;
    prj_folder prj_folder_;
    // Test-benches must be created after all project VIPs created:
    ambalib_tb_folder ambalib_tb_folder_;
#if GENCFG_PCIE_ENABLE
    pcie_dma_tb_folder pcie_dma_tb_folder_;     // put into prj/tb
#endif
#if CONFIG_GPU3D
    accel_tb_folder accel_tb_folder_;
#endif
    misclib_tb_folder misclib_tb_folder_;       // put into prj/tb
    cdc_tb_folder cdc_tb_folder_;               // put into prj/tb
    hdmi_tb_folder hdmi_tb_folder_;             // put into prj/tb
    mathlib_tb_folder mathlib_tb_folder_;
};


