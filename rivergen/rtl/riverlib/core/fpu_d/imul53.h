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

class imul53 : public ModuleObject {
 public:
    imul53(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            v_ena(this, "v_ena"),
            vb_mux(this, "vb_mux", "57", "17"),
            vb_sel(this, "vb_sel", "57"),
            vb_shift(this, "vb_shift", "7"),
            vb_sumInv(this, "vb_sumInv", "105"),
            vb_lshift_p1(this, "vb_lshift_p1", "7"),
            vb_lshift_p2(this, "vb_lshift_p2", "7") {
        }

     public:
        Logic1 v_ena;
        WireArray<Logic> vb_mux;
        Logic vb_sel;
        Logic vb_shift;
        Logic vb_sumInv;
        Logic vb_lshift_p1;
        Logic vb_lshift_p2;
    };

    void proc_comb();

 public:
    InPort i_clk;
    InPort i_nrst;
    InPort i_ena;
    InPort i_a;
    InPort i_b;
    OutPort o_result;
    OutPort o_shift;
    OutPort o_rdy;
    OutPort o_overflow;

 protected:
    RegSignal delay;
    RegSignal shift;
    RegSignal accum_ena;
    RegSignal b;
    RegSignal sum;
    RegSignal overflow;

    // process should be intialized last to make all signals available
    CombProcess comb;
};

class imul53_file : public FileObject {
 public:
    imul53_file(GenObject *parent) : FileObject(parent, "imul53"),
    m_(this, "") {}

 private:
    imul53 m_;
};

