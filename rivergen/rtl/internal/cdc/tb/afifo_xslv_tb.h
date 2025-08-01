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

#include <api_rtlgen.h>
#include <genconfig.h>
#include "../afifo_xslv.h"
#include "../../../sim/pll/pll_generic.h"
#include "../../ambalib/axi_slv.h"

using namespace sysvc;

class afifo_xslv_tb : public ModuleObject {
 public:
    afifo_xslv_tb(GenObject *parent, const char *name);

    virtual bool isTop() override { return true; }
    virtual bool isAsyncResetParam() override { return false; }

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            vb_mapinfo(this, "vb_mapinfo", NO_COMMENT),
            vb_xslvi(this, "vb_xslvi", NO_COMMENT),
            vb_xmsti(this, "vb_xmsti", NO_COMMENT) {
        }

     public:
        types_amba::mapinfo_type vb_mapinfo;
        types_amba::axi4_slave_in_type vb_xslvi;
        types_amba::axi4_master_in_type vb_xmsti;
    };

    // Clock1
    class TestClk1Process : public ProcObject {
     public:
        TestClk1Process(GenObject *parent, GenObject *clk) :
            ProcObject(parent, "test_clk1", clk, CLK_POSEDGE, 0, ACTIVE_NONE, NO_COMMENT),
            vb_xslvi(this, "vb_xslvi", NO_COMMENT) {
        }
     public:
        types_amba::axi4_slave_in_type vb_xslvi;
    };

    // Clock2
    class TestClk2Process : public ProcObject {
     public:
        TestClk2Process(GenObject *parent, GenObject *clk) :
            ProcObject(parent, "test_clk2", clk, CLK_POSEDGE, 0, ACTIVE_NONE, NO_COMMENT) {
        }
     public:
    };

    void proc_comb();
    void proc_test_clk1();
    void proc_test_clk2();

public:
    // Ports:
    Signal i_nrst;
    Signal w_clk1;
    Signal w_clk2;
    Logic wb_clk1_cnt;
    Logic wb_clk2_cnt;
    SignalStruct<types_amba::axi4_slave_in_type> wb_clk1_xslvi;
    SignalStruct<types_amba::axi4_slave_out_type> wb_clk1_xslvo;
    SignalStruct<types_amba::axi4_slave_in_type> wb_clk2_xmsto;
    SignalStruct<types_amba::axi4_slave_out_type> wb_clk2_xmsti;
    SignalStruct<types_amba::mapinfo_type> wb_slv_i_mapinfo;
    SignalStruct<types_pnp::dev_config_type> wb_slv_o_cfg;
    Signal w_slv_o_req_valid;
    Signal wb_slv_o_req_addr;
    Signal wb_slv_o_req_size;
    Signal w_slv_o_req_write;
    Signal wb_slv_o_req_wdata;
    Signal wb_slv_o_req_wstrb;
    Signal w_slv_o_req_last;
    Signal w_slv_i_req_ready;
    Signal w_slv_i_resp_valid;
    Signal wb_slv_i_resp_rdata;
    Signal w_slv_i_resp_err;
    Logic v_busy;
    Logic rd_valid;
    Logic req_ready;
    Logic rd_addr;
    Logic rd_data;
    LogicArray mem;


    // Sub-module instances:
    pll_generic clk1;
    pll_generic clk2;
    axi_slv slv0;
    afifo_xslv tt;

    CombProcess comb;
    TestClk1Process test_clk1;
    TestClk2Process test_clk2;
};

class afifo_xslv_tb_file : public FileObject {
 public:
    afifo_xslv_tb_file(GenObject *parent) :
        FileObject(parent, "afifo_xslv_tb"),
        afifo_xslv_tb_(this, "afifo_xslv_tb") { }

 private:
    afifo_xslv_tb afifo_xslv_tb_;
};

