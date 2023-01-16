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
    fifo_dbits(this, "fifo_dbits", "8"),
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
    i_protect(this, "i_protect", "1"),
    // params
    _state0_(this, "SPI states"),
    idle(this, "2", "idle", "0"),
    wait_edge(this, "2", "wait_edge", "1"),
    send_data(this, "2", "send_data", "2"),
    ending(this, "2", "ending", "3"),
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
    wb_rxfifo_count(this, "wb_rxfifo_count", "log2_fifosz"),
    _tx0_(this, "Tx FIFO signals:"),
    w_txfifo_we(this, "w_txfifo_we", "1"),
    wb_txfifo_wdata(this, "wb_txfifo_wdata", "8"),
    w_txfifo_re(this, "w_txfifo_re", "1"),
    wb_txfifo_rdata(this, "wb_txfifo_rdata", "8"),
    wb_txfifo_count(this, "wb_txfifo_count", "log2_fifosz"),
    // registers
    scaler(this, "scaler", "32"),
    scaler_cnt(this, "scaler_cnt", "32"),
    generate_crc(this, "generate_crc", "1"),
    level(this, "level", "1", "1"),
    cs(this, "cs", "1"),
    state(this, "state", "2", "idle"),
    ena_byte_cnt(this, "ena_byte_cnt", "16"),
    bit_cnt(this, "bit_cnt", "3"),
    tx_val(this, "tx_val", "8"),
    tx_shift(this, "tx_shift", "8", "-1"),
    rx_shift(this, "rx_shift", "8"),
    rx_ready(this, "rx_ready", "1"),
    crc7(this, "crc7", "7"),
    crc16(this, "crc16", "16"),
    spi_resp(this, "spi_resp", "8"),
    txmark(this, "txmark", "log2_fifosz"),
    rxmark(this, "rxmark", "log2_fifosz"),
    resp_valid(this, "resp_valid", "1"),
    resp_rdata(this, "resp_rdata", "32"),
    resp_err(this, "resp_err", "1"),
    //
    comb(this),
    pslv0(this, "pslv0"),
    rxfifo(this, "rxfifo"),
    txfifo(this, "txfifo")
{
    Operation::start(this);

    pslv0.vid.setObjValue(&glob_types_amba_->VENDOR_OPTIMITECH);
    pslv0.did.setObjValue(&glob_types_amba_->OPTIMITECH_SPI);
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
    SETVAL(comb.v_inv7, XOR2(BIT(crc7, 6), BIT(tx_shift, 7)));
    SETBIT(comb.vb_crc7, 6, BIT(crc7, 5));
    SETBIT(comb.vb_crc7, 5, BIT(crc7, 4));
    SETBIT(comb.vb_crc7, 4, BIT(crc7, 3));
    SETBIT(comb.vb_crc7, 3, XOR2(BIT(crc7, 2), comb.v_inv7));
    SETBIT(comb.vb_crc7, 2, BIT(crc7, 1));
    SETBIT(comb.vb_crc7, 1, BIT(crc7, 0));
    SETBIT(comb.vb_crc7, 0, comb.v_inv7);

    TEXT("CRC16 = x^16 + x^12 + x^5 + 1");
    SETVAL(comb.v_inv16, XOR2(BIT(crc16, 15), i_mosi));
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
    IF (AND2(NZ(comb.v_negedge), NZ(cs)));
        SETVAL(tx_shift, CC2(BITS(tx_shift, 6, 0), CONST("1", 1)));
        IF (NZ(bit_cnt));
            SETVAL(bit_cnt, DEC(bit_cnt));
        ELSE();
            SETZERO(cs);
        ENDIF();
    ENDIF();

TEXT();
    IF (NZ(comb.v_posedge));
        IF (NZ(rx_ready));
            SETZERO(rx_ready);
            SETONE(comb.v_rxfifo_we);
            SETVAL(comb.vb_rxfifo_wdata, rx_shift);
            SETZERO(rx_shift);
        ENDIF();

        TEXT();
        IF (NZ(cs));
            SETVAL(rx_shift, CC2(BITS(rx_shift, 6, 0), i_mosi));
            SETVAL(crc7, comb.vb_crc7);
            SETVAL(crc16, comb.vb_crc16);
        ENDIF();
    ENDIF();

TEXT();
    TEXT("Transmitter's state machine:");
    SWITCH (state);
    CASE(idle);
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
            SETVAL(crc7, ALLZEROS());
        ELSE();
            SETVAL(tx_val, ALLONES());
        ENDIF();
        ENDCASE();
    CASE(wait_edge);
        IF(NZ(comb.v_negedge));
            SETONE(cs);
            SETVAL(bit_cnt, CONST("7"));
            SETVAL(tx_shift, tx_val);
            SETVAL(state, send_data);
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
            SETONE(rx_ready);
        ENDIF();
        ENDCASE();
    CASE (ending);
        IF(EZ(cs));
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
    CASE (CONST("0x11", 10), "0x44: reserved 4 (txctrl)");
        SETBIT(comb.vb_rdata, 0, i_detected, "[0] sd card inserted");
        SETBIT(comb.vb_rdata, 1, i_protect, "[1] write protect");
        SETBIT(comb.vb_rdata, 2, i_mosi, "[2] mosi data bit");
        SETBITS(comb.vb_rdata, 5, 4, state, "[5:4] state machine");
        SETBIT(comb.vb_rdata, 7, generate_crc, "[7] Compute and generate CRC as the last Tx byte");
        SETBITS(comb.vb_rdata, 31, 16, ena_byte_cnt, "[31:16] Number of bytes to transmit");
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            SETVAL(generate_crc, BIT(wb_req_wdata, 7));
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
    SETVAL(w_rxfifo_we, comb.v_rxfifo_we);
    SETVAL(wb_rxfifo_wdata, comb.vb_rxfifo_wdata);
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
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_sclk, AND2_L(level, cs));
    SETVAL(o_miso, BIT(tx_shift, 7));
    SETVAL(o_cs, INV(cs));
}
