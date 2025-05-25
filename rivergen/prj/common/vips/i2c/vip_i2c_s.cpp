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

#include "vip_i2c_s.h"

vip_i2c_s::vip_i2c_s(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "vip_i2c_s", name, comment),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_scl(this, "i_scl", "1", "Clock upto 400 kHz (default 100 kHz)"),
    i_sda(this, "i_sda", "1", "Tri-state buffer output"),
    o_sda(this, "o_sda", "1", "Data output (tri-state buffer input)"),
    o_sda_dir(this, "o_sda_dir", "1", "Data to control tri-state buffer"),
    // params
    _state0_(this, "SPI states"),
    STATE_IDLE(this, "STATE_IDLE", "8", "0x00", NO_COMMENT),
    STATE_HEADER(this, "STATE_HEADER", "8", "0x02", NO_COMMENT),
    STATE_ACK_HEADER(this, "STATE_ACK_HEADER", "8", "0x04", NO_COMMENT),
    STATE_RX_DATA(this, "STATE_RX_DATA", "8", "0x08", NO_COMMENT),
    STATE_ACK_DATA(this, "STATE_ACK_DATA", "8", "0x10", NO_COMMENT),
    STATE_TX_DATA(this, "STATE_TX_DATA", "8", "0x20", NO_COMMENT),
    STATE_WAIT_ACK_DATA(this, "STATE_WAIT_ACK_DATA", "8", "0x40", NO_COMMENT),
    _t0_(this),
    PIN_DIR_INPUT(this, "PIN_DIR_INPUT", "1", "0", NO_COMMENT),
    PIN_DIR_OUTPUT(this, "PIN_DIR_OUTPUT", "1", "1", NO_COMMENT),
    // signals
    // registers
    sda(this, "sda", "1", "1"),
    scl(this, "scl", "1", "1"),
    state(this, "state", "8", "STATE_IDLE"),
    control_start(this, "control_start", "1", "0"),
    control_stop(this, "control_stop", "1", "0"),
    addr(this, "addr", "7", "'0"),
    read(this, "read", "1", "0"),
    rdata(this, "rdata", "8", "'0"),
    sda_dir(this, "sda_dir", "1", "PIN_DIR_OUTPUT", NO_COMMENT),
    txbyte(this, "txbyte", "9", "'1", "ack + data"),
    rxbyte(this, "rxbyte", "8", "'0", NO_COMMENT),
    bit_cnt(this, "bit_cnt", "3", "'0", NO_COMMENT),
    //
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void vip_i2c_s::proc_comb() {
    SETVAL(sda, i_sda);
    SETVAL(scl, i_scl);

TEXT();
    SETVAL(control_start, AND3_L(sda, INV_L(i_sda), i_scl));
    SETVAL(control_stop, AND3_L(INV_L(sda), i_sda, i_scl));
    SETVAL(comb.v_latch_data, AND2_L(INV_L(scl), i_scl));
    IF(NZ(comb.v_latch_data));
        SETVAL(rxbyte, CC2(BITS(rxbyte, 6, 0), i_sda));
    ENDIF();


TEXT();
    TEXT("Transmitter's state machine:");
    SWITCH (state);
    CASE(STATE_IDLE);
        SETZERO(txbyte);
        SETZERO(addr);
        SETZERO(read);
        SETZERO(rdata);
        SETVAL(sda_dir, PIN_DIR_INPUT);
        IF (NZ(control_start));
            TEXT("Start condition SDA goes LOW while SCL is HIGH");
            SETVAL(bit_cnt, CONST("7", 3));
            SETVAL(state, STATE_HEADER);
        ENDIF();
    ENDCASE();

    TEXT();
    CASE(STATE_HEADER);
        IF(NZ(comb.v_latch_data));
            IF (EZ(bit_cnt));
                SETVAL(sda_dir, PIN_DIR_OUTPUT);
                SETVAL(state, STATE_ACK_HEADER);
            ELSE();
                SETVAL(bit_cnt, DEC(bit_cnt));
            ENDIF();
        ENDIF();
    ENDCASE();

    TEXT();
    CASE(STATE_ACK_HEADER);
        IF(NZ(comb.v_latch_data));
            SETVAL(addr, BITS(rxbyte, 7, 1));
            SETVAL(read, BIT(rxbyte, 0));
            SETVAL(bit_cnt, CONST("7", 3));
            IF (EZ(BIT(rxbyte, 0)));
                TEXT("0=write");
                SETVAL(sda_dir, PIN_DIR_INPUT);
                SETVAL(state, STATE_RX_DATA);
            ELSE();
                TEXT("1=read");
                SETVAL(state, STATE_TX_DATA);
                SETVAL(txbyte, CONST("0x081", 9), "Some random value to transmit as 1-st byte");
            ENDIF();
        ENDIF();
    ENDCASE();

    TEXT();
    CASE(STATE_RX_DATA);
        IF(NZ(comb.v_latch_data));
            IF (EZ(bit_cnt));
                SETVAL(sda_dir, PIN_DIR_INPUT);
                SETVAL(state, STATE_ACK_DATA);
            ELSE();
                SETVAL(bit_cnt, DEC(bit_cnt));
            ENDIF();
        ENDIF();
    ENDCASE();

    TEXT();
    CASE(STATE_ACK_DATA);
        IF(NZ(comb.v_latch_data));
            SETVAL(rdata, rxbyte);
            IF (NZ(i_sda));
                TEXT("Not acked (last byte)");
                SETVAL(state, STATE_IDLE);
            ELSE();
                SETVAL(bit_cnt, CONST("7", 3));
                SETVAL(sda_dir, PIN_DIR_INPUT);
                SETVAL(state, STATE_RX_DATA);
            ENDIF();
        ENDIF();
    ENDCASE();

    TEXT();
    CASE (STATE_TX_DATA);
        IF(NZ(comb.v_latch_data));
            SETVAL(txbyte, CC2(BITS(txbyte, 7, 0), CONST("0", 1)));
            IF (EZ(bit_cnt));
                SETVAL(sda_dir, PIN_DIR_INPUT);
                SETVAL(state, STATE_WAIT_ACK_DATA);
            ELSE();
                SETVAL(bit_cnt, DEC(bit_cnt));
            ENDIF();
        ENDIF();
    ENDCASE();

    TEXT();
    CASE (STATE_WAIT_ACK_DATA);
        IF(NZ(comb.v_latch_data));
            SETVAL(sda_dir, PIN_DIR_OUTPUT);
            IF (NZ(i_sda));
                SETVAL(state, STATE_IDLE);
            ELSE();
                SETVAL(bit_cnt, CONST("7", 3));
                SETVAL(txbyte, CONST("0x85", 9), "Some random value to transmit");
                SETVAL(state, STATE_TX_DATA);
            ENDIF();
        ENDIF();
    ENDCASE();

    TEXT();
    CASEDEF();
        SETVAL(state, STATE_IDLE);
    ENDCASE();
    ENDSWITCH();

TEXT();
    IF(AND2(NE(state, STATE_IDLE), NZ(control_stop)));
        SETZERO(state);
    ENDIF();

TEXT();
    SYNC_RESET();

TEXT();
    SETVAL(o_sda, BIT(txbyte, 8));
    SETVAL(o_sda_dir, sda_dir);
}
