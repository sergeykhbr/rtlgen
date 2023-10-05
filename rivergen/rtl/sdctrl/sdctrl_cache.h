// 
//  Copyright 2023 Sergey Khabarov, sergeykhbr@gmail.com
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
#include "sdctrl_cfg.h"
#include "../riverlib/cache/tagmem.h"

using namespace sysvc;

class sdctrl_cache : public ModuleObject {
 public:
    sdctrl_cache(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            vb_cache_line_i_modified(this, "vb_cache_line_i_modified", "SDCACHE_LINE_BITS"),
            vb_line_rdata_o_modified(this, "vb_line_rdata_o_modified", "SDCACHE_LINE_BITS"),
            vb_line_rdata_o_wstrb(this, "vb_line_rdata_o_wstrb", "SDCACHE_BYTES_PER_LINE"),
            v_req_ready(this, "v_req_ready", "1"),
            t_cache_line_i(this, "t_cache_line_i", "SDCACHE_LINE_BITS"),
            vb_cached_data(this, "vb_cached_data", "64"),
            vb_uncached_data(this, "vb_uncached_data", "64"),
            v_resp_valid(this, "v_resp_valid", "1"),
            vb_resp_data(this, "vb_resp_data", "64"),
            v_flush_end(this, "v_flush_end", "1"),
            vb_line_wdata(this, "vb_line_wdata", "SDCACHE_LINE_BITS"),
            vb_line_wstrb(this, "vb_line_wstrb", "SDCACHE_BYTES_PER_LINE"),
            vb_req_mask(this, "vb_req_mask", "64"),
            vb_line_wflags(this, "vb_line_wflags", "SDCACHE_FL_TOTAL"),
            ridx(this, "ridx", "SUB(CFG_LOG2_SDCACHE_BYTES_PER_LINE,3)"),
            v_req_same_line(this, "v_req_same_line", "1"),
            v_mem_addr_last(this, "v_mem_addr_last", "1"),
            vb_addr_direct_next(this, "vb_addr_direct_next", "CFG_SDCACHE_ADDR_BITS") {
        }

     public:
        Logic vb_cache_line_i_modified;
        Logic vb_line_rdata_o_modified;
        Logic vb_line_rdata_o_wstrb;
        Logic v_req_ready;
        Logic t_cache_line_i;
        Logic vb_cached_data;
        Logic vb_uncached_data;
        Logic v_resp_valid;
        Logic vb_resp_data;
        Logic v_flush_end;
        Logic vb_line_wdata;
        Logic vb_line_wstrb;
        Logic vb_req_mask;
        Logic vb_line_wflags;
        Logic ridx;
        Logic v_req_same_line;
        Logic v_mem_addr_last;
        Logic vb_addr_direct_next;
    };

    void proc_comb();

 public:
    // io:
    InPort i_clk;
    InPort i_nrst;
    TextLine _ctrl0_;
    InPort i_req_valid;
    InPort i_req_write;
    InPort i_req_addr;
    InPort i_req_wdata;
    InPort i_req_wstrb;
    OutPort o_req_ready;
    OutPort o_resp_valid;
    OutPort o_resp_data;
    OutPort o_resp_err;
    InPort i_resp_ready;
    TextLine _mem0_;
    InPort i_req_mem_ready;
    OutPort o_req_mem_valid;
    OutPort o_req_mem_write;
    OutPort o_req_mem_addr;
    OutPort o_req_mem_data;
    InPort i_mem_data_valid;
    InPort i_mem_data;
    InPort i_mem_fault;
    TextLine _dbg0_;
    InPort i_flush_valid;
    OutPort o_flush_end;

    ParamI32D abus;
    ParamI32D ibits;
    ParamI32D lnbits;
    ParamI32D flbits;
    TextLine _1_;
    TextLine _2_;
    ParamLogic State_Idle;
    ParamLogic State_CheckHit;
    ParamLogic State_TranslateAddress;
    ParamLogic State_WaitGrant;
    ParamLogic State_WaitResp;
    ParamLogic State_CheckResp;
    ParamLogic State_SetupReadAdr;
    ParamLogic State_WriteBus;
    ParamLogic State_FlushAddr;
    ParamLogic State_FlushCheck;
    ParamLogic State_Reset;
    ParamLogic State_ResetWrite;
    TextLine _3_;
    ParamLogic LINE_BYTES_MASK;
    ParamLogic FLUSH_ALL_VALUE;

    Signal line_wdata_i;
    Signal line_wstrb_i;
    Signal line_wflags_i;
    Signal line_raddr_o;
    Signal line_rdata_o;
    Signal line_rflags_o;
    Signal line_hit_o;
    TextLine _snoop1_;
    Signal line_snoop_addr_i;
    Signal line_snoop_flags_o;

    // registers
    RegSignal req_write;
    RegSignal req_addr;
    RegSignal req_wdata;
    RegSignal req_wstrb;
    RegSignal state;
    RegSignal req_mem_valid;
    RegSignal req_mem_write;
    RegSignal mem_addr;
    RegSignal mem_fault;
    RegSignal write_first;
    RegSignal write_flush;
    RegSignal req_flush;
    RegSignal flush_cnt;
    RegSignal line_addr_i;
    RegSignal cache_line_i;
    RegSignal cache_line_o;

    CombProcess comb;
    TagMem mem0;
};

class sdctrl_cache_file : public FileObject {
 public:
    sdctrl_cache_file(GenObject *parent) :
        FileObject(parent, "sdctrl_cache"),
        sdctrl_cache_(this, "") {}

 private:
    sdctrl_cache sdctrl_cache_;
};

