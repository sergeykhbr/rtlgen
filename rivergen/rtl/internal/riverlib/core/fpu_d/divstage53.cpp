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

#include "divstage53.h"

divstage53::divstage53(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "divstage53", name, comment),
    i_mux_ena(this, "i_mux_ena", "1", "find first non-zero bit"),
    i_muxind(this, "i_muxind", "56", "bits indexes 8x7 bits bus"),
    i_divident(this, "i_divident", "61", "integer value"),
    i_divisor(this, "i_divisor", "53", "integer value"),
    o_dif(this, "o_dif", "53", "residual value"),
    o_bits(this, "o_bits", "8", "resulting bits"),
    o_muxind(this, "o_muxind", "7", "first found non-zero bits"),
    o_muxind_rdy(this, "o_muxind_rdy", "1", "seeking was successfull"),
    // registers
    wb_thresh(this, "wb_thresh", "62", "16", NO_COMMENT),
    wb_dif(this, "wb_dif", "61", "4", NO_COMMENT),
    // process
    comb(this)
{
    Operation::start(&comb);
    proc_comb();
}

void divstage53::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;

    SETVAL(comb.wb_divx2, CC2(i_divisor, CONST("0", 1)));
    SETVAL(comb.wb_divx3, ADD2(comb.wb_divx2, i_divisor));

TEXT();
    TEXT("stage 1 of 4");
    SETARRITEM(wb_thresh, 15, SUB2(CC2(CONST("0", 1), i_divident),
                                   CC3(CONST("0", 1), TO_U64(comb.wb_divx3), CONST("0", 6))));
    SETARRITEM(wb_thresh, 14, SUB2(CC2(CONST("0", 1), i_divident),
                                   CC3(CONST("0", 1), TO_U64(comb.wb_divx2), CONST("0", 6))));
    SETARRITEM(wb_thresh, 13, SUB2(CC2(CONST("0", 1), i_divident),
                                   CC3(CONST("0", 3), TO_U64(i_divisor), CONST("0", 6))));
    SETARRITEM(wb_thresh, 12, CC2(CONST("0", 1), i_divident));

TEXT();
    IF (EZ(BIT(ARRITEM(wb_thresh, 15), 61)));
        SETBITS(comb.wb_bits, 7, 6, CONST("3", 2));
        SETARRITEM(wb_dif, 0, BITS(ARRITEM(wb_thresh, 15), 60, 0));
    ELSIF (EZ(BIT(ARRITEM(wb_thresh, 14), 61)));
        SETBITS(comb.wb_bits, 7, 6, CONST("2", 2));
        SETARRITEM(wb_dif, 0, BITS(ARRITEM(wb_thresh, 14), 60, 0));
    ELSIF (EZ(BIT(ARRITEM(wb_thresh, 13), 61)));
        SETBITS(comb.wb_bits, 7, 6, CONST("1", 2));
        SETARRITEM(wb_dif, 0, BITS(ARRITEM(wb_thresh, 13), 60, 0));
    ELSE();
        SETBITS(comb.wb_bits, 7, 6, CONST("0", 2));
        SETARRITEM(wb_dif, 0, BITS(ARRITEM(wb_thresh, 12), 60, 0));
    ENDIF();

TEXT();
    // wb_thresh 62 bits
    // wb_dif 61 bits
    // wb_divx3, wb_divx2 55 bits
    TEXT("stage 2 of 4");
    SETARRITEM(wb_thresh, 11, SUB2(CC2(CONST("0", 1), ARRITEM(wb_dif, 0)),
                                   CC3(CONST("0", 3), TO_U64(comb.wb_divx3), CONST("0", 4))));
    SETARRITEM(wb_thresh, 10, SUB2(CC2(CONST("0", 1), ARRITEM(wb_dif, 0)),
                                   CC3(CONST("0", 3), TO_U64(comb.wb_divx2), CONST("0", 4))));
    SETARRITEM(wb_thresh, 9, SUB2(CC2(CONST("0", 1), ARRITEM(wb_dif, 0)),
                                  CC3(CONST("0", 5), TO_U64(i_divisor), CONST("0", 4))));
    SETARRITEM(wb_thresh, 8, CC2(CONST("0", 1), ARRITEM(wb_dif, 0)));

TEXT();
    IF (EZ(BIT(ARRITEM(wb_thresh, 11), 61)));
        SETBITS(comb.wb_bits, 5, 4, CONST("3", 2));
        SETARRITEM(wb_dif, 1, BITS(ARRITEM(wb_thresh, 11), 60, 0));
    ELSIF (EZ(BIT(ARRITEM(wb_thresh, 10), 61)));
        SETBITS(comb.wb_bits, 5, 4, CONST("2", 2));
        SETARRITEM(wb_dif, 1, BITS(ARRITEM(wb_thresh, 10), 60, 0));
    ELSIF (EZ(BIT(ARRITEM(wb_thresh, 9), 61)));
        SETBITS(comb.wb_bits, 5, 4, CONST("1", 2));
        SETARRITEM(wb_dif, 1, BITS(ARRITEM(wb_thresh, 9), 60, 0));
    ELSE();
        SETBITS(comb.wb_bits, 5, 4, CONST("0", 2));
        SETARRITEM(wb_dif, 1, BITS(ARRITEM(wb_thresh, 8), 60, 0));
    ENDIF();


TEXT();
    TEXT("stage 3 of 4");
    SETARRITEM(wb_thresh, 7, SUB2(CC2(CONST("0", 1), ARRITEM(wb_dif, 1)),
                                  CC3(CONST("0", 5), TO_U64(comb.wb_divx3), CONST("0", 2))));
    SETARRITEM(wb_thresh, 6, SUB2(CC2(CONST("0", 1), ARRITEM(wb_dif, 1)),
                                  CC3(CONST("0", 5), TO_U64(comb.wb_divx2), CONST("0", 2))));
    SETARRITEM(wb_thresh, 5, SUB2(CC2(CONST("0", 1), ARRITEM(wb_dif, 1)),
                                  CC3(CONST("0", 7), TO_U64(i_divisor), CONST("0", 2))));
    SETARRITEM(wb_thresh, 4, CC2(CONST("0", 1), ARRITEM(wb_dif, 1)));

    IF (EZ(BIT(ARRITEM(wb_thresh, 7), 61)));
        SETBITS(comb.wb_bits, 3, 2, CONST("3", 2));
        SETARRITEM(wb_dif, 2, BITS(ARRITEM(wb_thresh, 7), 60, 0));
    ELSIF (EZ(BIT(ARRITEM(wb_thresh, 6), 61)));
        SETBITS(comb.wb_bits, 3, 2, CONST("2", 2));
        SETARRITEM(wb_dif, 2, BITS(ARRITEM(wb_thresh, 6), 60, 0));
    ELSIF (EZ(BIT(ARRITEM(wb_thresh, 5), 61)));
        SETBITS(comb.wb_bits, 3, 2, CONST("1", 2));
        SETARRITEM(wb_dif, 2, BITS(ARRITEM(wb_thresh, 5), 60, 0));
    ELSE();
        SETBITS(comb.wb_bits, 3, 2, CONST("0", 2));
        SETARRITEM(wb_dif, 2, BITS(ARRITEM(wb_thresh, 4), 60, 0));
    ENDIF();

TEXT();
    TEXT("stage 4 of 4");
    SETARRITEM(wb_thresh, 3, SUB2(CC2(CONST("0", 1), ARRITEM(wb_dif, 2)),
                                  CC2(CONST("0", 7), TO_U64(comb.wb_divx3))));
    SETARRITEM(wb_thresh, 2, SUB2(CC2(CONST("0", 1), ARRITEM(wb_dif, 2)),
                                  CC2(CONST("0", 7), TO_U64(comb.wb_divx2))));
    SETARRITEM(wb_thresh, 1, SUB2(CC2(CONST("0", 1), ARRITEM(wb_dif, 2)),
                                  CC2(CONST("0", 9), TO_U64(i_divisor))));
    SETARRITEM(wb_thresh, 0, CC2(CONST("0", 1), ARRITEM(wb_dif, 2)));

    IF (EZ(BIT(ARRITEM(wb_thresh, 3), 61)));
        SETBITS(comb.wb_bits, 1, 0, CONST("3", 2));
        SETARRITEM(wb_dif, 3, BITS(ARRITEM(wb_thresh, 3), 60, 0));
    ELSIF (EZ(BIT(ARRITEM(wb_thresh, 2), 61)));
        SETBITS(comb.wb_bits, 1, 0, CONST("2", 2));
        SETARRITEM(wb_dif, 3, BITS(ARRITEM(wb_thresh, 2), 60, 0));
    ELSIF (EZ(BIT(ARRITEM(wb_thresh, 1), 61)));
        SETBITS(comb.wb_bits, 1, 0, CONST("1", 2));
        SETARRITEM(wb_dif, 3, BITS(ARRITEM(wb_thresh, 1), 60, 0));
    ELSE();
        SETBITS(comb.wb_bits, 1, 0, CONST("0", 2));
        SETARRITEM(wb_dif, 3, BITS(ARRITEM(wb_thresh, 0), 60, 0));
    ENDIF();

TEXT();
    TEXT("Number multiplexor");
    SETZERO(comb.wb_muxind);
    IF (NZ(i_mux_ena));
        IF (EZ(BIT(ARRITEM(wb_thresh, 15), 61)));
            SETVAL(comb.wb_muxind, BITS(i_muxind, 55, 49));
        ELSIF (EZ(BIT(ARRITEM(wb_thresh, 14), 61)));
            SETVAL(comb.wb_muxind, BITS(i_muxind, 55, 49));
        ELSIF (EZ(BIT(ARRITEM(wb_thresh, 13), 61)));
            SETVAL(comb.wb_muxind, BITS(i_muxind, 48, 42));
        ELSIF (EZ(BIT(ARRITEM(wb_thresh, 11), 61)));
            SETVAL(comb.wb_muxind, BITS(i_muxind, 41, 35));
        ELSIF (EZ(BIT(ARRITEM(wb_thresh, 10), 61)));
            SETVAL(comb.wb_muxind, BITS(i_muxind, 41, 35));
        ELSIF (EZ(BIT(ARRITEM(wb_thresh, 9), 61)));
            SETVAL(comb.wb_muxind, BITS(i_muxind, 34, 28));
        ELSIF (EZ(BIT(ARRITEM(wb_thresh, 7), 61)));
            SETVAL(comb.wb_muxind, BITS(i_muxind, 27, 21));
        ELSIF (EZ(BIT(ARRITEM(wb_thresh, 6), 61)));
            SETVAL(comb.wb_muxind, BITS(i_muxind, 27, 21));
        ELSIF (EZ(BIT(ARRITEM(wb_thresh, 5), 61)));
            SETVAL(comb.wb_muxind, BITS(i_muxind, 20, 14));
        ELSIF (EZ(BIT(ARRITEM(wb_thresh, 3), 61)));
            SETVAL(comb.wb_muxind, BITS(i_muxind, 13, 7));
        ELSIF (EZ(BIT(ARRITEM(wb_thresh, 2), 61)));
            SETVAL(comb.wb_muxind, BITS(i_muxind, 13, 7));
        ELSIF (EZ(BIT(ARRITEM(wb_thresh, 1), 61)));
            SETVAL(comb.wb_muxind, BITS(i_muxind, 6, 0));
        ELSE();
            SETVAL(comb.wb_muxind, BITS(i_muxind, 6, 0));
        ENDIF();
    ENDIF();

TEXT();
    SETZERO(comb.w_muxind_rdy);
    IF (AND2(NZ(i_mux_ena), NZ(comb.wb_bits)));
        SETONE(comb.w_muxind_rdy);
    ENDIF();

TEXT();
    SETVAL(o_bits, comb.wb_bits);
    SETVAL(o_dif, BITS(ARRITEM(wb_dif, 3), 52, 0));
    SETVAL(o_muxind, comb.wb_muxind);
    SETVAL(o_muxind_rdy, comb.w_muxind_rdy);
}

