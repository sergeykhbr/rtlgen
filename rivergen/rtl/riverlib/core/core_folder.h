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

#include <api.h>
#include "proc.h"
#include "bp_predec.h"
#include "bp_btb.h"
#include "bp.h"
#include "ic_csr_m2_s1.h"
#include "regibank.h"

class core_folder : public FolderObject {
 public:
    core_folder(GenObject *parent, river_cfg *cfg) :
        FolderObject(parent, "core"),
        bp_predec_(this, cfg),
        bp_btb_(this, cfg),
        bp_(this, cfg),
        ic_(this, cfg),
        regint_(this, cfg),
        proc_(this, cfg) {}

 protected:
    // subfolders:
    // files
    bp_predec_file bp_predec_;
    bp_btb_file bp_btb_;
    bp_file bp_;
    ic_csr_m2_s1_file ic_;
    regibank_file regint_;
    proc_file proc_;
};
