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

class IntMul : public ModuleObject {
 public:
    IntMul(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            vb_a1(this, "vb_a1", "RISCV_ARCH", "'0", NO_COMMENT),
            vb_a2(this, "vb_a2", "RISCV_ARCH", "'0", NO_COMMENT),
            wb_mux_lvl0(this, "wb_mux_lvl0", "2", "'0", NO_COMMENT),
            wb_lvl0(this, "wb_lvl0", "66", "32"),
            wb_lvl2(this, "wb_lvl2", "74", "8"),
            wb_lvl4(this, "wb_lvl4", "100", "2"),
            wb_lvl5(this, "wb_lvl5", "128"),
            wb_res32(this, "wb_res32", "128"),
            wb_res(this, "wb_res", "64", "'0", NO_COMMENT),
            vb_a1s(this, "vb_a1s", "64", "'0", NO_COMMENT),
            vb_a2s(this, "vb_a2s", "64", "'0", NO_COMMENT),
            v_a1s_nzero(this, "v_a1s_nzero", "1"),
            v_a2s_nzero(this, "v_a2s_nzero", "1"),
            v_ena(this, "v_ena", "1", "0", NO_COMMENT)
        {
        }

     public:
        Logic vb_a1;
        Logic vb_a2;
        Logic wb_mux_lvl0;
        WireArray<Logic> wb_lvl0;
        WireArray<Logic> wb_lvl2;
        WireArray<Logic> wb_lvl4;
        Logic wb_lvl5;
        Logic wb_res32;
        Logic wb_res;
        Logic vb_a1s;
        Logic vb_a2s;
        Logic v_a1s_nzero;
        Logic v_a2s_nzero;
        Logic1 v_ena;
    };

    void proc_comb();

 public:
    InPort i_clk;
    InPort i_nrst;
    InPort i_ena;
    InPort i_unsigned;
    InPort i_hsu;
    InPort i_high;
    InPort i_rv32;
    InPort i_a1;
    InPort i_a2;
    OutPort o_res;
    OutPort o_valid;

 protected:
     class MulLevel1Type : public RegArray {
      public:
        MulLevel1Type(GenObject *parent, const char *name, const char *width, const char *depth)
            : RegArray(parent, name, width, depth, NO_COMMENT) {}

        virtual bool isVcd() override { return false; }     // disable tracing
    };

     class MulLevel3Type : public RegArray {
      public:
        MulLevel3Type(GenObject *parent, const char *name, const char *width, const char *depth)
            : RegArray(parent, name, width, depth, NO_COMMENT) {}

        virtual bool isVcd() override { return false; }     // disable tracing
    };

    RegSignal busy;
    RegSignal ena;
    RegSignal a1;
    RegSignal a2;
    RegSignal unsign;
    RegSignal high;
    RegSignal rv32;
    RegSignal zero;
    RegSignal inv;
    RegSignal result;
    RegSignal a1_dbg;
    RegSignal a2_dbg;
    RegSignal reference_mul;
    MulLevel1Type lvl1;
    MulLevel3Type lvl3;


    // process should be intialized last to make all signals available
    CombProcess comb;
};

class int_mul_file : public FileObject {
 public:
    int_mul_file(GenObject *parent) : FileObject(parent, "int_mul"),
    m_(this, "IntMul") {}

 private:
    IntMul m_;
};

