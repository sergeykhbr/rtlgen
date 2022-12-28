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
#include "sfifo.h"

using namespace sysvc;

class apb_spi : public ModuleObject {
 public:
    apb_spi(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            v_posedge(this, "v_posedge", "1"),
            v_negedge(this, "v_negedge", "1"),
            v_txfifo_re(this, "v_txfifo_re", "1"),
            v_txfifo_we(this, "v_txfifo_we", "1"),
            vb_txfifo_wdata(this, "vb_txfifo_wdata", "8"),
            v_rxfifo_re(this, "v_rxfifo_re", "1"),
            v_rxfifo_we(this, "v_rxfifo_we", "1"),
            vb_rxfifo_wdata(this, "vb_rxfifo_wdata", "8"),
            v_xor7(this, "v_xor7", "1"),
            vb_crc7(this, "vb_crc", "8"),
            vb_rdata(this, "vb_rdata", "32") {
        }

     public:
        Logic v_posedge;
        Logic v_negedge;
        Logic v_txfifo_re;
        Logic v_txfifo_we;
        Logic vb_txfifo_wdata;
        Logic v_rxfifo_re;
        Logic v_rxfifo_we;
        Logic vb_rxfifo_wdata;
        Logic v_xor7;
        Logic vb_crc7;
        Logic vb_rdata;
    };

    void proc_comb();

 public:
    TmplParamI32D log2_fifosz;
    ParamI32D fifo_dbits;
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
    
    TextLine _state0_;
    ParamLogic idle;
    ParamLogic wait_edge;
    ParamLogic send_data;
    ParamLogic ending;

    Signal w_req_valid;
    Signal wb_req_addr;
    Signal w_req_write;
    Signal wb_req_wdata;

    TextLine _rx0_;
    Signal wb_rxfifo_thresh;
    Signal w_rxfifo_we;
    Signal wb_rxfifo_wdata;
    Signal w_rxfifo_re;
    Signal wb_rxfifo_rdata;
    Signal w_rxfifo_full;
    Signal w_rxfifo_empty;
    Signal w_rxfifo_less;
    Signal w_rxfifo_greater;

    TextLine _tx0_;
    Signal wb_txfifo_thresh;
    Signal w_txfifo_we;
    Signal wb_txfifo_wdata;
    Signal w_txfifo_re;
    Signal wb_txfifo_rdata;
    Signal w_txfifo_full;
    Signal w_txfifo_empty;
    Signal w_txfifo_less;
    Signal w_txfifo_greater;

    RegSignal scaler;
    RegSignal scaler_cnt;
    RegSignal generate_crc;
    RegSignal level;
    RegSignal cs;


    RegSignal state;
    RegSignal ena_byte_cnt;
    RegSignal bit_cnt;
    RegSignal tx_val;
    RegSignal tx_shift;
    RegSignal rx_shift;
    RegSignal rx_ready;
    RegSignal crc7;
    RegSignal spi_resp;
    RegSignal resp_valid;
    RegSignal resp_rdata;
    RegSignal resp_err;

    CombProcess comb;

    apb_slv pslv0;
    sfifo rxfifo;
    sfifo txfifo;
};

class apb_spi_file : public FileObject {
 public:
    apb_spi_file(GenObject *parent) :
        FileObject(parent, "apb_spi"),
        apb_spi_(this, "") {}

 private:
    apb_spi apb_spi_;
};

