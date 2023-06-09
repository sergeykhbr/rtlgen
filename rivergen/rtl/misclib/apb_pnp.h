// 
//  Copyright 2023 Sergey Khabarov, sergeykhbr@gmail.com
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

class apb_pnp : public ModuleObject {
 public:
    apb_pnp(GenObject *parent, const char *name);

 protected:
    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            cfgmap(this, "cfgmap", "32", "MUL(8,cfg_slots)"),
            vrdata(this, "vrdata", "CFG_SYSBUS_DATA_BITS") {
        }

     public:
        WireArray<Logic> cfgmap;
        Logic vrdata;
    };

    void proc_comb();

 public:
    TmplParamI32D cfg_slots;
    DefParamLogic hw_id;
    DefParamI32D cpu_max;
    DefParamI32D l2cache_ena;
    DefParamI32D plic_irq_max;
    // io:
    InPort i_clk;
    InPort i_nrst;
    InStruct<types_amba::mapinfo_type> i_mapinfo;
    InStruct<types_pnp::soc_pnp_vector> i_cfg;
    OutStruct<types_amba::dev_config_type> o_cfg;
    InStruct<types_amba::apb_in_type> i_apbi;
    OutStruct<types_amba::apb_out_type> o_apbo;
    OutPort o_irq;

 protected:

    Signal w_req_valid;
    Signal wb_req_addr;
    Signal w_req_write;
    Signal wb_req_wdata;

    RegSignal fw_id;
    RegSignal idt_l;
    RegSignal idt_m;
    RegSignal malloc_addr_l;
    RegSignal malloc_addr_m;
    RegSignal malloc_size_l;
    RegSignal malloc_size_m;
    RegSignal fwdbg1;
    RegSignal fwdbg2;
    RegSignal fwdbg3;
    RegSignal fwdbg4;
    RegSignal fwdbg5;
    RegSignal fwdbg6;
    RegSignal irq;
    RegSignal resp_valid;
    RegSignal resp_rdata;
    RegSignal resp_err;

    CombProcess comb;

    apb_slv pslv0;
};

class apb_pnp_file : public FileObject {
 public:
    apb_pnp_file(GenObject *parent) :
        FileObject(parent, "apb_pnp"),
        apb_pnp_(this, "") {}

 private:
    apb_pnp apb_pnp_;
};

