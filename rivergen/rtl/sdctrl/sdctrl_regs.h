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
    InPort i_sd_cmd;
    InPort i_sd_dat0;
    InPort i_sd_dat1;
    InPort i_sd_dat2;
    InPort i_sd_dat3;
    OutPort o_sck;
    OutPort o_sck_posedge;
    OutPort o_sck_negedge;
    OutPort o_watchdog;
    OutPort o_clear_cmderr;
    TextLine _cfg0_;
    OutPort o_pcie_12V_support;
    OutPort o_pcie_available;
    OutPort o_voltage_supply;
    OutPort o_check_pattern;
    InPort i_400khz_ena;
    InPort i_sdtype;
    InPort i_sdstate;
    TextLine _cmd0_;
    InPort i_cmd_state;
    InPort i_cmd_err;
    InPort i_cmd_req_valid;
    InPort i_cmd_req_cmd;
    InPort i_cmd_resp_valid;
    InPort i_cmd_resp_cmd;
    InPort i_cmd_resp_reg;
    InPort i_cmd_resp_crc7_rx;
    InPort i_cmd_resp_crc7_calc;
    
    Signal w_req_valid;
    Signal wb_req_addr;
    Signal w_req_write;
    Signal wb_req_wdata;

    RegSignal sclk_ena;
    RegSignal clear_cmderr;
    RegSignal scaler_400khz;
    RegSignal scaler_data;
    RegSignal scaler_cnt;
    RegSignal wdog;
    RegSignal wdog_cnt;
    RegSignal level;
    RegSignal resp_valid;
    RegSignal resp_rdata;
    RegSignal resp_err;
    RegSignal last_req_cmd;
    RegSignal last_resp_cmd;
    RegSignal last_resp_crc7_rx;
    RegSignal last_resp_crc7_calc;
    RegSignal last_resp_reg;
    RegSignal pcie_12V_support;
    RegSignal pcie_available;
    RegSignal voltage_supply;
    RegSignal check_pattern;

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

