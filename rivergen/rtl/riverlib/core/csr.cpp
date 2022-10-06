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

#include "csr.h"

CsrRegs::CsrRegs(GenObject *parent, const char *name) :
    ModuleObject(parent, "CsrRegs", name),
    hartid(this, "hartid", "0"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_sp(this, "i_sp", "RISCV_ARCH", "Stack Pointer for border control"),
    i_req_valid(this, "i_req_valid", "1", "Access to CSR request"),
    o_req_ready(this, "o_req_ready", "1", "CSR module is ready to accept request"),
    i_req_type(this, "i_req_type", "CsrReq_TotalBits", "Request type: [0]-read csr; [1]-write csr; [2]-change mode"),
    i_req_addr(this, "i_req_addr", "12", "Requested CSR address"),
    i_req_data(this, "i_req_data", "RISCV_ARCH", "CSR new value"),
    o_resp_valid(this, "o_resp_valid", "1", "CSR module Response is valid"),
    i_resp_ready(this, "i_resp_ready", "1", "Executor is ready to accept response"),
    o_resp_data(this, "o_resp_data", "RISCV_ARCH", "Responded CSR data"),
    o_resp_exception(this, "o_resp_exception", "1", "exception on CSR access"),
    i_e_halted(this, "i_e_halted", "1", "core is halted confirmation flag"),
    i_e_pc(this, "i_e_pc", "CFG_CPU_ADDR_BITS", "current latched instruction pointer in executor"),
    i_e_instr(this, "i_e_instr", "32", "current latched opcode in executor"),
    i_irq_pending(this, "i_irq_pending", "IRQ_TOTAL", "Per Hart pending interrupts pins"),
    o_irq_pending(this, "o_irq_pending", "IRQ_TOTAL", "Enabled and Unmasked interrupt pending bits"),
    o_wakeup(this, "o_wakeup", "1", "There's pending bit even if interrupts globally disabled"),
    o_stack_overflow(this, "o_stack_overflow", "1", "stack overflow exception"),
    o_stack_underflow(this, "o_stack_underflow", "1", "stack underflow exception"),
    i_f_flush_ready(this, "i_f_flush_ready", "1", "fetcher is ready to accept Flush $I request"),
    i_e_valid(this, "i_e_valid", "1", "instructuin executed flag"),
    i_m_memop_ready(this, "i_m_memop_ready", "1", "memaccess module is ready to accept the request"),
    i_flushd_end(this, "i_flushd_end", "1"),
    i_mtimer(this, "i_mtimer", "64", "Read-only shadow value of memory-mapped mtimer register (see CLINT)."),
    o_executed_cnt(this, "o_executed_cnt", "64", "Number of executed instructions"),
    _io0_(this),
    o_step(this, "o_step", "1", "Stepping enabled"),
    i_dbg_progbuf_ena(this, "i_dbg_progbuf_ena", "1", "Executing progbuf is in progress"),
    o_progbuf_end(this, "o_progbuf_end", "1", "End of execution from prog buffer"),
    o_progbuf_error(this, "o_progbuf_error", "1", "exception during progbuf execution"),
    o_flushd_valid(this, "o_flushd_valid", "1", "clear specified addr in DCache"),
    o_flushi_valid(this, "o_flushi_valid", "1", "clear specified addr in ICache"),
    o_flushmmu_valid(this, "o_flushmmu_valid", "1", "clear specific leaf entry in MMU"),
    o_flush_addr(this, "o_flush_addr", "CFG_CPU_ADDR_BITS", "Cache address to flush. All ones means flush all."),
    _io1_(this),
    o_mpu_region_we(this, "o_mpu_region_we", "1", "write enable into MPU"),
    o_mpu_region_idx(this, "o_mpu_region_idx", "CFG_MPU_TBL_WIDTH", "selected MPU region"),
    o_mpu_region_addr(this, "o_mpu_region_addr", "CFG_CPU_ADDR_BITS", "MPU region base address"),
    o_mpu_region_mask(this, "o_mpu_region_mask", "CFG_CPU_ADDR_BITS", "MPU region mask"),
    o_mpu_region_flags(this, "o_mpu_region_flags", "CFG_MPU_FL_TOTAL", "{ena, cachable, r, w, x}"),
    _io2_(this),
    o_immu_ena(this, "o_immu_ena", "1", "Instruction MMU enabled in U and S modes. Sv48 only."),
    o_dmmu_ena(this, "o_dmmu_ena", "1", "Data MMU enabled in U and S modes or MPRV bit is HIGH. Sv48 only."),
    o_mmu_ppn(this, "o_mmu_ppn", "44", "Physical Page Number"),
    // param
    State_Idle(this, "State_Idle", "0"),
    State_RW(this, "State_RW", "1"),
    State_Exception(this, "State_Exception", "2"),
    State_Breakpoint(this, "State_Breakpoint", "3"),
    State_Interrupt(this, "State_Interrupt", "4"),
    State_TrapReturn(this, "State_TrapReturn", "5"),
    State_Halt(this, "State_Halt", "6"),
    State_Resume(this, "State_Resume", "7"),
    State_Wfi(this, "State_Wfi", "8"),
    State_Fence(this, "State_Fence", "9"),
    State_Response(this, "State_Response", "10"),
    _fence0_(this),
    Fence_None(this, "3", "Fence_None", "0"),
    Fence_Data(this, "3", "Fence_Data", "1"),
    Fence_DataWaitEnd(this, "3", "Fence_DataWaitEnd", "2"),
    Fence_Fetch(this, "3", "Fence_Fetch", "3"),
    Fence_MMU(this, "3", "Fence_MMU", "4"),
    Fence_End(this, "3", "Fence_End", "5"),
    _fence1_(this),
    SATP_MODE_SV48(this, "4", "SATP_MODE_SV48", "9", "48-bits Page mode"),
    // struct definitions
    RegModeTypeDef_(this),
    // registers
    xmode(this, "xmode"),
    state(this, "state", "4", "State_Idle"),
    fencestate(this, "fencestate", "3", "Fence_None"),
    irq_pending(this, "irq_pending", "IRQ_TOTAL"),
    cmd_type(this, "cmd_type", "CsrReq_TotalBits"),
    cmd_addr(this, "cmd_addr", "12"),
    cmd_data(this, "cmd_data", "RISCV_ARCH"),
    cmd_exception(this, "cmd_exception", "1", "0", "exception on CSR access"),
    progbuf_end(this, "progbuf_end", "1"),
    progbuf_err(this, "progbuf_err", "1"),
    mip_ssip(this, "mip_ssip", "1", "0", "page 34: SSIP is writable to re-request SW irq from machine to supervisor"),
    mip_stip(this, "mip_stip", "1", "0", "page 34: SSIP is writable in mip"),
    mip_seip(this, "mip_seip", "1", "0", "page 34: SSIP is writable in mip"),
    medeleg(this, "medeleg", "64"),
    mideleg(this, "mideleg", "IRQ_TOTAL"),
    mcountinhibit(this, "mcountinhibit", "32", "0", "When non zero stop specified performance counter"),
    mstackovr(this, "mstackovr", "CFG_CPU_ADDR_BITS"),
    mstackund(this, "mstackund", "CFG_CPU_ADDR_BITS"),
    mpu_addr(this, "mpu_addr", "CFG_CPU_ADDR_BITS"),
    mpu_mask(this, "mpu_mask", "CFG_CPU_ADDR_BITS"),
    mpu_idx(this, "mpu_idx", "CFG_MPU_TBL_WIDTH"),
    mpu_flags(this, "mpu_flags", "CFG_MPU_FL_TOTAL"),
    mpu_we(this, "mpu_we", "1"),
    immu_ena(this, "immu_ena", "1", "0", "Instruction MMU SV48 enabled in U- and S- modes"),
    dmmu_ena(this, "dmmu_ena", "1", "0", "Data MMU SV48 enabled in U- and S- modes, MPRV bit"),
    satp_ppn(this, "satp_ppn", "44", "0", "Physcal Page Number"),
    satp_mode(this, "satp_mode", "4", "0", "Supervisor Address Translation and Protection mode"),
    mode(this, "mode", "2", "PRV_M"),
    mprv(this, "mprv", "1", "0", "Modify PRiVilege. (Table 8.5) If MPRV=0, load and stores as normal, when MPRV=1, use translation of previous mode"),
    tvm(this, "tvm", "1", "0", "Trap Virtual Memory bit. When 1 SFENCE.VMA or SINVAL.VMA or rw access to SATP raise an illegal instruction"),
    ex_fpu_invalidop(this, "ex_fpu_invalidop", "1", "0", "FPU Exception: invalid operation"),
    ex_fpu_divbyzero(this, "ex_fpu_divbyzero", "1", "0", "FPU Exception: divide by zero"),
    ex_fpu_overflow(this, "ex_fpu_overflow", "1", "0", "FPU Exception: overflow"),
    ex_fpu_underflow(this, "ex_fpu_underflow", "1", "0", "FPU Exception: underflow"),
    ex_fpu_inexact(this, "ex_fpu_inexact", "1", "0", "FPU Exception: inexact"),
    trap_addr(this, "trap_addr", "CFG_CPU_ADDR_BITS"),
    mcycle_cnt(this, "mcycle_cnt", "64", "0", "Cycle in clocks."),
    minstret_cnt(this, "minstret_cnt", "64", "0", "Number of the instructions the hart has retired"),
    dscratch0(this, "dscratch0", "RISCV_ARCH"),
    dscratch1(this, "dscratch1", "RISCV_ARCH"),
    dpc(this, "dpc", "CFG_CPU_ADDR_BITS", "CFG_RESET_VECTOR"),
    halt_cause(this, "halt_cause", "3", "0", "1=ebreak instruction; 2=breakpoint exception; 3=haltreq; 4=step"),
    dcsr_ebreakm(this, "dcsr_ebreakm", "1", "0", "Enter or not into Debug Mode on EBREAK instruction"),
    dcsr_stopcount(this, "dcsr_stopcount", "1"),
    dcsr_stoptimer(this, "dcsr_stoptimer", "1"),
    dcsr_step(this, "dcsr_step", "1"),
    dcsr_stepie(this, "dcsr_stepie", "1", "0", "interrupt 0=dis;1=ena during stepping"),
    stepping_mode_cnt(this, "stepping_mode_cnt", "RISCV_ARCH", "0"),
    ins_per_step(this, "ins_per_step", "RISCV_ARCH", "1", "Number of steps before halt in stepping mode"),
    //flushi_ena(this, "flushi_ena", "1"),
    //flushi_addr(this, "flushi_addr", "CFG_CPU_ADDR_BITS"),
    // process
    comb(this)
{
}

void CsrRegs::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;
    SETZERO(mpu_we);
    //SETVAL(dbg_e_valid, i_e_valid, "used in RtlWrapper to count executed instructions");
    SETVAL(comb.vb_xpp, ARRITEM(xmode, TO_INT(mode), xmode->xpp));

TEXT();
    SETVAL(comb.vb_xtvec_off_edeleg, ARRITEM(xmode, comb.iM, xmode->xtvec_off));
    IF (AND2(LE(mode, cfg->PRV_S), NZ(BIT(medeleg, TO_INT(BITS(cmd_addr, 4, 0))))));
        TEXT("Exception delegation to S-mode");
        SETONE(comb.v_medeleg_ena);
        SETVAL(comb.vb_xtvec_off_edeleg, ARRITEM(xmode, comb.iS, xmode->xtvec_off));
    ENDIF();

TEXT();
    SETVAL(comb.vb_xtvec_off_ideleg, ARRITEM(xmode, comb.iM, xmode->xtvec_off));
    IF (AND2(LE(mode, cfg->PRV_S), NZ(BIT(mideleg, TO_INT(BITS(cmd_addr, 3, 0))))));
        TEXT("Interrupt delegation to S-mode");
        SETONE(comb.v_mideleg_ena);
        SETVAL(comb.vb_xtvec_off_ideleg, ARRITEM(xmode, comb.iS, xmode->xtvec_off));
    ENDIF();

TEXT();
    SWITCH (state);
    CASE(State_Idle);
        SETZERO(progbuf_end);
        SETZERO(progbuf_err);
        SETONE(comb.v_req_ready);
        IF (NZ(i_req_valid));
            SETVAL(cmd_type, i_req_type);
            SETVAL(cmd_addr, i_req_addr);
            SETVAL(cmd_data, i_req_data);
            SETZERO(cmd_exception);
            IF (NZ(BIT(i_req_type, cfg->CsrReq_ExceptionBit)));
                SETVAL(state, State_Exception);
            ELSIF(NZ(BIT(i_req_type, cfg->CsrReq_BreakpointBit)));
                SETVAL(state, State_Breakpoint);
            ELSIF (NZ(BIT(i_req_type, cfg->CsrReq_HaltBit)));
                SETVAL(state, State_Halt);
            ELSIF (NZ(BIT(i_req_type, cfg->CsrReq_ResumeBit)));
                SETVAL(state, State_Resume);
            ELSIF (NZ(BIT(i_req_type, cfg->CsrReq_InterruptBit)));
                SETVAL(state, State_Interrupt);
            ELSIF (NZ(BIT(i_req_type, cfg->CsrReq_TrapReturnBit)));
                SETVAL(state, State_TrapReturn);
            ELSIF (NZ(BIT(i_req_type, cfg->CsrReq_WfiBit)));
                SETVAL(state, State_Wfi);
            ELSIF (NZ(BIT(i_req_type, cfg->CsrReq_FenceBit)));
                SETVAL(state, State_Fence);
                IF (NZ(BITS(i_req_addr, 1, 0)));
                    TEXT("FENCE or FENCE.I");
                    SETVAL(fencestate, Fence_Data);
                ELSIF(AND3(NZ(BIT(i_req_addr,2)), EZ(tvm), NE(mode, cfg->PRV_S)));
                    TEXT("FENCE.VMA: is illegal in S-mode when TVM bit=1");
                    SETVAL(fencestate, Fence_MMU);
                ELSE();
                    TEXT("Illegal fence");
                    SETVAL(state, State_Response);
                    SETONE(cmd_exception);
                ENDIF();
            ELSE();
                SETVAL(state, State_RW);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (State_Exception);
        SETVAL(state, State_Response);
        SETBITONE(comb.vb_e_emux, TO_INT(BITS(cmd_addr, 4, 0)));
        SETVAL(comb.vb_xtval, cmd_data);
        SETVAL(comb.wb_trap_cause, BITS(cmd_addr, 4, 0));
        SETVAL(cmd_data, comb.vb_xtvec_off_edeleg);
        IF (NZ(i_dbg_progbuf_ena));
            SETONE(progbuf_err);
            SETONE(progbuf_end);
            SETONE(cmd_exception);
        ENDIF();
        IF (EQ(cmd_addr, cfg->EXCEPTION_CallFromXMode));
            SETVAL(comb.wb_trap_cause, ADD2(cmd_addr, mode));
        ENDIF();
        ENDCASE();
    CASE (State_Breakpoint, "software breakpoint");
        SETVAL(state, State_Response);
        IF (NZ(i_dbg_progbuf_ena));
            TEXT("do not modify halt cause in debug mode");
            SETONE(progbuf_end);
            SETVAL(cmd_data, ALLONES(), "signal to executor to switch into Debug Mode and halt");
        ELSIF (NZ(dcsr_ebreakm));
            SETVAL(halt_cause, cfg->HALT_CAUSE_EBREAK);
            SETVAL(dpc, cmd_data);
            SETVAL(cmd_data, ALLONES(), "signal to executor to switch into Debug Mode and halt");
        ELSE();
            SETBITONE(comb.vb_e_emux, cfg->EXCEPTION_Breakpoint);
            SETVAL(comb.wb_trap_cause, BITS(cmd_addr, 4, 0));
            SETVAL(comb.vb_xtval, i_e_pc);
            SETVAL(cmd_data, comb.vb_xtvec_off_edeleg, "Jump to exception handler");
        ENDIF();
        ENDCASE();
    CASE (State_Halt);
        SETVAL(state, State_Response);
        SETVAL(halt_cause, BITS(cmd_addr, 2, 0), "Halt Request or Step done");
        SETVAL(dpc, i_e_pc);
        ENDCASE();
    CASE (State_Resume);
        SETVAL(state, State_Response);
        IF (NZ(i_dbg_progbuf_ena));
            SETZERO(cmd_data);
        ELSE();
            SETVAL(cmd_data, dpc);
        ENDIF();
        ENDCASE();
    CASE (State_Interrupt);
        SETVAL(state, State_Response);
        SETBITONE(comb.vb_e_imux, TO_INT(BITS(cmd_addr, 3, 0)));
        SETVAL(comb.wb_trap_cause, BITS(cmd_addr, 4, 0));
        SETVAL(cmd_data, comb.vb_xtvec_off_ideleg);
        IF (EQ(ARRITEM_B(xmode, TO_INT(mode), xmode->xtvec_mode), CONST("1", 2)));
            TEXT("vectorized");
            SETVAL(cmd_data, ADD2(comb.vb_xtvec_off_ideleg,
                                  CC2(comb.wb_trap_cause, CONST("0", 2))));
        ENDIF();
        ENDCASE();
    CASE (State_TrapReturn);
        SETVAL(state, State_Response);
        SETONE(comb.v_csr_trapreturn);
        SETVAL(cmd_data, ARRITEM(xmode, TO_INT(mode), xmode->xepc));
        ENDCASE();
    CASE (State_RW);
        SETVAL(state, State_Response);
        TEXT("csr[9:8] encode the loweset priviledge level that can access to CSR");
        TEXT("csr[11:10] register is read/write (00, 01 or 10) or read-only (11)");
        IF (LS(mode, BITS(cmd_addr,9,8)));
            TEXT("Not enough priv to access this register");
            SETONE(cmd_exception);
        ELSE ();
            SETVAL(comb.v_csr_rena, BIT(cmd_type, cfg->CsrReq_ReadBit));
            SETVAL(comb.v_csr_wena, BIT(cmd_type, cfg->CsrReq_WriteBit));
        ENDIF();
        TEXT("All operation into CSR implemented through the Read-Modify-Write");
        TEXT("and we cannot generate exception on write access into read-only regs");
        TEXT("So do not check bits csr[11:10], otherwise always will be the exception.");
        ENDCASE();
    CASE (State_Wfi);
        SETVAL(state, State_Response);
        SETZERO(cmd_data, "no error, valid for all mdoes");
        ENDCASE();
    CASE (State_Fence);
        IF (EQ(fencestate, Fence_End));
            SETZERO(cmd_data);
            SETVAL(state, State_Response);
            SETVAL(fencestate, Fence_None);
        ENDIF();
        ENDCASE();
    CASE (State_Response);
        SETONE(comb.v_resp_valid);
        IF (NZ(i_resp_ready));
            SETVAL(state, State_Idle);
        ENDIF();
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    TEXT("Caches flushing state machine");
    SWITCH (fencestate);
    CASE (Fence_None);
        ENDCASE();
    CASE (Fence_Data);
        SETONE(comb.v_flushd);
        IF(NZ(i_m_memop_ready));
            SETVAL(fencestate, Fence_DataWaitEnd);
        ENDIF();
        ENDCASE();
    CASE (Fence_DataWaitEnd);
        IF(NZ(i_flushd_end));
            TEXT("[0] flush data");
            TEXT("[1] flush fetch");
            TEXT("[2] flush mmu");
            IF (NZ(BIT(cmd_addr, 1)));
                SETVAL(fencestate, Fence_Fetch);
            ELSIF (NZ(BIT(cmd_addr, 2)));
                SETVAL(fencestate, Fence_MMU);
            ELSE();
                SETVAL(fencestate, Fence_End);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (Fence_Fetch);
        SETONE(comb.v_flushi);
        IF (NZ(i_f_flush_ready));
            IF (NZ(BIT(cmd_addr, 2)));
                SETVAL(fencestate, Fence_MMU);
            ELSE();
                SETVAL(fencestate, Fence_End);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (Fence_MMU);
        SETONE(comb.v_flushmmu);
        SETVAL(fencestate, Fence_End);
        ENDCASE();
    CASE (Fence_End);
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    TEXT("CSR registers. Priviledge Arch. V20211203, page 9(19):");
    TEXT("    CSR[11:10] indicate whether the register is read/write (00, 01, or 10) or read-only (11)"),
    TEXT("    CSR[9:8] encode the lowest privilege level that can access the CSR"),
    SWITCH (cmd_addr);
    // User Trap Setup
    CASE (CONST("0x000", 12), "ustatus: [URW] User status register");
        ENDCASE();
    CASE (CONST("0x004", 12), "uie: [URW] User interrupt-enable register");
        ENDCASE();
    CASE (CONST("0x005", 12), "ustatus: [URW] User trap handler base address");
        ENDCASE();
    // User Trap handling
    CASE (CONST("0x040", 12), "uscratch: [URW] Scratch register for user trap handlers");
        ENDCASE();
    CASE (CONST("0x041", 12), "uepc: [URW] User exception program counter");
        SETVAL(comb.vb_rdata, ARRITEM(xmode, comb.iU, xmode->xepc));
        IF (comb.v_csr_wena);
            SETARRITEM(xmode, comb.iU, xmode->xepc, BITS(cmd_data, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0")));
        ENDIF();
        ENDCASE();
    CASE (CONST("0x042", 12), "ucause: [URW] User trap cause");
        ENDCASE();
    CASE (CONST("0x043", 12), "utval: [URW] User bad address or instruction");
        ENDCASE();
    CASE (CONST("0x044", 12), "uip: [URW] User interrupt pending");
        ENDCASE();
    // User Floating-Point
    CASE (CONST("0x001", 12), "fflags: [URW] Floating-Point Accrued Exceptions");
        SETBIT(comb.vb_rdata, 0, ex_fpu_inexact);
        SETBIT(comb.vb_rdata, 1, ex_fpu_underflow);
        SETBIT(comb.vb_rdata, 2, ex_fpu_overflow);
        SETBIT(comb.vb_rdata, 3, ex_fpu_divbyzero);
        SETBIT(comb.vb_rdata, 4, ex_fpu_invalidop);
        ENDCASE();
    CASE (CONST("0x002", 12), "fflags: [URW] Floating-Point Dynamic Rounding Mode");
        IF (cfg->CFG_HW_FPU_ENABLE);
            SETBITS(comb.vb_rdata, 2, 0, CONST("4", 3), "Round mode: round to Nearest (RMM)");
        ENDIF();
        ENDCASE();
    CASE (CONST("0x003", 12), "fcsr: [URW] Floating-Point Control and Status Register (frm + fflags)");
        SETBIT(comb.vb_rdata, 0, ex_fpu_inexact);
        SETBIT(comb.vb_rdata, 1, ex_fpu_underflow);
        SETBIT(comb.vb_rdata, 2, ex_fpu_overflow);
        SETBIT(comb.vb_rdata, 3, ex_fpu_divbyzero);
        SETBIT(comb.vb_rdata, 4, ex_fpu_invalidop);
        IF (cfg->CFG_HW_FPU_ENABLE);
            SETBITS(comb.vb_rdata, 7, 5, CONST("4", 3), "Round mode: round to Nearest (RMM)");
        ENDIF();
        ENDCASE();
    // User Counter/Timers
    CASE (CONST("0xC00", 12), "cycle: [URO] User Cycle counter for RDCYCLE pseudo-instruction");
        IF (ANDx(2, &EQ(mode, cfg->PRV_U), 
                    &ORx(2, &EZ(BIT(ARRITEM_B(xmode, comb.iM, xmode->xcounteren), 0)),
                            &EZ(BIT(ARRITEM_B(xmode, comb.iS, xmode->xcounteren), 0)))));
            TEXT("Available only if all more prv. bit CY are set");
            SETONE(cmd_exception);
        ELSIF (ANDx(2, &EQ(mode, cfg->PRV_S),
                       &EZ(BIT(ARRITEM_B(xmode, comb.iM, xmode->xcounteren), 0))));
            TEXT("Available only if bit CY is set");
            SETONE(cmd_exception);
        ELSE();
            SETVAL(comb.vb_rdata, mcycle_cnt, "Read-only shadows of mcycle");
        ENDIF();
        ENDCASE();
    CASE (CONST("0xC01", 12), "time: [URO] User Timer for RDTIME pseudo-instruction");
        IF (ANDx(2, &EQ(mode, cfg->PRV_U), 
                    &ORx(2, &EZ(BIT(ARRITEM_B(xmode, comb.iM, xmode->xcounteren), 1)),
                            &EZ(BIT(ARRITEM_B(xmode, comb.iS, xmode->xcounteren), 1)))));
            TEXT("Available only if all more prv. bit TM are set");
            SETONE(cmd_exception);
        ELSIF (ANDx(2, &EQ(mode, cfg->PRV_S),
                       &EZ(BIT(ARRITEM_B(xmode, comb.iM, xmode->xcounteren), 1))));
            TEXT("Available only if bit TM is set");
            SETONE(cmd_exception);
        ELSE();
            SETVAL(comb.vb_rdata, i_mtimer);
        ENDIF();
        ENDCASE();
    CASE (CONST("0xC03", 12), "insret: [URO] User Instructions-retired counter for RDINSTRET pseudo-instruction");
        IF (ANDx(2, &EQ(mode, cfg->PRV_U),
                    &ORx(2, &EZ(BIT(ARRITEM_B(xmode, comb.iM, xmode->xcounteren), 2)),
                            &EZ(BIT(ARRITEM_B(xmode, comb.iS, xmode->xcounteren), 2)))));
            TEXT("Available only if all more prv. bit IR are set");
            SETONE(cmd_exception);
        ELSIF (ANDx(2, &EQ(mode, cfg->PRV_S),
                       &EZ(BIT(ARRITEM_B(xmode, comb.iM, xmode->xcounteren), 2))));
            TEXT("Available only if bit IR is set");
            SETONE(cmd_exception);
        ELSE();
            SETVAL(comb.vb_rdata, minstret_cnt, "Read-only shadow of minstret");
        ENDIF();
        ENDCASE();
    // Supervisor Trap Setup
    CASE (CONST("0x100", 12), "sstatus: [SRW] Supervisor status register");
        TEXT("[0] WPRI");
        SETBIT(comb.vb_rdata, 1, ARRITEM(xmode, comb.iS, xmode->xie));
        TEXT("[4:2] WPRI");
        SETBIT(comb.vb_rdata, 5, ARRITEM(xmode, comb.iS, xmode->xpie));
        TEXT("[6] UBE: Endianess: 0=little-endian; 1=big-endian. Instruction fetch is always little endian");
        TEXT("[7] WPRI");
        SETBIT(comb.vb_rdata, 8, ARRITEM(xmode, comb.iS, BIT(xmode->xpp, 0)), "SPP can have onle 0 or 1 values, so 1 bit only");
        TEXT("[10:9] VS");
        TEXT("[12:11] WPRI");
        IF (cfg->CFG_HW_FPU_ENABLE);
            SETBITS(comb.vb_rdata, 14, 13, CONST("0x1", 2), "FS field: Initial state");
        ENDIF();
        TEXT("[16:15] XS");
        TEXT("[17] WPRI");
        TEXT("[18] SUM");
        TEXT("[19] MXR");
        TEXT("[31:20] WPRI");
        SETBITS(comb.vb_rdata, 33, 32, CONST("0x2", 2), "UXL: User is 64-bits");
        TEXT("[62:34] WPRI");
        TEXT("[63] SD. Read-only bit that summize FS, VS or XS fields");
        IF (NZ(comb.v_csr_wena));
            SETARRITEM(xmode, comb.iS, xmode->xie, BIT(cmd_data, 1));
            SETARRITEM(xmode, comb.iS, xmode->xpie, BIT(cmd_data, 5));
            SETARRITEM(xmode, comb.iS, xmode->xpp, CC2(CONST("0", 1), BIT(cmd_data, 8)));
        ENDIF();
        ENDCASE();
    CASE (CONST("0x104", 12), "sie: [SRW] Supervisor interrupt-enable register");
        SETBIT(comb.vb_rdata, DEC(cfg->IRQ_SSIP), ARRITEM(xmode, comb.iS, xmode->xsie));
        SETBIT(comb.vb_rdata, DEC(cfg->IRQ_STIP), ARRITEM(xmode, comb.iS, xmode->xtie));
        SETBIT(comb.vb_rdata, DEC(cfg->IRQ_SEIP), ARRITEM(xmode, comb.iS, xmode->xeie));
        IF (comb.v_csr_wena);
            TEXT("Write only supported interrupts");
            SETARRITEM(xmode, comb.iS, xmode->xsie, BIT(cmd_data, cfg->IRQ_SSIP));
            SETARRITEM(xmode, comb.iS, xmode->xtie, BIT(cmd_data, cfg->IRQ_STIP));
            SETARRITEM(xmode, comb.iS, xmode->xeie, BIT(cmd_data, cfg->IRQ_SEIP));
        ENDIF();
        ENDCASE();
    CASE (CONST("0x105", 12), "stvec: [SRW] Supervisor trap handler base address");
        SETVAL(comb.vb_rdata, ARRITEM(xmode, comb.iS, xmode->xtvec_off));
        SETBITS(comb.vb_rdata, 1, 0, ARRITEM(xmode, comb.iS, xmode->xtvec_mode));
        IF (NZ(comb.v_csr_wena));
            SETARRITEM(xmode, comb.iS, xmode->xtvec_off,
                CC2(BITS(cmd_data, DEC(cfg->RISCV_ARCH), CONST("2")), CONST("0", 2)));
            SETARRITEM(xmode, comb.iS, xmode->xtvec_mode, BITS(cmd_data, 1, 0));
        ENDIF();
        ENDCASE();
    CASE (CONST("0x106", 12), "scounteren: [SRW] Supervisor counter enable");
        SETVAL(comb.vb_rdata, ARRITEM(xmode, comb.iS, xmode->xcounteren));
        IF (NZ(comb.v_csr_wena));
            SETARRITEM(xmode, comb.iS, xmode->xcounteren, BITS(cmd_data, 15, 0));
        ENDIF();
        ENDCASE();
    // Supervisor configuration
    CASE (CONST("0x10A", 12), "senvcfg: [SRW] Supervisor environment configuration register");
        ENDCASE();
    // Supervisor trap handling
    CASE (CONST("0x140", 12), "sscratch: [SRW] Supervisor register for supervisor trap handlers");
        SETVAL(comb.vb_rdata, ARRITEM(xmode, comb.iS, xmode->xscratch));
        IF (comb.v_csr_wena);
            SETARRITEM(xmode, comb.iS, xmode->xscratch, cmd_data);
        ENDIF();
        ENDCASE();
    CASE (CONST("0x141", 12), "sepc: [SRW] Supervisor exception program counter");
        SETVAL(comb.vb_rdata, ARRITEM(xmode, comb.iS, xmode->xepc));
        IF (comb.v_csr_wena);
            SETARRITEM(xmode, comb.iS, xmode->xepc, BITS(cmd_data, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0")));
        ENDIF();
        ENDCASE();
    CASE (CONST("0x142", 12), "scause: [SRW] Supervisor trap cause");
        SETBIT(comb.vb_rdata, 63, ARRITEM(xmode, comb.iS, xmode->xcause_irq));
        SETBITS(comb.vb_rdata, 4, 0, ARRITEM(xmode, comb.iS, xmode->xcause_code));
        ENDCASE();
    CASE (CONST("0x143", 12), "stval: [SRW] Supervisor bad address or instruction");
        SETVAL(comb.vb_rdata, ARRITEM(xmode, comb.iS, xmode->xtval));
        IF (comb.v_csr_wena);
            SETARRITEM(xmode, comb.iS, xmode->xtval, cmd_data);
        ENDIF();
        ENDCASE();
    CASE (CONST("0x144", 12), "sip: [SRW] Supervisor interrupt pending");
        SETBITS(comb.vb_rdata, 15, 0, AND2_L(irq_pending, CONST("0x0222", 16)), "see fig 4.7. Only s-bits are visible");
        IF (comb.v_csr_wena);
            SETVAL(mip_ssip, BIT(cmd_data, cfg->IRQ_SSIP));
            SETVAL(mip_stip, BIT(cmd_data, cfg->IRQ_STIP));
            SETVAL(mip_seip, BIT(cmd_data, cfg->IRQ_SEIP));
        ENDIF();
        ENDCASE();
    // Supervisor Proction and Translation
    CASE (CONST("0x180", 12), "satp: [SRW] Supervisor address translation and protection");
        TEXT("Writing unssoprted MODE[63:60], entire write has no effect");
        TEXT("    MODE = 0 Bare. No translation or protection");
        TEXT("    MODE = 9 Sv48. Page based 48-bit virtual addressing");
        IF (AND2(NZ(tvm), EQ(mode, cfg->PRV_S)));
            TEXT("SATP is illegal in S-mode when TVM=1");
            SETONE(cmd_exception);
        ELSE();
            SETBITS(comb.vb_rdata, 43, 0, satp_ppn);
            SETBITS(comb.vb_rdata, 63, 60, satp_mode);
            IF (ANDx(2, &NZ(comb.v_csr_wena),
                        &ORx(2, &EZ(BITS(cmd_data, 63, 60)),
                                &EQ(TO_U32(BITS(cmd_data, 63, 60)), SATP_MODE_SV48))));
                SETVAL(satp_ppn, BITS(cmd_data, 43, 0));
                SETVAL(satp_mode, BITS(cmd_data, 63, 60));
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (CONST("0x5A8", 12), "scontext: [SRW] Supervisor-mode context register");
        ENDCASE();
    // Hypervisor not implemented

    // Machine Information Registers
    CASE (CONST("0xF11", 12), "mvendorid: [MRO] Vendor ID");
        SETVAL(comb.vb_rdata, cfg->CFG_VENDOR_ID);
        ENDCASE();
    CASE (CONST("0xF12", 12), "marchid: [MRO] Architecture ID");
        ENDCASE();
    CASE (CONST("0xF13", 12), "mimplementationid: [MRO] Implementation ID");
        SETVAL(comb.vb_rdata, cfg->CFG_IMPLEMENTATION_ID);
        ENDCASE();
    CASE (CONST("0xF14", 12), "mhartid: [MRO] Hardware thread ID");
        SETBITS(comb.vb_rdata, 63, 0, hartid);
        ENDCASE();
    CASE (CONST("0xF15", 12), "mconfigptr: [MRO] Pointer to configuration data structure");
        ENDCASE();
    // Machine Trap setup
    CASE (CONST("0x300", 12), "mstatus: [MRW] Machine mode status register");
        TEXT("[0] WPRI");
        SETBIT(comb.vb_rdata, 1, ARRITEM(xmode, comb.iS, xmode->xie));
        TEXT("[2] WPRI");
        SETBIT(comb.vb_rdata, 3, ARRITEM(xmode, comb.iM, xmode->xie));
        TEXT("[4] WPRI");
        SETBIT(comb.vb_rdata, 5, ARRITEM(xmode, comb.iS, xmode->xpie));
        TEXT("[6] UBE: Endianess: 0=little-endian; 1=big-endian. Instruction fetch is always little endian");
        SETBIT(comb.vb_rdata, 7, ARRITEM(xmode, comb.iM, xmode->xpie));
        SETBIT(comb.vb_rdata, 8, ARRITEM(xmode, comb.iS, BIT(xmode->xpp, 0)), "SPP can have onle 0 or 1 values, so 1 bit only");
        TEXT("[10:9] VS");
        SETBITS(comb.vb_rdata, 12, 11, ARRITEM(xmode, comb.iM, xmode->xpp));
        IF (cfg->CFG_HW_FPU_ENABLE);
            SETBITS(comb.vb_rdata, 14, 13, CONST("0x1", 2), "FS field: Initial state");
        ENDIF();
        TEXT("[16:15] XS");
        SETBIT(comb.vb_rdata, 17, mprv);
        TEXT("[18] SUM");
        TEXT("[19] MXR");
        SETBIT(comb.vb_rdata, 20, tvm, "Trap Virtual Memory");
        TEXT("[21] TW");
        TEXT("[22] TSR");
        TEXT("[31:23] WPRI");
        SETBITS(comb.vb_rdata, 33, 32, CONST("0x2", 2), "UXL: User is 64-bits");
        SETBITS(comb.vb_rdata, 35, 34, CONST("0x2", 2), "SXL: Supervisor is 64-bits");
        TEXT("[36] SBE");
        TEXT("[37] MBE");
        TEXT("[62:38] WPRI");
        TEXT("[63] SD. Read-only bit that summize FS, VS or XS fields");
        IF (NZ(comb.v_csr_wena));
            SETARRITEM(xmode, comb.iS, xmode->xie, BIT(cmd_data, 1));
            SETARRITEM(xmode, comb.iM, xmode->xie, BIT(cmd_data, 3));
            SETARRITEM(xmode, comb.iS, xmode->xpie, BIT(cmd_data, 5));
            SETARRITEM(xmode, comb.iM, xmode->xpie, BIT(cmd_data, 7));
            SETARRITEM(xmode, comb.iS, xmode->xpp, CC2(CONST("0", 1), BIT(cmd_data, 8)));
            SETARRITEM(xmode, comb.iM, xmode->xpp, BITS(cmd_data, 12, 11));
            SETVAL(mprv, BIT(cmd_data, 17));
            SETVAL(tvm, BIT(cmd_data, 20));
        ENDIF();
        ENDCASE();
    CASE (CONST("0x301", 12), "misa: [MRW] ISA and extensions");
        TEXT("Base[XLEN-1:XLEN-2]");
        TEXT("     1 = 32");
        TEXT("     2 = 64");
        TEXT("     3 = 128");
        TEXT();
        SETBITS(comb.vb_rdata, DEC(cfg->RISCV_ARCH), SUB2(cfg->RISCV_ARCH, CONST("2")), CONST("2", 2));
        TEXT("BitCharacterDescription");
        TEXT("     0  A Atomic extension");
        TEXT("     1  B Tentatively reserved for Bit operations extension");
        TEXT("     2  C Compressed extension");
        TEXT("     3  D Double-precision Foating-point extension");
        TEXT("     4  E RV32E base ISA (embedded)");
        TEXT("     5  F Single-precision Foating-point extension");
        TEXT("     6  G Additional standard extensions present");
        TEXT("     7  H Hypervisor mode implemented");
        TEXT("     8  I RV32I/64I/128I base ISA");
        TEXT("     9  J Reserved");
        TEXT("     10 K Reserved");
        TEXT("     11 L Tentatively reserved for Decimal Floating-Point extension");
        TEXT("     12 M Integer Multiply/Divide extension");
        TEXT("     13 N User-level interrupts supported");
        TEXT("     14 O Reserved");
        TEXT("     15 P Tentatively reserved for Packed-SIMD extension");
        TEXT("     16 Q Quad-precision Foating-point extension");
        TEXT("     17 R Reserved");
        TEXT("     18 S Supervisor mode implemented");
        TEXT("     19 T Tentatively reserved for Transactional Memory extension");
        TEXT("     20 U User mode implemented");
        TEXT("     21 V Tentatively reserved for Vector extension");
        TEXT("     22 W Reserved");
        TEXT("     23 X Non-standard extensions present");
        TEXT("     24 Y Reserved");
        TEXT("     25 Z Reserve");
        SETBITONE(comb.vb_rdata, 'A' - 'A', "A-extension");
        SETBITONE(comb.vb_rdata, 'I' - 'A', "I-extension");
        SETBITONE(comb.vb_rdata, 'M' - 'A', "M-extension");
        SETBITONE(comb.vb_rdata, 'S' - 'A', "S-extension");
        SETBITONE(comb.vb_rdata, 'U' - 'A', "U-extension");
        SETBITONE(comb.vb_rdata, 'C' - 'A', "C-extension");
        IF (cfg->CFG_HW_FPU_ENABLE);
            SETBITONE(comb.vb_rdata, 'D' - 'A', "D-extension");
        ENDIF();
        ENDCASE();
    CASE (CONST("0x302", 12), "medeleg: [MRW] Machine exception delegation");
        SETVAL(comb.vb_rdata, medeleg);
        IF (comb.v_csr_wena);
            TEXT("page 31. Read-only zero for exceptions that could not be delegated, especially Call from M-mode");
            SETVAL(medeleg, AND2_L(cmd_data, CONST("0xb3ff", 64)));
        ENDIF();
        ENDCASE();
    CASE (CONST("0x303", 12), "mideleg: [MRW] Machine interrupt delegation");
        SETVAL(comb.vb_rdata, mideleg);
        IF (comb.v_csr_wena);
            TEXT("No need to delegate machine interrupts to supervisor (but possible)");
            SETVAL(mideleg, AND2_L(BITS(cmd_data, DEC(cfg->IRQ_TOTAL), CONST("0")), CONST("0x222", 12)));
        ENDIF();
        ENDCASE();
    CASE (CONST("0x304", 12), "mie: [MRW] Machine interrupt enable bit");
        SETBIT(comb.vb_rdata, DEC(cfg->IRQ_SSIP), ARRITEM(xmode, comb.iS, xmode->xsie));
        SETBIT(comb.vb_rdata, DEC(cfg->IRQ_MSIP), ARRITEM(xmode, comb.iM, xmode->xsie));
        SETBIT(comb.vb_rdata, DEC(cfg->IRQ_STIP), ARRITEM(xmode, comb.iS, xmode->xtie));
        SETBIT(comb.vb_rdata, DEC(cfg->IRQ_MTIP), ARRITEM(xmode, comb.iM, xmode->xtie));
        SETBIT(comb.vb_rdata, DEC(cfg->IRQ_SEIP), ARRITEM(xmode, comb.iS, xmode->xeie));
        SETBIT(comb.vb_rdata, DEC(cfg->IRQ_MEIP), ARRITEM(xmode, comb.iM, xmode->xeie));
        IF (comb.v_csr_wena);
            TEXT("Write only supported interrupts");
            SETARRITEM(xmode, comb.iS, xmode->xsie, BIT(cmd_data, cfg->IRQ_SSIP));
            SETARRITEM(xmode, comb.iM, xmode->xsie, BIT(cmd_data, cfg->IRQ_MSIP));
            SETARRITEM(xmode, comb.iS, xmode->xtie, BIT(cmd_data, cfg->IRQ_STIP));
            SETARRITEM(xmode, comb.iM, xmode->xtie, BIT(cmd_data, cfg->IRQ_MTIP));
            SETARRITEM(xmode, comb.iS, xmode->xeie, BIT(cmd_data, cfg->IRQ_SEIP));
            SETARRITEM(xmode, comb.iM, xmode->xeie, BIT(cmd_data, cfg->IRQ_MEIP));
        ENDIF();
        ENDCASE();
    CASE (CONST("0x305", 12), "mtvec: [MRW] Machine trap-handler base address");
        SETVAL(comb.vb_rdata, ARRITEM(xmode, comb.iM, xmode->xtvec_off));
        SETBITS(comb.vb_rdata, 1, 0, ARRITEM(xmode, comb.iM, xmode->xtvec_mode));
        IF (NZ(comb.v_csr_wena));
            SETARRITEM(xmode, comb.iM, xmode->xtvec_off,
                CC2(BITS(cmd_data, DEC(cfg->RISCV_ARCH), CONST("2")), CONST("0", 2)));
            SETARRITEM(xmode, comb.iM, xmode->xtvec_mode, BITS(cmd_data, 1, 0));
        ENDIF();
        ENDCASE();
    CASE (CONST("0x306", 12), "mcounteren: [MRW] Machine counter enable");
        SETVAL(comb.vb_rdata, ARRITEM(xmode, comb.iM, xmode->xcounteren));
        IF (NZ(comb.v_csr_wena));
            SETARRITEM(xmode, comb.iM, xmode->xcounteren, BITS(cmd_data, 15, 0));
        ENDIF();
        ENDCASE();
    // Machine Trap Handling
    CASE (CONST("0x340", 12), "mscratch: [MRW] Machine scratch register");
        SETVAL(comb.vb_rdata, ARRITEM(xmode, comb.iM, xmode->xscratch));
        IF (comb.v_csr_wena);
            SETARRITEM(xmode, comb.iM, xmode->xscratch, cmd_data);
        ENDIF();
        ENDCASE();
    CASE (CONST("0x341", 12), "mepc: [MRW] Machine program counter");
        SETVAL(comb.vb_rdata, ARRITEM(xmode, comb.iM, xmode->xepc));
        IF (comb.v_csr_wena);
            SETARRITEM(xmode, comb.iM, xmode->xepc, BITS(cmd_data, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0")));
        ENDIF();
        ENDCASE();
    CASE (CONST("0x342", 12), "mcause: [MRW] Machine trap cause");
        SETBIT(comb.vb_rdata, 63, ARRITEM(xmode, comb.iM, xmode->xcause_irq));
        SETBITS(comb.vb_rdata, 4, 0, ARRITEM(xmode, comb.iM, xmode->xcause_code));
        ENDCASE();
    CASE (CONST("0x343", 12), "mtval: [MRW] Machine bad address or instruction");
        SETVAL(comb.vb_rdata, ARRITEM(xmode, comb.iM, xmode->xtval));
        IF (comb.v_csr_wena);
            SETARRITEM(xmode, comb.iM, xmode->xtval, cmd_data);
        ENDIF();
        ENDCASE();
    CASE (CONST("0x344", 12), "mip: [MRW] Machine interrupt pending");
        SETBITS(comb.vb_rdata, DEC(cfg->IRQ_TOTAL), CONST("0"), irq_pending);
        IF (comb.v_csr_wena);
            SETVAL(mip_ssip, BIT(cmd_data, cfg->IRQ_SSIP));
            SETVAL(mip_stip, BIT(cmd_data, cfg->IRQ_STIP));
            SETVAL(mip_seip, BIT(cmd_data, cfg->IRQ_SEIP));
        ENDIF();
        ENDCASE();
    CASE (CONST("0x34A", 12), "mtinst: [MRW] Machine trap instruction (transformed)");
        ENDCASE();
    CASE (CONST("0x34B", 12), "mtval2: [MRW] Machine bad guest physical register");
        ENDCASE();
    // Machine Configuration
    CASE (CONST("0x30A", 12), "menvcfg: [MRW] Machine environment configuration register");
        ENDCASE();
    CASE (CONST("0x747", 12), "mseccfg: [MRW] Machine security configuration register");
        ENDCASE();
    // Machine protection and tanslation
    CASE (CONST("0x3A0", 12), "pmpcfg0: [MRW] Physical memory protection configuration");
        ENDCASE();
    CASE (CONST("0x3A2", 12), "pmpcfg2: [MRW] Physical memory protection configuration");
        ENDCASE();
    //..
    CASE (CONST("0x3AE", 12), "pmpcfg14: [MRW] Physical memory protection configuration");
        ENDCASE();
    CASE (CONST("0x3B0", 12), "pmpaddr0: [MRW] Physical memory protection address register");
        ENDCASE();
    CASE (CONST("0x3B1", 12), "pmpaddr1: [MRW] Physical memory protection address register");
        ENDCASE();
    //..
    CASE (CONST("0x3EF", 12), "pmpaddr63: [MRW] Physical memory protection address register");
        ENDCASE();
    // Machine Counter/Timers
    CASE (CONST("0xB00", 12), "mcycle: [MRW] Machine cycle counter");
        SETVAL(comb.vb_rdata, mcycle_cnt);
        IF (comb.v_csr_wena);
            SETVAL(mcycle_cnt, cmd_data);
        ENDIF();
        ENDCASE();
    CASE (CONST("0xB02", 12), "minstret: [MRW] Machine instructions-retired counter");
        SETVAL(comb.vb_rdata, minstret_cnt);
        IF (comb.v_csr_wena);
            SETVAL(minstret_cnt, cmd_data);
        ENDIF();
        ENDCASE();
    // Machine counter setup
    CASE (CONST("0x320", 12), "mcountinhibit: [MRW] Machine counter-inhibit register");
        SETVAL(comb.vb_rdata, mcountinhibit);
        IF (NZ(comb.v_csr_wena));
            SETVAL(mcountinhibit, BITS(cmd_data, 15, 0));
        ENDIF();
        ENDCASE();
    CASE (CONST("0x323", 12), "mpevent3: [MRW] Machine performance-monitoring event selector");
        ENDCASE();
    CASE (CONST("0x324", 12), "mpevent4: [MRW] Machine performance-monitoring event selector");
        ENDCASE();
    //..
    CASE (CONST("0x33F", 12), "mpevent31: [MRW] Machine performance-monitoring event selector");
        ENDCASE();
    // Debug/Trace Registers (shared with Debug Mode)
    CASE (CONST("0x7A0", 12), "tselect: [MRW] Debug/Trace trigger register select");
        ENDCASE();
    CASE (CONST("0x7A1", 12), "tdata1: [MRW] First Debug/Trace trigger data register");
        ENDCASE();
    CASE (CONST("0x7A2", 12), "tdata2: [MRW] Second Debug/Trace trigger data register");
        ENDCASE();
    CASE (CONST("0x7A3", 12), "tdata3: [MRW] Third Debug/Trace trigger data register");
        ENDCASE();
    CASE (CONST("0x7A8", 12), "mcontext: [MRW] Machine-mode context register");
        ENDCASE();
    // Debug Mode registers
    CASE (CONST("0x7B0", 12), "dcsr: [DRW] Debug control and status register");
        SETBITS(comb.vb_rdata, 31, 28, CONST("4", 4), "xdebugver: 4=External debug supported");
        SETBIT(comb.vb_rdata, 15, dcsr_ebreakm);
        SETBIT(comb.vb_rdata, 11, dcsr_stepie, "interrupt dis/ena during step");
        SETBIT(comb.vb_rdata, 10, dcsr_stopcount, "don't increment any counter");
        SETBIT(comb.vb_rdata, 9, dcsr_stoptimer, "don't increment timer");
        SETBITS(comb.vb_rdata, 8, 6, halt_cause);
        SETBIT(comb.vb_rdata, 2, dcsr_step);
        SETBITS(comb.vb_rdata, 1, 0, CONST("3", 2), "prv: privilege in debug mode: 3=machine");
        IF (NZ(comb.v_csr_wena));
            SETVAL(dcsr_ebreakm, BIT(cmd_data, 15));
            SETVAL(dcsr_stepie, BIT(cmd_data, 11));
            SETVAL(dcsr_stopcount, BIT(cmd_data, 10));
            SETVAL(dcsr_stoptimer, BIT(cmd_data, 9));
            SETVAL(dcsr_step, BIT(cmd_data, 2));
        ENDIF();
        ENDCASE();
    CASE (CONST("0x7B1", 12), "dpc: [DRW] Debug PC");
        TEXT("Upon entry into debug mode DPC must contains:");
        TEXT("       cause        |   Address");
        TEXT("--------------------|----------------");
        TEXT(" ebreak             |  Address of ebreak instruction");
        TEXT(" single step        |  Address of next instruction to be executed");
        TEXT(" trigger (HW BREAK) |  if timing=0, cause isntruction, if timing=1 enxt instruction");
        TEXT(" halt request       |  next instruction");
        TEXT();
        IF (NZ(i_e_halted));
            SETVAL(comb.vb_rdata, dpc);
        ELSE();
            TEXT("make visible current pc for the debugger even in running state");
            SETVAL(comb.vb_rdata, i_e_pc);
        ENDIF();
        IF (NZ(comb.v_csr_wena));
            SETVAL(dpc, cmd_data);
        ENDIF();
        ENDCASE();
    CASE (CONST("0x7B2", 12), "dscratch0: [DRW] Debug scratch register 0");
        SETVAL(comb.vb_rdata, dscratch0);
        IF (NZ(comb.v_csr_wena));
            SETVAL(dscratch0, cmd_data);
        ENDIF();
        ENDCASE();
    CASE (CONST("0x7B3", 12), "dscratch1: [DRW] Debug scratch register 1");
        SETVAL(comb.vb_rdata, dscratch1);
        IF (comb.v_csr_wena);
            SETVAL(dscratch1, cmd_data);
        ENDIF();
        ENDCASE();
    // River specific CSRs
    CASE (CONST("0xBC0", 12), "mstackovr: [MRW] Machine Stack Overflow");
        SETVAL(comb.vb_rdata, mstackovr);
        IF (NZ(comb.v_csr_wena));
            SETVAL(mstackovr, BITS(cmd_data, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0")));
        ENDIF();
        ENDCASE();
    CASE (CONST("0xBC1", 12), "mstackund: [MRW] Machine Stack Underflow");
        SETVAL(comb.vb_rdata, mstackund);
        IF (NZ(comb.v_csr_wena));
            SETVAL(mstackund, BITS(cmd_data, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0")));
        ENDIF();
        ENDCASE();
    CASE (CONST("0xBC2", 12), "mpu_addr: [MWO] MPU address");
        IF (NZ(comb.v_csr_wena));
            SETVAL(mpu_addr, BITS(cmd_data, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0")));
        ENDIF();
        ENDCASE();
    CASE (CONST("0xBC3", 12), "mpu_mask: [MWO] MPU mask");
        IF (NZ(comb.v_csr_wena));
            SETVAL(mpu_mask, BITS(cmd_data, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0")));
        ENDIF();
        ENDCASE();
    CASE (CONST("0xBC4", 12), "mpu_ctrl: [MRW] MPU flags and write ena");
        SETVAL(comb.vb_rdata, CC2(cfg->CFG_MPU_TBL_SIZE, CONST("0", 8)));
        IF (NZ(comb.v_csr_wena));
            SETVAL(mpu_idx, BITS(cmd_data, ADD2(CONST("8"), DEC(cfg->CFG_MPU_TBL_WIDTH)), CONST("8")));
            SETVAL(mpu_flags, BITS(cmd_data, DEC(cfg->CFG_MPU_FL_TOTAL), CONST("0")));
            SETVAL(mpu_we, BIT(cmd_data, 7));
        ENDIF();
        ENDCASE();
//    CASE (CONST("0x800", 12), "flushi: [UWO]");
//        IF (NZ(comb.v_csr_wena));
//            SETONE(flushi_ena);
//            SETVAL(flushi_addr, BITS(cmd_data, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0")));
//        ENDIF();
//        ENDCASE();
    CASEDEF();
        TEXT("Not implemented CSR:");
        IF (EQ(state, State_RW));
            SETONE(cmd_exception);
        ENDIF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    IF (NZ(comb.v_csr_rena));
        SETVAL(cmd_data, comb.vb_rdata);
    ENDIF();

TEXT();
TEXT();
    IF (NZ(comb.v_csr_trapreturn));
        IF (EQ(mode, cmd_addr));
            SETVAL(mode, comb.vb_xpp);
            SETARRITEM(xmode, TO_INT(mode), xmode->xpie, CONST("1", 1), "xPIE is set to 1 always, page 21");
            SETARRITEM(xmode, TO_INT(mode), xmode->xpp, cfg->PRV_U, "Set to least-privildged supported mode (U), page 21");
        ELSE();
            SETONE(cmd_exception, "xret not matched to current mode");
        ENDIF();
        IF (NE(ARRITEM_B(xmode, TO_INT(mode), xmode->xpp), cfg->PRV_M), "see page 21");
            SETZERO(mprv);
        ENDIF();
    ENDIF();

TEXT();
    TEXT("Check MMU:");
    SETZERO(immu_ena);
    SETZERO(dmmu_ena);
    IF (EQ(satp_mode, SATP_MODE_SV48), "Only SV48 implemented");
        IF (EZ(BIT(mode, 1)));
            TEXT("S and U modes");
            SETONE(immu_ena);
            SETONE(dmmu_ena);
        ELSIF(AND2(NZ(mprv), EZ(BIT(comb.vb_xpp, 1))));
            TEXT("Previous state is S or U mode");
            TEXT("Instruction address-translation and protection are unaffected");
            SETONE(dmmu_ena);
        ENDIF();
    ENDIF();

TEXT();
    IF (ANDx(2, &LE(mode, cfg->PRV_S),
                &OR2(AND2_L(comb.vb_e_emux, medeleg), AND2_L(comb.vb_e_imux, mideleg))));
        TEXT("Trap delegated to Supervisor mode");
        SETARRITEM(xmode, comb.iS, xmode->xpp, mode);
        SETARRITEM(xmode, comb.iS, xmode->xpie, ARRITEM(xmode, TO_INT(mode), xmode->xie));
        SETARRITEM(xmode, comb.iS, xmode->xie, CONST("0", 1));
        SETARRITEM(xmode, comb.iS, xmode->xepc, i_e_pc, "current latched instruction not executed overwritten by exception/interrupt");
        SETARRITEM(xmode, comb.iS, xmode->xtval, comb.vb_xtval, "trap value/bad address");
        SETARRITEM(xmode, comb.iS, xmode->xcause_code, comb.wb_trap_cause);
        SETARRITEM(xmode, comb.iS, xmode->xcause_irq, INV(OR_REDUCE(comb.vb_e_emux)));
        SETVAL(mode, cfg->PRV_S);
    ELSIF (OR2(NZ(comb.vb_e_emux), NZ(comb.vb_e_imux)));
        TEXT("By default all exceptions and interrupts handled in M-mode (not delegations)");
        SETARRITEM(xmode, comb.iM, xmode->xpp, mode);
        SETARRITEM(xmode, comb.iM, xmode->xpie, ARRITEM(xmode, TO_INT(mode), xmode->xie));
        SETARRITEM(xmode, comb.iM, xmode->xie, CONST("0", 1));
        SETARRITEM(xmode, comb.iM, xmode->xepc, i_e_pc, "current latched instruction not executed overwritten by exception/interrupt");
        SETARRITEM(xmode, comb.iM, xmode->xtval, comb.vb_xtval, "trap value/bad address");
        SETARRITEM(xmode, comb.iM, xmode->xcause_code, comb.wb_trap_cause);
        SETARRITEM(xmode, comb.iM, xmode->xcause_irq, INV(OR_REDUCE(comb.vb_e_emux)));
        SETVAL(mode, cfg->PRV_M);
        SETZERO(immu_ena);
        SETZERO(dmmu_ena);
    ENDIF();

TEXT();
    TEXT("Step is not enabled or interrupt enabled during stepping");
    IF (NZ(OR2(INV(dcsr_step), dcsr_stepie)));
        IF (ARRITEM(xmode, comb.iM, xmode->xie));
            TEXT("ALL not-delegated interrupts");
            SETVAL(comb.vb_irq_ena, INV_L(mideleg));
        ENDIF();
        IF (ARRITEM(xmode, comb.iS, xmode->xie));
            TEXT("Delegated to S-mode:");
            SETVAL(comb.vb_irq_ena, OR2_L(comb.vb_irq_ena, mideleg));
        ENDIF();
    ENDIF();

TEXT();
    TEXT("The following pending interrupt could be set in mip:");
    SETBIT(comb.vb_pending, cfg->IRQ_MSIP, AND2(BIT(i_irq_pending, cfg->IRQ_MSIP),
                                                ARRITEM(xmode, comb.iM, xmode->xsie)));
    SETBIT(comb.vb_pending, cfg->IRQ_MTIP, AND2(BIT(i_irq_pending, cfg->IRQ_MTIP),
                                                ARRITEM(xmode, comb.iM, xmode->xtie)));
    SETBIT(comb.vb_pending, cfg->IRQ_MEIP, AND2(BIT(i_irq_pending, cfg->IRQ_MEIP),
                                                ARRITEM(xmode, comb.iM, xmode->xeie)));
    SETBIT(comb.vb_pending, cfg->IRQ_SSIP, AND2(OR2(BIT(i_irq_pending, cfg->IRQ_SSIP), mip_ssip),
                                                ARRITEM(xmode, comb.iS, xmode->xsie)));
    SETBIT(comb.vb_pending, cfg->IRQ_STIP, AND2(OR2(BIT(i_irq_pending, cfg->IRQ_STIP), mip_stip),
                                                ARRITEM(xmode, comb.iS, xmode->xtie)));
    SETBIT(comb.vb_pending, cfg->IRQ_SEIP, AND2(OR2(BIT(i_irq_pending, cfg->IRQ_SEIP), mip_seip),
                                                ARRITEM(xmode, comb.iS, xmode->xeie)));
    SETVAL(irq_pending, comb.vb_pending);

TEXT();
    SETZERO(comb.w_mstackovr);
    IF (AND2(NZ(mstackovr),
             LS(BITS(i_sp, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0")), mstackovr)));
        SETONE(comb.w_mstackovr);
        SETZERO(mstackovr);
    ENDIF();
    SETZERO(comb.w_mstackund);
    IF (AND2(NZ(mstackund),
             GT(BITS(i_sp, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0")), mstackund)));
        SETONE(comb.w_mstackund);
        SETZERO(mstackund);
    ENDIF();

TEXT();
    TEXT("if (i_fpu_valid.read()) {");
    TEXT("    v.ex_fpu_invalidop = i_ex_fpu_invalidop.read();");
    TEXT("    v.ex_fpu_divbyzero = i_ex_fpu_divbyzero.read();");
    TEXT("    v.ex_fpu_overflow = i_ex_fpu_overflow.read();");
    TEXT("    v.ex_fpu_underflow = i_ex_fpu_underflow.read();");
    TEXT("    v.ex_fpu_inexact = i_ex_fpu_inexact.read();");
    TEXT("}");


TEXT();
    TEXT("stopcount: do not increment any counters including cycle and instret");
    IF (AND3(EZ(i_e_halted), EZ(dcsr_stopcount), EZ(BIT(mcountinhibit, 0))));
        SETVAL(mcycle_cnt, INC(mcycle_cnt));
    ENDIF();
    IF (ANDx(4, &NZ(i_e_valid),
                &EZ(dcsr_stopcount),
                &EZ(i_dbg_progbuf_ena),
                &EZ(BIT(mcountinhibit, 2))));
        SETVAL(minstret_cnt, INC(minstret_cnt));
    ENDIF();


TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_req_ready, comb.v_req_ready);
    SETVAL(o_resp_valid, comb.v_resp_valid);
    SETVAL(o_resp_data, cmd_data);
    SETVAL(o_resp_exception, cmd_exception);
    SETVAL(o_progbuf_end, AND2(progbuf_end, i_resp_ready));
    SETVAL(o_progbuf_error, AND2(progbuf_err, i_resp_ready));
    SETVAL(o_irq_pending, AND2_L(irq_pending, comb.vb_irq_ena));
    SETVAL(o_wakeup, OR_REDUCE(irq_pending));
    SETVAL(o_stack_overflow, comb.w_mstackovr);
    SETVAL(o_stack_underflow, comb.w_mstackund);
    SETVAL(o_executed_cnt, minstret_cnt);
    SETVAL(o_mpu_region_we, mpu_we);
    SETVAL(o_mpu_region_idx, mpu_idx);
    SETVAL(o_mpu_region_addr, mpu_addr);
    SETVAL(o_mpu_region_mask, mpu_mask);
    SETVAL(o_mpu_region_flags, mpu_flags);
    SETVAL(o_immu_ena, immu_ena);
    SETVAL(o_dmmu_ena, dmmu_ena);
    SETVAL(o_mmu_ppn, satp_ppn);
    SETVAL(o_step, dcsr_step);
    SETVAL(o_flushd_valid, comb.v_flushd);
    SETVAL(o_flushi_valid, comb.v_flushi);
    SETVAL(o_flushmmu_valid, comb.v_flushmmu);
    SETVAL(o_flush_addr, BITS(cmd_data, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0")));
}
