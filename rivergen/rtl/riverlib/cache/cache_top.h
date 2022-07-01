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

using namespace sysvc;

class CacheTop : public ModuleObject {
 public:
    CacheTop(GenObject *parent);

 protected:
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
    OutPort o_resp_ctrl_executable;
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
    OutPort o_resp_data_er_mpu_load;
    OutPort o_resp_data_er_mpu_store;
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
    InPort i_flush_address;
    InPort i_flush_valid;
    InPort i_data_flush_address;
    InPort i_data_flush_valid;
    OutPort o_data_flush_end;


    RegSignal test;
};

class cache_top : public FileObject {
 public:
    cache_top(GenObject *parent);

 private:
    CacheTop cache_;
};

