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

using namespace sysvc;

class ic_csr_m2_s1 : public ModuleObject {
 public:
    ic_csr_m2_s1(GenObject *parent);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent)
            : ProcObject(parent, "comb") {}
     protected:
    };


 protected:
    InPort i_clk;
    InPort i_nrst;
    TextLine _Master0_;
    InPort i_m0_req_valid;
    OutPort o_m0_req_ready;

    CombProcess comb;

    Reg midx;
    Reg acquired;
};

class ic_csr_m2_s1_file : public FileObject {
 public:
    ic_csr_m2_s1_file(GenObject *parent);

 private:
    ic_csr_m2_s1 ic_;
};

