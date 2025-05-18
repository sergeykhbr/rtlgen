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
#include "prj/prj_folder.h"
#include "rtl/rtl_folder.h"
#include "prj/impl/asic/target_cfg.h"   // top level must be instantiated after rtl
#include "rtl/internal/pcie_dma/tb/pcie_dma_tb_folder.h"

using namespace sysvc;

class RiverProject : public ProjectObject {
 public:
    RiverProject(const char *rootpath);

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
                asic_folder(GenObject *parent) : FolderObject(parent, "asic"), target_cfg_(this) {}
              protected:
                target_cfg target_cfg_;
            } asic_folder_;
        } impl_folder_;
    };

 protected:
    target_folder target_folder_;
    rtl_folder rtl_;
    prj_folder prj_;
    // Test-benches must be created after all project VIPs created:
    pcie_dma_tb_folder pcie_dma_tb_folder_;     // put into prj/tb
};


