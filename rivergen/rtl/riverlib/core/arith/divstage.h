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
            ProcObject(parent, "comb") {
        }

     public:
    };

    void proc_comb();

 public:
    InPort i_divident;
    InPort i_divisor;
    OutPort o_resid;
    OutPort o_bits;

 protected:

    // process should be intialized last to make all signals available
    CombProcess comb;
};

class divstage_file : public FileObject {
 public:
    divstage_file(GenObject *parent) : FileObject(parent, "divstage"),
    divstage_(this, "") {}

 private:
    divstage64 divstage_;
};

