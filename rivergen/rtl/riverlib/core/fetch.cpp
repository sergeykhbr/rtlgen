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

#include "fetch.h"

InstrFetch::InstrFetch(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "InstrFetch", name, comment),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_bp_valid(this, "i_bp_valid", "1"),
    i_bp_pc(this, "i_bp_pc", "RISCV_ARCH"),
    o_requested_pc(this, "o_requested_pc", "RISCV_ARCH"),
    o_fetching_pc(this, "o_fetching_pc", "RISCV_ARCH"),
    i_mem_req_ready(this, "i_mem_req_ready", "1"),
    o_mem_addr_valid(this, "o_mem_addr_valid", "1"),
    o_mem_addr(this, "o_mem_addr", "RISCV_ARCH"),
    i_mem_data_valid(this, "i_mem_data_valid", "1"),
    i_mem_data_addr(this, "i_mem_data_addr", "RISCV_ARCH"),
    i_mem_data(this, "i_mem_data", "64"),
    i_mem_load_fault(this, "i_mem_load_fault", "1"),
    i_mem_page_fault_x(this, "i_mem_page_fault_x", "1"),
    o_mem_resp_ready(this, "o_mem_resp_ready", "1"),
    i_flush_pipeline(this, "i_flush_pipeline", "1", "reset pipeline and cache"),
    i_progbuf_ena(this, "i_progbuf_ena", "1", "executing from prog buffer"),
    i_progbuf_pc(this, "i_progbuf_pc", "RISCV_ARCH", "progbuf counter"),
    i_progbuf_instr(this, "i_progbuf_instr", "64", "progbuf instruction"),
    o_instr_load_fault(this, "o_instr_load_fault", "1"),
    o_instr_page_fault_x(this, "o_instr_page_fault_x", "1"),
    o_pc(this, "o_pc", "RISCV_ARCH"),
    o_instr(this, "o_instr", "64"),
    // param
    Idle(this, "Idle", "0"),
    WaitReqAccept(this, "WaitReqAccept", "1"),
    WaitResp(this, "WaitResp", "2"),
    // registers
    state(this, "state", "2", "Idle"),
    req_valid(this, "req_valid", "1"),
    resp_ready(this, "resp_ready", "1"),
    req_addr(this, "req_addr", "RISCV_ARCH", "'1"),
    mem_resp_shadow(this, "mem_resp_shadow", "RISCV_ARCH", "'1", "the same as memory response but internal"),
    pc(this, "pc", "RISCV_ARCH", "'1"),
    instr(this, "instr", "64", "'0", NO_COMMENT),
    instr_load_fault(this, "instr_load_fault", "1"),
    instr_page_fault_x(this, "instr_page_fault_x", "1"),
    progbuf_ena(this, "progbuf_ena", "1"),
    // process
    comb(this)
{
}

void InstrFetch::proc_comb() {

    SWITCH (state);
    CASE(Idle);
        SETZERO(req_valid);
        SETZERO(resp_ready);
        SETZERO(progbuf_ena);
        IF (NZ(i_progbuf_ena));
            TEXT("Execution from buffer");
            SETONE(progbuf_ena);
            SETVAL(pc, i_progbuf_pc);
            SETVAL(instr, i_progbuf_instr);
            SETZERO(instr_load_fault);
            SETZERO(instr_page_fault_x);
        ELSIF (NZ(i_bp_valid));
            SETVAL(state, WaitReqAccept);
            SETVAL(req_addr, i_bp_pc);
            SETONE(req_valid);
        ENDIF();
        ENDCASE();
    CASE(WaitReqAccept);
        IF (NZ(i_mem_req_ready));
            SETVAL(req_valid, AND2(i_bp_valid, INV(i_progbuf_ena)));
            SETVAL(req_addr, i_bp_pc);
            SETVAL(mem_resp_shadow, req_addr);
            SETONE(resp_ready);
            SETVAL(state, WaitResp);
        ELSIF (NZ(i_bp_valid));
            TEXT("re-write requested address (while it wasn't accepted)");
            SETVAL(req_addr, i_bp_pc);
        ENDIF();
        ENDCASE();
    CASE(WaitResp);
        IF (NZ(i_mem_data_valid));
            SETVAL(pc, i_mem_data_addr);
            SETVAL(instr, i_mem_data);
            SETVAL(instr_load_fault, i_mem_load_fault);
            SETVAL(instr_page_fault_x, i_mem_page_fault_x);
            SETVAL(req_valid, AND2(i_bp_valid, INV(i_progbuf_ena)));

            TEXT();
            IF (NZ(req_valid));
                IF (NZ(i_mem_req_ready));
                    SETVAL(req_addr, i_bp_pc);
                    SETVAL(mem_resp_shadow, req_addr);
                ELSE();
                    SETVAL(state, WaitReqAccept);
                ENDIF();
            ELSIF (AND2(NZ(i_bp_valid), EZ(i_progbuf_ena)));
                SETVAL(req_addr, i_bp_pc);
                SETVAL(state, WaitReqAccept);
            ELSE();
                SETVAL(req_addr, ALLONES());
                SETVAL(state, Idle);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    IF (NZ(i_flush_pipeline));
        TEXT("Clear pipeline stage");
        SETZERO(req_valid);
        SETVAL(pc, ALLONES());
        SETZERO(instr);
        SETZERO(instr_load_fault);
        SETZERO(instr_page_fault_x);
    ENDIF();

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_mem_addr_valid, req_valid);
    SETVAL(o_mem_addr, req_addr);
    SETVAL(o_mem_resp_ready, resp_ready);
    SETVAL(o_instr_load_fault, instr_load_fault);
    SETVAL(o_instr_page_fault_x, instr_page_fault_x);
    SETVAL(o_requested_pc, req_addr);
    SETVAL(o_fetching_pc, mem_resp_shadow);
    SETVAL(o_pc, pc);
    SETVAL(o_instr, instr);
}
