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
    // core <-> mmu interface
    o_core_req_ready(this, "o_core_req_ready", "1"),
    i_core_req_valid(this, "i_core_req_valid", "1"),
    i_core_req_addr(this, "i_core_req_addr", "RISCV_ARCH"),
    i_core_req_fetch(this, "i_core_req_fetch", "1", "Memory request from 0=fetcher; 1=memaccess"),
    i_core_req_type(this, "i_core_req_type", "MemopType_Total", "Memory operation type"),
    i_core_req_wdata(this, "i_core_req_wdata", "64", "Data path requested data (write transaction)"),
    i_core_req_wstrb(this, "i_core_req_wstrb", "8", "8-bytes aligned strobs"),
    i_core_req_size(this, "i_core_req_size", "2", "1,2,4 or 8-bytes operation for uncached access"),
    o_core_resp_valid(this, "o_core_resp_valid", "1"),
    o_core_resp_addr(this, "o_core_resp_addr", "RISCV_ARCH"),
    o_core_resp_data(this, "o_core_resp_data", "64"),
    o_core_resp_load_fault(this, "o_core_resp_load_fault", "1", "Ex.2./Ex.5. Instruction access fault when = 0 and fetch or Load access fault"),
    o_core_resp_store_fault(this, "o_core_resp_store_fault", "1", "Ex.7. Store/AMO access fault"),
    o_core_resp_page_x_fault(this, "o_core_resp_page_x_fault", "1", "Ex.12 Instruction page fault"),
    o_core_resp_page_r_fault(this, "o_core_resp_page_r_fault", "1", "Ex.13 Load page fault"),
    o_core_resp_page_w_fault(this, "o_core_resp_page_w_fault", "1", "Ex.15 Store/AMO page fault"),
    i_core_resp_ready(this, "i_core_resp_ready", "1"),
    // mmu <-> cache interface
    i_mem_req_ready(this, "i_mem_req_ready", "1"),
    o_mem_req_valid(this, "o_mem_req_valid", "1"),
    o_mem_req_addr(this, "o_mem_req_addr", "RISCV_ARCH"),
    o_mem_req_type(this, "o_mem_req_type", "MemopType_Total", "Memory operation type"),
    o_mem_req_wdata(this, "o_mem_req_wdata", "64", "Data path requested data (write transaction)"),
    o_mem_req_wstrb(this, "o_mem_req_wstrb", "8", "8-bytes aligned strobs"),
    o_mem_req_size(this, "o_mem_req_size", "2", "1,2,4 or 8-bytes operation for uncached access"),
    i_mem_resp_valid(this, "i_mem_resp_valid", "1"),
    i_mem_resp_addr(this, "i_mem_resp_addr", "RISCV_ARCH"),
    i_mem_resp_data(this, "i_mem_resp_data", "64"),
    i_mem_resp_load_fault(this, "i_mem_resp_load_fault", "1"),
    i_mem_resp_store_fault(this, "i_mem_resp_store_fault", "1"),
    o_mem_resp_ready(this, "o_mem_resp_ready", "1"),
    // fence
    i_mmu_ena(this, "i_mmu_ena", "1", "MMU enabled in U and S modes. Sv39 or Sv48 are implemented."),
    i_mmu_sv39(this, "i_mmu_sv39", "1", "MMU sv39 is active"),
    i_mmu_sv48(this, "i_mmu_sv48", "1", "MMU sv48 is active"),
    i_mmu_ppn(this, "i_mmu_ppn", "44", "Physical Page Number from SATP CSR"),
    i_mprv(this, "i_mprv", "1", "modify priviledge flag can be active in m-mode"),
    i_mxr(this, "i_mxr", "1", "make executabale readable"),
    i_sum(this, "i_sum", "1", "permit Supervisor User Mode access"),
    i_fence(this, "i_fence", "1", "reset TBL entries at specific address"),
    i_fence_addr(this, "i_fence_addr", "RISCV_ARCH", "Fence address: 0=clean all TBL"),
    // param
    Idle(this, "Idle", "0"),
    WaitRespNoMmu(this, "WaitRespNoMmu", "1"),
    WaitRespLast(this, "WaitRespLast", "2"),
    CheckTlb(this, "CheckTlb", "3"),
    CacheReq(this, "CacheReq", "4"),
    WaitResp(this, "WaitResp", "5"),
    HandleResp(this, "HandleResp", "6"),
    UpdateTlb(this, "UpdateTlb", "7"),
    AcceptCore(this, "AcceptCore", "8"),
    FlushTlb(this, "FlushTlb", "9"),
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
    state(this, "state", "4", "FlushTlb"),
    req_flush(this, "req_flush", "1"),
    req_x(this, "req_x", "1"),
    req_r(this, "req_r", "1"),
    req_w(this, "req_w", "1"),
    req_pa(this, "req_pa", "RISCV_ARCH"),
    req_type(this, "req_type", "MemopType_Total"),
    req_wdata(this, "req_wdata", "64"),
    req_wstrb(this, "req_wstrb", "8"),
    req_size(this, "req_size", "2"),
    last_mmu_ena(this, "last_mmu_ena", "1"),
    last_va(this, "last_va", "RISCV_ARCH", "-1"),
    last_pa(this, "last_pa", "52", "-1"),
    last_permission(this, "last_permission", "8", "0", "Last permisison flags: DAGUXWRV"),
    last_page_size(this, "last_page_size", "2"),
    resp_addr(this, "resp_addr", "RISCV_ARCH"),
    resp_data(this, "resp_data", "64"),
    resp_load_fault(this, "resp_load_fault", "1"),
    resp_store_fault(this, "resp_store_fault", "1"),
    ex_page_fault(this, "ex_page_fault", "1"),
    tlb_hit(this, "tlb_hit", "1"),
    tlb_level(this, "tlb_level", "4"),
    tlb_page_size(this, "tlb_page_size", "2"),
    tlb_wdata(this, "tlb_wdata", "CFG_MMU_PTE_DWIDTH"),
    tlb_flush_cnt(this, "tlb_flush_cnt", "CFG_MMU_TLB_AWIDTH", "-1"),
    tlb_flush_adr(this, "tlb_flush_adr", "CFG_MMU_TLB_AWIDTH"),
    // process
    comb(this),
    // sub-modules
    tlb(this, "tlb", "CFG_MMU_TLB_AWIDTH", "CFG_MMU_PTE_DWIDTH")
{
    Operation::start(this);
    NEW(tlb, tlb.getName().c_str());
        CONNECT(tlb, 0, tlb.i_clk, i_clk);
        CONNECT(tlb, 0, tlb.i_addr, wb_tlb_adr);
        CONNECT(tlb, 0, tlb.i_wena, w_tlb_wena);
        CONNECT(tlb, 0, tlb.i_wdata, wb_tlb_wdata);
        CONNECT(tlb, 0, tlb.o_rdata, wb_tlb_rdata);
    ENDNEW();
}

void Mmu::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;

    SETVAL(comb.t_idx_lsb, ADD2(CONST("12"), MUL2(CONST("9"), TO_INT(last_page_size))));
    SETVAL(comb.vb_tlb_adr, BITSW(i_core_req_addr, comb.t_idx_lsb, cfg->CFG_MMU_TLB_AWIDTH));

TEXT();
    IF (NZ(i_core_req_fetch));
        SETONE(comb.v_core_req_x);
    ELSIF(ORx(2, &EZ(i_core_req_type),
                 &EQ(i_core_req_type, cfg->MemopType_Reserve)));
        SETONE(comb.v_core_req_r);
    ELSE();
        SETONE(comb.v_core_req_w);
    ENDIF();
    IF (EZ(BIT(last_permission, PTE_A)));
        SETONE(comb.last_page_fault_x);
        SETONE(comb.last_page_fault_r);
        SETONE(comb.last_page_fault_w);
    ENDIF();
    IF (EZ(BIT(last_permission, PTE_X)));
        SETONE(comb.last_page_fault_x);
    ENDIF();
    IF (EZ(BIT(last_permission, PTE_R)));
        SETONE(comb.last_page_fault_r);
    ENDIF();
    IF (OR2(EZ(BIT(last_permission, PTE_W)), EZ(BIT(last_permission, PTE_D))));
        SETONE(comb.last_page_fault_w);
    ENDIF();

TEXT();
    TEXT("Start Page Physical Address");
    SETBITS(comb.vb_pte_start_va, 43, 0, i_mmu_ppn);
    IF (NZ(BIT(i_mmu_ppn, 43)));
        SETBITS(comb.vb_pte_start_va, 51, 44, ALLONES());
    ENDIF();
    TEXT("Page walking base Physical Address");
    SETBITS(comb.vb_resp_ppn, 43, 0, BITS(resp_data, 53, 10));
    SETVAL(comb.v_va_ena, i_mmu_ena);
    TEXT("M-mode can opearate with physical and virtual addresses when MPRV=1");
    IF (AND2(NZ(i_mprv), EZ(AND_REDUCE(BITS(i_core_req_addr, 63, 48)))));
        TEXT("Use physical address");
        SETZERO(comb.v_va_ena);
    ENDIF();
    SETVAL(comb.vb_level0_off, CC2(BITS(last_va, 47, 39), CONST("0",3)));
    SETVAL(comb.vb_level1_off, CC2(BITS(last_va, 38, 30), CONST("0",3)));
    SETVAL(comb.vb_level2_off, CC2(BITS(last_va, 29, 21), CONST("0",3)));
    SETVAL(comb.vb_level3_off, CC2(BITS(last_va, 20, 12), CONST("0",3)));

TEXT();
    TEXT("Pages: 4 KB, 8MB, 16 GB and 32 TB for sv48 only");
    TEXT("Check the last hit depending page size:");
    SETZERO(comb.v_last_valid);
    IF (ANDx(2, &EQ(last_page_size, CONST("0", 2)),
                &EQ(BITS(i_core_req_addr, 63, 12), BITS(last_va, 63, 12))));
        SETONE(comb.v_last_valid);
        SETVAL(comb.vb_last_pa_req, CC2(last_pa, BITS(i_core_req_addr, 11, 0)));
    ELSIF (ANDx(2, &EQ(last_page_size, CONST("1", 2)),
                &EQ(BITS(i_core_req_addr, 63, 21), BITS(last_va, 63, 21))));
        SETONE(comb.v_last_valid);
        SETVAL(comb.vb_last_pa_req, CC2(BITS(last_pa, 51, 9), BITS(i_core_req_addr, 20, 0)));
    ELSIF (ANDx(2, &EQ(last_page_size, CONST("2", 2)),
                &EQ(BITS(i_core_req_addr, 63, 30), BITS(last_va, 63, 30))));
        SETONE(comb.v_last_valid);
        SETVAL(comb.vb_last_pa_req, CC2(BITS(last_pa, 51, 18), BITS(i_core_req_addr, 29, 0)));
    ELSIF (ANDx(2, &EQ(last_page_size, CONST("3", 2)),
                &EQ(BITS(i_core_req_addr, 63, 39), BITS(last_va, 63, 39))));
        SETONE(comb.v_last_valid);
        SETVAL(comb.vb_last_pa_req, CC2(BITS(last_pa, 51, 27), BITS(i_core_req_addr, 38, 0)));
    ENDIF();

TEXT();
    TEXT("Check table hit depending page size:");
    SETBITS(comb.vb_tlb_pa0, 63, 12, BIG_TO_U64(BITS(wb_tlb_rdata, 63, 12)));
    SETBITS(comb.vb_tlb_pa0, 11, 0, BITS(last_va, 11, 0));
    SETBITS(comb.vb_tlb_pa1, 63, 21, BIG_TO_U64(BITS(wb_tlb_rdata, 63, 21)));
    SETBITS(comb.vb_tlb_pa1, 20, 0, BITS(last_va, 20, 0));
    SETBITS(comb.vb_tlb_pa2, 63, 30, BIG_TO_U64(BITS(wb_tlb_rdata, 63, 30)));
    SETBITS(comb.vb_tlb_pa2, 29, 0, BITS(last_va, 29, 0));
    SETBITS(comb.vb_tlb_pa3, 63, 39, BIG_TO_U64(BITS(wb_tlb_rdata, 63, 39)));
    SETBITS(comb.vb_tlb_pa3, 38, 0, BITS(last_va, 38, 0));

TEXT();
    IF (NZ(BIT(wb_tlb_rdata, PTE_V)));
        IF (EQ(BIG_TO_U64(BITS(wb_tlb_rdata, 9, 8)), CONST("3", 2)));
            TEXT("32 TB pages:");
            SETVAL(comb.v_tlb_hit, INV(XOR2(BITS(last_va, 63, 39), BIG_TO_U64(BITS(wb_tlb_rdata, 115, 91)))));
            SETVAL(comb.vb_tlb_pa_hit, comb.vb_tlb_pa3);
        ELSIF (EQ(BIG_TO_U64(BITS(wb_tlb_rdata, 9, 8)), CONST("2", 2)));
            TEXT("16 GB pages:");
            SETVAL(comb.v_tlb_hit, INV(XOR2(BITS(last_va, 63, 30), BIG_TO_U64(BITS(wb_tlb_rdata, 115, 82)))));
            SETVAL(comb.vb_tlb_pa_hit, comb.vb_tlb_pa2);
        ELSIF (EQ(BIG_TO_U64(BITS(wb_tlb_rdata, 9, 8)), CONST("1", 2)));
            TEXT("8 MB pages:");
            SETVAL(comb.v_tlb_hit, INV(XOR2(BITS(last_va, 63, 21), BIG_TO_U64(BITS(wb_tlb_rdata, 115, 73)))));
            SETVAL(comb.vb_tlb_pa_hit, comb.vb_tlb_pa1);
        ELSE();
            TEXT("4 KB pages:");
            SETVAL(comb.v_tlb_hit, INV(XOR2(BITS(last_va, 63, 12), BIG_TO_U64(BITS(wb_tlb_rdata, 115, 64)))));
            SETVAL(comb.vb_tlb_pa_hit, comb.vb_tlb_pa0);
        ENDIF();
    ENDIF();

TEXT();
    SETBITS(comb.t_tlb_wdata, 115, 64, BITS(last_va, 63, 12));
    SETBITS(comb.t_tlb_wdata, 63, 12, comb.vb_resp_ppn);
    SETBITS(comb.t_tlb_wdata, 9, 8, tlb_page_size);
    SETBITS(comb.t_tlb_wdata, 7, 0, BITS(resp_data, 7, 0));

TEXT();
    SWITCH (state);
    CASE(Idle);
        SETZERO(tlb_hit);
        SETZERO(resp_load_fault);
        SETZERO(resp_store_fault);
        SETZERO(ex_page_fault);
        IF (NZ(i_core_req_valid));
            SETVAL(last_mmu_ena, AND2(i_mmu_ena, comb.v_va_ena));
            SETVAL(last_va, i_core_req_addr);
            SETVAL(req_type, i_core_req_type);
            SETVAL(req_wdata, i_core_req_wdata);
            SETVAL(req_wstrb, i_core_req_wstrb);
            SETVAL(req_size, i_core_req_size);
        ENDIF();
        IF(NZ(req_flush));
            SETZERO(req_flush);
            SETZERO(tlb_wdata);
            SETVAL(state, FlushTlb);
        ELSIF (OR2(EZ(i_mmu_ena), EZ(comb.v_va_ena)), "MMU disabled");
            TEXT("Direct connection to Cache");
            SETVAL(comb.v_core_req_ready, i_mem_req_ready);
            SETVAL(comb.v_core_resp_valid, i_mem_resp_valid);
            SETVAL(comb.vb_core_resp_addr, i_mem_resp_addr);
            SETVAL(comb.vb_core_resp_data, i_mem_resp_data);
            SETVAL(comb.v_core_resp_load_fault, i_mem_resp_load_fault);
            SETVAL(comb.v_core_resp_store_fault, i_mem_resp_store_fault);
            SETVAL(comb.v_mem_req_valid, i_core_req_valid);
            SETVAL(comb.vb_mem_req_addr, i_core_req_addr);
            SETVAL(comb.vb_mem_req_type, i_core_req_type);
            SETVAL(comb.vb_mem_req_wdata, i_core_req_wdata);
            SETVAL(comb.vb_mem_req_wstrb, i_core_req_wstrb);
            SETVAL(comb.vb_mem_req_size, i_core_req_size);
            SETVAL(comb.v_mem_resp_ready, i_core_resp_ready);
            IF (NZ(AND2(i_core_req_valid, i_mem_req_ready)));
                SETVAL(state, WaitRespNoMmu);
            ENDIF();
        ELSIF (AND2(NZ(last_mmu_ena), NZ(comb.v_last_valid)), "MMU enabled: Check the request to the same page:");
            TEXT("Direct connection to cache with the fast changing va to last_pa");
            SETVAL(comb.v_core_req_ready, i_mem_req_ready);
            SETVAL(comb.v_core_resp_valid, i_mem_resp_valid);
            SETVAL(comb.vb_core_resp_addr, last_va);
            SETVAL(comb.vb_core_resp_data, i_mem_resp_data);
            SETVAL(comb.v_core_resp_load_fault, i_mem_resp_load_fault);
            SETVAL(comb.v_core_resp_store_fault, i_mem_resp_store_fault);
            SETVAL(comb.v_mem_req_valid, i_core_req_valid);
            SETVAL(comb.vb_mem_req_addr, comb.vb_last_pa_req);
            SETVAL(comb.vb_mem_req_type, i_core_req_type);
            SETVAL(comb.vb_mem_req_wdata, i_core_req_wdata);
            SETVAL(comb.vb_mem_req_wstrb, i_core_req_wstrb);
            SETVAL(comb.vb_mem_req_size, i_core_req_size);
            SETVAL(comb.v_mem_resp_ready, i_core_resp_ready);
            IF (NZ(AND2(i_core_req_valid, i_mem_req_ready)));
                SETVAL(state, WaitRespLast);
            ENDIF();
        ELSE();
            TEXT("MMU enabled: check TLB");
            SETONE(comb.v_core_req_ready);
            IF (NZ(i_core_req_valid));
                SETVAL(state, CheckTlb);
                SETVAL(req_x, comb.v_core_req_x);
                SETVAL(req_r, comb.v_core_req_r);
                SETVAL(req_w, comb.v_core_req_w);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(WaitRespNoMmu);
        // Direct connection to Cache
        SETVAL(comb.v_core_req_ready, i_mem_req_ready);
        SETVAL(comb.v_core_resp_valid, i_mem_resp_valid);
        SETVAL(comb.vb_core_resp_addr, i_mem_resp_addr);
        SETVAL(comb.vb_core_resp_data, i_mem_resp_data);
        SETVAL(comb.v_core_resp_load_fault, i_mem_resp_load_fault);
        SETVAL(comb.v_core_resp_store_fault, i_mem_resp_store_fault);
        SETVAL(comb.v_mem_req_valid, i_core_req_valid);
        SETVAL(comb.vb_mem_req_addr, i_core_req_addr);
        SETVAL(comb.vb_mem_req_type, i_core_req_type);
        SETVAL(comb.vb_mem_req_wdata, i_core_req_wdata);
        SETVAL(comb.vb_mem_req_wstrb, i_core_req_wstrb);
        SETVAL(comb.vb_mem_req_size, i_core_req_size);
        SETVAL(comb.v_mem_resp_ready, i_core_resp_ready);
        IF (NZ(AND2(i_mem_resp_valid, i_core_resp_ready)));
            IF (NZ(i_mmu_ena));
                TEXT("Do not accept new request because MMU state changed");
                SETZERO(comb.v_core_req_ready);
                SETZERO(comb.v_mem_req_valid);
            ENDIF();
            IF (OR2(EZ(comb.v_core_req_ready), EZ(comb.v_mem_req_valid)));
                SETVAL(state, Idle);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(WaitRespLast);
        SETVAL(comb.v_core_req_ready, i_mem_req_ready);
        SETVAL(comb.v_core_resp_valid, i_mem_resp_valid);
        SETVAL(comb.vb_core_resp_addr, last_va);
        SETVAL(comb.vb_core_resp_data, i_mem_resp_data);
        SETVAL(comb.v_core_resp_load_fault, i_mem_resp_load_fault);
        SETVAL(comb.v_core_resp_store_fault, i_mem_resp_store_fault);
        SETVAL(comb.v_mem_req_valid, i_core_req_valid);
        SETVAL(comb.vb_mem_req_addr, comb.vb_last_pa_req);
        SETVAL(comb.vb_mem_req_type, i_core_req_type);
        SETVAL(comb.vb_mem_req_wdata, i_core_req_wdata);
        SETVAL(comb.vb_mem_req_wstrb, i_core_req_wstrb);
        SETVAL(comb.vb_mem_req_size, i_core_req_size);
        SETVAL(comb.v_mem_resp_ready, i_core_resp_ready);
        IF (NZ(AND2(i_mem_resp_valid, i_core_resp_ready)));
            IF (EZ(comb.v_last_valid));
                TEXT("Do not accept new request because of new VA request");
                SETZERO(comb.v_core_req_ready);
                SETZERO(comb.v_mem_req_valid);
            ENDIF();
            IF (OR2(EZ(comb.v_core_req_ready), EZ(comb.v_mem_req_valid)));
                SETVAL(state, Idle);
            ELSE();
                SETVAL(last_va, i_core_req_addr);
                SETVAL(req_type, i_core_req_type);
                SETVAL(req_wdata, i_core_req_wdata);
                SETVAL(req_wstrb, i_core_req_wstrb);
                SETVAL(req_size, i_core_req_size);
                SETVAL(req_x, comb.v_core_req_x);
                SETVAL(req_r, comb.v_core_req_r);
                SETVAL(req_w, comb.v_core_req_w);
                IF(OR3(AND2(comb.v_core_req_x, comb.last_page_fault_x),
                      AND2(comb.v_core_req_r, comb.last_page_fault_r),
                      AND2(comb.v_core_req_w, comb.last_page_fault_w)));
                    TEXT("New request to the same page has not permission");
                    SETONE(ex_page_fault);
                    SETVAL(state, AcceptCore);
                ENDIF();
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(CheckTlb);
        IF (NZ(comb.v_tlb_hit));
            TEXT("TLB hit");
            SETONE(tlb_hit);
            SETVAL(last_pa, BIG_TO_U64(BITS(wb_tlb_rdata, 63, 12)));
            SETVAL(last_permission, BIG_TO_U64(BITS(wb_tlb_rdata, 7, 0)));
            SETVAL(req_pa, comb.vb_tlb_pa_hit);
        ELSE();
            TEXT("TLB miss");
            IF (NZ(i_mmu_sv39));
                SETVAL(tlb_level, CONST("0x2", 4), "Start page decoding sv39");
                SETVAL(tlb_page_size, CONST("2", 2));
                SETVAL(req_pa, CC2(comb.vb_pte_start_va, comb.vb_level1_off));
            ELSE();
                SETVAL(tlb_level, CONST("0x1", 4), "Start page decoding sv48");
                SETVAL(tlb_page_size, CONST("3", 2));
                SETVAL(req_pa, CC2(comb.vb_pte_start_va, comb.vb_level0_off));
            ENDIF();
        ENDIF();
        SETVAL(state, CacheReq);
        ENDCASE();
    CASE(CacheReq);
        SETONE(comb.v_mem_req_valid);
        SETVAL(comb.vb_mem_req_addr, req_pa);
        IF (EZ(tlb_hit));
            SETZERO(comb.vb_mem_req_type, "Load tlb item");
        ELSE();
            SETVAL(comb.vb_mem_req_type, req_type);
        ENDIF();
        SETVAL(comb.vb_mem_req_wdata, req_wdata);
        SETVAL(comb.vb_mem_req_wstrb, req_wstrb);
        SETVAL(comb.vb_mem_req_size, req_size);
        IF (NZ(i_mem_req_ready));
            SETVAL(state, WaitResp);
        ENDIF();
        ENDCASE();
    CASE(WaitResp);
        SETONE(comb.v_mem_resp_ready);
        IF (NZ(i_mem_resp_valid));
            SETVAL(resp_addr, i_mem_resp_addr);
            SETVAL(resp_data, i_mem_resp_data);
            SETVAL(resp_load_fault, i_mem_resp_load_fault, "Hardware error Load (unmapped access)");
            SETVAL(resp_store_fault, i_mem_resp_store_fault, "Hardware error Store/AMO (unmapped access)");
            IF (NZ(OR3(tlb_hit, i_mem_resp_load_fault, i_mem_resp_store_fault)));
                SETVAL(state, AcceptCore);
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
            SETVAL(state, AcceptCore);
        ELSIF(EZ(OR3(BIT(resp_data, PTE_R), BIT(resp_data, PTE_W), BIT(resp_data, PTE_X))));
            TEXT("PTE is a apointer to the next level");
            SETVAL(state, CacheReq);
            SETVAL(tlb_level, LSH(tlb_level, 1));
            SETVAL(tlb_page_size, DEC(tlb_page_size));
            IF(NZ(BIT(tlb_level, 0)));
                SETVAL(req_pa, CC2(comb.vb_resp_ppn, comb.vb_level1_off));
            ELSIF(NZ(BIT(tlb_level, 1)));
                SETVAL(req_pa, CC2(comb.vb_resp_ppn, comb.vb_level2_off));
            ELSIF(NZ(BIT(tlb_level, 2)));
                SETVAL(req_pa, CC2(comb.vb_resp_ppn, comb.vb_level3_off));
            ELSE();
                TEXT("It was the last level");
                SETONE(ex_page_fault);
                SETVAL(state, AcceptCore);
            ENDIF();
        ELSE();
            TEXT("PTE is a leaf");
            IF (EZ(BIT(resp_data, PTE_A)));
                SETVAL(state, AcceptCore);
                SETONE(ex_page_fault);
            ELSIF (AND2(NZ(req_x), EZ(BIT(resp_data, PTE_X))));
                SETVAL(state, AcceptCore);
                SETONE(ex_page_fault);
            ELSIF (AND2(NZ(req_r), EZ(BIT(resp_data, PTE_R))));
                SETVAL(state, AcceptCore);
                SETONE(ex_page_fault);
            ELSIF (AND2(NZ(req_w), OR2(EZ(BIT(resp_data, PTE_W)), EZ(BIT(resp_data, PTE_D)))));
                SETVAL(state, AcceptCore);
                SETONE(ex_page_fault);
            ELSE();
                SETVAL(state, UpdateTlb);
            ENDIF();
            SETVAL(last_permission, BITS(resp_data, 7, 0));
            SETVAL(last_page_size, tlb_page_size);
            SETVAL(tlb_wdata, comb.t_tlb_wdata);
            TEXT("Pages more than 4KB support:");
            IF (NZ(BIT(tlb_level, 0)));
                SETVAL(req_pa, CC2(BITS(comb.vb_resp_ppn, 51, 27), BITS(last_va, 38, 0)));
                SETVAL(last_pa, CC2(BITS(comb.vb_resp_ppn, 51, 27), CONST("0", 27)));
            ELSIF (NZ(BIT(tlb_level, 1)));
                SETVAL(req_pa, CC2(BITS(comb.vb_resp_ppn, 51, 18), BITS(last_va, 29, 0)));
                SETVAL(last_pa, CC2(BITS(comb.vb_resp_ppn, 51, 18), CONST("0", 18)));
            ELSIF (NZ(BIT(tlb_level, 2)));
                SETVAL(req_pa, CC2(BITS(comb.vb_resp_ppn, 51, 9), BITS(last_va, 20, 0)));
                SETVAL(last_pa, CC2(BITS(comb.vb_resp_ppn, 51, 9), CONST("0", 9)));
            ELSE();
                SETVAL(req_pa, CC2(comb.vb_resp_ppn, BITS(last_va, 11, 0)));
                SETVAL(last_pa, comb.vb_resp_ppn);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(UpdateTlb);
        TEXT("Translation is finished: write va/pa into TLB memory");
        SETONE(comb.v_tlb_wena);
        SETVAL(comb.vb_tlb_adr, BITSW(last_va, comb.t_idx_lsb, cfg->CFG_MMU_TLB_AWIDTH));
        SETVAL(state, CacheReq, "Read data by physical address");
        SETONE(tlb_hit);
        ENDCASE();
    CASE(AcceptCore);
        SETONE(comb.v_core_resp_valid);
        SETVAL(comb.vb_core_resp_addr, last_va);
        SETVAL(comb.vb_core_resp_data, resp_data);
        SETVAL(comb.v_core_resp_load_fault, resp_load_fault);
        SETVAL(comb.v_core_resp_store_fault, resp_store_fault);
        IF (NZ(i_core_resp_ready));
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
        SETVAL(last_va, ALLONES());
        SETVAL(last_pa, ALLONES());
        SETZERO(last_mmu_ena);
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
    IF (NZ(i_fence));
        TEXT("Clear whole table ignoring i_fence_addr");
        SETONE(req_flush);
        SETVAL(tlb_flush_cnt, ALLONES());
        SETZERO(tlb_flush_adr);
    ENDIF();

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(w_tlb_wena, comb.v_tlb_wena);
    SETVAL(wb_tlb_adr, comb.vb_tlb_adr);
    SETVAL(wb_tlb_wdata, tlb_wdata);
    SETVAL(o_core_req_ready, comb.v_core_req_ready);
    SETVAL(o_core_resp_valid, comb.v_core_resp_valid);
    SETVAL(o_core_resp_addr, comb.vb_core_resp_addr);
    SETVAL(o_core_resp_data, comb.vb_core_resp_data);
    SETVAL(o_core_resp_load_fault, comb.v_core_resp_load_fault);
    SETVAL(o_core_resp_store_fault, comb.v_core_resp_store_fault);
    SETVAL(o_core_resp_page_x_fault, AND2(ex_page_fault, req_x));
    SETVAL(o_core_resp_page_r_fault, AND2(ex_page_fault, req_r));
    SETVAL(o_core_resp_page_w_fault, AND2(ex_page_fault, req_w));
    SETVAL(o_mem_req_valid, comb.v_mem_req_valid);
    SETVAL(o_mem_req_addr, comb.vb_mem_req_addr);
    SETVAL(o_mem_req_type, comb.vb_mem_req_type);
    SETVAL(o_mem_req_wdata, comb.vb_mem_req_wdata);
    SETVAL(o_mem_req_wstrb, comb.vb_mem_req_wstrb);
    SETVAL(o_mem_req_size, comb.vb_mem_req_size);
    SETVAL(o_mem_resp_ready, comb.v_mem_resp_ready);
}
