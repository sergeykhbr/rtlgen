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

#include "fdiv_d.h"

DoubleDiv::DoubleDiv(GenObject *parent, const char *name) :
    ModuleObject(parent, "DoubleDiv", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_ena(this, "i_ena", "1"),
    i_a(this, "i_a", "64", "Operand 1"),
    i_b(this, "i_b", "64", "Operand 2"),
    o_res(this, "o_res", "64", "Result"),
    o_illegal_op(this, "o_illegal_op", "1"),
    o_divbyzero(this, "o_divbyzero", "1"),
    o_overflow(this, "o_overflow", "1"),
    o_underflow(this, "o_underflow", "1"),
    o_valid(this, "o_valid", "1", "Result is valid"),
    o_busy(this, "o_busy", "1", "Multiclock instruction under processing"),
    // signals
    w_idiv_ena(this, "w_idiv_ena", "1"),
    wb_divident(this, "wb_divident", "53"),
    wb_divisor(this, "wb_divisor", "53"),
    wb_idiv_result(this, "wb_idiv_result", "105"),
    wb_idiv_lshift(this, "wb_idiv_lshift", "7"),
    w_idiv_rdy(this, "w_idiv_rdy", "1"),
    w_idiv_overflow(this, "w_idiv_overflow", "1"),
    w_idiv_zeroresid(this, "w_idiv_zeroresid", "1"),
    // registers
    busy(this, "busy", "1"),
    ena(this, "ena", "5"),
    a(this, "a", "64"),
    b(this, "b", "64"),
    result(this, "result", "64"),
    zeroA(this, "zeroA", "1"),
    zeroB(this, "zeroB", "1"),
    divisor(this, "divisor", "53"),
    preShift(this, "preShift", "6"),
    expAB(this, "expAB", "13"),
    expAlign(this, "expAlign", "12"),
    mantAlign(this, "mantAlign", "105"),
    postShift(this, "postShift", "12"),
    mantPostScale(this, "mantPostScale", "105"),
    nanRes(this, "nanRes", "1"),
    overflow(this, "overflow", "1"),
    underflow(this, "underflow", "1"),
    illegal_op(this, "illegal_op", "1"),
    // process
    comb(this),
    u_idiv53(this, "u_idiv53")
{
    Operation::start(this);

    NEW(u_idiv53, u_idiv53.getName().c_str());
        CONNECT(u_idiv53, 0, u_idiv53.i_nrst, i_nrst);
        CONNECT(u_idiv53, 0, u_idiv53.i_clk, i_clk);
        CONNECT(u_idiv53, 0, u_idiv53.i_ena, w_idiv_ena);
        CONNECT(u_idiv53, 0, u_idiv53.i_divident, wb_divident);
        CONNECT(u_idiv53, 0, u_idiv53.i_divisor, wb_divisor);
        CONNECT(u_idiv53, 0, u_idiv53.o_result, wb_idiv_result);
        CONNECT(u_idiv53, 0, u_idiv53.o_lshift, wb_idiv_lshift);
        CONNECT(u_idiv53, 0, u_idiv53.o_rdy, w_idiv_rdy);
        CONNECT(u_idiv53, 0, u_idiv53.o_overflow, w_idiv_overflow);
        CONNECT(u_idiv53, 0, u_idiv53.o_zero_resid, w_idiv_zeroresid);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void DoubleDiv::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;
    GenObject *i;

    SETBIT(comb.vb_ena, 0, AND2(i_ena, INV(busy)));
    SETBIT(comb.vb_ena, 1, BIT(ena, 0));
    SETBITS(comb.vb_ena, 4, 2, CC2(BITS(ena, 3, 2), w_idiv_rdy));
    SETVAL(ena, comb.vb_ena);

TEXT();
    IF (NZ(i_ena));
        SETONE(busy);
        SETZERO(overflow);
        SETZERO(underflow);
        SETZERO(illegal_op);
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
        SETVAL(comb.divisor, comb.mantB);
    ELSE();
        TEXT("multiplexer for operation with zero expanent");
        SETVAL(comb.divisor, comb.mantB);
        i = &FOR ("i", CONST("1"), CONST("53"), "++");
            IF (AND2(EZ(comb.preShift), NZ(BIT(comb.mantB, SUB2(CONST("52"), *i))));
                SETVAL(comb.divisor, LSH(comb.mantB, *i));
                SETVAL(comb.preShift, *i);
            ENDIF();
        ENDFOR();
    ENDIF();

TEXT();
    TEXT("expA - expB + 1023");
    SETVAL(comb.expAB_t, ADD2(CC2(CONST("0", 1), BITS(a, 62, 52)), CONST("1023", 12)));
    SETVAL(comb.expAB, SUB2(CC2(CONST("0", 1), comb.expAB_t), CC2(CONST("0", 2), BITS(b, 62, 52))), "signed value");

TEXT();
    IF (NZ(BIT(ena, 0)));
        SETVAL(divisor, comb.divisor);
        SETVAL(preShift, comb.preShift);
        SETVAL(expAB, comb.expAB);
        SETVAL(zeroA, comb.zeroA);
        SETVAL(zeroB, comb.zeroB);
    ENDIF();

TEXT();
    SETVAL(w_idiv_ena, BIT(ena, 1));
    SETVAL(wb_divident, comb.mantA);
    SETVAL(wb_divisor, divisor);

TEXT();
    TEXT("idiv53 module:");
    i = &FOR ("i", CONST("0"), CONST("105"), "++");
        IF (EQ(*i, wb_idiv_lshift));
            SETVAL(comb.mantAlign, LSH(wb_idiv_result, *i));
        ENDIF();
    ENDFOR();

TEXT();
    expShift = (0, r.preShift.read()) - (0, wb_idiv_lshift.read());
    if (r.b.read()(62, 52) == 0 && r.a.read()(62, 52) != 0) {
        expShift = expShift - 1;
    } else if (r.b.read()(62, 52) != 0 && r.a.read()(62, 52) == 0) {
        expShift = expShift + 1;
    }

TEXT();
    expAlign = r.expAB.read() + (expShift[11], expShift);
    if (expAlign[12] == 1) {
        postShift = ~expAlign(11, 0) + 2;
    } else {
        postShift = 0;
    }

TEXT();
    if (w_idiv_rdy == 1) {
        v.expAlign = expAlign(11, 0);
        v.mantAlign = mantAlign;
        v.postShift = postShift;

TEXT();
        TEXT("Exceptions:");
        v.nanRes = 0;
        if (expAlign == 0x7FF) {
            v.nanRes = 1;
        }
        v.overflow = !expAlign[12] && expAlign[11];
        v.underflow = expAlign[12] && expAlign[11];
    }

TEXT();
    TEXT("Prepare to mantissa post-scale");
    mantPostScale = 0;
    if (r.postShift.read() == 0) {
        mantPostScale = r.mantAlign.read();
    } else if (r.postShift.read() < 105) {
        for (unsigned i = 0; i < 105; i++) {
            if (i == r.postShift.read()) {
                mantPostScale = r.mantAlign.read() >> i;
            }
        }
    }
    if (r.ena.read()[2] == 1) {
        v.mantPostScale = mantPostScale;
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
    TEXT("Check Borders");
    nanA = 0;
    if (r.a.read()(62, 52) == 0x7ff) {
        nanA = 1;
    }
    nanB = 0;
    if (r.b.read()(62, 52) == 0x7ff) {
        nanB = 1;
    }
    mantZeroA = 0;
    if (r.a.read()(51, 0) == 0) {
        mantZeroA = 1;
    }
    mantZeroB = 0;
    if (r.b.read()(51, 0) == 0) {
        mantZeroB = 1;
    }

TEXT();
    TEXT("Result multiplexers:");
    if (nanA && mantZeroA && nanB && mantZeroB) {
        res[63] = 1;
    } else if (nanA && !mantZeroA) {
        res[63] = signA;
    } else if (nanB && !mantZeroB) {
        res[63] = signB;
    } else if (r.zeroA.read() && r.zeroB.read()) {
        res[63] = 1;
    } else {
        res[63] = r.a.read()[63] ^ r.b.read()[63];
    }

TEXT();
    if (nanB && !mantZeroB) {
        res(62, 52) = r.b.read()(62, 52);
    } else if ((r.underflow.read() || r.zeroA.read()) && !r.zeroB.read()) {
        res(62, 52) = 0;
    } else if (r.overflow.read() || r.zeroB.read()) {
        res(62, 52) = 0x7FF;
    } else if (nanA) {
        res(62, 52) = r.a.read()(62, 52);
    } else if ((nanB && mantZeroB) || r.expAlign.read()[11]) {
        res(62, 52) = 0;
    } else {
        res(62, 52) = r.expAlign.read()(10, 0)
                       + (mantOnes && rndBit && !r.overflow.read());
    }

TEXT();
    if ((r.zeroA.read() && r.zeroB.read())
        || (nanA & mantZeroA & nanB & mantZeroB)) {
        res[51] = 1;
        res(50, 0) = 0;
    } else if (nanA && !mantZeroA) {
        res[51] = 1;
        res(50, 0) = r.a.read()(50, 0);
    } else if (nanB && !mantZeroB) {
        res[51] = 1;
        res(50, 0) = r.b.read()(50, 0);
    } else if (r.overflow.read() | r.nanRes.read() | (nanA && mantZeroA)
        || (nanB && mantZeroB)) {
        res(51, 0) = 0;
    } else {
        res(51, 0) = mantShort(51, 0) + rndBit;
    }

TEXT();
    if (r.ena.read()[3] == 1) {
        v.result = res;
        v.illegal_op = nanA | nanB;
        v.busy = 0;
    }


TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_res, result);
    SETVAL(o_illegal_op, illegal_op);
    SETVAL(o_divbyzero, zeroB);
    SETVAL(o_overflow, overflow);
    SETVAL(o_underflow, underflow);
    SETVAL(o_valid, BIT(ena, 4));
    SETVAL(o_busy, busy);
}

