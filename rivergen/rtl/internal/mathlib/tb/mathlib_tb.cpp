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
    w_signed(this, "w_signed", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_v0(this, "wb_v0", "8", RSTVAL_ZERO, NO_COMMENT),
    wb_v1(this, "wb_v1", "8", RSTVAL_ZERO, NO_COMMENT),
    wb_v2(this, "wb_v2", "8", RSTVAL_ZERO, NO_COMMENT),
    wb_v3(this, "wb_v3", "8", RSTVAL_ZERO, NO_COMMENT),
    wb_m(this, "wb_m", "128", RSTVAL_ZERO, NO_COMMENT),
    wb_res0(this, "wb_res0", "18", RSTVAL_ZERO, NO_COMMENT),
    wb_res1(this, "wb_res1", "18", RSTVAL_ZERO, NO_COMMENT),
    wb_res2(this, "wb_res2", "18", RSTVAL_ZERO, NO_COMMENT),
    wb_res3(this, "wb_res3", "18", RSTVAL_ZERO, NO_COMMENT),
    // submodules:
    clk(this, "clk", NO_COMMENT),
    im8(this, "im8", NO_COMMENT),
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
    NEW(im8, im8.getName().c_str());
        CONNECT(im8, 0, im8.i_nrst, w_nrst);
        CONNECT(im8, 0, im8.i_clk, w_clk);
        CONNECT(im8, 0, im8.i_signed, w_signed);
        CONNECT(im8, 0, im8.i_v0, wb_v0);
        CONNECT(im8, 0, im8.i_v1, wb_v1);
        CONNECT(im8, 0, im8.i_v2, wb_v2);
        CONNECT(im8, 0, im8.i_v3, wb_v3);
        CONNECT(im8, 0, im8.i_m, wb_m);
        CONNECT(im8, 0, im8.o_res0, wb_res0);
        CONNECT(im8, 0, im8.o_res1, wb_res1);
        CONNECT(im8, 0, im8.o_res2, wb_res2);
        CONNECT(im8, 0, im8.o_res3, wb_res3);
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
    SETVAL(wb_m, CCx(16, &comb.vb_m33,
                         &comb.vb_m32,
                         &comb.vb_m31,
                         &comb.vb_m30,
                         &comb.vb_m23,
                         &comb.vb_m22,
                         &comb.vb_m21,
                         &comb.vb_m20,
                         &comb.vb_m13,
                         &comb.vb_m12,
                         &comb.vb_m11,
                         &comb.vb_m10,
                         &comb.vb_m03,
                         &comb.vb_m02,
                         &comb.vb_m01,
                         &comb.vb_m00));
}

void mathlib_tb::proc_test_clk() {
    IF (EZ(w_nrst));
        SETZERO(wb_clk_cnt);
    ELSE();
        SETVAL(wb_clk_cnt, INC(wb_clk_cnt));
    ENDIF();

    IF (EQ(wb_clk_cnt, CONST("0x20", 32)));
        SETVAL(wb_v0, CONST("37", 8));
        SETVAL(wb_v1, CONST("15", 8));
        SETVAL(wb_v2, CONST("84", 8));
        SETVAL(wb_v3, CONST("127", 8));
        SETONE(w_signed);
    ELSIF (EQ(wb_clk_cnt, CONST("0x21", 32)));
        SETVAL(wb_v0, CONST("255", 8));
        SETVAL(wb_v1, CONST("255", 8));
        SETVAL(wb_v2, CONST("255", 8));
        SETVAL(wb_v3, CONST("255", 8));
        SETZERO(w_signed);
    ELSE();
        SETZERO(wb_v0);
        SETZERO(wb_v1);
        SETZERO(wb_v2);
        SETZERO(wb_v3);
        SETZERO(w_signed);
    ENDIF();
}

