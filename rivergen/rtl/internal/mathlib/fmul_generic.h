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
#include "fmul_istage.h"
#include "lzd_scaler.h"

using namespace sysvc;

class fmul_generic : public ModuleObject {
 public:
    fmul_generic(GenObject *parent, const char *name, const char *comment);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            vb_ena(this, "vb_ena", "5", "'0", NO_COMMENT),
            vb_mant_res_rnd(this, "vb_mant_res_rnd", "ADD(mantbits,2)", "'0", NO_COMMENT),
            vb_exp_res_rnd(this, "vb_exp_res_rnd", "ADD(expbits,2)", "'0", NO_COMMENT),
            v_underflow(this, "v_underflow", "1", "0", NO_COMMENT),
            v_overflow(this, "v_overflow", "1", "0", NO_COMMENT),
            vb_mantA(this, "vb_mantA", "ADD(mantbits,1)", "'0", NO_COMMENT),
            vb_mantB(this, "vb_mantB", "ADD(mantbits,1)", "'0", NO_COMMENT),
            vb_expA_t(this, "vb_expA_t", "ADD(expbits,2)", "'0", NO_COMMENT),
            vb_expB_t(this, "vb_expB_t", "ADD(expbits,2)", "'0", NO_COMMENT),
            vb_expAB_t(this, "vb_expAB_t", "ADD(expbits,2)", "'0", NO_COMMENT),
            mant_even(this, "mant_even", "1"),
            mant05(this, "mant05", "1"),
            mant_rnd(this, "rndBit", "1", "0", NO_COMMENT) {
        }

     public:
        Logic vb_ena;
        Logic vb_mant_res_rnd;
        Logic vb_exp_res_rnd;
        Logic v_underflow;
        Logic v_overflow;
        Logic vb_mantA;
        Logic vb_mantB;
        Logic vb_expA_t;
        Logic vb_expB_t;
        Logic vb_expAB_t;
        Logic mant_even;
        Logic mant05;
        Logic1 mant_rnd;
    };

    void proc_comb();

 public:
    TmplParamI32D fbits;
    TmplParamI32D expbits;
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
    ParamI32D shiftbits;
    ParamI32D explevel;
    ParamI32D hex_chunks;
    ParamI32D latency;

 protected:
    WireArray<Signal> wb_hex_i;
    WireArray<Signal> wb_carry_i;
    WireArray<Signal> wb_zres_i;
    WireArray<Signal> wb_mant_lsb;
    WireArray<Signal> wb_mant_msb;
    Signal wb_mant_full;
    Signal wb_mant_aligned_idx;
    Signal wb_mant_aligned;

    RegSignal ena;
    RegSignal a;
    RegSignal b;
    RegSignal result;
    RegSignal sign;
    RegArray  mantA;
    RegArray  mantB;
    WireArray<RegSignal> expAB;
    RegSignal lzd_noscaling;
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
    ModuleArray<fmul_istage> stagex;
    lzd_scaler scaler0;
};

class fmul_generic_file : public FileObject {
 public:
    fmul_generic_file(GenObject *parent) : FileObject(parent, "fmul_generic"),
    fmul_generic_(this, "fmul_generic", NO_COMMENT) {}

 private:
    fmul_generic fmul_generic_;
};

