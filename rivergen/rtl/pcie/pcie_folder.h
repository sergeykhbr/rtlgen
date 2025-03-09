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
#include "pcie_cfg.h"
#include "pcie_dma.h"
#include "apb_pcie.h"

class pcie_folder : public FolderObject {
 public:
    pcie_folder(GenObject *parent) :
        FolderObject(parent, "pcie"),
        pcie_cfg_(this),
        pcie_dma_file_(this),
        apb_pcie_file_(this) {}

    virtual std::string getLibName() override { return getName(); }

 protected:
    // subfolders:
    // files
    pcie_cfg pcie_cfg_;
    pcie_dma_file pcie_dma_file_;
    apb_pcie_file apb_pcie_file_;
};
