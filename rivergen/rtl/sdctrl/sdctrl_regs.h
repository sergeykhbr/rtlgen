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
#include "../ambalib/types_amba.h"
#include "../ambalib/types_pnp.h"
#include "../ambalib/apb_slv.h"

using namespace sysvc;

class sdctrl_regs : public ModuleObject {
 public:
    sdctrl_regs(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            v_posedge(this, "v_posedge", "1"),
            v_negedge(this, "v_negedge", "1"),
            vb_rdata(this, "vb_rdata", "32") {
        }

     public:
        Logic v_posedge;
        Logic v_negedge;
        Logic vb_rdata;
    };

    void proc_comb();

 public:
    // io:
    InPort i_clk;
    InPort i_nrst;
    InStruct<types_amba::mapinfo_type> i_pmapinfo;
    OutStruct<types_pnp::dev_config_type> o_pcfg;
    InStruct<types_amba::apb_in_type> i_apbi;
    OutStruct<types_amba::apb_out_type> o_apbo;
    OutPort o_sck;
    OutPort o_sck_posedge;
    OutPort o_sck_negedge;
    
    Signal w_req_valid;
    Signal wb_req_addr;
    Signal w_req_write;
    Signal wb_req_wdata;

    RegSignal scaler;
    RegSignal scaler_cnt;
    RegSignal wdog;
    RegSignal wdog_cnt;
    RegSignal level;

    RegSignal resp_valid;
    RegSignal resp_rdata;
    RegSignal resp_err;

    CombProcess comb;

    apb_slv pslv0;
};

class sdctrl_regs_file : public FileObject {
 public:
    sdctrl_regs_file(GenObject *parent) :
        FileObject(parent, "sdctrl_regs"),
        sdctrl_regs_(this, "") {}

 private:
    sdctrl_regs sdctrl_regs_;
};

