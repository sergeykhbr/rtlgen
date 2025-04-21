// 
//  Copyright 2022 Sergey Khabarov, sergeykhbr@gmail.com
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

#include "apb_uart.h"

apb_uart::apb_uart(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "apb_uart", name, comment),
    log2_fifosz(this, "log2_fifosz", "4", NO_COMMENT),
    sim_speedup_rate(this, "sim_speedup_rate", "0", "simulation speed-up: 0=no speed up, 1=2x, 2=4x, etc"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_mapinfo(this, "i_mapinfo", "interconnect slot information"),
    o_cfg(this, "o_cfg", "Device descriptor"),
    i_apbi(this, "i_apbi", "APB  Slave to Bridge interface"),
    o_apbo(this, "o_apbo", "APB Bridge to Slave interface"),
    i_rd(this, "i_rd", "1"),
    o_td(this, "o_td", "1"),
    o_irq(this, "o_irq", "1"),
    // params
    fifosz(this, "fifosz", "POW2(1,log2_fifosz)"),
    _state0_(this, "Rx/Tx states"),
    idle(this, "idle", "3", "0", NO_COMMENT),
    startbit(this, "startbit", "3", "1", NO_COMMENT),
    data(this, "data", "3", "2", NO_COMMENT),
    parity(this, "parity", "3", "3", NO_COMMENT),
    stopbit(this, "stopbit", "3", "4", NO_COMMENT),
    // signals
    w_req_valid(this, "w_req_valid", "1"),
    wb_req_addr(this, "wb_req_addr", "32"),
    w_req_write(this, "w_req_write", "1"),
    wb_req_wdata(this, "wb_req_wdata", "32"),
    // registers
    scaler(this, "scaler", "32", "'0", NO_COMMENT),
    scaler_cnt(this, "scaler_cnt", "32", "'0", NO_COMMENT),
    level(this, "level", "1", "1"),
    err_parity(this, "err_parity", "1"),
    err_stopbit(this, "err_stopbit", "1"),
    fwcpuid(this, "fwcpuid", "32", "'0", NO_COMMENT),
    _rx0_(this),
    rx_fifo(this, &i_clk, CLK_POSEDGE, &i_nrst, ACTIVE_LOW, "rx_fifo",
            "8", "fifosz", RSTVAL_ZERO, NO_COMMENT),
    rx_state(this, "rx_state", "3", "idle"),
    rx_ena(this, "rx_ena", "1"),
    rx_ie(this, "rx_ie", "1"),
    rx_ip(this, "rx_ip", "1"),
    rx_nstop(this, "rx_nstop", "1"),
    rx_par(this, "rx_par", "1"),
    rx_wr_cnt(this, "rx_wr_cnt", "log2_fifosz", "'0", NO_COMMENT),
    rx_rd_cnt(this, "rx_rd_cnt", "log2_fifosz", "'0", NO_COMMENT),
    rx_byte_cnt(this, "rx_byte_cnt", "log2_fifosz", "'0", NO_COMMENT),
    rx_irq_thresh(this, "rx_irq_thresh", "log2_fifosz", "'0", NO_COMMENT),
    rx_frame_cnt(this, "rx_frame_cnt", "4", "'0", NO_COMMENT),
    rx_stop_cnt(this, "rx_stop_cnt", "1"),
    rx_shift(this, "rx_shift", "11", "'0", NO_COMMENT),
    _tx0_(this),
    tx_fifo(this, &i_clk, CLK_POSEDGE, &i_nrst, ACTIVE_LOW, "tx_fifo",
            "8", "fifosz", RSTVAL_ZERO, NO_COMMENT),
    tx_state(this, "tx_state", "3", "idle"),
    tx_ena(this, "tx_ena", "1"),
    tx_ie(this, "tx_ie", "1"),
    tx_ip(this, "tx_ip", "1"),
    tx_nstop(this, "tx_nstop", "1"),
    tx_par(this, "tx_par", "1"),
    tx_wr_cnt(this, "tx_wr_cnt", "log2_fifosz", "'0", NO_COMMENT),
    tx_rd_cnt(this, "tx_rd_cnt", "log2_fifosz", "'0", NO_COMMENT),
    tx_byte_cnt(this, "tx_byte_cnt", "log2_fifosz", "'0", NO_COMMENT),
    tx_irq_thresh(this, "tx_irq_thresh", "log2_fifosz", "'0", NO_COMMENT),
    tx_frame_cnt(this, "tx_frame_cnt", "4", "'0", NO_COMMENT),
    tx_stop_cnt(this, "tx_stop_cnt", "1"),
    tx_shift(this, "tx_shift", "11", "'1"),
    tx_amo_guard(this, "tx_amo_guard", "1", "0", "AMO operation read-modify-write often hit on full flag border"),
    resp_valid(this, "resp_valid", "1"),
    resp_rdata(this, "resp_rdata", "32", "'0", NO_COMMENT),
    resp_err(this, "resp_err", "1"),
    //
    comb(this),
    pslv0(this, "pslv0")
{
    Operation::start(this);

    pslv0.vid.setObjValue(&glob_pnp_cfg_->VENDOR_OPTIMITECH);
    pslv0.did.setObjValue(&glob_pnp_cfg_->OPTIMITECH_UART);
    NEW(pslv0, pslv0.getName().c_str());
        CONNECT(pslv0, 0, pslv0.i_clk, i_clk);
        CONNECT(pslv0, 0, pslv0.i_nrst, i_nrst);
        CONNECT(pslv0, 0, pslv0.i_mapinfo, i_mapinfo);
        CONNECT(pslv0, 0, pslv0.o_cfg, o_cfg);
        CONNECT(pslv0, 0, pslv0.i_apbi, i_apbi);
        CONNECT(pslv0, 0, pslv0.o_apbo, o_apbo);
        CONNECT(pslv0, 0, pslv0.o_req_valid, w_req_valid);
        CONNECT(pslv0, 0, pslv0.o_req_addr, wb_req_addr);
        CONNECT(pslv0, 0, pslv0.o_req_write, w_req_write);
        CONNECT(pslv0, 0, pslv0.o_req_wdata, wb_req_wdata);
        CONNECT(pslv0, 0, pslv0.i_resp_valid, resp_valid);
        CONNECT(pslv0, 0, pslv0.i_resp_rdata, resp_rdata);
        CONNECT(pslv0, 0, pslv0.i_resp_err, resp_err);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void apb_uart::proc_comb() {
    SETVAL(comb.vb_rx_fifo_rdata, ARRITEM(rx_fifo, TO_INT(rx_rd_cnt), rx_fifo));
    SETVAL(comb.vb_tx_fifo_rdata, ARRITEM(tx_fifo, TO_INT(tx_rd_cnt), tx_fifo));

TEXT();
    TEXT("Check FIFOs counters with thresholds:");
    IF (LS(tx_byte_cnt, tx_irq_thresh));
        SETVAL(tx_ip, tx_ie);
    ENDIF();

    TEXT();
    IF (GT(rx_byte_cnt, rx_irq_thresh));
        SETVAL(rx_ip, rx_ie);
    ENDIF();

TEXT();
    TEXT("Transmitter's FIFO:");
    SETVAL(comb.vb_tx_wr_cnt_next, INC(tx_wr_cnt));
    IF (EQ(comb.vb_tx_wr_cnt_next, tx_rd_cnt));
        SETONE(comb.v_tx_fifo_full);
    ENDIF();

    TEXT();
    IF (EQ(tx_rd_cnt, tx_wr_cnt));
        SETONE(comb.v_tx_fifo_empty);
        SETZERO(tx_byte_cnt);
    ENDIF();

    TEXT("Receiver's FIFO:");
    SETVAL(comb.vb_rx_wr_cnt_next, INC(rx_wr_cnt));
    IF (EQ(comb.vb_rx_wr_cnt_next, rx_rd_cnt));
        SETONE(comb.v_rx_fifo_full);
    ENDIF();
 
    TEXT();
    IF (EQ(rx_rd_cnt, rx_wr_cnt));
        SETONE(comb.v_rx_fifo_empty);
        SETZERO(rx_byte_cnt);
    ENDIF();

TEXT();
    TEXT("system bus clock scaler to baudrate:");
    IF (NZ(scaler));
        IF (EQ(scaler_cnt, DEC(scaler)));
            SETZERO(scaler_cnt);
            SETVAL(level, INV(level));
            SETVAL(comb.v_posedge_flag, INV(level));
            SETVAL(comb.v_negedge_flag, (level));
        ELSE();
            SETVAL(scaler_cnt, INC(scaler_cnt));
        ENDIF();

        TEXT();
        IF (ANDx(2, &AND2(EQ(rx_state, idle),  NZ(i_rd)),
                    &AND2(EQ(tx_state, idle), NZ(comb.v_tx_fifo_empty))));
            SETZERO(scaler_cnt);
            SETONE(level);
        ENDIF();
    ENDIF();

TEXT();
    TEXT("Transmitter's state machine:");
    IF (NZ(comb.v_posedge_flag));
        SWITCH (tx_state);
        CASE(idle);
            IF (AND2(EZ(comb.v_tx_fifo_empty), NZ(tx_ena)));
                TEXT("stopbit=1,parity=xor,data[7:0],startbit=0");
                IF (NZ(tx_par));
                    SETVAL(comb.v_par, XORx(8, &BIT(comb.vb_tx_fifo_rdata, 7),
                                               &BIT(comb.vb_tx_fifo_rdata, 6),
                                               &BIT(comb.vb_tx_fifo_rdata, 5),
                                               &BIT(comb.vb_tx_fifo_rdata, 4),
                                               &BIT(comb.vb_tx_fifo_rdata, 3),
                                               &BIT(comb.vb_tx_fifo_rdata, 2),
                                               &BIT(comb.vb_tx_fifo_rdata, 1),
                                               &BIT(comb.vb_tx_fifo_rdata, 0)));
                    SETVAL(tx_shift, CC4(CONST("1", 1), comb.v_par, comb.vb_tx_fifo_rdata, CONST("0", 1)));
                ELSE();
                    SETVAL(tx_shift, CC3(CONST("3", 2), comb.vb_tx_fifo_rdata, CONST("0", 1)));
                ENDIF();
                    
                TEXT();
                SETVAL(tx_state, startbit);
                SETVAL(tx_rd_cnt, INC(tx_rd_cnt));
                SETVAL(tx_byte_cnt, DEC(tx_byte_cnt));
                SETZERO(tx_frame_cnt);
            ELSE();
                SETVAL(tx_shift, ALLONES());
            ENDIF();
            ENDCASE();
        CASE(startbit);
            SETVAL(tx_state, data);
            ENDCASE();
        CASE (data);
            IF (EQ(tx_frame_cnt, CONST("8")));
                IF (NZ(tx_par));
                    SETVAL(tx_state, parity);
                ELSE();
                    SETVAL(tx_state, stopbit);
                    SETVAL(tx_stop_cnt, tx_nstop);
                ENDIF();
            ENDIF();
            ENDCASE();
        CASE (parity);
            SETVAL(tx_state, stopbit);
            ENDCASE();
        CASE (stopbit);
            IF (EZ(tx_stop_cnt));
                SETVAL(tx_state, idle);
                SETVAL(tx_shift, ALLONES());
            ELSE();
                SETZERO(tx_stop_cnt);
            ENDIF();
            ENDCASE();
        CASEDEF();
            ENDCASE();
        ENDSWITCH();
        
        TEXT();
        IF (AND2(NE(tx_state, idle), NE(tx_state, stopbit)));
            SETVAL(tx_frame_cnt, INC(tx_frame_cnt));
            SETVAL(tx_shift, CC2(CONST("1", 1), BITS(tx_shift, 10, 1)));
        ENDIF();
    ENDIF("posedge");

TEXT();
    TEXT("Receiver's state machine:");
    IF (NZ(comb.v_negedge_flag));
        SWITCH (rx_state);
        CASE (idle);
            IF (AND2(EZ(i_rd), NZ(rx_ena)));
                SETVAL(rx_state, data);
                SETZERO(rx_shift);
                SETZERO(rx_frame_cnt);
            ENDIF();
            ENDCASE();
        CASE (data);
            SETVAL(rx_shift, CC2(i_rd, BITS(rx_shift, 7, 1)));
            IF (EQ(rx_frame_cnt, CONST("7", 4)));
                IF (NZ(rx_par));
                    SETVAL(rx_state, parity);
                ELSE();
                    SETVAL(rx_state, stopbit);
                    SETVAL(rx_stop_cnt, rx_nstop);
                ENDIF();
            ELSE();
                SETVAL(rx_frame_cnt, INC(rx_frame_cnt));
            ENDIF();
            ENDCASE();
        CASE (parity);
            SETVAL(comb.v_par, XORx(8, &BIT(rx_shift, 7),
                                       &BIT(rx_shift, 6),
                                       &BIT(rx_shift, 5),
                                       &BIT(rx_shift, 4),
                                       &BIT(rx_shift, 3),
                                       &BIT(rx_shift, 2),
                                       &BIT(rx_shift, 1),
                                       &BIT(rx_shift, 0)));
            IF (EQ(comb.v_par, i_rd));
                SETZERO(err_parity);
            ELSE();
                SETONE(err_parity);
            ENDIF();

            TEXT();
            SETVAL(rx_state, stopbit);
            ENDCASE();
        CASE (stopbit);
            IF (EZ(i_rd));
                SETONE(err_stopbit);
            ELSE();
                SETZERO(err_stopbit);
            ENDIF();

            TEXT();
            IF (EZ(rx_stop_cnt));
                IF (EZ(comb.v_rx_fifo_full));
                    SETONE(comb.v_rx_fifo_we);
                ENDIF();
                SETVAL(rx_state, idle);
            ELSE();
                SETZERO(rx_stop_cnt);
            ENDIF();
            ENDCASE();
        CASEDEF();
            ENDCASE();
        ENDSWITCH();
    ENDIF();

TEXT();
    TEXT("Registers access:");
    SWITCH (BITS(wb_req_addr, 11, 2));
    CASE (CONST("0", 10), "0x00: txdata");
        SETBIT(comb.vb_rdata, 31, comb.v_tx_fifo_full);
        IF (NZ(w_req_valid));
            IF (NZ(w_req_write));
                SETVAL(comb.v_tx_fifo_we, AND2_L(INV_L(comb.v_tx_fifo_full), INV_L(tx_amo_guard)));
            ELSE();
                SETVAL(tx_amo_guard, comb.v_tx_fifo_full, "skip next write");
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (CONST("1", 10), "0x04: rxdata");
        SETBIT(comb.vb_rdata, 31, comb.v_rx_fifo_empty);
        SETBITS(comb.vb_rdata, 7, 0, comb.vb_rx_fifo_rdata); 
        IF (NZ(w_req_valid));
            IF (NZ(w_req_write));
                TEXT("do nothing:");
            ELSE();
                SETVAL(comb.v_rx_fifo_re, INV(comb.v_rx_fifo_empty));
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (CONST("2", 10), "0x08: txctrl");
        SETBIT(comb.vb_rdata, 0, tx_ena, "[0] tx ena");
        SETBIT(comb.vb_rdata, 1, tx_nstop, "[1] Number of stop bits");
        SETBIT(comb.vb_rdata, 2, tx_par, "[2] parity bit enable");
        SETBITS(comb.vb_rdata, 18, 16, BITS(tx_irq_thresh, 2, 0), "[18:16] FIFO threshold to raise irq");
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            SETVAL(tx_ena, BIT(wb_req_wdata, 0));
            SETVAL(tx_nstop, BIT(wb_req_wdata, 1));
            SETVAL(tx_par, BIT(wb_req_wdata, 2));
            SETVAL(tx_irq_thresh, BITS(wb_req_wdata, 18, 16));
        ENDIF();
        ENDCASE();
    CASE (CONST("3", 10), "0x0C: rxctrl");
        SETBIT(comb.vb_rdata, 0, rx_ena, "[0] txena");
        SETBIT(comb.vb_rdata, 1, rx_nstop, "[1] Number of stop bits");
        SETBIT(comb.vb_rdata, 2, rx_par);
        SETBITS(comb.vb_rdata, 18, 16, BITS(rx_irq_thresh, 2, 0));
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            SETVAL(rx_ena, BIT(wb_req_wdata, 0));
            SETVAL(rx_nstop, BIT(wb_req_wdata, 1));
            SETVAL(rx_par, BIT(wb_req_wdata, 2));
            SETVAL(rx_irq_thresh, BITS(wb_req_wdata, 18, 16));
        ENDIF();
        ENDCASE();
    CASE (CONST("4", 10), "0x10: ie");
        SETBIT(comb.vb_rdata, 0, tx_ie);
        SETBIT(comb.vb_rdata, 1, rx_ie);
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            SETVAL(tx_ie, BIT(wb_req_wdata, 0));
            SETVAL(rx_ie, BIT(wb_req_wdata, 1));
        ENDIF();
        ENDCASE();
    CASE (CONST("5", 10), "0x14: ip");
        SETBIT(comb.vb_rdata, 0, tx_ip);
        SETBIT(comb.vb_rdata, 1, rx_ip);
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            SETVAL(tx_ip, BIT(wb_req_wdata, 0));
            SETVAL(rx_ip, BIT(wb_req_wdata, 1));
        ENDIF();
        ENDCASE();
    CASE (CONST("6", 10), "0x18: scaler");
        SETVAL(comb.vb_rdata, scaler);
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            SETVAL(scaler, BITS(wb_req_wdata, CONST("30"), sim_speedup_rate));
            SETZERO(scaler_cnt);
        ENDIF();
        ENDCASE();
    CASE (CONST("7", 10), "0x1C: fwcpuid");
        SETVAL(comb.vb_rdata, fwcpuid);
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            IF (OR2(EZ(fwcpuid), EZ(wb_req_wdata)));
                SETVAL(fwcpuid, wb_req_wdata);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    IF (NZ(comb.v_rx_fifo_we));
        SETVAL(rx_wr_cnt, INC(rx_wr_cnt));
        SETVAL(rx_byte_cnt, INC(rx_byte_cnt));
        SETARRITEM(rx_fifo, TO_INT(rx_wr_cnt), rx_fifo, BITS(rx_shift, 7, 0));
    ELSIF (NZ(comb.v_rx_fifo_re));
        SETVAL(rx_rd_cnt, INC(rx_rd_cnt));
        SETVAL(rx_byte_cnt, DEC(rx_byte_cnt));
    ENDIF();
    IF (NZ(comb.v_tx_fifo_we));
        SETVAL(tx_wr_cnt, INC(tx_wr_cnt));
        SETVAL(tx_byte_cnt, INC(tx_byte_cnt));
        SETARRITEM(tx_fifo, TO_INT(tx_wr_cnt), tx_fifo, BITS(wb_req_wdata, 7, 0));
    ENDIF();


TEXT();
    SETVAL(resp_valid, w_req_valid);
    SETVAL(resp_rdata, comb.vb_rdata);
    SETZERO(resp_err);

TEXT();
    SYNC_RESET();

TEXT();
    SETVAL(o_td, BIT(tx_shift, 0));
    SETVAL(o_irq, OR2_L(tx_ip, rx_ip));
}
