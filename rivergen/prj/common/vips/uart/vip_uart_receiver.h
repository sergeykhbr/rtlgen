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

#include <api.h>

using namespace sysvc;

class vip_uart_receiver : public ModuleObject {
 public:
    vip_uart_receiver(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            vb_rdata(this, "vb_rdata", "32") {
        }

     public:
        Logic vb_rdata;
    };

    void proc_comb();

 public:
    // io:
    InPort i_clk;
    InPort i_clken;
    InPort i_rx;
    OutPort o_rdy;
    InPort i_rdy_clr;
    OutPort o_data;
    InPort i_scaler;

    ParamLogic startbit;
    ParamLogic data;
    ParamLogic stopbit;

    Signal wb_sample_max;
    Signal wb_sample_mid;

    RegSignal state;
    RegSignal sample;
    RegSignal bitpos;
    RegSignal scratch;

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

