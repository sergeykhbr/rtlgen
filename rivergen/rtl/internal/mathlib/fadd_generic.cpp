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
    latency(this, "latency", "10", "Cycles: 1 in latch + 2 scaler + 2 rnd + 1 out latch + 1?"),
    // signals
    wb_mant_aligned_idx(this, "wb_mant_aligned_idx", "shiftbits", "'0", NO_COMMENT),
    wb_mant_aligned(this, "wb_mant_aligned", "mantmaxbits", "'0", NO_COMMENT),
    // registers
    ena(this, "ena", "latency", "'0", NO_COMMENT),
    a(this, "a", "fbits", "'0", NO_COMMENT),
    b(this, "b", "fbits", "'0", NO_COMMENT),
    result(this, "result", "fbits", "'0", NO_COMMENT),
    sub(this, "sub", "3", "'0", "0=adder; 1=substruct"),
    inv(this, "inv", "2", "'0", "1=mant result should be inverted"),
    signA(this, "signA", "1", "'0", NO_COMMENT),
    signB(this, "signB", "1", "'0", NO_COMMENT),
    mantA(this, "mantA", "ADD(mantbits,1)", "'0", NO_COMMENT),
    mantB(this, "mantB", "ADD(mantbits,1)", "'0", NO_COMMENT),
    mantA_swapped(this, "mantA_swapped", "ADD(mantbits,1)", "'0", NO_COMMENT),
    mantB_swapped(this, "mantB_swapped", "ADD(mantbits,1)", "'0", NO_COMMENT),
    mantA_descaled(this, "mantA_descaled", "SUB(mantmaxbits,1)", "'0", NO_COMMENT),
    mantB_descaled(this, "mantB_descaled", "SUB(mantmaxbits,1)", "'0", NO_COMMENT),
    expA(this, "expA", "expbits", "'0", NO_COMMENT),
    expB(this, "expB", "expbits", "'0", NO_COMMENT),
    expAB(this, "expAB", "ADD(expbits,2)", "'0", NO_COMMENT),
    exp_dif(this, "exp_dif", "ADD(expbits,2)", "'0", NO_COMMENT),
    exp_max(this, &i_clk, CLK_POSEDGE, &i_nrst, ACTIVE_LOW, "exp_max", "ADD(expbits,2)", "5", "'0", NO_COMMENT),
    mant_sum_mod(this, "mant_sum_mod", "mantmaxbits", "'0", "Module after mantissas adding"),
    lzd_noscaling(this, "lzd_noscaling", "1", "0", NO_COMMENT),
    lzd_noscaling_z(this, "lzd_noscaling_z", "1", "0", NO_COMMENT),
    sign_res(this, "sign_res", "5", "'0", NO_COMMENT),
    exp_res(this, "exp_res", "ADD(expbits,2)", "'0", NO_COMMENT),
    mant_res(this, "mant_res", "mantbits", "'0", NO_COMMENT),
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
        CONNECT(scaler0, 0, scaler0.i_m, mant_sum_mod);
        CONNECT(scaler0, 0, scaler0.i_noscale, lzd_noscaling_z);
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
    SETVAL(comb.v_expa_nzero, OR_REDUCE(BITS(a, SUB2(fbits, CONST("2")), mantbits)));
    SETVAL(comb.v_expb_nzero, OR_REDUCE(BITS(b, SUB2(fbits, CONST("2")), mantbits)));
    SETVAL(comb.vb_mantA, CC2(comb.v_expa_nzero, BITS(a, DEC(mantbits), CONST("0"))));
    SETVAL(comb.vb_mantB, CC2(comb.v_expb_nzero, BITS(b, DEC(mantbits), CONST("0"))));

    TEXT();
    SETVAL(sub, CC2(BITS(sub, 1, 0), XOR2(BIT(a, DEC(fbits)), BIT(b, DEC(fbits)))));
    SETVAL(signA, BIT(a, DEC(fbits)));
    SETVAL(signB, BIT(b, DEC(fbits)));
    SETVAL(mantA, comb.vb_mantA);
    SETVAL(mantB, comb.vb_mantB);

    TEXT();
    TEXT("expA - expB + EXPONENT_ZERO_LEVEL");
    SETVAL(comb.vb_expA_t, SUB2(CC2(CONST("0", 2), BITS(a, SUB2(fbits, CONST("2")), mantbits)), comb.v_expa_nzero));
    SETVAL(comb.vb_expB_t, SUB2(CC2(CONST("0", 2), BITS(b, SUB2(fbits, CONST("2")), mantbits)), comb.v_expb_nzero));
    SETVAL(comb.vb_expAB_t, SUB2(comb.vb_expA_t, comb.vb_expB_t));
    SETVAL(expA, SUB2(BITS(a, SUB2(fbits, CONST("2")), mantbits), comb.v_expa_nzero));
    SETVAL(expB, SUB2(BITS(b, SUB2(fbits, CONST("2")), mantbits), comb.v_expb_nzero));
    SETVAL(expAB, comb.vb_expAB_t);

    TEXT();
    TEXT("Swap value, so that exponent is always A >= B:");
    IF (EZ(BIT(expAB, INC(expbits))));
        SETVAL(mantA_swapped, mantA);
        SETVAL(mantB_swapped, mantB);
        SETVAL(exp_dif, expAB);
        SETARRITEM(exp_max, CONST("0"), exp_max, expA);
        SETVAL(inv, CC2(BIT(inv, 0), signA));
    ELSE();
        TEXT("Swap A <-> B");
        SETVAL(mantA_swapped, mantB);
        SETVAL(mantB_swapped, mantA);
        SETVAL(exp_dif, INC(INV_L(expAB)));
        SETARRITEM(exp_max, CONST("0"), exp_max, expB);
        SETVAL(inv, CC2(BIT(inv, 0), signB));
    ENDIF();
    i = &FOR_INC(DEC(CONST("5")));
        SETARRITEM(exp_max, INC(*i), exp_max, ARRITEM(exp_max, *i, exp_max));
    ENDFOR();

    TEXT();
    SETVAL(mantA_descaled, CC2(mantA_swapped, CONST("0", "mantbits")));
    SETVAL(comb.vb_mantB_descaled, CC2(mantB_swapped, CONST("0", "mantbits")));
    IF (GE(exp_dif, mantbits));
        SETZERO(mantB_descaled);
    ELSE();
        SETVAL(mantB_descaled, RSH(comb.vb_mantB_descaled, TO_INT(exp_dif)));
    ENDIF();

    TEXT();
    TEXT("Do not scale mantissa if the pre-scaled exponent <= 0:");
    SETZERO(lzd_noscaling);
    IF (ORx(2, &NZ(BIT(exp_dif, INC(expbits))),
                &EZ(exp_dif)));
        SETVAL(lzd_noscaling, BIT(ena, 2));
    ENDIF();
    SETVAL(lzd_noscaling_z, lzd_noscaling);

    TEXT();
    TEXT("Make mantissa always positive and latch was inversion or not");
    TEXT("      - Goes to scaler input when ena[4] == 1");
    TEXT("      - Output is ready on        ena[6] == 1");
    IF (NZ(BIT(sub, 2)));
        SETVAL(comb.vb_mant_sum, SUB2(CC2(CONST("0", 2), mantA_descaled), CC2(CONST("0", 2), mantB_descaled)));
    ELSE();
        SETVAL(comb.vb_mant_sum, ADD2(CC2(CONST("0", 2), mantA_descaled), CC2(CONST("0", 2), mantB_descaled)));
    ENDIF();
    IF (NZ(BIT(comb.vb_mant_sum, mantmaxbits)));
        SETVAL(sign_res, CC2(BITS(sign_res, 4, 0), XOR2(CONST("1", 1), BIT(inv, 1))));
        SETVAL(mant_sum_mod, INC(INV_L(BITS(comb.vb_mant_sum, DEC(mantmaxbits), CONST("0")))));
    ELSE();
        SETVAL(sign_res, CC2(BITS(sign_res, 4, 0), BIT(inv, 1)));
        SETVAL(mant_sum_mod, BITS(comb.vb_mant_sum, DEC(mantmaxbits), CONST("0")));
    ENDIF();

    TEXT();
    TEXT("mant_res_unsigned goes to 'scaler0' submodule input");
    TEXT("  - latency for output is 2 clocks");
    IF (NZ(BIT(wb_mant_aligned, DEC(mantmaxbits))));
        SETVAL(comb.vb_mant_idx_normal, SUB2(wb_mant_aligned_idx, CONST("2", "shiftbits")));
        IF (EZ(wb_mant_aligned_idx));
            SETVAL(exp_res, ADD2(ARRITEM(exp_max, CONST("4"), exp_max), CONST("2")));
        ELSIF (EQ(wb_mant_aligned_idx, CONST("1", "shiftbits")));
            SETVAL(exp_res, ADD2(ARRITEM(exp_max, CONST("4"), exp_max), CONST("1")));
        ELSE();
            SETVAL(exp_res, SUB2(ARRITEM(exp_max, CONST("4"), exp_max), comb.vb_mant_idx_normal));
        ENDIF();
    ELSE();
        SETZERO(exp_res);
    ENDIF();
    SETVAL(mant_res, BITS(wb_mant_aligned, SUB2(mantmaxbits, CONST("2")), SUB2(mantmaxbits, INC(mantbits))), "exclude 1 older bit");
    
    TEXT();
    TEXT("Rounding bit:");
    SETVAL(comb.v_mant_even, INV(BIT(wb_mant_aligned, SUB2(mantmaxbits, INC(mantbits)))));
    SETVAL(comb.v_mant_rnd, BIT(wb_mant_aligned, SUB2(mantmaxbits, ADD2(mantbits, CONST("2")))));
    SETVAL(comb.v_mant05, AND2_L(comb.v_mant_rnd, INV_L(OR_REDUCE(BITS(wb_mant_aligned, SUB2(mantmaxbits, ADD2(mantbits, CONST("3"))), CONST("0"))))));
    SETVAL(rnd_res, AND2(comb.v_mant_rnd, INV(AND2(comb.v_mant05, comb.v_mant_even))));
    
    TEXT();
    SETVAL(comb.vb_mant_res_rnd, ADD2(CC2(CONST("0", 1), mant_res), CC2(ALLZEROS(), rnd_res)));
    SETVAL(comb.vb_exp_res_rnd, ADD2(exp_res, CC2(ALLZEROS(), BIT(comb.vb_mant_res_rnd, mantbits))));

    TEXT();
    TEXT("Overflow: exponent is positive but out-of-range of 'expbits':");
    SETVAL(comb.v_overflow, AND2(INV(BIT(comb.vb_exp_res_rnd, INC(expbits))), BIT(comb.vb_exp_res_rnd, expbits)), "FP32 (exp-8): 01.****.****");
    SETVAL(overflow, comb.v_overflow);

    TEXT();
    TEXT("Undeflow: exponent is negative:");
    SETVAL(comb.v_underflow, BIT(comb.vb_exp_res_rnd, INC(expbits)));
    SETVAL(underflow, comb.v_underflow);

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
    SETVAL(result, CC3(BIT(sign_res, 4), exp_res_rnd, mant_res_rnd));

    TEXT();
    SYNC_RESET();

    TEXT();
    SETVAL(o_res, result);
    SETVAL(o_ex, ex);
    SETVAL(o_valid, BIT(ena, DEC(latency)));
}

