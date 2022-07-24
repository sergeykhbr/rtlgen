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

    SETVAL(comb.vb_instr, i_f_instr);

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
            IF (AND2(EQ(comb.vb_opcode2, CONST("2", 3)), EZ(BITS(comb.vb_instr, 24, 20))));
                SETBITONE(comb.vb_dec, "Instr_LR_W");
            ELSIF (AND2(EQ(comb.vb_opcode2, CONST("3", 3)), EZ(BITS(comb.vb_instr ,24, 20))));
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
        SETVAL(comb.vb_radr1, CC2(CONST("0", 1), BITS(comb.vb_instr, 19, 15)));
        SETVAL(comb.vb_radr2, BITS(comb.vb_instr, 24, 20));
        SETBITS(comb.vb_imm, 11, 1, CC3(BIT(comb.vb_instr, 7),
                                        BITS(comb.vb_instr, 30, 25),
                                        BITS(comb.vb_instr, 11, 8)));
        IF (NZ(BIT(comb.vb_instr, 31)));
            SETBITS(comb.vb_imm, DEC(cfg->RISCV_ARCH), CONST("12"), ALLONES());
        ENDIF();
        SWITCH (comb.vb_opcode2);
        CASE (CONST("0", 3));
            SETBITONE(comb.vb_dec, "Instr_BEQ");
            ENDCASE();
        CASE (CONST("1", 3));
            SETBITONE(comb.vb_dec, "Instr_BNE");
            ENDCASE();
        CASE (CONST("4", 3));
            SETBITONE(comb.vb_dec, "Instr_BLT");
            ENDCASE();
        CASE (CONST("5", 3));
            SETBITONE(comb.vb_dec, "Instr_BGE");
            ENDCASE();
        CASE (CONST("6", 3));
            SETBITONE(comb.vb_dec, "Instr_BLTU");
            ENDCASE();
        CASE (CONST("7", 3));
            SETBITONE(comb.vb_dec, "Instr_BGEU");
            ENDCASE();
        CASEDEF();
            SETONE(comb.v_error);
            ENDCASE();
        ENDSWITCH();
        ENDCASE();
    CASE (OPCODE_JAL);
        SETBITONE(comb.vb_isa_type, cfg->ISA_UJ_type);
        SETBITONE(comb.vb_dec, "Instr_JAL");
        SETVAL(comb.vb_waddr, CC2(CONST("0", 1), BITS(comb.vb_instr, 11, 7)), "rd");
        SETBITS(comb.vb_imm, 19, 1, CC3(BITS(comb.vb_instr, 19, 12),
                                        BIT(comb.vb_instr, 20),
                                        BITS(comb.vb_instr, 30, 21)));
        IF (NZ(BIT(comb.vb_instr, 31)));
            SETBITS(comb.vb_imm, DEC(cfg->RISCV_ARCH), CONST("20"), ALLONES());
        ENDIF();
        ENDCASE();
    CASE (OPCODE_JALR);
        SETBITONE(comb.vb_isa_type, cfg->ISA_I_type);
        SETVAL(comb.vb_radr1, CC2(CONST("0", 1), BITS(comb.vb_instr, 19, 15)));
        SETVAL(comb.vb_waddr, BITS(comb.vb_instr, 11, 7), "rd");
        SETBITS(comb.vb_imm, 11, 0, BITS(comb.vb_instr, 31, 20));
        IF (NZ(BIT(comb.vb_instr, 31)));
            SETBITS(comb.vb_imm, DEC(cfg->RISCV_ARCH), CONST("12"), ALLONES());
        ENDIF();
        SWITCH (comb.vb_opcode2);
        CASE (CONST("0", 3));
            SETBITONE(comb.vb_dec, "Instr_JALR");
            ENDCASE();
        CASEDEF();
            SETONE(comb.v_error);
            ENDCASE();
        ENDSWITCH();
        ENDCASE();
    CASE (OPCODE_LB);
        SETBITONE(comb.vb_isa_type, cfg->ISA_I_type);
        SETVAL(comb.vb_radr1, CC2(CONST("0", 1), BITS(comb.vb_instr, 19, 15)));
        SETVAL(comb.vb_waddr, BITS(comb.vb_instr, 11, 7), "rd");
        SETBITS(comb.vb_imm, 11, 0, BITS(comb.vb_instr, 31, 20));
        IF (NZ(BIT(comb.vb_instr, 31)));
            SETBITS(comb.vb_imm, DEC(cfg->RISCV_ARCH), CONST("12"), ALLONES());
        ENDIF();
        SWITCH (comb.vb_opcode2);
        CASE (CONST("0", 3));
            SETBITONE(comb.vb_dec, "Instr_LB");
            ENDCASE();
        CASE (CONST("1", 3));
            SETBITONE(comb.vb_dec, "Instr_LH");
            ENDCASE();
        CASE (CONST("2", 3));
            SETBITONE(comb.vb_dec, "Instr_LW");
            ENDCASE();
        CASE (CONST("3", 3));
            SETBITONE(comb.vb_dec, "Instr_LD");
            ENDCASE();
        CASE (CONST("4", 3));
            SETBITONE(comb.vb_dec, "Instr_LBU");
            ENDCASE();
        CASE (CONST("5", 3));
            SETBITONE(comb.vb_dec, "Instr_LHU");
            ENDCASE();
        CASE (CONST("6", 3));
            SETBITONE(comb.vb_dec, "Instr_LWU");
            ENDCASE();
        CASEDEF();
            SETONE(comb.v_error);
            ENDCASE();
        ENDSWITCH();
        ENDCASE();
    CASE (OPCODE_LUI);
        SETBITONE(comb.vb_isa_type, cfg->ISA_U_type);
        SETBITONE(comb.vb_dec, "Instr_LUI");
        SETVAL(comb.vb_waddr, BITS(comb.vb_instr, 11, 7), "rd");
        SETBITS(comb.vb_imm, 31, 12, BITS(comb.vb_instr, 31, 12));
        IF (NZ(BIT(comb.vb_instr, 31)));
            SETBITS(comb.vb_imm, DEC(cfg->RISCV_ARCH), CONST("32"), ALLONES());
        ENDIF();
        ENDCASE();
    CASE (OPCODE_SB);
        SETBITONE(comb.vb_isa_type, cfg->ISA_S_type);
        SETVAL(comb.vb_radr1, CC2(CONST("0", 1), BITS(comb.vb_instr, 19, 15)));
        SETVAL(comb.vb_radr2, CC2(CONST("0", 1), BITS(comb.vb_instr, 24, 20)));
        SETBITS(comb.vb_imm, 11, 0, CC2(BITS(comb.vb_instr, 31, 25), BITS(comb.vb_instr, 11, 7)));
        IF (NZ(BIT(comb.vb_instr, 31)));
            SETBITS(comb.vb_imm, DEC(cfg->RISCV_ARCH), CONST("12"), ALLONES());
        ENDIF();
        SWITCH (comb.vb_opcode2);
        CASE (CONST("0"));
            SETBITONE(comb.vb_dec, "Instr_SB");
            ENDCASE();
        CASE (CONST("1"));
            SETBITONE(comb.vb_dec, "Instr_SH");
            ENDCASE();
        CASE (CONST("2"));
            SETBITONE(comb.vb_dec, "Instr_SW");
            ENDCASE();
        CASE (CONST("3"));
            SETBITONE(comb.vb_dec, "Instr_SD");
            ENDCASE();
        CASEDEF();
            SETONE(comb.v_error);
            ENDCASE();
        ENDSWITCH();
        ENDCASE();
    CASE (OPCODE_CSRR);
        SETBITONE(comb.vb_isa_type, cfg->ISA_I_type);
        SETVAL(comb.vb_radr1, CC2(CONST("0", 1), BITS(comb.vb_instr, 19, 15)));
        SETVAL(comb.vb_waddr, BITS(comb.vb_instr, 11, 7), "rd");
        SETVAL(comb.vb_csr_addr, BITS(comb.vb_instr, 31, 20));
        SETBITS(comb.vb_imm, 11, 0, BITS(comb.vb_instr, 31, 20));
        IF (NZ(BIT(comb.vb_instr, 31)));
            SETBITS(comb.vb_imm, DEC(cfg->RISCV_ARCH), CONST("12"), ALLONES());
        ENDIF();
        SWITCH (comb.vb_opcode2);
        CASE (CONST("0"));
            IF (EQ(comb.vb_instr, CONST("0x00000073", 32)));
                SETBITONE(comb.vb_dec, "Instr_ECALL");
            ELSIF (EQ(comb.vb_instr, CONST("0x00100073", 32)));
                SETBITONE(comb.vb_dec, "Instr_EBREAK");
            ELSIF (EQ(comb.vb_instr, CONST("0x00200073", 32)));
                SETBITONE(comb.vb_dec, "Instr_URET");
            ELSIF (EQ(comb.vb_instr, CONST("0x10200073", 32)));
                SETBITONE(comb.vb_dec, "Instr_SRET");
            ELSIF (EQ(comb.vb_instr, CONST("0x10500073", 32)));
                SETBITONE(comb.vb_dec, "Instr_WFI");
            ELSIF (EQ(comb.vb_instr, CONST("0x20200073", 32)));
                SETBITONE(comb.vb_dec, "Instr_HRET");
            ELSIF (EQ(comb.vb_instr, CONST("0x30200073", 32)));
                SETBITONE(comb.vb_dec, "Instr_MRET");
            ELSE();
                SETONE(comb.v_error);
            ENDIF();
            ENDCASE();
        CASE (CONST("1"));
            SETBITONE(comb.vb_dec, "Instr_CSRRW");
            ENDCASE();
        CASE (CONST("2"));
            SETBITONE(comb.vb_dec, "Instr_CSRRS");
            ENDCASE();
        CASE (CONST("3"));
            SETBITONE(comb.vb_dec, "Instr_CSRRC");
            ENDCASE();
        CASE (CONST("5"));
            SETBITONE(comb.vb_dec, "Instr_CSRRWI");
            ENDCASE();
        CASE (CONST("6"));
            SETBITONE(comb.vb_dec, "Instr_CSRRSI");
            ENDCASE();
        CASE (CONST("7"));
            SETBITONE(comb.vb_dec, "Instr_CSRRCI");
            ENDCASE();
        CASEDEF();
            SETONE(comb.v_error);
            ENDCASE();
        ENDSWITCH();
        ENDCASE();
    CASE (OPCODE_FENCE);
        SWITCH (comb.vb_opcode2);
        CASE (CONST("0"));
            SETBITONE(comb.vb_dec, "Instr_FENCE");
            ENDCASE();
        CASE (CONST("1"));
            SETBITONE(comb.vb_dec, "Instr_FENCE_I");
            ENDCASE();
        CASEDEF();
            SETONE(comb.v_error);
            ENDCASE();
        ENDSWITCH();
        ENDCASE();
    CASEDEF();
        IF (fpu_ena);
            SWITCH (comb.vb_opcode1);
            CASE (OPCODE_FPU_LD);
                SETBITONE(comb.vb_isa_type, cfg->ISA_I_type);
                SETVAL(comb.vb_radr1, CC2(CONST("0", 1), BITS(comb.vb_instr, 19, 15)));
                SETVAL(comb.vb_waddr, CC2(CONST("1", 1), BITS(comb.vb_instr, 11, 7)), "rd");
                SETBITS(comb.vb_imm, 11, 0, BITS(comb.vb_instr, 31, 20));
                IF (NZ(BIT(comb.vb_instr, 31)));
                    SETBITS(comb.vb_imm, DEC(cfg->RISCV_ARCH), CONST("12"), ALLONES());
                ENDIF();
                IF (EQ(comb.vb_opcode2, CONST("3")));
                    SETBITONE(comb.vb_dec, "Instr_FLD");
                ELSE();
                    SETONE(comb.v_error);
                ENDIF();
                ENDCASE();
            CASE (OPCODE_FPU_SD);
                SETBITONE(comb.vb_isa_type, cfg->ISA_S_type);
                SETVAL(comb.vb_radr1, CC2(CONST("0", 1), BITS(comb.vb_instr, 19, 15)));
                SETVAL(comb.vb_radr2, CC2(CONST("1", 1), BITS(comb.vb_instr, 24, 20)));
                SETBITS(comb.vb_imm, 11, 0, CC2(BITS(comb.vb_instr, 31, 25), BITS(comb.vb_instr, 11, 7)));
                IF (NZ(BIT(comb.vb_instr, 31)));
                    SETBITS(comb.vb_imm, DEC(cfg->RISCV_ARCH), CONST("12"), ALLONES());
                ENDIF();
                IF (EQ(comb.vb_opcode2, CONST("3", 3)));
                    SETBITONE(comb.vb_dec, "Instr_FSD");
                ELSE();
                    SETONE(comb.v_error);
                ENDIF();
                ENDCASE();
            CASE (OPCODE_FPU_OP);
                SETBITONE(comb.vb_isa_type, cfg->ISA_R_type);
                SETVAL(comb.vb_radr1, CC2(CONST("1", 1), BITS(comb.vb_instr, 19, 15)));
                SETVAL(comb.vb_radr2, CC2(CONST("1", 1), BITS(comb.vb_instr, 24, 20)));
                SETVAL(comb.vb_waddr, CC2(CONST("1", 1), BITS(comb.vb_instr, 11, 7)), "rd");
                SWITCH (BITS(comb.vb_instr, 31, 25));
                CASE (CONST("0x1", 7));
                    SETBITONE(comb.vb_dec, "Instr_FADD_D");
                    ENDCASE();
                CASE (CONST("0x5", 7));
                    SETBITONE(comb.vb_dec, "Instr_FSUB_D");
                    ENDCASE();
                CASE (CONST("0x9", 7));
                    SETBITONE(comb.vb_dec, "Instr_FMUL_D");
                    ENDCASE();
                CASE (CONST("0xD", 7));
                    SETBITONE(comb.vb_dec, "Instr_FDIV_D");
                    ENDCASE();
                CASE (CONST("0x15", 7));
                    IF (EQ(comb.vb_opcode2, CONST("0", 3)));
                        SETBITONE(comb.vb_dec, "Instr_FMIN_D");
                    ELSIF (EQ(comb.vb_opcode2, CONST("1", 3)));
                        SETBITONE(comb.vb_dec, "Instr_FMAX_D");
                    ELSE();
                        SETONE(comb.v_error);
                    ENDIF();
                    ENDCASE();
                CASE (CONST("0x51", 7));
                    SETBITZERO(comb.vb_waddr, 5);
                    IF (EQ(comb.vb_opcode2, CONST("0", 3)));
                        SETBITONE(comb.vb_dec, "Instr_FLE_D");
                    ELSIF (EQ(comb.vb_opcode2, CONST("1", 3)));
                        SETBITONE(comb.vb_dec, "Instr_FLT_D");
                    ELSIF (EQ(comb.vb_opcode2, CONST("2", 3)));
                        SETBITONE(comb.vb_dec, "Instr_FEQ_D");
                    ELSE();
                        SETONE(comb.v_error);
                    ENDIF();
                    ENDCASE();
                CASE (CONST("0x61", 7));
                    SETBITZERO(comb.vb_waddr, 5);
                    IF (EQ(BITS(comb.vb_instr, 24, 20), CONST("0", 5)));
                        SETBITONE(comb.vb_dec, "Instr_FCVT_W_D");
                    ELSIF (EQ(BITS(comb.vb_instr, 24, 20), CONST("1", 5)));
                        SETBITONE(comb.vb_dec, "Instr_FCVT_WU_D");
                    ELSIF (EQ(BITS(comb.vb_instr, 24, 20), CONST("2", 5)));
                        SETBITONE(comb.vb_dec, "Instr_FCVT_L_D");
                    ELSIF (EQ(BITS(comb.vb_instr, 24, 20), CONST("3", 5)));
                        SETBITONE(comb.vb_dec, "Instr_FCVT_LU_D");
                    ELSE();
                        SETONE(comb.v_error);
                    ENDIF();
                    ENDCASE();
                CASE (CONST("0x69", 7));
                    SETBITZERO(comb.vb_radr1, 5);
                    IF (EQ(BITS(comb.vb_instr, 24, 20), CONST("0", 5)));
                        SETBITONE(comb.vb_dec, "Instr_FCVT_D_W");
                    ELSIF (EQ(BITS(comb.vb_instr, 24, 20), CONST("1", 5)));
                        SETBITONE(comb.vb_dec, "Instr_FCVT_D_WU");
                    ELSIF (EQ(BITS(comb.vb_instr, 24, 20), CONST("2", 5)));
                        SETBITONE(comb.vb_dec, "Instr_FCVT_D_L");
                    ELSIF (EQ(BITS(comb.vb_instr, 24, 20), CONST("3", 5)));
                        SETBITONE(comb.vb_dec, "Instr_FCVT_D_LU");
                    ELSE();
                        SETONE(comb.v_error);
                    ENDIF();
                    ENDCASE();
                CASE (CONST("0x71", 7));
                    SETBITZERO(comb.vb_waddr, 5);
                    IF (AND2(EZ(BITS(comb.vb_instr,24, 20)), EZ(comb.vb_opcode2)));
                        SETBITONE(comb.vb_dec, "Instr_FMOV_X_D");
                    ELSE();
                        SETONE(comb.v_error);
                    ENDIF();
                    ENDCASE();
                CASE (CONST("0x79", 7));
                    SETBITZERO(comb.vb_radr1, 5);
                    IF (AND2(EZ(BITS(comb.vb_instr, 24, 20)), EZ(comb.vb_opcode2)));
                        SETBITONE(comb.vb_dec, "Instr_FMOV_D_X");
                    ELSE();
                        SETONE(comb.v_error);
                    ENDIF();
                    ENDCASE();
                CASEDEF();
                    SETONE(comb.v_error);
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASEDEF();
                SETONE(comb.v_error);
                ENDCASE();
            ENDSWITCH();
        ELSE();
            TEXT("FPU disabled");
            SETONE(comb.v_error);
        ENDIF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    SETVAL(comb.v_amo, ORx(18, &BIT(comb.vb_dec, "Instr_AMOADD_W"),
                               &BIT(comb.vb_dec, "Instr_AMOXOR_W"),
                               &BIT(comb.vb_dec, "Instr_AMOOR_W"),
                               &BIT(comb.vb_dec, "Instr_AMOAND_W"),
                               &BIT(comb.vb_dec, "Instr_AMOMIN_W"),
                               &BIT(comb.vb_dec, "Instr_AMOMAX_W"),
                               &BIT(comb.vb_dec, "Instr_AMOMINU_W"),
                               &BIT(comb.vb_dec, "Instr_AMOMAXU_W"),
                               &BIT(comb.vb_dec, "Instr_AMOSWAP_W"),
                               &BIT(comb.vb_dec, "Instr_AMOADD_D"),
                               &BIT(comb.vb_dec, "Instr_AMOXOR_D"),
                               &BIT(comb.vb_dec, "Instr_AMOOR_D"),
                               &BIT(comb.vb_dec, "Instr_AMOAND_D"),
                               &BIT(comb.vb_dec, "Instr_AMOMIN_D"),
                               &BIT(comb.vb_dec, "Instr_AMOMAX_D"),
                               &BIT(comb.vb_dec, "Instr_AMOMINU_D"),
                               &BIT(comb.vb_dec, "Instr_AMOMAXU_D"),
                               &BIT(comb.vb_dec, "Instr_AMOSWAP_D")));

TEXT();
    SETVAL(comb.v_memop_store, ORx(7, &BIT(comb.vb_dec, "Instr_SD"),
                                      &BIT(comb.vb_dec, "Instr_SW"),
                                      &BIT(comb.vb_dec, "Instr_SH"),
                                      &BIT(comb.vb_dec, "Instr_SB"),
                                      &BIT(comb.vb_dec, "Instr_FSD"),
                                      &BIT(comb.vb_dec, "Instr_SC_W"),
                                      &BIT(comb.vb_dec, "Instr_SC_D")));

TEXT();
    SETVAL(comb.v_memop_load, ORx(28, &BIT(comb.vb_dec, "Instr_LD"),
                                      &BIT(comb.vb_dec, "Instr_LW"),
                                      &BIT(comb.vb_dec, "Instr_LH"),
                                      &BIT(comb.vb_dec, "Instr_LB"),
                                      &BIT(comb.vb_dec, "Instr_LWU"),
                                      &BIT(comb.vb_dec, "Instr_LHU"),
                                      &BIT(comb.vb_dec, "Instr_LBU"),
                                      &BIT(comb.vb_dec, "Instr_FLD"),
                                      &BIT(comb.vb_dec, "Instr_AMOADD_W"),
                                      &BIT(comb.vb_dec, "Instr_AMOXOR_W"),
                                      &BIT(comb.vb_dec, "Instr_AMOOR_W"),
                                      &BIT(comb.vb_dec, "Instr_AMOAND_W"),
                                      &BIT(comb.vb_dec, "Instr_AMOMIN_W"),
                                      &BIT(comb.vb_dec, "Instr_AMOMAX_W"),
                                      &BIT(comb.vb_dec, "Instr_AMOMINU_W"),
                                      &BIT(comb.vb_dec, "Instr_AMOMAXU_W"),
                                      &BIT(comb.vb_dec, "Instr_AMOSWAP_W"),
                                      &BIT(comb.vb_dec, "Instr_LR_W"),
                                      &BIT(comb.vb_dec, "Instr_AMOADD_D"),
                                      &BIT(comb.vb_dec, "Instr_AMOXOR_D"),
                                      &BIT(comb.vb_dec, "Instr_AMOOR_D"),
                                      &BIT(comb.vb_dec, "Instr_AMOAND_D"),
                                      &BIT(comb.vb_dec, "Instr_AMOMIN_D"),
                                      &BIT(comb.vb_dec, "Instr_AMOMAX_D"),
                                      &BIT(comb.vb_dec, "Instr_AMOMINU_D"),
                                      &BIT(comb.vb_dec, "Instr_AMOMAXU_D"),
                                      &BIT(comb.vb_dec, "Instr_AMOSWAP_D"),
                                      &BIT(comb.vb_dec, "Instr_LR_D")));

TEXT();
    SETVAL(comb.v_memop_sign_ext, ORx(14, &BIT(comb.vb_dec, "Instr_LD"),
                                          &BIT(comb.vb_dec, "Instr_LW"),
                                          &BIT(comb.vb_dec, "Instr_LH"),
                                          &BIT(comb.vb_dec, "Instr_LB"),
                                          &BIT(comb.vb_dec, "Instr_AMOADD_W"),
                                          &BIT(comb.vb_dec, "Instr_AMOXOR_W"),
                                          &BIT(comb.vb_dec, "Instr_AMOOR_W"),
                                          &BIT(comb.vb_dec, "Instr_AMOAND_W"),
                                          &BIT(comb.vb_dec, "Instr_AMOMIN_W"),
                                          &BIT(comb.vb_dec, "Instr_AMOMAX_W"),
                                          &BIT(comb.vb_dec, "Instr_AMOMINU_W"),
                                          &BIT(comb.vb_dec, "Instr_AMOMAXU_W"),
                                          &BIT(comb.vb_dec, "Instr_AMOSWAP_W"),
                                          &BIT(comb.vb_dec, "Instr_LR_W")));

TEXT();
    SETVAL(comb.v_f64, ORx(21,
        &BIT(comb.vb_dec, "Instr_FADD_D"),
        &BIT(comb.vb_dec, "Instr_FSUB_D"),
        &BIT(comb.vb_dec, "Instr_FMUL_D"),
        &BIT(comb.vb_dec, "Instr_FDIV_D"),
        &BIT(comb.vb_dec, "Instr_FMIN_D"),
        &BIT(comb.vb_dec, "Instr_FMAX_D"),
        &BIT(comb.vb_dec, "Instr_FLE_D"),
        &BIT(comb.vb_dec, "Instr_FLT_D"),
        &BIT(comb.vb_dec, "Instr_FEQ_D"),
        &BIT(comb.vb_dec, "Instr_FCVT_W_D"),
        &BIT(comb.vb_dec, "Instr_FCVT_WU_D"),
        &BIT(comb.vb_dec, "Instr_FCVT_L_D"),
        &BIT(comb.vb_dec, "Instr_FCVT_LU_D"),
        &BIT(comb.vb_dec, "Instr_FMOV_X_D"),
        &BIT(comb.vb_dec, "Instr_FCVT_D_W"),
        &BIT(comb.vb_dec, "Instr_FCVT_D_WU"),
        &BIT(comb.vb_dec, "Instr_FCVT_D_L"),
        &BIT(comb.vb_dec, "Instr_FCVT_D_LU"),
        &BIT(comb.vb_dec, "Instr_FMOV_D_X"),
        &BIT(comb.vb_dec, "Instr_FLD"),
        &BIT(comb.vb_dec, "Instr_FSD")));

TEXT();
    IF (NZ(ORx(15, &BIT(comb.vb_dec, "Instr_LD"),
                   &BIT(comb.vb_dec, "Instr_SD"),
                   &BIT(comb.vb_dec, "Instr_FLD"),
                   &BIT(comb.vb_dec, "Instr_FSD"),
                   &BIT(comb.vb_dec, "Instr_AMOADD_D"),
                   &BIT(comb.vb_dec, "Instr_AMOXOR_D"),
                   &BIT(comb.vb_dec, "Instr_AMOOR_D"),
                   &BIT(comb.vb_dec, "Instr_AMOAND_D"),
                   &BIT(comb.vb_dec, "Instr_AMOMIN_D"),
                   &BIT(comb.vb_dec, "Instr_AMOMAX_D"),
                   &BIT(comb.vb_dec, "Instr_AMOMINU_D"),
                   &BIT(comb.vb_dec, "Instr_AMOMAXU_D"),
                   &BIT(comb.vb_dec, "Instr_AMOSWAP_D"),
                   &BIT(comb.vb_dec, "Instr_LR_D"),
                   &BIT(comb.vb_dec, "Instr_SC_D"))));
        SETVAL(comb.vb_memop_size, cfg->MEMOP_8B);
    ELSIF (NZ(ORx(14, &BIT(comb.vb_dec, "Instr_LW"),
                      &BIT(comb.vb_dec, "Instr_LWU"),
                      &BIT(comb.vb_dec, "Instr_SW"),
                      &BIT(comb.vb_dec, "Instr_AMOADD_W"),
                      &BIT(comb.vb_dec, "Instr_AMOXOR_W"),
                      &BIT(comb.vb_dec, "Instr_AMOOR_W"),
                      &BIT(comb.vb_dec, "Instr_AMOAND_W"),
                      &BIT(comb.vb_dec, "Instr_AMOMIN_W"),
                      &BIT(comb.vb_dec, "Instr_AMOMAX_W"),
                      &BIT(comb.vb_dec, "Instr_AMOMINU_W"),
                      &BIT(comb.vb_dec, "Instr_AMOMAXU_W"),
                      &BIT(comb.vb_dec, "Instr_AMOSWAP_W"),
                      &BIT(comb.vb_dec, "Instr_LR_W"),
                      &BIT(comb.vb_dec, "Instr_SC_W"))));
        SETVAL(comb.vb_memop_size, cfg->MEMOP_4B);
    ELSIF (NZ(OR3(BIT(comb.vb_dec, "Instr_LH"),
                  BIT(comb.vb_dec, "Instr_LHU"),
                  BIT(comb.vb_dec, "Instr_SH"))));
        SETVAL(comb.vb_memop_size, cfg->MEMOP_2B);
    ELSE();
        SETVAL(comb.vb_memop_size, cfg->MEMOP_1B);
    ENDIF();

TEXT();
    SETVAL(comb.v_unsigned_op, ORx(11, &BIT(comb.vb_dec, "Instr_DIVU"),
                                       &BIT(comb.vb_dec, "Instr_REMU"),
                                       &BIT(comb.vb_dec, "Instr_DIVUW"),
                                       &BIT(comb.vb_dec, "Instr_REMUW"),
                                       &BIT(comb.vb_dec, "Instr_MULHU"),
                                       &BIT(comb.vb_dec, "Instr_FCVT_WU_D"),
                                       &BIT(comb.vb_dec, "Instr_FCVT_LU_D"),
                                       &BIT(comb.vb_dec, "Instr_AMOMINU_W"),
                                       &BIT(comb.vb_dec, "Instr_AMOMAXU_W"),
                                       &BIT(comb.vb_dec, "Instr_AMOMINU_D"),
                                       &BIT(comb.vb_dec, "Instr_AMOMAXU_D")));

TEXT();
    SETVAL(comb.v_rv32, ORx(25, &BIT(comb.vb_dec, "Instr_ADDW"),
                                &BIT(comb.vb_dec, "Instr_ADDIW"),
                                &BIT(comb.vb_dec, "Instr_SLLW"),
                                &BIT(comb.vb_dec, "Instr_SLLIW"),
                                &BIT(comb.vb_dec, "Instr_SRAW"),
                                &BIT(comb.vb_dec, "Instr_SRAIW"),
                                &BIT(comb.vb_dec, "Instr_SRLW"),
                                &BIT(comb.vb_dec, "Instr_SRLIW"),
                                &BIT(comb.vb_dec, "Instr_SUBW"),
                                &BIT(comb.vb_dec, "Instr_DIVW"),
                                &BIT(comb.vb_dec, "Instr_DIVUW"),
                                &BIT(comb.vb_dec, "Instr_MULW"),
                                &BIT(comb.vb_dec, "Instr_REMW"),
                                &BIT(comb.vb_dec, "Instr_REMUW"),
                                &BIT(comb.vb_dec, "Instr_AMOADD_W"),
                                &BIT(comb.vb_dec, "Instr_AMOXOR_W"),
                                &BIT(comb.vb_dec, "Instr_AMOOR_W"),
                                &BIT(comb.vb_dec, "Instr_AMOAND_W"),
                                &BIT(comb.vb_dec, "Instr_AMOMIN_W"),
                                &BIT(comb.vb_dec, "Instr_AMOMAX_W"),
                                &BIT(comb.vb_dec, "Instr_AMOMINU_W"),
                                &BIT(comb.vb_dec, "Instr_AMOMAXU_W"),
                                &BIT(comb.vb_dec, "Instr_AMOSWAP_W"),
                                &BIT(comb.vb_dec, "Instr_LR_W"),
                                &BIT(comb.vb_dec, "Instr_SC_W")));

TEXT();
    SETVAL(comb.v_f64, ORx(21, &BIT(comb.vb_dec, "Instr_FADD_D"),
                               &BIT(comb.vb_dec, "Instr_FSUB_D"),
                               &BIT(comb.vb_dec, "Instr_FMUL_D"),
                               &BIT(comb.vb_dec, "Instr_FDIV_D"),
                               &BIT(comb.vb_dec, "Instr_FMIN_D"),
                               &BIT(comb.vb_dec, "Instr_FMAX_D"),
                               &BIT(comb.vb_dec, "Instr_FLE_D"),
                               &BIT(comb.vb_dec, "Instr_FLT_D"),
                               &BIT(comb.vb_dec, "Instr_FEQ_D"),
                               &BIT(comb.vb_dec, "Instr_FCVT_W_D"),
                               &BIT(comb.vb_dec, "Instr_FCVT_WU_D"),
                               &BIT(comb.vb_dec, "Instr_FCVT_L_D"),
                               &BIT(comb.vb_dec, "Instr_FCVT_LU_D"),
                               &BIT(comb.vb_dec, "Instr_FMOV_X_D"),
                               &BIT(comb.vb_dec, "Instr_FCVT_D_W"),
                               &BIT(comb.vb_dec, "Instr_FCVT_D_WU"),
                               &BIT(comb.vb_dec, "Instr_FCVT_D_L"),
                               &BIT(comb.vb_dec, "Instr_FCVT_D_LU"),
                               &BIT(comb.vb_dec, "Instr_FMOV_D_X"),
                               &BIT(comb.vb_dec, "Instr_FLD"),
                               &BIT(comb.vb_dec, "Instr_FSD")));

TEXT();
    SETVAL(pc, i_f_pc);
    SETVAL(isa_type, comb.vb_isa_type);
    SETVAL(instr_vec, comb.vb_dec);
    SETVAL(instr, i_f_instr);
    SETVAL(memop_store, comb.v_memop_store);
    SETVAL(memop_load, comb.v_memop_load);
    SETVAL(memop_sign_ext, comb.v_memop_sign_ext);
    SETVAL(memop_size, comb.vb_memop_size);
    SETVAL(unsigned_op, comb.v_unsigned_op);
    SETVAL(rv32, comb.v_rv32);
    SETVAL(f64, comb.v_f64);
    SETVAL(compressed, comb.v_compressed);
    SETVAL(amo, comb.v_amo);
    SETVAL(instr_load_fault, i_instr_load_fault);
    SETVAL(instr_executable, i_instr_executable);
    SETVAL(instr_unimplemented, comb.v_error);
    SETVAL(radr1, comb.vb_radr1);
    SETVAL(radr2, comb.vb_radr2);
    SETVAL(waddr, comb.vb_waddr);
    SETVAL(csr_addr, comb.vb_csr_addr);
    SETVAL(imm, comb.vb_imm);
    SETVAL(progbuf_ena, i_progbuf_ena);

TEXT();
    SYNC_RESET(*this, &NZ(i_flush_pipeline));

TEXT();
    SETVAL(o_pc, pc);
    SETVAL(o_instr, instr);
    SETVAL(o_memop_load, memop_load);
    SETVAL(o_memop_store, memop_store);
    SETVAL(o_memop_sign_ext, memop_sign_ext);
    SETVAL(o_memop_size, memop_size);
    SETVAL(o_unsigned_op, unsigned_op);
    SETVAL(o_rv32, rv32);
    SETVAL(o_f64, f64);
    SETVAL(o_compressed, compressed);
    SETVAL(o_amo, amo);
    SETVAL(o_isa_type, isa_type);
    SETVAL(o_instr_vec, instr_vec);
    SETVAL(o_exception, instr_unimplemented);
    SETVAL(o_instr_load_fault, instr_load_fault);
    SETVAL(o_instr_executable, instr_executable);
    SETVAL(o_radr1, radr1);
    SETVAL(o_radr2, radr2);
    SETVAL(o_waddr, waddr);
    SETVAL(o_csr_addr, csr_addr);
    SETVAL(o_imm, imm);
    SETVAL(o_progbuf_ena, progbuf_ena);
}
