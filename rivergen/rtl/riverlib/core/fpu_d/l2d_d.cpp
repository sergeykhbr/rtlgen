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

#include "l2d_d.h"

Long2Double::Long2Double(GenObject *parent, const char *name) :
    ModuleObject(parent, "Long2Double", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_ena(this, "i_ena", "1"),
    i_signed(this, "i_signed", "1"),
    i_w32(this, "i_w32", "1"),
    i_a(this, "i_a", "64", "Operand 1"),
    o_res(this, "o_res", "64", "Result"),
    o_valid(this, "o_valid", "1", "Result is valid"),
    o_busy(this, "o_busy", "1", "Multiclock instruction under processing"),
    // registers
    busy(this, "busy", "1"),
    ena(this, "ena", "3"),
    signA(this, "signA", "1"),
    absA(this, "absA", "64"),
    result(this, "result", "64"),
    op_signed(this, "op_signed", "1"),
    mantAlign(this, "mantAlign", "64"),
    lshift(this, "lshift", "6"),
    // process
    comb(this)
{
    Operation::start(&comb);
    proc_comb();
}

void Long2Double::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;

    SETVAL(comb.v_ena, AND2(i_ena, INV(busy)));
    SETVAL(ena, CC2(BITS(ena, 1, 0), comb.v_ena));
    IF (EZ(i_w32));
        SETVAL(comb.v_signA, BIT(i_a, 63));
        SETVAL(comb.vb_A, i_a);
    ELSIF (NZ(AND2(i_signed, BIT(i_a, 31))));
        SETONE(comb.v_signA);
        SETBITS(comb.vb_A, 63, 32, ALLONES());
        SETBITS(comb.vb_A, 31, 0, BITS(i_a, 31, 0));
    ELSE();
        SETZERO(comb.v_signA);
        SETBITS(comb.vb_A, 31, 0, BITS(i_a, 31, 0));
        SETBITS(comb.vb_A, 63, 32, ALLZEROS());
    ENDIF();

TEXT();
    IF (NZ(i_ena));
        SETONE(busy);
        IF (NZ(AND2(i_signed, comb.v_signA)));
            SETONE(signA);
            SETVAL(absA, INC(INV_L(comb.vb_A)));
        ELSE();
            SETZERO(signA);
            SETVAL(absA, comb.vb_A);
        ENDIF();
        SETVAL(op_signed, i_signed);
    ENDIF();

TEXT();
    TEXT("multiplexer, probably if/elsif in rtl:");
    SETVAL(lshift, CONST("63", 6));
    IF (NZ(BIT(absA ,63)));
        SETVAL(comb.mantAlign, absA);
    ELSE();
        GenObject &i = FOR("i", CONST("1"), CONST("64"), "++");
            IF (AND2(EQ(comb.lshift, CONST("63", 6)), NZ(BIT(absA, SUB2(CONST("63"), i)))));
                SETVAL(comb.mantAlign, LSH(absA, i));
                SETVAL(comb.lshift, i);
            ENDIF();
        ENDFOR();
    ENDIF();

TEXT();
    IF (NZ(BIT(ena, 0)));
        SETVAL(mantAlign, comb.mantAlign);
        SETVAL(lshift, comb.lshift);
    ENDIF();

TEXT();
    IF (EZ(absA));
        SETZERO(comb.expAlign);
    ELSE();
        SETVAL(comb.expAlign, SUB2(CONST("1086", 11), lshift));
    ENDIF();

TEXT();
    SETVAL(comb.mantEven, BIT(mantAlign, 11));
    IF (EQ(BITS(mantAlign, 10, 0), CONST("0x7ff", 11)));
        SETONE(comb.mant05);
    ENDIF();
    SETVAL(comb.rndBit, AND2(BIT(mantAlign, 10), INV(AND2(comb.mant05, comb.mantEven))));
    IF (EQ(BITS(mantAlign, 63, 11), CONST("0x001fffffffffffff", 52)));
        SETONE(comb.mantOnes);
    ENDIF();

TEXT();
    TEXT("Result multiplexers:");
    SETBIT(comb.res, 63, AND2(signA, op_signed));
    SETBITS(comb.res, 62, 52, ADD2(comb.expAlign, AND2(comb.mantOnes, comb.rndBit)));
    SETBITS(comb.res, 51, 0, ADD2(BITS(mantAlign, 62, 11), comb.rndBit));

TEXT();
    IF (NZ(BIT(ena, 1)));
        SETVAL(result, comb.res);
        SETZERO(busy);
    ENDIF();

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_res, result);
    SETVAL(o_valid, BIT(ena, 2));
    SETVAL(o_busy, busy);
}

