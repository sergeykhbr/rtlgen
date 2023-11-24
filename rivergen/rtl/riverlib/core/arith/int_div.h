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
#include "divstage64.h"

using namespace sysvc;

class IntDiv : public ModuleObject {
 public:
    IntDiv(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            v_invert64(this, "v_invert64", "1"),
            v_invert32(this, "v_invert32", "1"),
            vb_a1(this, "vb_a1", "64"),
            vb_a2(this, "vb_a2", "64"),
            vb_rem(this, "vb_rem", "64"),
            vb_div(this, "vb_div", "64"),
            v_a1_m0(this, "v_a1_m0", "1", "0", "a1 == -0ll"),
            v_a2_m1(this, "v_a2_m1", "1", "0", "a2 == -1ll"),
            v_ena(this, "v_ena", "1"),
            t_divisor(this, "t_divisor", "120") {
        }

     public:
        Logic v_invert64;
        Logic v_invert32;
        Logic vb_a1;
        Logic vb_a2;
        Logic vb_rem;
        Logic vb_div;
        Logic v_a1_m0;
        Logic v_a2_m1;
        Logic1 v_ena;
        Logic t_divisor;
    };

    void proc_comb();

 public:
    InPort i_clk;
    InPort i_nrst;
    InPort i_ena;
    InPort i_unsigned;
    InPort i_rv32;
    InPort i_residual;
    InPort i_a1;
    InPort i_a2;
    OutPort o_res;
    OutPort o_valid;

 protected:
    Signal wb_divisor0_i;
    Signal wb_divisor1_i;
    Signal wb_resid0_o;
    Signal wb_resid1_o;
    Signal wb_bits0_o;
    Signal wb_bits1_o;

    RegSignal rv32;
    RegSignal resid;
    RegSignal invert;
    RegSignal div_on_zero;
    RegSignal overflow;
    RegSignal busy;
    RegSignal ena;
    RegSignal divident_i;
    RegSignal divisor_i;
    RegSignal bits_i;
    RegSignal result;
    RegSignal reference_div;
    RegSignal a1_dbg;
    RegSignal a2_dbg;

    // process should be intialized last to make all signals available
    CombProcess comb;
    // sub-modules
    divstage64 stage0;
    divstage64 stage1;
};

class int_div_file : public FileObject {
 public:
    int_div_file(GenObject *parent) : FileObject(parent, "int_div"),
    m_(this, "") {}

 private:
    IntDiv m_;
};

