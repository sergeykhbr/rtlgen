// 
//  Copyright 2025 Sergey Khabarov, sergeykhbr@gmail.com
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

#include "apb_i2c.h"

apb_i2c::apb_i2c(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "apb_i2c", name, comment),
    log2_fifosz(this, "log2_fifosz", "9", NO_COMMENT),
    fifo_dbits(this, "fifo_dbits", "8"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_mapinfo(this, "i_mapinfo", "interconnect slot information"),
    o_cfg(this, "o_cfg", "Device descriptor"),
    i_apbi(this, "i_apbi", "APB  Slave to Bridge interface"),
    o_apbo(this, "o_apbo", "APB Bridge to Slave interface"),
    o_scl(this, "o_scl", "1", "Clock output upto 400 kHz (default 100 kHz)"),
    o_sda(this, "o_sda", "1", "Data output"),
    o_sda_dir(this, "o_sda_dir", "1", "Data tri-state buffer control"),
    i_sda(this, "i_sda", "1"),
    // params
    _state0_(this, "SPI states"),
    STATE_IDLE(this, "STATE_IDLE", "7", "0x00", NO_COMMENT),
    STATE_HEADER(this, "STATE_HEADER", "7", "0x01", NO_COMMENT),
    STATE_ACK_HEADER(this, "STATE_ACK_HEADER", "7", "0x02", NO_COMMENT),
    STATE_RX_DATA(this, "STATE_RX_DATA", "7", "0x04", NO_COMMENT),
    STATE_ACK_DATA(this, "STATE_ACK_DATA", "7", "0x08", NO_COMMENT),
    STATE_TX_DATA(this, "STATE_TX_DATA", "7", "0x10", NO_COMMENT),
    STATE_WAIT_ACK_DATA(this, "STATE_WAIT_ACK_DATA", "7", "0x20", NO_COMMENT),
    STATE_STOP(this, "STATE_STOP", "7", "0x40", NO_COMMENT),
    // signals
    w_req_valid(this, "w_req_valid", "1"),
    wb_req_addr(this, "wb_req_addr", "32"),
    w_req_write(this, "w_req_write", "1"),
    wb_req_wdata(this, "wb_req_wdata", "32"),
    _rx0_(this, "Rx FIFO signals:"),
    w_rxfifo_we(this, "w_rxfifo_we", "1"),
    wb_rxfifo_wdata(this, "wb_rxfifo_wdata", "8"),
    w_rxfifo_re(this, "w_rxfifo_re", "1"),
    wb_rxfifo_rdata(this, "wb_rxfifo_rdata", "8"),
    wb_rxfifo_count(this, "wb_rxfifo_count", "ADD(log2_fifosz,1)"),
    _tx0_(this, "Tx FIFO signals:"),
    w_txfifo_we(this, "w_txfifo_we", "1"),
    wb_txfifo_wdata(this, "wb_txfifo_wdata", "8"),
    w_txfifo_re(this, "w_txfifo_re", "1"),
    wb_txfifo_rdata(this, "wb_txfifo_rdata", "8"),
    wb_txfifo_count(this, "wb_txfifo_count", "ADD(log2_fifosz,1)"),
    // registers
    scaler(this, "scaler", "32", "'0", NO_COMMENT),
    scaler_cnt(this, "scaler_cnt", "32", "'0", NO_COMMENT),
    wdog(this, "wdog", "16", "'0", NO_COMMENT),
    wdog_cnt(this, "wdog_cnt", "16", "'0", NO_COMMENT),
    generate_crc(this, "generate_crc", "1"),
    rx_ena(this, "rx_ena", "1"),
    rx_synced(this, "rx_synced", "1"),
    rx_data_block(this, "rx_data_block", "1", "0", "Wait 0xFE start data block marker"),
    level(this, "level", "1", "1"),
    cs(this, "cs", "1"),
    state(this, "state", "3", "idle"),
    ena_byte_cnt(this, "ena_byte_cnt", "16", "'0", NO_COMMENT),
    bit_cnt(this, "bit_cnt", "3", "'0", NO_COMMENT),
    tx_val(this, "tx_val", "8", "'0", NO_COMMENT),
    rx_val(this, "rx_val", "8", "'0", NO_COMMENT),
    shiftreg(this, "shiftreg", "8", "'1"),
    rx_ready(this, "rx_ready", "1"),
    crc7(this, "crc7", "7", "'0", NO_COMMENT),
    crc16(this, "crc16", "16", "'0", NO_COMMENT),
    spi_resp(this, "spi_resp", "8", "'0", NO_COMMENT),
    txmark(this, "txmark", "log2_fifosz", "'0", NO_COMMENT),
    rxmark(this, "rxmark", "log2_fifosz", "'0", NO_COMMENT),
    resp_valid(this, "resp_valid", "1"),
    resp_rdata(this, "resp_rdata", "32", "'0", NO_COMMENT),
    resp_err(this, "resp_err", "1"),
    //
    comb(this),
    pslv0(this, "pslv0", NO_COMMENT),
    rxfifo(this, "rxfifo", NO_COMMENT),
    txfifo(this, "txfifo", NO_COMMENT)
{
    Operation::start(this);

    pslv0.vid.setObjValue(&glob_pnp_cfg_->VENDOR_OPTIMITECH);
    pslv0.did.setObjValue(&glob_pnp_cfg_->OPTIMITECH_I2C);
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

TEXT();
    rxfifo.dbits.setObjValue(&fifo_dbits);
    rxfifo.log2_depth.setObjValue(&log2_fifosz);
    NEW(rxfifo, rxfifo.getName().c_str());
        CONNECT(rxfifo, 0, rxfifo.i_clk, i_clk);
        CONNECT(rxfifo, 0, rxfifo.i_nrst, i_nrst);
        CONNECT(rxfifo, 0, rxfifo.i_we, w_rxfifo_we);
        CONNECT(rxfifo, 0, rxfifo.i_wdata, wb_rxfifo_wdata);
        CONNECT(rxfifo, 0, rxfifo.i_re, w_rxfifo_re);
        CONNECT(rxfifo, 0, rxfifo.o_rdata, wb_rxfifo_rdata);
        CONNECT(rxfifo, 0, rxfifo.o_count, wb_rxfifo_count);
    ENDNEW();

TEXT();
    txfifo.dbits.setObjValue(&fifo_dbits);
    txfifo.log2_depth.setObjValue(&log2_fifosz);
    NEW(txfifo, txfifo.getName().c_str());
        CONNECT(txfifo, 0, txfifo.i_clk, i_clk);
        CONNECT(txfifo, 0, txfifo.i_nrst, i_nrst);
        CONNECT(txfifo, 0, txfifo.i_we, w_txfifo_we);
        CONNECT(txfifo, 0, txfifo.i_wdata, wb_txfifo_wdata);
        CONNECT(txfifo, 0, txfifo.i_re, w_txfifo_re);
        CONNECT(txfifo, 0, txfifo.o_rdata, wb_txfifo_rdata);
        CONNECT(txfifo, 0, txfifo.o_count, wb_txfifo_count);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void apb_i2c::proc_comb() {
    TEXT("system bus clock scaler to baudrate:");
    IF (NZ(scaler));
        IF (EQ(scaler_cnt, DEC(scaler)));
            SETZERO(scaler_cnt);
            SETVAL(level, INV(level));
            SETVAL(comb.v_posedge, INV(level));
            SETVAL(comb.v_negedge, (level));
        ELSE();
            SETVAL(scaler_cnt, INC(scaler_cnt));
        ENDIF();
    ENDIF();

TEXT();
    IF(EZ(rx_ena));
        SETVAL(comb.vb_shiftreg_next, CC2(BITS(shiftreg, 6, 0), CONST("1", 1)));
    ELSE();
        SETVAL(comb.vb_shiftreg_next, CC2(BITS(shiftreg, 6, 0), i_sda));
    ENDIF();
    IF (NZ(cs));
        IF(ORx(2, &AND2(NZ(comb.v_negedge), EZ(rx_ena)),
                  &AND2(NZ(comb.v_posedge), NZ(rx_ena))));
            SETVAL(shiftreg, comb.vb_shiftreg_next);
        ENDIF();
    ENDIF();


TEXT();
    TEXT("Transmitter's state machine:");
    SWITCH (state);
    CASE(STATE_IDLE);
        IF (NZ(ena_byte_cnt));
        ENDIF();
    ENDCASE();

    TEXT();
    CASE(STATE_HEADER);
        IF(NZ(comb.v_negedge));
        ENDIF();
    ENDCASE();

    TEXT();
    CASE(STATE_ACK_HEADER);
    ENDCASE();

    TEXT();
    CASE(STATE_RX_DATA);
        SETVAL(state, STATE_ACK_DATA);
    ENDCASE();

    TEXT();
    CASE(STATE_ACK_DATA);
        SETVAL(state, STATE_STOP);
    ENDCASE();

    TEXT();
    CASE (STATE_TX_DATA);
        SETVAL(state, STATE_WAIT_ACK_DATA);
    ENDCASE();

    TEXT();
    CASE (STATE_WAIT_ACK_DATA);
        SETVAL(state, STATE_STOP);
    ENDCASE();

    TEXT();
    CASE (STATE_STOP);
        SETVAL(state, STATE_IDLE);
    ENDCASE();

    TEXT();
    CASEDEF();
        SETVAL(state, STATE_IDLE);
    ENDCASE();
    ENDSWITCH();

TEXT();
    TEXT("Registers access:");
    SWITCH (BITS(wb_req_addr, 11, 2));
    CASE (CONST("0x0", 10), "0x00: scldiv");
        SETVAL(comb.vb_rdata, scaler);
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            SETVAL(scaler, BITS(wb_req_wdata, 30, 0));
            SETZERO(scaler_cnt);
        ENDIF();
        ENDCASE();
    CASE (CONST("0x1", 10), "0x04: status");
        SETBIT(comb.vb_rdata, 0, i_sda, "[0] input SDA data bit");
        SETBITS(comb.vb_rdata, 7, 1, state, "[7:1] state machine");
        ENDCASE();
    CASE (CONST("0x2", 10), "0x8: Tx FIFO Data");
        SETBIT(comb.vb_rdata, 31, AND_REDUCE(wb_txfifo_count));
        IF (NZ(w_req_valid));
            IF (NZ(w_req_write));
                SETVAL(comb.v_txfifo_we, CONST("1", 1));
                SETVAL(comb.vb_txfifo_wdata, BITS(wb_req_wdata, 7, 0));
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (CONST("0x3", 10), "0xC: Rx FIFO Data");
        SETBITS(comb.vb_rdata, 7, 0, wb_rxfifo_rdata); 
        SETBIT(comb.vb_rdata, 31, INV(OR_REDUCE(wb_rxfifo_count)));
        IF (NZ(w_req_valid));
            IF (NZ(w_req_write));
                TEXT("do nothing:");
            ELSE();
                SETVAL(comb.v_rxfifo_re, CONST("1", 1));
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (CONST("0x4", 10), "0x10: Tx FIFO Watermark");
        SETBITS(comb.vb_rdata, DEC(log2_fifosz), CONST("0"), txmark);
        IF (NZ(w_req_valid));
            IF (NZ(w_req_write));
                SETVAL(txmark, BITS(wb_req_wdata, DEC(log2_fifosz), CONST("0")));
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (CONST("0x5", 10), "0x14: Rx FIFO Watermark");
        SETBITS(comb.vb_rdata, DEC(log2_fifosz), CONST("0"), rxmark);
        IF (NZ(w_req_valid));
            IF (NZ(w_req_write));
                SETVAL(rxmark, BITS(wb_req_wdata, DEC(log2_fifosz), CONST("0")));
            ENDIF();
        ENDIF();
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    SETVAL(w_rxfifo_we, rx_ready);
    SETVAL(wb_rxfifo_wdata, rx_val);
    SETVAL(w_rxfifo_re, comb.v_rxfifo_re);

TEXT();
    SETVAL(w_txfifo_we, comb.v_txfifo_we);
    SETVAL(wb_txfifo_wdata, comb.vb_txfifo_wdata);
    SETVAL(w_txfifo_re, comb.v_txfifo_re);

TEXT();
    SETVAL(resp_valid, w_req_valid);
    SETVAL(resp_rdata, comb.vb_rdata);
    SETZERO(resp_err);

TEXT();
    SYNC_RESET();

TEXT();
    SETVAL(o_scl, AND2_L(level, cs));
    SETVAL(o_sda, OR2(rx_ena, BIT(shiftreg, 7)));
    SETVAL(o_sda_dir, INV(cs));
}
