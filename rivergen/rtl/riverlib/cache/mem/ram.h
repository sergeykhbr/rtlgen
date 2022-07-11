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

class ram : public ModuleObject {
 public:
    ram(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb") {
            Operation::start(this);
            ram *p = static_cast<ram *>(getParent());
            p->proc_comb();
        }
    };

    void proc_comb();

 public:
    TmplParamI32D abits;
    TmplParamI32D dbits;

    InPort i_clk;
    InPort i_adr;
    InPort i_wena;
    InPort i_wdata;
    OutPort o_rdata;

    ParamI32D DEPTH;

    RegSignal adr;
    WireArray<Signal> mem;

    // process should be intialized last to make all signals available
    CombProcess comb;
};

class ram_file : public FileObject {
 public:
    ram_file(GenObject *parent) :
        FileObject(parent, "ram"),
        ram_(this, "") {}

 private:
    ram ram_;
};

