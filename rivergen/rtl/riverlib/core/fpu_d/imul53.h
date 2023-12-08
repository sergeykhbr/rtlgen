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
#include "zeroenc.h"

using namespace sysvc;

class imul53 : public ModuleObject {
 public:
    imul53(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            v_ena(this, "v_ena", "1", "0", NO_COMMENT),
            vb_mux(this, "vb_mux", "57", "17"),
            vb_sel(this, "vb_sel", "57"),
            vb_shift(this, "vb_shift", "7"),
            vb_sumInv(this, "vb_sumInv", "105") {
        }

     public:
        Logic1 v_ena;
        WireArray<Logic> vb_mux;
        Logic vb_sel;
        Logic vb_shift;
        Logic vb_sumInv;
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
    Signal wb_sumInv;
    Signal wb_lshift;

    RegSignal delay;
    RegSignal shift;
    RegSignal accum_ena;
    RegSignal b;
    RegSignal sum;
    RegSignal overflow;

    // process should be intialized last to make all signals available
    CombProcess comb;

    zeroenc enc0;
};

class imul53_file : public FileObject {
 public:
    imul53_file(GenObject *parent) : FileObject(parent, "imul53"),
    m_(this, "imul53") {}

 private:
    imul53 m_;
};

