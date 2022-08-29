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
#include "../river_cfg.h"
#include "../../ambalib/types_amba.h"

using namespace sysvc;

class L2SerDes : public ModuleObject {
 public:
    L2SerDes(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb") {
        }

     public:
    };

    void proc_comb();

 public:
    // io:
    InPort i_clk;
    InPort i_nrst;
    OutStruct<river_cfg::axi4_l2_in_type> o_l2i;
    InStruct<river_cfg::axi4_l2_out_type> i_l2o;
    InStruct<types_amba::axi4_master_in_type> i_msti;
    OutStruct<types_amba::axi4_master_out_type> o_msto;

    CombProcess comb;
};

class l2serdes_file : public FileObject {
 public:
    l2serdes_file(GenObject *parent) :
        FileObject(parent, "l2serdes"),
        l2serdes_(this, "") {}

 private:
    L2SerDes l2serdes_;
};

