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
#include "tagmemcoupled.h"

using namespace sysvc;

class ICacheLru : public ModuleObject {
 public:
    ICacheLru(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            t_cache_line_i(this, "t_cache_line_i", "ICACHE_LINE_BITS"),
            v_req_ready(this, "v_req_ready", "1"),
            v_resp_valid(this, "v_resp_valid", "1"),
            vb_cached_data(this, "vb_cached_data", "64"),
            vb_uncached_data(this, "vb_uncached_data", "64"),
            vb_resp_data(this, "vb_resp_data", "64"),
            v_resp_er_load_fault(this, "v_resp_er_load_fault", "1"),
            v_direct_access(this, "v_direct_access", "1"),
            v_invalidate(this, "v_invalidate", "1"),
            v_line_cs_read(this, "v_line_cs_read", "1"),
            v_line_cs_write(this, "v_line_cs_write", "1"),
            vb_line_addr(this, "vb_line_addr", "CFG_CPU_ADDR_BITS"),
            vb_line_wdata(this, "vb_line_wdata", "ICACHE_LINE_BITS"),
            vb_line_wstrb(this, "vb_line_wstrb", "ICACHE_BYTES_PER_LINE"),
            v_line_wflags(this, "v_line_wflags", "ITAG_FL_TOTAL"),
            sel_cached("0", "sel_cached", this),
            sel_uncached("0", "sel_uncached", this),
            v_ready_next(this, "v_ready_next", "1"),
            vb_addr_direct_next(this, "vb_addr_direct_next", "CFG_CPU_ADDR_BITS") {
        }

     public:
        Logic t_cache_line_i;
        Logic v_req_ready;
        Logic v_resp_valid;
        Logic vb_cached_data;
        Logic vb_uncached_data;
        Logic vb_resp_data;
        Logic v_resp_er_load_fault;
        Logic v_direct_access;
        Logic v_invalidate;
        Logic v_line_cs_read;
        Logic v_line_cs_write;
        Logic vb_line_addr;
        Logic vb_line_wdata;
        Logic vb_line_wstrb;
        Logic v_line_wflags;
        I32D sel_cached;
        I32D sel_uncached;
        Logic v_ready_next;
        Logic vb_addr_direct_next;
    };

    void proc_comb();

 public:
    InPort i_clk;
    InPort i_nrst;
    TextLine _ctrl0_;
    InPort i_req_valid;
    InPort i_req_addr;
    OutPort o_req_ready;
    OutPort o_resp_valid;
    OutPort o_resp_addr;
    OutPort o_resp_data;
    OutPort o_resp_load_fault;
    OutPort o_resp_executable;
    OutPort o_resp_writable;
    OutPort o_resp_readable;
    InPort i_resp_ready;
    TextLine _mem0_;
    InPort i_req_mem_ready;
    OutPort o_req_mem_valid;
    OutPort o_req_mem_type;
    OutPort o_req_mem_size;
    OutPort o_req_mem_addr;
    OutPort o_req_mem_strob;
    OutPort o_req_mem_data;
    InPort i_mem_data_valid;
    InPort i_mem_data;
    InPort i_mem_load_fault;
    TextLine _mpu0;
    OutPort o_mpu_addr;
    InPort i_mpu_flags;
    TextLine _flush0_;
    InPort i_flush_address;
    InPort i_flush_valid;

    ParamI32D abus;
    ParamI32D waybits;
    ParamI32D ibits;
    ParamI32D lnbits;
    ParamI32D flbits;
    ParamLogic State_Idle;
    ParamLogic State_CheckHit;
    ParamLogic State_TranslateAddress;
    ParamLogic State_WaitGrant;
    ParamLogic State_WaitResp;
    ParamLogic State_CheckResp;
    ParamLogic State_SetupReadAdr;
    ParamLogic State_FlushAddr;
    ParamLogic State_FlushCheck;
    ParamLogic State_Reset;
    ParamLogic State_ResetWrite;
    ParamLogic LINE_BYTES_MASK;

    Signal line_direct_access_i;
    Signal line_invalidate_i;
    Signal line_re_i;
    Signal line_we_i;
    Signal line_addr_i;
    Signal line_wdata_i;
    Signal line_wstrb_i;
    Signal line_wflags_i;
    Signal line_raddr_o;
    Signal line_rdata_o;
    Signal line_rflags_o;
    Signal line_hit_o;
    Signal line_hit_next_o;

    // registers
    RegSignal req_addr;
    RegSignal req_addr_next;
    RegSignal write_addr;
    RegSignal state;
    RegSignal req_mem_valid;
    RegSignal mem_addr;
    RegSignal req_mem_type;
    RegSignal req_mem_size;
    RegSignal executable;
    RegSignal load_fault;
    RegSignal req_flush;
    RegSignal req_flush_all;
    RegSignal req_flush_addr;
    RegSignal req_flush_cnt;
    RegSignal flush_cnt;
    RegSignal cache_line_i;

    CombProcess comb;
    TagMemCoupled mem0;
};

class icache_lru_file : public FileObject {
 public:
    icache_lru_file(GenObject *parent) :
        FileObject(parent, "icache_lru"),
        icache_(this, "") {}

 private:
    ICacheLru icache_;
};

