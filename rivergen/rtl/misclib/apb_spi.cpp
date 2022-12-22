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

#include "apb_spi.h"

apb_spi::apb_spi(GenObject *parent, const char *name) :
    ModuleObject(parent, "apb_spi", name),
    log2_fifosz(this, "log2_fifosz", "9"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_mapinfo(this, "i_mapinfo", "interconnect slot information"),
    o_cfg(this, "o_cfg", "Device descriptor"),
    i_apbi(this, "i_apbi", "APB  Slave to Bridge interface"),
    o_apbo(this, "o_apbo", "APB Bridge to Slave interface"),
    o_cs(this, "o_cs", "1"),
    o_sclk(this, "o_sclk", "1"),
    o_miso(this, "o_miso", "1"),
    i_mosi(this, "i_mosi", "1"),
    i_detected(this, "i_detected", "1"),
    // params
    fifosz(this, "fifosz", "POW2(1,log2_fifosz)"),
    _state0_(this, "SPI states"),
    idle(this, "2", "idle", "0"),
    send_data(this, "2", "send_data", "1"),
    receive_data(this, "2", "receive_data", "2"),
    // signals
    w_req_valid(this, "w_req_valid", "1"),
    wb_req_addr(this, "wb_req_addr", "32"),
    w_req_write(this, "w_req_write", "1"),
    wb_req_wdata(this, "wb_req_wdata", "32"),
    // registers
    scaler(this, "scaler", "32"),
    scaler_cnt(this, "scaler_cnt", "32"),
    level(this, "level", "1", "1"),
    rx_fifo(this, "rx_fifo", "8", "fifosz", true),
    tx_fifo(this, "tx_fifo", "8", "fifosz", true),
    state(this, "state", "2", "idle"),
    wr_cnt(this, "wr_cnt", "log2_fifosz"),
    rd_cnt(this, "rd_cnt", "log2_fifosz"),
    bit_cnt(this, "bit_cnt", "3"),
    tx_shift(this, "tx_shift", "8", "-1"),
    rx_shift(this, "rx_shift", "8"),
    spi_resp(this, "rx_shift", "8"),
    resp_valid(this, "resp_valid", "1"),
    resp_rdata(this, "resp_rdata", "32"),
    resp_err(this, "resp_err", "1"),
    //
    comb(this),
    pslv0(this, "pslv0")
{
    Operation::start(this);

    pslv0.vid.setObjValue(&glob_types_amba_->VENDOR_OPTIMITECH);
    pslv0.did.setObjValue(&glob_types_amba_->OPTIMITECH_UART);
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

void apb_spi::proc_comb() {
    //SETVAL(comb.vb_rx_fifo_rdata, ARRITEM(rx_fifo, TO_INT(rx_rd_cnt), rx_fifo));
    //SETVAL(comb.vb_tx_fifo_rdata, ARRITEM(tx_fifo, TO_INT(tx_rd_cnt), tx_fifo));


TEXT();
    TEXT("Transmitter's FIFO:");
    SETVAL(comb.vb_tx_wr_cnt_next, INC(wr_cnt));
    IF (EQ(comb.vb_tx_wr_cnt_next, rd_cnt));
        SETONE(comb.v_tx_fifo_full);
    ENDIF();

    TEXT();
    IF (EQ(rd_cnt, wr_cnt));
        SETONE(comb.v_tx_fifo_empty);
        SETZERO(bit_cnt);
    ENDIF();

    TEXT("Receiver's FIFO:");
//    SETVAL(comb.vb_rx_wr_cnt_next, INC(rx_wr_cnt));
    //IF (EQ(comb.vb_rx_wr_cnt_next, rx_rd_cnt));
        //SETONE(comb.v_rx_fifo_full);
    //ENDIF();
 
    //TEXT();
    //IF (EQ(rx_rd_cnt, rx_wr_cnt));
        //SETONE(comb.v_rx_fifo_empty);
        //SETZERO(rx_byte_cnt);
    //ENDIF();

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
    ENDIF();

TEXT();
    TEXT("Transmitter's state machine:");
    IF (NZ(comb.v_posedge_flag));
        SWITCH (state);
        CASE(idle);
            IF (EZ(comb.v_tx_fifo_empty));
                SETVAL(tx_shift, comb.vb_tx_fifo_rdata);
                SETVAL(state, send_data);
                SETVAL(rd_cnt, INC(rd_cnt));
                SETVAL(bit_cnt, CONST("7"));
            ELSE();
                SETVAL(tx_shift, ALLONES());
            ENDIF();
            ENDCASE();
        CASE(send_data);
            IF(EZ(bit_cnt));
                SETVAL(state, receive_data);
                SETVAL(bit_cnt, CONST("7"));
            ENDIF();
            ENDCASE();
        CASE (receive_data);
            IF(EZ(BITS(bit_cnt, 2, 0)));
                SETVAL(state, idle);
                SETVAL(spi_resp, rx_shift);
            ENDIF();
            ENDCASE();
        CASE (parity);
            SETVAL(tx_state, stopbit);
            ENDCASE();
        CASE (stopbit);
            IF (EZ(tx_stop_cnt));
                SETVAL(tx_state, idle);
            ELSE();
                SETZERO(tx_stop_cnt);
            ENDIF();
            ENDCASE();
        CASEDEF();
            ENDCASE();
        ENDSWITCH();
        
        TEXT();
        IF (NE(tx_state, idle));
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
            SETVAL(scaler, BITS(wb_req_wdata, 30, 0));
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
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_td, BIT(tx_shift, 0));
    SETVAL(o_irq, OR2_L(tx_ip, rx_ip));
}
