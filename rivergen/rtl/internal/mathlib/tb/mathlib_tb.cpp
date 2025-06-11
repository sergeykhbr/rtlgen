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

#include "mathlib_tb.h"

mathlib_tb::mathlib_tb(GenObject *parent, const char *name) :
    ModuleObject(parent, "mathlib_tb", name, NO_COMMENT),
    // parameters
    // Ports
    w_nrst(this, "w_nrst", "1", "1", "Power-on system reset active LOW"),
    w_clk(this, "w_clk", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_clk_cnt(this, "wb_clk_cnt", "32", "'0", NO_COMMENT),
    wb_a(this, "wb_a", "8", RSTVAL_ZERO, NO_COMMENT),
    wb_b(this, "wb_b", "8", RSTVAL_ZERO, NO_COMMENT),
    wb_res(this, "wb_res", "16", RSTVAL_ZERO, NO_COMMENT),
    w_signed(this, "w_signed", "1", RSTVAL_ZERO, NO_COMMENT),
    // submodules:
    clk(this, "clk", NO_COMMENT),
    mul0(this, "mul0", NO_COMMENT),
    // processes:
    comb(this),
    test_clk(this, &w_clk)
{
    Operation::start(this);

    clk.period.setObjValue(new FloatConst(25.0));  // 40 MHz
    NEW(clk, clk.getName().c_str());
        CONNECT(clk, 0, clk.o_clk, w_clk);
    ENDNEW();

    TEXT();
    NEW(mul0, mul0.getName().c_str());
        CONNECT(mul0, 0, mul0.i_nrst, w_nrst);
        CONNECT(mul0, 0, mul0.i_clk, w_clk);
        CONNECT(mul0, 0, mul0.i_a, wb_a);
        CONNECT(mul0, 0, mul0.i_b, wb_b);
        CONNECT(mul0, 0, mul0.i_signed, w_signed);
        CONNECT(mul0, 0, mul0.o_res, wb_res);
    ENDNEW();

    TEXT();
    INITIAL();
        SETZERO(w_nrst);
        SETVAL_DELAY(w_nrst, CONST("1", 1), *new FloatConst(800.0));
    ENDINITIAL();

    Operation::start(&comb);
    proc_comb();

    Operation::start(&test_clk);
    proc_test_clk();
}

void mathlib_tb::proc_comb() {
}


void mathlib_tb::proc_test_clk() {
    IF (EZ(w_nrst));
        SETZERO(wb_clk_cnt);
    ELSE();
        SETVAL(wb_clk_cnt, INC(wb_clk_cnt));
    ENDIF();

    IF (EQ(wb_clk_cnt, CONST("0x20", 32)));
        SETVAL(wb_a, CONST("127", 8));
        SETVAL(wb_b, CONST("0x81", 8), "-127");
        SETONE(w_signed);
    ELSIF (EQ(wb_clk_cnt, CONST("0x21", 32)));
        SETVAL(wb_a, CONST("255", 8));
        SETVAL(wb_b, CONST("255", 8));
        SETZERO(w_signed);
    ELSE();
        SETZERO(wb_a);
        SETZERO(wb_b);
        SETZERO(w_signed);
    ENDIF();
}

