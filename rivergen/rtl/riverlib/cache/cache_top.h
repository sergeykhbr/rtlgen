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
#include "icache_lru.h"
#include "dcache_lru.h"
#include "mpu.h"
#include "../core/queue.h"

using namespace sysvc;

class CacheTop : public ModuleObject {
 public:
    CacheTop(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            vb_ctrl_bus(this, "vb_ctrl_bus", "QUEUE_WIDTH"),
            vb_data_bus(this, "vb_data_bus", "QUEUE_WIDTH"),
            vb_queue_bus(this, "vb_queue_bus", "QUEUE_WIDTH"),
            ctrl_path_id(this, "ctrl_path_id"),
            data_path_id(this, "data_path_id"),
            v_queue_we(this, "v_queue_we", "1"),
            v_queue_re(this, "v_queue_re", "1"),
            v_req_mem_path_o(this, "v_req_mem_path_o", "1"),
            vb_req_mem_type_o(this, "vb_req_mem_type_o", "REQ_MEM_TYPE_BITS"),
            vb_req_mem_size_o(this, "vb_req_mem_size_o", "3"),
            vb_req_mem_addr_o(this, "vb_req_mem_addr_o", "CFG_CPU_ADDR_BITS") {
        }

     public:
        Logic vb_ctrl_bus;
        Logic vb_data_bus;
        Logic vb_queue_bus;
        Logic1 ctrl_path_id;
        Logic1 data_path_id;
        Logic v_queue_we;
        Logic v_queue_re;
        Logic v_req_mem_path_o;
        Logic vb_req_mem_type_o;
        Logic vb_req_mem_size_o;
        Logic vb_req_mem_addr_o;
    };

    void proc_comb();

 public:
    DefParamBOOL coherence_ena;

    InPort i_clk;
    InPort i_nrst;
    TextLine _ControlPath0_;
    InPort i_req_ctrl_valid;
    InPort i_req_ctrl_addr;
    OutPort o_req_ctrl_ready;
    OutPort o_resp_ctrl_valid;
    OutPort o_resp_ctrl_addr;
    OutPort o_resp_ctrl_data;
    OutPort o_resp_ctrl_load_fault;
    InPort i_resp_ctrl_ready;
    TextLine _DataPath0_;
    InPort i_req_data_valid;
    InPort i_req_data_type;
    InPort i_req_data_addr;
    InPort i_req_data_wdata;
    InPort i_req_data_wstrb;
    InPort i_req_data_size;
    OutPort o_req_data_ready;
    OutPort o_resp_data_valid;
    OutPort o_resp_data_addr;
    OutPort o_resp_data_data;
    OutPort o_resp_data_fault_addr;
    OutPort o_resp_data_load_fault;
    OutPort o_resp_data_store_fault;
    InPort i_resp_data_ready;
    TextLine _MemInterface0_;
    InPort i_req_mem_ready;
    OutPort o_req_mem_path;
    OutPort o_req_mem_valid;
    OutPort o_req_mem_type;
    OutPort o_req_mem_size;
    OutPort o_req_mem_addr;
    OutPort o_req_mem_strob;
    OutPort o_req_mem_data;
    InPort i_resp_mem_valid;
    InPort i_resp_mem_path;
    InPort i_resp_mem_data;
    InPort i_resp_mem_load_fault;
    InPort i_resp_mem_store_fault;
    TextLine _MpuInterface0_;
    InPort i_mpu_region_we;
    InPort i_mpu_region_idx;
    InPort i_mpu_region_addr;
    InPort i_mpu_region_mask;
    InPort i_mpu_region_flags;
    TextLine _DSnoopInterface0_;
    InPort i_req_snoop_valid;
    InPort i_req_snoop_type;
    OutPort o_req_snoop_ready;
    InPort i_req_snoop_addr;
    InPort i_resp_snoop_ready;
    OutPort o_resp_snoop_valid;
    OutPort o_resp_snoop_data;
    OutPort o_resp_snoop_flags;
    TextLine _DebugSignals0_;
    InPort i_flushi_valid;
    InPort i_flushi_addr;
    InPort i_flushd_valid;
    InPort i_flushd_addr;
    OutPort o_flushd_end;

    ParamI32D DATA_PATH;
    ParamI32D CTRL_PATH;
    ParamI32D QUEUE_WIDTH;

    class CacheOutputType : public StructObject {
     public:
        CacheOutputType(GenObject *parent, const char *name="", const char *comment="")
            : StructObject(parent, "CacheOutputType", name, -1, comment),
            req_mem_valid(this, "req_mem_valid", "1"),
            req_mem_type(this, "req_mem_type", "REQ_MEM_TYPE_BITS"),
            req_mem_size(this, "req_mem_size", "3"),
            req_mem_addr(this, "req_mem_addr", "CFG_CPU_ADDR_BITS"),
            req_mem_strob(this, "req_mem_strob", "DCACHE_BYTES_PER_LINE"),
            req_mem_wdata(this, "req_mem_wdata", "DCACHE_LINE_BITS"),
            mpu_addr(this, "mpu_addr", "CFG_CPU_ADDR_BITS") {
        }
     public:
        Signal req_mem_valid;
        Signal req_mem_type;
        Signal req_mem_size;
        Signal req_mem_addr;
        Signal req_mem_strob;
        Signal req_mem_wdata;
        Signal mpu_addr;
    } CacheOutputTypeDef_;

    CacheOutputType i;
    CacheOutputType d;
    TextLine _iface0_;
    Signal w_ctrl_resp_mem_data_valid;
    Signal wb_ctrl_resp_mem_data;
    Signal w_ctrl_resp_mem_load_fault;
    Signal w_ctrl_req_ready;
    TextLine _iface1_;
    Signal w_data_resp_mem_data_valid;
    Signal wb_data_resp_mem_data;
    Signal w_data_resp_mem_load_fault;
    Signal w_data_req_ready;
    Signal wb_mpu_iflags;
    Signal wb_mpu_dflags;
    TextLine _iface2_;
    Signal queue_re_i;
    Signal queue_we_i;
    Signal queue_wdata_i;
    Signal queue_rdata_o;
    Signal queue_full_o;
    Signal queue_nempty_o;

    CombProcess comb;

    ICacheLru i1;
    DCacheLru d0;
    MPU mpu0;
    Queue queue0;

};

class cache_top_file : public FileObject {
 public:
    cache_top_file(GenObject *parent) :
        FileObject(parent, "cache_top"),
        cache_(this, "") {}

 private:
    CacheTop cache_;
};

