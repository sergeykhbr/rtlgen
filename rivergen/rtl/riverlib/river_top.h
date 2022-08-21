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
#include "river_cfg.h"
#include "cache/cache_top.h"
#include "core/proc.h"

using namespace sysvc;

class RiverTop : public ModuleObject {
 public:
    RiverTop(GenObject *parent, const char *name);

 public:
    DefParamUI32D hartid;
    DefParamBOOL fpu_ena;
    DefParamBOOL coherence_ena;
    DefParamBOOL tracer_ena;
    // Ports:
    InPort i_clk;
    InPort i_nrst;
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
    TextLine _DSnoopInterface0_;
    InPort i_req_snoop_valid;
    InPort i_req_snoop_type;
    OutPort o_req_snoop_ready;
    InPort i_req_snoop_addr;
    InPort i_resp_snoop_ready;
    OutPort o_resp_snoop_valid;
    OutPort o_resp_snoop_data;
    OutPort o_resp_snoop_flags;
    TextLine _Interrupts0_;
    InPort i_msip;
    InPort i_mtip;
    InPort i_meip;
    InPort i_seip;
    TextLine _Debug0;
    InPort i_haltreq;
    InPort i_resumereq;
    InPort i_dport_req_valid;
    InPort i_dport_type;
    InPort i_dport_addr;
    InPort i_dport_wdata;
    InPort i_dport_size;
    OutPort o_dport_req_ready;
    InPort i_dport_resp_ready;
    OutPort o_dport_resp_valid;
    OutPort o_dport_resp_error;
    OutPort o_dport_rdata;
    InPort i_progbuf;
    OutPort o_halted;

    // Signals:
    TextLine _ControlPath0_;
    Signal w_req_ctrl_ready;
    Signal w_req_ctrl_valid;
    Signal wb_req_ctrl_addr;
    Signal w_resp_ctrl_valid;
    Signal wb_resp_ctrl_addr;
    Signal wb_resp_ctrl_data;
    Signal w_resp_ctrl_load_fault;
    Signal w_resp_ctrl_executable;
    Signal w_resp_ctrl_ready;
    TextLine _DataPath0_;
    Signal w_req_data_ready;
    Signal w_req_data_valid;
    Signal wb_req_data_type;
    Signal wb_req_data_addr;
    Signal wb_req_data_wdata;
    Signal wb_req_data_wstrb;
    Signal wb_req_data_size;
    Signal w_resp_data_valid;
    Signal wb_resp_data_addr;
    Signal wb_resp_data_data;
    Signal w_resp_data_load_fault;
    Signal w_resp_data_store_fault;
    Signal w_resp_data_er_mpu_load;
    Signal w_resp_data_er_mpu_store;
    Signal wb_resp_data_fault_addr;
    Signal w_resp_data_ready;
    Signal w_mpu_region_we;
    Signal wb_mpu_region_idx;
    Signal wb_mpu_region_addr;
    Signal wb_mpu_region_mask;
    Signal wb_mpu_region_flags;
    Signal wb_flush_address;
    Signal w_flush_valid;
    Signal wb_data_flush_address;
    Signal w_data_flush_valid;
    Signal w_data_flush_end;

    // Sub-module instances:
    Processor proc0;
    CacheTop cache0;
};

class river_top : public FileObject {
 public:
    river_top(GenObject *parent) :
        FileObject(parent, "river_top"),
        top_(this, "") { }

 private:
    RiverTop top_;
};

