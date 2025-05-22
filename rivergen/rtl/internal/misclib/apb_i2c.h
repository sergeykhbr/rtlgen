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
#include "../ambalib/apb_slv.h"
#include "sfifo.h"

using namespace sysvc;

class apb_i2c : public ModuleObject {
 public:
    apb_i2c(GenObject *parent, const char *name, const char *comment);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            v_posedge(this, "v_posedge", "1"),
            v_negedge(this, "v_negedge", "1"),
            v_txfifo_re(this, "v_txfifo_re", "1"),
            v_txfifo_we(this, "v_txfifo_we", "1"),
            vb_txfifo_wdata(this, "vb_txfifo_wdata", "8", "'0", NO_COMMENT),
            v_rxfifo_re(this, "v_rxfifo_re", "1"),
            v_inv7(this, "v_inv7", "1"),
            vb_crc7(this, "vb_crc7", "7", "'0", NO_COMMENT),
            v_inv16(this, "v_inv16", "1"),
            vb_crc16(this, "vb_crc16", "16", "'0", NO_COMMENT),
            vb_rdata(this, "vb_rdata", "32", "'0", NO_COMMENT),
            vb_shiftreg_next(this, "vb_shiftreg_next", "8", "'0", NO_COMMENT) {
        }

     public:
        Logic v_posedge;
        Logic v_negedge;
        Logic v_txfifo_re;
        Logic v_txfifo_we;
        Logic vb_txfifo_wdata;
        Logic v_rxfifo_re;
        Logic v_inv7;
        Logic vb_crc7;
        Logic v_inv16;
        Logic vb_crc16;
        Logic vb_rdata;
        Logic vb_shiftreg_next;
    };

    void proc_comb();

 public:
    TmplParamI32D log2_fifosz;
    ParamI32D fifo_dbits;
    // io:
    InPort i_clk;
    InPort i_nrst;
    InStruct<types_amba::mapinfo_type> i_mapinfo;
    OutStruct<types_pnp::dev_config_type> o_cfg;
    InStruct<types_amba::apb_in_type> i_apbi;
    OutStruct<types_amba::apb_out_type> o_apbo;
    OutPort o_scl;
    OutPort o_sda;
    OutPort o_sda_dir;
    InPort i_sda;
    
    TextLine _state0_;
    ParamLogic STATE_IDLE;
    ParamLogic STATE_HEADER;
    ParamLogic STATE_ACK_HEADER;
    ParamLogic STATE_RX_DATA;
    ParamLogic STATE_ACK_DATA;
    ParamLogic STATE_TX_DATA;
    ParamLogic STATE_WAIT_ACK_DATA;
    ParamLogic STATE_STOP;

    Signal w_req_valid;
    Signal wb_req_addr;
    Signal w_req_write;
    Signal wb_req_wdata;

    TextLine _rx0_;
    Signal w_rxfifo_we;
    Signal wb_rxfifo_wdata;
    Signal w_rxfifo_re;
    Signal wb_rxfifo_rdata;
    Signal wb_rxfifo_count;

    TextLine _tx0_;
    Signal w_txfifo_we;
    Signal wb_txfifo_wdata;
    Signal w_txfifo_re;
    Signal wb_txfifo_rdata;
    Signal wb_txfifo_count;

    RegSignal scaler;
    RegSignal scaler_cnt;
    RegSignal wdog;
    RegSignal wdog_cnt;
    RegSignal generate_crc;
    RegSignal rx_ena;
    RegSignal rx_synced;
    RegSignal rx_data_block;    // wait 0xFE marker
    RegSignal level;
    RegSignal cs;


    RegSignal state;
    RegSignal shiftreg;
    RegSignal ena_byte_cnt;
    RegSignal bit_cnt;
    RegSignal tx_val;
    RegSignal rx_val;
    RegSignal rx_ready;
    RegSignal crc7;
    RegSignal crc16;
    RegSignal spi_resp;
    RegSignal txmark;
    RegSignal rxmark;
    RegSignal resp_valid;
    RegSignal resp_rdata;
    RegSignal resp_err;

    CombProcess comb;

    apb_slv pslv0;
    sfifo rxfifo;
    sfifo txfifo;
};

class apb_i2c_file : public FileObject {
 public:
    apb_i2c_file(GenObject *parent) :
        FileObject(parent, "apb_i2c"),
        apb_i2c_(this, "apb_i2c", NO_COMMENT) {}

 private:
    apb_i2c apb_i2c_;
};

