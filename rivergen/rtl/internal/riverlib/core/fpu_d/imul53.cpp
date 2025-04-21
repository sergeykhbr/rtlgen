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

#include "imul53.h"

imul53::imul53(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "imul53", name, comment),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_ena(this, "i_ena", "1", "enable pulse (1 clock)"),
    i_a(this, "i_a", "53", "integer value"),
    i_b(this, "i_b", "53", "integer value"),
    o_result(this, "o_result", "106", "resulting bits"),
    o_shift(this, "o_shift", "7", "first non-zero bit index"),
    o_rdy(this, "o_rdy", "1", "delayed 'enable' signal"),
    o_overflow(this, "o_overflow", "1", "overflow flag"),
    // signals:
    wb_sumInv(this, "wb_sumInv", "105"),
    wb_lshift(this, "wb_lshift", "7"),
    // registers
    delay(this, "delay", "16", "'0", NO_COMMENT),
    shift(this, "shift", "7", "'0", NO_COMMENT),
    accum_ena(this, "accum_ena", "1"),
    b(this, "b", "56", "'0", NO_COMMENT),
    sum(this, "sum", "106", "'0", NO_COMMENT),
    overflow(this, "overflow", "1"),
    // process
    comb(this),
    // submodules
    enc0(this, "enc0")
{
    Operation::start(this);

    enc0.iwidth.setObjValue(&CONST("105"));
    enc0.shiftwidth.setObjValue(&CONST("7"));
    NEW(enc0, enc0.getName().c_str());
        CONNECT(enc0, 0, enc0.i_value, wb_sumInv);
        CONNECT(enc0, 0, enc0.o_shift, wb_lshift);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void imul53::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;
    GenObject *i;

TEXT();
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
    SETVAL(comb.v_ena, i_ena);
    SETVAL(delay, CC2(BITS(delay, 14, 0), comb.v_ena));

TEXT();
    IF (NZ(i_ena));
        SETVAL(b, CC2(CONST("0", 3), i_b));
        SETZERO(overflow);
        SETONE(accum_ena);
        SETZERO(sum);
        SETZERO(shift);
    ELSIF (NZ(BIT(delay, 13)));
        SETZERO(accum_ena);
    ENDIF();

TEXT();
    SWITCH (BITS(b, 55, 52));
    CASE(CONST("1", 4));
        SETVAL(comb.vb_sel, ARRITEM(comb.vb_mux, 1));
        ENDCASE();
    CASE(CONST("2", 4));
        SETVAL(comb.vb_sel, ARRITEM(comb.vb_mux, 2));
        ENDCASE();
    CASE(CONST("3", 4));
        SETVAL(comb.vb_sel, ARRITEM(comb.vb_mux, 3));
        ENDCASE();
    CASE(CONST("4", 4));
        SETVAL(comb.vb_sel, ARRITEM(comb.vb_mux, 4));
        ENDCASE();
    CASE(CONST("5", 4));
        SETVAL(comb.vb_sel, ARRITEM(comb.vb_mux, 5));
        ENDCASE();
    CASE(CONST("6", 4));
        SETVAL(comb.vb_sel, ARRITEM(comb.vb_mux, 6));
        ENDCASE();
    CASE(CONST("7", 4));
        SETVAL(comb.vb_sel, ARRITEM(comb.vb_mux, 7));
        ENDCASE();
    CASE(CONST("8", 4));
        SETVAL(comb.vb_sel, ARRITEM(comb.vb_mux, 8));
        ENDCASE();
    CASE(CONST("9", 4));
        SETVAL(comb.vb_sel, ARRITEM(comb.vb_mux, 9));
        ENDCASE();
    CASE(CONST("10", 4));
        SETVAL(comb.vb_sel, ARRITEM(comb.vb_mux, 10));
        ENDCASE();
    CASE(CONST("11", 4));
        SETVAL(comb.vb_sel, ARRITEM(comb.vb_mux, 11));
        ENDCASE();
    CASE(CONST("12", 4));
        SETVAL(comb.vb_sel, ARRITEM(comb.vb_mux, 12));
        ENDCASE();
    CASE(CONST("13", 4));
        SETVAL(comb.vb_sel, ARRITEM(comb.vb_mux, 13));
        ENDCASE();
    CASE(CONST("14", 4));
        SETVAL(comb.vb_sel, ARRITEM(comb.vb_mux, 14));
        ENDCASE();
    CASE(CONST("15", 4));
        SETVAL(comb.vb_sel, ARRITEM(comb.vb_mux, 15));
        ENDCASE();
    CASEDEF();
        SETZERO(comb.vb_sel);
        ENDCASE();
    ENDSWITCH();
    IF (NZ(accum_ena));
        SETVAL(sum, ADD2(CC2(sum, CONST("0", 4)), comb.vb_sel));
        SETVAL(b, CC2(b, CONST("0", 4)));
    ENDIF();

TEXT();
    TEXT("To avoid timing constrains violation try to implement parallel demux");
    TEXT("for Xilinx Vivado");
    i = &FOR ("i", CONST("0"), CONST("104"), "++");
        SETBIT(comb.vb_sumInv, INC(*i), BIT(sum, SUB2(CONST("103"), *i)));
    ENDFOR();
    SETVAL(wb_sumInv, comb.vb_sumInv);

TEXT();
    IF (NZ(BIT(sum, 105)));
        SETVAL(comb.vb_shift, ALLONES());
        SETONE(overflow);
    ELSIF (NZ(BIT(sum, 104)));
        SETZERO(comb.vb_shift);
    ELSE();
        SETVAL(comb.vb_shift, wb_lshift);
    ENDIF();

TEXT();
    IF (NZ(BIT(delay, 14)));
        SETVAL(shift, comb.vb_shift);
        SETZERO(overflow);
        IF (EQ(comb.vb_shift, CONST("0x7f", 7)));
            SETONE(overflow);
        ENDIF();
    ENDIF();

TEXT();
    SYNC_RESET();

TEXT();
    SETVAL(o_result, sum);
    SETVAL(o_shift, shift);
    SETVAL(o_overflow, overflow);
    SETVAL(o_rdy, BIT(delay, 15));
}

