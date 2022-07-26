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

#include "int_mul.h"

IntMul::IntMul(GenObject *parent, const char *name) :
    ModuleObject(parent, "IntMul", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_ena(this, "i_ena", "1", "Enable bit"),
    i_unsigned(this, "i_unsigned", "1", "Unsigned operands"),
    i_hsu(this, "i_hsu", "1", "MULHSU instruction: signed * unsigned"),
    i_high(this, "i_high", "1", "High multiplied bits [127:64]"),
    i_rv32(this, "i_rv32", "1", "32-bits operands enabled"),
    i_a1(this, "i_a1", "RISCV_ARCH", "Operand 1"),
    i_a2(this, "i_a2", "RISCV_ARCH", "Operand 2"),
    o_res(this, "o_res", "RISCV_ARCH", "Result"),
    o_valid(this, "o_valid", "1", "Result is valid"),
    // registers
    busy(this, "busy", "1"),
    ena(this, "ena", "4"),
    a1(this, "a1", "RISCV_ARCH"),
    a2(this, "a2", "RISCV_ARCH"),
    unsign(this, "unsign", "1"),
    high(this, "high", "1"),
    rv32(this, "rv32", "1"),
    zero(this, "zero", "1"),
    inv(this, "inv", "1"),
    result(this, "result", "128"),
    a1_dbg(this, "a1_dbg", "RISCV_ARCH"),
    a2_dbg(this, "a2_dbg", "RISCV_ARCH"),
    reference_mul(this, "reference_mul", "RISCV_ARCH", "0", "Used for run-time comparision"),
    lvl1(this, "lvl1", "69", "16", true),
    lvl3(this, "lvl3", "83", "4", true),
    // process
    comb(this)
{
    Operation::start(&comb);
    proc_comb();
}

void IntMul::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;
    GenObject *i;

TEXT();
    IF (NZ(BITS(i_a1, 62, 0)));
        SETONE(comb.v_a1s_nzero);
    ENDIF();
    IF (NZ(AND2(comb.v_a1s_nzero, BIT(i_a1, 63))));
        SETVAL(comb.vb_a1s, INC(INV_L(i_a1)));
    ELSE();
        SETVAL(comb.vb_a1s, i_a1);
    ENDIF();

TEXT();
    IF (NZ(BITS(i_a2, 62, 0)));
        SETONE(comb.v_a2s_nzero);
    ENDIF();
    IF (NZ(AND2(comb.v_a2s_nzero, BIT(i_a2, 63))));
        SETVAL(comb.vb_a2s, INC(INV_L(i_a2)));
    ELSE();
        SETVAL(comb.vb_a2s, i_a2);
    ENDIF();

TEXT();
    SETVAL(comb.v_ena, AND2(i_ena, INV(busy)));
    SETVAL(ena, CC2(BITS(ena, 2, 0), comb.v_ena));

TEXT();
    IF (NZ(i_ena));
        SETONE(busy);
        SETZERO(inv);
        SETZERO(zero);
        IF (NZ(i_rv32));
            SETVAL(a1, BITS(i_a1, 31, 0));
            IF (AND2(EZ(i_unsigned), NZ(BIT(i_a1,31))));
                SETBITS(a1, 63, 32, ALLONES());
            ENDIF();
            SETVAL(a2, BITS(i_a2, 31, 0));
            IF (AND2(EZ(i_unsigned), NZ(BIT(i_a2, 31))));
                SETBITS(a2, 63, 32, ALLONES());
            ENDIF();
        ELSIF (NZ(i_high));
            IF (NZ(i_hsu));
                IF (OR2(EZ(comb.v_a1s_nzero), EZ(i_a2)));
                    SETONE(zero);
                ENDIF();
                SETVAL(inv, BIT(i_a1, 63));
                SETVAL(a1, comb.vb_a1s);
                SETVAL(a2, i_a2);
            ELSIF (NZ(i_unsigned));
                SETVAL(a1, i_a1);
                SETVAL(a2, i_a2);
            ELSE();
                SETVAL(zero, OR2(INV(comb.v_a1s_nzero), INV(comb.v_a2s_nzero)));
                SETVAL(inv, XOR2(BIT(i_a1, 63), BIT(i_a2, 63)));
                SETVAL(a1, comb.vb_a1s);
                SETVAL(a2, comb.vb_a2s);
            ENDIF();
        ELSE();
            SETVAL(a1, i_a1);
            SETVAL(a2, i_a2);
        ENDIF();
        SETVAL(rv32, i_rv32);
        SETVAL(unsign, i_unsigned);
        SETVAL(high, i_high);

TEXT();
        TEXT("Just for run-rime control (not for VHDL)");
        SETVAL(a1_dbg, i_a1);
        SETVAL(a2_dbg, i_a2);
        //SETVAL(reference_mul, CALL("compute_reference", i_unsigned,
        //                                                i_rv32,
        //                                                i_a1,
        //                                                i_a2));
    ENDIF();

TEXT();
    IF (NZ(BIT(ena, 0)));
        
        i = &FOR("i", CONST("0"), CONST("32"), "++");
            SETVAL(comb.wb_mux_lvl0, BITS(a2, INC(MUL2(CONST("2"), *i)), MUL2(CONST("2"), *i)));
            IF (EQ(comb.wb_mux_lvl0, CONST("0", 2)));
                SETARRITEM(comb.wb_lvl0, *i, comb.wb_lvl0, ALLZEROS());
            ELSIF (EQ(comb.wb_mux_lvl0, CONST("1", 2)));
                SETARRITEM(comb.wb_lvl0, *i, comb.wb_lvl0, CC2(CONST("0", 2), TO_BIG(66, a1)));
            ELSIF (EQ(comb.wb_mux_lvl0, CONST("2", 2)));
                SETARRITEM(comb.wb_lvl0, *i, comb.wb_lvl0, CC2(TO_BIG(66, a1), CONST("0", 1)));
            ELSE();
                SETARRITEM(comb.wb_lvl0, *i, comb.wb_lvl0, 
                            ADDx(2, &CC2(CONST("0", 2), TO_BIG(66, a1)),
                                    &CC2(TO_BIG(66, a1), CONST("0", 1))));
            ENDIF();
        ENDFOR();

        i = &FOR ("i", CONST("0"),CONST("16"), "++");
            SETARRITEM(lvl1, *i, lvl1, ADDx(2, &CC2(TO_BIG(69, ARRITEM(comb.wb_lvl0, INC(MUL2(CONST("2"), *i)), comb.wb_lvl0)), CONST("0", 2)),
                                               &TO_BIG(69, ARRITEM(comb.wb_lvl0, MUL2(CONST("2"), *i), comb.wb_lvl0))));
        ENDFOR();
    ENDIF();

TEXT();
    IF (NZ(BIT(ena, 1)));
        i = & FOR ("i", CONST("0"), CONST("8"), "++");
            SETARRITEM(comb.wb_lvl2, *i, comb.wb_lvl2,
                        ADDx(2, &CC2(TO_BIG(74, ARRITEM(lvl1, INC(MUL2(CONST("2"), *i)), lvl1)), CONST("0", 4)),
                                &TO_BIG(74, ARRITEM(lvl1, MUL2(CONST("2"), *i), lvl1))));
        ENDFOR();

        i = & FOR ("i", CONST("0"), CONST("4"), "++");
            SETARRITEM(lvl3, *i, lvl3,
                        ADDx(2, &CC2(TO_BIG(83, ARRITEM(comb.wb_lvl2, INC(MUL2(CONST("2"), *i)), comb.wb_lvl2)), CONST("0", 8)),
                                &TO_BIG(83, ARRITEM(comb.wb_lvl2, MUL2(CONST("2"), *i), comb.wb_lvl2))));
        ENDFOR();
    ENDIF();

TEXT();
    IF (NZ(BIT(ena, 2)));
        SETZERO(busy);
        i = &FOR ("i", CONST("0"), CONST("2"), "++");
            SETARRITEM(comb.wb_lvl4, *i, comb.wb_lvl4,
                        ADDx(2, &CC2(TO_BIG(100, ARRITEM(lvl3, INC(MUL2(CONST("2"), *i)), lvl3)), CONST("0", 16)),
                               &TO_BIG(100, ARRITEM(lvl3, MUL2(CONST("2"), *i), lvl3))));
        ENDFOR();

        SETVAL(comb.wb_lvl5, ADDx(2, &CC2(TO_BIG(128, ARRITEM(comb.wb_lvl4, CONST("1"), comb.wb_lvl4)), CONST("0", 32)),
                                     &TO_BIG(128, ARRITEM(comb.wb_lvl4, CONST("0"), comb.wb_lvl4))));
        IF (NZ(rv32));
            SETBITS(comb.wb_res32, 31, 0, BITS(comb.wb_lvl5 ,31, 0));
            IF (OR2(NZ(unsign), EZ(BIT(comb.wb_lvl5, 31))));
                SETBITS(comb.wb_res32, 127, 32, ALLZEROS());
            ELSE();
                SETBITS(comb.wb_res32, 127, 32, ALLONES());
            ENDIF();
            SETVAL(result, comb.wb_res32);
        ELSIF (NZ(high));
            IF (NZ(zero));
                SETZERO(result);
            ELSIF (NZ(inv));
                SETVAL(result, INV_L(comb.wb_lvl5));
            ELSE();
                SETVAL(result, comb.wb_lvl5);
            ENDIF();
        ELSE();
            SETVAL(result, comb.wb_lvl5);
        ENDIF();
    ENDIF();

TEXT();
    SETVAL(comb.wb_res, BITS(result, 63, 0));
    IF (NZ(high));
        SETVAL(comb.wb_res, BITS(result, 127, 64));
    ENDIF();


TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_res, comb.wb_res);
    SETVAL(o_valid, BIT(ena, 3));
}

