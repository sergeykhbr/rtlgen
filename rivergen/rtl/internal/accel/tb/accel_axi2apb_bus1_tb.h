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
#include "../types_accel_bus1.h"
#include "../accel_axi2apb_bus1.h"
#include "../../ambalib/apb_slv.h"

using namespace sysvc;

class accel_axi2apb_bus1_tb : public ModuleObject {
 public:
    accel_axi2apb_bus1_tb(GenObject *parent, const char *name);

    virtual bool isTop() override { return true; }
    virtual bool isAsyncResetParam() override { return false; }

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            vb_xslv_mapinfo(this, "vb_xslv_mapinfo", NO_COMMENT),
            vb_pslv_mapinfo(this, "vb_pslv_mapinfo", NO_COMMENT),
            vb_xslvi(this, "vb_xslvi", NO_COMMENT),
            vb_bar(this, "vb_bar", "48", "'0", NO_COMMENT) {
        }

     public:
        types_amba::mapinfo_type vb_xslv_mapinfo;
        types_amba::mapinfo_type vb_pslv_mapinfo;
        types_amba::axi4_slave_in_type vb_xslvi;
        Logic vb_bar;
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
    SignalStruct<types_amba::mapinfo_type> wb_xslv_mapinfo;
    SignalStruct<types_amba::mapinfo_type> wb_pslv_mapinfo;
    types_accel_bus1::bus1_mapinfo_vector   vec_o_mapinfo;
    SignalStruct<types_pnp::dev_config_type> wb_xslv_cfg;
    SignalStruct<types_pnp::dev_config_type> wb_pslv_cfg;
    SignalStruct<types_amba::axi4_slave_in_type> wb_i_xslvi;
    SignalStruct<types_amba::axi4_slave_out_type> wb_o_xslvo;
    types_accel_bus1::bus1_apb_in_vector   vec_apbi;
    types_accel_bus1::bus1_apb_out_vector  vec_apbo;
    Signal w_req_valid;
    Signal wb_req_addr;
    Signal w_req_write;
    Signal wb_req_wdata;
    Signal w_resp_valid;
    Signal wb_resp_rdata;
    Signal w_resp_err;
    STRING msg;

    // registers
    RegSignal clk_cnt;
    RegSignal err_cnt;
    RegSignal test_cnt;
    RegSignal test_state;
    RegSignal apb_wait_states;
    RegSignal test_pause_cnt;
    RegSignal xsize;
    RegSignal aw_valid;
    RegSignal aw_addr;
    RegSignal w_wait_states;
    RegSignal w_wait_cnt;
    RegSignal w_valid;
    RegSignal w_data;
    RegSignal w_strb;
    RegSignal b_wait_states;
    RegSignal b_wait_cnt;
    RegSignal b_ready;
    RegSignal ar_valid;
    RegSignal ar_addr;
    RegSignal r_wait_states;
    RegSignal r_wait_cnt;
    RegSignal r_ready;
    RegSignal compare_ena;
    RegSignal compare_a;
    RegSignal compare_b;
    RegSignal end_of_test;
    RegSignal apbstate;
    RegSignal apbram;
    RegSignal preq_ready;
    RegSignal presp_valid;
    RegSignal presp_rdata;
    RegSignal presp_delay;

    // Sub-module instances:
    pll_generic clk0;
    accel_axi2apb_bus1 bus1;
    apb_slv pslv0;

    CombProcess comb;
    TestProcess test;
};

class accel_axi2apb_bus1_tb_file : public FileObject {
 public:
    accel_axi2apb_bus1_tb_file(GenObject *parent) :
        FileObject(parent, "accel_axi2apb_bus1_tb"),
        accel_axi2apb_bus1_tb_(this, "accel_axi2apb_bus1_tb") { }

 private:
    accel_axi2apb_bus1_tb accel_axi2apb_bus1_tb_;
};

