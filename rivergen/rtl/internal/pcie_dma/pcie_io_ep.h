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
#include "pcie_io_rx_engine.h"
#include "pcie_io_tx_engine.h"

using namespace sysvc;

class pcie_io_ep : public ModuleObject {
 public:
    pcie_io_ep(GenObject *parent, const char *name, const char *comment);

 protected:
    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            v_mem_valid(this, "v_mem_valid", "1", RSTVAL_ZERO),
            v_mem_wren(this, "v_mem_wren", "1", RSTVAL_ZERO),
            vb_mem_addr(this, "vb_mem_addr", "13", "'0"),
            vb_mem_wstrb(this, "vb_mem_wstrb", "8", "'0"),
            vb_mem_data(this, "vb_mem_data", "32", "'0") {
        }
        Logic v_mem_valid;
        Logic v_mem_wren;
        Logic vb_mem_addr;
        Logic vb_mem_wstrb;
        Logic vb_mem_data;
    };

    void proc_comb();

 public:
    TmplParamI32D C_DATA_WIDTH;
    TmplParamI32D KEEP_WIDTH;
    // io:
    InPort i_nrst;
    InPort i_clk;
    TextLine _t0_;
    InPort i_s_axis_tx_tready;
    OutPort o_s_axis_tx_tdata;
    OutPort o_s_axis_tx_tkeep;
    OutPort o_s_axis_tx_tlast;
    OutPort o_s_axis_tx_tvalid;
    OutPort o_tx_src_dsc;
    TextLine _t1_;
    InPort i_m_axis_rx_tdata;
    InPort i_m_axis_rx_tkeep;
    InPort i_m_axis_rx_tlast;
    InPort i_m_axis_rx_tvalid;
    OutPort o_m_axis_rx_tready;
    InPort i_m_axis_rx_tuser;
    InPort i_cfg_completer_id;
    TextLine _t3_;
    InPort i_req_mem_ready;
    OutPort o_req_mem_valid;
    OutPort o_req_mem_write;
    OutPort o_req_mem_bytes;
    OutPort o_req_mem_addr;
    OutPort o_req_mem_strob;
    OutPort o_req_mem_data;
    OutPort o_req_mem_last;
    InPort i_resp_mem_valid;
    InPort i_resp_mem_last;
    InPort i_resp_mem_fault;
    InPort i_resp_mem_addr;
    InPort i_resp_mem_data;
    OutPort o_resp_mem_ready;

    Signal w_req_compl_int;
    Signal w_req_compl_wd;
    Signal w_compl_done_int;
    Signal wb_req_tc;
    Signal w_req_td;
    Signal w_req_ep;
    Signal wb_req_attr;
    Signal wb_req_len;
    Signal wb_req_rid;
    Signal wb_req_tag;
    Signal wb_req_be;
    Signal wb_req_addr;
    Signal wb_req_bytes;

    pcie_io_rx_engine EP_RX_inst;
    pcie_io_tx_engine EP_TX_inst;

    CombProcess comb;
};

class pcie_io_ep_file : public FileObject {
 public:
    pcie_io_ep_file(GenObject *parent) :
        FileObject(parent, "pcie_io_ep"),
        pcie_io_ep_(this, "pcie_io_ep", NO_COMMENT) {}

 private:
    pcie_io_ep pcie_io_ep_;
};

