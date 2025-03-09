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
#include "../techmap/cdc/cdc_afifo.h"
#include "pcie_cfg.h"

using namespace sysvc;

class pcie_dma : public ModuleObject {
 public:
    pcie_dma(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            vb_xmst_cfg(this, "vb_xmst_cfg", "dev_config_none", NO_COMMENT),
            vb_tx_data(this, "vb_tx_data", "64", RSTVAL_ZERO, NO_COMMENT),
            vb_tx_strob(this, "vb_tx_strob", "8", RSTVAL_ZERO, NO_COMMENT),
            v_tx_last(this, "v_tx_last", "1", RSTVAL_ZERO, NO_COMMENT),
            vb_rx_data(this, "vb_rx_data", "64", RSTVAL_ZERO, NO_COMMENT),
            v_rx_last(this, "v_rx_last", "1", RSTVAL_ZERO, NO_COMMENT) {
        }

     public:
        StructVar<types_pnp::dev_config_type> vb_xmst_cfg;
        Logic vb_tx_data;
        Logic vb_tx_strob;
        Logic v_tx_last;
        Logic vb_rx_data;
        Logic v_rx_last;
    };

    void proc_comb();

 public:
    // io:
    InPort i_sys_nrst;
    InPort i_sys_clk;
    InPort i_pcie_usr_rst;
    InPort i_pcie_usr_clk;
    TextLine _text0_;
    InStruct<types_dma::dma64_in_type> i_pcie_dmai;
    OutStruct<types_dma::dma64_out_type> o_pcie_dmao;
    TextLine _text1_;
    OutStruct<types_pnp::dev_config_type> o_xmst_cfg;
    InStruct<types_amba::axi4_master_in_type> i_xmsti;
    OutStruct<types_amba::axi4_master_out_type> o_xmsto;
    
    Signal wb_txfifo_payload_i;
    Signal wb_txfifo_payload_o;
    Signal w_txfifo_full;
    Signal w_txfifo_empty;
    Signal w_txfifo_rd;

    Signal wb_rxfifo_payload_i;
    Signal wb_rxfifo_payload_o;
    Signal w_rxfifo_full;
    Signal w_rxfifo_empty;
    Signal w_rxfifo_wr;

    CombProcess comb;

    ParamI32D TXFIFO_WIDTH;
    ParamI32D RXFIFO_WIDTH;

    cdc_afifo txfifo;
    cdc_afifo rxfifo;
};

class pcie_dma_file : public FileObject {
 public:
    pcie_dma_file(GenObject *parent) :
        FileObject(parent, "pcie_dma"),
        pcie_dma_(this, "pcie_dma") {}

 private:
    pcie_dma pcie_dma_;
};

