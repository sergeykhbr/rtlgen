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
#include "divstage53.h"

using namespace sysvc;

class idiv53 : public ModuleObject {
 public:
    idiv53(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            v_ena(this, "v_ena", "1", "0", NO_COMMENT),
            vb_muxind(this, "vb_muxind", "56", "'0", NO_COMMENT),
            vb_bits(this, "vb_bits", "105", "'0", NO_COMMENT),
            v_mux_ena_i(this, "v_mux_ena_i", "1") {
        }

     public:
        Logic1 v_ena;
        Logic vb_muxind;
        Logic vb_bits;
        Logic v_mux_ena_i;
    };

    void proc_comb();

 public:
    InPort i_clk;
    InPort i_nrst;
    InPort i_ena;
    InPort i_divident;
    InPort i_divisor;
    OutPort o_result;
    OutPort o_lshift;
    OutPort o_rdy;
    OutPort o_overflow;
    OutPort o_zero_resid;

 protected:
    Signal w_mux_ena_i;
    Signal wb_muxind_i;
    Signal wb_divident_i;
    Signal wb_divisor_i;
    Signal wb_dif_o;
    Signal wb_bits_o;
    Signal wb_muxind_o;
    Signal w_muxind_rdy_o;

    RegSignal delay;
    RegSignal lshift;
    RegSignal lshift_rdy;
    RegSignal divisor;
    RegSignal divident;
    RegSignal bits;
    RegSignal overflow;
    RegSignal zero_resid;

    // process should be intialized last to make all signals available
    CombProcess comb;
    // sumodule
    divstage53 divstage0;
};

class idiv53_file : public FileObject {
 public:
    idiv53_file(GenObject *parent) : FileObject(parent, "idiv53"),
    m_(this, "idiv53") {}

 private:
    idiv53 m_;
};

