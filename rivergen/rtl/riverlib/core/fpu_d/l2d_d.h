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

using namespace sysvc;

class Long2Double : public ModuleObject {
 public:
    Long2Double(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            v_ena(this, "v_ena"),
            mantAlign(this, "mantAlign", "64"),
            lshift(this, "lshift", "6"),
            expAlign(this, "expAlign", "11"),
            mantEven(this, "mantEven", "1"),
            mant05(this, "mant05", "1"),
            mantOnes(this, "mantOnes", "1"),
            rndBit(this, "rndBit", "1"),
            v_signA(this, "v_signA", "1"),
            vb_A(this, "vb_A", "64"),
            res(this, "res", "64") {
        }

     public:
        Logic1 v_ena;
        Logic mantAlign;
        Logic lshift;
        Logic expAlign;
        Logic mantEven;
        Logic mant05;
        Logic mantOnes;
        Logic rndBit;
        Logic v_signA;
        Logic vb_A;
        Logic res;
    };

    void proc_comb();

 public:
    InPort i_clk;
    InPort i_nrst;
    InPort i_ena;
    InPort i_signed;
    InPort i_w32;
    InPort i_a;
    OutPort o_res;
    OutPort o_valid;
    OutPort o_busy;

 protected:
    RegSignal busy;
    RegSignal ena;
    RegSignal signA;
    RegSignal absA;
    RegSignal result;
    RegSignal op_signed;
    RegSignal mantAlign;
    RegSignal lshift;

    // process should be intialized last to make all signals available
    CombProcess comb;
};

class l2d_d_file : public FileObject {
 public:
    l2d_d_file(GenObject *parent) : FileObject(parent, "l2d_d"),
    m_(this, "") {}

 private:
    Long2Double m_;
};

