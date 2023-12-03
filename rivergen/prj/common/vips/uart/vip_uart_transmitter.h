// 
//  Copyright 2023 Sergey Khabarov, sergeykhbr@gmail.com
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

class vip_uart_transmitter : public ModuleObject {
 public:
    vip_uart_transmitter(GenObject *parent, const char *name, const char *comment);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            v_next(this, "v_next", "1") {
        }

     public:
        Logic v_next;
    };

    void proc_comb();

 public:
    DefParamI32D scaler;
    ParamI32D scaler_max;
    // io:
    InPort i_nrst;
    InPort i_clk;
    InPort i_we;
    InPort i_wdata;
    OutPort o_full;
    OutPort o_tx;

    ParamLogic idle;
    ParamLogic startbit;
    ParamLogic data;
    ParamLogic stopbit;

    RegSignal state;
    RegSignal sample;
    RegSignal txdata_rdy;
    RegSignal txdata;
    RegSignal shiftreg;
    RegSignal bitpos;
    RegSignal overflow;

    CombProcess comb;
};

class vip_uart_transmitter_file : public FileObject {
 public:
    vip_uart_transmitter_file(GenObject *parent) :
        FileObject(parent, "vip_uart_transmitter"),
        vip_uart_transmitter_(this, "vip_uart_transmitter", NO_COMMENT) {}

 private:
    vip_uart_transmitter vip_uart_transmitter_;
};

