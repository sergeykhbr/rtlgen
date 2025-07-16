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
#include "io/io_folder.h"
#include "pll/pll_folder.h"
#include "mem/mem_folder.h"
#include "ddr3_phy/ddr3_phy_folder.h"

class sim_folder : public FolderObject {
 public:
    sim_folder(GenObject *parent) :
        FolderObject(parent, "sim"),
        io_folder_(this),
        pll_folder_(this),
        mem_folder_(this),
        ddr3_phy_folder_(this) {}

 protected:
    io_folder io_folder_;
    pll_folder pll_folder_;
    mem_folder mem_folder_;
    ddr3_phy_folder ddr3_phy_folder_;
};
