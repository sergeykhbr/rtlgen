// 
//  Copyright 2025 Sergey Khabarov, sergeykhbr@gmail.com
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
#include "types_amba.h"

using namespace sysvc;

class axi_dma : public ModuleObject {
 public:
    axi_dma(GenObject *parent, const char *name, const char *depth);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject* parent)
            : CombinationalProcess(parent, "comb"),
            vb_req_mem_bytes_m1(this, "vb_req_mem_bytes_m1", "10", "'0", NO_COMMENT),
            vb_req_addr_inc(this, "vb_req_addr_inc", "CFG_SYSBUS_ADDR_BITS", "'0", NO_COMMENT),
            vb_r_data_swap(this, "vb_r_data_swap", "CFG_SYSBUS_DATA_BITS", "'0", NO_COMMENT),
            vmsto(this, "vmsto", "axi4_master_out_none", NO_COMMENT) {
        }
     public:
        Logic vb_req_mem_bytes_m1;
        Logic vb_req_addr_inc;
        Logic vb_r_data_swap;
        StructVar<types_amba::axi4_master_out_type> vmsto;
    };

    void proc_comb();

public:
    DefParamI32D abits;
    DefParamI32D userbits;
    // Ports:
    InPort i_nrst;
    InPort i_clk;
    OutPort o_req_mem_ready;
    InPort i_req_mem_valid;
    InPort i_req_mem_write;
    InPort i_req_mem_bytes;
    InPort i_req_mem_addr;
    InPort i_req_mem_strob;
    InPort i_req_mem_data;
    InPort i_req_mem_last;
    OutPort o_resp_mem_valid;
    OutPort o_resp_mem_last;
    OutPort o_resp_mem_fault;
    OutPort o_resp_mem_addr;
    OutPort o_resp_mem_data;
    InPort i_resp_mem_ready;
    InStruct<types_amba::axi4_master_in_type> i_msti;
    OutStruct<types_amba::axi4_master_out_type> o_msto;
    OutPort o_dbg_valid;
    OutPort o_dbg_payload;

    ParamLogic state_idle;
    ParamLogic state_ar;
    ParamLogic state_r;
    ParamLogic state_r_wait_accept;
    ParamLogic state_aw;
    ParamLogic state_w;
    ParamLogic state_w_wait_accept;
    ParamLogic state_b;

    // Signals:
    RegSignal state;
    RegSignal ar_valid;
    RegSignal aw_valid;
    RegSignal w_valid;
    RegSignal r_ready;
    RegSignal b_ready;
    RegSignal req_addr;
    RegSignal req_wdata;
    RegSignal req_wstrb;
    RegSignal req_size;
    RegSignal req_len;
    RegSignal req_last;
    RegSignal req_ready;
    RegSignal resp_valid;
    RegSignal resp_last;
    RegSignal resp_addr;
    RegSignal resp_data;
    RegSignal resp_error;
    RegSignal1 user_count;
    RegSignal dbg_valid;
    RegSignal dbg_payload;

    // functions
    // Sub-module instances:
    // process
    CombProcess comb;
};

class axi_dma_file : public FileObject {
 public:
    axi_dma_file(GenObject *parent) :
        FileObject(parent, "axi_dma"),
        axi_dma_(this, "axi_dma", "0") { }

 private:
    axi_dma axi_dma_;
};

