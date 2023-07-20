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
    baudrate(this, "baudrate", "115200"),
    scaler(this, "scaler", "8"),
    pll_period(this, "pll_period", "DIV(1.0,MUL(MUL(2,scaler),baudrate))"),
    i_nrst(this, "i_nrst", "1"),
    i_rx(this, "i_rx", "1"),
    // params
    // signals
    w_clk(this, "w_clk", "1"),
    w_rdy(this, "w_rdy", "1"),
    w_rdy_clr(this, "w_rdy_clr", "1"),
    wb_rdata(this, "wb_rdata", "8"),
    // registers
    //
    clk0(this, "clk0"),
    rx0(this, "rx0"),
    comb(this)
{
    Operation::start(this);

    // Create and connet Sub-modules:
    clk0.period.setObjValue(&pll_period);
    NEW(clk0, clk0.getName().c_str());
        CONNECT(clk0, 0, clk0.o_clk, w_clk);
    ENDNEW();

    rx0.scaler.setObjValue(&scaler);
    NEW(rx0, rx0.getName().c_str());
        CONNECT(rx0, 0, rx0.i_nrst, i_nrst);
        CONNECT(rx0, 0, rx0.i_clk, w_clk);
        CONNECT(rx0, 0, rx0.i_rx, i_rx);
        CONNECT(rx0, 0, rx0.o_rdy, w_rdy);
        CONNECT(rx0, 0, rx0.i_rdy_clr, w_rdy_clr);
        CONNECT(rx0, 0, rx0.o_data, wb_rdata);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void vip_uart_top::proc_comb() {
    IF (NZ(w_rdy));
        IF (EQ(wb_rdata, CONST("0x0A", 8)));
            TEXT("End of line");
        ELSE();
            TEXT("Add symbol to string");
        ENDIF();
    ENDIF();

    TEXT();
    SETVAL(w_rdy_clr, w_rdy);
}
