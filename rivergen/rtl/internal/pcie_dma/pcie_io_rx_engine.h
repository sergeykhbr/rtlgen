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

class pcie_io_rx_engine : public ModuleObject {
 public:
    pcie_io_rx_engine(GenObject *parent, const char *name, const char *comment);

    virtual bool isAsyncResetParam() override { return false; }

 protected:
    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            vb_req_addr_1_0(this, "vb_req_addr_1_0", "2", "'0"),
            vb_add_be20(this, "vb_add_be20", "2", "'0"),
            vb_add_be21(this, "vb_add_be21", "2", "'0"),
            vb_req_bytes(this, "vb_req_bytes", "10", "'0"),
            vb_region_select(this, "vb_region_select", "2", "'0") {
        }

     public:
        Logic vb_req_addr_1_0;
        Logic vb_add_be20;
        Logic vb_add_be21;
        Logic vb_req_bytes;
        Logic vb_region_select;
    };

    void proc_comb();

 public:
    TmplParamI32D C_DATA_WIDTH;
    TmplParamI32D KEEP_WIDTH;
    // io:
    InPort i_nrst;
    InPort i_clk;
    TextLine _t0_;
    InPort i_m_axis_rx_tdata;
    InPort i_m_axis_rx_tkeep;
    InPort i_m_axis_rx_tlast;
    InPort i_m_axis_rx_tvalid;
    OutPort o_m_axis_rx_tready;
    InPort i_m_axis_rx_tuser;
    TextLine _t1_;
    OutPort o_tx_ena;
    OutPort o_tx_completion;
    OutPort o_tx_with_data;
    InPort i_compl_done;
    TextLine _t2_;
    OutPort o_req_tc;                        // Memory Read TC
    OutPort o_req_td;                        // Memory Read TD
    OutPort o_req_ep;                        // Memory Read EP
    OutPort o_req_attr;                      // Memory Read Attribute
    OutPort o_req_len;                       // Memory Read Length (1DW)
    OutPort o_req_rid;                       // Memory Read Requestor ID
    OutPort o_req_tag;                       // Memory Read Tag
    OutPort o_req_be;                        // Memory Read Byte Enables
    OutPort o_req_addr;                      // Memory Read Address
    TextLine _t33_;
    InPort i_req_mem_ready;
    OutPort o_req_mem_valid;
    OutPort o_req_mem_write;
    OutPort o_req_mem_bytes;
    OutPort o_req_mem_addr;
    OutPort o_req_mem_strob;
    OutPort o_req_mem_data;
    OutPort o_req_mem_last;
    InPort i_resp_mem_valid;

    TextLine _t4_;
    ParamLogic PIO_RX_MEM_RD32_FMT_TYPE;
    ParamLogic PIO_RX_MEM_WR32_FMT_TYPE;
    ParamLogic PIO_RX_MEM_RD64_FMT_TYPE;
    ParamLogic PIO_RX_MEM_WR64_FMT_TYPE;
    ParamLogic PIO_RX_IO_RD32_FMT_TYPE;
    ParamLogic PIO_RX_IO_WR32_FMT_TYPE;
    TextLine _t5_;
    ParamLogic PIO_RX_RST_STATE;
    ParamLogic PIO_RX_MEM_RD32_DW1DW2;
    ParamLogic PIO_RX_MEM_WR32_DW1DW2;
    ParamLogic PIO_RX_MEM_RD64_DW1DW2;
    ParamLogic PIO_RX_MEM_WR64_DW1DW2;
    ParamLogic PIO_RX_MEM_WR_BURST;
    ParamLogic PIO_RX_IO_WR_DW1DW2;
    ParamLogic PIO_RX_WAIT_TX_COMPLETION;
    TextLine _tlp_resp0_;
    ParamLogic TLP_NON_POSTED;
    ParamLogic TLP_POSTED;
    ParamLogic TLP_COMPLETION;

    RegSignal m_axis_rx_tready;
    RegSignal req_valid;
    RegSignal req_last;
    RegSignal req_tc;
    RegSignal req_td;
    RegSignal req_ep;
    RegSignal req_attr;
    RegSignal req_len;
    RegSignal req_rid;
    RegSignal req_tag;
    RegSignal req_be;
    RegSignal req_bytes;
    RegSignal req_addr;
    RegSignal wr_en;
    RegSignal wr_data;
    RegSignal wr_strob;
    RegSignal wr_data_dw1;
    RegSignal wr_dw1_valid;
    RegSignal state;
    RegSignal tlp_type;
    RegSignal tlp_resp;

    CombProcess comb;
};

class pcie_io_rx_engine_file : public FileObject {
 public:
    pcie_io_rx_engine_file(GenObject *parent) :
        FileObject(parent, "pcie_io_rx_engine"),
        pcie_io_rx_engine_(this, "pcie_io_rx_engine", NO_COMMENT) {}

 private:
    pcie_io_rx_engine pcie_io_rx_engine_;
};

