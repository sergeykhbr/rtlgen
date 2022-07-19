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
    i_msip(this, "i_msip", "1", "machine software pening interrupt"),
    i_mtip(this, "i_mtip", "1", "machine timer pening interrupt"),
    i_meip(this, "i_meip", "1", "machine external pening interrupt"),
    i_seip(this, "i_seip", "1", "supervisor external pening interrupt"),
    o_irq_software(this, "o_irq_software", "1", "software interrupt pending bit"),
    o_irq_timer(this, "o_irq_timer", "1", "timer interrupt pending bit"),
    o_irq_external(this, "o_irq_external", "1", "external interrupt pending bit"),
    o_stack_overflow(this, "o_stack_overflow", "1", "stack overflow exception"),
    o_stack_underflow(this, "o_stack_underflow", "1", "stack underflow exception"),
    i_e_valid(this, "i_e_valid", "1", "instructuin executed flag"),
    o_executed_cnt(this, "o_executed_cnt", "64", "Number of executed instructions"),
    _io0_(this),
    o_step(this, "o_step", "1", "Stepping enabled"),
    i_dbg_progbuf_ena(this, "i_dbg_progbuf_ena", "1", "Executing progbuf is in progress"),
    o_progbuf_end(this, "o_progbuf_end", "1", "End of execution from prog buffer"),
    o_progbuf_error(this, "o_progbuf_error", "1", "exception during progbuf execution"),
    o_flushi_ena(this, "o_flushi_ena", "1", "clear specified addr in ICache without execution of fence.i"),
    o_flushi_addr(this, "o_flushi_addr", "CFG_CPU_ADDR_BITS", "ICache address to flush"),
    _io1_(this),
    o_mpu_region_we(this, "o_mpu_region_we", "1", "write enable into MPU"),
    o_mpu_region_idx(this, "o_mpu_region_idx", "CFG_MPU_TBL_WIDTH", "selected MPU region"),
    o_mpu_region_addr(this, "o_mpu_region_addr", "CFG_CPU_ADDR_BITS", "MPU region base address"),
    o_mpu_region_mask(this, "o_mpu_region_mask", "CFG_CPU_ADDR_BITS", "MPU region mask"),
    o_mpu_region_flags(this, "o_mpu_region_flags", "CFG_MPU_FL_TOTAL", "{ena, cachable, r, w, x}"),
    _io2_(this),
    o_mmu_ena(this, "o_mmu_ena", "1", "MMU enabled in U and S modes. Sv48 only."),
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
    State_Response(this, "State_Response", "9"),
    SATP_MODE_SV48(this, "4", "SATP_MODE_SV48", "9", "48-bits Page mode"),
    // registers
    state(this, "state", "4", "State_Idle"),
    cmd_type(this, "cmd_type", "CsrReq_TotalBits"),
    cmd_addr(this, "cmd_addr", "12"),
    cmd_data(this, "cmd_data", "RISCV_ARCH"),
    cmd_exception(this, "cmd_exception", "1", "0", "exception on CSR access"),
    progbuf_end(this, "progbuf_end", "1"),
    progbuf_err(this, "progbuf_err", "1"),
    mtvec(this, "mtvec", "RISCV_ARCH"),
    mtvec_mode(this, "mtvec_mode", "2"),
    mtval(this, "mtval", "RISCV_ARCH"),
    mscratch(this, "mscratch", "RISCV_ARCH"),
    mstackovr(this, "mstackovr", "CFG_CPU_ADDR_BITS"),
    mstackund(this, "mstackund", "CFG_CPU_ADDR_BITS"),
    mpu_addr(this, "mpu_addr", "CFG_CPU_ADDR_BITS"),
    mpu_mask(this, "mpu_mask", "CFG_CPU_ADDR_BITS"),
    mpu_idx(this, "mpu_idx", "CFG_MPU_TBL_WIDTH"),
    mpu_flags(this, "mpu_flags", "CFG_MPU_FL_TOTAL"),
    mpu_we(this, "mpu_we", "1"),
    mmu_ena(this, "mmu_ena", "1", "0", "MMU SV48 enabled in U- and S- modes"),
    satp_ppn(this, "satp_ppn", "44", "0", "Physcal Page Number"),
    satp_mode(this, "satp_mode", "4", "0", "Supervisor Address Translation and Protection mode"),
    mepc(this, "mepc", "CFG_CPU_ADDR_BITS"),
    uepc(this, "uepc", "CFG_CPU_ADDR_BITS"),
    mode(this, "mode", "2", "PRV_M"),
    uie(this, "uie", "1", "0", "mstatus: User level interrupts ena for current priv. mode"),
    mie(this, "mie", "1", "0", "mstatus: Machine level interrupts ena for current priv. mode"),
    mpie(this, "mpie", "1", "0", "mstatus: Previous MIE value"),
    mpp(this, "mpp", "2", "0", "mstatus: Previous mode"),
    usie(this, "usie", "1", "0", "mie: User software interrupt enable"),
    ssie(this, "ssie", "1", "0", "mie: Supervisor software interrupt enable"),
    msie(this, "msie", "1", "0", "mie: machine software interrupt enable"),
    utie(this, "utie", "1", "0", "mie: User timer interrupt enable"),
    stie(this, "stie", "1", "0", "mie: Supervisor timer interrupt enable"),
    mtie(this, "mtie", "1", "0", "mie: Machine timer interrupt enable"),
    ueie(this, "ueie", "1", "0", "mie: User external interrupt enable"),
    seie(this, "seie", "1", "0", "mie: Supervisor external interrupt enable"),
    meie(this, "meie", "1", "0", "mie: Machine external interrupt enable"),
    usip(this, "usip", "1", "0", "mip: user software interrupt pending"),
    ssip(this, "ssip", "1", "0", "mip: supervisor software interrupt pending"),
    msip(this, "msip", "1", "0", "mip: machine software interrupt pending"),
    utip(this, "utip", "1", "0", "mip: user timer interrupt pending"),
    stip(this, "stip", "1", "0", "mip: supervisor timer interrupt pending"),
    mtip(this, "mtip", "1", "0", "mip: machine timer interrupt pending"),
    ueip(this, "ueip", "1", "0", "mip: user external interrupt pending"),
    seip(this, "seip", "1", "0", "mip: supervisor external interrupt pending"),
    meip(this, "meip", "1", "0", "mip: machine external interrupt pending"),
    ex_fpu_invalidop(this, "ex_fpu_invalidop", "1", "0", "FPU Exception: invalid operation"),
    ex_fpu_divbyzero(this, "ex_fpu_divbyzero", "1", "0", "FPU Exception: divide by zero"),
    ex_fpu_overflow(this, "ex_fpu_overflow", "1", "0", "FPU Exception: overflow"),
    ex_fpu_underflow(this, "ex_fpu_underflow", "1", "0", "FPU Exception: underflow"),
    ex_fpu_inexact(this, "ex_fpu_inexact", "1", "0", "FPU Exception: inexact"),
    trap_irq(this, "trap_irq", "1"),
    trap_cause(this, "trap_cause", "5"),
    trap_addr(this, "trap_addr", "CFG_CPU_ADDR_BITS"),
    timer(this, "timer", "64", "0", "Timer in clocks."),
    cycle_cnt(this, "cycle_cnt", "64", "0", "Cycle in clocks."),
    executed_cnt(this, "executed_cnt", "64", "0", "Number of valid executed instructions"),
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
    flushi_ena(this, "flushi_ena", "1"),
    flushi_addr(this, "flushi_addr", "CFG_CPU_ADDR_BITS"),
    // process
    comb(this)
{
}

void CsrRegs::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;
    //SETVAL(dbg_e_valid, i_e_valid, "used in RtlWrapper to count executed instructions");
    SETZERO(comb.vb_rdata);
    SETZERO(comb.v_req_halt);
    SETZERO(comb.v_req_resume);
    SETZERO(comb.v_req_progbuf);
    SETZERO(flushi_ena);
    SETZERO(flushi_addr);
    SETZERO(comb.v_req_ready);
    SETZERO(comb.v_resp_valid);
    SETZERO(comb.v_csr_rena);
    SETZERO(comb.v_csr_wena);
    SETZERO(comb.v_csr_trapreturn);
    SETZERO(comb.w_trap_valid);
    SETZERO(comb.v_trap_irq);
    SETZERO(comb.wb_trap_cause);
    SETZERO(comb.vb_mtval);
    SETZERO(mpu_we);
    SETVAL(comb.vb_mtvec_off, CC2(BITS(mtvec, DEC(cfg->RISCV_ARCH), CONST("2")), CONST("0",2)));

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
            IF (BIT(i_req_type, cfg->CsrReq_ExceptionBit));
                SETVAL(state, State_Exception);
            ELSIF(BIT(i_req_type, cfg->CsrReq_BreakpointBit));
                SETVAL(state, State_Breakpoint);
            ELSIF (BIT(i_req_type, cfg->CsrReq_HaltBit));
                SETVAL(state, State_Halt);
            ELSIF (BIT(i_req_type, cfg->CsrReq_ResumeBit));
                SETVAL(state, State_Resume);
            ELSIF (BIT(i_req_type, cfg->CsrReq_InterruptBit));
                SETVAL(state, State_Interrupt);
            ELSIF (BIT(i_req_type, cfg->CsrReq_TrapReturnBit));
                SETVAL(state, State_TrapReturn);
            ELSIF (BIT(i_req_type, cfg->CsrReq_WfiBit));
                SETVAL(state, State_Wfi);
            ELSE();
                SETVAL(state, State_RW);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (State_Exception);
        SETVAL(state, State_Response);
        SETONE(comb.w_trap_valid);
        SETVAL(comb.vb_mtval, cmd_data);
        SETVAL(comb.wb_trap_cause, BITS(cmd_addr, 4, 0));
        SETVAL(cmd_data, comb.vb_mtvec_off);
        IF (NZ(i_dbg_progbuf_ena));
            SETONE(progbuf_err);
            SETONE(progbuf_end);
            SETONE(cmd_exception);
        ENDIF();
        IF (EQ(cmd_addr, CONST("EXCEPTION_CallFromUmode")));
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
            SETONE(comb.w_trap_valid);
            SETVAL(comb.wb_trap_cause, BITS(cmd_addr, 4, 0));
            SETVAL(comb.vb_mtval, i_e_pc);
            SETVAL(cmd_data, comb.vb_mtvec_off, "Jump to exception handler");
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
        SETONE(comb.w_trap_valid);
        SETVAL(comb.wb_trap_cause, ADD2(MUL2(CONST("4"), cmd_addr), cfg->PRV_M));
        SETONE(comb.v_trap_irq);
        IF (EQ(mtvec_mode, CONST("1", 2)));
            TEXT("vectorized");
            SETVAL(cmd_data, ADD2(comb.vb_mtvec_off, MUL2(CONST("4"), comb.wb_trap_cause)));
        ELSE();
            SETVAL(cmd_data, comb.vb_mtvec_off);
        ENDIF();
        ENDCASE();
    CASE (State_TrapReturn);
        SETVAL(state, State_Response);
        SETONE(comb.v_csr_trapreturn);
        IF (EQ(cmd_addr, cfg->CSR_mepc));
            SETVAL(cmd_data, mepc);
        ELSIF (EQ(cmd_addr, cfg->CSR_hepc));
            SETZERO(cmd_data);
        ELSIF (EQ(cmd_addr, cfg->CSR_sepc));
            SETZERO(cmd_data);
        ELSIF (EQ(cmd_addr, cfg->CSR_uepc));
            SETVAL(cmd_data, uepc);
        ELSE();
            SETZERO(cmd_data);
        ENDIF();
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
    TEXT("CSR registers:");
    SWITCH (cmd_addr);
    CASE (cfg->CSR_fflags);
        SETBIT(comb.vb_rdata, 0, ex_fpu_inexact);
        SETBIT(comb.vb_rdata, 1, ex_fpu_underflow);
        SETBIT(comb.vb_rdata, 2, ex_fpu_overflow);
        SETBIT(comb.vb_rdata, 3, ex_fpu_divbyzero);
        SETBIT(comb.vb_rdata, 4, ex_fpu_invalidop);
        ENDCASE();
    CASE (cfg->CSR_frm);
        IF (cfg->CFG_HW_FPU_ENABLE);
            SETBITS(comb.vb_rdata, 2, 0, CONST("4", 3), "Round mode: round to Nearest (RMM)");
        ENDIF();
        ENDCASE();
    CASE (cfg->CSR_fcsr);
        SETBIT(comb.vb_rdata, 0, ex_fpu_inexact);
        SETBIT(comb.vb_rdata, 1, ex_fpu_underflow);
        SETBIT(comb.vb_rdata, 2, ex_fpu_overflow);
        SETBIT(comb.vb_rdata, 3, ex_fpu_divbyzero);
        SETBIT(comb.vb_rdata, 4, ex_fpu_invalidop);
        IF (cfg->CFG_HW_FPU_ENABLE);
            SETBITS(comb.vb_rdata, 7, 5, CONST("4", 3), "Round mode: round to Nearest (RMM)");
        ENDIF();
        ENDCASE();
    CASE (cfg->CSR_misa);
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
        SETBITONE(comb.vb_rdata, 'A' - 'A');
        SETBITONE(comb.vb_rdata, 'I' - 'A');
        SETBITONE(comb.vb_rdata, 'M' - 'A');
        SETBITONE(comb.vb_rdata, 'U' - 'A');
        SETBITONE(comb.vb_rdata, 'C' - 'A');
        IF (cfg->CFG_HW_FPU_ENABLE);
            SETBITONE(comb.vb_rdata, 'D' - 'A');
        ENDIF();
        ENDCASE();
    CASE (cfg->CSR_mvendorid);
        SETVAL(comb.vb_rdata, cfg->CFG_VENDOR_ID);
        ENDCASE();
    CASE (cfg->CSR_marchid);
        ENDCASE();
    CASE (cfg->CSR_mimplementationid);
        SETVAL(comb.vb_rdata, cfg->CFG_IMPLEMENTATION_ID);
        ENDCASE();
    CASE (cfg->CSR_mhartid);
        SETBITS(comb.vb_rdata, 63, 0, hartid);
        ENDCASE();
    CASE (cfg->CSR_uepc, "User mode program counter");
        SETVAL(comb.vb_rdata, uepc);
        IF (comb.v_csr_wena);
            SETVAL(uepc, BITS(cmd_data, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0")));
        ENDIF();
        ENDCASE();
    CASE (cfg->CSR_mstatus, "Machine mode status register");
        SETBIT(comb.vb_rdata, 0, uie);
        SETBIT(comb.vb_rdata, 3, mie);
        SETBIT(comb.vb_rdata, 7, mpie);
        SETBITS(comb.vb_rdata, 12, 11, mpp);
        IF (cfg->CFG_HW_FPU_ENABLE);
            SETBITS(comb.vb_rdata, 14, 13, CONST("0x1", 2), "FS field: Initial state");
        ENDIF();
        SETBITS(comb.vb_rdata, 33, 32, CONST("0x2", 2), "UXL: User mode supported 64-bits");
        IF (NZ(comb.v_csr_wena));
            SETVAL(uie, BIT(cmd_data, 0));
            SETVAL(mie, BIT(cmd_data, 3));
            SETVAL(mpie, BIT(cmd_data, 7));
            SETVAL(mpp, BITS(cmd_data, 12, 11));
        ENDIF();
        ENDCASE();
    CASE (cfg->CSR_medeleg, "Machine exception delegation");
        ENDCASE();
    CASE (cfg->CSR_mideleg, "Machine interrupt delegation");
        ENDCASE();
    CASE (cfg->CSR_mie, "Machine interrupt enable bit");
        SETBIT(comb.vb_rdata, 0, usie, "user software interrupt");
        SETBIT(comb.vb_rdata, 1, ssie, "super-user software interrupt");
        SETBIT(comb.vb_rdata, 3, msie, "machine software interrupt");
        SETBIT(comb.vb_rdata, 4, utie, "user timer interrupt");
        SETBIT(comb.vb_rdata, 5, stie, "super-user timer interrupt");
        SETBIT(comb.vb_rdata, 7, mtie, "machine timer interrupt");
        SETBIT(comb.vb_rdata, 8, ueie, "user external interrupt");
        SETBIT(comb.vb_rdata, 9, seie, "super-user external interrupt");
        SETBIT(comb.vb_rdata, 11, meie, "machine external interrupt");
        IF (comb.v_csr_wena);
            SETVAL(usie, BIT(cmd_data, 0));
            SETVAL(ssie, BIT(cmd_data, 1));
            SETVAL(msie, BIT(cmd_data, 3));
            SETVAL(utie, BIT(cmd_data, 4));
            SETVAL(stie, BIT(cmd_data, 5));
            SETVAL(mtie, BIT(cmd_data, 7));
            SETVAL(ueie, BIT(cmd_data, 8));
            SETVAL(seie, BIT(cmd_data, 9));
            SETVAL(meie, BIT(cmd_data, 11));
        ENDIF();
        ENDCASE();
    CASE (cfg->CSR_mtvec);
        SETVAL(comb.vb_rdata, mtvec);
        SETBITS(comb.vb_rdata, 1, 0, mtvec_mode);
        IF (NZ(comb.v_csr_wena));
            SETVAL(mtvec, CC2(BITS(cmd_data, DEC(cfg->RISCV_ARCH), CONST("2")), CONST("0", 2)));
            SETVAL(mtvec_mode, BITS(cmd_data, 1, 0));
        ENDIF();
        ENDCASE();
    CASE (cfg->CSR_mscratch, "Machine scratch register");
        SETVAL(comb.vb_rdata, mscratch);
        IF (comb.v_csr_wena);
            SETVAL(mscratch, cmd_data);
        ENDIF();
        ENDCASE();
    CASE (cfg->CSR_satp, "Supervisor Address Translation and Protection");
        TEXT("Writing unssoprted MODE[63:60], entire write has no effect");
        TEXT("    MODE = 0 Bare. No translation or protection");
        TEXT("    MODE = 9 Sv48. Page based 48-bit virtual addressing");
        SETBITS(comb.vb_rdata, 43, 0, satp_ppn);
        SETBITS(comb.vb_rdata, 63, 60, satp_mode);
        IF (AND2(NZ(comb.v_csr_wena),
                 OR2(EZ(BITS(cmd_data, 63, 60)),
                     EQ(BITS(cmd_data, 63, 60), SATP_MODE_SV48))));
            SETVAL(satp_ppn, BITS(cmd_data, 43, 0));
            SETVAL(satp_mode, BITS(cmd_data, 63, 60));
        ENDIF();
        ENDCASE();
    CASE (cfg->CSR_mepc, "Machine program counter");
        SETVAL(comb.vb_rdata, mepc);
        IF (comb.v_csr_wena);
            SETVAL(mepc, BITS(cmd_data, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0")));
        ENDIF();
        ENDCASE();
    CASE (cfg->CSR_mcause, "Machine trap cause");
        SETZERO(comb.vb_rdata);
        SETBIT(comb.vb_rdata, 63, trap_irq);
        SETBITS(comb.vb_rdata, 4, 0, trap_cause);
        ENDCASE();
    CASE (cfg->CSR_mtval, "Machine bad address or instruction");
        SETVAL(comb.vb_rdata, mtval);
        IF (comb.v_csr_wena);
            SETVAL(mtval, cmd_data);
        ENDIF();
        ENDCASE();
    CASE (cfg->CSR_mip, "Machine interrupt pending");
        SETBIT(comb.vb_rdata, 0, usip, "user software pending bit");
        SETBIT(comb.vb_rdata, 1, ssip, "super-user software pending bit");
        SETBIT(comb.vb_rdata, 3, msip, "machine software pending bit");
        SETBIT(comb.vb_rdata, 4, utip, "user timer pending bit");
        SETBIT(comb.vb_rdata, 5, stip, "super-user timer pending bit");
        SETBIT(comb.vb_rdata, 7, mtip, "RO: machine timer pending bit");
        SETBIT(comb.vb_rdata, 8, ueip, "user external pending bit");
        SETBIT(comb.vb_rdata, 9, seip, "super-user external pending bit");
        SETBIT(comb.vb_rdata, 11, meip, "RO: machine external pending bit (cleared by writing into mtimecmp)");
        ENDCASE();
    CASE (cfg->CSR_cycle);
        SETVAL(comb.vb_rdata, cycle_cnt);
        ENDCASE();
    CASE (cfg->CSR_time);
        SETVAL(comb.vb_rdata, timer);
        ENDCASE();
    CASE (cfg->CSR_insret);
        SETVAL(comb.vb_rdata, executed_cnt);
        ENDCASE();
    CASE (cfg->CSR_mstackovr, "Machine Stack Overflow");
        SETVAL(comb.vb_rdata, mstackovr);
        IF (NZ(comb.v_csr_wena));
            SETVAL(mstackovr, BITS(cmd_data, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0")));
        ENDIF();
        ENDCASE();
    CASE (cfg->CSR_mstackund, "Machine Stack Underflow");
        SETVAL(comb.vb_rdata, mstackund);
        IF (NZ(comb.v_csr_wena));
            SETVAL(mstackund, BITS(cmd_data, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0")));
        ENDIF();
        ENDCASE();
    CASE (cfg->CSR_mpu_addr, "[WO] MPU address");
        IF (NZ(comb.v_csr_wena));
            SETVAL(mpu_addr, BITS(cmd_data, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0")));
        ENDIF();
        ENDCASE();
    CASE (cfg->CSR_mpu_mask, "[WO] MPU mask");
        IF (NZ(comb.v_csr_wena));
            SETVAL(mpu_mask, BITS(cmd_data, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0")));
        ENDIF();
        ENDCASE();
    CASE (cfg->CSR_mpu_ctrl, "[WO] MPU flags and write ena");
        SETVAL(comb.vb_rdata, CC2(cfg->CFG_MPU_TBL_SIZE, CONST("0", 8)));
        IF (NZ(comb.v_csr_wena));
            SETVAL(mpu_idx, BITS(cmd_data, ADD2(CONST("8"), DEC(cfg->CFG_MPU_TBL_WIDTH)), CONST("8")));
            SETVAL(mpu_flags, BITS(cmd_data, DEC(cfg->CFG_MPU_FL_TOTAL), CONST("0")));
            SETONE(mpu_we);
        ENDIF();
        ENDCASE();
    CASE (cfg->CSR_flushi);
        IF (NZ(comb.v_csr_wena));
            SETONE(flushi_ena);
            SETVAL(flushi_addr, BITS(cmd_data, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0")));
        ENDIF();
        ENDCASE();
    CASE (cfg->CSR_dcsr);
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
    CASE (cfg->CSR_dpc);
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
    CASE (cfg->CSR_dscratch0);
        SETVAL(comb.vb_rdata, dscratch0);
        IF (NZ(comb.v_csr_wena));
            SETVAL(dscratch0, cmd_data);
        ENDIF();
        ENDCASE();
    CASE (cfg->CSR_dscratch1);
        SETVAL(comb.vb_rdata, dscratch1);
        IF (comb.v_csr_wena);
            SETVAL(dscratch1, cmd_data);
        ENDIF();
        ENDCASE();
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
        IF (AND2(EQ(mode, cfg->PRV_M), EQ(cmd_addr, cfg->CSR_mepc)));
            SETVAL(mie, mpie);
            SETONE(mpie);
            SETVAL(mode, mpp);
            IF (AND2(LE(mpp, cfg->PRV_S), EQ(satp_mode, SATP_MODE_SV48)));
                SETONE(mmu_ena);
            ELSE();
                SETZERO(mmu_ena);
            ENDIF();
            SETVAL(mpp, cfg->PRV_U);
        ELSIF (AND2(EQ(mode, cfg->PRV_U), EQ(cmd_addr, cfg->CSR_uepc)));
            SETVAL(mie, mpie);
            SETZERO(mpie, "Interrupts in a user mode actually not supported");
            SETVAL(mode, mpp);
            IF (AND2(LE(mpp, cfg->PRV_S), EQ(satp_mode, SATP_MODE_SV48)));
                SETONE(mmu_ena);
            ELSE();
                SETZERO(mmu_ena);
            ENDIF();
            SETVAL(mpp, cfg->PRV_U);
        ELSE();
            SETONE(cmd_exception);
        ENDIF();
    ENDIF();

TEXT();
    IF (NZ(comb.w_trap_valid));
        SETZERO(mie);
        SETVAL(mpp, mode);
        SETVAL(mepc, i_e_pc, "current latched instruction not executed overwritten by exception/interrupt");
        SETVAL(mtval, comb.vb_mtval, "additional information for hwbreakpoint, memaccess faults and illegal opcodes");
        SETVAL(trap_cause, comb.wb_trap_cause);
        SETVAL(trap_irq, comb.v_trap_irq);
        SETVAL(mode, cfg->PRV_M);
        SETZERO(mmu_ena);
        IF (EQ(mode, cfg->PRV_U));
            SETVAL(mpie, uie);
        ELSIF (EQ(mode, cfg->PRV_M));
            SETVAL(mpie, mie);
        ENDIF();
    ENDIF();

TEXT();
    SETVAL(msip, i_msip);
    SETVAL(comb.v_sw_irq, AND4(msip, msie, mie, OR2(INV(dcsr_step), dcsr_stepie)));

TEXT();
    SETVAL(mtip, i_mtip);
    SETVAL(comb.v_tmr_irq, AND4(mtip, mtie, mie, OR2(INV(dcsr_step), dcsr_stepie)));

TEXT();
    SETVAL(meip, i_meip);
    SETVAL(seip, i_seip);
    SETVAL(comb.v_ext_irq, AND4(meip, meie, mie, OR2(INV(dcsr_step), dcsr_stepie)));


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
    IF (OR2(EZ(i_e_halted), EZ(dcsr_stopcount)));
        SETVAL(cycle_cnt, INC(cycle_cnt));
    ENDIF();
    IF (OR2(AND2(i_e_valid, INV(dcsr_stopcount)),
            AND2(i_e_valid, INV(AND2(i_dbg_progbuf_ena, dcsr_stopcount)))));
        SETVAL(executed_cnt, INC(executed_cnt));
    ENDIF();
    IF (INV(AND2(OR2(i_e_halted, i_dbg_progbuf_ena), dcsr_stoptimer)));
        SETVAL(timer, INC(timer));
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
    SETVAL(o_irq_software, comb.v_sw_irq);
    SETVAL(o_irq_timer, comb.v_tmr_irq);
    SETVAL(o_irq_external, comb.v_ext_irq);
    SETVAL(o_stack_overflow, comb.w_mstackovr);
    SETVAL(o_stack_underflow, comb.w_mstackund);
    SETVAL(o_executed_cnt, executed_cnt);
    SETVAL(o_mpu_region_we, mpu_we);
    SETVAL(o_mpu_region_idx, mpu_idx);
    SETVAL(o_mpu_region_addr, mpu_addr);
    SETVAL(o_mpu_region_mask, mpu_mask);
    SETVAL(o_mpu_region_flags, mpu_flags);
    SETVAL(o_mmu_ena, mmu_ena);
    SETVAL(o_mmu_ppn, satp_ppn);
    SETVAL(o_step, dcsr_step);
    SETVAL(o_flushi_ena, flushi_ena);
    SETVAL(o_flushi_addr, flushi_addr);
}
