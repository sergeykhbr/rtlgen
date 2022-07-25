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

class divstage64 : public ModuleObject {
 public:
    divstage64(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            vb_bits(this, "vb_bits", "4") {
        }

     public:
        Logic vb_bits;
    };

    void proc_comb();

 public:
    InPort i_divident;
    InPort i_divisor;
    OutPort o_resid;
    OutPort o_bits;

 protected:
    WireArray<Signal> wb_thresh;
    Signal wb_dif;
    Signal wb_divx1;
    Signal wb_divx2;
    Signal wb_divx3;
    Signal wb_divx4;
    Signal wb_divx5;
    Signal wb_divx6;
    Signal wb_divx7;
    Signal wb_divx8;
    Signal wb_divx9;
    Signal wb_divx10;
    Signal wb_divx11;
    Signal wb_divx12;
    Signal wb_divx13;
    Signal wb_divx14;
    Signal wb_divx15;
    Signal wb_divx16;
    Signal wb_divident;
    Signal wb_divisor;

    // process should be intialized last to make all signals available
    CombProcess comb;
};

class divstage64_file : public FileObject {
 public:
    divstage64_file(GenObject *parent) : FileObject(parent, "divstage64"),
    divstage64_(this, "") {}

 private:
    divstage64 divstage64_;
};

