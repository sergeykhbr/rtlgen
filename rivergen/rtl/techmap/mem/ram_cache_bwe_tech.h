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
#include "ram_tech.h"

using namespace sysvc;

class ram_cache_bwe_tech : public ModuleObject {
 public:
    ram_cache_bwe_tech(GenObject *parent, const char *name, const char *depth,
            const char *gen_abits="6", const char *gen_dbits="128");

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            vb_rdata(this, "vb_rdata", "dbits") {
        }

        Logic vb_rdata;
    };

    void proc_comb();

 public:
    TmplParamI32D abits;
    TmplParamI32D dbits;

    InPort i_clk;
    InPort i_addr;
    InPort i_wena;
    InPort i_wdata;
    OutPort o_rdata;

    WireArray<Signal> wb_we;
    WireArray<Signal> wb_wdata;
    WireArray<Signal> wb_rdata;

    // process should be intialized last to make all signals available
    CombProcess comb;
    ram_tech rx;
};

class ram_cache_bwe_tech_file : public FileObject {
 public:
    ram_cache_bwe_tech_file(GenObject *parent) :
        FileObject(parent, "ram_cache_bwe_tech"),
        ram_cache_bwe_tech_(this, "ram_cache_bwe_tech", "0") {}

 private:
    ram_cache_bwe_tech ram_cache_bwe_tech_;
};

