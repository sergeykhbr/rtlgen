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

#include "vip_uart_receiver.h"

vip_uart_receiver::vip_uart_receiver(GenObject *parent, const char *name) :
    ModuleObject(parent, "vip_uart_receiver", name),
    i_clk(this, "i_clk", "1"),
    i_clken(this, "i_clken", "1"),
    i_rx(this, "i_rx", "1"),
    o_rdy(this, "o_rdy", "1"),
    i_rdy_clr(this, "i_rdy_clr", "1"),
    o_data(this, "o_data", "8"),
    i_scaler(this, "i_scaler", "32"),
    // params
    startbit(this, "2", "startbit", "0"),
    data(this, "2", "data", "1"),
    stopbit(this, "2", "stopbit", "2"),
    // signals
    wb_sample_max(this, "wb_sample_max", "32"),
    wb_sample_mid(this, "wb_sample_mid", "32"),
    // registers
    state(this, "state", "2", "startbit"),
    sample(this, "sample", "32"),
    bitpos(this, "bitpos", "4"),
    scratch(this, "scratch", "8"),
    //
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void vip_uart_receiver::proc_comb() {
}
