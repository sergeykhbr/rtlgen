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

jtagtap::jtagtap(GenObject *parent, const char *name) :
    ModuleObject(parent, "jtagtap", name),
    abits(this, "abits", "7"),
    irlen(this, "irlen", "5"),
    idcode(this, "idcode", "32", "0x10e31913"),
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
    IR_IDCODE(this, "irlen", "IR_IDCODE", "0x01"),
    IR_DTMCONTROL(this, "irlen", "IR_DTMCONTROL", "0x10"),
    IR_DBUS(this, "irlen", "IR_DBUS", "0x11"),
    IR_BYPASS(this, "irlen", "IR_BYPASS", "0x1f"),
    _dmi0_(this),
    DMISTAT_SUCCESS(this, "2", "DMISTAT_SUCCESS", "0x0"),
    DMISTAT_RESERVED(this, "2", "DMISTAT_RESERVED", "0x1"),
    DMISTAT_FAILED(this, "2", "DMISTAT_FAILED", "0x2"),
    DMISTAT_BUSY(this, "2", "DMISTAT_BUSY", "0x3"),
    _dtm0_(this),
    _dtm1_(this, "DTMCONTROL register bits"),
    DTMCONTROL_DMIRESET(this, "DTMCONTROL_DMIRESET", "16"),
    DTMCONTROL_DMIHARDRESET(this, "DTMCONTROL_DMIHARDRESET", "17"),
    _scan0_(this),
    _scan1_(this, "JTAG states:"),
    RESET_TAP(this, "4", "RESET_TAP", "0"),
    IDLE(this, "4", "IDLE", "1"),
    SELECT_DR_SCAN(this, "4", "SELECT_DR_SCAN", "2"),
    CAPTURE_DR(this, "4", "CAPTURE_DR", "3"),
    SHIFT_DR(this, "4", "SHIFT_DR", "4"),
    EXIT1_DR(this, "4", "EXIT1_DR", "5"),
    PAUSE_DR(this, "4", "PAUSE_DR", "6"),
    EXIT2_DR(this, "4", "EXIT2_DR", "7"),
    UPDATE_DR(this, "4", "UPDATE_DR", "8"),
    SELECT_IR_SCAN(this, "4", "SELECT_IR_SCAN", "9"),
    CAPTURE_IR(this, "4", "CAPTURE_IR", "10"),
    SHIFT_IR(this, "4", "SHIFT_IR", "11"),
    EXIT1_IR(this, "4", "EXIT1_IR", "12"),
    PAUSE_IR(this, "4", "PAUSE_IR", "13"),
    EXIT2_IR(this, "4", "EXIT2_IR", "14"),
    UPDATE_IR(this, "4", "UPDATE_IR", "15"),
    // registers
    state(this, "state", "4", "RESET_TAP"),
    dr_length(this, "dr_length", "7"),
    dr(this, "dr", "drlen", "idcode"),
    bypass(this, "bypass", "1"),
    datacnt(this, "datacnt", "32"),
    dmi_busy(this, "dmi_busy", "1"),
    err_sticky(this, "err_sticky", "2"),
    ir(this, "ir", "irlen", "IR_IDCODE"),
    dmi_addr(this, "dmi_addr", "abits"),
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
