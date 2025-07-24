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
    compare_a(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "compare_a", "32", "13", "'0", NO_COMMENT),
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
    unsigned ia = 0;
    unsigned ib = 0;
    float fa = 0;
    float fb = 0;

    SETVAL(clk_cnt, INC(clk_cnt));
    SETZERO(w_ena);
    SETZERO(wb_a);
    SETZERO(wb_b);

    IF (EQ(clk_cnt, CONST("10")));
        fa = 3.1f;
        fb = 0.006f;
        SETONE(w_ena);
        SETVAL(wb_a, CONST_FP32(fa));
        SETVAL(wb_b, CONST_FP32(fb));
        SETARRITEM(compare_a, CONST("0"), compare_a, CONST_FP32(fa * fb));
    ELSIF (EQ(clk_cnt, CONST("11")));
        ia = 0x3F800000;//0x000000ff;
        ib = 0x00000020;
        fa = *reinterpret_cast<float *>(&ia);
        fb = *reinterpret_cast<float *>(&ib);
        SETONE(w_ena);
        SETVAL(wb_a, CONST_FP32(fa));
        SETVAL(wb_b, CONST_FP32(fb));
        SETARRITEM(compare_a, CONST("0"), compare_a, CONST_FP32(fa * fb));
    ELSIF (EQ(clk_cnt, CONST("12")));
        ia = 0xafffffff;
        ib = 0x7f7ffffe;
        fa = *reinterpret_cast<float *>(&ia);
        fb = *reinterpret_cast<float *>(&ib);
        SETONE(w_ena);
        SETVAL(wb_a, CONST_FP32(fa));
        SETVAL(wb_b, CONST_FP32(fb));
        SETARRITEM(compare_a, CONST("0"), compare_a, CONST_FP32(fa * fb));
    ELSIF (EQ(clk_cnt, CONST("20")));
        ia = 0x3f000001;
        ib = 0x00801010;
        fa = *reinterpret_cast<float *>(&ia);
        fb = *reinterpret_cast<float *>(&ib);
        SETONE(w_ena);
        SETVAL(wb_a, CONST_FP32(fa));
        SETVAL(wb_b, CONST_FP32(fb));
        SETARRITEM(compare_a, CONST("0"), compare_a, CONST_FP32(fa * fb));
    ELSIF (EQ(clk_cnt, CONST("21")));
        ia = 0x3f000002;
        ib = 0x00081010;
        fa = *reinterpret_cast<float *>(&ia);
        fb = *reinterpret_cast<float *>(&ib);
        SETONE(w_ena);
        SETVAL(wb_a, CONST_FP32(fa));
        SETVAL(wb_b, CONST_FP32(fb));
        SETARRITEM(compare_a, CONST("0"), compare_a, CONST_FP32(fa * fb));
    ELSE();
    ENDIF();

    GenObject *i = &FOR_INC(DEC(CONST("13")));
        SETARRITEM(compare_a, INC(*i), compare_a, ARRITEM(compare_a, *i, compare_a));
    ENDFOR();
}

void fmul_tb::proc_test_clk() {
}

