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
#include "../../techmap/mem/ram_cache_bwe_tech.h"
#include "mem/ram.h"

using namespace sysvc;

class TagMem : public ModuleObject {
 public:
    TagMem(GenObject *parent, 
            const char *name,
            const char *gen_abus = "64", 
            const char *gen_ibits = "6", 
            const char *gen_lnbits = "5", 
            const char *gen_flbits = "4",
            const char *gen_snoop = "0");

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            vb_index(this, "vb_index", "ibits"),
            vb_raddr(this, "vb_raddr", "abus"),
            vb_tagi_wdata(this, "vb_tagi_wdata", "TAG_WITH_FLAGS"),
            v_hit(this, "v_hit", "1"),
            vb_snoop_index(this, "vb_snoop_index", "ibits"),
            vb_snoop_tagaddr(this, "vb_snoop_tagaddr", "TAG_BITS"),
            vb_snoop_flags(this, "vb_snoop_flags", "flbits") {
        }

     public:
        Logic vb_index;
        Logic vb_raddr;
        Logic vb_tagi_wdata;
        Logic v_hit;
        Logic vb_snoop_index;
        Logic vb_snoop_tagaddr;
        Logic vb_snoop_flags;
    };

    void proc_comb();

 public:
    TmplParamI32D abus;
    TmplParamI32D ibits;
    TmplParamI32D lnbits;
    TmplParamI32D flbits;
    TmplParamI32D snoop;

    InPort i_clk;
    InPort i_nrst;
    InPort i_addr;
    InPort i_wstrb;
    InPort i_wdata;
    InPort i_wflags;
    OutPort o_raddr;
    OutPort o_rdata;
    OutPort o_rflags;
    OutPort o_hit;
    TextLine _snoop0_;
    InPort i_snoop_addr;
    OutPort o_snoop_flags;

    ParamI32D TAG_BITS;
    ParamI32D TAG_WITH_FLAGS;

    Signal wb_index;
    Signal wb_tago_rdata;
    Signal wb_tagi_wdata;
    Signal w_tagi_we;
    Signal wb_snoop_index;
    Signal wb_snoop_tagaddr;
    Signal wb_tago_snoop_rdata;

    RegSignal tagaddr;
    RegSignal index;
    RegSignal snoop_tagaddr;
    // process should be intialized last to make all signals available
    CombProcess comb;
    // sub-modules
    ram_cache_bwe_tech data0;
    ram tag0;
    ram tagsnoop0;
};

class tagmem_file : public FileObject {
 public:
    tagmem_file(GenObject *parent) :
        FileObject(parent, "tagmem"),
        TagMem_(this, "") {}

 private:
    TagMem TagMem_;
};

