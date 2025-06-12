// 
//  Copyright 2025 Sergey Khabarov, sergeykhbr@gmail.com
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
#include "mul_i8.h"

using namespace sysvc;

class mul_4x4_i8 : public ModuleObject {
 public:
    mul_4x4_i8(GenObject *parent, const char *name, const char *comment);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb") {
        }
     public:
    };
    void proc_comb();

 public:
    // io:
    InPort i_nrst;
    InPort i_clk;
    InPort i_signed;
    InPort i_v0;
    InPort i_v1;
    InPort i_v2;
    InPort i_v3;
    InPort i_m;
    OutPort o_res0;
    OutPort o_res1;
    OutPort o_res2;
    OutPort o_res3;

    Signal wb_b00;
    Signal wb_b01;
    Signal wb_b02;
    Signal wb_b03;
    Signal wb_b10;
    Signal wb_b11;
    Signal wb_b12;
    Signal wb_b13;
    Signal wb_b20;
    Signal wb_b21;
    Signal wb_b22;
    Signal wb_b23;
    Signal wb_b30;
    Signal wb_b31;
    Signal wb_b32;
    Signal wb_b33;
    Signal wb_m00;
    Signal wb_m01;
    Signal wb_m02;
    Signal wb_m03;
    Signal wb_m10;
    Signal wb_m11;
    Signal wb_m12;
    Signal wb_m13;
    Signal wb_m20;
    Signal wb_m21;
    Signal wb_m22;
    Signal wb_m23;
    Signal wb_m30;
    Signal wb_m31;
    Signal wb_m32;
    Signal wb_m33;
    RegSignal sum0a;
    RegSignal sum0b;
    RegSignal sum1a;
    RegSignal sum1b;
    RegSignal sum2a;
    RegSignal sum2b;
    RegSignal sum3a;
    RegSignal sum3b;
    RegSignal res0;
    RegSignal res1;
    RegSignal res2;
    RegSignal res3;
    RegSignal sign;

    mul_i8 m00;
    mul_i8 m01;
    mul_i8 m02;
    mul_i8 m03;
    mul_i8 m10;
    mul_i8 m11;
    mul_i8 m12;
    mul_i8 m13;
    mul_i8 m20;
    mul_i8 m21;
    mul_i8 m22;
    mul_i8 m23;
    mul_i8 m30;
    mul_i8 m31;
    mul_i8 m32;
    mul_i8 m33;
    CombProcess comb;
};

class mul_4x4_i8_file : public FileObject {
 public:
    mul_4x4_i8_file(GenObject *parent) :
        FileObject(parent, "mul_4x4_i8"),
        mul_4x4_i8_(this, "mul_4x4_i8", NO_COMMENT) {}

 private:
    mul_4x4_i8 mul_4x4_i8_;
};

