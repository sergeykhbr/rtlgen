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
    i_prv(this, "i_prv", "2", "CPU priviledge level"),
    i_satp(this, "i_satp", "RISCV_ARCH", "Supervisor Adress Translation and Protection"),
    i_flush_pipeline(this, "i_flush_pipeline", "1", "reset pipeline and cache"),
    // param
    Idle(this, "Idle", "0"),
    CheckTlb(this, "CheckTlb", "1"),
    WaitReqAccept(this, "WaitReqAccept", "2"),
    WaitResp(this, "WaitResp", "3"),
    // signals
    wb_tlb_adr(this, "wb_tlb_adr", "CFG_MMU_TLB_AWIDTH"),
    w_tlb_wena(this, "w_tlb_wena", "1"),
    wb_tlb_wdata(this, "wb_tlb_wdata", "CFG_MMU_PTE_DWIDTH"),
    wb_tlb_rdata(this, "wb_tlb_rdata", "CFG_MMU_PTE_DWIDTH"),
    // registers
    state(this, "state", "2", "Idle"),
    req_va(this, "req_va", "CFG_CPU_ADDR_BITS"),
    last_va(this, "last_va", "CFG_CPU_ADDR_BITS", "-1"),
    last_pa(this, "last_pa", "CFG_CPU_ADDR_BITS", "-1"),
    req_valid(this, "req_valid"),
    resp_ready(this, "resp_ready"),
    req_addr(this, "req_addr", "CFG_CPU_ADDR_BITS", "-1"),
    mem_resp_shadow(this, "mem_resp_shadow", "CFG_CPU_ADDR_BITS", "-1", "the same as memory response but internal"),
    pc(this, "pc", "CFG_CPU_ADDR_BITS", "-1"),
    instr(this, "instr", "64"),
    instr_load_fault(this, "instr_load_fault"),
    instr_executable(this, "instr_executable"),
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
    SETZERO(comb.v_sv39);
    SETZERO(comb.v_sv48);
    SETZERO(comb.v_fetch_req_ready);
    SETZERO(comb.v_fetch_data_valid);
    SETZERO(comb.vb_fetch_data_addr);
    SETZERO(comb.vb_fetch_data);
    SETZERO(comb.v_fetch_load_fault);
    SETZERO(comb.v_fetch_executable);
    SETZERO(comb.v_mem_addr_valid);
    SETZERO(comb.vb_mem_addr);
    SETZERO(comb.v_mem_resp_ready);

TEXT();
    TEXT("S- or U- mode plus Sv paging enabled in SATP");
    IF(EQ(BITS(i_satp,63,60), CONST("8", 4)));
        SETONE(comb.v_sv39);
    ELSIF(EQ(BITS(i_satp,63,60), CONST("9", 4)));
        SETONE(comb.v_sv48);
    ENDIF();
    SETVAL(comb.v_mmu_ena, AND2(INV(BIT(i_prv,1)), OR2(comb.v_sv39, comb.v_sv48)));

TEXT();
    IF (EZ(comb.v_mmu_ena));
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
    ELSIF (EQ(i_fetch_addr, last_va), "Check the request to the same page:");
        TEXT("Direct connection to cache with the fast changing va to last_pa");
        SETVAL(comb.v_fetch_req_ready, i_mem_req_ready);
        SETVAL(comb.v_fetch_data_valid, i_mem_data_valid);
        SETVAL(comb.vb_fetch_data_addr, i_mem_data_addr);
        SETVAL(comb.vb_fetch_data, i_mem_data);
        SETVAL(comb.v_fetch_load_fault, i_mem_load_fault);
        SETVAL(comb.v_fetch_executable, i_mem_executable);
        SETVAL(comb.v_mem_addr_valid, i_fetch_addr_valid);
        SETVAL(comb.vb_mem_addr, last_pa);
        SETVAL(comb.v_mem_resp_ready, i_fetch_resp_ready);
    ELSE();
        TEXT("MMU enabled, check TLB");
        SWITCH (state);
        CASE(Idle);
            SETONE(comb.v_fetch_req_ready);
            IF (NZ(i_fetch_addr_valid));
                SETVAL(state, CheckTlb);
                SETVAL(req_va, i_fetch_addr);
            ENDIF();
            SETZERO(req_valid);
            SETZERO(resp_ready);
            SETVAL(state, WaitReqAccept);
            SETVAL(req_addr, i_fetch_addr);
            SETONE(req_valid);
            ENDCASE();
        CASE(CheckTlb);
            IF (EQ(req_va, BIG_TO_U64(BITS(wb_tlb_wdata, 127, 64))));
            ENDIF();
            ENDCASE();
        CASE(WaitReqAccept);
            IF (i_mem_req_ready);
                //SETVAL(req_valid, AND2(i_bp_valid, INV(i_progbuf_ena)));
                //SETVAL(req_addr, i_bp_pc);
                SETVAL(mem_resp_shadow, req_addr);
                SETONE(resp_ready);
                SETVAL(state, WaitResp);
            //ELSIF (i_bp_valid);
            //    TEXT("re-write requested address (while it wasn't accepted)");
            //    SETVAL(req_addr, i_bp_pc);
            ENDIF();
            ENDCASE();
        CASE(WaitResp);
            IF (NZ(i_mem_data_valid));
                SETVAL(pc, i_mem_data_addr);
                SETVAL(instr, i_mem_data);
                SETVAL(instr_load_fault, i_mem_load_fault);
                SETVAL(instr_executable, i_mem_executable);
                //SETVAL(req_valid, AND2(i_bp_valid, INV(i_progbuf_ena)));

                TEXT();
                IF (NZ(req_valid));
                    IF (NZ(i_mem_req_ready));
                        //SETVAL(req_addr, i_bp_pc);
                        SETVAL(mem_resp_shadow, req_addr);
                    ELSE();
                        SETVAL(state, WaitReqAccept);
                    ENDIF();
                //ELSIF (AND2(i_bp_valid, INV(i_progbuf_ena)));
                //    SETVAL(req_addr, i_bp_pc);
                //    SETVAL(state, WaitReqAccept);
                ELSE();
                    SETVAL(req_addr, ALLONES());
                    SETVAL(state, Idle);
                ENDIF();
            ENDIF();
            ENDCASE();
        CASEDEF();
            ENDCASE();
        ENDSWITCH();
    ENDIF();

TEXT();
    IF (NZ(i_flush_pipeline));
        TEXT("Clear pipeline stage");
        SETZERO(req_valid);
        SETVAL(pc, ALLONES());
        SETZERO(instr);
        SETZERO(instr_load_fault);
        SETZERO(instr_executable);
    ENDIF();

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(wb_tlb_adr, BITS(i_fetch_addr, DEC(CONST("ADD(12,CFG_MMU_TLB_AWIDTH)")), CONST("12")));
    SETVAL(o_fetch_req_ready, comb.v_fetch_req_ready);
    SETVAL(o_fetch_data_valid, comb.v_fetch_data_valid);
    SETVAL(o_fetch_data_addr, comb.vb_fetch_data_addr);
    SETVAL(o_fetch_data, comb.vb_fetch_data);
    SETVAL(o_fetch_load_fault, comb.v_fetch_load_fault);
    SETVAL(o_fetch_executable, comb.v_fetch_executable);
    SETVAL(o_mem_addr_valid, comb.v_mem_addr_valid);
    SETVAL(o_mem_addr, comb.vb_mem_addr);
    SETVAL(o_mem_resp_ready, comb.v_mem_resp_ready);
}
