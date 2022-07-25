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

class AluLogic : public ModuleObject {
 public:
    AluLogic(GenObject *parent, const char *name);

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
    InPort i_mode;
    InPort i_a1;
    InPort i_a2;
    OutPort o_res;

 protected:
    RegSignal res;

    // process should be intialized last to make all signals available
    CombProcess comb;
};

class alu_logic_file : public FileObject {
 public:
    alu_logic_file(GenObject *parent) : FileObject(parent, "alu_logic"),
    alu_(this, "") {}

 private:
    AluLogic alu_;
};

