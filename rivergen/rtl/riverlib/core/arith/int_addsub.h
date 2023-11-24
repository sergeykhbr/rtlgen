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

class IntAddSub : public ModuleObject {
 public:
    IntAddSub(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            vb_rdata1(this, "vb_rdata1", "RISCV_ARCH"),
            vb_rdata2(this, "vb_rdata2", "RISCV_ARCH"),
            vb_add(this, "vb_add", "RISCV_ARCH"),
            vb_sub(this, "vb_sub", "RISCV_ARCH"),
            vb_res(this, "vb_res", "RISCV_ARCH") {
        }

     public:
        Logic vb_rdata1;
        Logic vb_rdata2;
        Logic vb_add;
        Logic vb_sub;
        Logic vb_res;
    };

    void proc_comb();

 public:
    InPort i_clk;
    InPort i_nrst;
    InPort i_mode;
    InPort i_a1;
    InPort i_a2;
    OutPort o_res;

 protected:
    RegSignal res;

    // process should be intialized last to make all signals available
    CombProcess comb;
};

class int_addsub_file : public FileObject {
 public:
    int_addsub_file(GenObject *parent) : FileObject(parent, "int_addsub"),
    m_(this, "") {}

 private:
    IntAddSub m_;
};

