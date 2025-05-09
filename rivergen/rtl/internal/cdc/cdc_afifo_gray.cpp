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

#include "cdc_afifo_gray.h"

cdc_afifo_gray::cdc_afifo_gray(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "cdc_afifo_gray", name, comment),
    abits(this, "abits", "3", NO_COMMENT),
    i_nrst(this, "i_nrst", "1", NO_COMMENT),
    i_clk(this, "i_clk", "1", NO_COMMENT),
    i_ena(this, "i_ena", "1", NO_COMMENT),
    i_q2_gray(this, "i_q2_gray", "ADD(abits,1)", NO_COMMENT),
    o_addr(this, "o_addr", "abits", NO_COMMENT),
    o_gray(this, "o_gray", "ADD(abits,1)", NO_COMMENT),
    o_empty(this, "o_empty", "1", NO_COMMENT),
    o_full(this, "o_full", "1", NO_COMMENT),
    // Signal
    wb_bin_next(this, "wb_bin_next", "ADD(abits,1)", RSTVAL_ZERO, NO_COMMENT),
    wb_gray_next(this, "wb_gray_next", "ADD(abits,1)", RSTVAL_ZERO, NO_COMMENT),
    bin(this, "bin", "ADD(abits,1)", RSTVAL_ZERO, NO_COMMENT),
    gray(this, "gray", "ADD(abits,1)", RSTVAL_ZERO, NO_COMMENT),
    empty(this, "empty", "1", RSTVAL_ZERO, NO_COMMENT),
    full(this, "full", "1", RSTVAL_ZERO, NO_COMMENT),
    // process
    comb(this, "proc_comb", 0, CLK_ALWAYS, 0, ACTIVE_NONE, NO_COMMENT),
    ff(this, "proc_ff", &i_clk, CLK_POSEDGE, &i_nrst, ACTIVE_LOW, NO_COMMENT)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();

    Operation::start(&ff);
    proc_ff();
}

void cdc_afifo_gray::proc_comb() {
    ASSIGN(wb_bin_next, ADD2(bin, CC2(CONST("'0"), i_ena)));
    ASSIGN(wb_gray_next, XOR2(RSH(wb_bin_next, CONST("1")), wb_bin_next));
    ASSIGN(o_addr, BITS(bin, DEC(abits), CONST("0")));
    ASSIGN(o_gray, gray);
    ASSIGN(o_empty, empty);
    ASSIGN(o_full, full);
}

void cdc_afifo_gray::proc_ff() {
    IF (EZ(i_nrst));
        SETVAL_NB(bin, CONST("'0"));
        SETVAL_NB(gray, CONST("'0"));
        SETVAL_NB(empty, CONST("1", "1"));
        SETVAL_NB(full, CONST("0", "1"));
    ELSE();
        SETVAL_NB(bin, wb_bin_next);
        SETVAL_NB(gray, wb_gray_next);
        SETVAL_NB(empty, EQ(wb_gray_next, i_q2_gray));
        TEXT("Optimized version of 3 conditions:");
        TEXT("    wb_gray_next[abits] != i_q2_ptr[abits]");
        TEXT("    wb_gray_next[abits-1] != i_q2_ptr[abits-1]");
        TEXT("    wb_gray_next[abits-2:0] == i_q2_ptr[abits-2:0]");
        SETVAL_NB(full, EQ(wb_gray_next, CC2(INV_L(BITS(i_q2_gray, abits, DEC(abits))),
                                             BITS(i_q2_gray, SUB2(abits, CONST("2")), CONST("0")))));
    ENDIF();
}

