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

#include <api_rtlgen.h>
#include "../../river_cfg.h"
#include "fadd_d.h"
#include "fdiv_d.h"
#include "fmul_d.h"
#include "d2l_d.h"
#include "l2d_d.h"

using namespace sysvc;

class FpuTop : public ModuleObject {
 public:
    FpuTop(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

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
    Signal w_fadd_d;
    Signal w_fsub_d;
    Signal w_feq_d;
    Signal w_flt_d;
    Signal w_fle_d;
    Signal w_fmax_d;
    Signal w_fmin_d;
    Signal w_fcvt_signed;
    Signal wb_res_fadd;
    Signal w_valid_fadd;
    Signal w_illegalop_fadd;
    Signal w_overflow_fadd;
    Signal w_busy_fadd;

    Signal wb_res_fdiv;
    Signal w_valid_fdiv;
    Signal w_illegalop_fdiv;
    Signal w_divbyzero_fdiv;
    Signal w_overflow_fdiv;
    Signal w_underflow_fdiv;
    Signal w_busy_fdiv;

    Signal wb_res_fmul;
    Signal w_valid_fmul;
    Signal w_illegalop_fmul;
    Signal w_overflow_fmul;
    Signal w_busy_fmul;

    Signal wb_res_d2l;
    Signal w_valid_d2l;
    Signal w_overflow_d2l;
    Signal w_underflow_d2l;
    Signal w_busy_d2l;

    Signal wb_res_l2d;
    Signal w_valid_l2d;
    Signal w_busy_l2d;

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

