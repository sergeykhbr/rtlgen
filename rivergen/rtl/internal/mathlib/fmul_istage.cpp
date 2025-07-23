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

#include "fmul_istage.h"

fmul_istage::fmul_istage(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "fmul_istage", name, comment),
    idx(this, "idx", "0", "stage index"),
    ibits(this, "ibits", "22", "Input signal bitwise"),
    mbits(this, "mbits", "4", "Operator m bitwise: possible values 4 (and potentially 3)"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_a(this, "i_a", "ibits", "integer value"),
    i_m(this, "i_m", "mbits", "4-bits value to multiply 0..15 using adders"),
    i_carry(this, "i_carry", "ibits", "Carry value: A * M + Carry"),
    i_zres(this, "i_zres", "ibits", "value to delay. Only (4*idx) of ibits will be delayed"),
    o_result(this, "o_result", "ibits", "resulting bits concatated with z-value"),
    o_carry(this, "o_carry", "ibits", "resulting carry bits"),
    // signals:
    // registers
    zres(this, "zres", "ibits", "'0", "cannot create array of template with different width bits"),
    res(this, "res", "ADD(ibits,4)", "'0", NO_COMMENT),
    // process
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void fmul_istage::proc_comb() {
    SETARRITEM(comb.vb_mux, 0, ALLZEROS());
    SETARRITEM(comb.vb_mux, 1, i_a);                            // 1*a
    SETARRITEM(comb.vb_mux, 2, CC2(i_a, CONST("0", 1)));        // 2*a
    SETARRITEM(comb.vb_mux, 3, ADD2(ARRITEM(comb.vb_mux, 2), ARRITEM(comb.vb_mux, 1)));          // 2*a + 1*a
    SETARRITEM(comb.vb_mux, 4, CC2(i_a, CONST("0", 2)));    // 4*a
    SETARRITEM(comb.vb_mux, 5, ADD2(ARRITEM(comb.vb_mux, 4), ARRITEM(comb.vb_mux, 1)));          // 4*a + 1*a
    SETARRITEM(comb.vb_mux, 6, ADD2(ARRITEM(comb.vb_mux, 4), ARRITEM(comb.vb_mux, 2)));          // 4*a + 2*a
    SETARRITEM(comb.vb_mux, 8, CC2(i_a, CONST("0", 3)));    // 8*a
    SETARRITEM(comb.vb_mux, 7, SUB2(ARRITEM(comb.vb_mux, 8), ARRITEM(comb.vb_mux, 1)));          // 8*a - 1*a
    SETARRITEM(comb.vb_mux, 9, ADD2(ARRITEM(comb.vb_mux, 8), ARRITEM(comb.vb_mux, 1)));          // 8*a + 1*a
    SETARRITEM(comb.vb_mux, 10, ADD2(ARRITEM(comb.vb_mux, 8), ARRITEM(comb.vb_mux, 2)));         // 8*a + 2*a
    SETARRITEM(comb.vb_mux, 11, ADD2(ARRITEM(comb.vb_mux, 10), ARRITEM(comb.vb_mux, 1)));        // (8*a + 2*a) + 1*a
    SETARRITEM(comb.vb_mux, 12, ADD2(ARRITEM(comb.vb_mux, 8), ARRITEM(comb.vb_mux, 4)));         // 8*a + 4*a
    SETARRITEM(comb.vb_mux, 16, CC2(i_a, CONST("0", 4))); // unused
    SETARRITEM(comb.vb_mux, 13, SUB2(ARRITEM(comb.vb_mux, 16), ARRITEM(comb.vb_mux, 3)));        // 16*a - (2*a + 1*a)
    SETARRITEM(comb.vb_mux, 14, SUB2(ARRITEM(comb.vb_mux, 16), ARRITEM(comb.vb_mux, 2)));        // 16*a - 2*a
    SETARRITEM(comb.vb_mux, 15, SUB2(ARRITEM(comb.vb_mux, 16), ARRITEM(comb.vb_mux, 1)));        // 16*a - 1*a

    TEXT();
    SWITCH (i_m);
    CASE(CONST("1", 4));
        SETVAL(comb.vb_res, ADD2(ARRITEM(comb.vb_mux, 1), CC2(CONST("0", 4), i_carry)));
        ENDCASE();
    CASE(CONST("2", 4));
        SETVAL(comb.vb_res, ADD2(ARRITEM(comb.vb_mux, 2), CC2(CONST("0", 4), i_carry)));
        ENDCASE();
    CASE(CONST("3", 4));
        SETVAL(comb.vb_res, ADD2(ARRITEM(comb.vb_mux, 3), CC2(CONST("0", 4), i_carry)));
        ENDCASE();
    CASE(CONST("4", 4));
        SETVAL(comb.vb_res, ADD2(ARRITEM(comb.vb_mux, 4), CC2(CONST("0", 4), i_carry)));
        ENDCASE();
    CASE(CONST("5", 4));
        SETVAL(comb.vb_res, ADD2(ARRITEM(comb.vb_mux, 5), CC2(CONST("0", 4), i_carry)));
        ENDCASE();
    CASE(CONST("6", 4));
        SETVAL(comb.vb_res, ADD2(ARRITEM(comb.vb_mux, 6), CC2(CONST("0", 4), i_carry)));
        ENDCASE();
    CASE(CONST("7", 4));
        SETVAL(comb.vb_res, ADD2(ARRITEM(comb.vb_mux, 7), CC2(CONST("0", 4), i_carry)));
        ENDCASE();
    CASE(CONST("8", 4));
        SETVAL(comb.vb_res, ADD2(ARRITEM(comb.vb_mux, 8), CC2(CONST("0", 4), i_carry)));
        ENDCASE();
    CASE(CONST("9", 4));
        SETVAL(comb.vb_res, ADD2(ARRITEM(comb.vb_mux, 9), CC2(CONST("0", 4), i_carry)));
        ENDCASE();
    CASE(CONST("10", 4));
        SETVAL(comb.vb_res, ADD2(ARRITEM(comb.vb_mux, 10), CC2(CONST("0", 4), i_carry)));
        ENDCASE();
    CASE(CONST("11", 4));
        SETVAL(comb.vb_res, ADD2(ARRITEM(comb.vb_mux, 11), CC2(CONST("0", 4), i_carry)));
        ENDCASE();
    CASE(CONST("12", 4));
        SETVAL(comb.vb_res, ADD2(ARRITEM(comb.vb_mux, 12), CC2(CONST("0", 4), i_carry)));
        ENDCASE();
    CASE(CONST("13", 4));
        SETVAL(comb.vb_res, ADD2(ARRITEM(comb.vb_mux, 13), CC2(CONST("0", 4), i_carry)));
        ENDCASE();
    CASE(CONST("14", 4));
        SETVAL(comb.vb_res, ADD2(ARRITEM(comb.vb_mux, 14), CC2(CONST("0", 4), i_carry)));
    ENDCASE();
    CASE(CONST("15", 4));
        SETVAL(comb.vb_res, ADD2(ARRITEM(comb.vb_mux, 15), CC2(CONST("0", 4), i_carry)));
    ENDCASE();
    CASEDEF();
        SETVAL(comb.vb_res, CC2(CONST("0", 4), i_carry));
    ENDCASE();
    ENDSWITCH();

    TEXT();
    SETVAL(res, comb.vb_res);

    TEXT();
    IFGEN (NZ(idx), new StringConst("n0"));
        SETBITS(comb.vb_zres, DEC(MUL2(idx,mbits)), CONST("0"), BITS(i_zres, DEC(MUL2(idx,mbits)), CONST("0")));
        SETBITS(comb.vb_res_idx, DEC(MUL2(idx,mbits)), CONST("0"), zres);
    ENDIFGEN(new StringConst("n0"));
    SETVAL(zres, comb.vb_zres);
    SETBITSW(comb.vb_res_idx, MUL2(idx,mbits), CONST("4"), BITS(res, 3, 0));

    TEXT();
    SYNC_RESET();

    TEXT();
    SETVAL(o_result, comb.vb_res_idx);
    SETVAL(o_carry, BITS(res, ADD2(ibits, CONST("3")), CONST("4")));
}

