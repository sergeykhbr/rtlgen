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

#include "mul_b8.h"

mul_b8::mul_b8(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "mul_b8", name, comment),
    // IO
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_a(this, "i_a", "8", "multiplier A"),
    i_b(this, "i_b", "8", "multiplier B"),
    i_signed(this, "i_signed", "1", "0=unsigned; 1=signed operation"),
    o_res(this, "o_res", "16", "result"),
    // params
    // signals
    // registers
    ua(this, "ua", "8", "'0", NO_COMMENT),
    ub(this, "ub", "8", "'0", NO_COMMENT),
    inv(this, "inv", "4", RSTVAL_ZERO, NO_COMMENT),
    lvl1_0(this, "lvl1_0", "11", RSTVAL_ZERO, NO_COMMENT),
    lvl1_1(this, "lvl1_1", "11", RSTVAL_ZERO, NO_COMMENT),
    lvl1_2(this, "lvl1_2", "11", RSTVAL_ZERO, NO_COMMENT),
    lvl1_3(this, "lvl1_3", "11", RSTVAL_ZERO, NO_COMMENT),
    lvl2_0(this, "lvl2_0", "14", RSTVAL_ZERO, NO_COMMENT),
    lvl2_1(this, "lvl2_1", "14", RSTVAL_ZERO, NO_COMMENT),
    lvl3(this, "lvl3", "16", "'0", NO_COMMENT),
    res(this, "res", "16", "'0", NO_COMMENT),
    //
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void mul_b8::proc_comb() {
    IF (AND2(NZ(i_signed), NZ(BIT(i_a, 7))));
        SETVAL(ua, INC(INV_L(i_a)));
    ELSE();
        SETVAL(ua, i_a);
    ENDIF();
    IF (AND2(NZ(i_signed), NZ(BIT(i_b, 7))));
        SETVAL(ub, INC(INV_L(i_b)));
    ELSE();
        SETVAL(ub, i_b);
    ENDIF();
    SETVAL(inv, CC2(BITS(inv, 2, 0), AND2_L(i_signed, XOR2(BIT(i_a, 7), BIT(i_b, 7)))));

    TEXT();
    TEXT("Level 1, bits[1:0]");
    IF (NZ(BIT(ub, 0)));
        SETVAL(comb.vb_lvl1_00, CC3(CONST("0", 2), ua, CONST("0", 1)));
    ENDIF();
    IF (NZ(BIT(ub, 1)));
        SETVAL(comb.vb_lvl1_01, CC3(CONST("0", 1), ua, CONST("0", 2)));
    ENDIF();
    SETVAL(lvl1_0, ADD2(comb.vb_lvl1_00, comb.vb_lvl1_01));

    TEXT();
    TEXT("Level 1, bits[3:2]");
    IF (NZ(BIT(ub, 2)));
        SETVAL(comb.vb_lvl1_10, CC3(CONST("0", 2), ua, CONST("0", 1)));
    ENDIF();
    IF (NZ(BIT(ub, 3)));
        SETVAL(comb.vb_lvl1_11, CC3(CONST("0", 1), ua, CONST("0", 2)));
    ENDIF();
    SETVAL(lvl1_1, ADD2(comb.vb_lvl1_10, comb.vb_lvl1_11));

    TEXT();
    TEXT("Level 1, bits[5:4]");
    IF (NZ(BIT(ub, 4)));
        SETVAL(comb.vb_lvl1_20, CC3(CONST("0", 2), ua, CONST("0", 1)));
    ENDIF();
    IF (NZ(BIT(ub, 5)));
        SETVAL(comb.vb_lvl1_21, CC3(CONST("0", 1), ua, CONST("0", 2)));
    ENDIF();
    SETVAL(lvl1_2, ADD2(comb.vb_lvl1_20, comb.vb_lvl1_21));

    TEXT();
    TEXT("Level 1, bits[7:6]");
    IF (NZ(BIT(ub, 6)));
        SETVAL(comb.vb_lvl1_30, CC3(CONST("0", 2), ua, CONST("0", 1)));
    ENDIF();
    IF (NZ(BIT(ub, 7)));
        SETVAL(comb.vb_lvl1_31, CC3(CONST("0", 1), ua, CONST("0", 2)));
    ENDIF();
    SETVAL(lvl1_3, ADD2(comb.vb_lvl1_30, comb.vb_lvl1_31));


    TEXT();
    TEXT();
    TEXT("Level 2, bits[3:0]");
    SETVAL(lvl2_0, ADD2(CC2(CONST("0", 3), lvl1_0), CC3(CONST("0", 1), lvl1_1, CONST("0", 2))));
    TEXT("Level 2, bits[7:4]");
    SETVAL(lvl2_1, ADD2(CC2(CONST("0", 3), lvl1_2), CC3(CONST("0", 1), lvl1_3, CONST("0", 2))));


    TEXT();
    TEXT();
    TEXT("Level 3, result");
    SETVAL(lvl3, ADD2(CC2(CONST("0", 2), lvl2_0), CC2(BITS(lvl2_1, 11, 0), CONST("0", 4))));
    IF (NZ(BIT(inv, 3)));
        SETVAL(res, INC(INV_L(lvl3)));
    ELSE();
        SETVAL(res, lvl3);
    ENDIF();

    TEXT();
    SYNC_RESET();

    TEXT();
    SETVAL(o_res, res);
}

