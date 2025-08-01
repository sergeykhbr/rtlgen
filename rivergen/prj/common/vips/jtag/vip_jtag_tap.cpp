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

#include "vip_jtag_tap.h"

vip_jtag_tap::vip_jtag_tap(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "vip_jtag_tap", name, comment),
    // Ports
    i_nrst(this, "i_nrst", "1", NO_COMMENT),
    i_tck(this, "i_tck", "1", NO_COMMENT),
    i_req_valid(this, "i_req_valid", "1", "request"),
    i_req_irlen(this, "i_req_irlen", "4", "irlen in an range 1..15: ARM=4"),
    i_req_ir(this, "i_req_ir", "16", "ir reg"),
    i_req_drlen(this, "i_req_drlen", "7", "drlen in an range 1..64: ARM=35/32"),
    i_req_dr(this, "i_req_dr", "64", "dr reg"),
    o_resp_valid(this, "o_resp_valid", "1", "response valid"),
    o_resp_data(this, "o_resp_data", "64", "response data"),
    o_trst(this, "o_trst", "1", "Must be open-train, pullup"),
    o_tck(this, "o_tck", "1"),
    o_tms(this, "o_tms", "1"),
    o_tdo(this, "o_tdo", "1"),
    i_tdi(this, "i_tdi", "1"),
    // param
    _scan0_(this),
    _scan1_(this, "JTAG states:"),
    RESET_TAP(this, "RESET_TAP", "4", "0", NO_COMMENT),
    IDLE(this, "IDLE", "4", "1", NO_COMMENT),
    SELECT_DR_SCAN1(this, "SELECT_DR_SCAN1", "4", "2", NO_COMMENT),
    SELECT_DR_SCAN(this, "SELECT_DR_SCAN", "4", "3", NO_COMMENT),
    CAPTURE_DR(this, "CAPTURE_DR", "4", "4", NO_COMMENT),
    SHIFT_DR(this, "SHIFT_DR", "4", "5", NO_COMMENT),
    EXIT1_DR(this, "EXIT1_DR", "4", "6", NO_COMMENT),
    UPDATE_DR(this, "UPDATE_DR", "4", "7", NO_COMMENT),
    SELECT_IR_SCAN(this, "SELECT_IR_SCAN", "4", "8", NO_COMMENT),
    CAPTURE_IR(this, "CAPTURE_IR", "4", "9", NO_COMMENT),
    SHIFT_IR(this, "SHIFT_IR", "4", "10", NO_COMMENT),
    EXIT1_IR(this, "EXIT1_IR", "4", "11", NO_COMMENT),
    UPDATE_IR(this, "UPDATE_IR", "4", "12", NO_COMMENT),
    INIT_RESET(this, "INIT_RESET", "4", "15", NO_COMMENT),
    w_tck(this, "w_tck", "1", RSTVAL_ZERO, NO_COMMENT),
    text0(this, "posedge registers:"),
    req_valid(this, &i_tck, CLK_POSEDGE, &i_nrst, ACTIVE_LOW, "req_valid", "1", RSTVAL_ZERO, NO_COMMENT),
    req_irlen(this, &i_tck, CLK_POSEDGE, &i_nrst, ACTIVE_LOW, "req_irlen", "4", "0x1", NO_COMMENT),
    req_drlen(this, &i_tck, CLK_POSEDGE, &i_nrst, ACTIVE_LOW, "req_drlen", "7", "0x1", NO_COMMENT),
    req_ir(this, &i_tck, CLK_POSEDGE, &i_nrst, ACTIVE_LOW, "req_ir", "16", RSTVAL_ZERO, NO_COMMENT),
    req_dr(this, &i_tck, CLK_POSEDGE, &i_nrst, ACTIVE_LOW, "req_dr", "64", RSTVAL_ZERO, NO_COMMENT),
    state(this, &i_tck, CLK_POSEDGE, &i_nrst, ACTIVE_LOW, "state", "4", "INIT_RESET", NO_COMMENT),
    trst(this, &i_tck, CLK_NEGEDGE, &i_nrst, ACTIVE_LOW, "trst", "1", RSTVAL_ZERO, NO_COMMENT),
    tms(this, &i_tck, CLK_NEGEDGE, &i_nrst, ACTIVE_LOW, "tms", "1", "1", NO_COMMENT),
    tdo(this, &i_tck, CLK_NEGEDGE, &i_nrst, ACTIVE_LOW, "tdo", "1", RSTVAL_ZERO, NO_COMMENT),
    dr_length(this, &i_tck, CLK_POSEDGE, &i_nrst, ACTIVE_LOW, "dr_length", "7", "0x7F", NO_COMMENT),
    dr(this, &i_tck, CLK_POSEDGE, &i_nrst, ACTIVE_LOW, "dr", "64", "'0", NO_COMMENT),
    bypass(this, &i_tck, CLK_POSEDGE, &i_nrst, ACTIVE_LOW, "bypass", "1", RSTVAL_ZERO, NO_COMMENT),
    datacnt(this, &i_tck, CLK_POSEDGE, &i_nrst, ACTIVE_LOW, "datacnt", "32", "'0", NO_COMMENT),
    shiftreg(this, &i_tck, CLK_POSEDGE, &i_nrst, ACTIVE_LOW, "shiftreg", "64", "'0", NO_COMMENT),
    resp_valid(this, &i_tck, CLK_POSEDGE, &i_nrst, ACTIVE_LOW, "resp_valid", "1", "0", NO_COMMENT),
    resp_data(this, &i_tck, CLK_POSEDGE, &i_nrst, ACTIVE_LOW, "resp_data", "64", "'0", NO_COMMENT),
    ir(this, &i_tck, CLK_POSEDGE, &i_nrst, ACTIVE_LOW, "ir", "16", "'1", NO_COMMENT),
    comb(this)
{
    Operation::start(this);


    Operation::start(&comb);
    proc_comb();
}

void vip_jtag_tap::proc_comb() {
    SETZERO(tms);
    SETZERO(resp_valid);
    IF (NZ(req_drlen));
        SETVAL(comb.vi_dr_idx, DEC(TO_INT(req_drlen)));
    ELSE();
        SETZERO(comb.vi_dr_idx);
    ENDIF();

    TEXT();
    IF(NZ(i_req_valid));
        SETONE(req_valid);
        SETVAL(req_irlen, i_req_irlen);
        SETVAL(req_ir, i_req_ir);
        SETVAL(req_drlen, i_req_drlen);
        SETVAL(req_dr, i_req_dr);
    ENDIF();

    TEXT();
    SWITCH (state);
    CASE(INIT_RESET);
        SETONE(trst);
        SETONE(tms);
        TEXT("127 clocks to reset JTAG state machine without TRST");
        IF(NZ(dr_length));
            SETVAL(dr_length, DEC(dr_length));
        ELSE();
            SETVAL(state, RESET_TAP);
        ENDIF();
    ENDCASE();
    CASE(RESET_TAP);
        SETONE(trst);
        SETVAL(ir, ALLONES());
        SETVAL(state, IDLE);
    ENDCASE();
    CASE(IDLE);
        SETZERO(trst);
        SETZERO(resp_data);
        IF (NZ(req_valid));
            SETZERO(req_valid);
            SETONE(tms);
            SETVAL(state, SELECT_DR_SCAN1);
        ENDIF();
    ENDCASE();
    CASE(SELECT_DR_SCAN1);
        SETONE(tms);
        SETVAL(state, SELECT_IR_SCAN);
    ENDCASE();
    CASE(SELECT_IR_SCAN);
        SETVAL(state, CAPTURE_IR);
        SETVAL(ir, req_ir);
    ENDCASE();
    CASE(CAPTURE_IR);
        SETVAL(state, SHIFT_IR);
        SETVAL(comb.vb_shiftreg, ir);
        SETVAL(datacnt, req_irlen);
    ENDCASE();
    CASE(SHIFT_IR);
        IF (LE(datacnt, CONST("1", 6)));
            SETONE(tms);
            SETVAL(state, EXIT1_IR);
        ELSE();
            SETVAL(comb.vb_shiftreg, CC2(CONST("0", 1), BITS(shiftreg, 63, 1)));
            SETVAL(datacnt, DEC(datacnt));
        ENDIF();
    ENDCASE();
    CASE(EXIT1_IR);
        SETONE(tms);
        SETVAL(state, UPDATE_IR);
    ENDCASE();
    CASE(UPDATE_IR);
        SETONE(tms);
        SETVAL(state, SELECT_DR_SCAN);
    ENDCASE();
    CASE(SELECT_DR_SCAN);
        SETVAL(state, CAPTURE_DR);
    ENDCASE();
    CASE(CAPTURE_DR);
        SETVAL(comb.vb_shiftreg, req_dr);
        SETVAL(datacnt, req_drlen);
        SETVAL(state, SHIFT_DR);
    ENDCASE();
    CASE(SHIFT_DR);
        SETVAL(comb.vb_shiftreg, CC2(CONST("0", 1), BITS(shiftreg, 63, 1)));
        IF (LE(datacnt, CONST("1", 6)));
            SETONE(tms);
            SETVAL(state, EXIT1_DR);
        ELSE();
            SETVAL(datacnt, DEC(datacnt));
        ENDIF();
    ENDCASE();
    CASE(EXIT1_DR);
        SETONE(tms);
        SETVAL(state, UPDATE_DR);
    ENDCASE();
    CASE(UPDATE_DR);
        SETONE(resp_valid);
        SETVAL(resp_data, shiftreg);
        SETVAL(state, IDLE);
    ENDCASE();
    CASEDEF();
        SETVAL(state, IDLE);
    ENDCASE();
    ENDSWITCH();
    
TEXT();
    SETVAL(tdo, BIT(shiftreg, 0));
    SETBIT(comb.vb_shiftreg, comb.vi_dr_idx, i_tdi);
    SETVAL(shiftreg, comb.vb_shiftreg);
    SETVAL(o_trst, trst);
    SETVAL(o_tck, i_tck);
    SETVAL(o_tms, tms);
    SETVAL(o_tdo, tdo);
    SETVAL(o_resp_valid, resp_valid);
    SETVAL(o_resp_data, resp_data);
}
