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

apb_uart::apb_uart(GenObject *parent, const char *name) :
    ModuleObject(parent, "apb_uart", name),
    log2_fifosz(this, "log2_fifosz", "4"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_apbi(this, "i_apbi", "APB  Slave to Bridge interface"),
    o_apbo(this, "o_apbo", "APB Bridge to Slave interface"),
    o_cfg(this, "o_cfg"),
    i_rd(this, "i_rd", "1"),
    o_td(this, "o_td", "1"),
    o_irq(this, "o_irq", "1"),
    // params
    fifosz(this, "fifosz", "POW2(1,log2_fifosz)"),
    _state0_(this, "Rx/Tx states"),
    idle(this, "3", "idle", "0"),
    startbit(this, "3", "startbit", "1"),
    data(this, "3", "data", "2"),
    parity(this, "3", "parity", "3"),
    stopbit(this, "3", "stopbit", "4"),
    // signals
    wb_tx_fifo_rdata(this, "wb_tx_fifo_rdata", "8"),
    w_req_valid(this, "w_req_valid", "1"),
    wb_req_addr(this, "wb_req_addr", "32"),
    w_req_write(this, "w_req_write", "1"),
    wb_req_wdata(this, "wb_req_wdata", "32"),
    w_resp_valid(this, "w_resp_valid", "1"),
    w_resp_err(this, "w_resp_err", "1"),
    // registers
    scaler(this, "scaler", "32"),
    scaler_cnt(this, "scaler_cnt", "32"),
    level(this, "level", "1"),
    rx_fifo(this, "rx_fifo", "8", "fifosz", true),
    rx_state(this, "rx_state", "3", "idle"),
    rx_ie(this, "rx_ie", "1"),
    rx_ip(this, "rx_ip", "1"),
    rx_wr_cnt(this, "rx_wr_cnt", "log2_fifosz"),
    rx_rd_cnt(this, "rx_rd_cnt", "log2_fifosz"),
    rx_byte_cnt(this, "rx_byte_cnt", "log2_fifosz"),
    rx_irq_thresh(this, "rx_irq_thresh", "log2_fifosz"),
    tx_fifo(this, "tx_fifo", "8", "fifosz", true),
    tx_state(this, "tx_state", "3", "idle"),
    tx_ena(this, "tx_ena", "1"),
    tx_ie(this, "tx_ie", "1"),
    tx_ip(this, "tx_ip", "1"),
    tx_nstop(this, "tx_nstop", "1"),
    tx_par(this, "tx_par", "1"),
    tx_wr_cnt(this, "tx_wr_cnt", "log2_fifosz"),
    tx_rd_cnt(this, "tx_rd_cnt", "log2_fifosz"),
    tx_byte_cnt(this, "tx_byte_cnt", "log2_fifosz"),
    tx_irq_thresh(this, "tx_irq_thresh", "log2_fifosz"),
    tx_frame_cnt(this, "tx_frame_cnt", "4"),
    tx_stop_cnt(this, "tx_stop_cnt", "1"),
    tx_shift(this, "tx_shift", "11"),
    resp_valid(this, "resp_valid", "1"),
    resp_rdata(this, "resp_rdata", "32"),
    resp_err(this, "resp_err", "1"),
    //
    comb(this),
    pslv0(this, "pslv0")
{
    Operation::start(this);

    NEW(pslv0, pslv0.getName().c_str());
        CONNECT(pslv0, 0, pslv0.i_clk, i_clk);
        CONNECT(pslv0, 0, pslv0.i_nrst, i_nrst);
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
    TEXT("system bus clock scaler to baudrate:");
    IF (NZ(scaler));
        IF (EQ(scaler_cnt, DEC(scaler)));
            SETZERO(scaler_cnt);
            SETVAL(level, INV(level));
            SETVAL(comb.posedge_flag, INV(level));
            SETVAL(comb.negedge_flag, (level));
        ELSE();
            SETVAL(scaler_cnt, INC(scaler_cnt));
        ENDIF();

        TEXT();
        IF (AND3(EQ(rx_state, idle),  NZ(i_rd), EQ(tx_state, idle)));
            SETZERO(scaler_cnt);
            SETONE(level);
        ENDIF();
    ENDIF();

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
    IF (EQ(INC(tx_wr_cnt), tx_rd_cnt));
        SETONE(comb.tx_fifo_full);
    ENDIF();

    TEXT();
    IF (EQ(tx_rd_cnt, tx_wr_cnt));
        SETONE(comb.tx_fifo_empty);
        SETZERO(tx_byte_cnt);
    ENDIF();

    TEXT("Receiver's FIFO:");
    IF (EQ(INC(rx_wr_cnt), rx_rd_cnt));
        SETONE(comb.rx_fifo_full);
    ENDIF();
 
    TEXT();
    IF (EQ(rx_rd_cnt, rx_wr_cnt));
        SETONE(comb.rx_fifo_empty);
        SETZERO(rx_byte_cnt);
    ENDIF();

TEXT();
    TEXT("Transmitter's state machine:");
    IF (NZ(comb.posedge_flag));
        SWITCH (tx_state);
        CASE(idle);
            IF (AND2(EZ(comb.tx_fifo_empty), NZ(tx_ena)));
                TEXT("stopbit=1,parity=xor,data[7:0],startbit=0");
                IF (NZ(tx_par));
                    SETVAL(comb.par, XORx(8, &BIT(wb_tx_fifo_rdata, 7),
                                             &BIT(wb_tx_fifo_rdata, 6),
                                             &BIT(wb_tx_fifo_rdata, 5),
                                             &BIT(wb_tx_fifo_rdata, 4),
                                             &BIT(wb_tx_fifo_rdata, 3),
                                             &BIT(wb_tx_fifo_rdata, 2),
                                             &BIT(wb_tx_fifo_rdata, 1),
                                             &BIT(wb_tx_fifo_rdata, 0)));
                    SETVAL(tx_shift, CC4(CONST("1", 1), comb.par, wb_tx_fifo_rdata, CONST("0", 1)));
                ELSE();
                    SETVAL(tx_shift, CC3(CONST("3", 2), wb_tx_fifo_rdata, CONST("0", 1)));
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
    SETVAL(resp_valid, w_req_valid);
    SETZERO(resp_err);

TEXT();
    SWITCH(BITS(wb_req_addr, 11, 2));
    CASE (CONST("0", 10));
        ENDCASE();
    CASE (CONST("1", 10));
        ENDCASE();
    CASE (CONST("2", 10));
        ENDCASE();
    CASE (CONST("3", 10));
        ENDCASE();
    CASEDEF();
        SETONE(resp_err);
        ENDCASE();
    ENDSWITCH();
    SETVAL(resp_rdata, comb.vb_rdata);

TEXT();
    SYNC_RESET(*this);

}
