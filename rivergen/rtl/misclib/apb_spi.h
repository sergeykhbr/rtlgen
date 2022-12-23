// 
//  Copyright 2022 Sergey Khabarov, sergeykhbr@gmail.com
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

#include <api.h>
#include "../ambalib/types_amba.h"
#include "../ambalib/apb_slv.h"

using namespace sysvc;

class apb_spi : public ModuleObject {
 public:
    apb_spi(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            vb_rdata(this, "vb_rdata", "32"),
            vb_tx_wr_cnt_next(this, "vb_tx_wr_cnt_next", "log2_fifosz"),
            v_tx_fifo_full(this, "v_tx_fifo_full", "1"),
            v_tx_fifo_empty(this, "v_tx_fifo_empty", "1"),
            vb_tx_fifo_rdata(this, "vb_tx_fifo_rdata", "8"),
            v_tx_fifo_we(this, "v_tx_fifo_we", "1"),
            vb_rx_wr_cnt_next(this, "vb_rx_wr_cnt_next", "log2_fifosz"),
            v_rx_fifo_full(this, "v_rx_fifo_full", "1"),
            v_rx_fifo_empty(this, "v_rx_fifo_empty", "1"),
            vb_rx_fifo_rdata(this, "vb_rx_fifo_rdata", "8"),
            v_rx_fifo_we(this, "v_rx_fifo_we", "1"),
            v_rx_fifo_re(this, "v_rx_fifo_re", "1"),
            v_negedge_flag(this, "v_negedge_flag", "1"),
            v_posedge_flag(this, "v_posedge_flag", "1"),
            v_par(this, "par", "1") {
        }

     public:
        Logic vb_rdata;
        Logic vb_tx_wr_cnt_next;
        Logic v_tx_fifo_full;
        Logic v_tx_fifo_empty;
        Logic vb_tx_fifo_rdata;
        Logic v_tx_fifo_we;
        Logic vb_rx_wr_cnt_next;
        Logic v_rx_fifo_full;
        Logic v_rx_fifo_empty;
        Logic vb_rx_fifo_rdata;
        Logic v_rx_fifo_we;
        Logic v_rx_fifo_re;
        Logic v_negedge_flag;
        Logic v_posedge_flag;
        Logic v_par;
    };

    void proc_comb();

 public:
    TmplParamI32D log2_fifosz;
    // io:
    InPort i_clk;
    InPort i_nrst;
    InStruct<types_amba::mapinfo_type> i_mapinfo;
    OutStruct<types_amba::dev_config_type> o_cfg;
    InStruct<types_amba::apb_in_type> i_apbi;
    OutStruct<types_amba::apb_out_type> o_apbo;
    OutPort o_cs;
    OutPort o_sclk;
    OutPort o_miso;
    InPort i_mosi;
    InPort i_detected;
    InPort i_protect;
    
    ParamI32D fifosz;
    TextLine _state0_;
    ParamLogic idle;
    ParamLogic send_data;
    ParamLogic receive_data;

    Signal w_req_valid;
    Signal wb_req_addr;
    Signal w_req_write;
    Signal wb_req_wdata;

    RegSignal scaler;
    RegSignal scaler_cnt;
    RegSignal level;
    RegSignal cs;

    WireArray<RegSignal> rx_fifo;
    WireArray<RegSignal> tx_fifo;

    RegSignal state;
    RegSignal wr_cnt;
    RegSignal rd_cnt;
    RegSignal bit_cnt;
    RegSignal tx_shift;
    RegSignal rx_shift;
    RegSignal spi_resp;
    RegSignal resp_valid;
    RegSignal resp_rdata;
    RegSignal resp_err;

    CombProcess comb;

    apb_slv pslv0;
};

class apb_spi_file : public FileObject {
 public:
    apb_spi_file(GenObject *parent) :
        FileObject(parent, "apb_spi"),
        apb_spi_(this, "") {}

 private:
    apb_spi apb_spi_;
};

