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

using namespace sysvc;

class DoubleAdd : public ModuleObject {
 public:
    DoubleAdd(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            v_ena(this, "v_ena"),
            signOp(this, "signOp"),
            signA(this, "signA"),
            signB(this, "signB"),
            signOpB(this, "signOpB"),
            mantA(this, "mantA", "53"),
            mantB(this, "mantB", "53"),
            mantDif(this, "mantDif", "54"),
            expDif(this, "expDif", "12"),
            v_flMore(this, "v_flMore", "1"),
            v_flEqual(this, "v_flEqual", "1"),
            v_flLess(this, "v_flLess", "1"),
            vb_preShift(this, "vb_preShift", "12"),
            v_signOpMore(this, "v_signOpMore", "1"),
            vb_expMore(this, "vb_expMore", "11"),
            vb_mantMore(this, "vb_mantMore", "53"),
            vb_mantLess(this, "vb_mantLess", "53"),
            mantMoreScale(this, "mantMoreScale", "105"),
            mantLessScale(this, "mantLessScale", "105"),
            vb_mantSum(this, "vb_mantSum", "106"),
            vb_lshift(this, "vb_lshift", "7"),
            vb_mantAlign(this, "vb_mantAlign", "105"),
            vb_expPostScale(this, "vb_expPostScale", "12"),
            vb_mantPostScale(this, "vb_mantPostScale", "105"),
            mantShort(this, "mantShort", "53"),
            tmpMant05(this, "tmpMant05", "52"),
            mantOnes(this, "mantOnes", "1"),
            mantEven(this, "mantEven", "1"),
            mant05(this, "mant05", "1"),
            rndBit(this, "rndBit"),
            mantZeroA(this, "mantZeroA", "1"),
            mantZeroB(this, "mantZeroB", "1"),
            allZero(this, "allZero", "1"),
            sumZero(this, "sumZero", "1"),
            nanA(this, "nanA", "1"),
            nanB(this, "nanB", "1"),
            nanAB(this, "nanAB", "1"),
            overflow(this, "overflow", "1"),
            resAdd(this, "resAdd", "64"),
            resEQ(this, "resEQ", "64"),
            resLT(this, "resLT", "64"),
            resLE(this, "resLE", "64"),
            resMax(this, "resMax", "64"),
            resMin(this, "resMin", "64") {
        }

     public:
        Logic1 v_ena;
        Logic1 signOp;
        Logic1 signA;
        Logic1 signB;
        Logic1 signOpB;
        Logic mantA;
        Logic mantB;
        Logic mantDif;
        Logic expDif;
        Logic v_flMore;
        Logic v_flEqual;
        Logic v_flLess;
        Logic vb_preShift;
        Logic v_signOpMore;
        Logic vb_expMore;
        Logic vb_mantMore;
        Logic vb_mantLess;
        Logic mantMoreScale;
        Logic mantLessScale;
        Logic vb_mantSum;
        Logic vb_lshift;
        Logic vb_mantAlign;
        Logic vb_expPostScale;
        Logic vb_mantPostScale;
        Logic mantShort;
        Logic tmpMant05;
        Logic mantOnes;
        Logic mantEven;
        Logic mant05;
        Logic1 rndBit;
        Logic mantZeroA;
        Logic mantZeroB;
        Logic allZero;
        Logic sumZero;
        Logic nanA;
        Logic nanB;
        Logic nanAB;
        Logic overflow;
        Logic resAdd;
        Logic resEQ;
        Logic resLT;
        Logic resLE;
        Logic resMax;
        Logic resMin;
    };

    void proc_comb();

 public:
    InPort i_clk;
    InPort i_nrst;
    InPort i_ena;
    InPort i_add;
    InPort i_sub;
    InPort i_eq;
    InPort i_lt;
    InPort i_le;
    InPort i_max;
    InPort i_min;
    InPort i_a;
    InPort i_b;
    OutPort o_res;
    OutPort o_illegal_op;
    OutPort o_overflow;
    OutPort o_valid;
    OutPort o_busy;

 protected:
    RegSignal busy;
    RegSignal ena;
    RegSignal a;
    RegSignal b;
    RegSignal result;
    RegSignal illegal_op;
    RegSignal overflow;
    RegSignal add;
    RegSignal sub;
    RegSignal eq;
    RegSignal lt;
    RegSignal le;
    RegSignal max;
    RegSignal min;
    RegSignal flMore;
    RegSignal flEqual;
    RegSignal flLess;
    RegSignal preShift;
    RegSignal signOpMore;
    RegSignal expMore;
    RegSignal mantMore;
    RegSignal mantLess;
    RegSignal mantLessScale;
    RegSignal mantSum;
    RegSignal lshift;
    RegSignal mantAlign;
    RegSignal expPostScale;
    RegSignal expPostScaleInv;
    RegSignal mantPostScale;

    // process should be intialized last to make all signals available
    CombProcess comb;
};

class fadd_d_file : public FileObject {
 public:
    fadd_d_file(GenObject *parent) : FileObject(parent, "fadd_d"),
    m_(this, "") {}

 private:
    DoubleAdd m_;
};

