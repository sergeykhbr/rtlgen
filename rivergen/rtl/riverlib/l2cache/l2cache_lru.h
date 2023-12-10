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
#include "../cache/tagmemnway.h"

using namespace sysvc;

class L2CacheLru : public ModuleObject {
 public:
    L2CacheLru(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            vb_cache_line_i_modified(this, "vb_cache_line_i_modified", "L2CACHE_LINE_BITS", "'0", NO_COMMENT),
            vb_line_rdata_o_modified(this, "vb_line_rdata_o_modified", "L2CACHE_LINE_BITS", "'0", NO_COMMENT),
            vb_line_rdata_o_wstrb(this, "vb_line_rdata_o_wstrb", "L2CACHE_BYTES_PER_LINE", "'0", NO_COMMENT),
            v_req_ready(this, "v_req_ready", "1"),
            t_cache_line_i(this, "t_cache_line_i", "L2CACHE_LINE_BITS", "'0", NO_COMMENT),
            vb_cached_data(this, "vb_cached_data", "L1CACHE_LINE_BITS", "'0", NO_COMMENT),
            vb_uncached_data(this, "vb_uncached_data", "L1CACHE_LINE_BITS", "'0", NO_COMMENT),
            v_resp_valid(this, "v_resp_valid", "1"),
            vb_resp_rdata(this, "vb_resp_rdata", "L1CACHE_LINE_BITS", "'0", NO_COMMENT),
            vb_resp_status(this, "vb_resp_status", "L2_REQ_TYPE_BITS", "'0", NO_COMMENT),
            v_direct_access(this, "v_direct_access", "1"),
            v_invalidate(this, "v_invalidate", "1"),
            v_flush_end(this, "v_flush_end", "1"),
            v_line_cs_read(this, "v_line_cs_read", "1"),
            v_line_cs_write(this, "v_line_cs_write", "1", "0", "'cs' should be active when write line and there's no new request"),
            vb_line_addr(this, "vb_line_addr", "CFG_CPU_ADDR_BITS", "'0", NO_COMMENT),
            vb_line_wdata(this, "vb_line_wdata", "L2CACHE_LINE_BITS", "'0", NO_COMMENT),
            vb_line_wstrb(this, "vb_line_wstrb", "L2CACHE_BYTES_PER_LINE", "'0", NO_COMMENT),
            vb_req_mask(this, "vb_req_mask", "L1CACHE_LINE_BITS", "'0", NO_COMMENT),
            v_line_wflags(this, "v_line_wflags", "L2TAG_FL_TOTAL", "'0", NO_COMMENT),
            ridx("0", "ridx", this),
            v_req_same_line(this, "v_req_same_line", "1"),
            v_ready_next(this, "v_ready_next", "1"),
            vb_req_type(this, "vb_req_type", "L2_REQ_TYPE_BITS", "'0", NO_COMMENT),
            vb_addr_direct_next(this, "vb_addr_direct_next", "CFG_CPU_ADDR_BITS", "'0", NO_COMMENT) {
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
         Logic vb_resp_rdata;
         Logic vb_resp_status;
         Logic v_direct_access;
         Logic v_invalidate;
         Logic v_flush_end;
         Logic v_line_cs_read;
         Logic v_line_cs_write;
         Logic vb_line_addr;
         Logic vb_line_wdata;
         Logic vb_line_wstrb;
         Logic vb_req_mask;
         Logic v_line_wflags;
         I32D ridx;
         Logic v_req_same_line;
         Logic v_ready_next;
         Logic vb_req_type;
         Logic vb_addr_direct_next;
    };

    void proc_comb();

 public:
    // io:
    InPort i_clk;
    InPort i_nrst;
    InPort i_req_valid;
    InPort i_req_type;
    InPort i_req_size;
    InPort i_req_prot;
    InPort i_req_addr;
    InPort i_req_wdata;
    InPort i_req_wstrb;
    OutPort o_req_ready;
    OutPort o_resp_valid;
    OutPort o_resp_rdata;
    OutPort o_resp_status;
    TextLine _mem0_;
    InPort i_req_mem_ready;
    OutPort o_req_mem_valid;
    OutPort o_req_mem_type;
    OutPort o_req_mem_size;
    OutPort o_req_mem_prot;
    OutPort o_req_mem_addr;
    OutPort o_req_mem_strob;
    OutPort o_req_mem_data;
    InPort i_mem_data_valid;
    InPort i_mem_data;
    InPort i_mem_data_ack;
    InPort i_mem_load_fault;
    InPort i_mem_store_fault;
    TextLine _flush0_;
    InPort i_flush_address;
    InPort i_flush_valid;
    OutPort o_flush_end;

    ParamI32D abus;
    ParamI32D lnbits;
    ParamI32D flbits;
    ParamI32D ways;
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
    TextLine _snoop1_;
    Signal line_snoop_addr_i;
    Signal line_snoop_ready_o;
    Signal line_snoop_flags_o;

    // registers
    RegSignal req_type;
    RegSignal req_size;
    RegSignal req_prot;
    RegSignal req_addr;
    RegSignal req_wdata;
    RegSignal req_wstrb;
    RegSignal state;
    RegSignal req_mem_valid;
    RegSignal req_mem_type;
    RegSignal mem_addr;
    RegSignal rb_resp;
    RegSignal write_first;
    RegSignal write_flush;
    RegSignal mem_wstrb;
    RegSignal req_flush;
    RegSignal req_flush_all;
    RegSignal req_flush_addr;
    RegSignal req_flush_cnt;
    RegSignal flush_cnt;
    RegSignal cache_line_i;
    RegSignal cache_line_o;

    CombProcess comb;
    TagMemNWay mem0;
};

class l2cache_lru_file : public FileObject {
 public:
    l2cache_lru_file(GenObject *parent) :
        FileObject(parent, "l2cache_lru"),
        l2cache_(this, "L2CacheLru") {}

 private:
    L2CacheLru l2cache_;
};

