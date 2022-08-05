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

#include "fpu_top.h"

FpuTop::FpuTop(GenObject *parent, const char *name) :
    ModuleObject(parent, "FpuTop", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_ena(this, "i_ena", "1"),
    i_ivec(this, "i_ivec", "Instr_FPU_Total"),
    i_a(this, "i_a", "64", "Operand 1"),
    i_b(this, "i_b", "64", "Operand 2"),
    o_res(this, "o_res", "64", "Result"),
    o_ex_invalidop(this, "o_ex_invalidop", "1", "Exception: invalid operation"),
    o_ex_divbyzero(this, "o_ex_divbyzero", "1", "Exception: divide by zero"),
    o_ex_overflow(this, "o_ex_overflow", "1", "Exception: overflow"),
    o_ex_underflow(this, "o_ex_underflow", "1", "Exception: underflow"),
    o_ex_inexact(this, "o_ex_inexact", "1", "Exception: inexact"),
    o_valid(this, "o_valid", "1", "Result is valid"),
    // signals
    w_fadd_d(this, "w_fadd_d", "1"),
    w_fsub_d(this, "w_fsub_d", "1"),
    w_feq_d(this, "w_feq_d", "1"),
    w_flt_d(this, "w_flt_d", "1"),
    w_fle_d(this, "w_fle_d", "1"),
    w_fmax_d(this, "w_fmax_d", "1"),
    w_fmin_d(this, "w_fmin_d", "1"),
    w_fcvt_signed(this, "w_fcvt_signed", "1"),
    wb_res_fadd(this, "wb_res_fadd", "64"),
    w_valid_fadd(this, "w_valid_fadd", "1"),
    w_illegalop_fadd(this, "w_illegalop_fadd", "1"),
    w_overflow_fadd(this, "w_overflow_fadd", "1"),
    w_busy_fadd(this, "w_busy_fadd", "1"),
    wb_res_fdiv(this, "wb_res_fdiv", "64"),
    w_valid_fdiv(this, "w_valid_fdiv", "1"),
    w_illegalop_fdiv(this, "w_illegalop_fdiv", "1"),
    w_divbyzero_fdiv(this, "w_divbyzero_fdiv", "1"),
    w_overflow_fdiv(this, "w_overflow_fdiv", "1"),
    w_underflow_fdiv(this, "w_underflow_fdiv", "1"),
    w_busy_fdiv(this, "w_busy_fdiv", "1"),
    wb_res_fmul(this, "wb_res_fmul", "64"),
    w_valid_fmul(this, "w_valid_fmul", "1"),
    w_illegalop_fmul(this, "w_illegalop_fmul", "1"),
    w_overflow_fmul(this, "w_overflow_fmul", "1"),
    w_busy_fmul(this, "w_busy_fmul", "1"),
    wb_res_d2l(this, "wb_res_d2l", "64"),
    w_valid_d2l(this, "w_valid_d2l", "1"),
    w_overflow_d2l(this, "w_overflow_d2l", "1"),
    w_underflow_d2l(this, "w_underflow_d2l", "1"),
    w_busy_d2l(this, "w_busy_d2l", "1"),
    wb_res_l2d(this, "wb_res_l2d", "64"),
    w_valid_l2d(this, "w_valid_l2d", "1"),
    w_busy_l2d(this, "w_busy_l2d", "1"),
    // registers
    ivec(this, "ivec", "Instr_FPU_Total"),
    busy(this, "busy", "1"),
    ready(this, "ready", "1"),
    a(this, "a", "64"),
    b(this, "b", "64"),
    result(this, "result", "64"),
    ex_invalidop(this, "ex_invalidop", "1", "Exception: invalid operation"),
    ex_divbyzero(this, "ex_divbyzero", "1", "Exception: divide by zero"),
    ex_overflow(this, "ex_overflow", "1", "Exception: overflow"),
    ex_underflow(this, "ex_underflow", "1", "Exception: underflow"),
    ex_inexact(this, "ex_inexact", "1", "Exception: inexact"),
    ena_fadd(this, "ena_fadd", "1"),
    ena_fdiv(this, "ena_fdiv", "1"),
    ena_fmul(this, "ena_fmul", "1"),
    ena_d2l(this, "ena_d2l", "1"),
    ena_l2d(this, "ena_l2d", "1"),
    ena_w32(this, "ena_w32", "1"),
    // process
    comb(this),
    fadd_d0(this, "fadd_d0"),
    fdiv_d0(this, "fdiv_d0"),
    fmul_d0(this, "fmul_d0"),
    d2l_d0(this, "d2l_d0"),
    l2d_d0(this, "l2d_d0")
{
    Operation::start(this);

    NEW(fadd_d0, fadd_d0.getName().c_str());
        CONNECT(fadd_d0, 0, fadd_d0.i_clk, i_clk);
        CONNECT(fadd_d0, 0, fadd_d0.i_nrst, i_nrst);
        CONNECT(fadd_d0, 0, fadd_d0.i_ena, ena_fadd);
        CONNECT(fadd_d0, 0, fadd_d0.i_add, w_fadd_d);
        CONNECT(fadd_d0, 0, fadd_d0.i_sub, w_fsub_d);
        CONNECT(fadd_d0, 0, fadd_d0.i_eq, w_feq_d);
        CONNECT(fadd_d0, 0, fadd_d0.i_lt, w_flt_d);
        CONNECT(fadd_d0, 0, fadd_d0.i_le, w_fle_d);
        CONNECT(fadd_d0, 0, fadd_d0.i_max, w_fmax_d);
        CONNECT(fadd_d0, 0, fadd_d0.i_min, w_fmin_d);
        CONNECT(fadd_d0, 0, fadd_d0.i_a, a);
        CONNECT(fadd_d0, 0, fadd_d0.i_b, b);
        CONNECT(fadd_d0, 0, fadd_d0.o_res, wb_res_fadd);
        CONNECT(fadd_d0, 0, fadd_d0.o_illegal_op, w_illegalop_fadd);
        CONNECT(fadd_d0, 0, fadd_d0.o_overflow, w_overflow_fadd);
        CONNECT(fadd_d0, 0, fadd_d0.o_valid, w_valid_fadd);
        CONNECT(fadd_d0, 0, fadd_d0.o_busy, w_busy_fadd);
    ENDNEW();

    NEW(fdiv_d0, fdiv_d0.getName().c_str());
        CONNECT(fdiv_d0, 0, fdiv_d0.i_clk, i_clk);
        CONNECT(fdiv_d0, 0, fdiv_d0.i_nrst, i_nrst);
        CONNECT(fdiv_d0, 0, fdiv_d0.i_ena, ena_fdiv);
        CONNECT(fdiv_d0, 0, fdiv_d0.i_a, a);
        CONNECT(fdiv_d0, 0, fdiv_d0.i_b, b);
        CONNECT(fdiv_d0, 0, fdiv_d0.o_res, wb_res_fdiv);
        CONNECT(fdiv_d0, 0, fdiv_d0.o_illegal_op, w_illegalop_fdiv);
        CONNECT(fdiv_d0, 0, fdiv_d0.o_divbyzero, w_divbyzero_fdiv);
        CONNECT(fdiv_d0, 0, fdiv_d0.o_overflow, w_overflow_fdiv);
        CONNECT(fdiv_d0, 0, fdiv_d0.o_underflow, w_underflow_fdiv);
        CONNECT(fdiv_d0, 0, fdiv_d0.o_valid, w_valid_fdiv);
        CONNECT(fdiv_d0, 0, fdiv_d0.o_busy, w_busy_fdiv);
    ENDNEW();

    NEW(fmul_d0, fmul_d0.getName().c_str());
        CONNECT(fmul_d0, 0, fmul_d0.i_clk, i_clk);
        CONNECT(fmul_d0, 0, fmul_d0.i_nrst, i_nrst);
        CONNECT(fmul_d0, 0, fmul_d0.i_ena, ena_fmul);
        CONNECT(fmul_d0, 0, fmul_d0.i_a, a);
        CONNECT(fmul_d0, 0, fmul_d0.i_b, b);
        CONNECT(fmul_d0, 0, fmul_d0.o_res, wb_res_fmul);
        CONNECT(fmul_d0, 0, fmul_d0.o_illegal_op, w_illegalop_fmul);
        CONNECT(fmul_d0, 0, fmul_d0.o_overflow, w_overflow_fmul);
        CONNECT(fmul_d0, 0, fmul_d0.o_valid, w_valid_fmul);
        CONNECT(fmul_d0, 0, fmul_d0.o_busy, w_busy_fmul);
    ENDNEW();

    NEW(d2l_d0, d2l_d0.getName().c_str());
        CONNECT(d2l_d0, 0, d2l_d0.i_clk, i_clk);
        CONNECT(d2l_d0, 0, d2l_d0.i_nrst, i_nrst);
        CONNECT(d2l_d0, 0, d2l_d0.i_ena, ena_d2l);
        CONNECT(d2l_d0, 0, d2l_d0.i_signed, w_fcvt_signed);
        CONNECT(d2l_d0, 0, d2l_d0.i_w32, ena_w32);
        CONNECT(d2l_d0, 0, d2l_d0.i_a, a);
        CONNECT(d2l_d0, 0, d2l_d0.o_res, wb_res_d2l);
        CONNECT(d2l_d0, 0, d2l_d0.o_overflow, w_overflow_d2l);
        CONNECT(d2l_d0, 0, d2l_d0.o_underflow, w_underflow_d2l);
        CONNECT(d2l_d0, 0, d2l_d0.o_valid, w_valid_d2l);
        CONNECT(d2l_d0, 0, d2l_d0.o_busy, w_busy_d2l);
    ENDNEW();

    NEW(l2d_d0, l2d_d0.getName().c_str());
        CONNECT(l2d_d0, 0, l2d_d0.i_clk, i_clk);
        CONNECT(l2d_d0, 0, l2d_d0.i_nrst, i_nrst);
        CONNECT(l2d_d0, 0, l2d_d0.i_ena, ena_l2d);
        CONNECT(l2d_d0, 0, l2d_d0.i_signed, w_fcvt_signed);
        CONNECT(l2d_d0, 0, l2d_d0.i_w32, ena_w32);
        CONNECT(l2d_d0, 0, l2d_d0.i_a, a);
        CONNECT(l2d_d0, 0, l2d_d0.o_res, wb_res_l2d);
        CONNECT(l2d_d0, 0, l2d_d0.o_valid, w_valid_l2d);
        CONNECT(l2d_d0, 0, l2d_d0.o_busy, w_busy_l2d);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void FpuTop::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;

    SETVAL(comb.iv, i_ivec);
    SETZERO(ena_fadd);
    SETZERO(ena_fdiv);
    SETZERO(ena_fmul);
    SETZERO(ena_d2l);
    SETZERO(ena_l2d);
    SETZERO(ready);
    IF (AND2(NZ(i_ena), EZ(busy)));
        SETONE(busy);
        SETVAL(a, i_a);
        SETVAL(b, i_b);
        SETVAL(ivec, i_ivec);
        SETZERO(ex_invalidop);
        SETZERO(ex_divbyzero);
        SETZERO(ex_overflow);
        SETZERO(ex_underflow);
        SETZERO(ex_inexact);

TEXT();
        SETVAL(ena_fadd, ORx(7, &BIT(comb.iv, SUB2(CONST("Instr_FADD_D"), CONST("Instr_FADD_D"))),
                                &BIT(comb.iv, SUB2(CONST("Instr_FSUB_D"), CONST("Instr_FADD_D"))),
                                &BIT(comb.iv, SUB2(CONST("Instr_FLE_D"), CONST("Instr_FADD_D"))),
                                &BIT(comb.iv, SUB2(CONST("Instr_FLT_D"), CONST("Instr_FADD_D"))),
                                &BIT(comb.iv, SUB2(CONST("Instr_FEQ_D"), CONST("Instr_FADD_D"))),
                                &BIT(comb.iv, SUB2(CONST("Instr_FMAX_D"), CONST("Instr_FADD_D"))),
                                &BIT(comb.iv, SUB2(CONST("Instr_FMIN_D"), CONST("Instr_FADD_D")))));
        SETVAL(ena_fdiv, BIT(comb.iv, SUB2(CONST("Instr_FDIV_D"), CONST("Instr_FADD_D"))));
        SETVAL(ena_fmul, BIT(comb.iv, SUB2(CONST("Instr_FMUL_D"), CONST("Instr_FADD_D"))));
        SETVAL(ena_d2l, ORx(4, &BIT(comb.iv, SUB2(CONST("Instr_FCVT_LU_D"), CONST("Instr_FADD_D"))),
                               &BIT(comb.iv, SUB2(CONST("Instr_FCVT_L_D"), CONST("Instr_FADD_D"))),
                               &BIT(comb.iv, SUB2(CONST("Instr_FCVT_WU_D"), CONST("Instr_FADD_D"))),
                               &BIT(comb.iv, SUB2(CONST("Instr_FCVT_W_D"), CONST("Instr_FADD_D")))));
        SETVAL(ena_l2d, ORx(4, &BIT(comb.iv, SUB2(CONST("Instr_FCVT_D_LU"), CONST("Instr_FADD_D"))),
                               &BIT(comb.iv, SUB2(CONST("Instr_FCVT_D_L"), CONST("Instr_FADD_D"))),
                               &BIT(comb.iv, SUB2(CONST("Instr_FCVT_D_WU"), CONST("Instr_FADD_D"))),
                               &BIT(comb.iv, SUB2(CONST("Instr_FCVT_D_W"), CONST("Instr_FADD_D")))));
        SETVAL(ena_w32, ORx(4, &BIT(comb.iv, SUB2(CONST("Instr_FCVT_WU_D"), CONST("Instr_FADD_D"))),
                               &BIT(comb.iv, SUB2(CONST("Instr_FCVT_W_D"), CONST("Instr_FADD_D"))),
                               &BIT(comb.iv, SUB2(CONST("Instr_FCVT_D_WU"), CONST("Instr_FADD_D"))),
                               &BIT(comb.iv, SUB2(CONST("Instr_FCVT_D_W"), CONST("Instr_FADD_D")))));
    ENDIF();

#if 0
    if (r.busy.read() == 1 && (r.ivec.read()[Instr_FMOV_X_D - Instr_FADD_D]
                        | r.ivec.read()[Instr_FMOV_D_X - Instr_FADD_D]) == 1) {
        v.busy = 0;
        v.ready = 1;
        v.result = r.a;
    } else if (w_valid_fadd == 1) {
        v.busy = 0;
        v.ready = 1;
        v.result = wb_res_fadd;
        v.ex_invalidop = w_illegalop_fadd;
        v.ex_overflow = w_overflow_fadd;
    } else if (w_valid_fdiv == 1) {
        v.busy = 0;
        v.ready = 1;
        v.result = wb_res_fdiv;
        v.ex_invalidop = w_illegalop_fdiv;
        v.ex_divbyzero = w_divbyzero_fdiv;
        v.ex_overflow = w_overflow_fdiv;
        v.ex_underflow = w_underflow_fdiv;
    } else if (w_valid_fmul == 1) {
        v.busy = 0;
        v.ready = 1;
        v.result = wb_res_fmul;
        v.ex_invalidop = w_illegalop_fmul;
        v.ex_overflow = w_overflow_fmul;
    } else if (w_valid_d2l == 1) {
        v.busy = 0;
        v.ready = 1;
        v.result = wb_res_d2l;
        v.ex_overflow = w_overflow_d2l;
        v.ex_underflow = w_underflow_d2l;
    } else if (w_valid_l2d == 1) {
        v.busy = 0;
        v.ready = 1;
        v.result = wb_res_l2d;
    }


TEXT();
    SYNC_RESET(*this);

TEXT();
    w_fadd_d = r.ivec.read()[Instr_FADD_D - Instr_FADD_D].to_bool();
    w_fsub_d = r.ivec.read()[Instr_FSUB_D - Instr_FADD_D].to_bool();
    w_feq_d = r.ivec.read()[Instr_FEQ_D - Instr_FADD_D].to_bool();
    w_flt_d = r.ivec.read()[Instr_FLT_D - Instr_FADD_D].to_bool();
    w_fle_d = r.ivec.read()[Instr_FLE_D - Instr_FADD_D].to_bool();
    w_fmax_d = r.ivec.read()[Instr_FMAX_D - Instr_FADD_D].to_bool();
    w_fmin_d = r.ivec.read()[Instr_FMIN_D - Instr_FADD_D].to_bool();
    w_fcvt_signed = (r.ivec.read()[Instr_FCVT_L_D - Instr_FADD_D] |
                     r.ivec.read()[Instr_FCVT_D_L - Instr_FADD_D] |
                     r.ivec.read()[Instr_FCVT_W_D - Instr_FADD_D] |
                     r.ivec.read()[Instr_FCVT_D_W - Instr_FADD_D]).to_bool();
#endif
TEXT();
    SETVAL(o_res, result);
    SETVAL(o_ex_invalidop, ex_invalidop);
    SETVAL(o_ex_divbyzero, ex_divbyzero);
    SETVAL(o_ex_overflow, ex_overflow);
    SETVAL(o_ex_underflow, ex_underflow);
    SETVAL(o_ex_inexact, ex_inexact);
    SETVAL(o_valid, ready);
}

