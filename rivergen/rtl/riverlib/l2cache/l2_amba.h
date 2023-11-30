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
#include "../types_river.h"

using namespace sysvc;

class L2Amba : public ModuleObject {
 public:
    L2Amba(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            v_req_mem_ready(this, "v_req_mem_ready", "1"),
            v_resp_mem_valid(this, "v_resp_mem_valid", "1"),
            v_resp_mem_ack(this, "v_resp_mem_ack", "1"),
            v_mem_er_load_fault(this, "v_mem_er_load_fault", "1"),
            v_mem_er_store_fault(this, "v_mem_er_store_fault", "1"),
            v_next_ready(this, "v_next_ready", "1"),
            vmsto(this, "vmsto", NO_COMMENT) {
        }

     public:
        Logic v_req_mem_ready;
        Logic v_resp_mem_valid;
        Logic v_resp_mem_ack;
        Logic v_mem_er_load_fault;
        Logic v_mem_er_store_fault;
        Logic v_next_ready;
        types_river::axi4_l2_out_type vmsto;
    };

    void proc_comb();

 public:
    // io:
    InPort i_clk;
    InPort i_nrst;
    OutPort o_req_ready;
    InPort i_req_valid;
    InPort i_req_type;
    InPort i_req_size;
    InPort i_req_prot;
    InPort i_req_addr;
    InPort i_req_strob;
    InPort i_req_data;
    OutPort o_resp_data;
    OutPort o_resp_valid;
    OutPort o_resp_ack;
    OutPort o_resp_load_fault;
    OutPort o_resp_store_fault;
    InStruct<types_river::axi4_l2_in_type> i_msti;
    OutStruct<types_river::axi4_l2_out_type> o_msto;

    ParamLogic idle;
    ParamLogic reading;
    ParamLogic writing;
    ParamLogic wack;

    RegSignal state;

    CombProcess comb;
};

class l2_amba_file : public FileObject {
 public:
    l2_amba_file(GenObject *parent) :
        FileObject(parent, "l2_amba"),
        l2_amba_(this, "") {}

 private:
    L2Amba l2_amba_;
};

