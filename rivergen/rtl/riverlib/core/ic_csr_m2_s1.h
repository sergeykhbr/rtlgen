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
    ic_csr_m2_s1(GenObject *parent, const char *name, river_cfg *cfg);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) : ProcObject(parent, "comb") {
            Operation::start(this);
            ic_csr_m2_s1 *p = static_cast<ic_csr_m2_s1 *>(parent);
            p->proc_comb();
        }
    };

    void proc_comb();

 public:
    InPort i_clk;
    InPort i_nrst;
    TextLine _Master0_;
    InPort i_m0_req_valid;
    OutPort o_m0_req_ready;
    InPort i_m0_req_type;
    InPort i_m0_req_addr;
    InPort i_m0_req_data;
    OutPort o_m0_resp_valid;
    InPort i_m0_resp_ready;
    OutPort o_m0_resp_data;
    OutPort o_m0_resp_exception;
    TextLine _Master1_;
    InPort i_m1_req_valid;
    OutPort o_m1_req_ready;
    InPort i_m1_req_type;
    InPort i_m1_req_addr;
    InPort i_m1_req_data;
    OutPort o_m1_resp_valid;
    InPort i_m1_resp_ready;
    OutPort o_m1_resp_data;
    OutPort o_m1_resp_exception;
    TextLine _Slave0_;
    OutPort o_s0_req_valid;
    InPort i_s0_req_ready;
    OutPort o_s0_req_type;
    OutPort o_s0_req_addr;
    OutPort o_s0_req_data;
    InPort i_s0_resp_valid;
    OutPort o_s0_resp_ready;
    InPort i_s0_resp_data;
    InPort i_s0_resp_exception;

 protected:
    RegSignal midx;
    RegSignal acquired;

    // process should be intialized last to make all signals available
    CombProcess comb;
};

class ic_csr_m2_s1_file : public FileObject {
 public:
    ic_csr_m2_s1_file(GenObject *parent, river_cfg *cfg) :
        FileObject(parent, "ic_csr_m2_s1"),
        ic_(this, "", cfg) {}

 private:
    ic_csr_m2_s1 ic_;
};

