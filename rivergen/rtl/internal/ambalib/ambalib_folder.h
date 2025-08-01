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
#include "types_amba.h"
#include "types_pnp.h"
#include "axi_slv.h"
#include "apb_slv.h"
#include "axi_dma.h"

class ambalib_folder : public FolderObject {
 public:
    ambalib_folder(GenObject *parent) :
        FolderObject(parent, "ambalib"),
        types_amba_(this),
        types_pnp_(this),
        axi_slv_file_(this),
        apb_slv_file_(this),
        axi_dma_file_(this) {}

    virtual std::string getLibName() override { return getName(); }

 protected:
    // subfolders:
    // files
    types_amba types_amba_;
    types_pnp types_pnp_;
    axi_slv_file axi_slv_file_;
    apb_slv_file apb_slv_file_;
    axi_dma_file axi_dma_file_;
};
