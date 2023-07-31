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
    instnum(this, "instnum", "0"),
    baudrate(this, "baudrate", "115200"),
    scaler(this, "scaler", "8"),
    logpath(this, "logpath", "uart"),
    pll_period(this, "pll_period", "DIV(1.0,MUL(MUL(2,scaler),baudrate))"),
    EOF_0x0D(this, "8", "EOF_0x0D", "0x0D"),
    i_nrst(this, "i_nrst", "1"),
    i_rx(this, "i_rx", "1"),
    o_tx(this, "o_tx", "1"),
    // params
    // signals
    w_clk(this, "w_clk", "1"),
    w_rx_rdy(this, "w_rx_rdy", "1"),
    w_rx_rdy_clr(this, "w_rx_rdy_clr", "1"),
    w_tx_full(this, "w_tx_full", "1"),
    wb_rdata(this, "wb_rdata", "8"),
    rdatastr("", "rdatastr", this),
    outstr("", "outstr", this),
    outfilename("", "outfilename", this, "formatted string name with instnum"),
    fl("", "fl", this),
    fl_tmp("", "fl_tmp", this),
    // registers
    //
    clk0(this, "clk0"),
    rx0(this, "rx0"),
    tx0(this, "tx0"),
    U8ToString(this),
    comb(this),
    reg(this)
{
    Operation::start(this);
    INITIAL();
        DECLARE_TSTR();
        SETSTRF(outfilename, "%s_%1d.log", 2, &logpath, &instnum);
        FOPEN(fl, outfilename);

        TEXT();
        TEXT("Output string with each new symbol ended");
        SETSTRF(outfilename, "%s_%1d.log.tmp", 2, &logpath, &instnum);
        FOPEN(fl_tmp, outfilename);
    ENDINITIAL();


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
        CONNECT(rx0, 0, rx0.o_rdy, w_rx_rdy);
        CONNECT(rx0, 0, rx0.i_rdy_clr, w_rx_rdy_clr);
        CONNECT(rx0, 0, rx0.o_data, wb_rdata);
    ENDNEW();

    tx0.scaler.setObjValue(&scaler);
    NEW(tx0, tx0.getName().c_str());
        CONNECT(tx0, 0, tx0.i_nrst, i_nrst);
        CONNECT(tx0, 0, tx0.i_clk, w_clk);
        CONNECT(tx0, 0, tx0.i_we, w_rx_rdy);
        CONNECT(tx0, 0, tx0.i_wdata, wb_rdata);
        CONNECT(tx0, 0, tx0.o_full, w_tx_full);
        CONNECT(tx0, 0, tx0.o_tx, o_tx);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();

    Operation::start(&reg);
    proc_reg();
}

vip_uart_top::FunctionU8ToString::FunctionU8ToString(GenObject *parent)
    : FunctionObject(parent, "U8ToString"),
    symb(this, "symb", "8"),
    ostr("", "ostr", this) {
    ADDSTRU8(ostr, symb);
}

void vip_uart_top::proc_comb() {
    SETVAL(w_rx_rdy_clr, w_rx_rdy);
}

void vip_uart_top::proc_reg() {
    IF (NZ(w_rx_rdy));
        IF (EQ(wb_rdata, CONST("0x0A", 8)));
            TEXT("Use 0x0d as a new symbol in fl_tmp file:");
            CALLF(&rdatastr, U8ToString, 1, &EOF_0x0D);
            FWRITE(fl_tmp, rdatastr);  // end of line first
            FWRITE(fl_tmp, outstr);
            FFLUSH(fl_tmp);

            TEXT();
            DISPLAYSTR(outstr);
            FWRITE(fl, outstr);
            FFLUSH(fl);
            SETSTR(outstr, "");
        ELSIF(EQ(wb_rdata, CONST("0x0D", 8)));
            IF (NE(outstr, *new STRING("", "")));
                DISPLAYSTR(outstr);
                FWRITE(fl, outstr);
                FFLUSH(fl);
                SETSTR(outstr, "");
            ENDIF();
        ELSE();
            TEXT("Add symbol to string");
            CALLF(&rdatastr, U8ToString, 1, &wb_rdata);
            INCVAL(outstr, rdatastr);
            
            TEXT();
            TEXT("Output string with the line ending symbol 0x0D first:");
            CALLF(&rdatastr, U8ToString, 1, &EOF_0x0D);
            FWRITE(fl_tmp, rdatastr);  // end of line first
            FWRITE(fl_tmp, outstr);
            FFLUSH(fl_tmp);
        ENDIF();
    ENDIF();
}
