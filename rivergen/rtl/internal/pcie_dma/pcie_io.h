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
#include "../cdc/cdc_afifo.h"
#include "pio_ep.h"

using namespace sysvc;

class pcie_io : public ModuleObject {
 public:
    pcie_io(GenObject *parent, const char *name, const char *comment);

    virtual bool isAsyncResetParam() override { return false; }

 protected:
    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb") {
        }
    };

    void proc_comb();
    void proc_reqff();

 public:
    TmplParamI32D C_DATA_WIDTH;
    TmplParamI32D KEEP_WIDTH;
    // io:
    InPort i_clk;
    InPort i_user_clk;
    InPort i_user_reset;
    InPort i_user_lnk_up;
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
    InPort i_cfg_to_turnoff;
    OutPort o_cfg_turnoff_ok;
    InPort i_cfg_completer_id;

    TextLine _fifo0_;
    ParamI32D REQ_FIFO_WIDTH;
    ParamI32D RESP_FIFO_WIDTH;

    Signal wb_reqfifo_payload_i;
    Signal wb_reqfifo_payload_o;
    Signal w_m_axis_rx_tready;
    Signal w_m_axis_rx_tvalid;
    Signal wb_m_axis_rx_tdata;
    Signal wb_m_axis_rx_tkeep;
    Signal w_m_axis_rx_tlast;
    Signal wb_m_axis_rx_tuser;

    Signal wb_respfifo_payload_i;
    Signal wb_respfifo_payload_o;
    Signal w_s_axis_tx_tready;
    Signal w_s_axis_tx_tvalid;
    Signal w_tx_src_dsc;
    Signal w_s_axis_tx_tlast;
    Signal wb_s_axis_tx_tkeep;
    Signal wb_s_axis_tx_tdata;
    TextLine _t4_;
    Signal w_req_compl;
    Signal w_compl_done;
    Logic r_trn_pending;
    Logic r_cfg_turnoff_ok;

    CombProcess comb;
    ProcObject reqff;

    cdc_afifo reqfifo;
    cdc_afifo respfifo;
    pio_ep PIO_EP_inst;
};

class pcie_io_file : public FileObject {
 public:
    pcie_io_file(GenObject *parent) :
        FileObject(parent, "pcie_io"),
        pcie_io_(this, "pcie_io", NO_COMMENT) {}

 private:
    pcie_io pcie_io_;
};

