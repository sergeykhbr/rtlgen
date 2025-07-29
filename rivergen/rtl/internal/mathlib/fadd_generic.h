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

#pragma once

#include <api_rtlgen.h>
#include "lzd_scaler.h"

using namespace sysvc;

class fadd_generic : public ModuleObject {
 public:
    fadd_generic(GenObject *parent, const char *name, const char *comment);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            vb_mant_res_rnd(this, "vb_mant_res_rnd", "ADD(mantbits,1)", "'0", NO_COMMENT),
            vb_exp_res_rnd(this, "vb_exp_res_rnd", "ADD(expbits,2)", "'0", NO_COMMENT),
            v_underflow(this, "v_underflow", "1", "0", NO_COMMENT),
            v_overflow(this, "v_overflow", "1", "0", NO_COMMENT),
            vb_mantA(this, "vb_mantA", "ADD(mantbits,1)", "'0", NO_COMMENT),
            vb_mantB(this, "vb_mantB", "ADD(mantbits,1)", "'0", NO_COMMENT),
            vb_mantB_descaled(this, "vb_mantB_descaled", "SUB(mantmaxbits,1)", "'0", NO_COMMENT),
            vb_expA_t(this, "vb_expA_t", "ADD(expbits,2)", "'0", NO_COMMENT),
            vb_expB_t(this, "vb_expB_t", "ADD(expbits,2)", "'0", NO_COMMENT),
            vb_expAB_t(this, "vb_expAB_t", "ADD(expbits,2)", "'0", NO_COMMENT),
            vb_mant_idx_normal(this, "vb_mant_idx_normal", "shiftbits", "'0", NO_COMMENT),
            vb_mant_sum(this, "vb_mant_sum", "mantmaxbits", "'0", NO_COMMENT),
            v_mant_even(this, "v_mant_even", "1"),
            v_mant05(this, "v_mant05", "1"),
            v_mant_rnd(this, "v_mant_rnd", "1", "0", NO_COMMENT) {
        }

     public:
        Logic vb_mant_res_rnd;
        Logic vb_exp_res_rnd;
        Logic v_underflow;
        Logic v_overflow;
        Logic vb_mantA;
        Logic vb_mantB;
        Logic vb_mantB_descaled;
        Logic vb_expA_t;
        Logic vb_expB_t;
        Logic vb_expAB_t;
        Logic vb_mant_idx_normal;
        Logic vb_mant_sum;
        Logic v_mant_even;
        Logic v_mant05;
        Logic1 v_mant_rnd;
    };

    void proc_comb();

 public:
    TmplParamI32D fbits;
    TmplParamI32D expbits;
    TmplParamI32D shiftbits;
    // IOs
    InPort i_clk;
    InPort i_nrst;
    InPort i_ena;
    InPort i_a;
    InPort i_b;
    OutPort o_res;
    OutPort o_ex;
    OutPort o_valid;

    ParamI32D mantbits;
    ParamI32D mantmaxbits;
    ParamI32D explevel;
    ParamI32D latency;

 protected:
    Signal wb_mant_aligned_idx;
    Signal wb_mant_aligned;

    RegSignal ena;
    RegSignal a;
    RegSignal b;
    RegSignal result;
    RegSignal sub;
    RegSignal inv;
    RegSignal signA;
    RegSignal signB;
    RegSignal mantA;
    RegSignal mantB;
    RegSignal mantA_swapped;
    RegSignal mantB_swapped;
    RegSignal mantA_descaled;
    RegSignal mantB_descaled;
    RegSignal expA;
    RegSignal expB;
    RegSignal expAB;
    RegSignal exp_dif;
    RegArray exp_max;
    RegSignal mant_sum_mod;
    RegSignal lzd_noscaling;
    RegSignal sign_res;
    RegSignal exp_res;
    RegSignal mant_res;
    RegSignal rnd_res;
    RegSignal exp_res_rnd;
    RegSignal mant_res_rnd;
    RegSignal underflow;
    RegSignal overflow;
    RegSignal ex;

    // process should be intialized last to make all signals available
    CombProcess comb;
    // submodules
    lzd_scaler scaler0;
};

class fadd_generic_file : public FileObject {
 public:
    fadd_generic_file(GenObject *parent) : FileObject(parent, "fadd_generic"),
    fadd_generic_(this, "fadd_generic", NO_COMMENT) {}

 private:
    fadd_generic fadd_generic_;
};

