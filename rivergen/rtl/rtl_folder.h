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
#include "techmap/techmap_folder.h"
#include "riverlib/riverlib_folder.h"
#include "misclib/misclib_folder.h"
#if GENCFG_SD_CTRL_ENABLE
#include "sdctrl/sdctrl_folder.h"
#endif
#if GENCFG_PCIE_ENABLE
#include "pcie/pcie_folder.h"
#endif
#include "riscv_soc.h"


class rtl_folder : public FolderObject {
  public:
    rtl_folder(GenObject *parent) :
        FolderObject(parent, "rtl"),
        ambalib_folder_(this),
        techmap_folder_(this),
        riverlib_folder_(this),
        misclib_folder_(this),
#if GENCFG_SD_CTRL_ENABLE
        sdctrl_folder_(this),
#endif
#if GENCFG_PCIE_ENABLE
        pcie_folder_(this),
#endif
        riscv_soc_file_(this) {}

 protected:
    // subfolders:
    ambalib_folder ambalib_folder_;
    techmap_folder techmap_folder_;
    riverlib_folder riverlib_folder_;
    misclib_folder misclib_folder_;
#if GENCFG_SD_CTRL_ENABLE
    sdctrl_folder sdctrl_folder_;
#endif
#if GENCFG_PCIE_ENABLE
    pcie_folder pcie_folder_;
#endif
    // files
    riscv_soc_file riscv_soc_file_;
};
