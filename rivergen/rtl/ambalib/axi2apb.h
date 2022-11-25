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
#include "types_amba.h"
#include "axi_slv.h"

using namespace sysvc;

class axi2apb : public ModuleObject {
 public:
    axi2apb(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            vapbmo(this, "vapbmo") {
        }

     public:
        types_amba::apb_in_type vapbmo;
    };

    void proc_comb();

 public:
    // io:
    InPort i_clk;
    InPort i_nrst;
    InStruct<types_amba::mapinfo_type> i_mapinfo;
    OutStruct<types_amba::dev_config_type> o_cfg;
    InStruct<types_amba::axi4_slave_in_type> i_xslvi;
    OutStruct<types_amba::axi4_slave_out_type> o_xslvo;
    InStruct<types_amba::apb_out_type> i_apbmi;
    OutStruct<types_amba::apb_in_type> o_apbmo;

    ParamLogic State_Idle;
    ParamLogic State_setup;
    ParamLogic State_access;
    ParamLogic State_out;

    Signal w_req_valid;
    Signal wb_req_addr;
    Signal w_req_write;
    Signal wb_req_wdata;
    Signal wb_req_wstrb;
    Signal w_req_last;
    Signal w_req_ready;

    RegSignal state;
    RegSignal pvalid;
    RegSignal paddr;
    RegSignal pwdata;
    RegSignal prdata;
    RegSignal pwrite;
    RegSignal pstrb;
    RegSignal pprot;
    RegSignal pselx;
    RegSignal penable;
    RegSignal pslverr;
    RegSignal xsize;

    axi_slv axi0;

    CombProcess comb;
};

class axi2apb_file : public FileObject {
 public:
    axi2apb_file(GenObject *parent) :
        FileObject(parent, "axi2apb"),
        axi2apb_(this, "") {}

 private:
    axi2apb axi2apb_;
};

