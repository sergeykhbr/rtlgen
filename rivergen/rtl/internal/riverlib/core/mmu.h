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
#include "../../mem/ram_mmu_tech.h"

using namespace sysvc;

class Mmu : public ModuleObject {
 public:
    Mmu(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) : CombinationalProcess(parent, "comb"),
            v_core_req_x(this, "v_core_req_x", "1"),
            v_core_req_r(this, "v_core_req_r", "1"),
            v_core_req_w(this, "v_core_req_w", "1"),
            last_page_fault_x(this, "last_page_fault_x", "1"),
            last_page_fault_r(this, "last_page_fault_r", "1"),
            last_page_fault_w(this, "last_page_fault_w", "1"),
            v_core_req_ready(this, "v_core_req_ready", "1"),
            v_core_resp_valid(this, "v_core_resp_valid", "1"),
            vb_core_resp_addr(this, "vb_core_resp_addr", "RISCV_ARCH", "'0", NO_COMMENT),
            vb_core_resp_data(this, "vb_core_resp_data", "64", "'0", NO_COMMENT),
            v_core_resp_load_fault(this, "v_core_resp_load_fault", "1"),
            v_core_resp_store_fault(this, "v_core_resp_store_fault", "1"),
            vb_last_pa_req(this, "vb_last_pa_req", "RISCV_ARCH", "'0", NO_COMMENT),
            v_mem_req_valid(this, "v_mem_req_valid", "1"),
            vb_mem_req_addr(this, "vb_mem_req_addr", "RISCV_ARCH", "'0", NO_COMMENT),
            vb_mem_req_type(this, "vb_mem_req_type", "MemopType_Total", "'0", NO_COMMENT),
            vb_mem_req_wdata(this, "vb_mem_req_wdata", "64", "'0", NO_COMMENT),
            vb_mem_req_wstrb(this, "vb_mem_req_wstrb", "8", "'0", NO_COMMENT),
            vb_mem_req_size(this, "vb_mem_req_size", "2", "'0", NO_COMMENT),
            v_mem_resp_ready(this, "v_mem_resp_ready", "1"),
            v_tlb_wena(this, "v_tlb_wena", "1"),
            vb_tlb_adr(this, "vb_tlb_adr", "CFG_MMU_TLB_AWIDTH", "'0", NO_COMMENT),
            vb_pte_start_va(this, "vb_pte_start_va", "SUB(RISCV_ARCH,12)", "'0", NO_COMMENT),
            vb_resp_ppn(this, "vb_resp_ppn", "SUB(RISCV_ARCH,12)", "'0", NO_COMMENT),
            v_va_ena(this, "v_va_ena", "1"),
            vb_level0_off(this, "vb_level0_off", "12", "'0", NO_COMMENT),
            vb_level1_off(this, "vb_level1_off", "12", "'0", NO_COMMENT),
            vb_level2_off(this, "vb_level2_off", "12", "'0", NO_COMMENT),
            vb_level3_off(this, "vb_level3_off", "12", "'0", NO_COMMENT),
            v_last_valid(this, "v_last_valid", "1"),
            v_tlb_hit(this, "v_tlb_hit", "1"),
            vb_tlb_pa0(this, "vb_tlb_pa0", "RISCV_ARCH", "0", "4 KB page phys address"),
            vb_tlb_pa1(this, "vb_tlb_pa1", "RISCV_ARCH", "0", "8 MB page phys address"),
            vb_tlb_pa2(this, "vb_tlb_pa2", "RISCV_ARCH", "0", "16 GB page phys address"),
            vb_tlb_pa3(this, "vb_tlb_pa3", "RISCV_ARCH", "0", "32 TB page phys address"),
            vb_tlb_pa_hit(this, "vb_tlb_pa_hit", "RISCV_ARCH", "'0", NO_COMMENT),
            t_tlb_wdata(this, "t_tlb_wdata", "CFG_MMU_PTE_DWIDTH", "'0", NO_COMMENT),
            t_idx_lsb(this, "t_idx_lsb", "0", NO_COMMENT) {
            Operation::start(this);
            Mmu *p = static_cast<Mmu *>(parent);
            p->proc_comb();
        }
     public:
        Logic v_core_req_x;
        Logic v_core_req_r;
        Logic v_core_req_w;
        Logic last_page_fault_x;
        Logic last_page_fault_r;
        Logic last_page_fault_w;
        Logic v_core_req_ready;
        Logic v_core_resp_valid;
        Logic vb_core_resp_addr;
        Logic vb_core_resp_data;
        Logic v_core_resp_load_fault;
        Logic v_core_resp_store_fault;
        Logic vb_last_pa_req;
        Logic v_mem_req_valid;
        Logic vb_mem_req_addr;
        Logic vb_mem_req_type;
        Logic vb_mem_req_wdata;
        Logic vb_mem_req_wstrb;
        Logic vb_mem_req_size;
        Logic v_mem_resp_ready;
        Logic v_tlb_wena;
        Logic vb_tlb_adr;
        Logic vb_pte_start_va;
        Logic vb_resp_ppn;
        Logic v_va_ena;
        Logic vb_level0_off;
        Logic vb_level1_off;
        Logic vb_level2_off;
        Logic vb_level3_off;
        Logic v_last_valid;
        Logic v_tlb_hit;
        Logic vb_tlb_pa0;
        Logic vb_tlb_pa1;
        Logic vb_tlb_pa2;
        Logic vb_tlb_pa3;
        Logic vb_tlb_pa_hit;
        Logic t_tlb_wdata;
        I32D t_idx_lsb;
    };

    void proc_comb();

 public:
    InPort i_clk;
    InPort i_nrst;
    OutPort o_core_req_ready;
    InPort i_core_req_valid;
    InPort i_core_req_addr;
    InPort i_core_req_fetch;
    InPort i_core_req_type;
    InPort i_core_req_wdata;
    InPort i_core_req_wstrb;
    InPort i_core_req_size;
    OutPort o_core_resp_valid;
    OutPort o_core_resp_addr;
    OutPort o_core_resp_data;
    OutPort o_core_resp_load_fault;     // Ex.2. Instruction access fault when = 0 and fetch  / Ex.5. Load access fault
    OutPort o_core_resp_store_fault;    // Ex.7. Store/AMO access fault
    OutPort o_core_resp_page_x_fault;   // Ex.12 Instruction page fault
    OutPort o_core_resp_page_r_fault;   // Ex.13 Load page fault
    OutPort o_core_resp_page_w_fault;   // Ex.15 Store/AMO page fault
    InPort i_core_resp_ready;
    InPort i_mem_req_ready;
    OutPort o_mem_req_valid;
    OutPort o_mem_req_addr;
    OutPort o_mem_req_type;
    OutPort o_mem_req_wdata;
    OutPort o_mem_req_wstrb;
    OutPort o_mem_req_size;
    InPort i_mem_resp_valid;
    InPort i_mem_resp_addr;
    InPort i_mem_resp_data;
    InPort i_mem_resp_load_fault;
    InPort i_mem_resp_store_fault;
    OutPort o_mem_resp_ready;
    InPort i_mmu_ena;
    InPort i_mmu_sv39;
    InPort i_mmu_sv48;
    InPort i_mmu_ppn;
    InPort i_mprv;
    InPort i_mxr;
    InPort i_sum;
    InPort i_fence;
    InPort i_fence_addr;

 protected:
    ParamUI32D Idle;
    ParamUI32D WaitRespNoMmu;
    ParamUI32D WaitRespLast;
    ParamUI32D CheckTlb;
    ParamUI32D CacheReq;
    ParamUI32D WaitResp;
    ParamUI32D HandleResp;
    ParamUI32D UpdateTlb;
    ParamUI32D AcceptCore;
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
    RegSignal req_type;
    RegSignal req_wdata;
    RegSignal req_wstrb;
    RegSignal req_size;
    RegSignal req_flush;
    RegSignal last_mmu_ena;
    RegSignal last_va;
    RegSignal last_pa;
    RegSignal last_permission;
    RegSignal last_page_size;
    RegSignal resp_addr;
    RegSignal resp_data;
    RegSignal resp_load_fault;
    RegSignal resp_store_fault;
    RegSignal ex_page_fault;
    RegSignal tlb_hit;
    RegSignal tlb_level;
    RegSignal tlb_page_size;    
    RegSignal tlb_wdata;
    RegSignal tlb_flush_cnt;
    RegSignal tlb_flush_adr;

    // process should be intialized last to make all signals available
    CombProcess comb;

    // Sub-modules:
    ram_mmu_tech tlb;
};

class mmu_file : public FileObject {
 public:
    mmu_file(GenObject *parent) :
        FileObject(parent, "mmu"),
        Mmu_(this, "Mmu") {}

 private:
    Mmu Mmu_;
};

