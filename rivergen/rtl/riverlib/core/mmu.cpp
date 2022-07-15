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

#include "mmu.h"

Mmu::Mmu(GenObject *parent, const char *name) :
    ModuleObject(parent, "Mmu", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    // fetch <-> mmu interface
    o_fetch_req_ready(this, "o_fetch_req_ready", "1"),
    i_fetch_addr_valid(this, "i_fetch_addr_valid", "1"),
    i_fetch_addr(this, "i_fetch_addr", "CFG_CPU_ADDR_BITS"),
    o_fetch_data_valid(this, "o_fetch_data_valid", "1"),
    o_fetch_data_addr(this, "o_fetch_data_addr", "CFG_CPU_ADDR_BITS"),
    o_fetch_data(this, "o_fetch_data", "64"),
    o_fetch_load_fault(this, "o_fetch_load_fault", "1"),
    o_fetch_executable(this, "o_fetch_executable", "1"),
    o_fetch_page_fault(this, "o_fetch_page_fault", "1"),
    i_fetch_resp_ready(this, "i_fetch_resp_ready", "1"),
    // mmu <-> cache interface
    i_mem_req_ready(this, "i_mem_req_ready", "1"),
    o_mem_addr_valid(this, "o_mem_addr_valid", "1"),
    o_mem_addr(this, "o_mem_addr", "CFG_CPU_ADDR_BITS"),
    i_mem_data_valid(this, "i_mem_data_valid", "1"),
    i_mem_data_addr(this, "i_mem_data_addr", "CFG_CPU_ADDR_BITS"),
    i_mem_data(this, "i_mem_data", "64"),
    i_mem_load_fault(this, "i_mem_load_fault", "1"),
    i_mem_executable(this, "i_mem_executable", "1"),
    o_mem_resp_ready(this, "o_mem_resp_ready", "1"),
    // fence
    i_mmu_ena(this, "i_mmu_ena", "1", "MMU enabled in U and S modes. Sv48 only."),
    i_mmu_ppn(this, "i_mmu_ppn", "44", "Physical Page Number from SATP CSR"),
    i_flush_pipeline(this, "i_flush_pipeline", "1", "reset pipeline and cache"),
    // param
    Idle(this, "Idle", "0"),
    CheckTlb(this, "CheckTlb", "1"),
    CacheReq(this, "CacheReq", "2"),
    WaitResp(this, "WaitResp", "3"),
    HandleResp(this, "HandleResp", "4"),
    UpdateTlb(this, "UpdateTlb", "5"),
    AcceptFetch(this, "AcceptFetch", "6"),
    FlushTlb(this, "FlushTlb", "7"),
    PTE_V(this, "PTE_V", "0", "Valid PTE entry bit"),
    PTE_R(this, "PTE_R", "1", "Read access PTE entry bit"),
    PTE_W(this, "PTE_W", "2", "Write Access PTE entry bit"),
    PTE_X(this, "PTE_X", "3", "Execute Access PTE entry bit"),
    PTE_U(this, "PTE_U", "4", "Accessible in U-mode"),
    PTE_G(this, "PTE_G", "5", "Global mapping"),
    PTE_A(this, "PTE_A", "6", "Accessed bit"),
    PTE_D(this, "PTE_D", "7", "Dirty bit"),
    // signals
    wb_tlb_adr(this, "wb_tlb_adr", "CFG_MMU_TLB_AWIDTH"),
    w_tlb_wena(this, "w_tlb_wena", "1"),
    wb_tlb_wdata(this, "wb_tlb_wdata", "CFG_MMU_PTE_DWIDTH"),
    wb_tlb_rdata(this, "wb_tlb_rdata", "CFG_MMU_PTE_DWIDTH"),
    // registers
    state(this, "state", "2", "Idle"),
    req_x(this, "req_x", "1"),
    req_r(this, "req_r", "1"),
    req_w(this, "req_w", "1"),
    req_pa(this, "req_pa", "CFG_CPU_ADDR_BITS"),
    last_va(this, "last_va", "CFG_CPU_ADDR_BITS", "-1"),
    last_pa(this, "last_pa", "52", "-1"),
    resp_addr(this, "resp_addr", "CFG_CPU_ADDR_BITS"),
    resp_data(this, "resp_data", "64"),
    pte_permission(this, "pte_permission", "8", "0", "See permission bits: DAGUXWRV"),
    ex_load_fault(this, "ex_load_fault", "1"),
    ex_mpu_executable(this, "ex_mpu_executable", "1"),
    ex_page_fault(this, "ex_page_fault", "1"),
    tlb_hit(this, "tlb_hit", "1"),
    tlb_level(this, "tlb_level", "4"),
    tlb_wdata(this, "tlb_wdata", "CFG_MMU_PTE_DWIDTH"),
    tlb_flush_cnt(this, "tlb_flush_cnt", "CFG_MMU_PTE_AWIDTH"),
    tlb_flush_adr(this, "tlb_flush_adr", "CFG_MMU_PTE_AWIDTH"),
    // process
    comb(this),
    // sub-modules
    tlb(this, "tlb")
{
    Operation::start(this);
    NEW(tlb, tlb.getName().c_str());
        CONNECT(tlb, 0, tlb.i_clk, i_clk);
        CONNECT(tlb, 0, tlb.i_adr, wb_tlb_adr);
        CONNECT(tlb, 0, tlb.i_wena, w_tlb_wena);
        CONNECT(tlb, 0, tlb.i_wdata, wb_tlb_wdata);
        CONNECT(tlb, 0, tlb.o_rdata, wb_tlb_rdata);
    ENDNEW();
}

void Mmu::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;

    SETZERO(comb.v_fetch_req_ready);
    SETZERO(comb.v_fetch_data_valid);
    SETZERO(comb.vb_fetch_data_addr);
    SETZERO(comb.vb_fetch_data);
    SETZERO(comb.v_fetch_load_fault);
    SETZERO(comb.v_fetch_executable);
    SETZERO(comb.v_mem_addr_valid);
    SETZERO(comb.vb_mem_addr);
    SETZERO(comb.v_mem_resp_ready);
    SETZERO(comb.vb_pte_start_va);
    SETZERO(comb.vb_pte_base_va);
    SETZERO(comb.v_tlb_wena);
    SETVAL(comb.vb_tlb_adr, BITS(i_fetch_addr, DEC(CONST("ADD(12,CFG_MMU_TLB_AWIDTH)")), CONST("12")));
    SETVAL(comb.v_mpu_fault, OR2(i_mem_load_fault, INV(i_mem_executable)));

    TEXT("Start Page Physical Address");
    SETBITS(comb.vb_pte_start_va, 43, 0, i_mmu_ppn);
    IF (NZ(BIT(i_mmu_ppn, 43)));
        SETBITS(comb.vb_pte_start_va, 51, 44, ALLONES());
    ENDIF();
    TEXT("Page walking base Physical Address");
    SETBITS(comb.vb_pte_base_va, 43, 0, BITS(resp_data, 53, 10));
    IF (NZ(BIT(resp_data, 53)));
        SETBITS(comb.vb_pte_base_va, 51, 44, ALLONES());
    ENDIF();
    SETVAL(comb.vb_level0_off, CC2(BITS(last_va, 47, 39), CONST("0",3)));
    SETVAL(comb.vb_level1_off, CC2(BITS(last_va, 38, 30), CONST("0",3)));
    SETVAL(comb.vb_level2_off, CC2(BITS(last_va, 29, 21), CONST("0",3)));
    SETVAL(comb.vb_level3_off, CC2(BITS(last_va, 20, 12), CONST("0",3)));

TEXT();
    TEXT("Temporary variables are neccessary in systemc");
    SETBITS(comb.t_req_pa, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("12"),
                           BIG_TO_U64(BITS(wb_tlb_rdata, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("12"))));
    SETBITS(comb.t_req_pa, 11, 0, BITS(last_va, 11, 0));
    SETZERO(comb.t_tlb_wdata);
    SETBITS(comb.t_tlb_wdata, 115, 64, comb.vb_pte_base_va);
    IF (NZ(BIT(resp_data, 53)));
        SETBITS(comb.t_tlb_wdata, 63, 56, ALLONES());
    ELSE();
        SETBITS(comb.t_tlb_wdata, 63, 56, ALLZEROS());
    ENDIF();
    SETBITS(comb.t_tlb_wdata, 55, 12, BITS(resp_data, 53, 10));
    SETBITS(comb.t_tlb_wdata, 7, 0, BITS(resp_data, 7, 0));

TEXT();
    SWITCH (state);
    CASE(Idle);
        SETZERO(tlb_hit);
        SETZERO(ex_page_fault);
        IF (NZ(i_fetch_addr_valid));
            SETVAL(last_va, i_fetch_addr);
        ENDIF();
        IF (EZ(i_mmu_ena), "MMU disabled");
            TEXT("Direct connection to Cache");
            SETVAL(comb.v_fetch_req_ready, i_mem_req_ready);
            SETVAL(comb.v_fetch_data_valid, i_mem_data_valid);
            SETVAL(comb.vb_fetch_data_addr, i_mem_data_addr);
            SETVAL(comb.vb_fetch_data, i_mem_data);
            SETVAL(comb.v_fetch_load_fault, i_mem_load_fault);
            SETVAL(comb.v_fetch_executable, i_mem_executable);
            SETVAL(comb.v_mem_addr_valid, i_fetch_addr_valid);
            SETVAL(comb.vb_mem_addr, i_fetch_addr);
            SETVAL(comb.v_mem_resp_ready, i_fetch_resp_ready);
        ELSIF(NZ(tlb_flush_cnt));
            SETVAL(state, FlushTlb);
            SETZERO(tlb_wdata);
        ELSIF (EQ(BITS(i_fetch_addr, 63, 12), BITS(last_va, 63, 12)), "MMU enabled: Check the request to the same page:");
            TEXT("Direct connection to cache with the fast changing va to last_pa");
            SETVAL(comb.v_fetch_req_ready, i_mem_req_ready);
            SETVAL(comb.v_fetch_data_valid, i_mem_data_valid);
            SETVAL(comb.vb_fetch_data_addr, last_va);
            SETVAL(comb.vb_fetch_data, i_mem_data);
            SETVAL(comb.v_fetch_load_fault, i_mem_load_fault);
            SETVAL(comb.v_fetch_executable, i_mem_executable);
            SETVAL(comb.v_mem_addr_valid, i_fetch_addr_valid);
            SETBITS(comb.vb_mem_addr, 63, 12, last_pa);
            SETBITS(comb.vb_mem_addr, 11, 0, BITS(i_fetch_addr, 11, 0));
            SETVAL(comb.v_mem_resp_ready, i_fetch_resp_ready);
        ELSE();
            TEXT("MMU enabled: check TLB");
            SETONE(comb.v_fetch_req_ready);
            IF (NZ(i_fetch_addr_valid));
                SETVAL(state, CheckTlb);
                SETONE(req_x);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(CheckTlb);
        IF (EQ(BITS(last_va, 63, 12), BIG_TO_U64(BITS(wb_tlb_rdata, 115, 64))));
            TEXT("TLB hit");
            SETONE(tlb_hit);
            SETVAL(last_pa, BIG_TO_U64(BITS(wb_tlb_rdata, 63, 12)));
            SETVAL(req_pa, comb.t_req_pa);
            SETVAL(pte_permission, BIG_TO_U64(BITS(wb_tlb_rdata, 7, 0)));
        ELSE();
            TEXT("TLB miss");
            SETVAL(tlb_level, CONST("0x1", 4), "Start page decoding");
            SETVAL(req_pa, CC2(comb.vb_pte_start_va, comb.vb_level0_off));
        ENDIF();
        SETVAL(state, CacheReq);
        ENDCASE();
    CASE(CacheReq);
        SETONE(comb.v_mem_addr_valid);
        SETVAL(comb.vb_mem_addr, req_pa);
        IF (i_mem_req_ready);
            SETVAL(state, WaitResp);
        ENDIF();
        ENDCASE();
    CASE(WaitResp);
        SETONE(comb.v_mem_resp_ready);
        IF (NZ(i_mem_data_valid));
            SETVAL(resp_addr, i_mem_data_addr);
            SETVAL(resp_data, i_mem_data);
            SETVAL(ex_load_fault, i_mem_load_fault, "Hardware error (unmapped access)");
            SETVAL(ex_mpu_executable, i_mem_executable, "MPU executable flag");
            IF (NZ(OR2(tlb_hit, comb.v_mpu_fault)));
                SETVAL(state, AcceptFetch);
            ELSE();
                SETVAL(state, HandleResp);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(HandleResp);
        IF (OR2(EZ(BIT(resp_data, PTE_V)),
                NZ(AND2(INV(BIT(resp_data, PTE_R)), BIT(resp_data, PTE_W)))), "v=0 or (r=0 && w=1)");
            TEXT("PTE is invalid");
            SETONE(ex_page_fault);
            SETVAL(state, AcceptFetch);
        ELSIF(EZ(OR2(BIT(resp_data, PTE_R), BIT(resp_data, PTE_X))));
            TEXT("PTE is a apointer to the next level");
            SETVAL(state, CacheReq);
            SETVAL(tlb_level, LSH(tlb_level, 1));
            IF(NZ(BIT(tlb_level, 0)));
                SETVAL(req_pa, CC2(comb.vb_pte_base_va, comb.vb_level1_off));
            ELSIF(NZ(BIT(tlb_level, 1)));
                SETVAL(req_pa, CC2(comb.vb_pte_base_va, comb.vb_level2_off));
            ELSIF(NZ(BIT(tlb_level, 2)));
                SETVAL(req_pa, CC2(comb.vb_pte_base_va, comb.vb_level3_off));
            ELSE();
                TEXT("It was the last level");
                SETONE(ex_page_fault);
                SETVAL(state, AcceptFetch);
            ENDIF();
        ELSE();
            TEXT("PTE is a leaf");
            IF (AND2(NZ(req_x), EZ(BIT(wb_tlb_rdata, PTE_X))));
                SETVAL(state, AcceptFetch);
                SETONE(ex_page_fault);
            ELSIF (OR2(EZ(BIT(wb_tlb_rdata, PTE_A)),
                       AND2(req_w, INV(BIT(wb_tlb_rdata, PTE_D)))));
                TEXT("Implement option 1: raise a page-fault instead of (2) memory update with the new A,D-bits");
                SETVAL(state, AcceptFetch);
                SETONE(ex_page_fault);
            ELSE();
                SETVAL(state, UpdateTlb);
            ENDIF();
            SETVAL(last_pa, comb.vb_pte_base_va);
            SETVAL(req_pa, CC2(comb.vb_pte_base_va, BITS(last_va, 11, 0)));
            SETVAL(tlb_wdata, comb.t_tlb_wdata);
        ENDIF();
        ENDCASE();
    CASE(UpdateTlb);
        TEXT("Translation is finished: write va/pa into TLB memory");
        SETONE(comb.v_tlb_wena);
        SETVAL(comb.vb_tlb_adr, BITS(last_va, DEC(CONST("ADD(12,CFG_MMU_TLB_AWIDTH)")), CONST("12")));
        SETVAL(state, CacheReq, "Read data by physical address");
        SETONE(tlb_hit);
        ENDCASE();
    CASE(AcceptFetch);
        SETONE(comb.v_fetch_data_valid);
        SETVAL(comb.vb_fetch_data_addr, last_va);
        SETVAL(comb.vb_fetch_data, resp_data);
        SETVAL(comb.v_fetch_load_fault, ex_load_fault);
        SETVAL(comb.v_fetch_executable, ex_mpu_executable);
        IF (NZ(i_fetch_resp_ready));
            SETVAL(state, Idle);
            IF (NZ(ex_page_fault));
                SETVAL(last_va, ALLONES());
                SETVAL(last_pa, ALLONES());
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(FlushTlb);
        SETONE(comb.v_tlb_wena);
        SETVAL(comb.vb_tlb_adr, tlb_flush_adr);
        IF (EZ(tlb_flush_cnt));
            SETVAL(state, Idle);
        ELSE();
            SETVAL(tlb_flush_cnt, DEC(tlb_flush_cnt));
            SETVAL(tlb_flush_adr, INC(tlb_flush_adr));
        ENDIF();
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    IF (NZ(i_flush_pipeline));
        TEXT("Clear pipeline stage");
        SETVAL(tlb_flush_cnt, ALLONES());
        SETVAL(tlb_flush_adr, ALLZEROS());
    ENDIF();

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(w_tlb_wena, comb.v_tlb_wena);
    SETVAL(wb_tlb_adr, comb.vb_tlb_adr);
    SETVAL(wb_tlb_wdata, tlb_wdata);
    SETVAL(o_fetch_req_ready, comb.v_fetch_req_ready);
    SETVAL(o_fetch_data_valid, comb.v_fetch_data_valid);
    SETVAL(o_fetch_data_addr, comb.vb_fetch_data_addr);
    SETVAL(o_fetch_data, comb.vb_fetch_data);
    SETVAL(o_fetch_load_fault, comb.v_fetch_load_fault);
    SETVAL(o_fetch_executable, comb.v_fetch_executable);
    SETVAL(o_fetch_page_fault, ex_page_fault);
    SETVAL(o_mem_addr_valid, comb.v_mem_addr_valid);
    SETVAL(o_mem_addr, comb.vb_mem_addr);
    SETVAL(o_mem_resp_ready, comb.v_mem_resp_ready);
}
