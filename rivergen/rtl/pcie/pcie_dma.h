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
            vb_xmsto(this, "vb_xmsto", "axi4_master_out_none", NO_COMMENT),
            vb_xbytes(this, "vb_xbytes", "XSIZE_TOTAL", RSTVAL_ZERO, "result of function call XSize2XBytes(xsize)"),
            v_req_ready(this, "v_req_ready", "1", RSTVAL_ZERO, NO_COMMENT),
            vb_req_addr(this, "vb_req_addr", "64", "0", NO_COMMENT),
            vb_req_addr1_0(this, "vb_req_addr1_0", "2", RSTVAL_ZERO, "address[1:0] restored from strob field be[3:0]"),
            vb_req_data(this, "vb_req_data", "64", RSTVAL_ZERO, NO_COMMENT),
            vb_req_strob(this, "vb_req_strob", "8", RSTVAL_ZERO, NO_COMMENT),
            v_req_last(this, "v_req_last", "1", RSTVAL_ZERO, NO_COMMENT),
            v_single_tlp32(this, "v_single_tlp32", "1", RSTVAL_ZERO, "single 32-bit dma transaction, trnasmit as 4DW with TLP header"),
            v_resp_valid(this, "v_resp_valid", "1", RSTVAL_ZERO, NO_COMMENT),
            vb_resp_data(this, "vb_resp_data", "64", RSTVAL_ZERO, NO_COMMENT),
            vb_resp_strob(this, "vb_resp_strob", "8", RSTVAL_ZERO, NO_COMMENT),
            v_resp_last(this, "v_resp_last", "1", RSTVAL_ZERO, NO_COMMENT) {
        }

     public:
        StructVar<types_pnp::dev_config_type> vb_xmst_cfg;
        StructVar<types_amba::axi4_master_out_type> vb_xmsto;
        Logic vb_xbytes;
        Logic v_req_ready;
        Logic vb_req_addr;
        Logic vb_req_addr1_0;
        Logic vb_req_data;
        Logic vb_req_strob;
        Logic v_req_last;
        Logic v_single_tlp32;
        Logic v_resp_valid;
        Logic vb_resp_data;
        Logic vb_resp_strob;
        Logic v_resp_last;
    };

    void proc_comb();

 public:
    // io:
    InPort i_nrst;
    InPort i_clk;
    InPort i_pcie_usr_rst;
    InPort i_pcie_usr_clk;
    TextLine _text0_;
    InPort i_pcie_completer_id;
    OutPort o_dma_state;
    InStruct<types_dma::pcie_dma64_in_type> i_pcie_dmai;
    OutStruct<types_dma::pcie_dma64_out_type> o_pcie_dmao;
    TextLine _text1_;
    OutStruct<types_pnp::dev_config_type> o_xmst_cfg;
    InStruct<types_amba::axi4_master_in_type> i_xmsti;
    OutStruct<types_amba::axi4_master_out_type> o_xmsto;
    
    TextLine _fmt0_;
    TextLine _fmt1_;
    ParamLogic TLP_FMT_3DW_NOPAYLOAD;
    ParamLogic TLP_FMT_4DW_NOPAYLOAD;
    ParamLogic TLP_FMT_3DW_PAYLOAD;
    ParamLogic TLP_FMT_4DW_PAYLOAD;
    TextLine _stat0_;
    TextLine _stat1_;
    ParamLogic TLP_STATUS_SUCCESS;
    ParamLogic TLP_STATUS_UNSUPPORTED;
    ParamLogic TLP_STATUS_ABORTED;
    
    TextLine _state0_;
    TextLine _state1_;
    ParamLogic STATE_RST;
    ParamLogic STATE_DW3DW4;
    ParamLogic STATE_AR;
    ParamLogic STATE_R_SINGLE32;
    ParamLogic STATE_R;
    ParamLogic STATE_AW;
    ParamLogic STATE_W;
    ParamLogic STATE_B;
    ParamLogic STATE_RESP_DW0DW1;
    ParamLogic STATE_RESP_DW2DW3;

    TextLine _fifo0_;
    ParamI32D REQ_FIFO_WIDTH;
    ParamI32D RESP_FIFO_WIDTH;

    Signal w_pcie_nrst;
    Signal wb_reqfifo_payload_i;
    Signal wb_reqfifo_payload_o;
    Signal w_reqfifo_full;
    Signal w_reqfifo_empty;
    Signal w_reqfifo_rd;

    Signal wb_respfifo_payload_i;
    Signal wb_respfifo_payload_o;
    Signal w_respfifo_full;
    Signal w_respfifo_empty;
    Signal w_respfifo_wr;

    RegSignal state;
    RegSignal dw0;
    RegSignal dw1;
    RegSignal dw2;
    RegSignal dw3;
    RegSignal req_rd_locked;
    RegSignal xlen;
    RegSignal xsize;
    RegSignal xaddr;
    RegSignal xwstrb;
    RegSignal xwdata;
    RegSignal xwena;    // AXI light: RW and W at the same time without burst
    RegSignal xrdata;
    RegSignal xerr;
    RegSignal resp_with_payload;
    RegSignal resp_data;
    RegSignal resp_status;
    RegSignal resp_cpl;
    RegSignal byte_cnt;

    CombProcess comb;

    cdc_afifo reqfifo;
    cdc_afifo respfifo;
};

class pcie_dma_file : public FileObject {
 public:
    pcie_dma_file(GenObject *parent) :
        FileObject(parent, "pcie_dma"),
        pcie_dma_(this, "pcie_dma") {}

 private:
    pcie_dma pcie_dma_;
};
