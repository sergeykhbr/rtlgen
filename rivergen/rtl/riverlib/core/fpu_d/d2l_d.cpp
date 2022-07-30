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

#include "d2l_d.h"

Double2Long::Double2Long(GenObject *parent, const char *name) :
    ModuleObject(parent, "Double2Long", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_ena(this, "i_ena", "1"),
    i_signed(this, "i_signed", "1"),
    i_w32(this, "i_w32", "1"),
    i_a(this, "i_a", "64", "Operand 1"),
    o_res(this, "o_res", "64", "Result"),
    o_overflow(this, "o_overflow", "1"),
    o_underflow(this, "o_underflow", "1"),
    o_valid(this, "o_valid", "1", "Result is valid"),
    o_busy(this, "o_busy", "1", "Multiclock instruction under processing"),
    // registers
    busy(this, "busy", "1"),
    ena(this, "ena", "3"),
    signA(this, "signA", "1"),
    expA(this, "expA", "11"),
    mantA(this, "mantA", "53"),
    result(this, "result", "64"),
    op_signed(this, "op_signed", "1"),
    w32(this, "w32", "1"),
    mantPostScale(this, "mantPostScale", "64"),
    overflow(this, "overflow", "1"),
    underflow(this, "underflow", "1"),
    // process
    comb(this)
{
    Operation::start(&comb);
    proc_comb();
}

void Double2Long::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;
    GenObject *i;

    SETVAL(comb.v_ena, AND2(i_ena, INV(busy)));
    SETVAL(ena, CC2(BITS(ena, 1, 0), comb.v_ena));

TEXT();
    SETBITS(comb.mantA, 51, 0, BITS(i_a, 51, 0));
    SETBIT(comb.mantA, 52, CONST("0", 1));
    IF (NZ(BITS(i_a, 62, 52)));
        SETBIT(comb.mantA, 52, CONST("1", 1));
    ENDIF();

TEXT();
    IF (NZ(i_ena));
        SETONE(busy);
        SETVAL(signA, BIT(i_a, 63));
        SETVAL(expA, BITS(i_a, 62, 52));
        SETVAL(mantA, comb.mantA);
        SETVAL(op_signed, i_signed);
        SETVAL(w32, i_w32);
        SETZERO(overflow);
        SETZERO(underflow);
    ENDIF();

TEXT();
    TEXT("(1086 - expA)[5:0]");
    SETVAL(comb.expShift, SUB2(CONST("0x3e", 6), BITS(expA, 5, 0)));
    IF (NZ(w32));
        IF (NZ(op_signed));
            SETVAL(comb.expMax, CONST("1053", 11));
        ELSE();
            SETVAL(comb.expMax, CONST("1085", 11));
        ENDIF();
    ELSE();
        IF (NZ(OR2(op_signed, signA)));
            SETVAL(comb.expMax, CONST("1085", 11));
        ELSE();
            SETVAL(comb.expMax, CONST("1086", 11));
        ENDIF();
    ENDIF();
    SETVAL(comb.expDif, SUB2(CC2(CONST("0", 1), comb.expMax), CC2(CONST("0", 1), expA)));

TEXT();
    SETVAL(comb.expDif_gr, BIT(comb.expDif, 11));
    SETZERO(comb.expDif_lt);
    IF (AND2(NE(expA, CONST("0x3FF", 11)), EZ(BIT(expA, 10))));
        SETONE(comb.expDif_lt);
    ENDIF();

TEXT();
    SETVAL(comb.mantPreScale, CC2(BIG_TO_U64(mantA), CONST("0", 11)));

TEXT();
    SETZERO(comb.mantPostScale);
    IF (NZ(comb.expDif_gr));
        SETONE(comb.overflow);
        SETZERO(comb.underflow);
    ELSIF (NZ(comb.expDif_lt));
        SETZERO(comb.overflow);
        SETONE(comb.underflow);
    ELSE();
        SETZERO(comb.overflow);
        SETZERO(comb.underflow);
        TEXT("Multiplexer, probably switch case in rtl");
        i = &FOR ("i", CONST("0"), CONST("64"), "++");
            IF (EQ(comb.expShift, *i));
                SETVAL(comb.mantPostScale, RSH(comb.mantPreScale, *i));
            ENDIF();
        ENDFOR();
    ENDIF();

TEXT();
    IF (NZ(BIT(ena, 0)));
        SETVAL(overflow, comb.overflow);
        SETVAL(underflow, comb.underflow);
        SETVAL(mantPostScale, comb.mantPostScale);
    ENDIF();

TEXT();
    TEXT("Result multiplexers:");
    SETVAL(comb.resSign, AND2(OR2(signA, overflow), INV(underflow)));
    IF (NZ(signA));
        SETVAL(comb.resMant, INC(INV_L(mantPostScale)));
    ELSE();
        SETVAL(comb.resMant, mantPostScale);
    ENDIF();

TEXT();
    SETVAL(comb.res, comb.resMant);
    IF (NZ(op_signed));
        IF (NZ(comb.resSign));
            IF (NZ(w32));
                SETBITS(comb.res, 63, 31, ALLONES());
            ELSE();
                SETBIT(comb.res, 63, CONST("1", 1));
            ENDIF();
        ENDIF();
    ELSE();
        IF (NZ(w32));
            SETBITS(comb.res, 63, 32, ALLZEROS());
        ELSIF (NZ(overflow));
            SETBIT(comb.res, 63, CONST("1", 1));
        ENDIF();
    ENDIF();

TEXT();
    IF (NZ(BIT(ena, 1)));
        SETVAL(result, comb.res);
        SETZERO(busy);
    ENDIF();

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_res, result);
    SETVAL(o_overflow, overflow);
    SETVAL(o_underflow, underflow);
    SETVAL(o_valid, BIT(ena, 2));
    SETVAL(o_busy, busy);
}

