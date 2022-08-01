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

DoubleAdd::DoubleAdd(GenObject *parent, const char *name) :
    ModuleObject(parent, "DoubleAdd", name),
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
    ena(this, "ena", "8"),
    a(this, "a", "64"),
    b(this, "b", "64"),
    result(this, "result", "64"),
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
    preShift(this, "preShift", "12"),
    signOpMore(this, "signOpMore", "1"),
    expMore(this, "expMore", "11"),
    mantMore(this, "mantMore", "53"),
    mantLess(this, "mantLess", "53"),
    mantLessScale(this, "mantLessScale", "105"),
    mantSum(this, "mantSum", "106"),
    lshift(this, "lshift", "7"),
    mantAlign(this, "mantAlign", "105"),
    expPostScale(this, "expPostScale", "12"),
    expPostScaleInv(this, "expPostScaleInv", "12"),
    mantPostScale(this, "mantPostScale", "105"),
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
    SETVAL(ena, CC2(BITS(ena, 1, 0), comb.v_ena));

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
    if (r.mantSum.read()[105] == 1) {
        TEXT("shift right");
        vb_lshift = 0x7F;
    } else if (r.mantSum.read()[104] == 1) {
        vb_lshift = 0;
    } else if (vb_lshift_p1 != 0) {
        vb_lshift = vb_lshift_p1;
    } else {
        vb_lshift = vb_lshift_p2;
    }
    if (r.ena.read()[3] == 1) {
        v.lshift = vb_lshift;
    }

TEXT();
    TEXT("Prepare to mantissa post-scale");
    vb_mantAlign = 0;
    if (r.lshift.read() == 0x7F) {
        vb_mantAlign = r.mantSum.read() >> 1;
    } else if (r.lshift.read() == 0) {
        vb_mantAlign = r.mantSum.read();
    } else {
        for (unsigned i = 1; i < 105; i++) {
            if (i == r.lshift.read()) {
                vb_mantAlign = r.mantSum.read() << i;
            }
        }
    }
    if (r.lshift.read() == 0x7F) {
        if (r.expMore.read() == 0x7FF) {
            vb_expPostScale = (0, r.expMore);
        } else {
            vb_expPostScale = (0, r.expMore.read()) + 1;
        }
    } else {
        if (r.expMore.read() == 0 && r.lshift.read() == 0) {
            vb_expPostScale = 1;
        } else {
            vb_expPostScale = (0, r.expMore.read()) - (0, r.lshift.read());
        }
    }
    if (signA ^ signOpB) {
        // subtractor only: result value becomes with exp=0
        if (r.expMore.read() != 0 && 
            (vb_expPostScale[11] == 1 || vb_expPostScale == 0)) {
            vb_expPostScale -= 1;
        }
    }
    if (r.ena.read()[4] == 1) {
        v.mantAlign = vb_mantAlign;
        v.expPostScale = vb_expPostScale;
        v.expPostScaleInv = ~vb_expPostScale + 1;
    }

TEXT();
    TEXT("Mantissa post-scale:");
    TEXT("   Scaled = SumScale>>(-ExpSum) only if ExpSum < 0;");
    vb_mantPostScale = r.mantAlign;
    if (r.expPostScale.read()[11] == 1) {
        for (unsigned i = 1; i < 105; i++) {
            if (i == r.expPostScaleInv.read()) {
                vb_mantPostScale = r.mantAlign.read() >> i;
            }
        }
    }
    if (r.ena.read()[5] == 1) {
        v.mantPostScale = vb_mantPostScale;
    }

TEXT();
    TEXT("Rounding bit");
    mantShort = r.mantPostScale.read().range(104, 52).to_uint64();
    tmpMant05 = r.mantPostScale.read().range(51, 0).to_uint64();
    mantOnes = 0;
    if (mantShort == 0x001fffffffffffff) {
        mantOnes = 1;
    }
    mantEven = r.mantPostScale.read()[52];
    mant05 = 0;
    if (tmpMant05 == 0x0008000000000000) {
        mant05 = 1;
    }
    rndBit = r.mantPostScale.read()[51] & !(mant05 & !mantEven);

TEXT();
    // Check borders
    mantZeroA = 0;
    if (r.a.read()(51, 0) == 0) {
        mantZeroA = 1;
    }
    mantZeroB = 0;
    if (r.b.read()(51, 0) == 0) {
        mantZeroB = 1;
    }

TEXT();
    // Exceptions
    allZero = 0;
    if (r.a.read()(62, 0) == 0 && r.b.read()(62, 0) == 0) {
        allZero = 1;
    }
    sumZero = 0;
    if (r.mantPostScale.read() == 0) {
        sumZero = 1;
    }
    nanA = 0;
    if (r.a.read()(62, 52) == 0x7ff) {
        nanA = 1;
    }
    nanB = 0;
    if (r.b.read()(62, 52) == 0x7ff) {
        nanB = 1;
    }
    nanAB = nanA && mantZeroA && nanB && mantZeroB;
    overflow = 0;
    if (r.expPostScale.read() == 0x7FF) {   // positive
        overflow = 1;
    }

TEXT();
    // Result multiplexers:
    if (nanAB && signOp) {
        resAdd[63] = signA ^ signOpB;
    } else if (nanA) {
        /** when both values are NaN, value B has higher priority if sign=1 */
        resAdd[63] = signA || (nanB && signOpB);
    } else if (nanB) {
        resAdd[63] = signOpB ^ (signOp && !mantZeroB);
    } else if (allZero) {
        resAdd[63] = signA && signOpB;
    } else if (sumZero) {
        resAdd[63] = 0;
    } else {
        resAdd[63] = r.signOpMore; 
    }

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
    if (nanA & mantZeroA & nanB & mantZeroB) {
        resAdd[51] = signOp;
        resAdd(50, 0) = 0;
    } else if (nanA && !(nanB && signOpB)) {
        /** when both values are NaN, value B has higher priority if sign=1 */
        resAdd[51] = 1;
        resAdd(50, 0) = r.a.read()(50, 0);
    } else if (nanB) {
        resAdd[51] = 1;
        resAdd(50, 0) = r.b.read()(50, 0);
    } else if (overflow) {
        resAdd(51, 0) = 0;
    } else {
        resAdd(51, 0) = mantShort(51, 0) + rndBit;
    }

TEXT();
    resEQ(63, 1) = 0;
    resEQ[0] = r.flEqual;

TEXT();
    resLT(63, 1) = 0;
    resLT[0] = r.flLess;

TEXT();
    resLE(63, 1) = 0;
    resLE[0] = r.flLess | r.flEqual;

TEXT();
    if (nanA | nanB) {
        resMax = r.b;
    } else if (r.flMore.read() == 1) {
        resMax = r.a;
    } else {
        resMax = r.b;
    }

TEXT();
    if (nanA | nanB) {
        resMin = r.b;
    } else if (r.flLess.read() == 1) {
        resMin = r.a;
    } else {
        resMin = r.b;
    }

TEXT();
    if (r.ena.read()[6] == 1) {
        if (r.eq.read() == 1) {
            v.result = resEQ;
        } else if (r.lt.read() == 1) {
            v.result = resLT;
        } else if (r.le.read() == 1) {
            v.result = resLE;
        } else if (r.max.read() == 1) {
            v.result = resMax;
        } else if (r.min.read() == 1) {
            v.result = resMin;
        } else {
            v.result = resAdd;
        }

TEXT();
        v.illegal_op = nanA | nanB;
        v.overflow = overflow;

TEXT();
        v.busy = 0;
        v.add = 0;
        v.sub = 0;
        v.eq = 0;
        v.lt = 0;
        v.le = 0;
        v.max = 0;
        v.min = 0;
    }

TEXT();
    SYNC_RESET(*this);

}

