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

#include "int_div.h"

IntDiv::IntDiv(GenObject *parent, const char *name) :
    ModuleObject(parent, "IntDiv", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_ena(this, "i_ena", "1", "Enable bit"),
    i_unsigned(this, "i_unsigned", "1", "Unsigned operands"),
    i_rv32(this, "i_rv32", "1", "32-bits operands enabled"),
    i_residual(this, "i_residual", "1", "Compute: 0 =division; 1=residual"),
    i_a1(this, "i_a1", "RISCV_ARCH", "Operand 1"),
    i_a2(this, "i_a2", "RISCV_ARCH", "Operand 2"),
    o_res(this, "o_res", "RISCV_ARCH", "Result"),
    o_valid(this, "o_valid", "1", "Result is valid"),
    // signals
    wb_divisor0_i(this, "wb_divisor0_i", "124"),
    wb_divisor1_i(this, "wb_divisor1_i", "124"),
    wb_resid0_o(this, "wb_resid0_o", "64"),
    wb_resid1_o(this, "wb_resid1_o", "64"),
    wb_bits0_o(this, "wb_bits0_o", "4"),
    wb_bits1_o(this, "wb_bits1_o", "4"),
    // registers
    rv32(this, "rv32", "1"),
    resid(this, "resid", "1"),
    invert(this, "invert", "1"),
    div_on_zero(this, "div_on_zero", "1"),
    overflow(this, "overflow", "1"),
    busy(this, "busy", "1"),
    ena(this, "ena", "10"),
    divident_i(this, "divident_i", "64"),
    divisor_i(this, "divisor_i", "120"),
    bits_i(this, "bits_i", "64"),
    result(this, "result", "RISCV_ARCH"),
    reference_div(this, "reference_div", "RISCV_ARCH"),
    a1_dbg(this, "a1_dbg", "64", "0", "Store this value for output in a case of error"),
    a2_dbg(this, "a2_dbg", "64"),
    // process
    comb(this),
    stage0(this, "stage0"),
    stage1(this, "stage1")
{
    Operation::start(this);

    NEW(stage0, stage0.getName().c_str());
        CONNECT(stage0, 0, stage0.i_divident, divident_i);
        CONNECT(stage0, 0, stage0.i_divisor, wb_divisor0_i);
        CONNECT(stage0, 0, stage0.o_bits, wb_bits0_o);
        CONNECT(stage0, 0, stage0.o_resid, wb_resid0_o);
    ENDNEW();

TEXT();
    NEW(stage1, stage1.getName().c_str());
        CONNECT(stage1, 0, stage1.i_divident, wb_resid0_o);
        CONNECT(stage1, 0, stage1.i_divisor, wb_divisor1_i);
        CONNECT(stage1, 0, stage1.o_bits, wb_bits1_o);
        CONNECT(stage1, 0, stage1.o_resid, wb_resid1_o);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void IntDiv::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;

    IF (NZ(i_rv32));
        IF (OR2(NZ(i_unsigned), EZ(BIT(i_a1, 31))));
            SETBITS(comb.vb_a1, 31, 0, BITS(i_a1, 31, 0));
        ELSE();
            SETBITS(comb.vb_a1, 31, 0, INC(INV_L(BITS(i_a1, 31, 0))));
        ENDIF();
        IF (OR2(NZ(i_unsigned), EZ(BIT(i_a2, 31))));
            SETBITS(comb.vb_a2, 31, 0, BITS(i_a2, 31, 0));
        ELSE();
            SETBITS(comb.vb_a2, 31, 0, INC(INV_L(BITS(i_a2, 31, 0))));
        ENDIF();
    ELSE();
        IF (OR2(NZ(i_unsigned), EZ(BIT(i_a1, 63))));
            SETBITS(comb.vb_a1, 63, 0,  i_a1);
        ELSE();
            SETBITS(comb.vb_a1, 63, 0, INC(INV_L(i_a1)));
        ENDIF();
        IF (OR2(NZ(i_unsigned), EZ(BIT(i_a2, 63))));
            SETBITS(comb.vb_a2, 63, 0, i_a2);
        ELSE();
            SETBITS(comb.vb_a2, 63, 0, INC(INV_L(i_a2)));
        ENDIF();
    ENDIF();

TEXT();
    IF(AND2(NZ(BIT(comb.vb_a1, 63)), EZ(BITS(comb.vb_a1, 62, 0))));
        SETONE(comb.v_a1_m0, "= (1ull << 63)");
    ENDIF();
    IF (NZ(AND_REDUCE(comb.vb_a2)));
        SETONE(comb.v_a2_m1, "= -1ll");
    ENDIF();

TEXT();
    SETVAL(comb.v_ena, AND2(i_ena, INV(busy)));
    SETVAL(ena, CC2(BITS(ena, 8, 0), comb.v_ena));

TEXT();
    IF (NZ(invert));
        SETVAL(comb.vb_rem, INC(INV_L(BIG_TO_U64(divident_i))));
    ELSE();
        SETVAL(comb.vb_rem, divident_i);
    ENDIF();

TEXT();
    IF (NZ(invert));
        SETVAL(comb.vb_div, INC(INV_L(BIG_TO_U64(bits_i))));
    ELSE();
        SETVAL(comb.vb_div, bits_i);
    ENDIF();

TEXT();
    TEXT("DIVW, DIVUW, REMW and REMUW sign-extended accordingly with");
    TEXT("User Level ISA v2.2");
    IF (NZ(rv32) );
        SETBITS(comb.vb_div, 63, 32, CONST("0", 32));
        SETBITS(comb.vb_rem, 63, 32, CONST("0", 32));
        IF (NZ(BIT(comb.vb_div,31)));
            SETBITS(comb.vb_div,63, 32, ALLONES());
        ENDIF();
        IF (NZ(BIT(comb.vb_rem, 31)));
            SETBITS(comb.vb_rem, 63, 32, ALLONES());
        ENDIF();
    ENDIF();

TEXT();
    IF (NZ(i_ena));
        SETONE(busy);
        SETVAL(rv32, i_rv32);
        SETVAL(resid, i_residual);

TEXT();
        SETVAL(divident_i, comb.vb_a1);
        SETBITS(comb.t_divisor, 119, 56, comb.vb_a2);
        SETVAL(divisor_i, comb.t_divisor);

        SETVAL(comb.v_invert32,
            ANDx(2, &INV(i_unsigned),
                    &ORx(2, &AND2(INV(i_residual), XOR2(BIT(i_a1, 31), BIT(i_a2, 31))),
                            &AND2(i_residual, BIT(i_a1, 31)))));
        SETVAL(comb.v_invert64,
            ANDx(2, &INV(i_unsigned),
                    &ORx(2, &AND2(INV(i_residual), XOR2(BIT(i_a1, 63), BIT(i_a2, 63))),
                           &AND2(i_residual,  BIT(i_a1, 63)))));
        SETVAL(invert, ORx(2, &AND2(INV(i_rv32), comb.v_invert64),
                              &AND2(i_rv32, comb.v_invert32)));

TEXT();
        IF (NZ(i_rv32));
            IF (NZ(i_unsigned));
                SETZERO(div_on_zero);
                IF (EZ(BITS(i_a2, 31, 0)));
                    SETONE(div_on_zero);
                ENDIF();
                SETZERO(overflow);
            ELSE();
                SETZERO(div_on_zero);
                IF (EZ(BITS(i_a2, 30, 0)));
                    SETONE(div_on_zero);
                ENDIF();
                SETVAL(overflow, AND2(comb.v_a1_m0, comb.v_a2_m1));
            ENDIF();
        ELSE();
            IF (NZ(i_unsigned));
                SETZERO(div_on_zero);
                IF (EZ(BITS(i_a2, 63, 0)));
                    SETONE(div_on_zero);
                ENDIF();
                SETZERO(overflow);
            ELSE();
                SETZERO(div_on_zero);
                IF (EZ(BITS(i_a2, 62, 0)));
                    SETONE(div_on_zero);
                ENDIF();
                SETVAL(overflow, AND2(comb.v_a1_m0, comb.v_a2_m1));
            ENDIF();
        ENDIF();

        SETVAL(a1_dbg, i_a1);
        SETVAL(a2_dbg, i_a2);
        TEXT("v.reference_div = compute_reference(i_unsigned.read(), i_rv32.read(),");
        TEXT("                             i_residual.read(),");
        TEXT("                             i_a1.read(), i_a2.read());");
    ELSIF (NZ(BIT(ena, 8)));
        SETZERO(busy);
        IF (NZ(resid));
            IF (NZ(overflow));
                SETZERO(result);
            ELSE();
                SETVAL(result, comb.vb_rem);
            ENDIF();
        ELSIF (NZ(div_on_zero));
            SETVAL(result, ALLONES());
        ELSIF (NZ(overflow));
            SETVAL(result, CONST("0x8000000000000000", 64));
        ELSE();
            SETVAL(result, comb.vb_div);
        ENDIF();
    ELSIF (NZ(busy));
        SETVAL(divident_i, wb_resid1_o);
        SETVAL(divisor_i, CC2(CONST("0", 8), BITS(divisor_i, 119, 8)));
        SETVAL(bits_i, CC3(bits_i, wb_bits0_o, wb_bits1_o));
    ENDIF();

    SETVAL(wb_divisor0_i, CC2(divisor_i, CONST("0", 4)));
    SETVAL(wb_divisor1_i, CC2(CONST("0", 4), divisor_i));

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_res, result);
    SETVAL(o_valid, BIT(ena, 9));
}

