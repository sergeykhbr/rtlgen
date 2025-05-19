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

using namespace sysvc;

class pcie_io_tx_engine : public ModuleObject {
 public:
    pcie_io_tx_engine(GenObject *parent, const char *name, const char *comment);

    virtual bool isAsyncResetParam() override { return false; }

 protected:
    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            vb_lower_addr(this, "vb_lower_addr", "7", "'0"),
            vb_s_axis_tx_tdata(this, "vb_s_axis_tx_tdata", "C_DATA_WIDTH", "'0") {
        }

     public:
        Logic vb_lower_addr;
        Logic vb_s_axis_tx_tdata;
    };

    void proc_comb();
    void proc_reqff();

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
    InPort i_req_compl;
    InPort i_req_compl_wd;
    OutPort o_compl_done;
    TextLine _t2_;
    InPort i_req_tc;
    InPort i_req_td;
    InPort i_req_ep;
    InPort i_req_attr;
    InPort i_req_len;
    InPort i_req_rid;
    InPort i_req_tag;
    InPort i_req_be;
    InPort i_req_addr;
    InPort i_req_bytes;
    TextLine _t3_;
    InPort i_dma_resp_valid;
    InPort i_dma_resp_last;
    InPort i_dma_resp_fault;
    InPort i_dma_resp_addr;
    InPort i_dma_resp_data;
    OutPort o_dma_resp_ready;
    InPort i_completer_id;

    TextLine _fmt0_;
    ParamLogic PIO_CPLD_FMT_TYPE;
    ParamLogic PIO_CPL_FMT_TYPE;
    ParamLogic PIO_TX_RST_STATE;
    ParamLogic PIO_TX_WAIT_DMA_RESP;
    ParamLogic PIO_TX_CPLD_QW1;
    ParamLogic PIO_TX_RD_FIRST;
    ParamLogic PIO_TX_RD_BURST;

    RegSignal s_axis_tx_tdata;
    RegSignal s_axis_tx_tkeep;
    RegSignal s_axis_tx_tlast;
    RegSignal s_axis_tx_tvalid;
    RegSignal dma_resp_ready;
    RegSignal compl_done;
    RegSignal req_compl_wd_q;
    RegSignal req_addr;
    RegSignal req_rid;
    RegSignal req_tag;
    RegSignal req_be;
    RegSignal rd_data;
    RegSignal rd_addr;
    RegSignal rd_last;
    RegSignal state;

    CombProcess comb;
    ProcObject reqff;
};

class pcie_io_tx_engine_file : public FileObject {
 public:
    pcie_io_tx_engine_file(GenObject *parent) :
        FileObject(parent, "pcie_io_tx_engine"),
        pcie_io_tx_engine_(this, "pcie_io_tx_engine", NO_COMMENT) {}

 private:
    pcie_io_tx_engine pcie_io_tx_engine_;
};

