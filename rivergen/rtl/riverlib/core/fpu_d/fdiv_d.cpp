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
            IF (AND2(EZ(comb.preShift), NZ(BIT(comb.mantB, SUB2(CONST("52"), *i)))));
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
    SETVAL(comb.expShift, SUB2(CC2(CONST("0", 6), preShift), CC2(CONST("0", 5), wb_idiv_lshift)));
    IF (AND2(EZ(BITS(b, 62, 52)), NZ(BITS(a, 62, 52))));
        SETVAL(comb.expShift, DEC(comb.expShift));
    ELSIF (AND2(NZ(BITS(b, 62, 52)), EZ(BITS(a, 62, 52))));
        SETVAL(comb.expShift, INC(comb.expShift));
    ENDIF();

TEXT();
    SETVAL(comb.expAlign, ADD2(expAB, CC2(BIT(comb.expShift, 11), comb.expShift)));
    IF (NZ(BIT(comb.expAlign, 12)));
        SETVAL(comb.postShift, ADD2(INV_L(BITS(comb.expAlign, 11, 0)), CONST("2", 12)));
    ELSE();
        SETZERO(comb.postShift);
    ENDIF();

TEXT();
    IF (NZ(w_idiv_rdy));
        SETVAL(expAlign, BITS(comb.expAlign, 11, 0));
        SETVAL(mantAlign, comb.mantAlign);
        SETVAL(postShift, comb.postShift);

TEXT();
        TEXT("Exceptions:");
        SETZERO(nanRes);
        IF (EQ(comb.expAlign, CONST("0x7FF", 13)));
            SETONE(nanRes);
        ENDIF();
        SETVAL(overflow, AND2(INV(BIT(comb.expAlign, 12)), BIT(comb.expAlign, 11)));
        SETVAL(underflow, AND2(BIT(comb.expAlign, 12), BIT(comb.expAlign, 11)));
    ENDIF();

TEXT();
    TEXT("Prepare to mantissa post-scale");
    IF (EZ(postShift));
        SETVAL(comb.mantPostScale, mantAlign);
    ELSIF (LS(postShift, CONST("105", 12)));
        i = &FOR ("i", CONST("0"), CONST("105"), "++");
            IF (EQ(*i, postShift));
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
    IF (AND4(comb.nanA, comb.mantZeroA, comb.nanB, comb.mantZeroB));
        SETBIT(comb.res, 63, CONST("1", 1));
    ELSIF (AND2(comb.nanA, INV(comb.mantZeroA)));
        SETBIT(comb.res, 63, comb.signA);
    ELSIF (AND2(comb.nanB, INV(comb.mantZeroB)));
        SETBIT(comb.res, 63, comb.signB);
    ELSIF (AND2(zeroA, zeroB));
        SETBIT(comb.res, 63, CONST("1", 1));
    ELSE();
        SETBIT(comb.res, 63, XOR2(BIT(a, 63), BIT(b, 63)));
    ENDIF();

TEXT();
    IF (NZ(AND2(comb.nanB, INV(comb.mantZeroB))));
        SETBITS(comb.res, 62, 52, BITS(b, 62, 52));
    ELSIF (NZ(AND2(OR2(underflow, zeroA), INV(zeroB))));
        SETBITS(comb.res, 62, 52, ALLZEROS());
    ELSIF (NZ(OR2(overflow, zeroB)));
        SETBITS(comb.res, 62, 52, ALLONES());
    ELSIF (NZ(comb.nanA));
        SETBITS(comb.res, 62, 52, BITS(a, 62, 52));
    ELSIF (NZ(OR2(AND2(comb.nanB, comb.mantZeroB), BIT(expAlign, 11))));
        SETBITS(comb.res, 62, 52, ALLZEROS());
    ELSE();
        SETBITS(comb.res, 62, 52, ADD2(BITS(expAlign, 10, 0),
                       AND3(comb.mantOnes, comb.rndBit, INV(overflow))));
    ENDIF();

TEXT();
    IF (NZ(ORx(2, &AND2(zeroA, zeroB),
                  &AND4(comb.nanA, comb.mantZeroA, comb.nanB, comb.mantZeroB))));
        SETBIT(comb.res, 51, CONST("1", 1));
        SETBITS(comb.res, 50, 0, ALLZEROS());
    ELSIF (NZ(AND2(comb.nanA, INV(comb.mantZeroA))));
        SETBIT(comb.res, 51, CONST("1", 1));
        SETBITS(comb.res, 50, 0, BITS(a, 50, 0));
    ELSIF (NZ(AND2(comb.nanB, INV(comb.mantZeroB))));
        SETBIT(comb.res, 51, CONST("1", 1));
        SETBITS(comb.res, 50, 0, BITS(b, 50, 0));
    ELSIF (NZ(ORx(4, &overflow,
                     &nanRes,
                     &AND2(comb.nanA, comb.mantZeroA),
                     &AND2(comb.nanB, comb.mantZeroB))));
        SETBITS(comb.res, 51, 0, ALLZEROS());
    ELSE();
        SETBITS(comb.res, 51, 0, ADD2(BITS(comb.mantShort, 51, 0), comb.rndBit));
    ENDIF();

TEXT();
    IF (NZ(BIT(ena, 3)));
        SETVAL(result, comb.res);
        SETVAL(illegal_op, OR2(comb.nanA, comb.nanB));
        SETZERO(busy);
    ENDIF();

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

