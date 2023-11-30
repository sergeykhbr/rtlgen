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
#include "imul53.h"

using namespace sysvc;

class DoubleMul : public ModuleObject {
 public:
    DoubleMul(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            vb_ena(this, "vb_ena", "5"),
            signA(this, "signA"),
            signB(this, "signB"),
            mantA(this, "mantA", "53"),
            mantB(this, "mantB", "53"),
            zeroA(this, "zeroA", "1"),
            zeroB(this, "zeroB", "1"),
            expAB_t(this, "expAB_t", "12"),
            expAB(this, "expAB", "13"),
            mantAlign(this, "mantAlign", "105"),
            expAlign_t(this, "expAlign_t", "13"),
            expAlign(this, "expAlign", "13"),
            postShift(this, "postShift", "12"),
            mantPostScale(this, "mantPostScale", "105"),
            mantShort(this, "mantShort", "53"),
            tmpMant05(this, "tmpMant05", "52"),
            mantOnes(this, "mantOnes", "1"),
            mantEven(this, "mantEven", "1"),
            mant05(this, "mant05", "1"),
            rndBit(this, "rndBit"),
            nanA(this, "nanA", "1"),
            nanB(this, "nanB", "1"),
            mantZeroA(this, "mantZeroA", "1"),
            mantZeroB(this, "mantZeroB", "1"),
            v_res_sign(this, "v_res_sign", "1"),
            vb_res_exp(this, "vb_res_exp", "11"),
            vb_res_mant(this, "vb_res_mant", "52") {
        }

     public:
        Logic vb_ena;
        Logic1 signA;
        Logic1 signB;
        Logic mantA;
        Logic mantB;
        Logic zeroA;
        Logic zeroB;
        Logic expAB_t;
        Logic expAB;
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
    InPort i_clk;
    InPort i_nrst;
    InPort i_ena;
    InPort i_a;
    InPort i_b;
    OutPort o_res;
    OutPort o_illegal_op;
    OutPort o_overflow;
    OutPort o_valid;
    OutPort o_busy;

 protected:
    Signal w_imul_ena;
    Signal wb_imul_result;
    Signal wb_imul_shift;
    Signal w_imul_rdy;
    Signal w_imul_overflow;

    RegSignal busy;
    RegSignal ena;
    RegSignal a;
    RegSignal b;
    RegSignal result;
    RegSignal zeroA;
    RegSignal zeroB;
    RegSignal mantA;
    RegSignal mantB;
    RegSignal expAB;
    RegSignal expAlign;
    RegSignal mantAlign;
    RegSignal postShift;
    RegSignal mantPostScale;
    RegSignal nanA;
    RegSignal nanB;
    RegSignal overflow;
    RegSignal illegal_op;

    // process should be intialized last to make all signals available
    CombProcess comb;
    // submodules
    imul53 u_imul53;
};

class fmul_d_file : public FileObject {
 public:
    fmul_d_file(GenObject *parent) : FileObject(parent, "fmul_d"),
    m_(this, "") {}

 private:
    DoubleMul m_;
};

