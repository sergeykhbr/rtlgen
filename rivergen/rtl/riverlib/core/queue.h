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
#include "../river_cfg.h"

using namespace sysvc;

class Queue : public ModuleObject {
 public:
    Queue(GenObject *parent, const char *name, const char *gen_abits="6", const char *gen_dbits="128");

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            nempty(this, "nempty", "1"),
            vb_data_o(this, "vb_data_o", "dbits"),
            full(this, "full", "1"),
            show_full(this, "show_full", "1")
        {
            Operation::start(this);
            Queue *p = static_cast<Queue *>(getParent());
            p->proc_comb();
        }

     public:
        Logic nempty;
        Logic vb_data_o;
        Logic full;
        Logic show_full;
    };

    void proc_comb();

 public:
    TmplParamI32D abits;
    TmplParamI32D dbits;

    InPort i_clk;
    InPort i_nrst;
    InPort i_re;
    InPort i_we;
    InPort i_wdata;
    OutPort o_rdata;
    OutPort o_full;
    OutPort o_nempty;

    ParamI32D DEPTH;

    RegSignal wcnt;
    WireArray<Signal> mem;

    // process should be intialized last to make all signals available
    CombProcess comb;
};

class queue_file : public FileObject {
 public:
    queue_file(GenObject *parent) :
        FileObject(parent, "queue"),
        queue_(this, "") {}

 private:
    Queue queue_;
};

