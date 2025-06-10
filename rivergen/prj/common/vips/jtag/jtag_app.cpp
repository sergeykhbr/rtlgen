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

#include "jtag_app.h"

jtag_app::jtag_app(GenObject *parent, const char *name) :
    ModuleObject(parent, "jtag_app", name, NO_COMMENT),
    // parameters
    // Ports
    o_trst(this, "o_trst", "1", "Must be open-train, pullup"),
    o_tck(this, "o_tck", "1"),
    o_tms(this, "o_tms", "1"),
    o_tdo(this, "o_tdo", "1"),
    i_tdi(this, "i_tdi", "1"),
    w_nrst(this, "w_nrst", "1", RSTVAL_ZERO, NO_COMMENT),
    w_tck(this, "w_tck", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_clk1_cnt(this, "wb_clk1_cnt", "32", "'0", NO_COMMENT),
    w_req_valid(this, "w_req_valid", "1", NO_COMMENT),
    wb_req_irlen(this, "wb_req_irlen", "4", NO_COMMENT),
    wb_req_ir(this, "wb_req_ir", "16", NO_COMMENT),
    wb_req_drlen(this, "wb_req_drlen", "7", NO_COMMENT),
    wb_req_dr(this, "wb_req_dr", "64", NO_COMMENT),
    w_resp_valid(this, "w_resp_valid", "1", NO_COMMENT),
    wb_resp_data(this, "wb_resp_data", "64", NO_COMMENT),
    // submodules:
    clk1(this, "clk1", NO_COMMENT),
    // processes:
    comb(this),
    test_clk1(this, &w_tck),
    tap(this, "tap", NO_COMMENT) {
    Operation::start(this);

    // Create and connet Sub-modules:
    clk1.period.setObjValue(new FloatConst(80.0));  // 12.5 MHz
    NEW(clk1, clk1.getName().c_str());
        CONNECT(clk1, 0, clk1.o_clk, w_tck);
    ENDNEW();

    TEXT();
    NEW(tap, tap.getName().c_str());
        CONNECT(tap, 0, tap.i_nrst, w_nrst);
        CONNECT(tap, 0, tap.i_tck, w_tck);
        CONNECT(tap, 0, tap.i_req_valid, w_req_valid);
        CONNECT(tap, 0, tap.i_req_irlen, wb_req_irlen);
        CONNECT(tap, 0, tap.i_req_ir, wb_req_ir);
        CONNECT(tap, 0, tap.i_req_drlen, wb_req_drlen);
        CONNECT(tap, 0, tap.i_req_dr, wb_req_dr);
        CONNECT(tap, 0, tap.o_resp_valid, w_resp_valid);
        CONNECT(tap, 0, tap.o_resp_data, wb_resp_data);
        CONNECT(tap, 0, tap.o_trst, o_trst);
        CONNECT(tap, 0, tap.o_tck, o_tck);
        CONNECT(tap, 0, tap.o_tms, o_tms);
        CONNECT(tap, 0, tap.o_tdo, o_tdo);
        CONNECT(tap, 0, tap.i_tdi, i_tdi);
    ENDNEW();

    INITIAL();
        SETZERO(w_nrst);
        SETVAL_DELAY(w_nrst, CONST("1", 1), *new FloatConst(800.0));
    ENDINITIAL();


    Operation::start(&comb);
    proc_comb();

    Operation::start(&test_clk1);
    proc_test_clk1();

}

void jtag_app::proc_comb() {
}


void jtag_app::proc_test_clk1() {
    IF (EZ(w_nrst));
        SETZERO(wb_clk1_cnt);
    ELSE();
        SETVAL(wb_clk1_cnt, INC(wb_clk1_cnt));
    ENDIF();

    TEXT();
    IF (EQ(wb_clk1_cnt, CONST("500", 32)));
        SETONE(w_req_valid);
        SETVAL(wb_req_irlen, CONST("5", 4));
        SETVAL(wb_req_ir, CONST("0x1", 5));
        SETVAL(wb_req_drlen, CONST("32", 7));
        SETZERO(wb_req_dr);
    ELSIF (EQ(wb_clk1_cnt, CONST("750", 32)));
        SETONE(w_req_valid);
        SETVAL(wb_req_irlen, CONST("5", 4));
        SETVAL(wb_req_ir, CONST("0x10", 5), "0x10 DTM_CONTROL");
        SETVAL(wb_req_drlen, CONST("32", 7));
        SETVAL(wb_req_dr, CONST("0x12345678", 32));
    ELSE();
        SETZERO(w_req_valid);
    ENDIF();
}

