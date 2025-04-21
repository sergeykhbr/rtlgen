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

#include "fadd_d.h"

DoubleAdd::DoubleAdd(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "DoubleAdd", name, comment),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_ena(this, "i_ena", "1"),
    i_add(this, "i_add", "1"),
    i_sub(this, "i_sub", "1"),
    i_eq(this, "i_eq", "1"),
    i_lt(this, "i_lt", "1"),
    i_le(this, "i_le", "1"),
    i_max(this, "i_max", "1"),
    i_min(this, "i_min", "1"),
    i_a(this, "i_a", "64", "Operand 1"),
    i_b(this, "i_b", "64", "Operand 2"),
    o_res(this, "o_res", "64", "Result"),
    o_illegal_op(this, "o_illegal_op", "1" ,"nanA | nanB"),
    o_overflow(this, "o_overflow", "1"),
    o_valid(this, "o_valid", "1", "Result is valid"),
    o_busy(this, "o_busy", "1", "Multiclock instruction under processing"),
    // registers
    busy(this, "busy", "1"),
    ena(this, "ena", "8", "'0", NO_COMMENT),
    a(this, "a", "64", "'0", NO_COMMENT),
    b(this, "b", "64", "'0", NO_COMMENT),
    result(this, "result", "64", "'0", NO_COMMENT),
    illegal_op(this, "illegal_op", "1"),
    overflow(this, "overflow", "1"),
    add(this, "add", "1"),
    sub(this, "sub", "1"),
    eq(this, "eq", "1"),
    lt(this, "lt", "1"),
    le(this, "le", "1"),
    max(this, "max", "1"),
    min(this, "min", "1"),
    flMore(this, "flMore", "1"),
    flEqual(this, "flEqual", "1"),
    flLess(this, "flLess", "1"),
    preShift(this, "preShift", "12", "'0", NO_COMMENT),
    signOpMore(this, "signOpMore", "1"),
    expMore(this, "expMore", "11", "'0", NO_COMMENT),
    mantMore(this, "mantMore", "53", "'0", NO_COMMENT),
    mantLess(this, "mantLess", "53", "'0", NO_COMMENT),
    mantLessScale(this, "mantLessScale", "105", "'0", NO_COMMENT),
    mantSum(this, "mantSum", "106", "'0", NO_COMMENT),
    lshift(this, "lshift", "7", "'0", NO_COMMENT),
    mantAlign(this, "mantAlign", "105", "'0", NO_COMMENT),
    expPostScale(this, "expPostScale", "12", "'0", NO_COMMENT),
    expPostScaleInv(this, "expPostScaleInv", "12", "'0", NO_COMMENT),
    mantPostScale(this, "mantPostScale", "105", "'0", NO_COMMENT),
    // process
    comb(this)
{
    Operation::start(&comb);
    proc_comb();
}

void DoubleAdd::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;
    GenObject *i;

    SETVAL(comb.v_ena, AND2(i_ena, INV(busy)));
    SETVAL(ena, CC2(BITS(ena, 6, 0), comb.v_ena));

TEXT();
    IF (NZ(i_ena));
        SETONE(busy);
        SETVAL(add, i_add);
        SETVAL(sub, i_sub);
        SETVAL(eq, i_eq);
        SETVAL(lt, i_lt);
        SETVAL(le, i_le);
        SETVAL(max, i_max);
        SETVAL(min, i_min);
        SETVAL(a, i_a);
        SETVAL(b, i_b);
        SETZERO(illegal_op);
        SETZERO(overflow);
    ENDIF();

TEXT();
    SETVAL(comb.signOp, OR3(sub, le, lt));
    SETVAL(comb.signA, BIT(a, 63));
    SETVAL(comb.signB, BIT(b, 63));
    SETVAL(comb.signOpB, XOR2(comb.signB, comb.signOp));

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
    IF (AND2(NZ(BITS(a, 62, 52)), EZ(BITS(b, 62, 52))));
        SETVAL(comb.expDif, DEC(BITS(a, 62, 52)));
    ELSIF (AND2(EZ(BITS(a, 62, 52)), NZ(BITS(b, 62, 52))));
        SETVAL(comb.expDif, SUB2(CONST("1", 12), BITS(b, 62, 52)));
    ELSE();
        SETVAL(comb.expDif, SUB2(BITS(a, 62, 52), BITS(b, 62, 52)));
    ENDIF();

TEXT();
    SETVAL(comb.mantDif, SUB2(CC2(CONST("0", 1), comb.mantA), CC2(CONST("0", 1), comb.mantB)));
    IF (EZ(comb.expDif));
        SETVAL(comb.vb_preShift, comb.expDif);
        IF (EZ(comb.mantDif));
            SETVAL(comb.v_flMore, AND2(INV(comb.signA), XOR2(comb.signA, comb.signB)));
            SETVAL(comb.v_flEqual, INV(XOR2(comb.signA, comb.signB)));
            SETVAL(comb.v_flLess, AND2(comb.signA, XOR2(comb.signA, comb.signB)));

TEXT();
            SETVAL(comb.v_signOpMore, comb.signA);
            SETVAL(comb.vb_expMore, BITS(a, 62, 52));
            SETVAL(comb.vb_mantMore, comb.mantA);
            SETVAL(comb.vb_mantLess, comb.mantB);
        ELSIF (EZ(BIT(comb.mantDif, 53)), "A > B");
            SETVAL(comb.v_flMore, INV(comb.signA));
            SETZERO(comb.v_flEqual);
            SETVAL(comb.v_flLess, comb.signA);

TEXT();
            SETVAL(comb.v_signOpMore, comb.signA);
            SETVAL(comb.vb_expMore, BITS(a, 62, 52));
            SETVAL(comb.vb_mantMore, comb.mantA);
            SETVAL(comb.vb_mantLess, comb.mantB);
        ELSE();
            SETVAL(comb.v_flMore, comb.signB);
            SETZERO(comb.v_flEqual);
            SETVAL(comb.v_flLess, INV(comb.signB));

TEXT();
            SETVAL(comb.v_signOpMore, comb.signOpB);
            SETVAL(comb.vb_expMore, BITS(b, 62, 52));
            SETVAL(comb.vb_mantMore, comb.mantB);
            SETVAL(comb.vb_mantLess, comb.mantA);
        ENDIF();
    ELSIF (EZ(BIT(comb.expDif, 11)));
        SETVAL(comb.v_flMore, INV(comb.signA));
        SETZERO(comb.v_flEqual);
        SETVAL(comb.v_flLess, comb.signA);

TEXT();
        SETVAL(comb.vb_preShift, comb.expDif);
        SETVAL(comb.v_signOpMore, comb.signA);
        SETVAL(comb.vb_expMore, BITS(a, 62, 52));
        SETVAL(comb.vb_mantMore, comb.mantA);
        SETVAL(comb.vb_mantLess, comb.mantB);
    ELSE();
        SETVAL(comb.v_flMore, comb.signB);
        SETZERO(comb.v_flEqual);
        SETVAL(comb.v_flLess, INV(comb.signB));

TEXT();
        SETVAL(comb.vb_preShift, INC(INV_L(comb.expDif)));
        SETVAL(comb.v_signOpMore, comb.signOpB);
        SETVAL(comb.vb_expMore, BITS(b, 62, 52));
        SETVAL(comb.vb_mantMore, comb.mantB);
        SETVAL(comb.vb_mantLess, comb.mantA);
    ENDIF();
    IF (NZ(BIT(ena, 0)));
        SETVAL(flMore, comb.v_flMore);
        SETVAL(flEqual, comb.v_flEqual);
        SETVAL(flLess, comb.v_flLess);
        SETVAL(preShift, comb.vb_preShift);
        SETVAL(signOpMore, comb.v_signOpMore);
        SETVAL(expMore, comb.vb_expMore);
        SETVAL(mantMore, comb.vb_mantMore);
        SETVAL(mantLess, comb.vb_mantLess);
    ENDIF();

TEXT();
    TEXT("Pre-scale 105-bits mantissa if preShift < 105:");
    TEXT("M = {mantM, 52'd0}");
    SETVAL(comb.mantLessScale, BIG_TO_U64(mantLess));
    SETVAL(comb.mantLessScale, LSH(comb.mantLessScale, 52));
    IF (NZ(BIT(ena, 1)));
        SETZERO(mantLessScale);
        i = &FOR ("i", CONST("0"), CONST("105"), "++");
            IF (EQ(*i, preShift));
                SETVAL(mantLessScale, RSH(comb.mantLessScale, *i));
            ENDIF();
        ENDFOR();
    ENDIF();

TEXT();
    SETVAL(comb.mantMoreScale, BIG_TO_U64(mantMore));
    SETVAL(comb.mantMoreScale, LSH(comb.mantMoreScale, 52));

TEXT();
    TEXT("106-bits adder/subtractor");
    IF (NZ(XOR2(comb.signA, comb.signOpB)));
        SETVAL(comb.vb_mantSum, SUB2(comb.mantMoreScale, mantLessScale));
    ELSE();
        SETVAL(comb.vb_mantSum, ADD2(comb.mantMoreScale, mantLessScale));
    ENDIF();

TEXT();
    IF (NZ(BIT(ena, 2)));
        SETVAL(mantSum, comb.vb_mantSum);
    ENDIF();

TEXT();
    TEXT("To avoid timing constrains violation try to implement parallel demux");
    TEXT("for Xilinx Vivado");
    i = &FOR("i", CONST("0"), CONST("104"), "++");
        SETBIT(comb.vb_mantSumInv, INC(*i), BIT(mantSum, SUB2(CONST("103"), *i)));
    ENDFOR();

TEXT();
    i = &FOR("i", CONST("0"), CONST("64"), "++");
        IF (AND2(EZ(comb.vb_lshift_p1), NZ(BIT(comb.vb_mantSumInv, *i))));
            SETVAL(comb.vb_lshift_p1, *i);
        ENDIF();
    ENDFOR();

TEXT();
    i = &FOR("i", CONST("0"), CONST("41"), "++");
        IF (AND2(EZ(comb.vb_lshift_p2), NZ(BIT(comb.vb_mantSumInv, ADD2(CONST("64"), *i)))));
            SETVAL(comb.vb_lshift_p2, *i);
            SETBIT(comb.vb_lshift_p2, 6, CONST("1", 1));
        ENDIF();
    ENDFOR();

TEXT();
    TEXT("multiplexer");
    IF (NZ(BIT(mantSum, 105)));
        TEXT("shift right");
        SETVAL(comb.vb_lshift, ALLONES());
    ELSIF (NZ(BIT(mantSum, 104)));
        SETZERO(comb.vb_lshift);
    ELSIF (NZ(comb.vb_lshift_p1));
        SETVAL(comb.vb_lshift, comb.vb_lshift_p1);
    ELSE();
        SETVAL(comb.vb_lshift, comb.vb_lshift_p2);
    ENDIF();
    IF (NZ(BIT(ena, 3)));
        SETVAL(lshift, comb.vb_lshift);
    ENDIF();

TEXT();
    TEXT("Prepare to mantissa post-scale");
    IF (EQ(lshift, CONST("0x7F", 7)));
        SETVAL(comb.vb_mantAlign, RSH(mantSum, CONST("1")));
    ELSIF (EZ(lshift));
        SETVAL(comb.vb_mantAlign, mantSum);
    ELSE();
        i = &FOR("i", CONST("1"), CONST("105"), "++");
            IF (EQ(*i, lshift));
                SETVAL(comb.vb_mantAlign, LSH(mantSum, *i));
            ENDIF();
        ENDFOR();
    ENDIF();
    IF (EQ(lshift, CONST("0x7F", 7)));
        IF (EQ(expMore, CONST("0x7FF", 11)));
            SETVAL(comb.vb_expPostScale, CC2(CONST("0", 1), expMore));
        ELSE();
            SETVAL(comb.vb_expPostScale, CC2(CONST("0", 1), INC(expMore)));
        ENDIF();
    ELSE();
        IF (AND2(EZ(expMore), EZ(lshift)));
            SETVAL(comb.vb_expPostScale, CONST("1", 12));
        ELSE();
            SETVAL(comb.vb_expPostScale, SUB2(CC2(CONST("0", 1), expMore), CC2(CONST("0", 1), lshift)));
        ENDIF();
    ENDIF();
    IF (NZ(XOR2(comb.signA, comb.signOpB)));
        TEXT("subtractor only: result value becomes with exp=0");
        IF (ANDx(2, &NZ(expMore),
                    &OR2(NZ(BIT(comb.vb_expPostScale, 11)), EZ(comb.vb_expPostScale))));
            SETVAL(comb.vb_expPostScale, DEC(comb.vb_expPostScale));
        ENDIF();
    ENDIF();
    IF (NZ(BIT(ena, 4)));
        SETVAL(mantAlign, comb.vb_mantAlign);
        SETVAL(expPostScale, comb.vb_expPostScale);
        SETVAL(expPostScaleInv, INC(INV_L(comb.vb_expPostScale)));
    ENDIF();

TEXT();
    TEXT("Mantissa post-scale:");
    TEXT("   Scaled = SumScale>>(-ExpSum) only if ExpSum < 0;");
    SETVAL(comb.vb_mantPostScale, mantAlign);
    IF (NZ(BIT(expPostScale, 11)));
        i = &FOR("i", CONST("1"), CONST("105"), "++");
            IF (EQ(*i, TO_INT(expPostScaleInv)));
                SETVAL(comb.vb_mantPostScale, RSH(mantAlign, *i));
            ENDIF();
        ENDFOR();
    ENDIF();
    IF (NZ(BIT(ena, 5)));
        SETVAL(mantPostScale, comb.vb_mantPostScale);
    ENDIF();

TEXT();
    TEXT("Rounding bit");
    SETVAL(comb.mantShort, BIG_TO_U64(BITS(mantPostScale, 104, 52)));
    SETVAL(comb.tmpMant05, BIG_TO_U64(BITS(mantPostScale, 51, 0)));
    SETZERO(comb.mantOnes);
    IF (EQ(comb.mantShort, CONST("0x001fffffffffffff", 53)));
        SETONE(comb.mantOnes);
    ENDIF();
    SETVAL(comb.mantEven, BIT(mantPostScale, 52));
    IF (EQ(comb.tmpMant05, CONST("0x0008000000000000", 52)));
        SETONE(comb.mant05);
    ENDIF();
    SETVAL(comb.rndBit, AND2(BIT(mantPostScale, 51), INV(AND2(comb.mant05, INV(comb.mantEven)))));

TEXT();
    TEXT("Check borders");
    IF (EZ(BITS(a, 51, 0)));
        SETONE(comb.mantZeroA);
    ENDIF();
    IF (EZ(BITS(b, 51, 0)));
        SETONE(comb.mantZeroB);
    ENDIF();

TEXT();
    TEXT("Exceptions");
    IF (AND2(EZ(BITS(a, 62, 0)), EZ(BITS(b, 62, 0))));
        SETONE(comb.allZero);
    ENDIF();
    IF (EZ(mantPostScale));
        SETONE(comb.sumZero);
    ENDIF();
    IF (EQ(BITS(a, 62, 52), CONST("0x7ff", 11)));
        SETONE(comb.nanA);
    ENDIF();
    IF (EQ(BITS(b, 62, 52), CONST("0x7ff", 11)));
        SETONE(comb.nanB);
    ENDIF();
    SETVAL(comb.nanAB, AND4(comb.nanA, comb.mantZeroA, comb.nanB, comb.mantZeroB));
    IF (EQ(expPostScale, CONST("0x7FF", 12)), "positive");
        SETONE(comb.overflow);
    ENDIF();

TEXT();
    TEXT("Result multiplexers:");
    IF (NZ(AND2(comb.nanAB, comb.signOp)));
        SETBIT(comb.resAdd, 63, XOR2(comb.signA, comb.signOpB));
    ELSIF (NZ(comb.nanA));
        TEXT("when both values are NaN, value B has higher priority if sign=1");
        SETBIT(comb.resAdd, 63, OR2(comb.signA, AND2(comb.nanB, comb.signOpB)));
    ELSIF (NZ(comb.nanB));
        SETBIT(comb.resAdd, 63, XOR2(comb.signOpB, AND2(comb.signOp, INV(comb.mantZeroB))));
    ELSIF (NZ(comb.allZero));
        SETBIT(comb.resAdd, 63, AND2(comb.signA, comb.signOpB));
    ELSIF (NZ(comb.sumZero));
        SETBIT(comb.resAdd, 63, CONST("0", 1));
    ELSE();
        SETBIT(comb.resAdd, 63, signOpMore);
    ENDIF();

TEXT();
    IF (NZ(OR2(comb.nanA, comb.nanB)));
        SETBITS(comb.resAdd, 62, 52, ALLONES());
    ELSIF (OR2(NZ(BIT(expPostScale, 11)), NZ(comb.sumZero)));
        SETBITS(comb.resAdd, 62, 52, ALLZEROS());
    ELSE();
        SETBITS(comb.resAdd,62, 52, ADD2(expPostScale,
                       AND3(comb.mantOnes, comb.rndBit, INV(comb.overflow))));
    ENDIF();

TEXT();
    IF (NZ(AND4(comb.nanA, comb.mantZeroA, comb.nanB, comb.mantZeroB)));
        SETBIT(comb.resAdd, 51, comb.signOp);
        SETBITS(comb.resAdd, 50, 0, ALLZEROS());
    ELSIF (NZ(AND2(comb.nanA, INV(AND2(comb.nanB, comb.signOpB)))));
        TEXT("when both values are NaN, value B has higher priority if sign=1");
        SETBIT(comb.resAdd, 51, CONST("1", 1));
        SETBITS(comb.resAdd, 50, 0, BITS(a, 50, 0));
    ELSIF (NZ(comb.nanB));
        SETBIT(comb.resAdd, 51, CONST("1", 1));
        SETBITS(comb.resAdd, 50, 0, BITS(b, 50, 0));
    ELSIF (NZ(comb.overflow));
        SETBITS(comb.resAdd, 51, 0, ALLZEROS());
    ELSE();
        SETBITS(comb.resAdd, 51, 0, ADD2(BITS(comb.mantShort, 51, 0), comb.rndBit));
    ENDIF();

TEXT();
    SETBITS(comb.resEQ, 63, 1, ALLZEROS());
    SETBIT(comb.resEQ, 0, flEqual);

TEXT();
    SETBITS(comb.resLT, 63, 1, ALLZEROS());
    SETBIT(comb.resLT, 0, flLess);

TEXT();
    SETBITS(comb.resLE, 63, 1, ALLZEROS());
    SETBIT(comb.resLE, 0, OR2(flLess, flEqual));

TEXT();
    IF (NZ(OR2(comb.nanA, comb.nanB)));
        SETVAL(comb.resMax, b);
    ELSIF (NZ(flMore));
        SETVAL(comb.resMax, a);
    ELSE();
        SETVAL(comb.resMax, b);
    ENDIF();

TEXT();
    IF (NZ(OR2(comb.nanA, comb.nanB)));
        SETVAL(comb.resMin, b);
    ELSIF (NZ(flLess));
        SETVAL(comb.resMin, a);
    ELSE();
        SETVAL(comb.resMin, b);
    ENDIF();

TEXT();
    IF (NZ(BIT(ena, 6)));
        IF (NZ(eq));
            SETVAL(result, comb.resEQ);
        ELSIF (NZ(lt));
            SETVAL(result, comb.resLT);
        ELSIF (NZ(le));
            SETVAL(result, comb.resLE);
        ELSIF (NZ(max));
            SETVAL(result, comb.resMax);
        ELSIF (NZ(min));
            SETVAL(result, comb.resMin);
        ELSE();
            SETVAL(result, comb.resAdd);
        ENDIF();

TEXT();
        SETVAL(illegal_op, OR2(comb.nanA, comb.nanB));
        SETVAL(overflow, comb.overflow);

TEXT();
        SETZERO(busy);
        SETZERO(add);
        SETZERO(sub);
        SETZERO(eq);
        SETZERO(lt);
        SETZERO(le);
        SETZERO(max);
        SETZERO(min);
    ENDIF();

TEXT();
    SYNC_RESET();

TEXT();
    SETVAL(o_res, result);
    SETVAL(o_illegal_op, illegal_op);
    SETVAL(o_overflow, overflow);
    SETVAL(o_valid, BIT(ena, 7));
    SETVAL(o_busy, busy);
}

