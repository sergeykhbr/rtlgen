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
#include "bufg/bufg_folder.h"
#include "mem/mem_folder.h"
#include "pll/pll_folder.h"
#include "cdc_axi_sync/cdc_axi_sync_folder.h"

class techmap_folder : public FolderObject {
 public:
    techmap_folder(GenObject *parent) :
        FolderObject(parent, "techmap"),
        bufg_folder_(this),
        mem_folder_(this),
        pll_folder_(this),
        cdc_axi_sync_folder_(this) {}

    virtual std::string getLibName() override { return getName(); }

 protected:
    bufg_folder bufg_folder_;
    mem_folder mem_folder_;
    pll_folder pll_folder_;
    cdc_axi_sync_folder cdc_axi_sync_folder_;
};
