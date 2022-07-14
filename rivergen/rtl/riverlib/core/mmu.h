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
#include "mmu_tlb.h"

using namespace sysvc;

class Mmu : public ModuleObject {
 public:
    Mmu(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) : ProcObject(parent, "comb"),
            v_sv39(this, "v_sv39", "1"),
            v_sv48(this, "v_sv48", "1"),
            v_mmu_ena(this, "v_mmu_ena", "1"),
            v_fetch_req_ready(this, "v_fetch_req_ready", "1"),
            v_fetch_data_valid(this, "v_fetch_data_valid", "1"),
            vb_fetch_data_addr(this, "vb_fetch_data_addr", "CFG_CPU_ADDR_BITS"),
            vb_fetch_data(this, "vb_fetch_data", "64"),
            v_fetch_load_fault(this, "v_fetch_load_fault", "1"),
            v_fetch_executable(this, "v_fetch_executable", "1"),
            v_mem_addr_valid(this, "v_mem_addr_valid", "1"),
            vb_mem_addr(this, "vb_mem_addr", "CFG_CPU_ADDR_BITS"),
            v_mem_resp_ready(this, "v_mem_resp_ready", "1") {
            Operation::start(this);
            Mmu *p = static_cast<Mmu *>(parent);
            p->proc_comb();
        }
     public:
        Logic v_sv39;
        Logic v_sv48;
        Logic v_mmu_ena;
        Logic v_fetch_req_ready;
        Logic v_fetch_data_valid;
        Logic vb_fetch_data_addr;
        Logic vb_fetch_data;
        Logic v_fetch_load_fault;
        Logic v_fetch_executable;
        Logic v_mem_addr_valid;
        Logic vb_mem_addr;
        Logic v_mem_resp_ready;
    };

    void proc_comb();

 public:
    InPort i_clk;
    InPort i_nrst;
    OutPort o_fetch_req_ready;
    InPort i_fetch_addr_valid;
    InPort i_fetch_addr;
    OutPort o_fetch_data_valid;
    OutPort o_fetch_data_addr;
    OutPort o_fetch_data;
    OutPort o_fetch_load_fault;
    OutPort o_fetch_executable;
    InPort i_fetch_resp_ready;
    InPort i_mem_req_ready;
    OutPort o_mem_addr_valid;
    OutPort o_mem_addr;
    InPort i_mem_data_valid;
    InPort i_mem_data_addr;
    InPort i_mem_data;
    InPort i_mem_load_fault;
    InPort i_mem_executable;
    OutPort o_mem_resp_ready;
    InPort i_prv;
    InPort i_satp;
    InPort i_flush_pipeline;

 protected:
    ParamUI32D Idle;
    ParamUI32D CheckTlb;
    ParamUI32D WaitReqAccept;
    ParamUI32D WaitResp;

    Signal wb_tlb_adr;
    Signal w_tlb_wena;
    Signal wb_tlb_wdata;
    Signal wb_tlb_rdata;

    RegSignal state;
    RegSignal req_va;
    RegSignal last_va;
    RegSignal last_pa;
    RegSignal req_valid;
    RegSignal resp_ready;
    RegSignal req_addr;
    RegSignal mem_resp_shadow;
    RegSignal pc;
    RegSignal instr;
    RegSignal instr_load_fault;
    RegSignal instr_executable;

    // process should be intialized last to make all signals available
    CombProcess comb;

    // Sub-modules:
    MmuTlb tlb;
};

class mmu_file : public FileObject {
 public:
    mmu_file(GenObject *parent) :
        FileObject(parent, "mmu"),
        Mmu_(this, "") {}

 private:
    Mmu Mmu_;
};

