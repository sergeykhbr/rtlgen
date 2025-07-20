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

class fmul_istage : public ModuleObject {
 public:
    fmul_istage(GenObject *parent, const char *name, const char *comment);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            vb_mux(this, "vb_mux", "ADD(ibits,4)", "17", NO_COMMENT),
            vb_res(this, "vb_res", "ADD(ibits,4)", "'0", NO_COMMENT),
            vb_tzd(this, "vb_shift", "3", "'0", "Trailing Zero Detector") {
        }

     public:
        WireArray<Logic> vb_mux;
        Logic vb_res;
        Logic vb_tzd;
    };

    void proc_comb();

 public:
    TmplParamI32D ibits;
    InPort i_clk;
    InPort i_nrst;
    InPort i_a;
    InPort i_m;
    InPort i_carry;
    OutPort o_result;
    OutPort o_carry;
    OutPort o_shift;

 protected:
    RegSignal m_carry;
    RegSignal sel;
    RegSignal res;
    RegSignal shift;

    CombProcess comb;
};

class fmul_istage_file : public FileObject {
 public:
    fmul_istage_file(GenObject *parent) : FileObject(parent, "fmul_istage"),
    fmul_istage_(this, "fmul_istage", NO_COMMENT) {}

 private:
    fmul_istage fmul_istage_;
};
