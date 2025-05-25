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
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_mapinfo(this, "i_mapinfo", "interconnect slot information"),
    o_cfg(this, "o_cfg", "Device descriptor"),
    i_apbi(this, "i_apbi", "APB  Slave to Bridge interface"),
    o_apbo(this, "o_apbo", "APB Bridge to Slave interface"),
    o_scl(this, "o_scl", "1", "Clock output upto 400 kHz (default 100 kHz)"),
    o_sda(this, "o_sda", "1", "Data output (tri-state buffer input)"),
    o_sda_dir(this, "o_sda_dir", "1", "Data to control tri-state buffer"),
    i_sda(this, "i_sda", "1", "Tri-state buffer output"),
    o_irq(this, "o_irq", "1", "Interrupt request"),
    o_nreset(this, "o_nreset", "1", "I2C slave reset. PCA9548 I2C mux must be de-asserted."),
    // params
    _state0_(this, "SPI states"),
    STATE_IDLE(this, "STATE_IDLE", "8", "0x00", NO_COMMENT),
    STATE_START(this, "STATE_START", "8", "0x01", NO_COMMENT),
    STATE_HEADER(this, "STATE_HEADER", "8", "0x02", NO_COMMENT),
    STATE_ACK_HEADER(this, "STATE_ACK_HEADER", "8", "0x04", NO_COMMENT),
    STATE_RX_DATA(this, "STATE_RX_DATA", "8", "0x08", NO_COMMENT),
    STATE_ACK_DATA(this, "STATE_ACK_DATA", "8", "0x10", NO_COMMENT),
    STATE_TX_DATA(this, "STATE_TX_DATA", "8", "0x20", NO_COMMENT),
    STATE_WAIT_ACK_DATA(this, "STATE_WAIT_ACK_DATA", "8", "0x40", NO_COMMENT),
    STATE_STOP(this, "STATE_STOP", "8", "0x80", NO_COMMENT),
    _t0_(this),
    PIN_DIR_INPUT(this, "PIN_DIR_INPUT", "1", "1", NO_COMMENT),
    PIN_DIR_OUTPUT(this, "PIN_DIR_OUTPUT", "1", "0", NO_COMMENT),
    // signals
    w_req_valid(this, "w_req_valid", "1"),
    wb_req_addr(this, "wb_req_addr", "32"),
    w_req_write(this, "w_req_write", "1"),
    wb_req_wdata(this, "wb_req_wdata", "32"),
    // registers
    scaler(this, "scaler", "16", "'0", NO_COMMENT),
    scaler_cnt(this, "scaler_cnt", "16", "'0", NO_COMMENT),
    setup_time(this, "setup_time", "16", "0x1", "Interval after negedge of the clock pulsse"),
    level(this, "level", "1", "1"),
    _a01_(this, "Connection through PCA9548 I2C multiplexer with address: 0x74/0b01110100"),
    _a0_(this, "Si570 Clock  (I2C switch position: 0), Addr = 0x5B"),
    _a1_(this, "FMC HPC      (I2C switch position: 1), Addr = 0xXX"),
    _a2_(this, "FMC LPC      (I2C switch position: 2), Addr = 0xXX"),
    _a3_(this, "IIC EEPROM   (I2C switch position: 3), Addr = 0x54"),
    _a4_(this, "SFP Module   (I2C switch position: 4), Addr = 0x50"),
    _a5_(this, "ADV7511 HDMI (I2C switch position: 5), Addr = 0x39"),
    _a6_(this, "DDR3 SODIMM  (I2C switch position: 6), Addr = 0x50, 0x18"),
    _a7_(this, "Si5324 Clock (I2C switch position: 7), Addr = 0x68"),
    addr(this, "addr", "7", "0x74", "I2C multiplexer"),
    R_nW(this, "R_nW", "1", "0", "0=Write; 1=read"),
    payload(this, "payload", "32", "'0", NO_COMMENT),
    state(this, "state", "8", "STATE_IDLE"),
    start(this, "start", "1", RSTVAL_ZERO, NO_COMMENT),
    sda_dir(this, "sda_dir", "1", "PIN_DIR_OUTPUT", NO_COMMENT),
    shiftreg(this, "shiftreg", "19", "'1", "1start+7adr+1rw+1ack+8data+ack"),
    rxbyte(this, "rxbyte", "8", "'0", NO_COMMENT),
    bit_cnt(this, "bit_cnt", "3", "'0", NO_COMMENT),
    byte_cnt(this, "byte_cnt", "4", "'0", NO_COMMENT),
    ack(this, "ack", "1", "0", NO_COMMENT),
    err_ack_header(this, "err_ack_header", "1", "0", NO_COMMENT),
    err_ack_data(this, "err_ack_data", "1", "0", NO_COMMENT),
    irq(this, "irq", "1", "0", NO_COMMENT),
    ie(this, "ie", "1", "0", NO_COMMENT),
    nreset(this, "nreset", "1", "0", "Active LOW (by default), could be any"),
    resp_valid(this, "resp_valid", "1"),
    resp_rdata(this, "resp_rdata", "32", "'0", NO_COMMENT),
    resp_err(this, "resp_err", "1"),
    //
    comb(this),
    pslv0(this, "pslv0", NO_COMMENT)
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

    Operation::start(&comb);
    proc_comb();
}

void apb_i2c::proc_comb() {
    TEXT("system bus clock scaler to baudrate:");
    IF (NZ(scaler));
        IF (EZ(state));
            SETZERO(scaler_cnt);
            SETONE(level);
        ELSIF (EQ(scaler_cnt, DEC(scaler)));
            SETZERO(scaler_cnt);
            SETVAL(level, INV(level));
        ELSE();
            TEXT("The data on the SDA line must remain stable during the");
            TEXT("HIGH period of the clock pulse.");
            SETVAL(scaler_cnt, INC(scaler_cnt));
            IF (EQ(scaler_cnt, setup_time));
                SETVAL(comb.v_change_data, INV_L(level));
                SETVAL(comb.v_latch_data, level);
            ENDIF();
        ENDIF();
    ENDIF();

TEXT();
    IF(NZ(comb.v_change_data));
        SETVAL(shiftreg, CC2(BITS(shiftreg, 17, 0), CONST("1", 1)));
    ENDIF();
    IF(NZ(comb.v_latch_data));
        SETVAL(rxbyte, CC2(BITS(rxbyte, 6, 0), i_sda));
    ENDIF();


TEXT();
    TEXT("Transmitter's state machine:");
    SWITCH (state);
    CASE(STATE_IDLE);
        SETZERO(start);
        SETVAL(shiftreg, ALLONES());
        SETVAL(sda_dir, PIN_DIR_OUTPUT);
        IF (NZ(start));
            TEXT("Start condition SDA goes LOW while SCL is HIGH");
            SETVAL(shiftreg, CCx(6, &CONST("0", 1),
                                    &addr,
                                    &R_nW,
                                    &CONST("0", 1),
                                    &BITS(payload, 7, 0),
                                    &CONST("1", 1)));
            SETVAL(payload, CC2(CONST("0", 8), BITS(payload, 31, 8)));
            SETVAL(state, STATE_START);
        ENDIF();
    ENDCASE();

    TEXT();
    CASE(STATE_START);
        IF(NZ(comb.v_change_data));
            SETVAL(bit_cnt, CONST("7", 3));
            SETVAL(state, STATE_HEADER);
        ENDIF();
    ENDCASE();

    TEXT();
    CASE(STATE_HEADER);
        IF(NZ(comb.v_change_data));
            IF (EZ(bit_cnt));
                SETVAL(sda_dir, PIN_DIR_INPUT);
                SETVAL(state, STATE_ACK_HEADER);
            ELSE();
                SETVAL(bit_cnt, DEC(bit_cnt));
            ENDIF();
        ENDIF();
    ENDCASE();

    TEXT();
    CASE(STATE_ACK_HEADER);
        IF(NZ(comb.v_latch_data));
            SETVAL(ack, i_sda);
        ENDIF();
        IF(NZ(comb.v_change_data));
            SETVAL(sda_dir, PIN_DIR_OUTPUT);
            IF (EZ(ack));
                SETVAL(bit_cnt, CONST("7", 3));
                IF (NZ(R_nW));
                    SETVAL(state, STATE_RX_DATA);
                    SETVAL(sda_dir, PIN_DIR_INPUT);
                ELSE();
                    SETVAL(state, STATE_TX_DATA);
                ENDIF();
            ELSE();
                SETONE(err_ack_header);
                SETVAL(state, STATE_STOP);
            ENDIF();
        ENDIF();
    ENDCASE();

    TEXT();
    CASE(STATE_RX_DATA);
        IF(NZ(comb.v_change_data));
            IF (EZ(bit_cnt));
                SETVAL(sda_dir, PIN_DIR_OUTPUT);
                SETVAL(byte_cnt, DEC(byte_cnt));
                SETVAL(payload, CC2(BITS(payload, 23, 0), rxbyte));
                TEXT("A master receiver must signal an end of data to the");
                TEXT("transmitter by not generating ACK on the last byte");
                IF(NZ(OR_REDUCE(BITS(byte_cnt, 3, 1))));
                    SETZERO(shiftreg);
                ELSE();
                    SETVAL(shiftreg, ALLONES());
                ENDIF();
                SETVAL(state, STATE_ACK_DATA);
            ELSE();
                SETVAL(bit_cnt, DEC(bit_cnt));
            ENDIF();
        ENDIF();
    ENDCASE();

    TEXT();
    CASE(STATE_ACK_DATA);
        IF(NZ(comb.v_change_data));
            IF (EZ(byte_cnt));
                SETVAL(state, STATE_STOP);
            ELSE();
                SETVAL(bit_cnt, CONST("7", 3));
                SETVAL(sda_dir, PIN_DIR_INPUT);
                SETVAL(state, STATE_RX_DATA);
            ENDIF();
        ENDIF();
    ENDCASE();

    TEXT();
    CASE (STATE_TX_DATA);
        IF(NZ(comb.v_change_data));
            IF (EZ(bit_cnt));
                SETZERO(shiftreg, "set LOW to generate STOP ocndition if last byte");
                SETVAL(sda_dir, PIN_DIR_INPUT);
                SETVAL(state, STATE_WAIT_ACK_DATA);
                IF (NZ(byte_cnt));
                    SETVAL(byte_cnt, DEC(byte_cnt));
                ENDIF();
            ELSE();
                SETVAL(bit_cnt, DEC(bit_cnt));
            ENDIF();
        ENDIF();
    ENDCASE();

    TEXT();
    CASE (STATE_WAIT_ACK_DATA);
        IF (NZ(comb.v_latch_data));
            SETVAL(ack, i_sda);
        ENDIF();
        IF(NZ(comb.v_change_data));
            SETVAL(sda_dir, PIN_DIR_OUTPUT);
            IF (OR2(NZ(ack), EZ(byte_cnt)));
                SETVAL(err_ack_data, ack);
                SETVAL(state, STATE_STOP);
            ELSE();
                SETVAL(bit_cnt, CONST("7", 3));
                SETVAL(shiftreg, CC2(BITS(payload, 7, 0), CONST("0x7FF", 11)));
                SETVAL(payload, CC2(CONST("0", 8), BITS(payload, 31, 8)));
                SETVAL(state, STATE_TX_DATA);
            ENDIF();
        ENDIF();
    ENDCASE();

    TEXT();
    CASE (STATE_STOP);
        IF(NZ(comb.v_latch_data));
            SETVAL(shiftreg, ALLONES());
            SETVAL(state, STATE_IDLE);
            SETVAL(irq, ie);
        ENDIF();
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
            SETVAL(scaler, BITS(wb_req_wdata, 15, 0));
            SETVAL(setup_time, BITS(wb_req_wdata, 31, 16));
            SETZERO(scaler_cnt);
        ENDIF();
    ENDCASE();
    CASE (CONST("0x1", 10), "0x04: control and status");
        SETBITS(comb.vb_rdata, 7, 0, state, "[7:0] state machine");
        SETBIT(comb.vb_rdata, 8, i_sda, "[8] input SDA data bit");
        SETBIT(comb.vb_rdata, 9, err_ack_header);
        SETBIT(comb.vb_rdata, 10, err_ack_data);
        SETBIT(comb.vb_rdata, 12, ie, "[12] Interrupt enable bit: 1=enabled");
        SETBIT(comb.vb_rdata, 13, irq, "[13] Interrupt pending bit. Clear on read.");
        SETBIT(comb.vb_rdata, 16, nreset, "[16] 0=unchanged; 1=set HIGH nreset");
        SETBIT(comb.vb_rdata, 17, nreset, "[17] 0=unchanged; 1=set LOW nreset");
        IF (NZ(w_req_valid));
            SETZERO(irq, "Reset irq on read");
            IF (NZ(w_req_write));
                SETZERO(err_ack_header);
                SETZERO(err_ack_data);
                SETVAL(ie, BIT(wb_req_wdata, 12));
                SETVAL(irq, BIT(wb_req_wdata, 13));
                IF (NZ(BIT(wb_req_wdata, 16)));
                    SETONE(nreset);
                ELSIF(NZ(BIT(wb_req_wdata, 17)));
                    SETZERO(nreset);
                ENDIF();
            ENDIF();
        ENDIF();
    ENDCASE();
    CASE (CONST("0x2", 10), "0x8: Addr");
        SETBIT(comb.vb_rdata, 31, R_nW);
        SETBITS(comb.vb_rdata, 19, 16, byte_cnt);
        SETBITS(comb.vb_rdata, 6, 0, addr);
        IF (NZ(w_req_valid));
            IF (NZ(w_req_write));
                SETVAL(R_nW, BIT(wb_req_wdata, 31));
                SETVAL(byte_cnt, BITS(wb_req_wdata, 19, 16));
                SETVAL(addr, BITS(wb_req_wdata, 6, 0));
                SETONE(start);
            ENDIF();
        ENDIF();
    ENDCASE();
    CASE (CONST("0x3", 10), "0xC: Payload");
        SETVAL(comb.vb_rdata, payload); 
        IF (NZ(w_req_valid));
            IF (NZ(w_req_write));
                SETVAL(payload, wb_req_wdata);
            ENDIF();
        ENDIF();
    ENDCASE();
    CASEDEF();
    ENDCASE();
    ENDSWITCH();

TEXT();
    SETVAL(resp_valid, w_req_valid);
    SETVAL(resp_rdata, comb.vb_rdata);
    SETZERO(resp_err);

TEXT();
    SYNC_RESET();

TEXT();
    SETVAL(o_scl, level);
    SETVAL(o_sda, BIT(shiftreg, 18));
    SETVAL(o_sda_dir, sda_dir);
    SETVAL(o_nreset, nreset);
}
