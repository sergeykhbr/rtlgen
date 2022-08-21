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
#include "tagmemnway.h"

using namespace sysvc;

class DCacheLru : public ModuleObject {
 public:
    DCacheLru(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            vb_cache_line_i_modified(this, "vb_cache_line_i_modified", "DCACHE_LINE_BITS"),
            vb_line_rdata_o_modified(this, "vb_line_rdata_o_modified", "DCACHE_LINE_BITS"),
            vb_line_rdata_o_wstrb(this, "vb_line_rdata_o_wstrb", "DCACHE_BYTES_PER_LINE"),
            v_req_ready(this, "v_req_ready", "1"),
            t_cache_line_i(this, "t_cache_line_i", "DCACHE_LINE_BITS"),
            vb_cached_data(this, "vb_cached_data", "64"),
            vb_uncached_data(this, "vb_uncached_data", "64"),
            v_resp_valid(this, "v_resp_valid", "1"),
            vb_resp_data(this, "vb_resp_data", "64"),
            v_resp_er_load_fault(this, "v_resp_er_load_fault", "1"),
            v_resp_er_store_fault(this, "v_resp_er_store_fault", "1"),
            v_direct_access(this, "v_direct_access", "1"),
            v_invalidate(this, "v_invalidate", "1"),
            v_flush_end(this, "v_flush_end", "1"),
            v_line_cs_read(this, "v_line_cs_read", "1"),
            v_line_cs_write(this, "v_line_cs_write", "1"),
            vb_line_addr(this, "vb_line_addr", "CFG_CPU_ADDR_BITS"),
            vb_line_wdata(this, "vb_line_wdata", "DCACHE_LINE_BITS"),
            vb_line_wstrb(this, "vb_line_wstrb", "DCACHE_BYTES_PER_LINE"),
            vb_req_mask(this, "vb_req_mask", "64"),
            v_line_wflags(this, "v_line_wflags", "DTAG_FL_TOTAL"),
            ridx(this, "ridx", "SUB(CFG_DLOG2_BYTES_PER_LINE,3)"),
            v_req_same_line(this, "v_req_same_line", "1"),
            v_ready_next(this, "v_ready_next", "1"),
            v_req_snoop_ready(this, "v_req_snoop_ready", "1"),
            v_req_snoop_ready_on_wait(this, "v_req_snoop_ready_on_wait", "1"),
            v_resp_snoop_valid(this, "v_resp_snoop_valid", "1"),
            vb_addr_direct_next(this, "vb_addr_direct_next", "CFG_CPU_ADDR_BITS"),
            t_req_type(this, "t_req_type", "MemopType_Total") {
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
        Logic v_resp_er_load_fault;
        Logic v_resp_er_store_fault;
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
        Logic ridx;
        Logic v_req_same_line;
        Logic v_ready_next;
        Logic v_req_snoop_ready;
        Logic v_req_snoop_ready_on_wait;
        Logic v_resp_snoop_valid;
        Logic vb_addr_direct_next;
        Logic t_req_type;
    };

    void proc_comb();

 public:
    DefParamBOOL coherence_ena;
    // io:
    InPort i_clk;
    InPort i_nrst;
    TextLine _ctrl0_;
    InPort i_req_valid;
    InPort i_req_type;
    InPort i_req_addr;
    InPort i_req_wdata;
    InPort i_req_wstrb;
    InPort i_req_size;
    OutPort o_req_ready;
    OutPort o_resp_valid;
    OutPort o_resp_addr;
    OutPort o_resp_data;
    OutPort o_resp_er_addr;
    OutPort o_resp_er_load_fault;
    OutPort o_resp_er_store_fault;
    OutPort o_resp_er_mpu_load;
    OutPort o_resp_er_mpu_store;
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
    InPort i_mem_store_fault;
    TextLine _mpu0;
    OutPort o_mpu_addr;
    InPort i_mpu_flags;
    TextLine _dsnop0_;
    InPort i_req_snoop_valid;
    InPort i_req_snoop_type;
    OutPort o_req_snoop_ready;
    InPort i_req_snoop_addr;
    InPort i_resp_snoop_ready;
    OutPort o_resp_snoop_valid;
    OutPort o_resp_snoop_data;
    OutPort o_resp_snoop_flags;
    TextLine _dbg0_;
    InPort i_flush_address;
    InPort i_flush_valid;
    OutPort o_flush_end;

    ParamI32D abus;
    ParamI32D waybits;
    ParamI32D ibits;
    ParamI32D lnbits;
    ParamI32D flbits;
    ParamI32D snoop;
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
    ParamLogic State_SnoopSetupAddr;
    ParamLogic State_SnoopReadData;
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
    TextLine _snoop1_;
    Signal line_snoop_addr_i;
    Signal line_snoop_ready_o;
    Signal line_snoop_flags_o;

    // registers
    RegSignal req_type;
    RegSignal req_addr;
    RegSignal req_wdata;
    RegSignal req_wstrb;
    RegSignal req_size;
    RegSignal state;
    RegSignal req_mem_valid;
    RegSignal req_mem_type;
    RegSignal req_mem_size;
    RegSignal mem_addr;
    RegSignal mpu_er_store;
    RegSignal mpu_er_load;
    RegSignal load_fault;
    RegSignal write_first;
    RegSignal write_flush;
    RegSignal write_share;
    RegSignal mem_wstrb;
    RegSignal req_flush;
    RegSignal req_flush_all;
    RegSignal req_flush_addr;
    RegSignal req_flush_cnt;
    RegSignal flush_cnt;
    RegSignal cache_line_i;
    RegSignal cache_line_o;
    RegSignal req_snoop_type;
    RegSignal snoop_flags_valid;
    RegSignal snoop_restore_wait_resp;
    RegSignal snoop_restore_write_bus;
    RegSignal req_addr_restore;

    CombProcess comb;
    TagMemNWay mem0;
};

class dcache_lru_file : public FileObject {
 public:
    dcache_lru_file(GenObject *parent) :
        FileObject(parent, "dcache_lru"),
        dcache_(this, "") {}

 private:
    DCacheLru dcache_;
};

