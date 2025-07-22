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

#include "fmul_generic.h"

fmul_generic::fmul_generic(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "fmul_generic", name, comment),
    fbits(this, "fbits", "32", "Input format: FP32 = 32, FP16 = 16, BF16 = 16"),
    expbits(this, "expbits", "8", "Exponent bitwidth: FP64 = 11, FP32 = 8, FP16 = 5, BF16 = 8"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_ena(this, "i_ena", "1"),
    i_a(this, "i_a", "fbits", "Operand 1"),
    i_b(this, "i_b", "fbits", "Operand 2"),
    o_res(this, "o_res", "fbits", "Result"),
    o_overflow(this, "o_overflow", "1"),
    o_valid(this, "o_valid", "1", "Result is valid"),
    // parameters
    mantbits(this, "mantbits", "23", "Mantissa bitwidth: FP64 = 52, FP32 = 23, FP16 = 10, BF16 = 7"),
    mantmaxbits(this, "mantbits", "MUL(2,ADD(mantbits,1))", "Mantissa maximum bitwidth before shifting"),
    shiftbits(this, "shiftbits", "6", "Mantissa shift value: must be $clog2(mantmaxbits)"),
    explevel(this, "explevel", "SUB(POW2(1,SUB(expbits,1)),1)", "Level 1 for exponent: 1023 (double); 127 (fp32)"),
    hex_chunks(this, "hex_chunks", "DIV(ADD(mantbits,3),4)", "Number of hex multipliers"),
    lzd_chunks(this, "lzd_chunks", "DIV(ADD(mantmaxbits,7),8)", "Leading Zero Detector chunks, each 8 bits"),
    lzd_bits(this, "lzd_bits", "MUL(8,lzd_chunks)", "8 bits aligned 2*(mantissa+1) bitwise"),
    // signals
    wb_imul_result(this, "wb_imul_result", "106"),
    wb_imul_shift(this, "wb_imul_shift", "7"),
    w_imul_rdy(this, "w_imul_rdy", "1"),
    w_imul_overflow(this, "w_imul_overflow", "1"),
    wb_hex_i(this, "wb_hex_i", "4", "hex_chunks", NO_COMMENT),
    wb_carry_i(this, "wb_carry_i", "ADD(mantbits,1)", "hex_chunks", NO_COMMENT),
    wb_zres_i(this, "wb_zres_i", "ADD(mantbits,1)", "hex_chunks", NO_COMMENT),
    wb_zshift_i(this, "wb_zshift_i", "shiftbits", "hex_chunks", NO_COMMENT),
    wb_mant_lsb(this, "wb_mant_lsb", "ADD(mantbits,1)", "hex_chunks", NO_COMMENT),
    wb_mant_msb(this, "wb_mant_msb", "ADD(mantbits,1)", "hex_chunks", NO_COMMENT),
    wb_hex_shift(this, "wb_hex_shift", "shiftbits", "hex_chunks", NO_COMMENT),
    // registers
    ena(this, "ena", "5", "'0", NO_COMMENT),
    a(this, "a", "fbits", "'0", NO_COMMENT),
    b(this, "b", "fbits", "'0", NO_COMMENT),
    result(this, "result", "fbits", "'0", NO_COMMENT),
    zeroA(this, "zeroA", "1"),
    zeroB(this, "zeroB", "1"),
    mantA(this, &i_clk, CLK_POSEDGE, &i_nrst, ACTIVE_LOW, "mantA", "ADD(mantbits,1)", "hex_chunks", "'0", NO_COMMENT),
    mantB(this, &i_clk, CLK_POSEDGE, &i_nrst, ACTIVE_LOW, "mantB", "ADD(mantbits,1)", "hex_chunks", "'0", NO_COMMENT),
    expAB(this, &i_clk, CLK_POSEDGE, &i_nrst, ACTIVE_LOW, "expAB", "ADD(expbits,2)", "hex_chunks", "'0", NO_COMMENT),
    lzb_mant_shift(this, &i_clk, CLK_POSEDGE, &i_nrst, ACTIVE_LOW, "lzb_mant_shift", "shiftbits", "lzd_chunks", "'0", NO_COMMENT),
    lzb_mant(this, &i_clk, CLK_POSEDGE, &i_nrst, ACTIVE_LOW, "lzb_mant", "lzd_bits", "lzd_chunks", "'0", NO_COMMENT),
    mant_mask(this, "mant_mask", "mantmaxbits", "'0", "Only masked bits could be scaled, we limited by exponent value that cannot be negative"),
    mant_aligned_idx(this, "mant_aligned_idx", "shiftbits", "'0", NO_COMMENT),
    mant_aligned(this, "mant_aligned", "mantmaxbits", "'0", NO_COMMENT),
    exp_clear(this, "exp_clear", "lzd_chunks", "'0", NO_COMMENT),
    expAlign(this, "expAlign", "ADD(expbits,1)", "'0", NO_COMMENT),
    mantAlign(this, "mantAlign", "SUB(MUL(2,mantbits),1)", "'0", NO_COMMENT),
    postShift(this, "postShift", "ADD(expbits,1)", "'0", NO_COMMENT),
    mantPostScale(this, "mantPostScale", "SUB(MUL(2,mantbits),1)", "'0", NO_COMMENT),
    nanA(this, "nanA", "1"),
    nanB(this, "nanB", "1"),
    overflow(this, "overflow", "1"),
    dbg_lzd(this, "dbg_lzd", "lzd_bits"),
    // process
    comb(this),
    stagex(this, "stagex", "hex_chunks", NO_COMMENT)
{
    Operation::start(this);

TEXT();
    GENERATE("istagegen");
    GenObject *i;
    i = &FORGEN("i", CONST("0"), hex_chunks, "++", new StringConst("istage_x"));
        stagex.idx.setObjValue(i);
        stagex.ibits.setObjValue(&INC(mantbits));
        stagex.mbits.setObjValue(&CONST("4"));
        stagex.shiftbits.setObjValue(&shiftbits);
        NEW(stagex, stagex.getName().c_str(), i);
            CONNECT(stagex, i, stagex.i_clk, i_clk);
            CONNECT(stagex, i, stagex.i_nrst, i_nrst);
            CONNECT(stagex, i, stagex.i_a, ARRITEM_B(mantA, *i, mantA));
            CONNECT(stagex, i, stagex.i_m, ARRITEM_B(wb_hex_i, *i, wb_hex_i));
            CONNECT(stagex, i, stagex.i_zres, ARRITEM_B(wb_zres_i, *i, wb_zres_i));
            CONNECT(stagex, i, stagex.i_zshift, ARRITEM_B(wb_zshift_i, *i, wb_zshift_i));
            CONNECT(stagex, i, stagex.i_carry, ARRITEM_B(wb_carry_i, *i, wb_carry_i));
            CONNECT(stagex, i, stagex.o_result, ARRITEM_B(wb_mant_lsb, *i, wb_mant_lsb));
            CONNECT(stagex, i, stagex.o_carry, ARRITEM_B(wb_mant_msb, *i, wb_mant_msb));
            CONNECT(stagex, i, stagex.o_shift, ARRITEM_B(wb_hex_shift, *i, wb_hex_shift));
        ENDNEW();
    ENDFORGEN(new StringConst("istage_x"));
    ENDGENERATE("istagegen");

    Operation::start(&comb);
    proc_comb();
}

void fmul_generic::proc_comb() {
    GenObject *i;

    SETBIT(comb.vb_ena, 0, i_ena);
    SETBIT(comb.vb_ena, 1, BIT(ena, 0));
    SETBITS(comb.vb_ena, 4, 2, CC2(BITS(ena, 3, 2), w_imul_rdy));
    SETVAL(ena, comb.vb_ena);

    TEXT();
    IF (NZ(i_ena));
        SETZERO(overflow);
        SETVAL(a, i_a);
        SETVAL(b, i_b);
    ENDIF();

    TEXT();
    SETVAL(comb.signA, BIT(a, DEC(fbits)));
    SETVAL(comb.signB, BIT(b, DEC(fbits)));

    TEXT();
    SETVAL(comb.zeroA, INV_L(OR_REDUCE(BITS(a, DEC(fbits), CONST("0")))));
    SETVAL(comb.zeroB, INV_L(OR_REDUCE(BITS(b, DEC(fbits), CONST("0")))));

    TEXT();
    SETVAL(comb.mantA, CCx(2, &OR_REDUCE(BITS(a, DEC(fbits), mantbits)),
                              &BITS(a, DEC(mantbits), CONST("0"))));
    SETVAL(comb.mantB, CCx(2, &OR_REDUCE(BITS(b, DEC(fbits), mantbits)),
                              &BITS(b, DEC(mantbits), CONST("0"))));

    TEXT();
    TEXT("");
    SETARRITEM(mantA, CONST("0"), mantA, comb.mantA);
    SETARRITEM(mantB, CONST("0"), mantB, comb.mantB);
    SETARRITEM(wb_carry_i, CONST("0"), wb_carry_i, ALLZEROS());
    SETARRITEM(wb_zres_i, CONST("0"), wb_zres_i, ALLZEROS());
    SETARRITEM(wb_zshift_i, CONST("0"), wb_zshift_i, ALLZEROS());
    i = &FOR_INC(DEC(hex_chunks));
        SETARRITEM(wb_hex_i, *i, wb_hex_i, BITS(ARRITEM(mantB, *i, mantB), 3, 0));
        SETARRITEM(wb_carry_i, INC(*i), wb_carry_i, ARRITEM(wb_mant_msb, *i, wb_mant_msb));
        SETARRITEM(wb_zres_i, INC(*i), wb_zres_i, ARRITEM(wb_mant_lsb, *i, wb_mant_lsb));
        SETARRITEM(wb_zshift_i, INC(*i), wb_zshift_i, ARRITEM(wb_zshift_i, *i, wb_zshift_i));
        SETARRITEM(mantA, INC(*i), mantA, ARRITEM(mantA, *i, mantA));
        SETARRITEM(mantB, INC(*i), mantB, CC2(CONST("0", 4), BITS(ARRITEM(mantB, *i, mantB), mantbits, CONST("4"))));
    ENDFOR();
    SETARRITEM(wb_hex_i, DEC(hex_chunks), wb_hex_i, BITS(ARRITEM(mantB, DEC(hex_chunks), mantB), 3, 0));

    TEXT();
    TEXT("expA - expB + EXPONENT_ZERO_LEVEL");
    SETVAL(comb.expAB_t, ADD2(CC2(CONST("0", 1), BITS(a, DEC(fbits), mantbits)), CC2(CONST("0", 1), BITS(b, DEC(fbits), mantbits))));
    SETARRITEM(expAB, CONST("0"), expAB, SUB2(CC2(CONST("0", 1), comb.expAB_t), TO_LOGIC(explevel, ADD2(expbits,CONST("2")))));
    i = &FOR_INC(DEC(hex_chunks));
        SETARRITEM(expAB, INC(*i), expAB, ARRITEM(expAB, *i, expAB));
    ENDFOR();

    TEXT();
    TEXT("Cannot scale mantissa's value more than exponent value");
    IF (GE(ARRITEM(expAB, DEC(hex_chunks), expAB), DEC(mantmaxbits)));
        SETVAL(mant_mask, ALLONES());
    ELSE();
        SETVAL(mant_mask, INV_L(RSH(ALLONES(), ARRITEM(expAB, DEC(hex_chunks), expAB))));
    ENDIF();

    TEXT();
    TEXT("Detect Leading bit of Mantissa MSB.");
    TEXT("Two stages:");
    TEXT("      - Use chunks 8-bits each and save shifted mantissa for each chunk");
    TEXT("      - Select the first non-zero shift");
    TEXT("      - Ignore bits that out-of-range of exponent");
    SETBITS(comb.vb_lzd, DEC(lzd_bits), SUB2(lzd_bits, mantmaxbits),
            CC2(ARRITEM(wb_mant_msb, DEC(hex_chunks), wb_mant_msb), ARRITEM(wb_mant_lsb, DEC(hex_chunks), wb_mant_lsb)));
    SETBITS(comb.vb_lzd_mask, DEC(lzd_bits), SUB2(lzd_bits, mantmaxbits), mant_mask);
    SETVAL(comb.vb_lzd_masked, AND2_L(comb.vb_lzd, comb.vb_lzd_mask));
    SETZERO(exp_clear);
    SETVAL(dbg_lzd, comb.vb_lzd, "REMOVE ME");
    i = &FOR_INC(lzd_chunks);
        IF (NZ(BIT(comb.vb_lzd, ADD2(MUL2(CONST("8"),*i), CONST("7")))));
            TEXT("No shift:");
            SETARRITEM(lzb_mant_shift, *i, lzb_mant_shift, TO_LOGIC(ADD2(MUL2(CONST("8"),*i), CONST("7")), shiftbits));
            IF (NE(*i, DEC(lzd_chunks)));
                SETARRITEM(lzb_mant, *i, lzb_mant, LSH(comb.vb_lzd, MUL2(CONST("8"), SUB2(DEC(lzd_chunks), *i))));
            ELSE();
                SETARRITEM(lzb_mant, *i, lzb_mant, comb.vb_lzd);
            ENDIF();
        ELSIF (NZ(BIT(comb.vb_lzd, ADD2(MUL2(CONST("8"),*i), CONST("6")))));
            SETARRITEM(lzb_mant_shift, *i, lzb_mant_shift, TO_LOGIC(ADD2(MUL2(CONST("8"),*i), CONST("6")), shiftbits));
            SETARRITEM(lzb_mant, *i, lzb_mant, LSH(comb.vb_lzd, ADD2(MUL2(CONST("8"), SUB2(DEC(lzd_chunks), *i)), CONST("1"))));
            SETBIT(exp_clear, *i, INV(BIT(comb.vb_lzd, ADD2(MUL2(CONST("8"),*i), CONST("6")))));
        ELSIF (NZ(BIT(comb.vb_lzd, ADD2(MUL2(CONST("8"),*i), CONST("5")))));
            SETARRITEM(lzb_mant_shift, *i, lzb_mant_shift, TO_LOGIC(ADD2(MUL2(CONST("8"),*i), CONST("5")), shiftbits));
            SETARRITEM(lzb_mant, *i, lzb_mant, LSH(comb.vb_lzd, ADD2(MUL2(CONST("8"), SUB2(DEC(lzd_chunks), *i)), CONST("2"))));
            SETBIT(exp_clear, *i, INV(BIT(comb.vb_lzd, ADD2(MUL2(CONST("8"),*i), CONST("5")))));
        ELSIF (NZ(BIT(comb.vb_lzd, ADD2(MUL2(CONST("8"),*i), CONST("4")))));
            SETARRITEM(lzb_mant_shift, *i, lzb_mant_shift, TO_LOGIC(ADD2(MUL2(CONST("8"),*i), CONST("4")), shiftbits));
            SETARRITEM(lzb_mant, *i, lzb_mant, LSH(comb.vb_lzd, ADD2(MUL2(CONST("8"), SUB2(DEC(lzd_chunks), *i)), CONST("3"))));
            SETBIT(exp_clear, *i, INV(BIT(comb.vb_lzd, ADD2(MUL2(CONST("8"),*i), CONST("4")))));
        ELSIF (NZ(BIT(comb.vb_lzd, ADD2(MUL2(CONST("8"),*i), CONST("3")))));
            SETARRITEM(lzb_mant_shift, *i, lzb_mant_shift, TO_LOGIC(ADD2(MUL2(CONST("8"),*i), CONST("3")), shiftbits));
            SETARRITEM(lzb_mant, *i, lzb_mant, LSH(comb.vb_lzd, ADD2(MUL2(CONST("8"), SUB2(DEC(lzd_chunks), *i)), CONST("4"))));
            SETBIT(exp_clear, *i, INV(BIT(comb.vb_lzd, ADD2(MUL2(CONST("8"),*i), CONST("3")))));
        ELSIF (NZ(BIT(comb.vb_lzd, ADD2(MUL2(CONST("8"),*i), CONST("2")))));
            SETARRITEM(lzb_mant_shift, *i, lzb_mant_shift, TO_LOGIC(ADD2(MUL2(CONST("8"),*i), CONST("2")), shiftbits));
            SETARRITEM(lzb_mant, *i, lzb_mant, LSH(comb.vb_lzd, ADD2(MUL2(CONST("8"), SUB2(DEC(lzd_chunks), *i)), CONST("5"))));
            SETBIT(exp_clear, *i, INV(BIT(comb.vb_lzd, ADD2(MUL2(CONST("8"),*i), CONST("2")))));
        ELSIF (NZ(BIT(comb.vb_lzd, ADD2(MUL2(CONST("8"),*i), CONST("1")))));
            SETARRITEM(lzb_mant_shift, *i, lzb_mant_shift, TO_LOGIC(ADD2(MUL2(CONST("8"),*i), CONST("1")), shiftbits));
            SETARRITEM(lzb_mant, *i, lzb_mant, LSH(comb.vb_lzd, ADD2(MUL2(CONST("8"), SUB2(DEC(lzd_chunks), *i)), CONST("6"))));
            SETBIT(exp_clear, *i, INV(BIT(comb.vb_lzd, ADD2(MUL2(CONST("8"),*i), CONST("1")))));
        ELSIF (NZ(BIT(comb.vb_lzd, ADD2(MUL2(CONST("8"),*i), CONST("0")))));
            SETARRITEM(lzb_mant_shift, *i, lzb_mant_shift, TO_LOGIC(ADD2(MUL2(CONST("8"),*i), CONST("0")), shiftbits));
            SETARRITEM(lzb_mant, *i, lzb_mant, LSH(comb.vb_lzd, ADD2(MUL2(CONST("8"), SUB2(DEC(lzd_chunks), *i)), CONST("7"))));
            SETBIT(exp_clear, *i, INV(BIT(comb.vb_lzd, ADD2(MUL2(CONST("8"),*i), CONST("0")))));
        ELSE();
            SETARRITEM(lzb_mant_shift, *i, lzb_mant_shift, ALLZEROS());
            SETARRITEM(lzb_mant, *i, lzb_mant, ALLZEROS());
        ENDIF();
    ENDFOR();

    TEXT();
    SETZERO(mant_aligned_idx);
    SETZERO(mant_aligned);
    i = &FOR_INC(lzd_chunks);
        IF (ORx(2, &NZ(BIT(ARRITEM(lzb_mant, *i, lzb_mant), DEC(lzd_bits))),
                   &NZ(BIT(exp_clear, *i))));
            SETVAL(mant_aligned_idx, ARRITEM(lzb_mant_shift, *i, lzb_mant_shift));
            SETVAL(mant_aligned, ARRITEM(lzb_mant, *i, lzb_mant));
        ENDIF();
    ENDFOR();
    

    TEXT();
    IF (NZ(BIT(ena, 0)));
        SETVAL(zeroA, comb.zeroA);
        SETVAL(zeroB, comb.zeroB);
    ENDIF();

/*
TEXT();
    TEXT("imul53 module:");
    IF (NZ(BIT(wb_imul_result, 105)));
        SETVAL(comb.mantAlign, BITS(wb_imul_result, 105, 1));
    ELSIF (NZ(BIT(wb_imul_result, 104)));
        SETVAL(comb.mantAlign, BITS(wb_imul_result, 104, 0));
    ELSE();
        i = &FOR ("i", CONST("1"), CONST("105"), "++");
            IF (EQ(*i, TO_INT(wb_imul_shift)));
                SETVAL(comb.mantAlign, LSH(wb_imul_result, *i));
            ENDIF();
        ENDFOR();
    ENDIF();

TEXT();
    SETVAL(comb.expAlign_t, INC(expAB));
    IF (NZ(BIT(wb_imul_result, 105)));
        SETVAL(comb.expAlign, comb.expAlign_t);
    ELSIF (OR2(EZ(BITS(a, 62, 52)), EZ(BITS(b, 62, 52))));
        SETVAL(comb.expAlign , SUB2(comb.expAlign_t, CC2(CONST("0", 6), wb_imul_shift)));
    ELSE();
        SETVAL(comb.expAlign, SUB2(expAB, CC2(CONST("0", 6), wb_imul_shift)));
    ENDIF();

TEXT();
    TEXT("IMPORTANT exception! new ZERO value");
    IF (OR2(NZ(BIT(comb.expAlign, 12)), EZ(comb.expAlign)));
        IF (ORx(4, &EZ(wb_imul_shift), 
                   &NZ(BIT(wb_imul_result, 105)),
                   &EZ(BITS(a, 62, 52)),
                   &EZ(BITS(b, 62, 52))));
            SETVAL(comb.postShift, ADD2(INV_L(BITS(comb.expAlign, 11, 0)), CONST("2", 12)));
        ELSE();
            SETVAL(comb.postShift, INC(INV_L(BITS(comb.expAlign, 11, 0))));
        ENDIF();
    ENDIF();

TEXT();
    IF (NZ(w_imul_rdy));
        SETVAL(expAlign, BITS(comb.expAlign, 11, 0));
        SETVAL(mantAlign, comb.mantAlign);
        SETVAL(postShift, comb.postShift);

TEXT();
        TEXT("Exceptions:");
        SETZERO(nanA);
        IF (EQ(BITS(a, 62, 52), CONST("0x7FF", 11)));
            SETONE(nanA);
        ENDIF();
        SETZERO(nanB);
        IF (EQ(BITS(b, 62, 52), CONST("0x7FF", 11)));
            SETONE(nanB);
        ENDIF();
        SETZERO(overflow);
        IF (AND2(EZ(BIT(comb.expAlign,12)), GE(comb.expAlign, CONST("0x7FF", 13))));
            SETONE(overflow);
        ENDIF();
    ENDIF();

TEXT();
    TEXT("Prepare to mantissa post-scale");
    IF (EZ(postShift));
        SETVAL(comb.mantPostScale, mantAlign);
    ELSIF (LS(postShift, CONST("105", 12)));
        i = &FOR ("i", CONST("1"), CONST("105"), "++");
            IF (EQ(*i, TO_INT(postShift)));
                SETVAL(comb.mantPostScale, RSH(mantAlign, *i));
            ENDIF();
        ENDFOR();
    ENDIF();
    IF (NZ(BIT(ena, 2)));
        SETVAL(mantPostScale, comb.mantPostScale);
    ENDIF();

TEXT();
    TEXT("Rounding bit");
    SETVAL(comb.mantShort, BIG_TO_U64(BITS(mantPostScale, 104, 52)));
    SETVAL(comb.tmpMant05, BIG_TO_U64(BITS(mantPostScale, 51, 0)));
    IF (EQ(comb.mantShort, CONST("0x001fffffffffffff", 53)));
        SETONE(comb.mantOnes);
    ENDIF();
    SETVAL(comb.mantEven, BIT(mantPostScale, 52));
    IF (EQ(comb.tmpMant05, CONST("0x0008000000000000", 52)));
        SETONE(comb.mant05);
    ENDIF();
    SETVAL(comb.rndBit, AND2(BIT(mantPostScale, 51), INV(AND2(comb.mant05, INV(comb.mantEven)))));

TEXT();
    TEXT("Check Borders");
    IF (EQ(BITS(a, 62, 52), CONST("0x7ff", 11)));
        SETONE(comb.nanA);
    ENDIF();
    IF (EQ(BITS(b, 62, 52), CONST("0x7ff", 11)));
        SETONE(comb.nanB);
    ENDIF();
    IF (EZ(BITS(a, 51, 0)));
        SETONE(comb.mantZeroA);
    ENDIF();
    IF (EZ(BITS(b, 51, 0)));
        SETONE(comb.mantZeroB);
    ENDIF();

TEXT();
    TEXT("Result multiplexers:");
    IF (OR2(AND3(comb.nanA, comb.mantZeroA, zeroB), AND3(comb.nanB, comb.mantZeroB, zeroA)));
        SETVAL(comb.v_res_sign, CONST("1", 1));
    ELSIF (NZ(AND2(comb.nanA, INV(comb.mantZeroA))));
        TEXT("when both values are NaN, value B has higher priority if sign=1");
        SETVAL(comb.v_res_sign, OR2(comb.signA, AND2(comb.nanA, comb.signB)));
    ELSIF (NZ(AND2(comb.nanB, INV(comb.mantZeroB))));
        SETVAL(comb.v_res_sign, comb.signB);
    ELSE();
        SETVAL(comb.v_res_sign, XOR2(BIT(a, 63), BIT(b, 63)));
    ENDIF();

TEXT();
    IF (NZ(comb.nanA));
        SETVAL(comb.vb_res_exp, BITS(a, 62, 52));
    ELSIF (NZ(comb.nanB));
        SETVAL(comb.vb_res_exp, BITS(b, 62, 52));
    ELSIF (NZ(OR3(BIT(expAlign, 11), zeroA, zeroB)));
        SETVAL(comb.vb_res_exp, ALLZEROS());
    ELSIF (NZ(overflow));
        SETVAL(comb.vb_res_exp, ALLONES());
    ELSE();
        SETVAL(comb.vb_res_exp, ADDx(2, &BITS(expAlign, 10, 0),
                                        &AND3(comb.mantOnes, comb.rndBit, INV(overflow))));
    ENDIF();

TEXT();
    IF (ORx(3, &AND3(comb.nanA, comb.mantZeroA, INV(comb.mantZeroB)),
               &AND3(comb.nanB, comb.mantZeroB, INV(comb.mantZeroA)),
               &AND3(INV(comb.nanA), INV(comb.nanB), overflow)));
        SETVAL(comb.vb_res_mant, ALLZEROS());
    ELSIF (NZ(AND2(comb.nanA, INV(AND2(comb.nanB, comb.signB)))));
        TEXT("when both values are NaN, value B has higher priority if sign=1");
        SETVAL(comb.vb_res_mant, CC2(CONST("1", 1), BITS(a, 50, 0)));
    ELSIF (NZ(comb.nanB));
        SETVAL(comb.vb_res_mant, CC2(CONST("1", 1), BITS(b, 50, 0)));
    ELSE();
        SETVAL(comb.vb_res_mant, ADD2(BITS(comb.mantShort, 51, 0), comb.rndBit));
    ENDIF();

TEXT();
    IF (NZ(BIT(ena, 3)));
        SETVAL(result, CC3(comb.v_res_sign, comb.vb_res_exp, comb.vb_res_mant));
        //SETVAL(illegal_op, OR2(comb.nanA, comb.nanB));
    ENDIF();
*/
TEXT();
    SYNC_RESET();

TEXT();
    SETVAL(o_res, result);
    SETVAL(o_overflow, overflow);
    SETVAL(o_valid, BIT(ena, 4));
}

