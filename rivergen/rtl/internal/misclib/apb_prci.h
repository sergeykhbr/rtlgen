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

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            vb_rdata(this, "vb_rdata", "32", "'0", NO_COMMENT) {
        }

     public:
        Logic vb_rdata;
    };

    void proc_comb();
    void proc_reqff();

 public:
    // io:
    InPort i_clk;
    InPort i_pwrreset;
    InPort i_dmireset;
    InPort i_sys_locked;
    InPort i_ddr_locked;
    InPort i_pcie_phy_rst;
    InPort i_pcie_phy_clk;
    InPort i_pcie_phy_lnk_up;
    OutPort o_sys_rst;
    OutPort o_sys_nrst;
    OutPort o_dbg_nrst;
    OutPort o_pcie_nrst;
    OutPort o_hdmi_nrst;
    InStruct<types_amba::mapinfo_type> i_mapinfo;
    OutStruct<types_pnp::dev_config_type> o_cfg;
    InStruct<types_amba::apb_in_type> i_apbi;
    OutStruct<types_amba::apb_out_type> o_apbo;

    Signal w_req_valid;
    Signal wb_req_addr;
    Signal w_req_write;
    Signal wb_req_wdata;

    Logic r_sys_rst;
    Signal r_sys_nrst;
    Signal r_dbg_nrst;
    Signal rb_pcie_nrst;
    Signal rb_hdmi_nrst;
    Signal r_sys_locked;
    Signal rb_ddr_locked;
    Signal rb_pcie_lnk_up;
    RegSignal resp_valid;
    RegSignal resp_rdata;
    RegSignal resp_err;

    CombProcess comb;
    ProcObject reqff;

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

