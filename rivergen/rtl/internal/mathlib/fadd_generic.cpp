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

#include "fadd_generic.h"

fadd_generic::fadd_generic(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "fadd_generic", name, comment),
    fbits(this, "fbits", "32", "Input format: FP32 = 32, FP16 = 16, BF16 = 16"),
    expbits(this, "expbits", "8", "Exponent bitwidth: FP64 = 11, FP32 = 8, FP16 = 5, BF16 = 8"),
    shiftbits(this, "shiftbits", "6", "Mantissa scale factor bits: must be $clog2(2*(fbits-expbits)), avoid using $clog2"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_ena(this, "i_ena", "1"),
    i_a(this, "i_a", "fbits", "Operand 1"),
    i_b(this, "i_b", "fbits", "Operand 2"),
    o_res(this, "o_res", "fbits", "Result"),
    o_ex(this, "o_ex", "1", "Exception, overflow or underflow"),
    o_valid(this, "o_valid", "1", "Result is valid"),
    // parameters
    mantbits(this, "mantbits", "SUB(SUB(fbits,expbits),1)", "Encoded mantissa bitwidth: FP64 = 52, FP32 = 23, FP16 = 10, BF16 = 7"),
    mantmaxbits(this, "mantmaxbits", "MUL(2,ADD(mantbits,1))", "Mantissa maximum bitwidth before shifting"),
    explevel(this, "explevel", "SUB(POW2(1,SUB(expbits,1)),1)", "Level 1 for exponent: 1023 (double); 127 (fp32)"),
    latency(this, "latency", "ADD(hex_chunks,7)", "Cycles: 1 in latch + hex_chunks + 2 scaler + 2 rnd + 1 out latch + 1?"),
    // signals
    wb_mant_full(this, "wb_mant_full", "mantmaxbits", NO_COMMENT),
    wb_mant_aligned_idx(this, "wb_mant_aligned_idx", "shiftbits", "'0", NO_COMMENT),
    wb_mant_aligned(this, "wb_mant_aligned", "mantmaxbits", "'0", NO_COMMENT),
    // registers
    ena(this, "ena", "latency", "'0", NO_COMMENT),
    a(this, "a", "fbits", "'0", NO_COMMENT),
    b(this, "b", "fbits", "'0", NO_COMMENT),
    result(this, "result", "fbits", "'0", NO_COMMENT),
    signA(this, "signA", "SUB(latency,2)", "'0", NO_COMMENT),
    signB(this, "signB", "SUB(latency,2)", "'0", NO_COMMENT),
    mantA(this, "mantA", "ADD(mantbits,1)", "'0", NO_COMMENT),
    mantB(this, "mantB", "ADD(mantbits,1)", "'0", NO_COMMENT),
    mantA_unsigned(this, "mantA_unsigned", "ADD(mantbits,1)", "'0", NO_COMMENT),
    mantB_unsigned(this, "mantB_unsigned", "ADD(mantbits,1)", "'0", NO_COMMENT),
    mantA_descaled(this, "mantA_descaled", "SUB(mantmaxbits,1)", "'0", NO_COMMENT),
    mantB_descaled(this, "mantB_descaled", "SUB(mantmaxbits,1)", "'0", NO_COMMENT),
    expAB(this, "expAB", "ADD(expbits,2)", "'0", NO_COMMENT),
    expAB_unsigned(this, "expAB_unsigned", "ADD(expbits,2)", "'0", NO_COMMENT),
    mant_res_signed(this, "mant_res_signed", "mantmaxbits", "'0", NO_COMMENT),
    mant_res_unsigned(this, "mant_res_unsigned", "mantmaxbits", "'0", NO_COMMENT),
    res_sign(this, "lzd_noscaling", "3", "'0", NO_COMMENT),
    lzd_noscaling(this, "lzd_noscaling", "1", "0", NO_COMMENT),
    exp_res(this, "exp_res", "ADD(expbits,2)", "'0", NO_COMMENT),
    mant_res(this, "mant_res", "ADD(mantbits,1)", "'0", NO_COMMENT),
    rnd_res(this, "rnd_res", "1", "0", NO_COMMENT),
    exp_res_rnd(this, "exp_res_rnd", "expbits", "'0", NO_COMMENT),
    mant_res_rnd(this, "mant_res_rnd", "mantbits", "'0", NO_COMMENT),
    underflow(this, "underflow", "1", RSTVAL_ZERO),
    overflow(this, "overflow", "1", RSTVAL_ZERO),
    ex(this, "ex", "1", RSTVAL_ZERO, "Exception: overflow or underflow"),
    // process
    comb(this),
    scaler0(this, "scaler0", NO_COMMENT)
{
    Operation::start(this);

    TEXT();
    scaler0.ibits.setObjValue(&mantmaxbits);
    scaler0.shiftbits.setObjValue(&shiftbits);
    NEW(scaler0, scaler0.getName().c_str());
        CONNECT(scaler0, 0, scaler0.i_clk, i_clk);
        CONNECT(scaler0, 0, scaler0.i_nrst, i_nrst);
        CONNECT(scaler0, 0, scaler0.i_m, wb_mant_full);
        CONNECT(scaler0, 0, scaler0.i_noscale, lzd_noscaling);
        CONNECT(scaler0, 0, scaler0.o_scaled, wb_mant_aligned);
        CONNECT(scaler0, 0, scaler0.o_scaled_factor, wb_mant_aligned_idx);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void fadd_generic::proc_comb() {
    GenObject *i;

    SETVAL(ena, CC2(BITS(ena, SUB2(latency, CONST("2")), CONST("0")), i_ena));

    TEXT();
    SETVAL(a, i_a);
    SETVAL(b, i_b);

    TEXT();
    SETVAL(signA, CC2(BITS(signA, SUB2(latency, CONST("4")), CONST("0")), BIT(a, DEC(fbits))));
    SETVAL(signB, CC2(BITS(signB, SUB2(latency, CONST("4")), CONST("0")), BIT(b, DEC(fbits))));

    TEXT();
    IF (EZ(BITS(a, SUB2(fbits, CONST("2")), mantbits)));
        SETVAL(comb.vb_mantA, CC2(BITS(a, DEC(mantbits), CONST("0")), CONST("0", 1)));
    ELSE();
        SETVAL(comb.vb_mantA, CC2(CONST("1", 1), BITS(a, DEC(mantbits), CONST("0"))));
    ENDIF();
    IF (EZ(BITS(b, SUB2(fbits, CONST("2")), mantbits)));
        SETVAL(comb.vb_mantB, CC2(BITS(b, DEC(mantbits), CONST("0")), CONST("0", 1)));
    ELSE();
        SETVAL(comb.vb_mantB, CC2(CONST("1", 1), BITS(b, DEC(mantbits), CONST("0"))));
    ENDIF();

    TEXT();
    SETVAL(mantA, comb.vb_mantA);
    SETVAL(mantB, comb.vb_mantB);

    TEXT();
    TEXT("expA - expB + EXPONENT_ZERO_LEVEL");
    SETVAL(comb.vb_expA_t, CC2(CONST("0", 2), BITS(a, SUB2(fbits, CONST("2")), mantbits)));
    SETVAL(comb.vb_expB_t, CC2(CONST("0", 2), BITS(b, SUB2(fbits, CONST("2")), mantbits)));
    SETVAL(comb.vb_expAB_t, SUB2(comb.vb_expA_t, comb.vb_expB_t));
    SETVAL(expAB, SUB2(comb.vb_expAB_t, TO_LOGIC(explevel, ADD2(expbits,CONST("2")))));

    TEXT();
    TEXT("Swap value, so that exponent is always A >= B:");
    IF (NZ(BIT(expAB, INC(expbits))));
        IF (NZ(BIT(signA, 0)));
            SETVAL(mantA_unsigned, INC(INV_L(mantA)));
        ELSE();
            SETVAL(mantA_unsigned, mantA);
        ENDIF();
        IF (NZ(BIT(signB, 0)));
            SETVAL(mantB_unsigned, INC(INV_L(mantB)));
        ELSE();
            SETVAL(mantB_unsigned, mantB);
        ENDIF();
        SETVAL(expAB_unsigned, expAB);
    ELSE();
        TEXT("Swap A <-> B");
        IF (NZ(BIT(signA, 0)));
            SETVAL(mantB_unsigned, INC(INV_L(mantA)));
        ELSE();
            SETVAL(mantB_unsigned, mantA);
        ENDIF();
        IF (NZ(BIT(signB, 0)));
            SETVAL(mantA_unsigned, INC(INV_L(mantB)));
        ELSE();
            SETVAL(mantA_unsigned, mantB);
        ENDIF();
        SETVAL(expAB_unsigned, INC(INV_L(expAB)));
    ENDIF();

    TEXT();
    IF (GE(expAB_unsigned, DEC(mantmaxbits)));
        SETVAL(mantA_descaled, CONST("0", "mantbits"));
        SETZERO(mantB_descaled);
    ELSE();
        SETVAL(mantA_descaled, LSH(mantA_unsigned, expAB_unsigned));
        SETVAL(mantB_descaled, mantB_unsigned);
    ENDIF();

    SETVAL(mant_res_signed, ADD2(CC2(CONST("0", 1), mantA_descaled), CC2(CONST("0", 1), mantB_descaled)));
    IF (NZ(BIT(mant_res_signed, DEC(mantmaxbits))));
        SETVAL(res_sign, CC2(BITS(res_sign, 1, 0), CONST("1", 1)));
        SETVAL(mant_res_unsigned, INC(INV_L(mant_res_signed)));
    ELSE();
        SETVAL(res_sign, CC2(BITS(res_sign, 1, 0), CONST("0", 1)));
        SETVAL(mant_res_unsigned, mant_res_signed);
    ENDIF();

    /*
    TEXT();
    TEXT("Full istage output is valid when (goes to Leading Zero Detector Scaler):");
    TEXT("      FP32: hex_chunks=6, ena[7]");
    TEXT("      ena[hex_chunks + 1] == 1");
    TEXT("      expAB[hex_chunks]");
    TEXT("      {wb_mant_msb[hex_chunks - 1], wb_mant_lsb[hex_chunks - 1]}");
    TEXT("");
    TEXT("Do not scale mantissa if the pre-scaled exponent <= 0:");
    SETZERO(lzd_noscaling);
    IF (ORx(2, &NZ(BIT(ARRITEM(expAB, DEC(hex_chunks), expAB), INC(expbits))),
                &EZ(ARRITEM(expAB, DEC(hex_chunks), expAB))));
        SETVAL(lzd_noscaling, BIT(ena, hex_chunks));
    ENDIF();
    SETVAL(wb_mant_full, CC2(ARRITEM(wb_mant_msb, DEC(hex_chunks), wb_mant_msb), ARRITEM(wb_mant_lsb, DEC(hex_chunks), wb_mant_lsb)));

    TEXT();
    SETVAL(exp_res, SUB2(ARRITEM(expAB, ADD2(hex_chunks, CONST("2")), expAB), wb_mant_aligned_idx));
    SETVAL(mant_res, BITS(wb_mant_aligned, DEC(mantmaxbits), SUB2(mantmaxbits, INC(mantbits))));

    TEXT();
    TEXT("Rounding bit:");
    SETVAL(comb.v_mant_even, INV(BIT(wb_mant_aligned, SUB2(mantmaxbits, INC(mantbits)))));
    SETVAL(comb.v_mant_rnd, BIT(wb_mant_aligned, SUB2(mantmaxbits, ADD2(mantbits, CONST("2")))));
    SETVAL(comb.v_mant05, AND2_L(comb.v_mant_rnd, INV_L(OR_REDUCE(BITS(wb_mant_aligned, SUB2(mantmaxbits, ADD2(mantbits, CONST("3"))), CONST("0"))))));
    SETVAL(rnd_res, AND2(comb.v_mant_rnd, INV(AND2(comb.v_mant05, comb.v_mant_even))));
    
    TEXT();
    SETVAL(comb.vb_mant_res_rnd, ADD2(CC2(CONST("0", 1), mant_res), CC2(ALLZEROS(), rnd_res)));
    SETVAL(comb.vb_exp_res_rnd, ADD2(exp_res, CC2(ALLZEROS(), BITS(comb.vb_mant_res_rnd, INC(mantbits), mantbits))));

    TEXT();
    TEXT("Overflow: exponent is positive but out-of-range of 'expbits':");
    SETVAL(comb.v_overflow, AND2(INV(BIT(comb.vb_exp_res_rnd, INC(expbits))), BIT(comb.vb_exp_res_rnd, expbits)), "FP32 (exp-8): 01.****.****");
    SETVAL(overflow, comb.v_overflow);

    TEXT();
    TEXT("Undeflow: exponent is negative:");
    SETVAL(comb.v_underflow, BIT(comb.vb_exp_res_rnd, INC(expbits)));
    SETVAL(underflow, comb.v_underflow);

    TEXT();
    TEXT("De-normals are the value with the zero exponent (Intel compiler flags):");
    TEXT("  FTZ enabled - when on sets denormals calculated results to zero");
    TEXT("  DAZ disabled - when on treats input denormals as zero.");
    TEXT();
    TEXT("No make sense to detect NaN and other things for GPU computation. No error handling");
    IF (NZ(comb.v_overflow));
        SETVAL(exp_res_rnd, ALLONES());
        SETVAL(mant_res_rnd, ALLONES());
    ELSIF (NZ(comb.v_underflow));
        SETVAL(exp_res_rnd, ALLZEROS());
        SETVAL(mant_res_rnd, ALLZEROS());
    ELSE();
        SETVAL(exp_res_rnd, BITS(comb.vb_exp_res_rnd, DEC(expbits), CONST("0")));
        SETVAL(mant_res_rnd, BITS(comb.vb_mant_res_rnd, DEC(mantbits), CONST("0")));
    ENDIF();
    SETVAL(ex, AND2_L(BIT(ena, SUB2(latency, CONST("2"))), OR2_L(overflow, underflow)));

    TEXT();
    SETVAL(result, CC3(BIT(sign, SUB2(latency, CONST("3"))), exp_res_rnd, mant_res_rnd));*/

    TEXT();
    SYNC_RESET();

    TEXT();
    SETVAL(o_res, result);
    SETVAL(o_ex, ex);
    SETVAL(o_valid, BIT(ena, DEC(latency)));
}

