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
            v_fetch_req_ready(this, "v_fetch_req_ready", "1"),
            v_fetch_data_valid(this, "v_fetch_data_valid", "1"),
            vb_fetch_data_addr(this, "vb_fetch_data_addr", "CFG_CPU_ADDR_BITS"),
            vb_fetch_data(this, "vb_fetch_data", "64"),
            v_fetch_load_fault(this, "v_fetch_load_fault", "1"),
            v_fetch_executable(this, "v_fetch_executable", "1"),
            v_mem_addr_valid(this, "v_mem_addr_valid", "1"),
            vb_mem_addr(this, "vb_mem_addr", "CFG_CPU_ADDR_BITS"),
            v_mem_resp_ready(this, "v_mem_resp_ready", "1"),
            v_mpu_fault(this, "v_mpu_fault", "1"),
            v_tlb_wena(this, "v_tlb_wena", "1"),
            vb_tlb_adr(this, "vb_tlb_adr", "CFG_MMU_TLB_AWIDTH"),
            vb_pte_start_va(this, "vb_pte_start_va", "SUB(CFG_CPU_ADDR_BITS,12)"),
            vb_pte_base_va(this, "vb_pte_base_va", "SUB(CFG_CPU_ADDR_BITS,12)"),
            vb_level0_off(this, "vb_level0_off", "12"),
            vb_level1_off(this, "vb_level1_off", "12"),
            vb_level2_off(this, "vb_level2_off", "12"),
            vb_level3_off(this, "vb_level3_off", "12"),
            t_req_pa(this, "t_req_pa", "CFG_CPU_ADDR_BITS"),
            t_tlb_wdata(this, "t_tlb_wdata", "CFG_MMU_PTE_DWIDTH") {
            Operation::start(this);
            Mmu *p = static_cast<Mmu *>(parent);
            p->proc_comb();
        }
     public:
        Logic v_fetch_req_ready;
        Logic v_fetch_data_valid;
        Logic vb_fetch_data_addr;
        Logic vb_fetch_data;
        Logic v_fetch_load_fault;
        Logic v_fetch_executable;
        Logic v_mem_addr_valid;
        Logic vb_mem_addr;
        Logic v_mem_resp_ready;
        Logic v_mpu_fault;
        Logic v_tlb_wena;
        Logic vb_tlb_adr;
        Logic vb_pte_start_va;
        Logic vb_pte_base_va;
        Logic vb_level0_off;
        Logic vb_level1_off;
        Logic vb_level2_off;
        Logic vb_level3_off;
        Logic t_req_pa;
        Logic t_tlb_wdata;
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
    OutPort o_fetch_page_fault;
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
    InPort i_mmu_ena;
    InPort i_mmu_ppn;
    InPort i_flush_pipeline;

 protected:
    ParamUI32D Idle;
    ParamUI32D CheckTlb;
    ParamUI32D CacheReq;
    ParamUI32D WaitResp;
    ParamUI32D HandleResp;
    ParamUI32D UpdateTlb;
    ParamUI32D AcceptFetch;
    ParamUI32D FlushTlb;

    ParamI32D PTE_V;
    ParamI32D PTE_R;
    ParamI32D PTE_W;
    ParamI32D PTE_X;
    ParamI32D PTE_U;
    ParamI32D PTE_G;
    ParamI32D PTE_A;
    ParamI32D PTE_D;

    Signal wb_tlb_adr;
    Signal w_tlb_wena;
    Signal wb_tlb_wdata;
    Signal wb_tlb_rdata;

    RegSignal state;
    RegSignal req_x;
    RegSignal req_r;
    RegSignal req_w;
    RegSignal req_pa;
    RegSignal last_va;
    RegSignal last_pa;
    RegSignal resp_addr;
    RegSignal resp_data;
    RegSignal pte_permission;
    RegSignal ex_load_fault;
    RegSignal ex_mpu_executable;
    RegSignal ex_page_fault;
    RegSignal tlb_hit;
    RegSignal tlb_level;
    RegSignal tlb_wdata;
    RegSignal tlb_flush_cnt;
    RegSignal tlb_flush_adr;

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

