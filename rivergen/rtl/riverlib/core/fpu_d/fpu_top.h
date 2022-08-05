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

#pragma once

#include <api.h>
#include "../../river_cfg.h"
#include "fadd_d.h"
#include "fdiv_d.h"
#include "fmul_d.h"
#include "d2l_d.h"
#include "l2d_d.h"

using namespace sysvc;

class FpuTop : public ModuleObject {
 public:
    FpuTop(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            iv(this, "iv", "Instr_FPU_Total"),
            ProcObject(parent, "comb") {
        }

     public:
        Logic iv;
    };

    void proc_comb();

 public:
    InPort i_clk;
    InPort i_nrst;
    InPort i_ena;
    InPort i_ivec;
    InPort i_a;
    InPort i_b;
    OutPort o_res;
    OutPort o_ex_invalidop;
    OutPort o_ex_divbyzero;
    OutPort o_ex_overflow;
    OutPort o_ex_underflow;
    OutPort o_ex_inexact;
    OutPort o_valid;

 protected:
    Logic w_fadd_d;
    Logic w_fsub_d;
    Logic w_feq_d;
    Logic w_flt_d;
    Logic w_fle_d;
    Logic w_fmax_d;
    Logic w_fmin_d;
    Logic w_fcvt_signed;
    Logic wb_res_fadd;
    Logic w_valid_fadd;
    Logic w_illegalop_fadd;
    Logic w_overflow_fadd;
    Logic w_busy_fadd;

    Logic wb_res_fdiv;
    Logic w_valid_fdiv;
    Logic w_illegalop_fdiv;
    Logic w_divbyzero_fdiv;
    Logic w_overflow_fdiv;
    Logic w_underflow_fdiv;
    Logic w_busy_fdiv;

    Logic wb_res_fmul;
    Logic w_valid_fmul;
    Logic w_illegalop_fmul;
    Logic w_overflow_fmul;
    Logic w_busy_fmul;

    Logic wb_res_d2l;
    Logic w_valid_d2l;
    Logic w_overflow_d2l;
    Logic w_underflow_d2l;
    Logic w_busy_d2l;

    Logic wb_res_l2d;
    Logic w_valid_l2d;
    Logic w_busy_l2d;

    RegSignal ivec;
    RegSignal busy;
    RegSignal ready;
    RegSignal a;
    RegSignal b;
    RegSignal result;
    RegSignal ex_invalidop;
    RegSignal ex_divbyzero;
    RegSignal ex_overflow;
    RegSignal ex_underflow;
    RegSignal ex_inexact;
    RegSignal ena_fadd;
    RegSignal ena_fdiv;
    RegSignal ena_fmul;
    RegSignal ena_d2l;
    RegSignal ena_l2d;
    RegSignal ena_w32;

    // submodules
    DoubleAdd fadd_d0;
    DoubleDiv fdiv_d0;
    DoubleMul fmul_d0;
    Double2Long d2l_d0;
    Long2Double l2d_d0;

    // process should be intialized last to make all signals available
    CombProcess comb;
};

class fpu_top_file : public FileObject {
 public:
    fpu_top_file(GenObject *parent) : FileObject(parent, "fpu_top"),
    m_(this, "") {}

 private:
    FpuTop m_;
};

