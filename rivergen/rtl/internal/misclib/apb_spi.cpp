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

apb_spi::apb_spi(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "apb_spi", name, comment),
    log2_fifosz(this, "log2_fifosz", "9", NO_COMMENT),
    fifo_dbits(this, "fifo_dbits", "8"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_mapinfo(this, "i_mapinfo", "interconnect slot information"),
    o_cfg(this, "o_cfg", "Device descriptor"),
    i_apbi(this, "i_apbi", "APB  Slave to Bridge interface"),
    o_apbo(this, "o_apbo", "APB Bridge to Slave interface"),
    o_cs(this, "o_cs", "1"),
    o_sclk(this, "o_sclk", "1"),
    o_mosi(this, "o_mosi", "1"),
    i_miso(this, "i_miso", "1"),
    i_detected(this, "i_detected", "1"),
    i_protect(this, "i_protect", "1"),
    // params
    _state0_(this, "SPI states"),
    idle(this, "idle", "3", "0", NO_COMMENT),
    wait_edge(this, "wait_edge", "3", "1", NO_COMMENT),
    send_data(this, "send_data", "3", "2", NO_COMMENT),
    recv_data(this, "recv_data", "3", "3", NO_COMMENT),
    recv_sync(this, "recv_sync", "3", "4", NO_COMMENT),
    ending(this, "ending", "3", "5", NO_COMMENT),
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

    pslv0.vid.setObjValue(SCV_get_cfg_type(this, "VENDOR_OPTIMITECH"));
    pslv0.did.setObjValue(SCV_get_cfg_type(this, "OPTIMITECH_SDCTRL_REG"));
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

void apb_spi::proc_comb() {
    TEXT("CRC7 = x^7 + x^3 + 1");
    SETVAL(comb.v_inv7, XOR2(BIT(crc7, 6), BIT(shiftreg, 7)));
    SETBIT(comb.vb_crc7, 6, BIT(crc7, 5));
    SETBIT(comb.vb_crc7, 5, BIT(crc7, 4));
    SETBIT(comb.vb_crc7, 4, BIT(crc7, 3));
    SETBIT(comb.vb_crc7, 3, XOR2(BIT(crc7, 2), comb.v_inv7));
    SETBIT(comb.vb_crc7, 2, BIT(crc7, 1));
    SETBIT(comb.vb_crc7, 1, BIT(crc7, 0));
    SETBIT(comb.vb_crc7, 0, comb.v_inv7);

    TEXT("CRC16 = x^16 + x^12 + x^5 + 1");
    SETVAL(comb.v_inv16, XOR2(BIT(crc16, 15), i_miso));
    SETBIT(comb.vb_crc16, 15, BIT(crc16, 14));
    SETBIT(comb.vb_crc16, 14, BIT(crc16, 13));
    SETBIT(comb.vb_crc16, 13, BIT(crc16, 12));
    SETBIT(comb.vb_crc16, 12, XOR2(BIT(crc16, 11), comb.v_inv16));
    SETBIT(comb.vb_crc16, 11, BIT(crc16, 10));
    SETBIT(comb.vb_crc16, 10, BIT(crc16, 9));
    SETBIT(comb.vb_crc16, 9, BIT(crc16, 8));
    SETBIT(comb.vb_crc16, 8, BIT(crc16, 7));
    SETBIT(comb.vb_crc16, 7, BIT(crc16, 6));
    SETBIT(comb.vb_crc16, 6, BIT(crc16, 5));
    SETBIT(comb.vb_crc16, 5, XOR2(BIT(crc16, 4), comb.v_inv16));
    SETBIT(comb.vb_crc16, 4, BIT(crc16, 3));
    SETBIT(comb.vb_crc16, 3, BIT(crc16, 2));
    SETBIT(comb.vb_crc16, 2, BIT(crc16, 1));
    SETBIT(comb.vb_crc16, 1, BIT(crc16, 0));
    SETBIT(comb.vb_crc16, 0, comb.v_inv16);


TEXT();
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
        SETVAL(comb.vb_shiftreg_next, CC2(BITS(shiftreg, 6, 0), i_miso));
    ENDIF();
    IF (NZ(cs));
        IF(ORx(2, &AND2(NZ(comb.v_negedge), EZ(rx_ena)),
                  &AND2(NZ(comb.v_posedge), NZ(rx_ena))));
            SETVAL(shiftreg, comb.vb_shiftreg_next);
        ENDIF();
    ENDIF();

TEXT();
    IF (AND2(NZ(comb.v_negedge), NZ(cs)));
        IF (NZ(bit_cnt));
            IF (ORx(2, &EZ(rx_ena),
                       &AND2(NZ(rx_ena), NZ(rx_synced))));
                SETVAL(bit_cnt, DEC(bit_cnt));
            ENDIF();
        ELSE();
            SETZERO(cs);
        ENDIF();
    ENDIF();

TEXT();
    SETZERO(rx_ready);
    IF (NZ(comb.v_posedge));
        IF (AND2(NZ(cs), OR2(EZ(rx_ena), NZ(rx_synced))));
            SETVAL(crc7, comb.vb_crc7);
            SETVAL(crc16, comb.vb_crc16);
        ENDIF();
    ENDIF();

TEXT();
    TEXT("Transmitter's state machine:");
    SWITCH (state);
    CASE(idle);
        SETVAL(wdog_cnt, wdog);
        IF (NZ(ena_byte_cnt));
            SETVAL(comb.v_txfifo_re, INV(rx_ena));
            IF (OR2(EZ(wb_txfifo_count), NZ(rx_ena)));
                TEXT("FIFO is empty or RX is enabled:");
                SETVAL(tx_val, ALLONES());
            ELSE();
                SETVAL(tx_val, wb_txfifo_rdata);
            ENDIF();
            SETVAL(state, wait_edge);
            SETVAL(ena_byte_cnt, DEC(ena_byte_cnt));
            SETVAL(crc7, ALLZEROS());
        ELSE();
            SETVAL(tx_val, ALLONES());
        ENDIF();
        ENDCASE();
    CASE(wait_edge);
        IF(NZ(comb.v_negedge));
            SETONE(cs);
            SETVAL(bit_cnt, CONST("7"));
            IF (NZ(rx_ena));
                SETZERO(shiftreg);
                IF (NZ(rx_data_block));
                    SETVAL(state, recv_sync);
                ELSE();
                    SETVAL(state, recv_data);
                ENDIF();
            ELSE();
                SETVAL(shiftreg, tx_val);
                SETVAL(state, send_data);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(send_data);
        IF(AND2(EZ(bit_cnt), NZ(comb.v_posedge)));
            IF (NZ(ena_byte_cnt));
                SETONE(comb.v_txfifo_re);
                IF (EZ(wb_txfifo_count));
                    TEXT("FIFO is empty:");
                    SETVAL(tx_val, ALLONES());
                ELSE();
                    SETVAL(tx_val, wb_txfifo_rdata);
                ENDIF();
                SETVAL(state, wait_edge);
                SETVAL(ena_byte_cnt, DEC(ena_byte_cnt));
            ELSIF(NZ(generate_crc));
                SETVAL(tx_val, CC2(comb.vb_crc7, CONST("1", 1)));
                SETZERO(generate_crc);
                SETVAL(state, wait_edge);
            ELSE();
                SETVAL(state, ending);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (recv_data);
        IF (NZ(comb.v_posedge));
            IF (EZ(rx_synced));
                SETVAL(rx_synced, AND2(NZ(cs), INV(i_miso)));
                IF (NZ(wdog_cnt));
                    SETVAL(wdog_cnt, DEC(wdog_cnt));
                ELSIF(EZ(wdog));
                    TEXT("Wait Start bit infinitely");
                ELSE();
                    TEXT("Wait Start bit time is out:");
                    SETONE(rx_synced);
                ENDIF();
            ENDIF();

            TEXT("Check RX shift ready");
            IF(EZ(bit_cnt));
                IF (NZ(ena_byte_cnt));
                    SETVAL(state, wait_edge);
                    SETVAL(ena_byte_cnt, DEC(ena_byte_cnt));
                ELSE();
                    SETVAL(state, ending);
                ENDIF();
                SETONE(rx_ready);
                SETVAL(rx_val, comb.vb_shiftreg_next);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (recv_sync);
        IF (NZ(comb.v_posedge));
            IF (ORx(2, &EQ(comb.vb_shiftreg_next, CONST("0xFE", 8)),
                       &EZ(wdog_cnt)));
                SETVAL(state, ending);
                SETVAL(rx_val, comb.vb_shiftreg_next);
                SETONE(rx_ready);
                SETZERO(ena_byte_cnt);
                SETZERO(bit_cnt);
                SETZERO(crc16);
            ELSE();
                SETVAL(wdog_cnt, DEC(wdog_cnt));
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (ending);
        IF (EZ(cs));
            SETVAL(state, idle);
        ENDIF();
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    TEXT("Registers access:");
    SWITCH (BITS(wb_req_addr, 11, 2));
    CASE (CONST("0x0", 10), "0x00: sckdiv");
        SETVAL(comb.vb_rdata, scaler);
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            SETVAL(scaler, BITS(wb_req_wdata, 30, 0));
            SETZERO(scaler_cnt);
        ENDIF();
        ENDCASE();
    CASE (CONST("0x2", 10), "0x08: reserved (watchdog)");
        SETBITS(comb.vb_rdata, 15, 0, wdog);
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            SETVAL(wdog, BITS(wb_req_wdata, 15, 0));
        ENDIF();
        ENDCASE();
    CASE (CONST("0x11", 10), "0x44: reserved 4 (txctrl)");
        SETBIT(comb.vb_rdata, 0, i_detected, "[0] sd card inserted");
        SETBIT(comb.vb_rdata, 1, i_protect, "[1] write protect");
        SETBIT(comb.vb_rdata, 2, i_miso, "[2] miso data bit");
        SETBITS(comb.vb_rdata, 6, 4, state, "[6:4] state machine");
        SETBIT(comb.vb_rdata, 7, generate_crc, "[7] Compute and generate CRC as the last Tx byte");
        SETBIT(comb.vb_rdata, 8, rx_ena, "[8] Receive data and write into FIFO only if rx_synced");
        SETBIT(comb.vb_rdata, 9, rx_synced, "[9] rx_ena=1 and start bit received");
        SETBIT(comb.vb_rdata, 10, rx_data_block, "[10] rx_data_block=1 receive certain template byte");
        SETBITS(comb.vb_rdata, 31, 16, ena_byte_cnt, "[31:16] Number of bytes to transmit");
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            SETVAL(generate_crc, BIT(wb_req_wdata, 7));
            SETVAL(rx_ena, BIT(wb_req_wdata, 8));
            SETVAL(rx_synced, BIT(wb_req_wdata, 9));
            SETVAL(rx_data_block, BIT(wb_req_wdata, 10));
            SETVAL(ena_byte_cnt, BITS(wb_req_wdata, 31, 16));
        ENDIF();
        ENDCASE();
    CASE (CONST("0x12", 10), "0x48: Tx FIFO Data");
        SETBIT(comb.vb_rdata, 31, AND_REDUCE(wb_txfifo_count));
        IF (NZ(w_req_valid));
            IF (NZ(w_req_write));
                SETVAL(comb.v_txfifo_we, CONST("1", 1));
                SETVAL(comb.vb_txfifo_wdata, BITS(wb_req_wdata, 7, 0));
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (CONST("0x13", 10), "0x4C: Rx FIFO Data");
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
    CASE (CONST("0x14", 10), "0x50: Tx FIFO Watermark");
        SETBITS(comb.vb_rdata, DEC(log2_fifosz), CONST("0"), txmark);
        IF (NZ(w_req_valid));
            IF (NZ(w_req_write));
                SETVAL(txmark, BITS(wb_req_wdata, DEC(log2_fifosz), CONST("0")));
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (CONST("0x15", 10), "0x54: Rx FIFO Watermark");
        SETBITS(comb.vb_rdata, DEC(log2_fifosz), CONST("0"), rxmark);
        IF (NZ(w_req_valid));
            IF (NZ(w_req_write));
                SETVAL(rxmark, BITS(wb_req_wdata, DEC(log2_fifosz), CONST("0")));
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (CONST("0x16", 10), "0x58: CRC16 value (reserved FU740)");
        SETBITS(comb.vb_rdata, 15, 0, crc16);
        IF (NZ(w_req_valid));
            IF (NZ(w_req_write));
                SETVAL(crc16, BITS(wb_req_wdata, 15, 0));
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
    SETVAL(o_sclk, AND2_L(level, cs));
    SETVAL(o_mosi, OR2(rx_ena, BIT(shiftreg, 7)));
    SETVAL(o_cs, INV(cs));
}
