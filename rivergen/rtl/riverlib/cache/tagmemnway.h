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
#include "tagmem.h"
#include "lrunway.h"

using namespace sysvc;

class TagMemNWay : public ModuleObject {
 public:
    TagMemNWay(GenObject *parent, 
            const char *name,
            const char *gen_abus = "64", 
            const char *gen_waybits = "2", 
            const char *gen_ibits = "6", 
            const char *gen_lnbits = "5", 
            const char *gen_flbits = "4",
            const char *gen_snoop = "0");

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            vb_raddr(this, "vb_raddr", "abus"),
            vb_rdata(this, "vb_rdata", "MUL(8,POW2(1,lnbits))"),
            vb_rflags(this, "vb_rflags", "flbits"),
            v_hit(this, "v_hit", "1"),
            vb_hit_idx(this, "vb_hit_idx", "waybits"),
            v_way_we(this, "v_way_we", "1"),
            vb_wstrb(this, "vb_wstrb", "POW2(1,lnbits)"),
            vb_wflags(this, "vb_wflags", "flbits"),
            v_snoop_ready(this, "v_snoop_ready", "1"),
            vb_snoop_flags(this, "vb_snoop_flags", "flbits") {
        }

     public:
        Logic vb_raddr;
        Logic vb_rdata;
        Logic vb_rflags;
        Logic v_hit;
        Logic vb_hit_idx;
        Logic v_way_we;
        Logic vb_wstrb;
        Logic vb_wflags;
        Logic v_snoop_ready;
        Logic vb_snoop_flags;
    };

    void proc_comb();

 public:
    TmplParamI32D abus;
    TmplParamI32D waybits;
    TmplParamI32D ibits;
    TmplParamI32D lnbits;
    TmplParamI32D flbits;
    TmplParamI32D snoop;

    InPort i_clk;
    InPort i_nrst;
    InPort i_direct_access;
    InPort i_invalidate;
    InPort i_re;
    InPort i_we;
    InPort i_addr;
    InPort i_wdata;
    InPort i_wstrb;
    InPort i_wflags;
    OutPort o_raddr;
    OutPort o_rdata;
    OutPort o_rflags;
    OutPort o_hit;
    TextLine _snoop0_;
    InPort i_snoop_addr;
    OutPort o_snoop_ready;
    OutPort o_snoop_flags;

    ParamI32D NWAYS;
    ParamI32D FL_VALID;

    class WayInType : public StructObject {
     public:
        WayInType(GenObject *parent, int idx, const char *comment="")
            : StructObject(parent, "WayInType", "", idx, comment),
            addr(this, "addr", "abus"),
            wstrb(this, "wstrb", "POW2(1,lnbits)"),
            wdata(this, "wdata", "MUL(8,POW2(1,lnbits))"),
            wflags(this, "wflags", "flbits"),
            snoop_addr(this, "snoop_addr", "abus") {
        }
     public:
        Signal addr;
        Signal wstrb;
        Signal wdata;
        Signal wflags;
        Signal snoop_addr;
    } WayInTypeDef_;

    class WayOutType : public StructObject {
     public:
        WayOutType(GenObject *parent, int idx, const char *comment="")
            : StructObject(parent, "WayOutType", "", idx, comment),
            raddr(this, "raddr", "abus"),
            rdata(this, "rdata", "MUL(8,POW2(1,lnbits))"),
            rflags(this, "rflags", "flbits"),
            hit(this, "hit", "1"),
            snoop_flags(this, "snoop_flags", "flbits") {
        }
     public:
        Signal raddr;
        Signal rdata;
        Signal rflags;
        Signal hit;
        Signal snoop_flags;
    } WayOutTypeDef_;

    Signal w_lrui_init;
    Signal wb_lrui_raddr;
    Signal wb_lrui_waddr;
    Signal w_lrui_up;
    Signal w_lrui_down;
    Signal wb_lrui_lru;
    Signal wb_lruo_lru;
    TStructArray<WayInType> way_i;
    TStructArray<WayOutType> way_o;

    RegSignal req_addr;
    RegSignal direct_access;
    RegSignal invalidate;
    RegSignal re;

    // process should be intialized last to make all signals available
    CombProcess comb;
    // sub-modules
    ModuleArray<TagMem> wayx;
    lrunway lru0;
};

class tagmemnway_file : public FileObject {
 public:
    tagmemnway_file(GenObject *parent) :
        FileObject(parent, "tagmemnway"),
        TagMemNWay_(this, "") {}

 private:
    TagMemNWay TagMemNWay_;
};

