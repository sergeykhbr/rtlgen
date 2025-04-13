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

#include "dbg_port.h"

DbgPort::DbgPort(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "DbgPort", name, comment),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    _idbg0_(this, "\"RIVER\" Debug interface"),
    i_dport_req_valid(this, "i_dport_req_valid", "1", "Debug access from DSU is valid"),
    i_dport_type(this, "i_dport_type", "DPortReq_Total", "Debug access type"),
    i_dport_addr(this, "i_dport_addr", "RISCV_ARCH", "Debug address (register or memory)"),
    i_dport_wdata(this, "i_dport_wdata", "RISCV_ARCH", "Write value"),
    i_dport_size(this, "i_dport_size", "3", "reg/mem access size:0=1B;...,4=128B;"),
    o_dport_req_ready(this, "o_dport_req_ready", "1"),
    i_dport_resp_ready(this, "i_dport_resp_ready", "1", "ready to accepd response"),
    o_dport_resp_valid(this, "o_dport_resp_valid", "1", "Response is valid"),
    o_dport_resp_error(this, "o_dport_resp_error", "1", "Something wrong during command execution"),
    o_dport_rdata(this, "o_dport_rdata", "RISCV_ARCH", "Response value"),
    _csr0_(this, "CSR bus master interface:"),
    o_csr_req_valid(this, "o_csr_req_valid", "1", "Region 0: Access to CSR bank is enabled."),
    i_csr_req_ready(this, "i_csr_req_ready", "1"),
    o_csr_req_type(this, "o_csr_req_type", "CsrReq_TotalBits", "Region 0: CSR operation read/modify/write"),
    o_csr_req_addr(this, "o_csr_req_addr", "12", "Address of the sub-region register"),
    o_csr_req_data(this, "o_csr_req_data", "RISCV_ARCH", "Write data"),
    i_csr_resp_valid(this, "i_csr_resp_valid", "1"),
    o_csr_resp_ready(this, "o_csr_resp_ready", "1"),
    i_csr_resp_data(this, "i_csr_resp_data", "RISCV_ARCH", "Region 0: CSR read value"),
    i_csr_resp_exception(this, "i_csr_resp_exception", "1", "Exception on CSR access"),
    i_progbuf(this, "i_progbuf", "MUL(32,CFG_PROGBUF_REG_TOTAL)", "progam buffer"),
    o_progbuf_ena(this, "o_progbuf_ena", "1", "Execution from the progbuffer is in progress"),
    o_progbuf_pc(this, "o_progbuf_pc", "RISCV_ARCH", "prog buffer instruction counter"),
    o_progbuf_instr(this, "o_progbuf_instr", "64", "prog buffer instruction opcode"),
    i_csr_progbuf_end(this, "i_csr_progbuf_end", "1", "End of execution from progbuf"),
    i_csr_progbuf_error(this, "i_csr_progbuf_error", "1", "Exception is occured during progbuf execution"),
    o_ireg_addr(this, "o_ireg_addr", "6"),
    o_ireg_wdata(this, "o_ireg_wdata", "RISCV_ARCH", "Write data"),
    o_ireg_ena(this, "o_ireg_ena", "1", "Region 1: Access to integer register bank is enabled"),
    o_ireg_write(this, "o_ireg_write", "1", "Region 1: Integer registers bank write pulse"),
    i_ireg_rdata(this, "i_ireg_rdata", "RISCV_ARCH", "Region 1: Integer register read value"),
    o_mem_req_valid(this, "o_mem_req_valid", "1", "Type 2: request is valid"),
    i_mem_req_ready(this, "i_mem_req_ready", "1", "Type 2: memory request was accepted"),
    i_mem_req_error(this, "i_mem_req_error", "1", "Type 2: memory request is invalid and cannot be processed"),
    o_mem_req_write(this, "o_mem_req_write", "1", "Type 2: is write"),
    o_mem_req_addr(this, "o_mem_req_addr", "RISCV_ARCH", "Type 2: Debug memory request"),
    o_mem_req_size(this, "o_mem_req_size", "2", "Type 2: memory operation size: 0=1B; 1=2B; 2=4B; 3=8B"),
    o_mem_req_wdata(this, "o_mem_req_wdata", "RISCV_ARCH", "Type 2: memory write data"),
    i_mem_resp_valid(this, "i_mem_resp_valid", "1", "Type 2: response is valid"),
    i_mem_resp_error(this, "i_mem_resp_error", "1", "Type 2: response error (MPU or unmapped access)"),
    i_mem_resp_rdata(this, "i_mem_resp_rdata", "RISCV_ARCH", "Type 2: Memory response from memaccess module"),
    i_e_pc(this, "i_e_pc", "RISCV_ARCH", "Instruction pointer"),
    i_e_npc(this, "i_e_npc", "RISCV_ARCH", "Next Instruction pointer"),
    i_e_call(this, "i_e_call", "1", "pseudo-instruction CALL"),
    i_e_ret(this, "i_e_ret", "1", "pseudo-instruction RET"),
    i_e_memop_valid(this, "i_e_memop_valid", "1", "Memory request from executor"),
    i_m_valid(this, "i_m_valid", "1", "Memory request processed"),
    // param
    idle(this, "idle", "4", "0", NO_COMMENT),
    csr_region(this, "csr_region", "4", "1", NO_COMMENT),
    reg_bank(this, "reg_bank", "4", "2", NO_COMMENT),
    reg_stktr_cnt(this, "reg_stktr_cnt", "4", "3", NO_COMMENT),
    reg_stktr_buf_adr(this, "reg_stktr_buf_adr", "4", "4", NO_COMMENT),
    reg_stktr_buf_dat(this, "reg_stktr_buf_dat", "4", "5", NO_COMMENT),
    exec_progbuf_start(this, "exec_progbuf_start", "4", "6", NO_COMMENT),
    exec_progbuf_next(this, "exec_progbuf_next", "4", "7", NO_COMMENT),
    exec_progbuf_waitmemop(this, "exec_progbuf_waitmemop", "4", "8", NO_COMMENT),
    abstract_mem_request(this, "abstract_mem_request", "4", "9", NO_COMMENT),
    abstract_mem_response(this, "abstract_mem_response", "4", "10", NO_COMMENT),
    wait_to_accept(this, "wait_to_accept", "4", "11", NO_COMMENT),
    // signals
    wb_stack_raddr(this, "wb_stack_raddr", "CFG_LOG2_STACK_TRACE_ADDR"),
    wb_stack_rdata(this, "wb_stack_rdata", "MUL(2,RISCV_ARCH)"),
    w_stack_we(this, "w_stack_we", "1"),
    wb_stack_waddr(this, "wb_stack_waddr", "CFG_LOG2_STACK_TRACE_ADDR"),
    wb_stack_wdata(this, "wb_stack_wdata", "MUL(2,RISCV_ARCH)"),
    // registers
    dport_write(this, "dport_write", "1"),
    dport_addr(this, "dport_addr", "RISCV_ARCH", "'0", NO_COMMENT),
    dport_wdata(this, "dport_wdata", "RISCV_ARCH", "'0", NO_COMMENT),
    dport_rdata(this, "dport_rdata", "RISCV_ARCH", "'0", NO_COMMENT),
    dport_size(this, "dport_size", "2", "'0", NO_COMMENT),
    dstate(this, "dstate", "4", "idle"),
    rdata(this, "rdata", "RISCV_ARCH", "'0", NO_COMMENT),
    stack_trace_cnt(this, "stack_trace_cnt", "CFG_LOG2_STACK_TRACE_ADDR", "0", "Stack trace buffer counter"),
    req_accepted(this, "req_accepted", "1"),
    resp_error(this, "resp_error", "1"),
    progbuf_ena(this, "progbuf_ena", "1"),
    progbuf_pc(this, "progbuf_pc", "RISCV_ARCH", "'0", NO_COMMENT),
    progbuf_instr(this, "progbuf_instr", "64", "'0", NO_COMMENT),
    // process
    comb(this),
    trbuf0(this, "trbuf0")
{
    Operation::start(this);

    GENERATE("tb");
        IFGEN (NE(glob_river_cfg_->CFG_LOG2_STACK_TRACE_ADDR, CONST("0")), new StringConst("tracebuf_en"));
            NEW(trbuf0, trbuf0.getName().c_str());
                CONNECT(trbuf0, 0, trbuf0.i_clk, i_clk);
                CONNECT(trbuf0, 0, trbuf0.i_raddr, wb_stack_raddr);
                CONNECT(trbuf0, 0, trbuf0.o_rdata, wb_stack_rdata);
                CONNECT(trbuf0, 0, trbuf0.i_we, w_stack_we);
                CONNECT(trbuf0, 0, trbuf0.i_waddr, wb_stack_waddr);
                CONNECT(trbuf0, 0, trbuf0.i_wdata, wb_stack_wdata);
            ENDNEW();
        ENDIFGEN(new StringConst("tracebuf_en"));
    ENDGENERATE("tb");
}

void DbgPort::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;

    SETVAL(comb.vb_idx, BITS(i_dport_addr, 11, 0));
    SETVAL(comb.vrdata, dport_rdata);

TEXT();
    IF (NE(cfg->CFG_LOG2_STACK_TRACE_ADDR, CONST("0")));
        IF (AND2(NZ(i_e_call), 
            NE(stack_trace_cnt, DEC(cfg->STACK_TRACE_BUF_SIZE))));
            SETONE(comb.v_stack_we);
            SETVAL(comb.vb_stack_waddr, stack_trace_cnt);
            SETVAL(comb.vb_stack_wdata, CC2(i_e_npc, i_e_pc));
            SETVAL(stack_trace_cnt, INC(stack_trace_cnt));
        ELSIF (AND2(NZ(i_e_ret), NZ(stack_trace_cnt)));
            SETVAL(stack_trace_cnt, DEC(stack_trace_cnt));
        ENDIF();
    ENDIF();

TEXT();
    SWITCH (dstate);
    CASE(idle);
        SETONE(comb.v_req_ready);
        SETZERO(comb.vrdata);
        SETZERO(req_accepted);
        SETZERO(resp_error);
        SETZERO(progbuf_ena);
        IF (NZ(i_dport_req_valid));
            IF (NZ(BIT(i_dport_type, cfg->DPortReq_RegAccess)));
                SETVAL(dport_write, BIT(i_dport_type, cfg->DPortReq_Write));
                SETVAL(dport_addr, i_dport_addr);
                SETVAL(dport_wdata, i_dport_wdata);
                IF (EQ(BITS(i_dport_addr, 15, 12), CONST("0x0", 4)));
                    SETVAL(dstate, csr_region);
                ELSIF (EQ(BITS(i_dport_addr, 15, 12), CONST("0x1", 4)));
                    SETVAL(dstate, reg_bank);
                ELSIF (EQ(BITS(i_dport_addr, 15, 12), CONST("0xC", 4)));
                    TEXT("non-standard extension");
                    IF (EQ(comb.vb_idx, CONST("64", 12)));
                        SETVAL(dstate, reg_stktr_cnt);
                    ELSIF (AND2(GE(comb.vb_idx, CONST("128")), 
                                LS(comb.vb_idx, ADD2(CONST("128"), MUL2(CONST("2"), cfg->STACK_TRACE_BUF_SIZE)))));
                        SETVAL(dstate, reg_stktr_buf_adr);
                    ENDIF();
                ELSE();
                    SETVAL(dstate, wait_to_accept);
                ENDIF();
            ELSIF (NZ(BIT(i_dport_type, cfg->DPortReq_Progexec)));
                SETVAL(dstate, exec_progbuf_start);
            ELSIF (NZ(BIT(i_dport_type, cfg->DPortReq_MemAccess)));
                SETVAL(dstate, abstract_mem_request);
                SETVAL(dport_write, BIT(i_dport_type, cfg->DPortReq_Write));
                SETVAL(dport_addr, i_dport_addr);
                SETVAL(dport_wdata, i_dport_wdata);
                SETVAL(dport_size, BITS(i_dport_size, 1,0));
            ELSE();
                TEXT("Unsupported request");
                SETVAL(dstate, wait_to_accept);
                SETONE(resp_error);
            ENDIF();
        ENDIF();
    ENDCASE();
    CASE (csr_region);
        SETVAL(comb.v_csr_req_valid, INV(req_accepted));
        SETVAL(comb.v_csr_resp_ready, req_accepted);
        IF (AND2(EZ(req_accepted), NZ(i_csr_req_ready)));
            SETONE(req_accepted);
        ENDIF();
        IF (NZ(dport_write));
            SETVAL(comb.vb_csr_req_type, cfg->CsrReq_WriteCmd);
        ELSE();
            SETVAL(comb.vb_csr_req_type, cfg->CsrReq_ReadCmd);
        ENDIF();
        SETVAL(comb.vb_csr_req_addr, BITS(dport_addr, 11, 0));
        SETVAL(comb.vb_csr_req_data, dport_wdata);
        IF (NZ(AND2(req_accepted, i_csr_resp_valid)));
            SETVAL(comb.vrdata, i_csr_resp_data);
            SETVAL(dstate, wait_to_accept);
        ENDIF();
        ENDCASE();
    CASE (reg_bank);
        SETONE(comb.v_o_ireg_ena);
        SETVAL(comb.vb_o_ireg_addr, BITS(dport_addr,5,0));
        SETVAL(comb.vrdata, i_ireg_rdata);
        IF (NZ(dport_write));
            SETONE(comb.v_o_ireg_write);
            SETVAL(comb.vb_o_ireg_wdata, dport_wdata);
        ENDIF();
        SETVAL(dstate, wait_to_accept);
        ENDCASE();
    CASE (reg_stktr_cnt);
        SETZERO(comb.vrdata);
        SETBITS(comb.vrdata, DEC(cfg->CFG_LOG2_STACK_TRACE_ADDR), CONST("0"), stack_trace_cnt);
        IF (NZ(dport_write));
            SETVAL(stack_trace_cnt,
                BITS(dport_wdata, DEC(cfg->CFG_LOG2_STACK_TRACE_ADDR), CONST("0")));
        ENDIF();
        SETVAL(dstate, wait_to_accept);
        ENDCASE();
    CASE (reg_stktr_buf_adr);
        SETVAL(comb.vb_stack_raddr, BITS(dport_addr, cfg->CFG_LOG2_STACK_TRACE_ADDR, CONST("1")));
        SETVAL(dstate, reg_stktr_buf_dat);
        ENDCASE();
    CASE (reg_stktr_buf_dat);
        IF (EZ(BIT(dport_addr, 0)));
            SETVAL(comb.vrdata, BITS(wb_stack_rdata, DEC(cfg->RISCV_ARCH), CONST("0")));
        ELSE();
            SETVAL(comb.vrdata, BITS(wb_stack_rdata, DEC(MUL2(CONST("2"), cfg->RISCV_ARCH)), cfg->RISCV_ARCH));
        ENDIF();
        SETVAL(dstate, wait_to_accept);
        ENDCASE();
    CASE (exec_progbuf_start);
        SETONE(progbuf_ena);
        SETZERO(progbuf_pc);
        SETVAL(progbuf_instr, BIG_TO_U64(BITS(i_progbuf, 63, 0)));
        SETVAL(dstate, exec_progbuf_next);
        ENDCASE();
    CASE (exec_progbuf_next);
        IF (NZ(i_csr_progbuf_end));
            SETZERO(progbuf_ena);
            SETVAL(resp_error, i_csr_progbuf_error);
            SETVAL(dstate, wait_to_accept);
        ELSIF (NZ(i_e_memop_valid));
            SETVAL(dstate, exec_progbuf_waitmemop);
        ELSE();
            SETVAL(comb.t_idx, BITS(i_e_npc,5,2));
            SETVAL(progbuf_pc, CC2(CONST("0", 58), (CC2(BITS(i_e_npc, 5, 2), CONST("0",2)))));
            IF (EQ(comb.t_idx, CONST("0xf", 4)));
                SETVAL(progbuf_instr, BIG_TO_U64(CC2(CONST("0", 32),
                                                     BITS(i_progbuf, CONST("255"), CONST("224")))));
            ELSE();
                SETVAL(progbuf_instr, BIG_TO_U64(BITSW(i_progbuf, MUL2(CONST("32"), comb.t_idx),
                                                                 CONST("64"))));
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (exec_progbuf_waitmemop);
        IF (NZ(i_m_valid));
            SETVAL(dstate, exec_progbuf_next);
        ENDIF();
        ENDCASE();
    CASE (abstract_mem_request);
        SETONE(comb.v_mem_req_valid);
        IF (NZ(i_mem_req_ready));
            IF (NZ(i_mem_req_error));
                SETVAL(dstate, wait_to_accept);
                SETONE(resp_error);
                SETVAL(comb.vrdata, ALLONES());
            ELSE();
                SETVAL(dstate, abstract_mem_response);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (abstract_mem_response);
        SETVAL(comb.vrdata, i_mem_resp_rdata);
        IF (NZ(i_mem_resp_valid));
            SETVAL(dstate, wait_to_accept);
            SETVAL(resp_error, i_mem_resp_error);
        ENDIF();
        ENDCASE();
    CASE (wait_to_accept);
        SETONE(comb.v_resp_valid);
        IF (NZ(i_dport_resp_ready));
            SETVAL(dstate, idle);
        ENDIF();
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    SETVAL(dport_rdata, comb.vrdata);

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(wb_stack_raddr, comb.vb_stack_raddr);
    SETVAL(w_stack_we, comb.v_stack_we);
    SETVAL(wb_stack_waddr, comb.vb_stack_waddr);
    SETVAL(wb_stack_wdata, comb.vb_stack_wdata);

TEXT();
    SETVAL(o_csr_req_valid, comb.v_csr_req_valid);
    SETVAL(o_csr_req_type, comb.vb_csr_req_type);
    SETVAL(o_csr_req_addr, comb.vb_csr_req_addr);
    SETVAL(o_csr_req_data, comb.vb_csr_req_data);
    SETVAL(o_csr_resp_ready, comb.v_csr_resp_ready);
    SETVAL(o_ireg_addr, comb.vb_o_ireg_addr);
    SETVAL(o_ireg_wdata, comb.vb_o_ireg_wdata);
    SETVAL(o_ireg_ena, comb.v_o_ireg_ena);
    SETVAL(o_ireg_write, comb.v_o_ireg_write);
    SETVAL(o_mem_req_valid, comb.v_mem_req_valid);
    SETVAL(o_mem_req_write, dport_write);
    SETVAL(o_mem_req_addr, dport_addr);
    SETVAL(o_mem_req_wdata, dport_wdata);
    SETVAL(o_mem_req_size, dport_size);
    SETVAL(o_progbuf_ena, progbuf_ena);
    SETVAL(o_progbuf_pc, progbuf_pc);
    SETVAL(o_progbuf_instr, progbuf_instr);
    SETVAL(o_dport_req_ready , comb.v_req_ready);
    SETVAL(o_dport_resp_valid, comb.v_resp_valid);
    SETVAL(o_dport_resp_error, resp_error);
    SETVAL(o_dport_rdata, dport_rdata);
}
