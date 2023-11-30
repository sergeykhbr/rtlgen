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

class Double2Long : public ModuleObject {
 public:
    Double2Long(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            v_ena(this, "v_ena"),
            mantA(this, "mantA", "53"),
            expDif_gr(this, "expDif_gr", "1", "0", "greater than 1023 + 63"),
            expDif_lt(this, "expDif_lt", "1", "0", "less than 1023"),
            overflow(this, "overflow", "1"),
            underflow(this, "underflow", "1"),
            expDif(this, "expDif", "12"),
            mantPreScale(this, "mantPreScale", "64"),
            mantPostScale(this, "mantPostScale", "64"),

            expMax(this, "expMax", "11"),
            expShift(this, "expShift", "6"),
            resSign(this, "resSign", "1"),
            resMant(this, "resMant", "64"),
            res(this, "res", "64") {
        }

     public:
        Logic1 v_ena;
        Logic mantA;
        Logic expDif_gr;
        Logic expDif_lt;
        Logic overflow;
        Logic underflow;
        Logic expDif;
        Logic mantPreScale;
        Logic mantPostScale;
        Logic expMax;
        Logic expShift;
        Logic resSign;
        Logic resMant;
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
    OutPort o_overflow;
    OutPort o_underflow;
    OutPort o_valid;
    OutPort o_busy;

 protected:
    RegSignal busy;
    RegSignal ena;
    RegSignal signA;
    RegSignal expA;
    RegSignal mantA;
    RegSignal result;
    RegSignal op_signed;
    RegSignal w32;
    RegSignal mantPostScale;
    RegSignal overflow;
    RegSignal underflow;

    // process should be intialized last to make all signals available
    CombProcess comb;
};

class d2l_d_file : public FileObject {
 public:
    d2l_d_file(GenObject *parent) : FileObject(parent, "d2l_d"),
    m_(this, "") {}

 private:
    Double2Long m_;
};

