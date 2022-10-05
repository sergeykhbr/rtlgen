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

class InstrFetch : public ModuleObject {
 public:
    InstrFetch(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) : ProcObject(parent, "comb") {
            Operation::start(this);
            InstrFetch *p = static_cast<InstrFetch *>(parent);
            p->proc_comb();
        }
    };

    void proc_comb();

 public:
    InPort i_clk;
    InPort i_nrst;
    InPort i_bp_valid;
    InPort i_bp_pc;
    OutPort o_requested_pc;
    OutPort o_fetching_pc;
    InPort i_mem_req_ready;
    OutPort o_mem_addr_valid;
    OutPort o_mem_addr;
    InPort i_mem_data_valid;
    InPort i_mem_data_addr;
    InPort i_mem_data;
    InPort i_mem_load_fault;
    InPort i_mem_page_fault_x;
    OutPort o_mem_resp_ready;
    InPort i_flush_pipeline;
    InPort i_progbuf_ena;
    InPort i_progbuf_pc;
    InPort i_progbuf_instr;
    OutPort o_instr_load_fault;
    OutPort o_instr_page_fault_x;
    OutPort o_pc;
    OutPort o_instr;

 protected:
    ParamUI32D Idle;
    ParamUI32D WaitReqAccept;
    ParamUI32D WaitResp;

    RegSignal state;
    RegSignal req_valid;
    RegSignal resp_ready;
    RegSignal req_addr;
    RegSignal mem_resp_shadow;
    RegSignal pc;
    RegSignal instr;
    RegSignal instr_load_fault;
    RegSignal instr_page_fault_x;
    RegSignal progbuf_ena;

    // process should be intialized last to make all signals available
    CombProcess comb;
};

class fetch_file : public FileObject {
 public:
    fetch_file(GenObject *parent) :
        FileObject(parent, "fetch"),
        InstrFetch_(this, "") {}

 private:
    InstrFetch InstrFetch_;
};

