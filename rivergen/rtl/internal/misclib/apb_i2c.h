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
            v_change_data(this, "v_change_data", "1"),
            v_latch_data(this, "v_latch_data", "1"),
            vb_rdata(this, "vb_rdata", "32", "'0", NO_COMMENT) {
        }

     public:
        Logic v_change_data;
        Logic v_latch_data;
        Logic vb_rdata;
    };

    void proc_comb();

 public:
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
    OutPort o_irq;
    
    TextLine _state0_;
    ParamLogic STATE_IDLE;
    ParamLogic STATE_START;
    ParamLogic STATE_HEADER;
    ParamLogic STATE_ACK_HEADER;
    ParamLogic STATE_RX_DATA;
    ParamLogic STATE_ACK_DATA;
    ParamLogic STATE_TX_DATA;
    ParamLogic STATE_WAIT_ACK_DATA;
    ParamLogic STATE_STOP;
    TextLine _t0_;
    ParamLogic PIN_DIR_INPUT;
    ParamLogic PIN_DIR_OUTPUT;

    Signal w_req_valid;
    Signal wb_req_addr;
    Signal w_req_write;
    Signal wb_req_wdata;

    RegSignal scaler;
    RegSignal scaler_cnt;
    RegSignal setup_time;
    RegSignal level;
    TextLine _a01_;
    TextLine _a0_;
    TextLine _a1_;
    TextLine _a2_;
    TextLine _a3_;
    TextLine _a4_;
    TextLine _a5_;
    TextLine _a6_;
    TextLine _a7_;
    RegSignal addr;
    RegSignal R_nW;
    RegSignal payload;

    RegSignal state;
    RegSignal start;
    RegSignal sda_dir;
    RegSignal shiftreg;
    RegSignal rxbyte;
    RegSignal bit_cnt;
    RegSignal byte_cnt;
    RegSignal ack;
    RegSignal err_ack_header;
    RegSignal err_ack_data;
    RegSignal irq;
    RegSignal ie;
    RegSignal resp_valid;
    RegSignal resp_rdata;
    RegSignal resp_err;

    CombProcess comb;

    apb_slv pslv0;
};

class apb_i2c_file : public FileObject {
 public:
    apb_i2c_file(GenObject *parent) :
        FileObject(parent, "apb_i2c"),
        apb_i2c_(this, "apb_i2c", NO_COMMENT) {}

 private:
    apb_i2c apb_i2c_;
};

