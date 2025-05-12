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
#include "pio_ep_mem_access.h"
#include "pio_rx_engine.h"
#include "pio_tx_engine.h"

using namespace sysvc;

class pio_ep : public ModuleObject {
 public:
    pio_ep(GenObject *parent, const char *name, const char *comment);

    virtual bool isAsyncResetParam() override { return false; }

 protected:
    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb") {
        }
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
    TextLine _t2_;
    OutPort o_req_compl;
    OutPort o_compl_done;
    InPort i_cfg_completer_id;

    Signal wb_rd_addr;
    Signal wb_rd_be;
    Signal wb_rd_data;
    Signal wb_wr_addr;
    Signal wb_wr_be;
    Signal wb_wr_data;
    Signal w_wr_en;
    Signal w_wr_busy;
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

    pio_ep_mem_access EP_MEM_inst;
    pio_rx_engine EP_RX_inst;
    pio_tx_engine EP_TX_inst;

    CombProcess comb;
};

class pio_ep_file : public FileObject {
 public:
    pio_ep_file(GenObject *parent) :
        FileObject(parent, "PIO_EP"),
        pio_ep_(this, "PIO_EP", NO_COMMENT) {}

 private:
    pio_ep pio_ep_;
};

