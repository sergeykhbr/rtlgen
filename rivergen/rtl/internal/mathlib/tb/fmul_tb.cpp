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

#include "fmul_tb.h"

fmul_tb::fmul_tb(GenObject *parent, const char *name) :
    ModuleObject(parent, "fmul_tb", name, NO_COMMENT),
    // parameters
    // Ports
    nrst(this, "nrst", "1", "1", "Power-on system reset active LOW"),
    clk(this, "clk", "1", RSTVAL_ZERO, NO_COMMENT),
    w_ena(this, "w_ena", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_a(this, "wb_a", "32", RSTVAL_ZERO, NO_COMMENT),
    wb_b(this, "wb_b", "32", RSTVAL_ZERO, NO_COMMENT),
    wb_res(this, "wb_res", "32", RSTVAL_ZERO, NO_COMMENT),
    w_valid(this, "w_valid", "1", RSTVAL_ZERO, NO_COMMENT),
    w_overflow(this, "w_overflow", "1", RSTVAL_ZERO, NO_COMMENT),
    // regs
    clk_cnt(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "clk_cnt", "32", "'0", NO_COMMENT),
    compare_a(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "compare_a", "32", "'0", NO_COMMENT),
    // submodules:
    pll0(this, "pll0", NO_COMMENT),
    mul_fp32(this, "mul_fp32", NO_COMMENT),
    // processes:
    comb(this),
    test_clk(this, &clk)
{
    Operation::start(this);

    pll0.period.setObjValue(new FloatConst(25.0));  // 40 MHz
    NEW(pll0, pll0.getName().c_str());
        CONNECT(pll0, 0, pll0.o_clk, clk);
    ENDNEW();

    TEXT();
    mul_fp32.fbits.setObjValue(new DecConst(32));
    mul_fp32.expbits.setObjValue(new DecConst(8));
    NEW(mul_fp32, mul_fp32.getName().c_str());
        CONNECT(mul_fp32, 0, mul_fp32.i_nrst, nrst);
        CONNECT(mul_fp32, 0, mul_fp32.i_clk, clk);
        CONNECT(mul_fp32, 0, mul_fp32.i_ena, w_ena);
        CONNECT(mul_fp32, 0, mul_fp32.i_a, wb_a);
        CONNECT(mul_fp32, 0, mul_fp32.i_b, wb_b);
        CONNECT(mul_fp32, 0, mul_fp32.o_res, wb_res);
        CONNECT(mul_fp32, 0, mul_fp32.o_valid, w_valid);
        CONNECT(mul_fp32, 0, mul_fp32.o_overflow, w_overflow);
    ENDNEW();

    TEXT();
    INITIAL();
        SETZERO(nrst);
        SETVAL_DELAY(nrst, CONST("1", 1), *new FloatConst(800.0));
    ENDINITIAL();

    Operation::start(&comb);
    proc_comb();

    Operation::start(&test_clk);
    proc_test_clk();
}

void fmul_tb::proc_comb() {
    SETVAL(clk_cnt, INC(clk_cnt));
    SETZERO(w_ena);

    IF (EQ(clk_cnt, CONST("10")));
        SETONE(w_ena);
        SETVAL(wb_a, CONST_FP32(3.1f));
        SETVAL(wb_b, CONST_FP32(0.006f));
        SETVAL(compare_a, CONST_FP32(3.1f * 0.006f));
    ENDIF();
}

void fmul_tb::proc_test_clk() {
}

