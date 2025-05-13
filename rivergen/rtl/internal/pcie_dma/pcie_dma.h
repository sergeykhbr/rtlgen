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
#include "../ambalib/types_amba.h"
#include "../ambalib/types_pnp.h"
#include "../ambalib/types_dma.h"
#include "../cdc/cdc_afifo.h"
#include "pcie_cfg.h"
#include "pcie_io_ep.h"

using namespace sysvc;

class pcie_dma : public ModuleObject {
 public:
    pcie_dma(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb") {
        }
     public:
    };

    void proc_comb();

 public:
    // io:
    InPort i_nrst;
    InPort i_clk;
    InPort i_pcie_phy_clk;
    TextLine _text0_;
    InPort i_pcie_completer_id;
    OutPort o_dma_state;
    InStruct<types_dma::pcie_dma64_in_type> i_pcie_dmai;
    OutStruct<types_dma::pcie_dma64_out_type> o_pcie_dmao;
    TextLine _text1_;
    OutStruct<types_pnp::dev_config_type> o_xmst_cfg;
    InStruct<types_amba::axi4_master_in_type> i_xmsti;
    OutStruct<types_amba::axi4_master_out_type> o_xmsto;
    TextLine _text2_;
    OutStruct<types_dma::pcie_dma64_in_type> o_dbg_pcie_dmai;
    
    TextLine _fmt0_;
    ParamI32D C_DATA_WIDTH;
    ParamI32D KEEP_WIDTH;

    TextLine _fifo0_;
    ParamI32D REQ_FIFO_WIDTH;
    ParamI32D RESP_FIFO_WIDTH;

    Signal w_pcie_dmai_valid;       // systemc workaroung, cannot pass to port part of in structure
    Signal w_pcie_dmai_ready;       // systemc workaroung, cannot pass to port part of in structure
    Signal wb_reqfifo_payload_i;
    Signal wb_reqfifo_payload_o;
    Signal w_reqfifo_wready;
    Signal w_reqfifo_rvalid;
    Signal w_reqfifo_rd;

    Signal wb_respfifo_payload_i;
    Signal wb_respfifo_payload_o;
    Signal w_respfifo_wready;
    Signal w_respfifo_rvalid;
    Signal w_respfifo_wr;

    Signal wb_m_axis_rx_tuser;
    Signal w_m_axis_rx_tlast;
    Signal wb_m_axis_rx_tkeep;
    Signal wb_m_axis_rx_tdata;
    Signal w_s_axis_tx_tlast;
    Signal wb_s_axis_tx_tkeep;
    Signal wb_s_axis_tx_tdata;
    Signal w_tx_src_dsc;
    Signal w_req_compl;
    Signal w_compl_done;

    CombProcess comb;

    cdc_afifo reqfifo;
    cdc_afifo respfifo;
    pcie_io_ep PIO_EP_inst;
};

class pcie_dma_file : public FileObject {
 public:
    pcie_dma_file(GenObject *parent) :
        FileObject(parent, "pcie_dma"),
        pcie_dma_(this, "pcie_dma") {}

 private:
    pcie_dma pcie_dma_;
};
