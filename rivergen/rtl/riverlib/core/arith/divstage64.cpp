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

#include "divstage64.h"

divstage64::divstage64(GenObject *parent, const char *name) :
    ModuleObject(parent, "divstage64", name),
    i_divident(this, "i_divident", "64", "integer value"),
    i_divisor(this, "i_divisor", "124", "integer value"),
    o_resid(this, "o_resid", "64", "residual"),
    o_bits(this, "o_bits", "4", "resulting bits"),
    // signals
    // process
    comb(this)
{
    Operation::start(&comb);
    proc_comb();
}

void divstage64::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;

    SETVAL(comb.wb_divident, BIG_TO_U64(i_divident));
    SETVAL(comb.wb_divisor, i_divisor);

TEXT();
    SETBITS(comb.wb_divx1, 63, 0, BITS(comb.wb_divisor, 63, 0));
    IF (NZ(BITS(comb.wb_divisor, 123, 64)));
        SETBITONE(comb.wb_divx1, 64);
    ELSE();
        SETBITZERO(comb.wb_divx1, 64);
    ENDIF();

TEXT();
    SETBITZERO(comb.wb_divx2, 0);
    SETBITS(comb.wb_divx2, 63, 1, BITS(comb.wb_divisor, 62, 0));
    IF (NZ(BITS(comb.wb_divisor, 123, 63)));
        SETBITONE(comb.wb_divx2, 64);
    ELSE();
        SETBITZERO(comb.wb_divx2, 64);
    ENDIF();

TEXT();
    SETBITS(comb.wb_divx3, 64, 0, ADD2(CC2(CONST("0", 1), BITS(comb.wb_divx2, 63, 0)),
                                  CC2(CONST("0", 1), BITS(comb.wb_divx1, 63, 0))));
    SETBIT(comb.wb_divx3, 64, OR2(BIT(comb.wb_divx3, 64), BIT(comb.wb_divx2, 64)));

TEXT();
    SETBITS(comb.wb_divx4, 1, 0, CONST("0", 2));
    SETBITS(comb.wb_divx4, 63, 2, BITS(comb.wb_divisor, 61, 0));
    IF (NZ(BITS(comb.wb_divisor, 123, 62)));
        SETBITONE(comb.wb_divx4, 64);
    ELSE();
        SETBITZERO(comb.wb_divx4, 64);
    ENDIF();

TEXT();
    SETBITS(comb.wb_divx5, 64, 0, ADD2(CC2(CONST("0", 1), BITS(comb.wb_divx4, 63, 0)),
                                  CC2(CONST("0", 1), BITS(comb.wb_divx1, 63, 0))));
    SETBIT(comb.wb_divx5, 64, OR2(BIT(comb.wb_divx5, 64), BIT(comb.wb_divx4, 64)));

TEXT();
    SETBITZERO(comb.wb_divx6, 0);
    SETBITS(comb.wb_divx6, 63, 1, BITS(comb.wb_divx3, 62, 0));
    SETBIT(comb.wb_divx6, 64, OR2(BIT(comb.wb_divx3, 64), BIT(comb.wb_divx3, 63)));

TEXT();
    SETBITS(comb.wb_divx8, 2, 0, CONST("0", 3));
    SETBITS(comb.wb_divx8, 63, 3, BITS(comb.wb_divisor, 60, 0));
    IF (NZ(BITS(comb.wb_divisor, 123, 61)));
        SETBITONE(comb.wb_divx8, 64);
    ELSE();
        SETBITZERO(comb.wb_divx8, 64);
    ENDIF();

TEXT();
    TEXT("7 = 8 - 1");
    SETBITS(comb.wb_divx7, 64, 0, SUB2(BITS(comb.wb_divx8, 64, 0), CC2(CONST("0", 1), BITS(comb.wb_divx1, 63, 0))));
    IF (OR2(NZ(BIT(comb.wb_divx7, 64)), NZ(BITS(comb.wb_divisor, 123, 62))));
        SETBITONE(comb.wb_divx7, 64);
    ELSE();
        SETBITZERO(comb.wb_divx7, 64);
    ENDIF();

TEXT();
    TEXT("9 = 8 + 1");
    SETBITS(comb.wb_divx9, 64, 0, ADD2(CC2(CONST("0", 1), BITS(comb.wb_divx8, 63, 0)),
                                  CC2(CONST("0", 1), BITS(comb.wb_divx1, 63, 0))));
    IF (OR2(NZ(BIT(comb.wb_divx9, 64)), NZ(BITS(comb.wb_divisor, 123, 61))));
        SETBITONE(comb.wb_divx9, 64);
    ELSE();
        SETBITZERO(comb.wb_divx9, 64);
    ENDIF();

TEXT();
    TEXT("10 = 8 + 2");
    SETBITS(comb.wb_divx10, 64, 0, ADD2(CC2(CONST("0", 1), BITS(comb.wb_divx8, 63, 0)),
                                   CC2(CONST("0", 1), BITS(comb.wb_divx2, 63, 0))));
    IF (OR2(NZ(BIT(comb.wb_divx10, 64)), NZ(BITS(comb.wb_divisor, 123, 61))));
        SETBITONE(comb.wb_divx10, 64);
    ELSE();
        SETBITZERO(comb.wb_divx10, 64);
    ENDIF();

TEXT();
    TEXT("11 = 8 + 3");
    SETBITS(comb.wb_divx11, 64, 0, ADD2(CC2(CONST("0", 1), BITS(comb.wb_divx8, 63, 0)),
                                   CC2(CONST("0", 1), BITS(comb.wb_divx3, 63, 0))));
    IF (OR2(NZ(BIT(comb.wb_divx11, 64)), NZ(BITS(comb.wb_divisor, 123, 61))));
        SETBITONE(comb.wb_divx11, 64);
    ELSE();
        SETBITZERO(comb.wb_divx11, 64);
    ENDIF();

TEXT();
    TEXT("12 = 3 << 2");
    SETBITS(comb.wb_divx12, 1, 0, CONST("0", 2));
    SETBITS(comb.wb_divx12, 63, 2, BITS(comb.wb_divx3, 61, 0));
    SETBIT(comb.wb_divx12, 64, OR3(BIT(comb.wb_divx3, 64), BIT(comb.wb_divx3, 63), BIT(comb.wb_divx3, 62)));

TEXT();
    TEXT("16 = divisor << 4");
    SETBITS(comb.wb_divx16, 3, 0, CONST("0", 4));
    SETBITS(comb.wb_divx16, 63, 4, BITS(comb.wb_divisor, 59, 0));
    IF (NZ(BITS(comb.wb_divisor, 123, 60)));
        SETBITONE(comb.wb_divx16, 64);
    ELSE();
        SETBITZERO(comb.wb_divx16, 64);
    ENDIF();

TEXT();
    TEXT("13 = 16 - 3");
    SETBITS(comb.wb_divx13, 64, 0, SUB2(BITS(comb.wb_divx16, 64, 0), CC2(CONST("0", 1), BITS(comb.wb_divx3, 63, 0))));
    IF (OR2(NZ(BIT(comb.wb_divx13, 64)), NZ(BITS(comb.wb_divisor, 123, 61))));
        SETBITONE(comb.wb_divx13, 64);
    ELSE();
        SETBITZERO(comb.wb_divx13, 64);
    ENDIF();

TEXT();
    TEXT("14 = 7 << 1");
    SETBITZERO(comb.wb_divx14, 0);
    SETBITS(comb.wb_divx14, 63, 1, BITS(comb.wb_divx7, 62, 0));
    SETBIT(comb.wb_divx14, 64, OR2(BIT(comb.wb_divx7, 64), BIT(comb.wb_divx7, 63)));

TEXT();
    TEXT("15 = 16 - 1");
    SETBITS(comb.wb_divx15, 64, 0, SUB2(BITS(comb.wb_divx16, 64, 0), CC2(CONST("0", 1), BITS(comb.wb_divx1, 63, 0))));
    IF (OR2(NZ(BIT(comb.wb_divx15, 64)), NZ(BITS(comb.wb_divisor, 123, 61))));
        SETBITONE(comb.wb_divx15, 64);
    ELSE();
        SETBITZERO(comb.wb_divx15, 64);
    ENDIF();

TEXT();
    SETARRITEM(comb.wb_thresh, CONST("15"), comb.wb_thresh, SUB2(CC2(CONST("0", 1), comb.wb_divident), CC2(CONST("0", 1), comb.wb_divx15)));
    SETARRITEM(comb.wb_thresh, CONST("14"), comb.wb_thresh, SUB2(CC2(CONST("0", 1), comb.wb_divident), CC2(CONST("0", 1), comb.wb_divx14)));
    SETARRITEM(comb.wb_thresh, CONST("13"), comb.wb_thresh, SUB2(CC2(CONST("0", 1), comb.wb_divident), CC2(CONST("0", 1), comb.wb_divx13)));
    SETARRITEM(comb.wb_thresh, CONST("12"), comb.wb_thresh, SUB2(CC2(CONST("0", 1), comb.wb_divident), CC2(CONST("0", 1), comb.wb_divx12)));
    SETARRITEM(comb.wb_thresh, CONST("11"), comb.wb_thresh, SUB2(CC2(CONST("0", 1), comb.wb_divident), CC2(CONST("0", 1), comb.wb_divx11)));
    SETARRITEM(comb.wb_thresh, CONST("10"), comb.wb_thresh, SUB2(CC2(CONST("0", 1), comb.wb_divident), CC2(CONST("0", 1), comb.wb_divx10)));
    SETARRITEM(comb.wb_thresh, CONST("9"), comb.wb_thresh, SUB2(CC2(CONST("0", 1), comb.wb_divident), CC2(CONST("0", 1), comb.wb_divx9)));
    SETARRITEM(comb.wb_thresh, CONST("8"), comb.wb_thresh, SUB2(CC2(CONST("0", 1), comb.wb_divident), CC2(CONST("0", 1), comb.wb_divx8)));
    SETARRITEM(comb.wb_thresh, CONST("7"), comb.wb_thresh, SUB2(CC2(CONST("0", 1), comb.wb_divident), CC2(CONST("0", 1), comb.wb_divx7)));
    SETARRITEM(comb.wb_thresh, CONST("6"), comb.wb_thresh, SUB2(CC2(CONST("0", 1), comb.wb_divident), CC2(CONST("0", 1), comb.wb_divx6)));
    SETARRITEM(comb.wb_thresh, CONST("5"), comb.wb_thresh, SUB2(CC2(CONST("0", 1), comb.wb_divident), CC2(CONST("0", 1), comb.wb_divx5)));
    SETARRITEM(comb.wb_thresh, CONST("4"), comb.wb_thresh, SUB2(CC2(CONST("0", 1), comb.wb_divident), CC2(CONST("0", 1), comb.wb_divx4)));
    SETARRITEM(comb.wb_thresh, CONST("3"), comb.wb_thresh, SUB2(CC2(CONST("0", 1), comb.wb_divident), CC2(CONST("0", 1), comb.wb_divx3)));
    SETARRITEM(comb.wb_thresh, CONST("2"), comb.wb_thresh, SUB2(CC2(CONST("0", 1), comb.wb_divident), CC2(CONST("0", 1), comb.wb_divx2)));
    SETARRITEM(comb.wb_thresh, CONST("1"), comb.wb_thresh, SUB2(CC2(CONST("0", 1), comb.wb_divident), CC2(CONST("0", 1), comb.wb_divx1)));
    SETARRITEM(comb.wb_thresh, CONST("0"), comb.wb_thresh, CC2(CONST("0", 1), comb.wb_divident));

TEXT();
    IF (EZ(BIT(ARRITEM(comb.wb_thresh, CONST("15"), comb.wb_thresh), 65)));
        SETVAL(comb.wb_bits, CONST("15", 4));
        SETVAL(comb.wb_dif, BIG_TO_U64(BITS(ARRITEM(comb.wb_thresh, CONST("15"), comb.wb_thresh), 63, 0)));
    ELSIF (EZ(BIT(ARRITEM(comb.wb_thresh, CONST("14"), comb.wb_thresh), 65)));
        SETVAL(comb.wb_bits, CONST("14", 4));
        SETVAL(comb.wb_dif, BIG_TO_U64(BITS(ARRITEM(comb.wb_thresh, CONST("14"), comb.wb_thresh), 63, 0)));
    ELSIF (EZ(BIT(ARRITEM(comb.wb_thresh, CONST("13"), comb.wb_thresh), 65)));
        SETVAL(comb.wb_bits, CONST("13", 4));
        SETVAL(comb.wb_dif, BIG_TO_U64(BITS(ARRITEM(comb.wb_thresh, CONST("13"), comb.wb_thresh), 63, 0)));
    ELSIF (EZ(BIT(ARRITEM(comb.wb_thresh, CONST("12"), comb.wb_thresh), 65)));
        SETVAL(comb.wb_bits, CONST("12", 4));
        SETVAL(comb.wb_dif, BIG_TO_U64(BITS(ARRITEM(comb.wb_thresh, CONST("12"), comb.wb_thresh), 63, 0)));
    ELSIF (EZ(BIT(ARRITEM(comb.wb_thresh, CONST("11"), comb.wb_thresh), 65)));
        SETVAL(comb.wb_bits, CONST("11", 4));
        SETVAL(comb.wb_dif, BIG_TO_U64(BITS(ARRITEM(comb.wb_thresh, CONST("11"), comb.wb_thresh), 63, 0)));
    ELSIF (EZ(BIT(ARRITEM(comb.wb_thresh, CONST("10"), comb.wb_thresh), 65)));
        SETVAL(comb.wb_bits, CONST("10", 4));
        SETVAL(comb.wb_dif, BIG_TO_U64(BITS(ARRITEM(comb.wb_thresh, CONST("10"), comb.wb_thresh), 63, 0)));
    ELSIF (EZ(BIT(ARRITEM(comb.wb_thresh, CONST("9"), comb.wb_thresh), 65)));
        SETVAL(comb.wb_bits, CONST("9", 4));
        SETVAL(comb.wb_dif, BIG_TO_U64(BITS(ARRITEM(comb.wb_thresh, CONST("9"), comb.wb_thresh), 63, 0)));
    ELSIF (EZ(BIT(ARRITEM(comb.wb_thresh, CONST("8"), comb.wb_thresh), 65)));
        SETVAL(comb.wb_bits, CONST("8", 4));
        SETVAL(comb.wb_dif, BIG_TO_U64(BITS(ARRITEM(comb.wb_thresh, CONST("8"), comb.wb_thresh), 63, 0)));
    ELSIF (EZ(BIT(ARRITEM(comb.wb_thresh, CONST("7"), comb.wb_thresh), 65)));
        SETVAL(comb.wb_bits, CONST("7", 4));
        SETVAL(comb.wb_dif, BIG_TO_U64(BITS(ARRITEM(comb.wb_thresh, CONST("7"), comb.wb_thresh), 63, 0)));
    ELSIF (EZ(BIT(ARRITEM(comb.wb_thresh, CONST("6"), comb.wb_thresh), 65)));
        SETVAL(comb.wb_bits, CONST("6", 4));
        SETVAL(comb.wb_dif, BIG_TO_U64(BITS(ARRITEM(comb.wb_thresh, CONST("6"), comb.wb_thresh), 63, 0)));
    ELSIF (EZ(BIT(ARRITEM(comb.wb_thresh, CONST("5"), comb.wb_thresh), 65)));
        SETVAL(comb.wb_bits, CONST("5", 4));
        SETVAL(comb.wb_dif, BIG_TO_U64(BITS(ARRITEM(comb.wb_thresh, CONST("5"), comb.wb_thresh), 63, 0)));
    ELSIF (EZ(BIT(ARRITEM(comb.wb_thresh, CONST("4"), comb.wb_thresh), 65)));
        SETVAL(comb.wb_bits, CONST("4", 4));
        SETVAL(comb.wb_dif, BIG_TO_U64(BITS(ARRITEM(comb.wb_thresh, CONST("4"), comb.wb_thresh), 63, 0)));
    ELSIF (EZ(BIT(ARRITEM(comb.wb_thresh, CONST("3"), comb.wb_thresh), 65)));
        SETVAL(comb.wb_bits, CONST("3", 4));
        SETVAL(comb.wb_dif, BIG_TO_U64(BITS(ARRITEM(comb.wb_thresh, CONST("3"), comb.wb_thresh), 63, 0)));
    ELSIF (EZ(BIT(ARRITEM(comb.wb_thresh, CONST("2"), comb.wb_thresh), 65)));
        SETVAL(comb.wb_bits, CONST("2", 4));
        SETVAL(comb.wb_dif, BIG_TO_U64(BITS(ARRITEM(comb.wb_thresh, CONST("2"), comb.wb_thresh), 63, 0)));
    ELSIF (EZ(BIT(ARRITEM(comb.wb_thresh, CONST("1"), comb.wb_thresh), 65)));
        SETVAL(comb.wb_bits, CONST("1", 4));
        SETVAL(comb.wb_dif, BIG_TO_U64(BITS(ARRITEM(comb.wb_thresh, CONST("1"), comb.wb_thresh), 63, 0)));
    ELSE();
        SETVAL(comb.wb_bits, CONST("0", 4));
        SETVAL(comb.wb_dif, BIG_TO_U64(BITS(ARRITEM(comb.wb_thresh, CONST("0"), comb.wb_thresh), 63, 0)));
    ENDIF();

TEXT();
    SETVAL(o_bits, comb.wb_bits);
    SETVAL(o_resid, comb.wb_dif);
}

