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

#include "jtagtap.h"

jtagtap::jtagtap(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "jtagtap", name, comment),
    abits(this, "abits", "7", NO_COMMENT),
    irlen(this, "irlen", "5", NO_COMMENT),
    idcode(this, "idcode", "32", "0x10e31913", NO_COMMENT),
    // Ports
    i_trst(this, "i_trst", "1", "Must be open-train, pullup"),
    i_tck(this, "i_tck", "1"),
    i_tms(this, "i_tms", "1"),
    i_tdi(this, "i_tdi", "1"),
    o_tdo(this, "o_tdo", "1"),
    o_dmi_req_valid(this, "o_dmi_req_valid", "1"),
    o_dmi_req_write(this, "o_dmi_req_write", "1"),
    o_dmi_req_addr(this, "o_dmi_req_addr", "7"),
    o_dmi_req_data(this, "o_dmi_req_data", "32"),
    i_dmi_resp_data(this, "i_dmi_resp_data", "32"),
    i_dmi_busy(this, "i_dmi_busy", "1"),
    i_dmi_error(this, "i_dmi_error", "1"),
    o_dmi_hardreset(this, "o_dmi_hardreset", "1"),
    // param
    _ir0_(this),
    drlen(this, "drlen", "ADD(ADD(abits,32),2)"),
    IR_IDCODE(this, "IR_IDCODE", "irlen", "0x01", NO_COMMENT),
    IR_DTMCONTROL(this, "IR_DTMCONTROL", "irlen", "0x10", NO_COMMENT),
    IR_DBUS(this, "IR_DBUS", "irlen", "0x11", NO_COMMENT),
    IR_BYPASS(this, "IR_BYPASS", "irlen", "0x1f", NO_COMMENT),
    _dmi0_(this),
    DMISTAT_SUCCESS(this, "DMISTAT_SUCCESS", "2", "0x0", NO_COMMENT),
    DMISTAT_RESERVED(this, "DMISTAT_RESERVED", "2", "0x1", NO_COMMENT),
    DMISTAT_FAILED(this, "DMISTAT_FAILED", "2", "0x2", NO_COMMENT),
    DMISTAT_BUSY(this, "DMISTAT_BUSY", "2", "0x3", NO_COMMENT),
    _dtm0_(this),
    _dtm1_(this, "DTMCONTROL register bits"),
    DTMCONTROL_DMIRESET(this, "DTMCONTROL_DMIRESET", "16"),
    DTMCONTROL_DMIHARDRESET(this, "DTMCONTROL_DMIHARDRESET", "17"),
    _scan0_(this),
    _scan1_(this, "JTAG states:"),
    RESET_TAP(this, "RESET_TAP", "4", "0", NO_COMMENT),
    IDLE(this, "IDLE", "4", "1", NO_COMMENT),
    SELECT_DR_SCAN(this, "SELECT_DR_SCAN", "4", "2", NO_COMMENT),
    CAPTURE_DR(this, "CAPTURE_DR", "4", "3", NO_COMMENT),
    SHIFT_DR(this, "SHIFT_DR", "4", "4", NO_COMMENT),
    EXIT1_DR(this, "EXIT1_DR", "4", "5", NO_COMMENT),
    PAUSE_DR(this, "PAUSE_DR", "4", "6", NO_COMMENT),
    EXIT2_DR(this, "EXIT2_DR", "4", "7", NO_COMMENT),
    UPDATE_DR(this, "UPDATE_DR", "4", "8", NO_COMMENT),
    SELECT_IR_SCAN(this, "SELECT_IR_SCAN", "4", "9", NO_COMMENT),
    CAPTURE_IR(this, "CAPTURE_IR", "4", "10", NO_COMMENT),
    SHIFT_IR(this, "SHIFT_IR", "4", "11", NO_COMMENT),
    EXIT1_IR(this, "EXIT1_IR", "4", "12", NO_COMMENT),
    PAUSE_IR(this, "PAUSE_IR", "4", "13", NO_COMMENT),
    EXIT2_IR(this, "EXIT2_IR", "4", "14", NO_COMMENT),
    UPDATE_IR(this, "UPDATE_IR", "4", "15", NO_COMMENT),
    text0(this, "posedge registers:"),
    state(this, &i_tck, CLK_POSEDGE, &i_trst, ACTIVE_HIGH, "state", "4", "RESET_TAP", NO_COMMENT),
    dr_length(this, &i_tck, CLK_POSEDGE, &i_trst, ACTIVE_HIGH, "dr_length", "7", "'0", NO_COMMENT),
    dr(this, &i_tck, CLK_POSEDGE, &i_trst, ACTIVE_HIGH, "dr", "drlen", "idcode", NO_COMMENT),
    bypass(this, &i_tck, CLK_POSEDGE, &i_trst, ACTIVE_HIGH, "bypass", "1", RSTVAL_ZERO, NO_COMMENT),
    datacnt(this, &i_tck, CLK_POSEDGE, &i_trst, ACTIVE_HIGH, "datacnt", "32", "'0", NO_COMMENT),
    dmi_busy(this, &i_tck, CLK_POSEDGE, &i_trst, ACTIVE_HIGH, "dmi_busy", "1", RSTVAL_ZERO, NO_COMMENT),
    err_sticky(this, &i_tck, CLK_POSEDGE, &i_trst, ACTIVE_HIGH, "err_sticky", "2", "'0", NO_COMMENT),
    text1(this, "negedge registers:"),
    ir(this, &i_tck, CLK_NEGEDGE, &i_trst, ACTIVE_HIGH, "ir", "irlen", "IR_IDCODE", NO_COMMENT),
    dmi_addr(this, &i_tck, CLK_NEGEDGE, &i_trst, ACTIVE_HIGH, "dmi_addr", "abits", "'0", NO_COMMENT),
    comb(this)
{
    Operation::start(this);


    Operation::start(&comb);
    proc_comb();
}

void jtagtap::proc_comb() {
    SETVAL(comb.vb_dr, dr);
    SETVAL(comb.vb_err_sticky, err_sticky);

TEXT();
    SWITCH (state);
    CASE(RESET_TAP);
        SETVAL(ir, IR_IDCODE);
        IF (NZ(i_tms));
            SETVAL(state, RESET_TAP);
        ELSE();
            SETVAL(state, IDLE);
        ENDIF();
        ENDCASE();
    CASE(IDLE);
        IF (NZ(i_tms));
            SETVAL(state, SELECT_DR_SCAN);
        ELSE();
            SETVAL(state, IDLE);
        ENDIF();
        ENDCASE();
    CASE(SELECT_DR_SCAN);
        IF (NZ(i_tms));
            SETVAL(state, SELECT_IR_SCAN);
        ELSE();
            SETVAL(state, CAPTURE_DR);
        ENDIF();
        ENDCASE();
    CASE(CAPTURE_DR);
        IF (NZ(i_tms));
            SETVAL(state, EXIT1_DR);
        ELSE();
            SETVAL(state, SHIFT_DR);
        ENDIF();
        IF (EQ(ir, IR_IDCODE));
            SETVAL(comb.vb_dr, idcode);
            SETVAL(dr_length, CONST("32", 7));
        ELSIF (EQ(ir, IR_DTMCONTROL));
            SETBITS(comb.vb_dr, 31, 0, ALLZEROS());
            SETBITS(comb.vb_dr, 3, 0, CONST("0x1", 4), "version");
            SETBITS(comb.vb_dr, 9, 4, abits, "the size of the address");
            SETBITS(comb.vb_dr, 11, 10, err_sticky);
            SETVAL(dr_length, CONST("32", 7));
        ELSIF (EQ(ir, IR_DBUS));
            IF (NZ(i_dmi_error));
                SETVAL(comb.vb_err_sticky, DMISTAT_FAILED);
                SETBITS(comb.vb_dr, 1, 0, DMISTAT_FAILED);
            ELSE();
                SETBITS(comb.vb_dr, 1, 0, err_sticky);
            ENDIF();
            SETBITS(comb.vb_dr, 33, 2, i_dmi_resp_data);
            SETBITS(comb.vb_dr, DEC(ADD2(CONST("34"), abits)), CONST("34"), dmi_addr);
            SETVAL(dr_length, ADD2(abits, CONST("34", 7)));
        ELSIF (EQ(ir, IR_BYPASS));
            SETBIT(comb.vb_dr, 0, bypass);
            SETVAL(dr_length, CONST("1", 7));
        ENDIF();
        SETZERO(datacnt);
        ENDCASE();
    CASE(SHIFT_DR);
        IF (NZ(i_tms));
            SETVAL(state, EXIT1_DR);
        ELSE();
            SETVAL(state, SHIFT_DR);
        ENDIF();
        IF (GT(dr_length, CONST("1", 7)));
            TEXT("For the bypass dr_length = 1");
            SETVAL(comb.vb_dr, CC2(CONST("0", 1), BITS(dr, DEC(drlen), CONST("1"))));
            SETBIT(comb.vb_dr, DEC(TO_INT(dr_length)), i_tdi);
        ELSE();
            SETBIT(comb.vb_dr, 0, i_tdi);
        ENDIF();
        SETVAL(datacnt, INC(datacnt), "debug counter no need in rtl");
        ENDCASE();
    CASE(EXIT1_DR);
        IF (NZ(i_tms));
            SETVAL(state, UPDATE_DR);
        ELSE();
            SETVAL(state, PAUSE_DR);
        ENDIF();
        ENDCASE();
    CASE(PAUSE_DR);
        IF (NZ(i_tms));
            SETVAL(state, EXIT2_DR);
        ELSE();
            SETVAL(state, PAUSE_DR);
        ENDIF();
        ENDCASE();
    CASE(EXIT2_DR);
        IF (NZ(i_tms));
            SETVAL(state, UPDATE_DR);
        ELSE();
            SETVAL(state, SHIFT_DR);
        ENDIF();
        ENDCASE();
    CASE(UPDATE_DR);
        IF (NZ(i_tms));
            SETVAL(state, SELECT_DR_SCAN);
        ELSE();
            SETVAL(state, IDLE);
        ENDIF();
        IF (EQ(ir, IR_DTMCONTROL));
            SETVAL(comb.v_dmi_hardreset, BIT(dr, DTMCONTROL_DMIHARDRESET));
            IF (NZ(BIT(dr, DTMCONTROL_DMIRESET)));
                SETVAL(comb.vb_err_sticky, DMISTAT_SUCCESS);
            ENDIF();
        ELSIF (EQ(ir, IR_BYPASS));
            SETVAL(bypass, BIT(dr, 0));
        ELSIF (EQ(ir, IR_DBUS));
            IF (NE(err_sticky, DMISTAT_SUCCESS));
                TEXT("This operation should never result in a busy or error response.");
            ELSIF (NZ(dmi_busy));
                SETVAL(comb.vb_err_sticky, DMISTAT_BUSY);
            ELSE();
                SETVAL(comb.v_dmi_req_valid, OR_REDUCE(BITS(dr, 1, 0)));
            ENDIF();
            SETVAL(comb.v_dmi_req_write, BIT(dr, 1));
            SETVAL(comb.vb_dmi_req_data, BITS(dr, 33, 2));
            SETVAL(comb.vb_dmi_req_addr, BITS(dr, DEC(ADD2(CONST("34"), abits )), CONST("34")));

            TEXT();
            SETVAL(dmi_addr, BITS(dr, DEC(ADD2(CONST("34"), abits)), CONST("34")));
        ENDIF();
        ENDCASE();
    CASE(SELECT_IR_SCAN);
        IF (NZ(i_tms));
            SETVAL(state, RESET_TAP);
        ELSE();
            SETVAL(state, CAPTURE_IR);
        ENDIF();
        ENDCASE();
    CASE(CAPTURE_IR);
        IF (NZ(i_tms));
            SETVAL(state, EXIT1_IR);
        ELSE();
            SETVAL(state, SHIFT_IR);
        ENDIF();
        SETBITS(comb.vb_dr, DEC(irlen), CONST("2"), BITS(ir, DEC(irlen), CONST("2")));
        SETBITS(comb.vb_dr, 1, 0, CONST("0x1", 2));
        ENDCASE();
    CASE(SHIFT_IR);
        IF (NZ(i_tms));
            SETVAL(state, EXIT1_IR);
        ELSE();
            SETVAL(state, SHIFT_IR);
        ENDIF();
        SETBIT(comb.vb_dr, DEC(irlen), i_tdi);
        SETBITS(comb.vb_dr, SUB2(irlen, CONST("2")), CONST("0"), BITS(dr, DEC(irlen), CONST("1")));
        ENDCASE();
    CASE(EXIT1_IR);
        IF (NZ(i_tms));
            SETVAL(state, UPDATE_IR);
        ELSE();
            SETVAL(state, PAUSE_IR);
        ENDIF();
        ENDCASE();
    CASE(PAUSE_IR);
        IF (NZ(i_tms));
            SETVAL(state, EXIT2_IR);
        ELSE();
            SETVAL(state, PAUSE_IR);
        ENDIF();
        ENDCASE();
    CASE(EXIT2_IR);
        IF (NZ(i_tms));
            SETVAL(state, UPDATE_IR);
        ELSE();
            SETVAL(state, SHIFT_IR);
        ENDIF();
        ENDCASE();
    CASE(UPDATE_IR);
        IF (NZ(i_tms));
            SETVAL(state, SELECT_DR_SCAN);
        ELSE();
            SETVAL(state, IDLE);
        ENDIF();
        SETVAL(ir, BITS(dr, DEC(irlen), CONST("0")));
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();
    SETVAL(dr, comb.vb_dr);
    SETVAL(dmi_busy, i_dmi_busy);
    SETVAL(err_sticky, comb.vb_err_sticky);

TEXT();
    SETVAL(o_tdo, BIT(dr, 0));
    SETVAL(o_dmi_req_valid, comb.v_dmi_req_valid);
    SETVAL(o_dmi_req_write, comb.v_dmi_req_write);
    SETVAL(o_dmi_req_data, comb.vb_dmi_req_data);
    SETVAL(o_dmi_req_addr, comb.vb_dmi_req_addr);
    SETVAL(o_dmi_hardreset, comb.v_dmi_hardreset);
}
