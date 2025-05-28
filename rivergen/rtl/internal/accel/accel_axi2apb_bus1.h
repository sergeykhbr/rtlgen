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
#include "../ambalib/types_amba.h"
#include "../ambalib/types_pnp.h"
#include "../ambalib/axi_slv.h"
#include "types_accel_bus1.h"

using namespace sysvc;

class accel_axi2apb_bus1 : public ModuleObject {
 public:
    accel_axi2apb_bus1(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            iselidx(this, "iselidx", "0", NO_COMMENT),
            vapbi(this, "vapbi", "ADD(CFG_BUS1_PSLV_TOTAL,1)", "apb_in_none", NO_COMMENT),
            vapbo(this, "vapbo", "ADD(CFG_BUS1_PSLV_TOTAL,1)", "apb_out_none", NO_COMMENT) {
        }

     public:
        I32D iselidx;
        ValueArray<StructVar<types_amba::apb_in_type>> vapbi;
        ValueArray<StructVar<types_amba::apb_out_type>> vapbo;
    };

    void proc_comb();

 public:
    // io:
    InPort i_clk;
    InPort i_nrst;
    InStruct<types_amba::mapinfo_type> i_mapinfo;
    OutStruct<types_pnp::dev_config_type> o_cfg;
    InStruct<types_amba::axi4_slave_in_type> i_xslvi;
    OutStruct<types_amba::axi4_slave_out_type> o_xslvo;
    InStruct<types_accel_bus1::bus1_apb_out_vector> i_apbo;
    OutStruct<types_accel_bus1::bus1_apb_in_vector> o_apbi;
    OutStruct<types_accel_bus1::bus1_mapinfo_vector> o_mapinfo;

    ParamLogic State_Idle;
    ParamLogic State_setup;
    ParamLogic State_access;
    ParamLogic State_out;

    Signal w_req_valid;
    Signal wb_req_addr;
    Signal wb_req_size;
    Signal w_req_write;
    Signal wb_req_wdata;
    Signal wb_req_wstrb;
    Signal w_req_last;
    Signal w_req_ready;

    RegSignal state;
    RegSignal selidx;
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
    RegSignal size;

    axi_slv axi0;

    CombProcess comb;
};

class accel_axi2apb_bus1_file : public FileObject {
 public:
    accel_axi2apb_bus1_file(GenObject *parent) :
        FileObject(parent, "accel_axi2apb_bus1"),
        accel_axi2apb_bus1_(this, "accel_axi2apb_bus1") {}

 private:
    accel_axi2apb_bus1 accel_axi2apb_bus1_;
};

