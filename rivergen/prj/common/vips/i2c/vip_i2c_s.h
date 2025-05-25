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

class vip_i2c_s : public ModuleObject {
 public:
    vip_i2c_s(GenObject *parent, const char *name, const char *comment);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            v_latch_data(this, "v_latch_data", "1") {
        }

     public:
        Logic v_latch_data;
    };

    void proc_comb();

 public:
    // io:
    InPort i_clk;
    InPort i_nrst;
    InPort i_scl;
    InPort i_sda;
    OutPort o_sda;
    OutPort o_sda_dir;
    
    TextLine _state0_;
    ParamLogic STATE_IDLE;
    ParamLogic STATE_HEADER;
    ParamLogic STATE_ACK_HEADER;
    ParamLogic STATE_RX_DATA;
    ParamLogic STATE_ACK_DATA;
    ParamLogic STATE_TX_DATA;
    ParamLogic STATE_WAIT_ACK_DATA;
    TextLine _t0_;
    ParamLogic PIN_DIR_INPUT;
    ParamLogic PIN_DIR_OUTPUT;


    RegSignal sda;
    RegSignal scl;

    RegSignal state;
    RegSignal control_start;
    RegSignal control_stop;
    RegSignal addr;
    RegSignal read;
    RegSignal rdata;
    RegSignal sda_dir;
    RegSignal txbyte;
    RegSignal rxbyte;
    RegSignal bit_cnt;

    CombProcess comb;
};

class vip_i2c_s_file : public FileObject {
 public:
    vip_i2c_s_file(GenObject *parent) :
        FileObject(parent, "vip_i2c_s"),
        vip_i2c_s_(this, "vip_i2c_s", NO_COMMENT) {}

 private:
    vip_i2c_s vip_i2c_s_;
};

