// 
//  Copyright 2025 Sergey Khabarov, sergeykhbr@gmail.com
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
#include <genconfig.h>
#include "../../../sim/pll/pll_generic.h"
#include "../types_accel_bus0.h"
#include "../accel_axictrl_bus0.h"
#include "../../ambalib/apb_slv.h"

using namespace sysvc;

class accel_axictrl_bus0_tb : public ModuleObject {
 public:
    accel_axictrl_bus0_tb(GenObject *parent, const char *name);

    virtual bool isTop() override { return true; }
    virtual bool isAsyncResetParam() override { return false; }

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            vb_m0_xmsto(this, "vb_m0_xmsto", NO_COMMENT),
            vb_m1_xmsto(this, "vb_m1_xmsto", NO_COMMENT),
            vb_bar(this, "vb_bar", "48", "'0", NO_COMMENT),
            vb_m0_w_burst_cnt_next(this, "vb_m0_w_burst_cnt_next", "4", "'0", NO_COMMENT),
            vb_test_cnt_inv(this, "vb_test_cnt_inv", "32", "'0") {
        }

     public:
        types_amba::axi4_master_out_type vb_m0_xmsto;
        types_amba::axi4_master_out_type vb_m1_xmsto;
        Logic vb_bar;
        Logic vb_m0_w_burst_cnt_next;
        Logic vb_test_cnt_inv;
    };


    class TestProcess : public ProcObject {
     public:
        TestProcess(GenObject *parent, GenObject *clk) :
            ProcObject(parent, "test", clk, CLK_POSEDGE, 0, ACTIVE_NONE, NO_COMMENT) {
        }
     public:
    };

    void comb_proc();
    void test_proc();

public:
    // Ports:
    Signal nrst;
    Signal clk;
    SignalStruct<types_pnp::dev_config_type> wb_bus0_cfg;
    SignalStruct<types_pnp::dev_config_type> wb_xslv0_cfg;
    types_accel_bus0::bus0_xmst_out_vector vec_i_xmsto;
    types_accel_bus0::bus0_xmst_in_vector vec_o_xmsti;
    types_accel_bus0::bus0_xslv_out_vector vec_i_xslvo;
    types_accel_bus0::bus0_xslv_in_vector vec_o_xslvi;
    types_accel_bus0::bus0_mapinfo_vector vec_o_mapinfo;
    Signal w_req_valid;
    Signal wb_req_addr;
    Signal wb_req_size;
    Signal w_req_write;
    Signal wb_req_wdata;
    Signal wb_req_wstrb;
    Signal w_req_last;
    Signal w_req_ready;
    Signal w_resp_valid;
    Signal wb_resp_rdata;
    Signal w_resp_err;
    SignalStruct<types_amba::axi4_master_in_type> wb_m0_xmsti;
    SignalStruct<types_amba::axi4_master_in_type> wb_m1_xmsti;
    STRING msg;

    // registers
    RegSignal clk_cnt;
    RegSignal err_cnt;
    RegSignal test_cnt;
    RegSignal test_pause_cnt;
    RegSignal m0_state;
    RegSignal m0_xsize;
    RegSignal m0_aw_valid;
    RegSignal m0_aw_addr;
    RegSignal m0_aw_xlen;
    RegSignal m0_w_wait_states;
    RegSignal m0_w_wait_cnt;
    RegSignal m0_w_valid;
    RegSignal m0_w_data;
    RegSignal m0_w_strb;
    RegSignal m0_w_last;
    RegSignal m0_w_burst_cnt;
    RegSignal m0_b_wait_states;
    RegSignal m0_b_wait_cnt;
    RegSignal m0_b_ready;
    RegSignal m0_ar_valid;
    RegSignal m0_ar_addr;
    RegSignal m0_ar_xlen;
    RegSignal m0_r_wait_states;
    RegSignal m0_r_wait_cnt;
    RegSignal m0_r_ready;
    RegSignal m0_r_burst_cnt;
    RegSignal m0_compare_ena;
    RegSignal m0_compare_a;
    RegSignal m0_compare_b;
    RegSignal m1_state;
    RegSignal end_of_test;
    RegSignal end_idle;
    RegSignal slvstate;
    RegSignal req_ready;
    RegSignal resp_valid;
    RegSignal resp_rdata;
    RegSignal resp_wait_states;
    RegSignal resp_wait_cnt;

    LogicArray mem;

    // Sub-module instances:
    pll_generic clk0;
    accel_axictrl_bus0 bus0;
    axi_slv xslv0;

    CombProcess comb;
    TestProcess test;
};

class accel_axictrl_bus0_tb_file : public FileObject {
 public:
    accel_axictrl_bus0_tb_file(GenObject *parent) :
        FileObject(parent, "accel_axictrl_bus0_tb"),
        accel_axictrl_bus0_tb_(this, "accel_axictrl_bus0_tb") { }

 private:
    accel_axictrl_bus0_tb accel_axictrl_bus0_tb_;
};

