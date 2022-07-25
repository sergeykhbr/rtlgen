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

class IntMul : public ModuleObject {
 public:
    IntMul(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb") {
        }

     public:
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


    // process should be intialized last to make all signals available
    CombProcess comb;
};

class int_mul_file : public FileObject {
 public:
    int_mul_file(GenObject *parent) : FileObject(parent, "int_mul"),
    m_(this, "") {}

 private:
    IntMul m_;
};

