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

#include "vip_spi_top.h"

vip_spi_top::vip_spi_top(GenObject *parent, const char *name) :
    ModuleObject(parent, "vip_spi_top", name),
    instnum(this, "instnum", "0"),
    baudrate(this, "baudrate", "2000000"),
    scaler(this, "scaler", "8"),
    pll_period(this, "pll_period", "DIV(1.0,MUL(MUL(2,scaler),baudrate))"),
    i_nrst(this, "i_nrst", "1"),
    i_csn(this, "i_csn", "1"),
    i_sclk(this, "i_sclk", "1"),
    i_mosi(this, "i_mosi", "1"),
    o_miso(this, "o_miso", "1"),
    o_vip_uart_loopback_ena(this, "o_vip_uart_loopback_ena", "1"),
    io_vip_gpio(this, "io_vip_gpio", "16"),
    // params
    // signals
    w_clk(this, "w_clk", "1"),
    w_req_valid(this, "w_req_valid", "1"),
    w_req_write(this, "w_req_write", "1"),
    wb_req_addr(this, "wb_req_addr", "32"),
    wb_req_wdata(this, "wb_req_wdata", "32"),
    w_req_ready(this, "w_req_ready", "1"),
    w_resp_valid(this, "w_resp_valid", "1"),
    wb_resp_rdata(this, "wb_resp_rdata", "32"),
    w_resp_ready(this, "w_resp_ready", "1"),
    // registers
    resp_valid(this, "resp_valid", "1"),
    resp_rdata(this, "resp_rdata", "32"),
    scratch0(this, "scratch0", "32"),
    scratch1(this, "scratch1", "32"),
    scratch2(this, "scratch2", "32"),
    uart_loopback(this, "uart_loopback", "32"),
    gpio_in(this, "gpio_in", "16"),
    gpio_out(this, "gpio_out", "16"),
    gpio_dir(this, "gpio_dir", "16"),
    //
    clk0(this, "clk0"),
    tx0(this, "tx0"),
    comb(this)
{
    Operation::start(this);

    // Create and connet Sub-modules:
    clk0.period.setObjValue(&pll_period);
    NEW(clk0, clk0.getName().c_str());
        CONNECT(clk0, 0, clk0.o_clk, w_clk);
    ENDNEW();

    tx0.scaler.setObjValue(&scaler);
    NEW(tx0, tx0.getName().c_str());
        CONNECT(tx0, 0, tx0.i_nrst, i_nrst);
        CONNECT(tx0, 0, tx0.i_clk, w_clk);
        CONNECT(tx0, 0, tx0.i_csn, i_csn);
        CONNECT(tx0, 0, tx0.i_sclk, i_sclk);
        CONNECT(tx0, 0, tx0.i_mosi, i_mosi);
        CONNECT(tx0, 0, tx0.o_miso, o_miso);
        CONNECT(tx0, 0, tx0.o_req_valid, w_req_valid);
        CONNECT(tx0, 0, tx0.o_req_write, w_req_write);
        CONNECT(tx0, 0, tx0.o_req_addr, wb_req_addr);
        CONNECT(tx0, 0, tx0.o_req_wdata, wb_req_wdata);
        CONNECT(tx0, 0, tx0.i_req_ready, w_req_ready);
        CONNECT(tx0, 0, tx0.i_resp_valid, w_resp_valid);
        CONNECT(tx0, 0, tx0.i_resp_rdata, wb_resp_rdata);
        CONNECT(tx0, 0, tx0.o_resp_ready, w_resp_ready);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void vip_spi_top::proc_comb() {
    SETVAL(comb.rdata, resp_rdata);

TEXT();
    IF (AND2(NZ(resp_valid), NZ(w_resp_ready)));
        SETZERO(resp_valid);
    ELSIF(NZ(w_req_valid));
        SETONE(resp_valid);
    ENDIF();

TEXT();
    SWITCH (BITS(wb_req_addr, 7, 2));
    CASE (CONST("0x0", 6), "[0x00] hwid");
        SETVAL(comb.rdata, CONST("0xCAFECAFE", 32));
        ENDCASE();
    CASE (CONST("0x1", 6), "[0x04] scratch0");
        SETVAL(comb.rdata, scratch0);
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            SETVAL(scratch0, wb_req_wdata);
        ENDIF();
        ENDCASE();
    CASE (CONST("0x2", 6), "[0x08] scratch1");
        SETVAL(comb.rdata, scratch0);
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            SETVAL(scratch1, wb_req_wdata);
        ENDIF();
        ENDCASE();
    CASE (CONST("0x3", 6), "[0x0C] scratch2");
        SETVAL(comb.rdata, scratch0);
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            SETVAL(scratch2, wb_req_wdata);
        ENDIF();
        ENDCASE();
    CASE (CONST("0x4", 6), "[0x10] uart control");
        SETBIT(comb.rdata, 0, uart_loopback);
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            SETVAL(uart_loopback, BIT(wb_req_wdata, 0));
        ENDIF();
        ENDCASE();
    CASE (CONST("0x5", 6), "[0x15] gpio in");
        SETBITS(comb.rdata, 15, 0, comb.vb_gpio_in);
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();
    SETVAL(resp_rdata, comb.rdata);

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETONE(w_req_ready);
    SETVAL(w_resp_valid, resp_valid);
    SETVAL(wb_resp_rdata, resp_rdata);
    SETVAL(o_vip_uart_loopback_ena, uart_loopback);
}
