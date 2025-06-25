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
#include "../types_amba.h"
#include "../types_pnp.h"

using namespace sysvc;

class axi_mst_generator : public ModuleObject {
 public:
    axi_mst_generator(GenObject *parent, const char *name);

    virtual bool isAsyncResetParam() override { return false; }

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            vb_xmsto(this, "vb_xmsto", NO_COMMENT),
            vb_bar(this, "vb_bar", "48", "'0", NO_COMMENT),
            vb_w_burst_cnt_next(this, "vb_w_burst_cnt_next", "4", "'0", NO_COMMENT),
            vb_run_cnt_inv(this, "vb_run_cnt_inv", "32", "'0"),
            v_writing(this, "v_writing", "1", RSTVAL_ZERO, NO_COMMENT),
            v_reading(this, "v_reading", "1", RSTVAL_ZERO, NO_COMMENT) {
        }

     public:
        types_amba::axi4_master_out_type vb_xmsto;
        Logic vb_bar;
        Logic vb_w_burst_cnt_next;
        Logic vb_run_cnt_inv;
        Logic v_writing;
        Logic v_reading;
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
    DefParamLogic req_bar;
    DefParamLogic unique_id;
    DefParamBOOL read_only;
    // Ports:
    InPort i_nrst;
    InPort i_clk;
    InStruct<types_amba::axi4_master_in_type> i_xmst;
    OutStruct<types_amba::axi4_master_out_type> o_xmst;
    InPort i_start_test;
    InPort i_test_selector;
    InPort i_show_result;
    OutPort o_writing;
    OutPort o_reading;
    STRING msg;

    // registers
    RegSignal err_cnt;
    RegSignal compare_cnt;
    RegSignal run_cnt;
    RegSignal state;
    RegSignal xsize;
    RegSignal aw_valid;
    RegSignal aw_addr;
    RegSignal aw_xlen;
    RegSignal w_use_axi_light;
    RegSignal w_wait_states;
    RegSignal w_wait_cnt;
    RegSignal w_valid;
    RegSignal w_data;
    RegSignal w_strb;
    RegSignal w_last;
    RegSignal w_burst_cnt;
    RegSignal b_wait_states;
    RegSignal b_wait_cnt;
    RegSignal b_ready;
    RegSignal ar_valid;
    RegSignal ar_addr;
    RegSignal ar_xlen;
    RegSignal r_wait_states;
    RegSignal r_wait_cnt;
    RegSignal r_ready;
    RegSignal r_burst_cnt;
    RegSignal compare_ena;
    RegSignal compare_a;
    RegSignal compare_b;

    // Sub-module instances:
    CombProcess comb;
    TestProcess test;
};

class axi_mst_generator_file : public FileObject {
 public:
    axi_mst_generator_file(GenObject *parent) :
        FileObject(parent, "axi_mst_generator"),
        axi_mst_generator_(this, "axi_mst_generator") { }

 private:
    axi_mst_generator axi_mst_generator_;
};

