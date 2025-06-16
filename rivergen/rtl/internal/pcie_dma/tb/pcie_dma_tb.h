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
#include "../pcie_dma.h"
#include "../../../sim/pll/pll_generic.h"
#include "../../ambalib/axi_slv.h"

using namespace sysvc;

class pcie_dma_tb : public ModuleObject {
 public:
    pcie_dma_tb(GenObject *parent, const char *name);

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

    // PCIE Clock
    class TestProcess : public ProcObject {
     public:
        TestProcess(GenObject *parent, GenObject *clk) :
            ProcObject(parent, "test", clk, CLK_POSEDGE, 0, ACTIVE_NONE, NO_COMMENT),
            vb_dmai(this, "vb_dmai", NO_COMMENT) {
        }
     public:
        types_dma::pcie_dma64_in_type vb_dmai;
    };

    // Bus Clock
    class BusProcess : public ProcObject {
     public:
        BusProcess(GenObject *parent, GenObject *clk) :
            ProcObject(parent, "bus", clk, CLK_POSEDGE, 0, ACTIVE_NONE, NO_COMMENT) {
        }
     public:
    };

    void comb_proc();
    void test_proc();
    void bus_proc();

public:
    // Ports:
    Signal i_nrst;
    Signal i_clk;
    Signal w_pci_clk;
    Logic wb_clk_cnt;
    Signal wb_i_pcie_completer_id;
    Signal wb_o_dma_state;
    SignalStruct<types_dma::pcie_dma64_in_type> wb_i_pcie_dmai;
    SignalStruct<types_dma::pcie_dma64_out_type> wb_o_pcie_dmao;
    SignalStruct<types_pnp::dev_config_type> wb_o_xmst_cfg;
    SignalStruct<types_amba::axi4_master_in_type> wb_i_xmsti;
    SignalStruct<types_amba::axi4_master_out_type> wb_o_xmsto;
    Signal w_o_dbg_valid;
    Signal wb_o_dbg_payload;
    SignalStruct<types_amba::mapinfo_type> wb_slv_i_mapinfo;
    SignalStruct<types_pnp::dev_config_type> wb_slv_o_cfg;
    SignalStruct<types_amba::axi4_slave_in_type> wb_slv_i_xslvi;
    SignalStruct<types_amba::axi4_slave_out_type> wb_slv_o_xslvo;
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
    Logic rd_valid;
    Logic rd_addr;
    LogicArray mem;


    // Sub-module instances:
    pll_generic clk0;
    pll_generic clk1;
    axi_slv slv0;
    pcie_dma tt;

    CombProcess comb;
    TestProcess test;
    BusProcess bus;
};

class pcie_dma_tb_file : public FileObject {
 public:
    pcie_dma_tb_file(GenObject *parent) :
        FileObject(parent, "pcie_dma_tb"),
        pcie_dma_tb_(this, "pcie_dma_tb") { }

 private:
    pcie_dma_tb pcie_dma_tb_;
};

