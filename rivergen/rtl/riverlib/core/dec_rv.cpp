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

#include "dec_rv.h"

DecoderRv::DecoderRv(GenObject *parent, const char *name) :
    ModuleObject(parent, "DecoderRv", name),
    fpu_ena(this, "fpu_ena", "true"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_flush_pipeline(this, "i_flush_pipeline", "1", "reset pipeline and cache"),
    i_progbuf_ena(this, "i_progbuf_ena", "1", "executing from progbuf"),
    i_f_pc(this, "i_f_pc", "CFG_CPU_ADDR_BITS", "Fetched pc"),
    i_f_instr(this, "i_f_instr", "32", "Fetched instruction value"),
    i_instr_load_fault(this, "i_instr_load_fault", "1", "fault instruction's address"),
    i_instr_executable(this, "i_instr_executable", "1", "MPU flag"),
    o_radr1(this, "o_radr1", "6", "register bank address 1 (rs1)"),
    o_radr2(this, "o_radr2", "6", "register bank address 2 (rs2)"),
    o_waddr(this, "o_waddr", "6", "register bank output (rd)"),
    o_csr_addr(this, "o_csr_addr", "12", "CSR bank output"),
    o_imm(this, "o_imm", "RISCV_ARCH", "immediate constant decoded from instruction"),
    o_pc(this, "o_pc", "CFG_CPU_ADDR_BITS", "Current instruction pointer value"),
    o_instr(this, "o_instr", "32", "Current instruction value"),
    o_memop_store(this, "o_memop_store", "1", "Store to memory operation"),
    o_memop_load(this, "o_memop_load", "1", "Load from memoru operation"),
    o_memop_sign_ext(this, "o_memop_sign_ext", "1", "Load memory value with sign extending"),
    o_memop_size(this, "o_memop_size", "2", "Memory transaction size"),
    o_rv32(this, "o_rv32", "1", "32-bits instruction"),
    o_compressed(this, "o_compressed", "1", "C-type instruction"),
    o_amo(this, "o_amo", "1", "A-type instruction"),
    o_f64(this, "o_f64", "1", "64-bits FPU (D-extension)"),
    o_unsigned_op(this, "o_unsigned_op", "1", "Unsigned operands"),
    o_isa_type(this, "o_isa_type", "ISA_Total", "Instruction format accordingly with ISA"),
    o_instr_vec(this, "o_instr_vec", "Instr_Total", "One bit per decoded instruction bus"),
    o_exception(this, "o_exception", "1", "Exception detected"),
    o_instr_load_fault(this, "o_instr_load_fault", "1", "fault instruction's address"),
    o_instr_executable(this, "o_instr_executable", "1", "MPU flag"),
    o_progbuf_ena(this, "o_progbuf_ena", "1", "Debug execution from progbuf"),
    // param
    OPCODE_LB(this, "5", "OPCODE_LB", "0x00", "00000: LB, LH, LW, LD, LBU, LHU, LWU"),
    OPCODE_FPU_LD(this, "5", "OPCODE_FPU_LD", "0x01", "00001: FLD"),
    OPCODE_FENCE(this, "5", "OPCODE_FENCE", "0x03", "00011: FENCE, FENCE_I"),
    OPCODE_ADDI(this, "5", "OPCODE_ADDI", "0x04", "00100: ADDI, ANDI, ORI, SLLI, SLTI, SLTIU, SRAI, SRLI, XORI"),
    OPCODE_AUIPC(this, "5", "OPCODE_AUIPC", "0x05", "00101: AUIPC"),
    OPCODE_ADDIW(this, "5", "OPCODE_ADDIW", "0x06", "00110: ADDIW, SLLIW, SRAIW, SRLIW"),
    OPCODE_SB(this, "5", "OPCODE_SB", "0x08", "01000: SB, SH, SW, SD"),
    OPCODE_FPU_SD(this, "5", "OPCODE_FPU_SD", "0x09", "01001: FSD"),
    OPCODE_AMO(this, "5", "OPCODE_AMO", "0x0B", "01011: Atomic opcode (AMO)"),
    OPCODE_ADD(this, "5", "OPCODE_ADD", "0x0C", "01100: ADD, AND, OR, SLT, SLTU, SLL, SRA, SRL, SUB, XOR, DIV, DIVU, MUL, REM, REMU"),
    OPCODE_LUI(this, "5", "OPCODE_LUI", "0x0D", "01101: LUI"),
    OPCODE_ADDW(this, "5", "OPCODE_ADDW", "0x0E", "01110: ADDW, SLLW, SRAW, SRLW, SUBW, DIVW, DIVUW, MULW, REMW, REMUW"),
    OPCODE_FPU_OP(this, "5", "OPCODE_FPU_OP", "0x14", "10100: FPU operation"),
    OPCODE_BEQ(this, "5", "OPCODE_BEQ", "0x18", "11000: BEQ, BNE, BLT, BGE, BLTU, BGEU"),
    OPCODE_JALR(this, "5", "OPCODE_JALR", "0x19", "11001: JALR"),
    OPCODE_JAL(this, "5", "OPCODE_JAL", "0x1B", "11011: JAL"),
    OPCODE_CSRR(this, "5", "OPCODE_CSRR", "0x1C", "11100: CSRRC, CSRRCI, CSRRS, CSRRSI, CSRRW, CSRRWI, URET, SRET, HRET, MRET"),
    // signals
    // registers
    pc(this, "pc", "CFG_CPU_ADDR_BITS", "-1"),
    isa_type(this, "isa_type", "ISA_Total"),
    instr_vec(this, "instr_vec", "Instr_Total"),
    instr(this, "instr", "32", "-1"),
    memop_store(this, "memop_store", "1"),
    memop_load(this, "memop_load", "1"),
    memop_sign_ext(this, "memop_sign_ext", "1"),
    memop_size(this, "memop_size", "2", "MEMOP_1B"),
    unsigned_op(this, "unsigned_op", "1"),
    rv32(this, "rv32", "1"),
    f64(this, "f64", "1"),
    compressed(this, "compressed", "1"),
    amo(this, "amo", "1"),
    instr_load_fault(this, "instr_load_fault", "1"),
    instr_executable(this, "instr_executable", "1"),
    instr_unimplemented(this, "instr_unimplemented", "1"),
    radr1(this, "radr1", "6"),
    radr2(this, "radr2", "6"),
    waddr(this, "waddr", "6"),
    csr_addr(this, "csr_addr", "12"),
    imm(this, "imm", "RISCV_ARCH"),
    progbuf_ena(this, "progbuf_ena", "1"),
    // process
    comb(this)
{
    Operation::start(this);
}

void DecoderRv::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;

    SETZERO(comb.v_error);
    SETZERO(comb.v_compressed);
    SETVAL(comb.vb_instr, i_f_instr);
    SETZERO(comb.vb_dec);
    SETZERO(comb.vb_isa_type);
    SETZERO(comb.vb_radr1);
    SETZERO(comb.vb_radr2);
    SETZERO(comb.vb_waddr);
    SETZERO(comb.vb_csr_addr);
    SETZERO(comb.vb_imm);
    SETZERO(comb.v_memop_store);
    SETZERO(comb.v_memop_load);
    SETZERO(comb.v_memop_sign_ext);
    SETZERO(comb.vb_memop_size);
    SETZERO(comb.v_unsigned_op);
    SETZERO(comb.v_rv32);
    SETZERO(comb.v_f64);
    SETZERO(comb.v_amo);

TEXT();
    IF (NE(BITS(comb.vb_instr, 1, 0), CONST("0x3", 2)));
        SETONE(comb.v_compressed);
    ENDIF();

TEXT();
    SETVAL(comb.vb_opcode1, BITS(comb.vb_instr, 6, 2));
    SETVAL(comb.vb_opcode2, BITS(comb.vb_instr, 14, 12));
    SWITCH(comb.vb_opcode1);
    CASE (OPCODE_AMO);
        SETBITONE(comb.vb_isa_type, cfg->ISA_R_type);
        SETVAL(comb.vb_radr1, CC2(CONST("0", 1), BITS(comb.vb_instr, 19, 15)));
        SETVAL(comb.vb_radr2, CC2(CONST("0", 1), BITS(comb.vb_instr, 24, 20)));
        SETVAL(comb.vb_waddr, BITS(comb.vb_instr, 11, 7));
        SWITCH (BITS(comb.vb_instr, 31, 27));
        CASE (CONST("0x0", 5));
            IF (EQ(comb.vb_opcode2, CONST("2", 3)));
                SETBITONE(comb.vb_dec, "Instr_AMOADD_W");
            ELSIF (EQ(comb.vb_opcode2, CONST("3", 3)));
                SETBITONE(comb.vb_dec, "Instr_AMOADD_D");
            ELSE();
                SETONE(comb.v_error);
            ENDIF();
            ENDCASE();
        CASE (CONST("0x1", 5));
            IF (EQ(comb.vb_opcode2, CONST("2", 3)));
                SETBITONE(comb.vb_dec, "Instr_AMOSWAP_W");
            ELSIF (EQ(comb.vb_opcode2, CONST("3", 3)));
                SETBITONE(comb.vb_dec, "Instr_AMOSWAP_D");
            ELSE();
                SETONE(comb.v_error);
            ENDIF();
            ENDCASE();
        CASE (CONST("0x2", 5));
            IF (AND2(EQ(comb.vb_opcode2, CONST("2", 3)), NZ(BITS(comb.vb_instr, 24, 20))));
                SETBITONE(comb.vb_dec, "Instr_LR_W");
            ELSIF (AND2(EQ(comb.vb_opcode2, CONST("3", 3)), NZ(BITS(comb.vb_instr ,24, 20))));
                SETBITONE(comb.vb_dec, "Instr_LR_D");
            ELSE();
                SETONE(comb.v_error);
            ENDIF();
            ENDCASE();
        CASE (CONST("0x3", 5));
            IF (EQ(comb.vb_opcode2, CONST("2", 3)));
                SETBITONE(comb.vb_dec, "Instr_SC_W");
            ELSIF (EQ(comb.vb_opcode2, CONST("3", 3)));
                SETBITONE(comb.vb_dec, "Instr_SC_D");
            ELSE();
                SETONE(comb.v_error);
            ENDIF();
            ENDCASE();
        CASE (CONST("0x4", 5));
            IF (EQ(comb.vb_opcode2, CONST("2", 3)));
                SETBITONE(comb.vb_dec, "Instr_AMOXOR_W");
            ELSIF (EQ(comb.vb_opcode2, CONST("3", 3)));
                SETBITONE(comb.vb_dec, "Instr_AMOXOR_D");
            ELSE();
                SETONE(comb.v_error);
            ENDIF();
            ENDCASE();
        CASE (CONST("0x8", 5));
            IF (EQ(comb.vb_opcode2, CONST("2", 3)));
                SETBITONE(comb.vb_dec, "Instr_AMOOR_W");
            ELSIF (EQ(comb.vb_opcode2, CONST("3", 3)));
                SETBITONE(comb.vb_dec, "Instr_AMOOR_D");
            ELSE();
                SETONE(comb.v_error);
            ENDIF();
            ENDCASE();
        CASE (CONST("0xC", 5));
            IF (EQ(comb.vb_opcode2, CONST("2", 3)));
                SETBITONE(comb.vb_dec, "Instr_AMOAND_W");
            ELSIF (EQ(comb.vb_opcode2, CONST("3", 3)));
                SETBITONE(comb.vb_dec, "Instr_AMOAND_D");
            ELSE();
                SETONE(comb.v_error);
            ENDIF();
            ENDCASE();
        CASE (CONST("0x10", 5));
            IF (EQ(comb.vb_opcode2, CONST("2", 3)));
                SETBITONE(comb.vb_dec, "Instr_AMOMIN_W");
            ELSIF (EQ(comb.vb_opcode2, CONST("3", 3)));
                SETBITONE(comb.vb_dec, "Instr_AMOMIN_D");
            ELSE();
                SETONE(comb.v_error);
            ENDIF();
            ENDCASE();
        CASE (CONST("0x14", 5));
            IF (EQ(comb.vb_opcode2, CONST("2", 3)));
                SETBITONE(comb.vb_dec, "Instr_AMOMAX_W");
            ELSIF (EQ(comb.vb_opcode2, CONST("3", 3)));
                SETBITONE(comb.vb_dec, "Instr_AMOMAX_D");
            ELSE();
                SETONE(comb.v_error);
            ENDIF();
            ENDCASE();
        CASE (CONST("0x18", 5));
            IF (EQ(comb.vb_opcode2, CONST("2", 3)));
                SETBITONE(comb.vb_dec, "Instr_AMOMINU_W");
            ELSIF (EQ(comb.vb_opcode2, CONST("3", 3)));
                SETBITONE(comb.vb_dec, "Instr_AMOMINU_D");
            ELSE();
                SETONE(comb.v_error);
            ENDIF();
            ENDCASE();
        CASE (CONST("0x1C", 5));
            IF (EQ(comb.vb_opcode2, CONST("2", 3)));
                SETBITONE(comb.vb_dec, "Instr_AMOMAXU_W");
            ELSIF (EQ(comb.vb_opcode2, CONST("3", 3)));
                SETBITONE(comb.vb_dec, "Instr_AMOMAXU_D");
            ELSE();
                SETONE(comb.v_error);
            ENDIF();
            ENDCASE();
        CASEDEF();
            SETONE(comb.v_error);
            ENDCASE();
        ENDSWITCH();
        ENDCASE();
    CASE (OPCODE_ADD);
        SETBITONE(comb.vb_isa_type, cfg->ISA_R_type);
        SETVAL(comb.vb_radr1, CC2(CONST("0", 1), BITS(comb.vb_instr, 19, 15)));
        SETVAL(comb.vb_radr2, CC2(CONST("0", 1), BITS(comb.vb_instr, 24, 20)));
        SETVAL(comb.vb_waddr, BITS(comb.vb_instr, 11, 7), "rdc");
        SWITCH (comb.vb_opcode2);
        CASE (CONST("0", 3));
            IF (EQ(BITS(comb.vb_instr, 31, 25), CONST("0x00", 7)));
                SETBITONE(comb.vb_dec, "Instr_ADD");
            ELSIF (EQ(BITS(comb.vb_instr, 31, 25), CONST("0x01", 7)));
                SETBITONE(comb.vb_dec, "Instr_MUL");
            ELSIF (EQ(BITS(comb.vb_instr, 31, 25), CONST("0x20", 7)));
                SETBITONE(comb.vb_dec, "Instr_SUB");
            ELSE();
                SETONE(comb.v_error);
            ENDIF();
            ENDCASE();
        CASE (CONST("0x1", 3));
            IF (EQ(BITS(comb.vb_instr, 31, 25), CONST("0x00", 7)));
                SETBITONE(comb.vb_dec, "Instr_SLL");
            ELSIF (EQ(BITS(comb.vb_instr, 31, 25), CONST("0x01", 7)));
                SETBITONE(comb.vb_dec, "Instr_MULH");
            ELSE();
                SETONE(comb.v_error);
            ENDIF();
            ENDCASE();
        CASE (CONST("0x2", 3));
            IF (EQ(BITS(comb.vb_instr, 31, 25), CONST("0x00", 7)));
                SETBITONE(comb.vb_dec, "Instr_SLT");
            ELSIF (EQ(BITS(comb.vb_instr, 31, 25), CONST("0x01", 7)));
                SETBITONE(comb.vb_dec, "Instr_MULHSU");
            ELSE();
                SETONE(comb.v_error);
            ENDIF();
            ENDCASE();
        CASE (CONST("0x3", 3));
            IF (EQ(BITS(comb.vb_instr, 31, 25), CONST("0x00", 7)));
                SETBITONE(comb.vb_dec, "Instr_SLTU");
            ELSIF (EQ(BITS(comb.vb_instr, 31, 25), CONST("0x01", 7)));
                SETBITONE(comb.vb_dec, "Instr_MULHU");
            ELSE();
                SETONE(comb.v_error);
            ENDIF();
            ENDCASE();
        CASE (CONST("0x4", 3));
            IF (EQ(BITS(comb.vb_instr, 31, 25), CONST("0x00", 7)));
                SETBITONE(comb.vb_dec, "Instr_XOR");
            ELSIF (EQ(BITS(comb.vb_instr, 31, 25), CONST("0x01", 7)));
                SETBITONE(comb.vb_dec, "Instr_DIV");
            ELSE();
                SETONE(comb.v_error);
            ENDIF();
            ENDCASE();
        CASE (CONST("0x5", 3));
            IF (EQ(BITS(comb.vb_instr, 31, 25), CONST("0x00", 7)));
                SETBITONE(comb.vb_dec, "Instr_SRL");
            ELSIF (EQ(BITS(comb.vb_instr, 31, 25), CONST("0x01", 7)));
                SETBITONE(comb.vb_dec, "Instr_DIVU");
            ELSIF (EQ(BITS(comb.vb_instr, 31, 25), CONST("0x20", 7)));
                SETBITONE(comb.vb_dec, "Instr_SRA");
            ELSE();
                SETONE(comb.v_error);
            ENDIF();
            ENDCASE();
        CASE (CONST("0x6", 3));
            IF (EQ(BITS(comb.vb_instr, 31, 25), CONST("0x00", 7)));
                SETBITONE(comb.vb_dec, "Instr_OR");
            ELSIF (EQ(BITS(comb.vb_instr, 31, 25), CONST("0x01", 7)));
                SETBITONE(comb.vb_dec, "Instr_REM");
            ELSE();
                SETONE(comb.v_error);
            ENDIF();
            ENDCASE();
        CASE (CONST("0x7", 3));
            IF (EQ(BITS(comb.vb_instr, 31, 25), CONST("0x00", 7)));
                SETBITONE(comb.vb_dec, "Instr_AND");
            ELSIF (EQ(BITS(comb.vb_instr, 31, 25), CONST("0x01", 7)));
                SETBITONE(comb.vb_dec, "Instr_REMU");
            ELSE();
                SETONE(comb.v_error);
            ENDIF();
            ENDCASE();
        CASEDEF();
            SETONE(comb.v_error);
            ENDCASE();
        ENDSWITCH();
        ENDCASE();
    CASE (OPCODE_ADDI);
        SETBITONE(comb.vb_isa_type, cfg->ISA_I_type);
        SETVAL(comb.vb_radr1, CC2(CONST("0", 1), BITS(comb.vb_instr, 19, 15)));
        SETVAL(comb.vb_waddr, BITS(comb.vb_instr, 11, 7), "rd");
        SETVAL(comb.vb_imm, BITS(comb.vb_instr, 31, 20));
        IF (NZ(BIT(comb.vb_instr, 31)));
            SETBITS(comb.vb_imm, DEC(cfg->RISCV_ARCH), CONST("12"), ALLONES());
        ENDIF();
        SWITCH (comb.vb_opcode2);
        CASE (CONST("0", 3));
            SETBITONE(comb.vb_dec, "Instr_ADDI");
            ENDCASE();
        CASE (CONST("0x1", 3));
            SETBITONE(comb.vb_dec, "Instr_SLLI");
            ENDCASE();
        CASE (CONST("0x2", 3));
            SETBITONE(comb.vb_dec, "Instr_SLTI");
            ENDCASE();
        CASE (CONST("0x3", 3));
            SETBITONE(comb.vb_dec, "Instr_SLTIU");
            ENDCASE();
        CASE (CONST("0x4", 3));
            SETBITONE(comb.vb_dec, "Instr_XORI");
            ENDCASE();
        CASE (CONST("0x5", 3));
            IF (EQ(BITS(comb.vb_instr, 31, 26), CONST("0x00", 6)));
                SETBITONE(comb.vb_dec, "Instr_SRLI");
            ELSIF (EQ(BITS(comb.vb_instr, 31, 26), CONST("0x10", 6)));
                SETBITONE(comb.vb_dec, "Instr_SRAI");
            ELSE();
                SETONE(comb.v_error);
            ENDIF();
            ENDCASE();
        CASE (CONST("0x6", 3));
            SETBITONE(comb.vb_dec, "Instr_ORI");
            ENDCASE();
        CASE (CONST("7", 3));
            SETBITONE(comb.vb_dec, "Instr_ANDI");
            ENDCASE();
        CASEDEF();
            SETONE(comb.v_error);
            ENDCASE();
        ENDSWITCH();
        ENDCASE();
    CASE (OPCODE_ADDIW);
        SETBITONE(comb.vb_isa_type, cfg->ISA_I_type);
        SETVAL(comb.vb_radr1, CC2(CONST("0", 1), BITS(comb.vb_instr, 19, 15)));
        SETVAL(comb.vb_waddr, BITS(comb.vb_instr, 11, 7), "rd");
        SETVAL(comb.vb_imm, BITS(comb.vb_instr, 31, 20));
        IF (NZ(BIT(comb.vb_instr, 31)));
            SETBITS(comb.vb_imm, DEC(cfg->RISCV_ARCH), CONST("12"), ALLONES());
        ENDIF();
        SWITCH (comb.vb_opcode2);
        CASE (CONST("0", 3));
            SETBITONE(comb.vb_dec, "Instr_ADDIW");
            ENDCASE();
        CASE (CONST("0x1", 3));
            SETBITONE(comb.vb_dec, "Instr_SLLIW");
            ENDCASE();
        CASE (CONST("0x5", 3));
            IF (EQ(BITS(comb.vb_instr, 31, 25), CONST("0x00", 7)));
                SETBITONE(comb.vb_dec, "Instr_SRLIW");
            ELSIF (EQ(BITS(comb.vb_instr, 31, 25), CONST("0x20", 7)));
                SETBITONE(comb.vb_dec, "Instr_SRAIW");
            ELSE();
                SETONE(comb.v_error);
            ENDIF();
            ENDCASE();
        CASEDEF();
            SETONE(comb.v_error);
            ENDCASE();
        ENDSWITCH();
        ENDCASE();
    CASE (OPCODE_ADDW);
        SETBIT(comb.vb_isa_type, cfg->ISA_R_type, CONST("1", 1));
        SETVAL(comb.vb_radr1, CC2(CONST("0", 1), BITS(comb.vb_instr, 19, 15)));
        SETVAL(comb.vb_radr2, CC2(CONST("0", 1), BITS(comb.vb_instr, 24, 20)));
        SETVAL(comb.vb_waddr, BITS(comb.vb_instr, 11, 7), "rd");
        SWITCH (comb.vb_opcode2);
        CASE (CONST("0", 3));
            IF (EQ(BITS(comb.vb_instr, 31, 25), CONST("0x00", 7)));
                SETBITONE(comb.vb_dec, "Instr_ADDW");
            ELSIF (EQ(BITS(comb.vb_instr, 31, 25), CONST("0x01", 7)));
                SETBITONE(comb.vb_dec, "Instr_MULW");
            ELSIF (EQ(BITS(comb.vb_instr, 31, 25), CONST("0x20", 7)));
                SETBITONE(comb.vb_dec, "Instr_SUBW");
            ELSE();
                SETONE(comb.v_error);
            ENDIF();
            ENDCASE();
        CASE (CONST("0x1", 3));
            SETBITONE(comb.vb_dec, "Instr_SLLW");
            ENDCASE();
        CASE (CONST("0x4", 3));
            IF (EQ(BITS(comb.vb_instr, 31, 25), CONST("0x01", 7)));
                SETBITONE(comb.vb_dec, "Instr_DIVW");
            ELSE();
                SETONE(comb.v_error);
            ENDIF();
            ENDCASE();
        CASE (CONST("0x5", 3));
            IF (EQ(BITS(comb.vb_instr, 31, 25), CONST("0x00", 7)));
                SETBITONE(comb.vb_dec, "Instr_SRLW");
            ELSIF (EQ(BITS(comb.vb_instr, 31, 25), CONST("0x01", 7)));
                SETBITONE(comb.vb_dec, "Instr_DIVUW");
            ELSIF (EQ(BITS(comb.vb_instr, 31, 25), CONST("0x20", 7)));
                SETBITONE(comb.vb_dec, "Instr_SRAW");
            ELSE();
                SETONE(comb.v_error);
            ENDIF();
            ENDCASE();
        CASE (CONST("0x6", 3));
            IF (EQ(BITS(comb.vb_instr, 31, 25), CONST("0x01", 7)));
                SETBITONE(comb.vb_dec, "Instr_REMW");
            ELSE();
                SETONE(comb.v_error);
            ENDIF();
            ENDCASE();
        CASE (CONST("0x7", 3));
            IF (EQ(BITS(comb.vb_instr, 31, 25), CONST("0x01", 7)));
                SETBITONE(comb.vb_dec, "Instr_REMUW");
            ELSE();
                SETONE(comb.v_error);
            ENDIF();
            ENDCASE();
        CASEDEF();
            SETONE(comb.v_error);
            ENDCASE();
        ENDSWITCH();
        ENDCASE();
    CASE (OPCODE_AUIPC);
        SETBITONE(comb.vb_isa_type, cfg->ISA_U_type);
        SETBITONE(comb.vb_dec, "Instr_AUIPC");
        SETVAL(comb.vb_waddr, BITS(comb.vb_instr, 11, 7), "rd");
        SETBITS(comb.vb_imm, 31, 12, BITS(comb.vb_instr, 31, 12));
        IF (NZ(BIT(comb.vb_instr, 31)));
            SETBITS(comb.vb_imm, DEC(cfg->RISCV_ARCH), CONST("32"), ALLONES());
        ENDIF();
        ENDCASE();
    CASE (OPCODE_BEQ);
        SETBITONE(comb.vb_isa_type, cfg->ISA_SB_type);
        SETVAL(comb.vb_radr1, CC2(CONST("0", 1), comb.vb_instr, 19, 15));
        SETVAL(comb.vb_radr2 = wb_instr(24, 20);
        SETBITS(comb.vb_imm, 11, 1, CC3(wb_instr[7], wb_instr(30, 25), wb_instr(11, 8)));
        IF (NZ(BIT(comb.vb_instr, 31)));
            vb_imm(RISCV_ARCH-1, 12) = ~0ull;
        ENDIF();
        SWITCH (comb.vb_opcode2);
        CASE (CONST("0", 3));
            wb_dec[Instr_BEQ] = 1;
            ENDCASE();
        CASE (CONST("1", 3));
            wb_dec[Instr_BNE] = 1;
            ENDCASE();
        CASE (CONST("4", 3));
            wb_dec[Instr_BLT] = 1;
            ENDCASE();
        CASE (CONST("5", 3));
            wb_dec[Instr_BGE] = 1;
            ENDCASE();
        CASE (CONST("6", 3));
            wb_dec[Instr_BLTU] = 1;
            ENDCASE();
        CASE (CONST("7", 3));
            wb_dec[Instr_BGEU] = 1;
            ENDCASE();
        CASEDEF();
            SETONE(comb.v_error);
            ENDCASE();
        ENDSWITCH();
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    SYNC_RESET(*this);
}
