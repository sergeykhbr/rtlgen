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

vip_uart_top::vip_uart_top(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "vip_uart_top", name, comment),
    instnum(this, "instnum", "0"),
    baudrate(this, "baudrate", "115200"),
    scaler(this, "scaler", "8"),
    logpath(this, "logpath", "uart"),
    pll_period(this, "pll_period", "DIV(1.0,MUL(MUL(2,scaler),baudrate))"),
    EOF_0x0D(this, "8", "EOF_0x0D", "0x0D"),
    i_nrst(this, "i_nrst", "1"),
    i_rx(this, "i_rx", "1"),
    o_tx(this, "o_tx", "1"),
    i_loopback_ena(this, "i_loopback_ena", "1", "redirect Rx bytes into Tx"),
    // params
    // signals
    w_clk(this, "w_clk", "1"),
    w_rx_rdy(this, "w_rx_rdy", "1"),
    w_rx_rdy_clr(this, "w_rx_rdy_clr", "1"),
    w_tx_we(this, "w_tx_we", "1"),
    w_tx_full(this, "w_tx_full", "1"),
    wb_rdata(this, "wb_rdata", "8"),
    wb_rdataz(this, "wb_rdataz", "8"),
    outstr("", "outstr", this),
#ifndef FROST3
    outstrtmp("", "outstrtmp", this),
#endif
    outfilename("", "outfilename", this, "formatted string name with instnum"),
    fl("", "fl", this),
#ifndef FROST3
    fl_tmp("", "fl_tmp", this),
#endif
    initdone(this, "initdone", "2"),
    // registers
    //
    clk0(this, "clk0", NO_COMMENT),
    rx0(this, "rx0", NO_COMMENT),
    tx0(this, "tx0", NO_COMMENT),
    U8ToString(this),
    comb(this),
    reg(this)
{
    Operation::start(this);
    INITIAL();
        DECLARE_TSTR();
        SETSTRF(outfilename, "%s_%1d.log", 2, &logpath, &instnum);
        FOPEN(fl, outfilename);

#ifndef FROST3
        TEXT();
        TEXT("Output string with each new symbol ended");
        SETSTRF(outfilename, "%s_%1d.log.tmp", 2, &logpath, &instnum);
        FOPEN(fl_tmp, outfilename);
#endif
    ENDINITIAL();


    // Create and connet Sub-modules:
    clk0.period.setObjValue(&pll_period);
    NEW(clk0, clk0.getName().c_str());
        CONNECT(clk0, 0, clk0.o_clk, w_clk);
    ENDNEW();

TEXT();
    rx0.scaler.setObjValue(&scaler);
    NEW(rx0, rx0.getName().c_str());
        CONNECT(rx0, 0, rx0.i_nrst, i_nrst);
        CONNECT(rx0, 0, rx0.i_clk, w_clk);
        CONNECT(rx0, 0, rx0.i_rx, i_rx);
        CONNECT(rx0, 0, rx0.o_rdy, w_rx_rdy);
        CONNECT(rx0, 0, rx0.i_rdy_clr, w_rx_rdy_clr);
        CONNECT(rx0, 0, rx0.o_data, wb_rdata);
    ENDNEW();

TEXT();
    tx0.scaler.setObjValue(&scaler);
    NEW(tx0, tx0.getName().c_str());
        CONNECT(tx0, 0, tx0.i_nrst, i_nrst);
        CONNECT(tx0, 0, tx0.i_clk, w_clk);
        CONNECT(tx0, 0, tx0.i_we, w_tx_we);
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
    istr("", "istr", this),
    symb(this, "symb", "8"),
    ostr("", "ostr", this) {
    ADDSTRU8(ostr, istr, symb);
}

void vip_uart_top::proc_comb() {
    SETVAL(w_tx_we, AND2_L(w_rx_rdy, i_loopback_ena));
    SETVAL(w_rx_rdy_clr, INV(w_tx_full));
    SETVAL(initdone, CC2(BIT(initdone, 0), CONST("1", 1)));

TEXT();
    SYNC_RESET(*this);
}

void vip_uart_top::proc_reg() {
#ifdef FROST3
    IF (NZ(w_rx_rdy));
        FWRITECHAR(fl, wb_rdata);
        FFLUSH(fl);
    ENDIF();
#else
    IF (EZ(BIT(initdone, 1)));
        SETSTR(outstrtmp, "");
        CALLF(&outstrtmp, U8ToString, 2, &outstrtmp, &EOF_0x0D);
    ENDIF();

TEXT();

    IF (NZ(w_rx_rdy));
        IF (AND2(EQ(wb_rdata, CONST("0x0A", 8)), NE(wb_rdataz, CONST("0x0D", 8))));
            TEXT("Create CR LF (0xd 0xa) instead of 0x0a:");
            CALLF(&outstr, U8ToString, 2, &outstr, &EOF_0x0D);
        ENDIF();
        TEXT("Add symbol to string:");
        CALLF(&outstr, U8ToString, 2, &outstr, &wb_rdata);
        CALLF(&outstrtmp, U8ToString, 2, &outstrtmp, &wb_rdata);

TEXT();
        IF (EQ(wb_rdata, CONST("0x0A", 8)));
            TEXT("Output simple string:");
            DISPLAYSTR(outstr);
            FWRITE(fl, outstr);
            FFLUSH(fl);
        ENDIF();

TEXT();
        TEXT("Output string with the line ending symbol 0x0D first:");
        FWRITE(fl_tmp, outstrtmp);
        FFLUSH(fl_tmp);

TEXT();
        TEXT("End-of-line");
        IF (EQ(wb_rdata, CONST("0x0A", 8)));
            SETSTR(outstr, "");
            SETSTR(outstrtmp, "");
            CALLF(&outstrtmp, U8ToString, 2, &outstrtmp, &EOF_0x0D);
        ENDIF();
        SETVAL(wb_rdataz, wb_rdata);
    ENDIF();
#endif
}
