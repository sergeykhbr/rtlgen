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

#include "sdctrl.h"

sdctrl::sdctrl(GenObject *parent, const char *name) :
    ModuleObject(parent, "sdctrl", name),
    log2_fifosz(this, "log2_fifosz", "9"),
    fifo_dbits(this, "fifo_dbits", "8"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_xmapinfo(this, "i_xmapinfo", "APB interconnect slot information"),
    o_xcfg(this, "o_xcfg", "APB Device descriptor"),
    i_xslvi(this, "i_xslvi", "AXI input interface to access SD-card memory"),
    o_xslvo(this, "o_xslvo", "AXI output interface to access SD-card memory"),
    i_pmapinfo(this, "i_pmapinfo", "APB interconnect slot information"),
    o_pcfg(this, "o_pcfg", "APB sd-controller configuration registers descriptor"),
    i_apbi(this, "i_apbi", "APB Slave to Bridge interface"),
    o_apbo(this, "o_apbo", "APB Bridge to Slave interface"),
    o_sclk(this, "o_sclk", "1", "Clock up to 50 MHz"),
    i_cmd(this, "i_cmd", "1", "Command response;"),
    o_cmd(this, "o_cmd", "1", "Command request; DO in SPI mode"),
    o_cmd_dir(this, "o_cmd_dir", "1", "Direction bit: 1=input; 0=output"),
    i_dat0(this, "i_dat0", "1", "Data Line[0] input; DI in SPI mode"),
    o_dat0(this, "o_dat0", "1", "Data Line[0] output"),
    o_dat0_dir(this, "o_dat0_dir", "1", "Direction bit: 1=input; 0=output"),
    i_dat1(this, "i_dat1", "1", "Data Line[1] input"),
    o_dat1(this, "o_dat1", "1", "Data Line[1] output"),
    o_dat1_dir(this, "o_dat1_dir", "1", "Direction bit: 1=input; 0=output"),
    i_dat2(this, "i_dat2", "1", "Data Line[2] input"),
    o_dat2(this, "o_dat2", "1", "Data Line[2] output"),
    o_dat2_dir(this, "o_dat2_dir", "1", "Direction bit: 1=input; 0=output"),
    i_cd_dat3(this, "i_cd_dat3", "1", "Card Detect / Data Line[3] input"),
    o_cd_dat3(this, "o_cd_dat3", "1", "Card Detect / Data Line[3] output; CS output in SPI mode"),
    o_cd_dat3_dir(this, "o_cd_dat3_dir", "1", "Direction bit: 1=input; 0=output"),
    i_detected(this, "i_detected", "1"),
    i_protect(this, "i_protect", "1"),
    // params
    _sdstate0_(this, "SD-card global state:"),
    SDSTATE_RESET(this, "2", "SDSTATE_RESET", "0"),
    _initstate0_(this, "SD-card initalization state:"),
    INITSTATE_CMD0(this, "3", "INITSTATE_CMD0", "0"),
    INITSTATE_CMD8(this, "3", "INITSTATE_CMD8", "1"),
    INITSTATE_CMD41(this, "3", "INITSTATE_CMD41", "2"),
    INITSTATE_CMD11(this, "3", "INITSTATE_CMD11", "3"),
    INITSTATE_CMD2(this, "3", "INITSTATE_CMD2", "4"),
    INITSTATE_CMD3(this, "3", "INITSTATE_CMD3", "5"),
    INITSTATE_ERROR(this, "3", "INITSTATE_ERROR", "6"),
    INITSTATE_DONE(this, "3", "INITSTATE_DONE", "7"),
    _state0_(this, "SPI states"),
    idle(this, "3", "idle", "0"),
    wait_edge(this, "3", "wait_edge", "1"),
    send_data(this, "3", "send_data", "2"),
    recv_data(this, "3", "recv_data", "3"),
    recv_sync(this, "3", "recv_sync", "4"),
    ending(this, "3", "ending", "5"),
    // signals
    w_preq_valid(this, "w_preq_valid", "1"),
    wb_preq_addr(this, "wb_preq_addr", "32"),
    w_preq_write(this, "w_preq_write", "1"),
    wb_preq_wdata(this, "wb_preq_wdata", "32"),
    w_mem_req_valid(this, "w_mem_req_valid", "1"),
    wb_mem_req_addr(this, "wb_mem_req_addr", "CFG_SYSBUS_ADDR_BITS"),
    wb_mem_req_size(this, "wb_mem_req_size", "8"),
    w_mem_req_write(this, "w_mem_req_write", "1"),
    wb_mem_req_wdata(this, "wb_mem_req_wdata", "CFG_SYSBUS_DATA_BITS"),
    wb_mem_req_wstrb(this, "wb_mem_req_wstrb", "CFG_SYSBUS_DATA_BYTES"),
    w_mem_req_last(this, "w_mem_req_last", "1"),
    w_mem_req_ready(this, "w_mem_req_ready", "1"),
    w_mem_resp_valid(this, "w_mem_resp_valid", "1"),
    wb_mem_resp_rdata(this, "wb_mem_resp_rdata", "CFG_SYSBUS_DATA_BITS"),
    wb_mem_resp_err(this, "wb_mem_resp_err", "1"),
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
    scaler(this, "scaler", "32"),
    scaler_cnt(this, "scaler_cnt", "32"),
    wdog(this, "wdog", "16"),
    wdog_cnt(this, "wdog_cnt", "16"),
    generate_crc(this, "generate_crc", "1"),
    rx_ena(this, "rx_ena", "1"),
    rx_synced(this, "rx_synced", "1"),
    rx_data_block(this, "rx_data_block", "1", "0", "Wait 0xFE start data block marker"),
    level(this, "level", "1", "1"),
    cs(this, "cs", "1"),
    sdstate(this, "sdstate", "2", "SDSTATE_RESET"),
    initstate(this, "initstate", "3", "INITSTATE_CMD0"),
    state(this, "state", "3", "idle"),
    ena_byte_cnt(this, "ena_byte_cnt", "16"),
    bit_cnt(this, "bit_cnt", "3"),
    tx_val(this, "tx_val", "8"),
    rx_val(this, "rx_val", "8"),
    shiftreg(this, "shiftreg", "8", "-1"),
    rx_ready(this, "rx_ready", "1"),
    crc7(this, "crc7", "7"),
    crc16(this, "crc16", "16"),
    spi_resp(this, "spi_resp", "8"),
    txmark(this, "txmark", "log2_fifosz"),
    rxmark(this, "rxmark", "log2_fifosz"),
    presp_valid(this, "presp_valid", "1"),
    presp_rdata(this, "presp_rdata", "32"),
    presp_err(this, "presp_err", "1"),
    //
    comb(this),
    pslv0(this, "pslv0"),
    xslv0(this, "xslv0"),
    rxfifo(this, "rxfifo"),
    txfifo(this, "txfifo")
{
    Operation::start(this);

    pslv0.vid.setObjValue(&glob_pnp_cfg_->VENDOR_OPTIMITECH);
    pslv0.did.setObjValue(&glob_pnp_cfg_->OPTIMITECH_SDCTRL_REG);
    NEW(pslv0, pslv0.getName().c_str());
        CONNECT(pslv0, 0, pslv0.i_clk, i_clk);
        CONNECT(pslv0, 0, pslv0.i_nrst, i_nrst);
        CONNECT(pslv0, 0, pslv0.i_mapinfo, i_pmapinfo);
        CONNECT(pslv0, 0, pslv0.o_cfg, o_pcfg);
        CONNECT(pslv0, 0, pslv0.i_apbi, i_apbi);
        CONNECT(pslv0, 0, pslv0.o_apbo, o_apbo);
        CONNECT(pslv0, 0, pslv0.o_req_valid, w_preq_valid);
        CONNECT(pslv0, 0, pslv0.o_req_addr, wb_preq_addr);
        CONNECT(pslv0, 0, pslv0.o_req_write, w_preq_write);
        CONNECT(pslv0, 0, pslv0.o_req_wdata, wb_preq_wdata);
        CONNECT(pslv0, 0, pslv0.i_resp_valid, presp_valid);
        CONNECT(pslv0, 0, pslv0.i_resp_rdata, presp_rdata);
        CONNECT(pslv0, 0, pslv0.i_resp_err, presp_err);
    ENDNEW();

    xslv0.vid.setObjValue(&glob_pnp_cfg_->VENDOR_OPTIMITECH);
    xslv0.did.setObjValue(&glob_pnp_cfg_->OPTIMITECH_SDCTRL_MEM);
    NEW(xslv0, xslv0.getName().c_str());
        CONNECT(xslv0, 0, xslv0.i_clk, i_clk);
        CONNECT(xslv0, 0, xslv0.i_nrst, i_nrst);
        CONNECT(xslv0, 0, xslv0.i_mapinfo, i_xmapinfo);
        CONNECT(xslv0, 0, xslv0.o_cfg, o_xcfg);
        CONNECT(xslv0, 0, xslv0.i_xslvi, i_xslvi);
        CONNECT(xslv0, 0, xslv0.o_xslvo, o_xslvo);
        CONNECT(xslv0, 0, xslv0.o_req_valid, w_mem_req_valid);
        CONNECT(xslv0, 0, xslv0.o_req_addr, wb_mem_req_addr);
        CONNECT(xslv0, 0, xslv0.o_req_size, wb_mem_req_size);
        CONNECT(xslv0, 0, xslv0.o_req_write, w_mem_req_write);
        CONNECT(xslv0, 0, xslv0.o_req_wdata, wb_mem_req_wdata);
        CONNECT(xslv0, 0, xslv0.o_req_wstrb, wb_mem_req_wstrb);
        CONNECT(xslv0, 0, xslv0.o_req_last, w_mem_req_last);
        CONNECT(xslv0, 0, xslv0.i_req_ready, w_mem_req_ready);
        CONNECT(xslv0, 0, xslv0.i_resp_valid, w_mem_resp_valid);
        CONNECT(xslv0, 0, xslv0.i_resp_rdata, wb_mem_resp_rdata);
        CONNECT(xslv0, 0, xslv0.i_resp_err, wb_mem_resp_err);
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

void sdctrl::proc_comb() {
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
    SETVAL(comb.v_inv16, XOR2(BIT(crc16, 15), i_dat0));
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
    TEXT("Registers access:");
    SWITCH (sdstate);
    CASE (SDSTATE_RESET);
        SWITCH (initstate);
        CASE (INITSTATE_CMD0);
            ENDCASE();
        CASE (INITSTATE_CMD8);
            ENDCASE();
        CASE (INITSTATE_CMD41);
            ENDCASE();
        CASE (INITSTATE_CMD11);
            ENDCASE();
        CASE (INITSTATE_CMD2);
            ENDCASE();
        CASE (INITSTATE_CMD3);
            ENDCASE();
        CASE (INITSTATE_ERROR);
            ENDCASE();
        CASE (INITSTATE_DONE);
            ENDCASE();
        CASEDEF();
            ENDCASE();
        ENDSWITCH();
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();


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
        SETVAL(comb.vb_shiftreg_next, CC2(BITS(shiftreg, 6, 0), i_dat0));
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
                SETVAL(rx_synced, AND2(NZ(cs), INV(i_dat0)));
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
    SWITCH (BITS(wb_preq_addr, 11, 2));
    CASE (CONST("0x0", 10), "0x00: sckdiv");
        SETVAL(comb.vb_rdata, scaler);
        IF (AND2(NZ(w_preq_valid), NZ(w_preq_write)));
            SETVAL(scaler, BITS(wb_preq_wdata, 30, 0));
            SETZERO(scaler_cnt);
        ENDIF();
        ENDCASE();
    CASE (CONST("0x2", 10), "0x08: reserved (watchdog)");
        SETBITS(comb.vb_rdata, 15, 0, wdog);
        IF (AND2(NZ(w_preq_valid), NZ(w_preq_write)));
            SETVAL(wdog, BITS(wb_preq_wdata, 15, 0));
        ENDIF();
        ENDCASE();
    CASE (CONST("0x11", 10), "0x44: reserved 4 (txctrl)");
        SETBIT(comb.vb_rdata, 0, i_detected, "[0] sd card inserted");
        SETBIT(comb.vb_rdata, 1, i_protect, "[1] write protect");
        SETBIT(comb.vb_rdata, 2, i_dat0, "[2] miso data bit");
        SETBITS(comb.vb_rdata, 6, 4, state, "[6:4] state machine");
        SETBIT(comb.vb_rdata, 7, generate_crc, "[7] Compute and generate CRC as the last Tx byte");
        SETBIT(comb.vb_rdata, 8, rx_ena, "[8] Receive data and write into FIFO only if rx_synced");
        SETBIT(comb.vb_rdata, 9, rx_synced, "[9] rx_ena=1 and start bit received");
        SETBIT(comb.vb_rdata, 10, rx_data_block, "[10] rx_data_block=1 receive certain template byte");
        SETBITS(comb.vb_rdata, 31, 16, ena_byte_cnt, "[31:16] Number of bytes to transmit");
        IF (AND2(NZ(w_preq_valid), NZ(w_preq_write)));
            SETVAL(generate_crc, BIT(wb_preq_wdata, 7));
            SETVAL(rx_ena, BIT(wb_preq_wdata, 8));
            SETVAL(rx_synced, BIT(wb_preq_wdata, 9));
            SETVAL(rx_data_block, BIT(wb_preq_wdata, 10));
            SETVAL(ena_byte_cnt, BITS(wb_preq_wdata, 31, 16));
        ENDIF();
        ENDCASE();
    CASE (CONST("0x12", 10), "0x48: Tx FIFO Data");
        SETBIT(comb.vb_rdata, 31, AND_REDUCE(wb_txfifo_count));
        IF (NZ(w_preq_valid));
            IF (NZ(w_preq_write));
                SETVAL(comb.v_txfifo_we, CONST("1", 1));
                SETVAL(comb.vb_txfifo_wdata, BITS(wb_preq_wdata, 7, 0));
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (CONST("0x13", 10), "0x4C: Rx FIFO Data");
        SETBITS(comb.vb_rdata, 7, 0, wb_rxfifo_rdata); 
        SETBIT(comb.vb_rdata, 31, INV(OR_REDUCE(wb_rxfifo_count)));
        IF (NZ(w_preq_valid));
            IF (NZ(w_preq_write));
                TEXT("do nothing:");
            ELSE();
                SETVAL(comb.v_rxfifo_re, CONST("1", 1));
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (CONST("0x14", 10), "0x50: Tx FIFO Watermark");
        SETBITS(comb.vb_rdata, DEC(log2_fifosz), CONST("0"), txmark);
        IF (NZ(w_preq_valid));
            IF (NZ(w_preq_write));
                SETVAL(txmark, BITS(wb_preq_wdata, DEC(log2_fifosz), CONST("0")));
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (CONST("0x15", 10), "0x54: Rx FIFO Watermark");
        SETBITS(comb.vb_rdata, DEC(log2_fifosz), CONST("0"), rxmark);
        IF (NZ(w_preq_valid));
            IF (NZ(w_preq_write));
                SETVAL(rxmark, BITS(wb_preq_wdata, DEC(log2_fifosz), CONST("0")));
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (CONST("0x16", 10), "0x58: CRC16 value (reserved FU740)");
        SETBITS(comb.vb_rdata, 15, 0, crc16);
        IF (NZ(w_preq_valid));
            IF (NZ(w_preq_write));
                SETVAL(crc16, BITS(wb_preq_wdata, 15, 0));
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
    SETVAL(presp_valid, w_preq_valid);
    SETVAL(presp_rdata, comb.vb_rdata);
    SETZERO(presp_err);

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_sclk, AND2_L(level, cs));
    SETVAL(o_cmd, OR2(rx_ena, BIT(shiftreg, 7)));
    SETVAL(o_cd_dat3, INV(cs));

    TEXT("Direction bits:");
    SETZERO(o_cmd_dir);
    SETONE(o_dat0_dir);
    SETONE(o_dat1_dir);
    SETONE(o_dat2_dir);
    SETZERO(o_cd_dat3_dir);
    TEXT("Memory request:");
    SETONE(w_mem_req_ready);
    SETONE(w_mem_resp_valid);
    SETVAL(wb_mem_resp_rdata, ALLONES());
    SETZERO(wb_mem_resp_err);

}
