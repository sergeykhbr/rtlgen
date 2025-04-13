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
#include "../river_cfg.h"
#include "tagmemnway.h"

using namespace sysvc;

class TagMemCoupled : public ModuleObject {
 public:
    TagMemCoupled(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    virtual bool isVcd() override { return false; }     // disable tracing

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            v_addr_sel(this, "v_addr_sel", "1"),
            v_addr_sel_r(this, "v_addr_sel_r", "1"),
            v_use_overlay(this, "v_use_overlay", "1"),
            v_use_overlay_r(this, "v_use_overlay_r", "1"),
            vb_index(this, "vb_index", "ibits", "'0", NO_COMMENT),
            vb_index_next(this, "vb_index_next", "ibits", "'0", NO_COMMENT),
            vb_addr_next(this, "vb_addr_next", "abus", "'0", NO_COMMENT),
            vb_addr_tag_direct(this, "vb_addr_tag_direct", "abus", "'0", NO_COMMENT),
            vb_addr_tag_next(this, "vb_addr_tag_next", "abus", "'0", NO_COMMENT),
            vb_raddr_tag(this, "vb_raddr_tag", "abus", "'0", NO_COMMENT),
            vb_o_raddr(this, "vb_o_raddr", "abus", "'0", NO_COMMENT),
            vb_o_rdata(this, "vb_o_rdata", "ADD(MUL(8,POW2(1,lnbits)),32)", "'0", NO_COMMENT),
            v_o_hit(this, "v_o_hit", "1"),
            v_o_hit_next(this, "v_o_hit_next", "1"),
            vb_o_rflags(this, "vb_o_rflags", "flbits", "'0", NO_COMMENT) {
        }

     public:
        Logic v_addr_sel;
        Logic v_addr_sel_r;
        Logic v_use_overlay;
        Logic v_use_overlay_r;
        Logic vb_index;
        Logic vb_index_next;
        Logic vb_addr_next;
        Logic vb_addr_tag_direct;
        Logic vb_addr_tag_next;
        Logic vb_raddr_tag;
        Logic vb_o_raddr;
        Logic vb_o_rdata;
        Logic v_o_hit;
        Logic v_o_hit_next;
        Logic vb_o_rflags;
    };

    void proc_comb();

 public:
    TmplParamI32D abus;
    TmplParamI32D waybits;
    TmplParamI32D ibits;
    TmplParamI32D lnbits;
    TmplParamI32D flbits;
    
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
    OutPort o_hit_next;

    ParamI32D LINE_SZ;
    ParamI32D TAG_START;
    ParamI32D EVEN;
    ParamI32D ODD;
    ParamI32D MemTotal;

    class tagmem_in_type : public StructObject {
     public:
        tagmem_in_type(GenObject *parent, const char *name, const char *comment)
            : StructObject(parent, "tagmem_in_type", name, comment),
            direct_access(this, "direct_access", "1"),
            invalidate(this, "invalidate", "1"),
            re(this, "re", "1"),
            we(this, "we", "1"),
            addr(this, "addr", "abus"),
            wdata(this, "wdata", "MUL(8,POW2(1,lnbits))"),
            wstrb(this, "wstrb", "POW2(1,lnbits)"),
            wflags(this, "wflags", "flbits"),
            snoop_addr(this, "snoop_addr", "abus") {
        }
     public:
        Signal direct_access;
        Signal invalidate;
        Signal re;
        Signal we;
        Signal addr;
        Signal wdata;
        Signal wstrb;
        Signal wflags;
        Signal snoop_addr;
    } tagmem_in_type_def_;

    class tagmem_out_type : public StructObject {
     public:
        tagmem_out_type(GenObject *parent, const char *name, const char *comment)
            : StructObject(parent, "tagmem_out_type", name, comment),
            raddr(this, "raddr", "abus"),
            rdata(this, "rdata", "MUL(8,POW2(1,lnbits))"),
            rflags(this, "rflags", "flbits"),
            hit(this, "hit", "1"),
            snoop_ready(this, "snoop_ready", "1"),
            snoop_flags(this, "snoop_flags", "flbits") {
        }
     public:
        Signal raddr;
        Signal rdata;
        Signal rflags;
        Signal hit;
        Signal snoop_ready;
        Signal snoop_flags;
    } tagmem_out_type_def_;

    StructArray<tagmem_in_type> linei;
    StructArray<tagmem_out_type> lineo;
    RegSignal req_addr;

    // process should be intialized last to make all signals available
    CombProcess comb;
    // sub-modules
    ModuleArray<TagMemNWay> memx;
};

class tagmemcoupled_file : public FileObject {
 public:
    tagmemcoupled_file(GenObject *parent) :
        FileObject(parent, "tagmemcoupled"),
        TagMemCoupled_(this, "TagMemCoupled") {}

 private:
    TagMemCoupled TagMemCoupled_;
};

