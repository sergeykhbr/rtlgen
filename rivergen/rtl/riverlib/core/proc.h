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

class Processor : public ModuleObject {
 public:
    Processor(GenObject *parent);

 protected:
    DefParam hartid;
    DefParam fpu_ena;
    DefParam tracer_ena;

    InPort i_clk;
    InPort i_nrst;
    TextLine _ControlPath0_;
    InPort i_req_ctrl_ready;
    OutPort o_req_ctrl_valid;
    OutPort o_req_ctrl_addr;
    InPort i_resp_ctrl_valid;
    InPort i_resp_ctrl_addr;
    InPort i_resp_ctrl_data;
    InPort i_resp_ctrl_load_fault;
    InPort i_resp_ctrl_executable;
    OutPort o_resp_ctrl_ready;
    TextLine _DataPath0_;
    InPort i_req_data_ready;
    OutPort o_req_data_valid;
    OutPort o_req_data_type;
    OutPort o_req_data_addr;
    OutPort o_req_data_wdata;
    OutPort o_req_data_wstrb;
    OutPort o_req_data_size;
    InPort i_resp_data_valid;
    InPort i_resp_data_addr;
    InPort i_resp_data_data;
    InPort i_resp_data_fault_addr;
    InPort i_resp_data_load_fault;
    InPort i_resp_data_store_fault;
    InPort i_resp_data_er_mpu_load;
    InPort i_resp_data_er_mpu_store;
    OutPort o_resp_data_ready;
    TextLine _Interrupts0_;
    InPort i_msip;
    InPort i_mtip;
    InPort i_meip;
    InPort i_seip;
    TextLine _MpuInterface0_;
    OutPort o_mpu_region_we;
    OutPort o_mpu_region_idx;
    OutPort o_mpu_region_addr;
    OutPort o_mpu_region_mask;
    OutPort o_mpu_region_flags;
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
    TextLine _CacheDbg0_;
    OutPort o_flush_address;
    OutPort o_flush_valid;
    OutPort o_data_flush_address;
    OutPort o_data_flush_valid;
    InPort i_data_flush_end;
};

class proc : public FileObject {
 public:
    proc(GenObject *parent);

 private:
    Processor proc_;
};

