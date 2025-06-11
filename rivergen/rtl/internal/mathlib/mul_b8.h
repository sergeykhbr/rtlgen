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

using namespace sysvc;

class mul_b8 : public ModuleObject {
 public:
    mul_b8(GenObject *parent, const char *name, const char *comment);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            vb_lvl1_00(this, "vb_lvl1_00", "11", "'0", NO_COMMENT),
            vb_lvl1_01(this, "vb_lvl1_01", "11", "'0", NO_COMMENT),
            vb_lvl1_10(this, "vb_lvl1_10", "11", "'0", NO_COMMENT),
            vb_lvl1_11(this, "vb_lvl1_11", "11", "'0", NO_COMMENT),
            vb_lvl1_20(this, "vb_lvl1_20", "11", "'0", NO_COMMENT),
            vb_lvl1_21(this, "vb_lvl1_21", "11", "'0", NO_COMMENT),
            vb_lvl1_30(this, "vb_lvl1_30", "11", "'0", NO_COMMENT),
            vb_lvl1_31(this, "vb_lvl1_31", "11", "'0", NO_COMMENT) {
        }
     public:
        Logic vb_lvl1_00;
        Logic vb_lvl1_01;
        Logic vb_lvl1_10;
        Logic vb_lvl1_11;
        Logic vb_lvl1_20;
        Logic vb_lvl1_21;
        Logic vb_lvl1_30;
        Logic vb_lvl1_31;
    };

    void proc_comb();

 public:
    // io:
    InPort i_nrst;
    InPort i_clk;
    InPort i_a;
    InPort i_b;
    InPort i_signed;
    OutPort o_res;

    RegSignal ua;
    RegSignal ub;
    RegSignal inv;
    RegSignal lvl1_0;
    RegSignal lvl1_1;
    RegSignal lvl1_2;
    RegSignal lvl1_3;
    RegSignal lvl2_0;
    RegSignal lvl2_1;
    RegSignal lvl3;
    RegSignal res;

    CombProcess comb;
};

class mul_b8_file : public FileObject {
 public:
    mul_b8_file(GenObject *parent) :
        FileObject(parent, "mul_b8"),
        mul_b8_(this, "mul_b8", NO_COMMENT) {}

 private:
    mul_b8 mul_b8_;
};

