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
    ram_cache_bwe_tech(GenObject *parent, const char *name, const char *comment);

    virtual bool isVcd() override { return false; }     // disable tracing
    virtual bool isMemory() override { return true; }
    virtual int  getMemoryAddrWidth() override { return static_cast<int>(abits.getValue()); }
    virtual int  getMemoryDataWidth() override { return static_cast<int>(dbits.getValue()); }

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            vb_rdata(this, "vb_rdata", "dbits") {
        }

        Logic vb_rdata;
    };

    void proc_comb();

 public:
    TmplParamI32D abits;
    TmplParamI32D dbits;
    //ParamI32D bytes_total;

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
    ModuleArray<ram_tech> rx;
};

class ram_cache_bwe_tech_file : public FileObject {
 public:
    ram_cache_bwe_tech_file(GenObject *parent) :
        FileObject(parent, "ram_cache_bwe_tech"),
        ram_cache_bwe_tech_(this, "ram_cache_bwe_tech", NO_COMMENT) {}

 private:
    ram_cache_bwe_tech ram_cache_bwe_tech_;
};

