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

class lrunway : public ModuleObject {
 public:
    lrunway(GenObject *parent, 
            const char *name,
            const char *gen_abits="6",
            const char *gen_waybits="2");

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            nempty(this, "nempty", "1"),
            vb_data_o(this, "vb_data_o", "dbits"),
            full(this, "full", "1"),
            show_full(this, "show_full", "1")
        {
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
    TmplParamI32D waybits;

    InPort i_clk;
    InPort i_init;
    InPort i_raddr;
    InPort i_waddr;
    InPort i_up;
    InPort i_down;
    InPort i_lru;
    OutPort o_lru;

    ParamI32D LINES_TOTAL;
    ParamI32D WAYS_TOTAL;
    ParamI32D LINE_WIDTH;


    RegSignal radr;
    WireArray<Signal> tbl;

    // process should be intialized last to make all signals available
    CombProcess comb;
};

class lrunway_file : public FileObject {
 public:
    lrunway_file(GenObject *parent) :
        FileObject(parent, "lrunway"),
        lrunway_(this, "") {}

 private:
    lrunway lrunway_;
};

