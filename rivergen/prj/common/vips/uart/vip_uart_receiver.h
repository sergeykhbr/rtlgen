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

class vip_uart_receiver : public ModuleObject {
 public:
    vip_uart_receiver(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            v_rx_pos(this, "v_rx_pos", "1"),
            v_rx_neg(this, "v_rx_neg", "1") {
        }

     public:
        Logic v_rx_pos;
        Logic v_rx_neg;
    };

    void proc_comb();

 public:
    DefParamI32D scaler;
    ParamI32D scaler_max;
    ParamI32D scaler_mid;
    // io:
    InPort i_nrst;
    InPort i_clk;
    InPort i_rx;
    OutPort o_rdy;
    InPort i_rdy_clr;
    OutPort o_data;

    ParamLogic startbit;
    ParamLogic data;
    ParamLogic stopbit;
    ParamLogic dummy;

    RegSignal rx;
    RegSignal state;
    RegSignal rdy;
    RegSignal rdata;
    RegSignal sample;
    RegSignal bitpos;
    RegSignal scratch;
    RegSignal rx_err;

    CombProcess comb;
};

class vip_uart_receiver_file : public FileObject {
 public:
    vip_uart_receiver_file(GenObject *parent) :
        FileObject(parent, "vip_uart_receiver"),
        vip_uart_receiver_(this, "") {}

 private:
    vip_uart_receiver vip_uart_receiver_;
};

