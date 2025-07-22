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

using namespace sysvc;

class fmul_generic : public ModuleObject {
 public:
    fmul_generic(GenObject *parent, const char *name, const char *comment);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            vb_ena(this, "vb_ena", "5", "'0", NO_COMMENT),
            vb_mant_last_inv(this, "vb_mant_last_inv", "mantmaxbits", "'0", NO_COMMENT),
            vb_mant_last(this, "vb_mant_last", "mantmaxbits", "'0", NO_COMMENT),
            vb_lzd(this, "vb_lzd", "MUL(8,lzd_chunks)", "'0", NO_COMMENT),
            vb_lzd_mask(this, "vb_lzd_mask", "MUL(8,lzd_chunks)", "'1", "chunk aligned 'mant_mask' value"),
            vb_lzd_masked(this, "vb_lzd_masked", "MUL(8,lzd_chunks)", "'0", NO_COMMENT),
            vb_lzd_last(this, "vb_lzd_last", "MUL(8,lzd_chunks)", "'0", NO_COMMENT),
            vb_exp_clear(this, "exp_clear", "lzd_chunks", "'0", NO_COMMENT),
            signA(this, "signA", "1", "0", NO_COMMENT),
            signB(this, "signB", "1", "0", NO_COMMENT),
            mantA(this, "mantA", "53", "'0", NO_COMMENT),
            mantB(this, "mantB", "53", "'0", NO_COMMENT),
            zeroA(this, "zeroA", "1"),
            zeroB(this, "zeroB", "1"),
            expAB_t(this, "expAB_t", "ADD(expbits,1)", "'0", NO_COMMENT),
            mantAlign(this, "mantAlign", "105", "'0", NO_COMMENT),
            expAlign_t(this, "expAlign_t", "ADD(expbits,2)", "'0", NO_COMMENT),
            expAlign(this, "expAlign", "ADD(expbits,2)", "'0", NO_COMMENT),
            postShift(this, "postShift", "ADD(expbits,1)", "'0", NO_COMMENT),
            mantPostScale(this, "mantPostScale", "105", "'0", NO_COMMENT),
            mantShort(this, "mantShort", "53", "'0", NO_COMMENT),
            tmpMant05(this, "tmpMant05", "52", "'0", NO_COMMENT),
            mantOnes(this, "mantOnes", "1"),
            mantEven(this, "mantEven", "1"),
            mant05(this, "mant05", "1"),
            rndBit(this, "rndBit", "1", "0", NO_COMMENT),
            nanA(this, "nanA", "1"),
            nanB(this, "nanB", "1"),
            mantZeroA(this, "mantZeroA", "1"),
            mantZeroB(this, "mantZeroB", "1"),
            v_res_sign(this, "v_res_sign", "1"),
            vb_res_exp(this, "vb_res_exp", "11", "'0", NO_COMMENT),
            vb_res_mant(this, "vb_res_mant", "52", "'0", NO_COMMENT) {
        }

     public:
        Logic vb_ena;
        Logic vb_mant_last_inv;
        Logic vb_mant_last;
        Logic vb_lzd;
        Logic vb_lzd_mask;
        Logic vb_lzd_masked;
        Logic vb_lzd_last;
        Logic vb_exp_clear;
        Logic1 signA;
        Logic1 signB;
        Logic mantA;
        Logic mantB;
        Logic zeroA;
        Logic zeroB;
        Logic expAB_t;
        Logic mantAlign;
        Logic expAlign_t;
        Logic expAlign;
        Logic postShift;
        Logic mantPostScale;
        Logic mantShort;
        Logic tmpMant05;
        Logic mantOnes;
        Logic mantEven;
        Logic mant05;
        Logic1 rndBit;
        Logic nanA;
        Logic nanB;
        Logic mantZeroA;
        Logic mantZeroB;
        Logic v_res_sign;
        Logic vb_res_exp;
        Logic vb_res_mant;
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
    OutPort o_overflow;
    OutPort o_valid;

    ParamI32D mantbits;
    ParamI32D mantmaxbits;
    ParamI32D shiftbits;
    ParamI32D explevel;
    ParamI32D hex_chunks;
    ParamI32D lzd_chunks;
    ParamI32D lzd_bits;

 protected:
    Signal wb_imul_result;
    Signal wb_imul_shift;
    Signal w_imul_rdy;
    Signal w_imul_overflow;
    WireArray<Signal> wb_hex_i;
    WireArray<Signal> wb_carry_i;
    WireArray<Signal> wb_zres_i;
    WireArray<Signal> wb_mant_lsb;
    WireArray<Signal> wb_mant_msb;

    RegSignal ena;
    RegSignal a;
    RegSignal b;
    RegSignal result;
    RegSignal zeroA;
    RegSignal zeroB;
    RegArray  mantA;
    RegArray  mantB;
    WireArray<RegSignal> expAB;
    WireArray<RegSignal> lzb_mant_shift;
    WireArray<RegSignal> lzb_mant;
    RegSignal mant_mask;
    RegSignal mant_last;
    RegSignal mant_aligned_idx;
    RegSignal mant_aligned;
    RegSignal exp_clear;
    RegSignal exp_res;
    RegSignal mant_res;
    RegSignal exp_res_rnd;
    RegSignal mant_res_rnd;
    RegSignal overflow;
    RegSignal dbg_lzd;

    // process should be intialized last to make all signals available
    CombProcess comb;
    // submodules
    ModuleArray<fmul_istage> stagex;
};

class fmul_generic_file : public FileObject {
 public:
    fmul_generic_file(GenObject *parent) : FileObject(parent, "fmul_generic"),
    fmul_generic_(this, "fmul_generic", NO_COMMENT) {}

 private:
    fmul_generic fmul_generic_;
};

