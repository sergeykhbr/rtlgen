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

#include "lzd_scaler.h"

lzd_scaler::lzd_scaler(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "lzd_scaler", name, comment),
    ibits(this, "ibits", "22", "Input/output bitwise"),
    shiftbits(this, "shiftbits", "4", "Operand shift value: must be $clog2(ibits)"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_m(this, "i_m", "ibits", "input operand"),
    i_noscale(this, "i_noscale", "1", "pass unscaled value directly to output"),
    o_scaled(this, "o_scaled", "ibits", "Scaled operand. 2 Cyles delay"),
    o_scaled_factor(this, "o_scaled_factor", "shiftbits", "Scaling factor. 2 Cyles delay"),
    // parameters
    lzd_chunks(this, "lzd_chunks", "DIV(ADD(ibits,7),8)", "Leading Zero Detector chunks, each 8 bits"),
    lzd_bits(this, "lzd_bits", "MUL(8,lzd_chunks)", "8 bits aligned bitwise"),
    // signals
    // registers
    lzd_factor(this, &i_clk, CLK_POSEDGE, &i_nrst, ACTIVE_LOW, "lzd_factor", "shiftbits", "lzd_chunks", "'0", NO_COMMENT),
    lzd_m(this, &i_clk, CLK_POSEDGE, &i_nrst, ACTIVE_LOW, "lzd_m", "lzd_bits", "lzd_chunks", "'0", NO_COMMENT),
    lzd_noscale(this, "lzd_noscale", "1", "0", NO_COMMENT),
    scaled_factor(this, "scaled_factor", "shiftbits", "'0", NO_COMMENT),
    scaled(this, "scaled", "ibits", "'0", NO_COMMENT),
    // process
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void lzd_scaler::proc_comb() {
    GenObject *i;
    SETVAL(lzd_noscale, i_noscale);

    TEXT();
    TEXT("Detect Leading bit of Mantissa MSB.");
    TEXT("Two stages:");
    TEXT("      - Use chunks 8-bits each and save shifted mantissa for each chunk");
    TEXT("      - Select the first non-zero shift");
    TEXT("      - Ignore bits that out-of-range of exponent");
    SETBITS(comb.vb_lzd, DEC(lzd_bits), SUB2(lzd_bits, ibits), i_m);
    i = &FOR_INC(lzd_chunks);
        IF (ORx(2, &NZ(BIT(comb.vb_lzd, ADD2(MUL2(CONST("8"),*i), CONST("7")))),
                   &NZ(i_noscale)));
            TEXT("No shift:");
            SETARRITEM(lzd_factor, *i, lzd_factor, TO_LOGIC(SUB2(ibits, ADD2(MUL2(CONST("8"),*i), CONST("8"))), shiftbits));
            IF (NE(*i, DEC(lzd_chunks)));
                SETARRITEM(lzd_m, *i, lzd_m, LSH(comb.vb_lzd, MUL2(CONST("8"), SUB2(DEC(lzd_chunks), *i))));
            ELSE();
                SETARRITEM(lzd_m, *i, lzd_m, comb.vb_lzd);
            ENDIF();
        ELSIF (NZ(BIT(comb.vb_lzd, ADD2(MUL2(CONST("8"),*i), CONST("6")))));
            SETARRITEM(lzd_factor, *i, lzd_factor, TO_LOGIC(SUB2(ibits, ADD2(MUL2(CONST("8"),*i), CONST("7"))), shiftbits));
            SETARRITEM(lzd_m, *i, lzd_m, LSH(comb.vb_lzd, ADD2(MUL2(CONST("8"), SUB2(DEC(lzd_chunks), *i)), CONST("1"))));
        ELSIF (NZ(BIT(comb.vb_lzd, ADD2(MUL2(CONST("8"),*i), CONST("5")))));
            SETARRITEM(lzd_factor, *i, lzd_factor, TO_LOGIC(SUB2(ibits, ADD2(MUL2(CONST("8"),*i), CONST("6"))), shiftbits));
            SETARRITEM(lzd_m, *i, lzd_m, LSH(comb.vb_lzd, ADD2(MUL2(CONST("8"), SUB2(DEC(lzd_chunks), *i)), CONST("2"))));
        ELSIF (NZ(BIT(comb.vb_lzd, ADD2(MUL2(CONST("8"),*i), CONST("4")))));
            SETARRITEM(lzd_factor, *i, lzd_factor, TO_LOGIC(SUB2(ibits, ADD2(MUL2(CONST("8"),*i), CONST("5"))), shiftbits));
            SETARRITEM(lzd_m, *i, lzd_m, LSH(comb.vb_lzd, ADD2(MUL2(CONST("8"), SUB2(DEC(lzd_chunks), *i)), CONST("3"))));
        ELSIF (NZ(BIT(comb.vb_lzd, ADD2(MUL2(CONST("8"),*i), CONST("3")))));
            SETARRITEM(lzd_factor, *i, lzd_factor, TO_LOGIC(SUB2(ibits, ADD2(MUL2(CONST("8"),*i), CONST("4"))), shiftbits));
            SETARRITEM(lzd_m, *i, lzd_m, LSH(comb.vb_lzd, ADD2(MUL2(CONST("8"), SUB2(DEC(lzd_chunks), *i)), CONST("4"))));
        ELSIF (NZ(BIT(comb.vb_lzd, ADD2(MUL2(CONST("8"),*i), CONST("2")))));
            SETARRITEM(lzd_factor, *i, lzd_factor, TO_LOGIC(SUB2(ibits, ADD2(MUL2(CONST("8"),*i), CONST("3"))), shiftbits));
            SETARRITEM(lzd_m, *i, lzd_m, LSH(comb.vb_lzd, ADD2(MUL2(CONST("8"), SUB2(DEC(lzd_chunks), *i)), CONST("5"))));
        ELSIF (NZ(BIT(comb.vb_lzd, ADD2(MUL2(CONST("8"),*i), CONST("1")))));
            SETARRITEM(lzd_factor, *i, lzd_factor, TO_LOGIC(SUB2(ibits, ADD2(MUL2(CONST("8"),*i), CONST("2"))), shiftbits));
            SETARRITEM(lzd_m, *i, lzd_m, LSH(comb.vb_lzd, ADD2(MUL2(CONST("8"), SUB2(DEC(lzd_chunks), *i)), CONST("6"))));
        ELSIF (NZ(BIT(comb.vb_lzd, ADD2(MUL2(CONST("8"),*i), CONST("0")))));
            SETARRITEM(lzd_factor, *i, lzd_factor, TO_LOGIC(SUB2(ibits, ADD2(MUL2(CONST("8"),*i), CONST("1"))), shiftbits));
            SETARRITEM(lzd_m, *i, lzd_m, LSH(comb.vb_lzd, ADD2(MUL2(CONST("8"), SUB2(DEC(lzd_chunks), *i)), CONST("7"))));
        ELSE();
            SETARRITEM(lzd_factor, *i, lzd_factor, ALLZEROS());
            SETARRITEM(lzd_m, *i, lzd_m, ALLZEROS());
        ENDIF();
    ENDFOR();

    TEXT();
    SETZERO(scaled_factor);
    SETZERO(scaled);
    IF (NZ(lzd_noscale));
        SETZERO(scaled_factor);
        SETVAL(scaled, BITS(ARRITEM(lzd_m, DEC(lzd_chunks), lzd_m), DEC(lzd_bits), SUB2(lzd_bits, ibits)));
    ELSE();
        i = &FOR_INC(lzd_chunks);
            IF (NZ(BIT(ARRITEM(lzd_m, *i, lzd_m), DEC(lzd_bits))));
                SETVAL(scaled_factor, ARRITEM(lzd_factor, *i, lzd_factor));
                SETVAL(scaled, BITS(ARRITEM(lzd_m, *i, lzd_m), DEC(lzd_bits), SUB2(lzd_bits, ibits)));
            ENDIF();
        ENDFOR();
    ENDIF();

    TEXT();
    SYNC_RESET();

    TEXT();
    SETVAL(o_scaled, scaled);
    SETVAL(o_scaled_factor, scaled_factor);
}

