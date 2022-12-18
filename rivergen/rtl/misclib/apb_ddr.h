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
#include "../ambalib/apb_slv.h"

using namespace sysvc;

class apb_ddr : public ModuleObject {
 public:
    apb_ddr(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            vb_rdata(this, "vb_rdata", "32") {
        }

     public:
        Logic vb_rdata;
    };

    void proc_comb();

 public:
    // io:
    InPort i_clk;
    InPort i_nrst;
    InStruct<types_amba::mapinfo_type> i_mapinfo;
    OutStruct<types_amba::dev_config_type> o_cfg;
    InStruct<types_amba::apb_in_type> i_apbi;
    OutStruct<types_amba::apb_out_type> o_apbo;
    InPort i_pll_locked;
    InPort i_init_calib_done;
    InPort i_device_temp;
    InPort i_sr_active;
    InPort i_ref_ack;
    InPort i_zq_ack;

    Signal w_req_valid;
    Signal wb_req_addr;
    Signal w_req_write;
    Signal wb_req_wdata;

    RegSignal pll_locked;
    RegSignal init_calib_done;
    RegSignal device_temp;
    RegSignal sr_active;
    RegSignal ref_ack;
    RegSignal zq_ack;
    RegSignal resp_valid;
    RegSignal resp_rdata;
    RegSignal resp_err;

    CombProcess comb;

    apb_slv pslv0;
};

class apb_ddr_file : public FileObject {
 public:
    apb_ddr_file(GenObject *parent) :
        FileObject(parent, "apb_ddr"),
        apb_ddr_(this, "") {}

 private:
    apb_ddr apb_ddr_;
};

