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
#include "../ambalib/apb_slv.h"

using namespace sysvc;

class apb_prci : public ModuleObject {
 public:
    apb_prci(GenObject *parent, const char *name, const char *comment);

    virtual GenObject *getResetPort() override { return &i_pwrreset; }
    virtual bool getResetActive() override { return true; }

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
    InPort i_pwrreset;
    InPort i_dmireset;
    InPort i_sys_locked;
    InPort i_ddr_locked;
    OutPort o_sys_rst;
    OutPort o_sys_nrst;
    OutPort o_dbg_nrst;
    InStruct<types_amba::mapinfo_type> i_mapinfo;
    OutStruct<types_pnp::dev_config_type> o_cfg;
    InStruct<types_amba::apb_in_type> i_apbi;
    OutStruct<types_amba::apb_out_type> o_apbo;

    Signal w_req_valid;
    Signal wb_req_addr;
    Signal w_req_write;
    Signal wb_req_wdata;

    RegSignal sys_rst;
    RegSignal sys_nrst;
    RegSignal dbg_nrst;
    RegSignal sys_locked;
    RegSignal ddr_locked;
    RegSignal resp_valid;
    RegSignal resp_rdata;
    RegSignal resp_err;

    CombProcess comb;

    apb_slv pslv0;
};

class apb_prci_file : public FileObject {
 public:
    apb_prci_file(GenObject *parent) :
        FileObject(parent, "apb_prci"),
        apb_prci_(this, "apb_prci", NO_COMMENT) {}

 private:
    apb_prci apb_prci_;
};

