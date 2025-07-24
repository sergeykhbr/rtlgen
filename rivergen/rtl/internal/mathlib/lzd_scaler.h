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

class lzd_scaler : public ModuleObject {
 public:
    lzd_scaler(GenObject *parent, const char *name, const char *comment);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            vb_lzd(this, "vb_lzd", "MUL(8,lzd_chunks)", "'0", NO_COMMENT),
            vb_lzd_noscaling(this, "vb_lzd_noscaling", "lzd_chunks", "'1", NO_COMMENT) {
        }

     public:
        Logic vb_lzd;
        Logic vb_lzd_noscaling;
    };

    void proc_comb();

 public:
    TmplParamI32D ibits;
    TmplParamI32D shiftbits;
    // IOs
    InPort i_clk;
    InPort i_nrst;
    InPort i_m;
    InPort i_noscale;
    OutPort o_scaled;
    OutPort o_scaled_factor;

    ParamI32D lzd_chunks;
    ParamI32D lzd_bits;

 protected:
    WireArray<RegSignal> lzd_factor;
    WireArray<RegSignal> lzd_m;
    RegSignal lzd_noscale;
    RegSignal scaled_factor;
    RegSignal scaled;

    // process should be intialized last to make all signals available
    CombProcess comb;
    // submodules
};

class lzd_scaler_file : public FileObject {
 public:
    lzd_scaler_file(GenObject *parent) : FileObject(parent, "lzd_scaler"),
    lzd_scaler_(this, "lzd_scaler", "Leading Zero Detector with scaling. 2 Cycles latency.") {}

 private:
    lzd_scaler lzd_scaler_;
};

