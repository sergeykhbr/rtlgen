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

#include "fmul_d.h"

DoubleMul::DoubleMul(GenObject *parent, const char *name) :
    ModuleObject(parent, "DoubleMul", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_ena(this, "i_ena", "1"),
    i_a(this, "i_a", "64", "Operand 1"),
    i_b(this, "i_b", "64", "Operand 2"),
    o_res(this, "o_res", "64", "Result"),
    o_illegal_op(this, "o_illegal_op", "1"),
    o_overflow(this, "o_overflow", "1"),
    o_valid(this, "o_valid", "1", "Result is valid"),
    o_busy(this, "o_busy", "1", "Multiclock instruction under processing"),
    // signals
    w_imul_ena(this, "w_imul_ena", "1"),
    wb_imul_result(this, "wb_imul_result", "106"),
    wb_imul_shift(this, "wb_imul_shift", "7"),
    w_imul_rdy(this, "w_imul_rdy", "1"),
    w_imul_overflow(this, "w_imul_overflow", "1"),
    // registers
    busy(this, "busy", "1"),
    ena(this, "ena", "5"),
    a(this, "a", "64"),
    b(this, "b", "64"),
    result(this, "result", "64"),
    zeroA(this, "zeroA", "1"),
    zeroB(this, "zeroB", "1"),
    mantA(this, "mantA", "53"),
    mantB(this, "mantB", "53"),
    expAB(this, "expAB", "13"),
    expAlign(this, "expAlign", "12"),
    mantAlign(this, "mantAlign", "105"),
    postShift(this, "postShift", "12"),
    mantPostScale(this, "mantPostScale", "105"),
    nanA(this, "nanA", "1"),
    nanB(this, "nanB", "1"),
    overflow(this, "overflow", "1"),
    illegal_op(this, "illegal_op", "1"),
    // process
    comb(this),
    u_imul53(this, "u_imul53")
{
    Operation::start(this);

    NEW(u_imul53, u_imul53.getName().c_str());
        CONNECT(u_imul53, 0, u_imul53.i_nrst, i_nrst);
        CONNECT(u_imul53, 0, u_imul53.i_clk, i_clk);
        CONNECT(u_imul53, 0, u_imul53.i_ena, w_imul_ena);
        CONNECT(u_imul53, 0, u_imul53.i_a, mantA);
        CONNECT(u_imul53, 0, u_imul53.i_b, mantB);
        CONNECT(u_imul53, 0, u_imul53.o_result, wb_imul_result);
        CONNECT(u_imul53, 0, u_imul53.o_shift, wb_imul_shift);
        CONNECT(u_imul53, 0, u_imul53.o_rdy, w_imul_rdy);
        CONNECT(u_imul53, 0, u_imul53.o_overflow, w_imul_overflow);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void DoubleMul::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;
    GenObject *i;

    SETBIT(comb.vb_ena, 0, AND2(i_ena, INV(busy)));
    SETBIT(comb.vb_ena, 1, BIT(ena, 0));
    SETBITS(comb.vb_ena, 4, 2, CC2(BITS(ena, 3, 2), w_imul_rdy));
    SETVAL(ena, comb.vb_ena);

TEXT();
    IF (NZ(i_ena));
        SETONE(busy);
        SETZERO(overflow);
        SETVAL(a, i_a);
        SETVAL(b, i_b);
    ENDIF();

TEXT();
    SETVAL(comb.signA, BIT(a, 63));
    SETVAL(comb.signB, BIT(b, 63));

TEXT();
    IF (EZ(BITS(a, 62, 0)));
        SETONE(comb.zeroA);
    ENDIF();

TEXT();
    IF (EZ(BITS(b, 62, 0)));
        SETONE(comb.zeroB);
    ENDIF();

TEXT();
    SETBITS(comb.mantA, 51, 0, BITS(a, 51, 0));
    SETBIT(comb.mantA, 52, CONST("0", 1));
    IF (NZ(BITS(a, 62, 52)));
        SETBIT(comb.mantA, 52, CONST("1", 1));
    ENDIF();

TEXT();
    SETBITS(comb.mantB, 51, 0, BITS(b, 51, 0));
    SETBIT(comb.mantB, 52, CONST("0", 1));
    IF (NZ(BITS(b, 62, 52)));
        SETBIT(comb.mantB, 52, CONST("1", 1));
    ENDIF();

TEXT();
    TEXT("expA - expB + 1023");
    SETVAL(comb.expAB_t, ADD2(CC2(CONST("0", 1), BITS(a, 62, 52)), CC2(CONST("0", 1), BITS(b, 62, 52))));
    SETVAL(comb.expAB, SUB2(CC2(CONST("0", 1), comb.expAB_t), CONST("1023", 13)));

TEXT();
    IF (NZ(BIT(ena, 0)));
        SETVAL(expAB, comb.expAB);
        SETVAL(zeroA, comb.zeroA);
        SETVAL(zeroB, comb.zeroB);
        SETVAL(mantA, comb.mantA);
        SETVAL(mantB, comb.mantB);
    ENDIF();

TEXT();
    SETVAL(w_imul_ena, BIT(ena, 1));

TEXT();
    TEXT("imul53 module:");
    IF (NZ(BIT(wb_imul_result, 105)));
        SETVAL(comb.mantAlign, BITS(wb_imul_result, 105, 1));
    ELSIF (NZ(BIT(wb_imul_result, 104)));
        SETVAL(comb.mantAlign, BITS(wb_imul_result, 104, 0));
    ELSE();
        i = &FOR ("i", CONST("1"), CONST("105"), "++");
            IF (EQ(*i, TO_INT(wb_imul_shift)));
                SETVAL(comb.mantAlign, LSH(wb_imul_result, *i));
            ENDIF();
        ENDFOR();
    ENDIF();

TEXT();
    SETVAL(comb.expAlign_t, INC(expAB));
    IF (NZ(BIT(wb_imul_result, 105)));
        SETVAL(comb.expAlign, comb.expAlign_t);
    ELSIF (OR2(EZ(BITS(a, 62, 52)), EZ(BITS(b, 62, 52))));
        SETVAL(comb.expAlign , SUB2(comb.expAlign_t, CC2(CONST("0", 6), wb_imul_shift)));
    ELSE();
        SETVAL(comb.expAlign, SUB2(expAB, CC2(CONST("0", 6), wb_imul_shift)));
    ENDIF();

TEXT();
    TEXT("IMPORTANT exception! new ZERO value");
    IF (OR2(NZ(BIT(comb.expAlign, 12)), EZ(comb.expAlign)));
        IF (ORx(4, &EZ(wb_imul_shift), 
                   &NZ(BIT(wb_imul_result, 105)),
                   &EZ(BITS(a, 62, 52)),
                   &EZ(BITS(b, 62, 52))));
            SETVAL(comb.postShift, ADD2(INV_L(BITS(comb.expAlign, 11, 0)), CONST("2", 12)));
        ELSE();
            SETVAL(comb.postShift, INC(INV_L(BITS(comb.expAlign, 11, 0))));
        ENDIF();
    ENDIF();

TEXT();
    IF (NZ(w_imul_rdy));
        SETVAL(expAlign, BITS(comb.expAlign, 11, 0));
        SETVAL(mantAlign, comb.mantAlign);
        SETVAL(postShift, comb.postShift);

TEXT();
        TEXT("Exceptions:");
        SETZERO(nanA);
        IF (EQ(BITS(a, 62, 52), CONST("0x7FF", 11)));
            SETONE(nanA);
        ENDIF();
        SETZERO(nanB);
        IF (EQ(BITS(b, 62, 52), CONST("0x7FF", 11)));
            SETONE(nanB);
        ENDIF();
        SETZERO(overflow);
        IF (AND2(EZ(BIT(comb.expAlign,12)), GE(comb.expAlign, CONST("0x7FF", 13))));
            SETONE(overflow);
        ENDIF();
    ENDIF();

TEXT();
    TEXT("Prepare to mantissa post-scale");
    IF (EZ(postShift));
        SETVAL(comb.mantPostScale, mantAlign);
    ELSIF (LS(postShift, CONST("105", 12)));
        i = &FOR ("i", CONST("1"), CONST("105"), "++");
            IF (EQ(*i, TO_INT(postShift)));
                SETVAL(comb.mantPostScale, RSH(mantAlign, *i));
            ENDIF();
        ENDFOR();
    ENDIF();
    IF (NZ(BIT(ena, 2)));
        SETVAL(mantPostScale, comb.mantPostScale);
    ENDIF();

TEXT();
    TEXT("Rounding bit");
    SETVAL(comb.mantShort, BIG_TO_U64(BITS(mantPostScale, 104, 52)));
    SETVAL(comb.tmpMant05, BIG_TO_U64(BITS(mantPostScale, 51, 0)));
    IF (EQ(comb.mantShort, CONST("0x001fffffffffffff", 53)));
        SETONE(comb.mantOnes);
    ENDIF();
    SETVAL(comb.mantEven, BIT(mantPostScale, 52));
    IF (EQ(comb.tmpMant05, CONST("0x0008000000000000", 52)));
        SETONE(comb.mant05);
    ENDIF();
    SETVAL(comb.rndBit, AND2(BIT(mantPostScale, 51), INV(AND2(comb.mant05, INV(comb.mantEven)))));

TEXT();
    TEXT("Check Borders");
    IF (EQ(BITS(a, 62, 52), CONST("0x7ff", 11)));
        SETONE(comb.nanA);
    ENDIF();
    IF (EQ(BITS(b, 62, 52), CONST("0x7ff", 11)));
        SETONE(comb.nanB);
    ENDIF();
    IF (EZ(BITS(a, 51, 0)));
        SETONE(comb.mantZeroA);
    ENDIF();
    IF (EZ(BITS(b, 51, 0)));
        SETONE(comb.mantZeroB);
    ENDIF();

TEXT();
    TEXT("Result multiplexers:");
    IF (OR2(AND3(comb.nanA, comb.mantZeroA, zeroB), AND3(comb.nanB, comb.mantZeroB, zeroA)));
        SETVAL(comb.v_res_sign, CONST("1", 1));
    ELSIF (NZ(AND2(comb.nanA, INV(comb.mantZeroA))));
        TEXT("when both values are NaN, value B has higher priority if sign=1");
        SETVAL(comb.v_res_sign, OR2(comb.signA, AND2(comb.nanA, comb.signB)));
    ELSIF (NZ(AND2(comb.nanB, INV(comb.mantZeroB))));
        SETVAL(comb.v_res_sign, comb.signB);
    ELSE();
        SETVAL(comb.v_res_sign, XOR2(BIT(a, 63), BIT(b, 63)));
    ENDIF();

TEXT();
    IF (NZ(comb.nanA));
        SETVAL(comb.vb_res_exp, BITS(a, 62, 52));
    ELSIF (NZ(comb.nanB));
        SETVAL(comb.vb_res_exp, BITS(b, 62, 52));
    ELSIF (NZ(OR3(BIT(expAlign, 11), zeroA, zeroB)));
        SETVAL(comb.vb_res_exp, ALLZEROS());
    ELSIF (NZ(overflow));
        SETVAL(comb.vb_res_exp, ALLONES());
    ELSE();
        SETVAL(comb.vb_res_exp, ADDx(2, &BITS(expAlign, 10, 0),
                                        &AND3(comb.mantOnes, comb.rndBit, INV(overflow))));
    ENDIF();

TEXT();
    IF (ORx(3, &AND3(comb.nanA, comb.mantZeroA, INV(comb.mantZeroB)),
               &AND3(comb.nanB, comb.mantZeroB, INV(comb.mantZeroA)),
               &AND3(INV(comb.nanA), INV(comb.nanB), overflow)));
        SETVAL(comb.vb_res_mant, ALLZEROS());
    ELSIF (NZ(AND2(comb.nanA, INV(AND2(comb.nanB, comb.signB)))));
        TEXT("when both values are NaN, value B has higher priority if sign=1");
        SETVAL(comb.vb_res_mant, CC2(CONST("1", 1), BITS(a, 50, 0)));
    ELSIF (NZ(comb.nanB));
        SETVAL(comb.vb_res_mant, CC2(CONST("1", 1), BITS(b, 50, 0)));
    ELSE();
        SETVAL(comb.vb_res_mant, ADD2(BITS(comb.mantShort, 51, 0), comb.rndBit));
    ENDIF();

TEXT();
    IF (NZ(BIT(ena, 3)));
        SETVAL(result, CC3(comb.v_res_sign, comb.vb_res_exp, comb.vb_res_mant));
        SETVAL(illegal_op, OR2(comb.nanA, comb.nanB));
        SETZERO(busy);
    ENDIF();

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_res, result);
    SETVAL(o_illegal_op, illegal_op);
    SETVAL(o_overflow, overflow);
    SETVAL(o_valid, BIT(ena, 4));
    SETVAL(o_busy, busy);
}

