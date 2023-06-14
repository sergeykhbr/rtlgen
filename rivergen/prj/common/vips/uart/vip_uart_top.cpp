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

#include "vip_uart_top.h"

vip_uart_top::vip_uart_top(GenObject *parent, const char *name) :
    ModuleObject(parent, "vip_uart_top", name),
    half_period(this, "half_period", "1"),
    i_rstn(this, "i_rstn", "1"),
    i_rx(this, "i_rx", "1"),
    // params
    // signals
    w_clk(this, "w_clk", "1"),
    wb_rdata(this, "wb_rdata", "8"),
    // registers
    //
    clk0(this, "clk0"),
    rx0(this, "rx0"),
    comb(this)
{
    Operation::start(this);

    // Create and connet Sub-modules:
    clk0.half_period.setObjValue(&half_period);
    NEW(clk0, clk0.getName().c_str());
        CONNECT(clk0, 0, clk0.o_clk, w_clk);
    ENDNEW();

    NEW(rx0, rx0.getName().c_str());
        CONNECT(rx0, 0, rx0.i_clk, w_clk);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void vip_uart_top::proc_comb() {
}
