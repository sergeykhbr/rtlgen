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

#include "fadd_tb.h"

fadd_tb::fadd_tb(GenObject *parent, const char *name) :
    ModuleObject(parent, "fadd_tb", name, NO_COMMENT),
    // parameters
    // Ports
    nrst(this, "nrst", "1", "1", "Power-on system reset active LOW"),
    clk(this, "clk", "1", RSTVAL_ZERO, NO_COMMENT),
    w_ena(this, "w_ena", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_a(this, "wb_a", "32", RSTVAL_ZERO, NO_COMMENT),
    wb_b(this, "wb_b", "32", RSTVAL_ZERO, NO_COMMENT),
    wb_res(this, "wb_res", "32", RSTVAL_ZERO, NO_COMMENT),
    w_valid(this, "w_valid", "1", RSTVAL_ZERO, NO_COMMENT),
    w_ex(this, "w_ex", "1", RSTVAL_ZERO, NO_COMMENT),
    w_compare_ena(this, "w_compare_ena", "1", "'0", NO_COMMENT),
    wb_compare_a(this, "wb_compare_a", "32", "'0", NO_COMMENT),
    w_show_result(this, "w_show_result", "1", "'0", NO_COMMENT),
    // regs
    clk_cnt(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "clk_cnt", "32", "'0", NO_COMMENT),
    compare_cnt(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "compare_cnt", "32", "'0", NO_COMMENT),
    err_cnt(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "err_cnt", "32", "'0", NO_COMMENT),
    compare_a(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "compare_a", "32", "13", "'0", NO_COMMENT),
    // submodules:
    pll0(this, "pll0", NO_COMMENT),
    tt(this, "tt", NO_COMMENT),
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
    tt.fbits.setObjValue(new DecConst(32));
    tt.expbits.setObjValue(new DecConst(8));
    tt.shiftbits.setObjValue(new DecConst(6));
    NEW(tt, tt.getName().c_str());
        CONNECT(tt, 0, tt.i_nrst, nrst);
        CONNECT(tt, 0, tt.i_clk, clk);
        CONNECT(tt, 0, tt.i_ena, w_ena);
        CONNECT(tt, 0, tt.i_a, wb_a);
        CONNECT(tt, 0, tt.i_b, wb_b);
        CONNECT(tt, 0, tt.o_res, wb_res);
        CONNECT(tt, 0, tt.o_valid, w_valid);
        CONNECT(tt, 0, tt.o_ex, w_ex);
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

void fadd_tb::proc_comb() {
    unsigned ia = 0;
    unsigned ib = 0;
    float fa = 0;
    float fb = 0;

    SETVAL(clk_cnt, INC(clk_cnt));
    SETZERO(w_ena);
    SETZERO(wb_a);
    SETZERO(wb_b);
    SETZERO(w_compare_ena);
    SETZERO(w_show_result);

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
    ELSIF (EQ(clk_cnt, CONST("22")));
        ia = 0xbfc00002;
        ib = 0x01091010;
        fa = *reinterpret_cast<float *>(&ia);
        fb = *reinterpret_cast<float *>(&ib);
        SETONE(w_ena);
        SETVAL(wb_a, CONST_FP32(fa));
        SETVAL(wb_b, CONST_FP32(fb));
        SETARRITEM(compare_a, CONST("0"), compare_a, CONST_FP32(fa * fb));
    ELSIF (EQ(clk_cnt, CONST("23")));
        ia = 0x35d00005;
        ib = 0x8309101f;
        fa = *reinterpret_cast<float *>(&ia);
        fb = *reinterpret_cast<float *>(&ib);
        SETONE(w_ena);
        SETVAL(wb_a, CONST_FP32(fa));
        SETVAL(wb_b, CONST_FP32(fb));
        SETARRITEM(compare_a, CONST("0"), compare_a, CONST_FP32(fa * fb));
    ELSIF (EQ(clk_cnt, CONST("24")));
        ia = 0xf5400509;
        ib = 0xa3e9103f;
        fa = *reinterpret_cast<float *>(&ia);
        fb = *reinterpret_cast<float *>(&ib);
        SETONE(w_ena);
        SETVAL(wb_a, CONST_FP32(fa));
        SETVAL(wb_b, CONST_FP32(fb));
        SETARRITEM(compare_a, CONST("0"), compare_a, CONST_FP32(fa * fb));
    ELSIF (EQ(clk_cnt, CONST("25")));
        ia = 0xff800001;
        ib = 0xff800000;
        fa = *reinterpret_cast<float *>(&ia);
        fb = *reinterpret_cast<float *>(&ib);
        SETONE(w_ena);
        SETVAL(wb_a, CONST_FP32(fa));
        SETVAL(wb_b, CONST_FP32(fb));
        SETARRITEM(compare_a, CONST("0"), compare_a, CONST_FP32(fa * fb));
    ELSIF (EQ(clk_cnt, CONST("26")));
        ia = 0xbf800001;
        ib = 0xbf800000;
        fa = *reinterpret_cast<float *>(&ia);
        fb = *reinterpret_cast<float *>(&ib);
        SETONE(w_ena);
        SETVAL(wb_a, CONST_FP32(fa));
        SETVAL(wb_b, CONST_FP32(fb));
        SETARRITEM(compare_a, CONST("0"), compare_a, CONST_FP32(fa * fb));
    ELSIF (EQ(clk_cnt, CONST("27")));
        ia = 0xbfc00002;
        ib = 0xbfc00001;
        fa = *reinterpret_cast<float *>(&ia);
        fb = *reinterpret_cast<float *>(&ib);
        SETONE(w_ena);
        SETVAL(wb_a, CONST_FP32(fa));
        SETVAL(wb_b, CONST_FP32(fb));
        SETARRITEM(compare_a, CONST("0"), compare_a, CONST_FP32(fa * fb));
    ELSIF (EQ(clk_cnt, CONST("28")));
        ia = 0x80800002;
        ib = 0x7f000003;
        fa = *reinterpret_cast<float *>(&ia);
        fb = *reinterpret_cast<float *>(&ib);
        SETONE(w_ena);
        SETVAL(wb_a, CONST_FP32(fa));
        SETVAL(wb_b, CONST_FP32(fb));
        SETARRITEM(compare_a, CONST("0"), compare_a, CONST_FP32(fa * fb));
    ELSIF (EQ(clk_cnt, CONST("29")));
        ia = 0x007fffff;
        ib = 0x7e7fffff;
        fa = *reinterpret_cast<float *>(&ia);
        fb = *reinterpret_cast<float *>(&ib);
        SETONE(w_ena);
        SETVAL(wb_a, CONST_FP32(fa));
        SETVAL(wb_b, CONST_FP32(fb));
        SETARRITEM(compare_a, CONST("0"), compare_a, CONST_FP32(fa * fb));
    ELSIF(EQ(clk_cnt, CONST("60")));
        SETONE(w_show_result);
    ENDIF();


    GenObject *i = &FOR_INC(DEC(CONST("13")));
        SETARRITEM(compare_a, INC(*i), compare_a, ARRITEM(compare_a, *i, compare_a));
    ENDFOR();

    TEXT();
    TEXT("Prepare compare statistic:");
    IF (AND2(NZ(w_valid), EZ(w_ex)));
        SETONE(w_compare_ena);
        SETVAL(compare_cnt, INC(compare_cnt));
        IF (NE(wb_res, ARRITEM(compare_a, DEC(CONST("13")), compare_a)));
            SETVAL(err_cnt, INC(err_cnt));
        ENDIF();
    ENDIF();
    SETVAL(wb_compare_a, ARRITEM(compare_a, DEC(CONST("13")), compare_a));
}

void fadd_tb::proc_test_clk() {
    IF (w_compare_ena);
        EXPECT_EQ(wb_compare_a, wb_res, "FADD compare");
    ENDIF();
    IF (NZ(w_show_result));
        DISPLAY_ERROR(compare_cnt, err_cnt, "FADD errors");
    ENDIF();
}

