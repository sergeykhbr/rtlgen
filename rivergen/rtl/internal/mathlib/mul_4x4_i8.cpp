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

#include "mul_4x4_i8.h"

mul_4x4_i8::mul_4x4_i8(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "mul_4x4_i8", name, comment),
    // IO
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_signed(this, "i_signed", "1", "0=unsigned; 1=signed operation"),
    i_v0(this, "i_v0", "8", "input vector coordinate 0"),
    i_v1(this, "i_v1", "8", "input vector coordinate 1"),
    i_v2(this, "i_v2", "8", "input vector coordinate 2"),
    i_v3(this, "i_v3", "8", "input vector coordinate 3"),
    i_m(this, "i_m", "128", "matrix rates 16 x 8 bits each"),
    o_res0(this, "o_res0", "18", "result vector coordinate 0"),
    o_res1(this, "o_res1", "18", "result vector coordinate 1"),
    o_res2(this, "o_res2", "18", "result vector coordinate 2"),
    o_res3(this, "o_res3", "18", "result vector coordinate 3"),
    // params
    // signals
    wb_b00(this, "wb_b00", "8", "'0", NO_COMMENT),
    wb_b01(this, "wb_b01", "8", "'0", NO_COMMENT),
    wb_b02(this, "wb_b02", "8", "'0", NO_COMMENT),
    wb_b03(this, "wb_b03", "8", "'0", NO_COMMENT),
    wb_b10(this, "wb_b10", "8", "'0", NO_COMMENT),
    wb_b11(this, "wb_b11", "8", "'0", NO_COMMENT),
    wb_b12(this, "wb_b12", "8", "'0", NO_COMMENT),
    wb_b13(this, "wb_b13", "8", "'0", NO_COMMENT),
    wb_b20(this, "wb_b20", "8", "'0", NO_COMMENT),
    wb_b21(this, "wb_b21", "8", "'0", NO_COMMENT),
    wb_b22(this, "wb_b22", "8", "'0", NO_COMMENT),
    wb_b23(this, "wb_b23", "8", "'0", NO_COMMENT),
    wb_b30(this, "wb_b30", "8", "'0", NO_COMMENT),
    wb_b31(this, "wb_b31", "8", "'0", NO_COMMENT),
    wb_b32(this, "wb_b32", "8", "'0", NO_COMMENT),
    wb_b33(this, "wb_b33", "8", "'0", NO_COMMENT),
    wb_m00(this, "wb_m00", "16", "'0", NO_COMMENT),
    wb_m01(this, "wb_m01", "16", "'0", NO_COMMENT),
    wb_m02(this, "wb_m02", "16", "'0", NO_COMMENT),
    wb_m03(this, "wb_m03", "16", "'0", NO_COMMENT),
    wb_m10(this, "wb_m10", "16", "'0", NO_COMMENT),
    wb_m11(this, "wb_m11", "16", "'0", NO_COMMENT),
    wb_m12(this, "wb_m12", "16", "'0", NO_COMMENT),
    wb_m13(this, "wb_m13", "16", "'0", NO_COMMENT),
    wb_m20(this, "wb_m20", "16", "'0", NO_COMMENT),
    wb_m21(this, "wb_m21", "16", "'0", NO_COMMENT),
    wb_m22(this, "wb_m22", "16", "'0", NO_COMMENT),
    wb_m23(this, "wb_m23", "16", "'0", NO_COMMENT),
    wb_m30(this, "wb_m30", "16", "'0", NO_COMMENT),
    wb_m31(this, "wb_m31", "16", "'0", NO_COMMENT),
    wb_m32(this, "wb_m32", "16", "'0", NO_COMMENT),
    wb_m33(this, "wb_m33", "16", "'0", NO_COMMENT),
    // registers
    sum0a(this, "sum0a", "17", RSTVAL_ZERO, NO_COMMENT),
    sum0b(this, "sum0b", "17", RSTVAL_ZERO, NO_COMMENT),
    sum1a(this, "sum1a", "17", RSTVAL_ZERO, NO_COMMENT),
    sum1b(this, "sum1b", "17", RSTVAL_ZERO, NO_COMMENT),
    sum2a(this, "sum2a", "17", RSTVAL_ZERO, NO_COMMENT),
    sum2b(this, "sum2b", "17", RSTVAL_ZERO, NO_COMMENT),
    sum3a(this, "sum3a", "17", RSTVAL_ZERO, NO_COMMENT),
    sum3b(this, "sum3b", "17", RSTVAL_ZERO, NO_COMMENT),
    res0(this, "res0", "18", RSTVAL_ZERO, NO_COMMENT),
    res1(this, "res1", "18", RSTVAL_ZERO, NO_COMMENT),
    res2(this, "res2", "18", RSTVAL_ZERO, NO_COMMENT),
    res3(this, "res3", "18", RSTVAL_ZERO, NO_COMMENT),
    sign(this, "sign", "6", RSTVAL_ZERO, NO_COMMENT),
    //
    m00(this, "m00", NO_COMMENT),
    m01(this, "m01", NO_COMMENT),
    m02(this, "m02", NO_COMMENT),
    m03(this, "m03", NO_COMMENT),
    m10(this, "m10", NO_COMMENT),
    m11(this, "m11", NO_COMMENT),
    m12(this, "m12", NO_COMMENT),
    m13(this, "m13", NO_COMMENT),
    m20(this, "m20", NO_COMMENT),
    m21(this, "m21", NO_COMMENT),
    m22(this, "m22", NO_COMMENT),
    m23(this, "m23", NO_COMMENT),
    m30(this, "m30", NO_COMMENT),
    m31(this, "m31", NO_COMMENT),
    m32(this, "m32", NO_COMMENT),
    m33(this, "m33", NO_COMMENT),
    comb(this)
{
    Operation::start(this);

    TEXT("Row 0:");
    NEW(m00, m00.getName().c_str());
        CONNECT(m00, 0, m00.i_nrst, i_nrst);
        CONNECT(m00, 0, m00.i_clk, i_clk);
        CONNECT(m00, 0, m00.i_a, i_v0);
        CONNECT(m00, 0, m00.i_b, wb_b00);
        CONNECT(m00, 0, m00.i_signed, i_signed);
        CONNECT(m00, 0, m00.o_res, wb_m00);
    ENDNEW();

    TEXT();
    NEW(m01, m01.getName().c_str());
        CONNECT(m01, 0, m01.i_nrst, i_nrst);
        CONNECT(m01, 0, m01.i_clk, i_clk);
        CONNECT(m01, 0, m01.i_a, i_v1);
        CONNECT(m01, 0, m01.i_b, wb_b01);
        CONNECT(m01, 0, m01.i_signed, i_signed);
        CONNECT(m01, 0, m01.o_res, wb_m01);
    ENDNEW();

    TEXT();
    NEW(m02, m02.getName().c_str());
        CONNECT(m02, 0, m02.i_nrst, i_nrst);
        CONNECT(m02, 0, m02.i_clk, i_clk);
        CONNECT(m02, 0, m02.i_a, i_v2);
        CONNECT(m02, 0, m02.i_b, wb_b02);
        CONNECT(m02, 0, m02.i_signed, i_signed);
        CONNECT(m02, 0, m02.o_res, wb_m02);
    ENDNEW();

    TEXT();
    NEW(m03, m03.getName().c_str());
        CONNECT(m03, 0, m03.i_nrst, i_nrst);
        CONNECT(m03, 0, m03.i_clk, i_clk);
        CONNECT(m03, 0, m03.i_a, i_v3);
        CONNECT(m03, 0, m03.i_b, wb_b03);
        CONNECT(m03, 0, m03.i_signed, i_signed);
        CONNECT(m03, 0, m03.o_res, wb_m03);
    ENDNEW();

    TEXT();
    TEXT("Row 1:");
    NEW(m10, m10.getName().c_str());
        CONNECT(m10, 0, m10.i_nrst, i_nrst);
        CONNECT(m10, 0, m10.i_clk, i_clk);
        CONNECT(m10, 0, m10.i_a, i_v0);
        CONNECT(m10, 0, m10.i_b, wb_b10);
        CONNECT(m10, 0, m10.i_signed, i_signed);
        CONNECT(m10, 0, m10.o_res, wb_m10);
    ENDNEW();

    TEXT();
    NEW(m11, m11.getName().c_str());
        CONNECT(m11, 0, m11.i_nrst, i_nrst);
        CONNECT(m11, 0, m11.i_clk, i_clk);
        CONNECT(m11, 0, m11.i_a, i_v1);
        CONNECT(m11, 0, m11.i_b, wb_b11);
        CONNECT(m11, 0, m11.i_signed, i_signed);
        CONNECT(m11, 0, m11.o_res, wb_m11);
    ENDNEW();

    TEXT();
    NEW(m12, m12.getName().c_str());
        CONNECT(m12, 0, m12.i_nrst, i_nrst);
        CONNECT(m12, 0, m12.i_clk, i_clk);
        CONNECT(m12, 0, m12.i_a, i_v2);
        CONNECT(m12, 0, m12.i_b, wb_b12);
        CONNECT(m12, 0, m12.i_signed, i_signed);
        CONNECT(m12, 0, m12.o_res, wb_m12);
    ENDNEW();

    TEXT();
    NEW(m13, m13.getName().c_str());
        CONNECT(m13, 0, m13.i_nrst, i_nrst);
        CONNECT(m13, 0, m13.i_clk, i_clk);
        CONNECT(m13, 0, m13.i_a, i_v3);
        CONNECT(m13, 0, m13.i_b, wb_b13);
        CONNECT(m13, 0, m13.i_signed, i_signed);
        CONNECT(m03, 0, m03.o_res, wb_m13);
    ENDNEW();

    TEXT();
    TEXT("Row 2:");
    NEW(m20, m20.getName().c_str());
        CONNECT(m20, 0, m20.i_nrst, i_nrst);
        CONNECT(m20, 0, m20.i_clk, i_clk);
        CONNECT(m20, 0, m20.i_a, i_v0);
        CONNECT(m20, 0, m20.i_b, wb_b20);
        CONNECT(m20, 0, m20.i_signed, i_signed);
        CONNECT(m20, 0, m20.o_res, wb_m20);
    ENDNEW();

    TEXT();
    NEW(m21, m21.getName().c_str());
        CONNECT(m21, 0, m21.i_nrst, i_nrst);
        CONNECT(m21, 0, m21.i_clk, i_clk);
        CONNECT(m21, 0, m21.i_a, i_v1);
        CONNECT(m21, 0, m21.i_b, wb_b21);
        CONNECT(m21, 0, m21.i_signed, i_signed);
        CONNECT(m21, 0, m21.o_res, wb_m21);
    ENDNEW();

    TEXT();
    NEW(m22, m22.getName().c_str());
        CONNECT(m22, 0, m22.i_nrst, i_nrst);
        CONNECT(m22, 0, m22.i_clk, i_clk);
        CONNECT(m22, 0, m22.i_a, i_v2);
        CONNECT(m22, 0, m22.i_b, wb_b22);
        CONNECT(m22, 0, m22.i_signed, i_signed);
        CONNECT(m22, 0, m22.o_res, wb_m22);
    ENDNEW();

    TEXT();
    NEW(m23, m23.getName().c_str());
        CONNECT(m23, 0, m23.i_nrst, i_nrst);
        CONNECT(m23, 0, m23.i_clk, i_clk);
        CONNECT(m23, 0, m23.i_a, i_v3);
        CONNECT(m23, 0, m23.i_b, wb_b23);
        CONNECT(m23, 0, m23.i_signed, i_signed);
        CONNECT(m23, 0, m23.o_res, wb_m23);
    ENDNEW();

    TEXT();
    TEXT("Row 3:");
    NEW(m30, m30.getName().c_str());
        CONNECT(m30, 0, m30.i_nrst, i_nrst);
        CONNECT(m30, 0, m30.i_clk, i_clk);
        CONNECT(m30, 0, m30.i_a, i_v0);
        CONNECT(m30, 0, m30.i_b, wb_b30);
        CONNECT(m30, 0, m30.i_signed, i_signed);
        CONNECT(m30, 0, m30.o_res, wb_m30);
    ENDNEW();

    TEXT();
    NEW(m31, m31.getName().c_str());
        CONNECT(m31, 0, m31.i_nrst, i_nrst);
        CONNECT(m31, 0, m31.i_clk, i_clk);
        CONNECT(m31, 0, m31.i_a, i_v1);
        CONNECT(m31, 0, m31.i_b, wb_b31);
        CONNECT(m31, 0, m31.i_signed, i_signed);
        CONNECT(m31, 0, m31.o_res, wb_m31);
    ENDNEW();

    TEXT();
    NEW(m32, m32.getName().c_str());
        CONNECT(m32, 0, m32.i_nrst, i_nrst);
        CONNECT(m32, 0, m32.i_clk, i_clk);
        CONNECT(m32, 0, m32.i_a, i_v2);
        CONNECT(m32, 0, m32.i_b, wb_b32);
        CONNECT(m32, 0, m32.i_signed, i_signed);
        CONNECT(m32, 0, m32.o_res, wb_m32);
    ENDNEW();

    TEXT();
    NEW(m33, m33.getName().c_str());
        CONNECT(m33, 0, m33.i_nrst, i_nrst);
        CONNECT(m33, 0, m33.i_clk, i_clk);
        CONNECT(m33, 0, m33.i_a, i_v3);
        CONNECT(m33, 0, m33.i_b, wb_b33);
        CONNECT(m33, 0, m33.i_signed, i_signed);
        CONNECT(m33, 0, m33.o_res, wb_m33);
    ENDNEW();


    Operation::start(&comb);
    proc_comb();
}

void mul_4x4_i8::proc_comb() {
    ASSIGN(wb_b00, TO_U32(BITS(i_m, 7, 0)));
    ASSIGN(wb_b01, TO_U32(BITS(i_m, 15, 8)));
    ASSIGN(wb_b02, TO_U32(BITS(i_m, 23, 16)));
    ASSIGN(wb_b03, TO_U32(BITS(i_m, 31, 24)));
    ASSIGN(wb_b10, TO_U32(BITS(i_m, 39, 32)));
    ASSIGN(wb_b11, TO_U32(BITS(i_m, 47, 40)));
    ASSIGN(wb_b12, TO_U32(BITS(i_m, 55, 48)));
    ASSIGN(wb_b13, TO_U32(BITS(i_m, 63, 56)));
    ASSIGN(wb_b20, TO_U32(BITS(i_m, 71, 64)));
    ASSIGN(wb_b21, TO_U32(BITS(i_m, 79, 72)));
    ASSIGN(wb_b22, TO_U32(BITS(i_m, 87, 80)));
    ASSIGN(wb_b23, TO_U32(BITS(i_m, 95, 88)));
    ASSIGN(wb_b30, TO_U32(BITS(i_m, 103, 96)));
    ASSIGN(wb_b31, TO_U32(BITS(i_m, 111, 104)));
    ASSIGN(wb_b32, TO_U32(BITS(i_m, 119, 112)));
    ASSIGN(wb_b33, TO_U32(BITS(i_m, 127, 120)));

    TEXT();
    SETVAL(sign, CC2(BITS(sign, 4, 0), i_signed));
    SETVAL(sum0a, ADD2(CC2(AND2_L(BIT(sign, 4), BIT(wb_m00, 15)), wb_m00),
                       CC2(AND2_L(BIT(sign, 4), BIT(wb_m01, 15)), wb_m01)));
    SETVAL(sum0b, ADD2(CC2(AND2_L(BIT(sign, 4), BIT(wb_m02, 15)), wb_m02),
                       CC2(AND2_L(BIT(sign, 4), BIT(wb_m03, 15)), wb_m03)));

    TEXT();
    SETVAL(sum1a, ADD2(CC2(AND2_L(BIT(sign, 4), BIT(wb_m10, 15)), wb_m10),
                       CC2(AND2_L(BIT(sign, 4), BIT(wb_m11, 15)), wb_m11)));
    SETVAL(sum1b, ADD2(CC2(AND2_L(BIT(sign, 4), BIT(wb_m12, 15)), wb_m12),
                       CC2(AND2_L(BIT(sign, 4), BIT(wb_m13, 15)), wb_m13)));

    TEXT();
    SETVAL(sum2a, ADD2(CC2(AND2_L(BIT(sign, 4), BIT(wb_m20, 15)), wb_m20),
                       CC2(AND2_L(BIT(sign, 4), BIT(wb_m21, 15)), wb_m21)));
    SETVAL(sum2b, ADD2(CC2(AND2_L(BIT(sign, 4), BIT(wb_m22, 15)), wb_m22),
                       CC2(AND2_L(BIT(sign, 4), BIT(wb_m23, 15)), wb_m23)));

    TEXT();
    SETVAL(sum3a, ADD2(CC2(AND2_L(BIT(sign, 4), BIT(wb_m30, 15)), wb_m30),
                       CC2(AND2_L(BIT(sign, 4), BIT(wb_m31, 15)), wb_m31)));
    SETVAL(sum3b, ADD2(CC2(AND2_L(BIT(sign, 4), BIT(wb_m32, 15)), wb_m32),
                       CC2(AND2_L(BIT(sign, 4), BIT(wb_m33, 15)), wb_m33)));


    TEXT();
    SETVAL(res0, ADD2(CC2(AND2_L(BIT(sign, 5), BIT(sum0a, 16)), sum0a),
                      CC2(AND2_L(BIT(sign, 5), BIT(sum0b, 16)), sum0b)));
    SETVAL(res1, ADD2(CC2(AND2_L(BIT(sign, 5), BIT(sum1a, 16)), sum1a),
                      CC2(AND2_L(BIT(sign, 5), BIT(sum1b, 16)), sum1b)));
    SETVAL(res2, ADD2(CC2(AND2_L(BIT(sign, 5), BIT(sum2a, 16)), sum2a),
                      CC2(AND2_L(BIT(sign, 5), BIT(sum2b, 16)), sum2b)));
    SETVAL(res3, ADD2(CC2(AND2_L(BIT(sign, 5), BIT(sum3a, 16)), sum3a),
                      CC2(AND2_L(BIT(sign, 5), BIT(sum3b, 16)), sum3b)));

    TEXT();
    SYNC_RESET();

    TEXT();
    SETVAL(o_res0, res0);
    SETVAL(o_res1, res1);
    SETVAL(o_res2, res2);
    SETVAL(o_res3, res3);
}

