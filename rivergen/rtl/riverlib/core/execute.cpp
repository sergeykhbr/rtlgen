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

#include "execute.h"

InstrExecute::InstrExecute(GenObject *parent, const char *name) :
    ModuleObject(parent, "InstrExecute", name),
    fpu_ena(this, "fpu_ena", "true"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_d_radr1(this, "i_d_radr1", "6", "rs1 address"),
    i_d_radr2(this, "i_d_radr2", "6", "rs2 address"),
    i_d_waddr(this, "i_d_waddr", "6", "rd address"),
    i_d_csr_addr(this, "i_d_csr_addr", "12", "decoded CSR address"),
    i_d_imm(this, "i_d_imm", "RISCV_ARCH", "immediate value"),
    i_d_pc(this, "i_d_pc", "CFG_CPU_ADDR_BITS", "Instruction pointer on decoded instruction"),
    i_d_instr(this, "i_d_instr", "32", "Decoded instruction value"),
    i_d_progbuf_ena(this, "i_d_progbuf_ena", "1", "instruction from progbuf passed decoder"),
    i_wb_waddr(this, "i_wb_waddr", "6", "write back address"),
    i_memop_store(this, "i_memop_store", "1", "Store to memory operation"),
    i_memop_load(this, "i_memop_load", "1", "Load from memoru operation"),
    i_memop_sign_ext(this, "i_memop_sign_ext", "1", "Load memory value with sign extending"),
    i_memop_size(this, "i_memop_size", "2", "Memory transaction size"),
    i_unsigned_op(this, "i_unsigned_op", "1", "Unsigned operands"),
    i_rv32(this, "i_rv32", "1", "32-bits instruction"),
    i_compressed(this, "i_compressed", "1", "C-extension (2-bytes length)"),
    i_amo(this, "i_amo", "1", "A-extension (atomic)"),
    i_f64(this, "i_f64", "1", "D-extension (FPU)"),
    i_isa_type(this, "i_isa_type", "ISA_Total", "Type of the instruction's structure (ISA spec.)"),
    i_ivec(this, "i_ivec", "Instr_Total", "One pulse per supported instruction."),
    i_stack_overflow(this, "i_stack_overflow", "1", "exception stack overflow"),
    i_stack_underflow(this, "i_stack_underflow", "1", "exception stack overflow"),
    i_unsup_exception(this, "i_unsup_exception", "1", "Unsupported instruction exception"),
    i_instr_load_fault(this, "i_instr_load_fault", "1", "fault instruction's address. Bus returned ERR on read transaction"),
    i_instr_executable(this, "i_instr_executable", "1", "MPU flag 'executable' not set for this memory region"),
    i_mem_ex_debug(this, "i_mem_ex_debug", "1", "Memoryaccess: Debug requested processed with error. Ignore it."),
    i_mem_ex_load_fault(this, "i_mem_ex_load_fault", "1", "Memoryaccess: Bus response with SLVERR or DECERR on read data"),
    i_mem_ex_store_fault(this, "i_mem_ex_store_fault", "1", "Memoryaccess: Bus response with SLVERR or DECERR on write data"),
    i_mem_ex_mpu_store(this, "i_mem_ex_mpu_store", "1", "Memoryaccess: MPU access error on storing data"),
    i_mem_ex_mpu_load(this, "i_mem_ex_mpu_load", "1", "Memoryaccess: MPU access error on load data"),
    i_mem_ex_addr(this, "i_mem_ex_addr", "CFG_CPU_ADDR_BITS", "Memoryaccess: exception address"),
    i_irq_software(this, "i_irq_software", "1", "software interrupt request from CSR register xSIP"),
    i_irq_timer(this, "i_irq_timer", "1", "interrupt request from wallclock timer"),
    i_irq_external(this, "i_irq_external", "1", "interrupt request from PLIC"),
    i_haltreq(this, "i_haltreq", "1", "halt request from debug unit"),
    i_resumereq(this, "i_resumereq", "1", "resume request from debug unit"),
    i_step(this, "i_step", "1", "resume with step"),
    i_dbg_progbuf_ena(this, "i_dbg_progbuf_ena", "1", "progbuf mode enabled"),
    i_rdata1(this, "i_rdata1", "RISCV_ARCH", "Integer/Float register value 1"),
    i_rtag1(this, "i_rtag1", "CFG_REG_TAG_WIDTH"),
    i_rdata2(this, "i_rdata2", "RISCV_ARCH", "Integer/Float register value 2"),
    i_rtag2(this, "i_rtag2", "CFG_REG_TAG_WIDTH"),
    o_radr1(this, "o_radr1", "6"),
    o_radr2(this, "o_radr2", "6"),
    o_reg_wena(this, "o_reg_wena", "1"),
    o_reg_waddr(this, "o_reg_waddr", "6", "Address to store result of the instruction (0=do not store)"),
    o_reg_wtag(this, "o_reg_wtag", "CFG_REG_TAG_WIDTH"),
    o_reg_wdata(this, "o_reg_wdata", "RISCV_ARCH", "Value to store"),
    o_csr_req_valid(this, "o_csr_req_valid", "1", "Access to CSR request"),
    i_csr_req_ready(this, "i_csr_req_ready", "1", "CSR module is ready to accept request"),
    o_csr_req_type(this, "o_csr_req_type", "CsrReq_TotalBits", "Request type: [0]-read csr; [1]-write csr; [2]-change mode"),
    o_csr_req_addr(this, "o_csr_req_addr", "12", "Requested CSR address"),
    o_csr_req_data(this, "o_csr_req_data", "RISCV_ARCH", "CSR new value"),
    i_csr_resp_valid(this, "i_csr_resp_valid", "1", "CSR module Response is valid"),
    o_csr_resp_ready(this, "o_csr_resp_ready", "1", "Executor is ready to accept response"),
    i_csr_resp_data(this, "i_csr_resp_data", "RISCV_ARCH", "Responded CSR data"),
    i_csr_resp_exception(this, "i_csr_resp_exception", "1", "Raise exception on CSR access"),
    o_memop_valid(this, "o_memop_valid", "1", "Request to memory is valid"),
    o_memop_debug(this, "o_memop_debug", "1", "Debug Request shouldn't modify registers in write back stage"),
    o_memop_sign_ext(this, "o_memop_sign_ext", "1", "Load data with sign extending"),
    o_memop_type(this, "o_memop_type", "MemopType_Total", "[0]: 1=store/0=Load data"),
    o_memop_size(this, "o_memop_size", "2", "0=1bytes; 1=2bytes; 2=4bytes; 3=8bytes"),
    o_memop_memaddr(this, "o_memop_memaddr", "CFG_CPU_ADDR_BITS", "Memory access address"),
    o_memop_wdata(this, "o_memop_wdata", "RISCV_ARCH"),
    i_memop_ready(this, "i_memop_ready", "1"),
    i_dbg_mem_req_valid(this, "i_dbg_mem_req_valid", "1", "Debug Request to memory is valid"),
    i_dbg_mem_req_write(this, "i_dbg_mem_req_write", "1", "0=read; 1=write"),
    i_dbg_mem_req_size(this, "i_dbg_mem_req_size", "2", "0=1bytes; 1=2bytes; 2=4bytes; 3=8bytes"),
    i_dbg_mem_req_addr(this, "i_dbg_mem_req_addr", "CFG_CPU_ADDR_BITS", "Memory access address"),
    i_dbg_mem_req_wdata(this, "i_dbg_mem_req_wdata", "RISCV_ARCH"),
    o_dbg_mem_req_ready(this, "o_dbg_mem_req_ready", "1", "Debug emmory request was accepted"),
    o_dbg_mem_req_error(this, "o_dbg_mem_req_error", "1", "Debug memory reques misaliged"),
    o_valid(this, "o_valid", "1", "Output is valid"),
    o_pc(this, "o_pc", "CFG_CPU_ADDR_BITS", "Valid instruction pointer"),
    o_npc(this, "o_npc", "CFG_CPU_ADDR_BITS", "Next instruction pointer. Next decoded pc must match to this value or will be ignored."),
    o_instr(this, "o_instr", "32", "Valid instruction value"),
    i_flushd_end(this, "i_flushd_end", "1"),
    o_flushd(this, "o_flushd", "1"),
    o_flushi(this, "o_flushi", "1"),
    o_flushi_addr(this, "o_flushi_addr", "CFG_CPU_ADDR_BITS"),
    o_call(this, "o_call", "1", "CALL pseudo instruction detected"),
    o_ret(this, "o_ret", "1", "RET pseudoinstruction detected (hw stack tracing)"),
    o_jmp(this, "o_jmp", "1", "Jump was executed"),
    o_halted(this, "o_halted", "1"),
    // param
    Res_Zero(this, "Res_Zero", "0"),
    Res_Reg2(this, "Res_Reg2", "1"),
    Res_Npc(this, "Res_Npc", "2"),
    Res_Ra(this, "Res_Ra", "3"),
    Res_Csr(this, "Res_Csr", "4"),
    Res_Alu(this, "Res_Alu", "5"),
    Res_AddSub(this, "Res_AddSub", "6"),
    Res_Shifter(this, "Res_Shifter", "7"),
    Res_IMul(this, "Res_IMul", "8"),
    Res_IDiv(this, "Res_IDiv", "9"),
    Res_FPU(this, "Res_FPU", "10"),
    Res_Total(this, "Res_Total", "11"),
    _state0_(this),
    State_Idle(this, "4", "State_Idle", "0"),
    State_WaitMemAcces(this, "4", "State_WaitMemAcces", "1"),
    State_WaitMulti(this, "4", "State_WaitMulti", "2"),
    State_WaitFlushingAccept(this, "4", "State_WaitFlushingAccept", "3"),
    State_Flushing_I(this, "4", "State_Flushing_I", "4"),
    State_Amo(this, "4", "State_Amo", "5"),
    State_Csr(this, "4", "State_Csr", "6"),
    State_Halted(this, "4", "State_Halted", "7"),
    State_DebugMemRequest(this, "4", "State_DebugMemRequest", "8"),
    State_DebugMemError(this, "4", "State_DebugMemError", "9"),
    State_Wfi(this, "4", "State_Wfi", "0xf"),
    _csrstate0_(this),
    CsrState_Idle(this, "2", "CsrState_Idle", "0"),
    CsrState_Req(this, "2", "CsrState_Req", "1"),
    CsrState_Resp(this, "2", "CsrState_Resp", "2"),
    _amostate0_(this),
    AmoState_WaitMemAccess(this, "2", "AmoState_WaitMemAccess", "0"),
    AmoState_Read(this, "2", "AmoState_Read", "1"),
    AmoState_Modify(this, "2", "AmoState_Modify", "2"),
    AmoState_Write(this, "2", "AmoState_Write", "3"),
    // structures
    select_type_def_(this, -1),
    input_mux_type_def_(this),
    // signals
    wb_select(this, "wb_select", "Res_Total"),
    wb_alu_mode(this, "wb_alu_mode", "3"),
    wb_addsub_mode(this, "wb_addsub_mode", "7"),
    wb_shifter_mode(this, "wb_shifter_mode", "4"),
    w_arith_residual_high(this, "w_arith_residual_high", "1"),
    w_mul_hsu(this, "w_mul_hsu", "1"),
    wb_fpu_vec(this, "wb_fpu_vec", "Instr_FPU_Total"),
    w_ex_fpu_invalidop(this, "w_ex_fpu_invalidop", "1", "FPU Exception: invalid operation"),
    w_ex_fpu_divbyzero(this, "w_ex_fpu_divbyzero", "1", "FPU Exception: divide by zero"),
    w_ex_fpu_overflow(this, "w_ex_fpu_overflow", "1", "FPU Exception: overflow"),
    w_ex_fpu_underflow(this, "w_ex_fpu_underflow", "1", "FPU Exception: underflow"),
    w_ex_fpu_inexact(this, "w_ex_fpu_inexact", "1", "FPU Exception: inexact"),
    w_hazard1(this, "w_hazard1", "1"),
    w_hazard2(this, "w_hazard2", "1"),
    //int t_waddr;
    //int t_tagcnt_wr;
    wb_rdata1(this, "wb_rdata1", "RISCV_ARCH"),
    wb_rdata2(this, "wb_rdata2", "RISCV_ARCH"),
    wb_shifter_a1(this, "wb_shifter_a1", "RISCV_ARCH", "Shifters operand 1"),
    wb_shifter_a2(this, "wb_shifter_a2", "6", "Shifters operand 2"),
    tag_expected(this, "tag_expected", "CFG_REG_TAG_WIDTH", "INTREGS_TOTAL"),
    // registers
    state(this, "state", "4", "State_Idle"),
    csrstate(this, "csrstate", "2", "CsrState_Idle"),
    amostate(this, "amostate", "2", "AmoState_WaitMemAccess"),
    pc(this, "pc", "CFG_CPU_ADDR_BITS"),
    npc(this, "npc", "CFG_CPU_ADDR_BITS", "CFG_RESET_VECTOR"),
    dnpc(this, "dnpc", "CFG_CPU_ADDR_BITS"),
    radr1(this, "radr1", "6"),
    radr2(this, "radr2", "6"),
    waddr(this, "waddr", "6"),
    rdata1(this, "rdata1", "RISCV_ARCH"),
    rdata2(this, "rdata2", "RISCV_ARCH"),
    ivec(this, "ivec", "Instr_Total"),
    isa_type(this, "isa_type", "ISA_Total"),
    imm(this, "imm", "RISCV_ARCH"),
    instr(this, "instr", "32"),
    tagcnt(this, "tagcnt", "MUL(CFG_REG_TAG_WIDTH,REGS_TOTAL)", "0", "N-bits tag per register (expected)"),
    reg_write(this, "reg_write", "1"),
    reg_waddr(this, "reg_waddr", "6"),
    reg_wtag(this, "reg_wtag", "CFG_REG_TAG_WIDTH"),
    csr_req_rmw(this, "csr_req_rmw", "1", "0", "csr read-modify-write request"),
    csr_req_type(this, "csr_req_type", "CsrReq_TotalBits"),
    csr_req_addr(this, "csr_req_addr", "12"),
    csr_req_data(this, "csr_req_data", "RISCV_ARCH"),
    memop_valid(this, "memop_valid", "1"),
    memop_debug(this, "memop_debug", "1", "0", "request from dport"),
    memop_halted(this, "memop_halted", "1", "0", "0=return to Idle; 1=return to halt state"),
    memop_type(this, "memop_type", "MemopType_Total"),
    memop_sign_ext(this, "memop_sign_ext", "1"),
    memop_size(this, "memop_size", "2"),
    memop_memaddr(this, "memop_memaddr", "CFG_CPU_ADDR_BITS"),
    memop_wdata(this, "memop_wdata", "RISCV_ARCH"),
    unsigned_op(this, "unsigned_op", "1"),
    rv32(this, "rv32", "1"),
    compressed(this, "compressed", "1"),
    f64(this, "f64", "1"),
    stack_overflow(this, "stack_overflow", "1"),
    stack_underflow(this, "stack_underflow", "1"),
    mem_ex_load_fault(this, "mem_ex_load_fault", "1"),
    mem_ex_store_fault(this, "mem_ex_store_fault", "1"),
    mem_ex_mpu_store(this, "mem_ex_mpu_store", "1"),
    mem_ex_mpu_load(this, "mem_ex_mpu_load", "1"),
    mem_ex_addr(this, "mem_ex_addr", "CFG_CPU_ADDR_BITS"),
    res_npc(this, "res_npc", "CFG_CPU_ADDR_BITS"),
    res_ra(this, "res_ra", "CFG_CPU_ADDR_BITS"),
    res_csr(this, "res_csr", "CFG_CPU_ADDR_BITS"),
    select(this, "select", "Res_Total"),
    valid(this, "valid", "1"),
    call(this, "call", "1"),
    ret(this, "ret", "1"),
    jmp(this, "jmp", "1"),
    flushd(this, "flushd", "1"),
    flushi(this, "flushi", "1"),
    flushi_addr(this, "flushi_addr", "CFG_CPU_ADDR_BITS"),
    stepdone(this, "stepdone", "1"),
    // process
    comb(this)
{
    Operation::start(this);
}

void InstrExecute::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;

    SETVAL(comb.vb_reg_waddr, i_d_waddr);

TEXT();
    IF (EQ(state, State_Idle));
        SETVAL(comb.mux.radr1, i_d_radr1);
        SETVAL(comb.mux.radr2, i_d_radr2);
        SETVAL(comb.mux.waddr, i_d_waddr);
        SETVAL(comb.mux.imm, i_d_imm);
        SETVAL(comb.mux.pc, i_d_pc);
        SETVAL(comb.mux.instr, i_d_instr);
        SETBIT(comb.mux.memop_type, cfg->MemopType_Store, i_memop_store);
        SETBIT(comb.mux.memop_type, cfg->MemopType_Locked, i_amo);
        SETBIT(comb.mux.memop_type, cfg->MemopType_Reserve,
                OR2(BIT(i_ivec, "Instr_LR_D"), BIT(i_ivec, "Instr_LR_W")));
        SETBIT(comb.mux.memop_type, cfg->MemopType_Release,
                OR2(BIT(i_ivec, "Instr_SC_D"), BIT(i_ivec, "Instr_SC_W")));
        SETVAL(comb.mux.memop_sign_ext, i_memop_sign_ext);
        SETVAL(comb.mux.memop_size, i_memop_size);
        SETVAL(comb.mux.unsigned_op, i_unsigned_op);
        SETVAL(comb.mux.rv32, i_rv32);
        SETVAL(comb.mux.compressed, i_compressed);
        SETVAL(comb.mux.f64, i_f64);
        SETVAL(comb.mux.ivec, i_ivec);
        SETVAL(comb.mux.isa_type, i_isa_type);
    ELSE();
        SETVAL(comb.mux.radr1, radr1);
        SETVAL(comb.mux.radr2, radr2);
        SETVAL(comb.mux.waddr, waddr);
        SETVAL(comb.mux.imm, imm);
        SETVAL(comb.mux.pc, pc);
        SETVAL(comb.mux.instr, instr);
        SETVAL(comb.mux.memop_type, memop_type);
        SETVAL(comb.mux.memop_sign_ext, memop_sign_ext);
        SETVAL(comb.mux.memop_size, memop_size);
        SETVAL(comb.mux.unsigned_op, unsigned_op);
        SETVAL(comb.mux.rv32, rv32);
        SETVAL(comb.mux.compressed, compressed);
        SETVAL(comb.mux.f64, f64);
        SETVAL(comb.mux.ivec, ivec);
        SETVAL(comb.mux.isa_type, isa_type);
    ENDIF();
    SETVAL(comb.wv, comb.mux.ivec);

TEXT();
    IF (NZ(BIT(comb.mux.isa_type, cfg->ISA_R_type)));
        SETVAL(comb.vb_rdata1, i_rdata1);
        SETVAL(comb.vb_rdata2, i_rdata2);
        SETONE(comb.v_check_tag1);
        SETONE(comb.v_check_tag2);
    ELSIF (NZ(BIT(comb.mux.isa_type, cfg->ISA_I_type)));
        SETVAL(comb.vb_rdata1, i_rdata1);
        SETVAL(comb.vb_rdata2, comb.mux.imm);
        SETONE(comb.v_check_tag1);
    ELSIF (NZ(BIT(comb.mux.isa_type, cfg->ISA_SB_type)));
        SETVAL(comb.vb_rdata1, i_rdata1);
        SETVAL(comb.vb_rdata2, i_rdata2);
        SETVAL(comb.vb_off, comb.mux.imm);
        SETONE(comb.v_check_tag1);
        SETONE(comb.v_check_tag2);
    ELSIF (NZ(BIT(comb.mux.isa_type, cfg->ISA_UJ_type)));
        SETVAL(comb.vb_rdata1, comb.mux.pc);
        SETVAL(comb.vb_off, comb.mux.imm);
        SETONE(comb.v_check_tag1);
    ELSIF (NZ(BIT(comb.mux.isa_type, cfg->ISA_U_type)));
        SETVAL(comb.vb_rdata1, comb.mux.pc);
        SETVAL(comb.vb_rdata2, comb.mux.imm);
    ELSIF (NZ(BIT(comb.mux.isa_type, cfg->ISA_S_type)));
        SETVAL(comb.vb_rdata1, i_rdata1);
        SETVAL(comb.vb_rdata2, i_rdata2);
        SETVAL(comb.vb_off, comb.mux.imm);
        SETONE(comb.v_check_tag1);
        SETONE(comb.v_check_tag2);
    ENDIF();

TEXT();
    SETVAL(comb.vb_memop_memaddr_load, ADD2(BITS(comb.vb_rdata1, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0")),
                                            BITS(comb.vb_rdata2, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0"))));
    SETVAL(comb.vb_memop_memaddr_store, ADD2(BITS(comb.vb_rdata1, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0")),
                                            BITS(comb.vb_off, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0"))));
    IF (EZ(BIT(comb.mux.memop_type, cfg->MemopType_Store)));
        SETVAL(comb.vb_memop_memaddr, comb.vb_memop_memaddr_load);
    ELSE();
        SETVAL(comb.vb_memop_memaddr, comb.vb_memop_memaddr_store);
    ENDIF();


TEXT();
    TEXT("Check that registers tags are equal to expeted ones");
    SETVAL(comb.t_radr1, TO_INT(comb.mux.radr1));
    SETVAL(comb.t_radr2, TO_INT(comb.mux.radr2));

    SETZERO(w_hazard1);
    IF (NE(BITS(tagcnt, DEC(MUL2(INC(cfg->CFG_REG_TAG_WIDTH), comb.t_radr1)),
                        MUL2(cfg->CFG_REG_TAG_WIDTH, comb.t_radr1)), i_rtag1));
        SETVAL(w_hazard1, comb.v_check_tag1);
    ENDIF();
    SETZERO(w_hazard2);
    IF (NE(BITS(tagcnt, DEC(MUL2(INC(cfg->CFG_REG_TAG_WIDTH), comb.t_radr2)),
                        MUL2(cfg->CFG_REG_TAG_WIDTH, comb.t_radr2)), i_rtag2));
        SETVAL(w_hazard2, comb.v_check_tag2);
    ENDIF();

TEXT();
    TEXT("Compute branch conditions:");
    SETVAL(comb.vb_sub64, SUB2(comb.vb_rdata1, comb.vb_rdata2));
    IF (EZ(comb.vb_sub64));
        SETONE(comb.v_eq, "equal");
    ENDIF();
    SETVAL(comb.v_ge, INV(BIT(comb.vb_sub64, 63)), "greater/equal (signed)");
    SETZERO(comb.v_geu);
    IF (GE(comb.vb_rdata1, comb.vb_rdata2));
        SETONE(comb.v_geu, "greater/equal (unsigned)");
    ENDIF();
    SETVAL(comb.v_lt, BIT(comb.vb_sub64, 63), "less (signed)");
    SETZERO(comb.v_ltu);
    IF (LS(comb.vb_rdata1, comb.vb_rdata2));
        SETONE(comb.v_ltu, "less (unsiged)");
    ENDIF();
    IF (NZ(comb.vb_sub64));
        SETONE(comb.v_neq, "not equal");
    ENDIF();

TEXT();
    TEXT("Relative Branch on some condition:");
    SETZERO(comb.v_pc_branch);
    IF (NZ(ORx(6, &AND2(BIT(comb.wv, "Instr_BEQ"), comb.v_eq),
                &AND2(BIT(comb.wv, "Instr_BGE"), comb.v_ge),
                &AND2(BIT(comb.wv, "Instr_BGEU"), comb.v_geu),
                &AND2(BIT(comb.wv, "Instr_BLT"), comb.v_lt),
                &AND2(BIT(comb.wv, "Instr_BLTU"), comb.v_ltu),
                &AND2(BIT(comb.wv, "Instr_BNE"), comb.v_neq))));
        SETONE(comb.v_pc_branch);
    ENDIF();

TEXT();
    SETVAL(wb_fpu_vec, BITS(comb.wv, cfg->Instr_FPU_Last, cfg->Instr_FPU_First), "directly connected i_ivec");
    SETVAL(comb.v_fence_d, BIT(comb.wv, "Instr_FENCE"));
    SETVAL(comb.v_fence_i, BIT(comb.wv, "Instr_FENCE_I"));
    SETVAL(w_arith_residual_high, ORx(7, &BIT(comb.wv, "Instr_REM"),
                                         &BIT(comb.wv, "Instr_REMU"),
                                         &BIT(comb.wv, "Instr_REMW"),
                                         &BIT(comb.wv, "Instr_REMUW"),
                                         &BIT(comb.wv, "Instr_MULH"),
                                         &BIT(comb.wv, "Instr_MULHSU"),
                                         &BIT(comb.wv, "Instr_MULHU")));

    SETVAL(w_mul_hsu, BIT(comb.wv, "Instr_MULHSU"));

TEXT();
    SETVAL(comb.v_instr_misaligned, BIT(comb.mux.pc, 0));
    IF (ORx(3, &AND2(NZ(BIT(comb.wv, "Instr_LD")), NZ(BITS(comb.vb_memop_memaddr_load, 2, 0))),
               &AND2(NZ(OR2(BIT(comb.wv, "Instr_LW"), BIT(comb.wv, "Instr_LWU"))), NZ(BITS(comb.vb_memop_memaddr_load, 1, 0))),
               &AND2(NZ(OR2(BIT(comb.wv, "Instr_LH"), BIT(comb.wv, "Instr_LHU"))), NZ(BIT(comb.vb_memop_memaddr_load, 0)))));
        SETONE(comb.v_load_misaligned);
    ENDIF();
    IF (ORx(3, &NZ(AND2(BIT(comb.wv, "Instr_SD"), NZ(BITS(comb.vb_memop_memaddr_store, 2, 0)))),
               &NZ(AND2(BIT(comb.wv, "Instr_SW"), NZ(BITS(comb.vb_memop_memaddr_store, 1, 0)))),
               &NZ(AND2(BIT(comb.wv, "Instr_SH"), NZ(BIT(comb.vb_memop_memaddr_store, 0))))));
        SETONE(comb.v_store_misaligned);
    ENDIF();
    IF (ORx(3, &AND2(EQ(i_dbg_mem_req_size, CONST("3", 2)), NZ(BITS(i_dbg_mem_req_addr, 2, 0))),
               &AND2(EQ(i_dbg_mem_req_size, CONST("2", 2)), NZ(BITS(i_dbg_mem_req_addr, 1, 0))),
               &AND2(EQ(i_dbg_mem_req_size, CONST("1", 2)), NZ(BIT(i_dbg_mem_req_addr, 0)))));
        SETONE(comb.v_debug_misaligned);
    ENDIF();
    IF (NZ(i_stack_overflow));
        SETONE(stack_overflow);
    ENDIF();
    IF (NZ(i_stack_underflow));
        SETONE(stack_underflow);
    ENDIF();
    IF (AND2(NZ(i_mem_ex_load_fault), EZ(i_mem_ex_debug)));
        SETONE(mem_ex_load_fault);
        SETVAL(mem_ex_addr, i_mem_ex_addr);
    ENDIF();
    IF (AND2(NZ(i_mem_ex_store_fault), EZ(i_mem_ex_debug)));
        SETONE(mem_ex_store_fault);
        SETVAL(mem_ex_addr, i_mem_ex_addr);
    ENDIF();
    IF (AND2(NZ(i_mem_ex_mpu_store), EZ(i_mem_ex_debug)));
        SETONE(mem_ex_mpu_store);
        SETVAL(mem_ex_addr, i_mem_ex_addr);
    ENDIF();
    IF (AND2(NZ(i_mem_ex_mpu_load), EZ(i_mem_ex_debug)));
        SETONE(mem_ex_mpu_load);
        SETVAL(mem_ex_addr, i_mem_ex_addr);
    ENDIF();

TEXT();
    SETVAL(comb.opcode_len, CONST("4", 3));
    IF (NZ(comb.mux.compressed));
        SETVAL(comb.opcode_len, CONST("2", 3));
    ENDIF();
    SETVAL(comb.vb_npc_incr, ADD2(comb.mux.pc, comb.opcode_len));

TEXT();
    IF (NZ(comb.v_pc_branch));
        SETVAL(comb.vb_prog_npc, ADD2(comb.mux.pc, BITS(comb.vb_off, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0"))));
    ELSIF (NZ(BIT(comb.wv, "Instr_JAL")));
        SETVAL(comb.vb_prog_npc, ADD2(BITS(comb.vb_rdata1, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0")),
                                      BITS(comb.vb_off, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0"))));
    ELSIF (NZ(BIT(comb.wv, "Instr_JALR")));
        SETVAL(comb.vb_prog_npc, ADD2(BITS(comb.vb_rdata1, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0")),
                                      BITS(comb.vb_rdata2, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0"))));
        SETBITZERO(comb.vb_prog_npc, 0);
    ELSE();
        SETVAL(comb.vb_prog_npc, comb.vb_npc_incr);
    ENDIF();

TEXT();
    SETBIT(comb.vb_select, Res_Reg2, ORx(2, &BIT(comb.mux.memop_type, cfg->MemopType_Store),
                                            &BIT(comb.wv, "Instr_LUI")));
    SETBITZERO(comb.vb_select, Res_Npc);
    SETBIT(comb.vb_select, Res_Ra, ORx(5, &comb.v_pc_branch,
                                          &BIT(comb.wv, "Instr_JAL"),
                                          &BIT(comb.wv, "Instr_JALR"),
                                          &BIT(comb.wv, "Instr_MRET"),
                                          &BIT(comb.wv, "Instr_URET")));
    SETBIT(comb.vb_select, Res_Csr, ORx(6, &BIT(comb.wv, "Instr_CSRRC"),
                                           &BIT(comb.wv, "Instr_CSRRCI"),
                                           &BIT(comb.wv, "Instr_CSRRS"),
                                           &BIT(comb.wv, "Instr_CSRRSI"),
                                           &BIT(comb.wv, "Instr_CSRRW"),
                                           &BIT(comb.wv, "Instr_CSRRWI")));
    SETBIT(comb.vb_select, Res_Alu, ORx(12, &BIT(comb.wv, "Instr_AND"),
                                            &BIT(comb.wv, "Instr_ANDI"),
                                            &BIT(comb.wv, "Instr_OR"),
                                            &BIT(comb.wv, "Instr_ORI"),
                                            &BIT(comb.wv, "Instr_XOR"),
                                            &BIT(comb.wv, "Instr_XORI"),
                                            &BIT(comb.wv, "Instr_AMOOR_D"),
                                            &BIT(comb.wv, "Instr_AMOOR_W"),
                                            &BIT(comb.wv, "Instr_AMOAND_D"),
                                            &BIT(comb.wv, "Instr_AMOAND_W"),
                                            &BIT(comb.wv, "Instr_AMOXOR_D"),
                                            &BIT(comb.wv, "Instr_AMOXOR_W")));
    SETBIT(comb.vb_select, Res_AddSub, ORx(23, &BIT(comb.wv, "Instr_ADD"),
                                             &BIT(comb.wv, "Instr_ADDI"),
                                             &BIT(comb.wv, "Instr_AUIPC"),
                                             &BIT(comb.wv, "Instr_ADDW"),
                                             &BIT(comb.wv, "Instr_ADDIW"),
                                             &BIT(comb.wv, "Instr_SUB"),
                                             &BIT(comb.wv, "Instr_SUBW"),
                                             &BIT(comb.wv, "Instr_SLT"),
                                             &BIT(comb.wv, "Instr_SLTI"),
                                             &BIT(comb.wv, "Instr_SLTU"),
                                             &BIT(comb.wv, "Instr_SLTIU"),
                                             &BIT(comb.wv, "Instr_AMOADD_D"),
                                             &BIT(comb.wv, "Instr_AMOADD_W"),
                                             &BIT(comb.wv, "Instr_AMOMIN_D"),
                                             &BIT(comb.wv, "Instr_AMOMIN_W"),
                                             &BIT(comb.wv, "Instr_AMOMAX_D"),
                                             &BIT(comb.wv, "Instr_AMOMAX_W"),
                                             &BIT(comb.wv, "Instr_AMOMINU_D"),
                                             &BIT(comb.wv, "Instr_AMOMINU_W"),
                                             &BIT(comb.wv, "Instr_AMOMAXU_D"),
                                             &BIT(comb.wv, "Instr_AMOMAXU_W"),
                                             &BIT(comb.wv, "Instr_AMOSWAP_D"),
                                             &BIT(comb.wv, "Instr_AMOSWAP_W")));
    SETBIT(comb.vb_select, Res_Shifter, ORx(13, &BIT(comb.wv, "Instr_SLL"),
                                              &BIT(comb.wv, "Instr_SLLI"),
                                              &BIT(comb.wv, "Instr_SLLW"),
                                              &BIT(comb.wv, "Instr_SLLIW"),
                                              &BIT(comb.wv, "Instr_SRL"),
                                              &BIT(comb.wv, "Instr_SRLI"),
                                              &BIT(comb.wv, "Instr_SRLW"),
                                              &BIT(comb.wv, "Instr_SRLIW"),
                                              &BIT(comb.wv, "Instr_SRA"),
                                              &BIT(comb.wv, "Instr_SRAI"),
                                              &BIT(comb.wv, "Instr_SRAW"),
                                              &BIT(comb.wv, "Instr_SRAW"),
                                              &BIT(comb.wv, "Instr_SRAIW")));
    SETBIT(comb.vb_select, Res_IMul, ORx(5, &BIT(comb.wv, "Instr_MUL"),
                                         &BIT(comb.wv, "Instr_MULW"),
                                         &BIT(comb.wv, "Instr_MULH"),
                                         &BIT(comb.wv, "Instr_MULHSU"),
                                         &BIT(comb.wv, "Instr_MULHU")));
    SETBIT(comb.vb_select, Res_IDiv, ORx(8, &BIT(comb.wv, "Instr_DIV"),
                                        &BIT(comb.wv, "Instr_DIVU"),
                                        &BIT(comb.wv, "Instr_DIVW"),
                                        &BIT(comb.wv, "Instr_DIVUW"),
                                        &BIT(comb.wv, "Instr_REM"),
                                        &BIT(comb.wv, "Instr_REMU"),
                                        &BIT(comb.wv, "Instr_REMW"),
                                        &BIT(comb.wv, "Instr_REMUW")));
    IF (fpu_ena);
        SETBIT(comb.vb_select, Res_FPU, AND2(comb.mux.f64, INV(OR2(BIT(comb.wv, "Instr_FSD"),
                                                                   BIT(comb.wv, "Instr_FLD")))));
    ENDIF();
    IF (EZ(BITS(comb.vb_select, DEC(Res_Total), INC(Res_Zero))));
        SETBITONE(comb.vb_select, Res_Zero, "load memory, fence");
    ENDIF();

TEXT();
    IF (AND2(NZ(OR2(BIT(comb.wv, "Instr_JAL"), BIT(comb.wv, "Instr_JALR"))),
             EQ(comb.mux.waddr, cfg->REG_RA)));
        SETONE(comb.v_call);
    ENDIF();
    IF (AND4(NZ(BIT(comb.wv, "Instr_JALR")),
             EZ(comb.vb_rdata2),
             NE(comb.mux.waddr, cfg->REG_RA),
             EQ(comb.mux.radr1, cfg->REG_RA)));
        SETONE(comb.v_ret);
    ENDIF();

    SETVAL(comb.v_mem_ex, ORx(4, &mem_ex_load_fault,
                                 &mem_ex_store_fault,
                                 &mem_ex_mpu_store,
                                 &mem_ex_mpu_load));
    SETVAL(comb.v_csr_cmd_ena, ORx(25, &i_haltreq,
                                    &AND2(i_step, stepdone),
                                    &i_unsup_exception,
                                    &i_instr_load_fault,
                                    &comb.v_mem_ex,
                                    &INV(i_instr_executable),
                                    &stack_overflow,
                                    &stack_underflow,
                                    &comb.v_instr_misaligned,
                                    &comb.v_load_misaligned,
                                    &comb.v_store_misaligned,
                                    &i_irq_software,
                                    &i_irq_timer,
                                    &i_irq_external,
                                    &BIT(comb.wv, "Instr_WFI"),
                                    &BIT(comb.wv, "Instr_EBREAK"),
                                    &BIT(comb.wv, "Instr_ECALL"),
                                    &BIT(comb.wv, "Instr_MRET"),
                                    &BIT(comb.wv, "Instr_URET"),
                                    &BIT(comb.wv, "Instr_CSRRC"),
                                    &BIT(comb.wv, "Instr_CSRRCI"),
                                    &BIT(comb.wv, "Instr_CSRRS"),
                                    &BIT(comb.wv, "Instr_CSRRSI"),
                                    &BIT(comb.wv, "Instr_CSRRW"),
                                    &BIT(comb.wv, "Instr_CSRRWI")));
    IF (NZ(i_haltreq));
        SETVAL(comb.vb_csr_cmd_type, cfg->CsrReq_HaltCmd);
        SETVAL(comb.vb_csr_cmd_addr, cfg->HALT_CAUSE_HALTREQ);
    ELSIF (AND2(NZ(i_step), NZ(stepdone)));
        SETVAL(comb.vb_csr_cmd_type, cfg->CsrReq_HaltCmd);
        SETVAL(comb.vb_csr_cmd_addr, cfg->HALT_CAUSE_STEP);
    ELSIF (NZ(comb.v_instr_misaligned));
        SETVAL(comb.vb_csr_cmd_type, cfg->CsrReq_ExceptionCmd);
        SETVAL(comb.vb_csr_cmd_addr, cfg->EXCEPTION_InstrMisalign, "Instruction address misaligned");
        SETVAL(comb.vb_csr_cmd_wdata, comb.mux.pc);
    ELSIF (OR2(NZ(i_instr_load_fault), EZ(i_instr_executable)));
        SETVAL(comb.vb_csr_cmd_type, cfg->CsrReq_ExceptionCmd);
        SETVAL(comb.vb_csr_cmd_addr, cfg->EXCEPTION_InstrFault, "Instruction access fault");
        SETVAL(comb.vb_csr_cmd_wdata, comb.mux.pc);
    ELSIF (i_unsup_exception);
        SETVAL(comb.vb_csr_cmd_type, cfg->CsrReq_ExceptionCmd);
        SETVAL(comb.vb_csr_cmd_addr, cfg->EXCEPTION_InstrIllegal, "Illegal instruction");
        SETVAL(comb.vb_csr_cmd_wdata, comb.mux.instr);
    ELSIF (NZ(BIT(comb.wv, "Instr_EBREAK")));
        SETVAL(comb.vb_csr_cmd_type, cfg->CsrReq_BreakpointCmd);
        SETVAL(comb.vb_csr_cmd_addr, cfg->EXCEPTION_Breakpoint);
    ELSIF (NZ(comb.v_load_misaligned));
        SETVAL(comb.vb_csr_cmd_type, cfg->CsrReq_ExceptionCmd);
        SETVAL(comb.vb_csr_cmd_addr, cfg->EXCEPTION_LoadMisalign, "Load address misaligned");
        SETVAL(comb.vb_csr_cmd_wdata, comb.vb_memop_memaddr_load);
    ELSIF (NZ(OR2(mem_ex_load_fault, mem_ex_mpu_load)));
        SETVAL(comb.vb_csr_cmd_type, cfg->CsrReq_ExceptionCmd);
        SETVAL(comb.vb_csr_cmd_addr, cfg->EXCEPTION_LoadFault, "Load access fault");
        SETVAL(comb.vb_csr_cmd_wdata, mem_ex_addr);
    ELSIF (NZ(comb.v_store_misaligned));
        SETVAL(comb.vb_csr_cmd_type, cfg->CsrReq_ExceptionCmd);
        SETVAL(comb.vb_csr_cmd_addr, cfg->EXCEPTION_StoreMisalign, "Store/AMO address misaligned");
        SETVAL(comb.vb_csr_cmd_wdata, comb.vb_memop_memaddr_store);
    ELSIF (NZ(OR2(mem_ex_store_fault, mem_ex_mpu_store)));
        SETVAL(comb.vb_csr_cmd_type, cfg->CsrReq_ExceptionCmd);
        SETVAL(comb.vb_csr_cmd_addr, cfg->EXCEPTION_StoreFault, "Store/AMO access fault");
        SETVAL(comb.vb_csr_cmd_wdata, mem_ex_addr);
    ELSIF (NZ(stack_overflow));
        SETVAL(comb.vb_csr_cmd_type, cfg->CsrReq_ExceptionCmd);
        SETVAL(comb.vb_csr_cmd_addr, cfg->EXCEPTION_StackOverflow, "Stack overflow");
    ELSIF (NZ(stack_underflow));
        SETVAL(comb.vb_csr_cmd_type, cfg->CsrReq_ExceptionCmd);
        SETVAL(comb.vb_csr_cmd_addr, cfg->EXCEPTION_StackUnderflow, "Stack Underflow");
    ELSIF (NZ(BIT(comb.wv, "Instr_ECALL")));
        SETVAL(comb.vb_csr_cmd_type, cfg->CsrReq_ExceptionCmd);
        SETVAL(comb.vb_csr_cmd_addr, cfg->EXCEPTION_CallFromXMode, "Environment call");
    ELSIF (NZ(i_irq_software));
        SETVAL(comb.vb_csr_cmd_type, cfg->CsrReq_InterruptCmd);
        SETVAL(comb.vb_csr_cmd_addr, cfg->INTERRUPT_XSoftware, "Software interrupt request");
    ELSIF (NZ(i_irq_timer));
        SETVAL(comb.vb_csr_cmd_type, cfg->CsrReq_InterruptCmd);
        SETVAL(comb.vb_csr_cmd_addr, cfg->INTERRUPT_XTimer, "Timer interrupt request");
    ELSIF (NZ(i_irq_external));
        SETVAL(comb.vb_csr_cmd_type, cfg->CsrReq_InterruptCmd);
        SETVAL(comb.vb_csr_cmd_addr, cfg->INTERRUPT_XExternal, "PLIC interrupt request");
    ELSIF (NZ(BIT(comb.wv, "Instr_WFI")));
        SETVAL(comb.vb_csr_cmd_type, cfg->CsrReq_WfiCmd);
        SETVAL(comb.vb_csr_cmd_addr, BITS(comb.mux.instr, 14, 12), "PRIV field");
    ELSIF (NZ(BIT(comb.wv, "Instr_MRET")));
        SETVAL(comb.vb_csr_cmd_type, cfg->CsrReq_TrapReturnCmd);
        SETVAL(comb.vb_csr_cmd_addr, cfg->CSR_mepc);
    ELSIF (NZ(BIT(comb.wv, "Instr_URET")));
        SETVAL(comb.vb_csr_cmd_type, cfg->CsrReq_TrapReturnCmd);
        SETVAL(comb.vb_csr_cmd_addr, cfg->CSR_uepc);
    ELSIF (NZ(BIT(comb.wv, "Instr_CSRRC")));
        SETVAL(comb.vb_csr_cmd_type, cfg->CsrReq_ReadCmd);
        SETVAL(comb.vb_csr_cmd_addr, i_d_csr_addr);
        SETVAL(comb.vb_csr_cmd_wdata, AND2_L(i_csr_resp_data, INV_L(rdata1)));
    ELSIF (NZ(BIT(comb.wv, "Instr_CSRRCI")));
        SETVAL(comb.vb_csr_cmd_type, cfg->CsrReq_ReadCmd);
        SETVAL(comb.vb_csr_cmd_addr, i_d_csr_addr);
        SETBITS(comb.vb_csr_cmd_wdata, DEC(cfg->RISCV_ARCH), CONST("5"), BITS(i_csr_resp_data, DEC(cfg->RISCV_ARCH), CONST("5")));
        SETBITS(comb.vb_csr_cmd_wdata, 4, 0, AND2_L(BITS(i_csr_resp_data, 4, 0), INV_L(BITS(radr1, 4, 0))), "zero-extending 5 to 64-bits");
    ELSIF (NZ(BIT(comb.wv, "Instr_CSRRS")));
        SETVAL(comb.vb_csr_cmd_type, cfg->CsrReq_ReadCmd);
        SETVAL(comb.vb_csr_cmd_addr, i_d_csr_addr);
        SETVAL(comb.vb_csr_cmd_wdata, OR2_L(i_csr_resp_data, rdata1));
    ELSIF (NZ(BIT(comb.wv, "Instr_CSRRSI")));
        SETVAL(comb.vb_csr_cmd_type, cfg->CsrReq_ReadCmd);
        SETVAL(comb.vb_csr_cmd_addr, i_d_csr_addr);
        SETBITS(comb.vb_csr_cmd_wdata, DEC(cfg->RISCV_ARCH), CONST("5"), BITS(i_csr_resp_data, DEC(cfg->RISCV_ARCH), CONST("5")));
        SETBITS(comb.vb_csr_cmd_wdata, 4, 0, OR2_L(BITS(i_csr_resp_data, 4, 0), BITS(radr1, 4, 0)), "zero-extending 5 to 64-bits");
    ELSIF (NZ(BIT(comb.wv, "Instr_CSRRW")));
        SETVAL(comb.vb_csr_cmd_type, cfg->CsrReq_ReadCmd);
        SETVAL(comb.vb_csr_cmd_addr, i_d_csr_addr);
        SETVAL(comb.vb_csr_cmd_wdata, rdata1);
    ELSIF (NZ(BIT(comb.wv, "Instr_CSRRWI")));
        SETVAL(comb.vb_csr_cmd_type, cfg->CsrReq_ReadCmd);
        SETVAL(comb.vb_csr_cmd_addr, i_d_csr_addr);
        SETBITS(comb.vb_csr_cmd_wdata, 4, 0, BITS(radr1, 4, 0), "zero-extending 5 to 64-bits");
    ENDIF();
}

