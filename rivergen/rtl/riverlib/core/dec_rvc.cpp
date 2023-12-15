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

#include "dec_rvc.h"

DecoderRvc::DecoderRvc(GenObject *parent, const char *name, const char *depth) :
    ModuleObject(parent, "DecoderRvc", name, depth),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_flush_pipeline(this, "i_flush_pipeline", "1", "reset pipeline and cache"),
    i_progbuf_ena(this, "i_progbuf_ena", "1", "executing from progbuf"),
    i_f_pc(this, "i_f_pc", "RISCV_ARCH", "Fetched pc"),
    i_f_instr(this, "i_f_instr", "32", "Fetched instruction value"),
    i_instr_load_fault(this, "i_instr_load_fault", "1", "fault instruction's address"),
    i_instr_page_fault_x(this, "i_instr_page_fault_x", "1", "IMMU page fault signal"),
    o_radr1(this, "o_radr1", "6", "register bank address 1 (rs1)"),
    o_radr2(this, "o_radr2", "6", "register bank address 2 (rs2)"),
    o_waddr(this, "o_waddr", "6", "register bank output (rd)"),
    o_csr_addr(this, "o_csr_addr", "12", "CSR bank output"),
    o_imm(this, "o_imm", "RISCV_ARCH", "immediate constant decoded from instruction"),
    o_pc(this, "o_pc", "RISCV_ARCH", "Current instruction pointer value"),
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
    o_instr_page_fault_x(this, "o_instr_page_fault_x", "1", "IMMU page fault signal"),
    o_progbuf_ena(this, "o_progbuf_ena", "1", "Debug execution from progbuf"),
    // param
    OPCODE_C_ADDI4SPN(this, "OPCODE_C_ADDI4SPN", "5", "0x00", NO_COMMENT),
    OPCODE_C_NOP_ADDI(this, "OPCODE_C_NOP_ADDI", "5", "0x01", NO_COMMENT),
    OPCODE_C_SLLI(this, "OPCODE_C_SLLI", "5", "0x02", NO_COMMENT),
    OPCODE_C_JAL_ADDIW(this, "OPCODE_C_JAL_ADDIW", "5", "0x05", NO_COMMENT),
    OPCODE_C_LW(this, "OPCODE_C_LW", "5", "0x08", NO_COMMENT),
    OPCODE_C_LI(this, "OPCODE_C_LI", "5", "0x09", NO_COMMENT),
    OPCODE_C_LWSP(this, "OPCODE_C_LWSP", "5", "0x0A", NO_COMMENT),
    OPCODE_C_LD(this, "OPCODE_C_LD", "5", "0x0C", NO_COMMENT),
    OPCODE_C_ADDI16SP_LUI(this, "OPCODE_C_ADDI16SP_LUI", "5", "0xD", NO_COMMENT),
    OPCODE_C_LDSP(this, "OPCODE_C_LDSP", "5", "0x0E", NO_COMMENT),
    OPCODE_C_MATH(this, "OPCODE_C_MATH", "5", "0x11", NO_COMMENT),
    OPCODE_C_JR_MV_EBREAK_JALR_ADD(this, "OPCODE_C_JR_MV_EBREAK_JALR_ADD", "5", "0x12", NO_COMMENT),
    OPCODE_C_J(this, "OPCODE_C_J", "5", "0x15", NO_COMMENT),
    OPCODE_C_SW(this, "OPCODE_C_SW", "5", "0x18", NO_COMMENT),
    OPCODE_C_BEQZ(this, "OPCODE_C_BEQZ", "5", "0x19", NO_COMMENT),
    OPCODE_C_SWSP(this, "OPCODE_C_SWSP", "5", "0x1A", NO_COMMENT),
    OPCODE_C_SD(this, "OPCODE_C_SD", "5", "0x1C", NO_COMMENT),
    OPCODE_C_BNEZ(this, "OPCODE_C_BNEZ", "5", "0x1D", NO_COMMENT),
    OPCODE_C_SDSP(this, "OPCODE_C_SDSP", "5", "0x1E", NO_COMMENT),
    // signals
    // registers
    pc(this, "pc", "RISCV_ARCH", "'1"),
    isa_type(this, "isa_type", "ISA_Total", "'0", NO_COMMENT),
    instr_vec(this, "instr_vec", "Instr_Total"),
    instr(this, "instr", "16", "'1"),
    memop_store(this, "memop_store", "1"),
    memop_load(this, "memop_load", "1"),
    memop_sign_ext(this, "memop_sign_ext", "1"),
    memop_size(this, "memop_size", "2", "MEMOP_1B"),
    rv32(this, "rv32", "1"),
    instr_load_fault(this, "instr_load_fault", "1"),
    instr_page_fault_x(this, "instr_page_fault_x", "1"),
    instr_unimplemented(this, "instr_unimplemented", "1"),
    radr1(this, "radr1", "6", "'0", NO_COMMENT),
    radr2(this, "radr2", "6", "'0", NO_COMMENT),
    waddr(this, "waddr", "6", "'0", NO_COMMENT),
    imm(this, "imm", "RISCV_ARCH", "'0", NO_COMMENT),
    progbuf_ena(this, "progbuf_ena", "1"),
    // process
    comb(this)
{
    o_instr_vec.disableVcd();
    instr_vec.disableVcd();

    Operation::start(this);
}

void DecoderRvc::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;

    SETVAL(comb.vb_instr, i_f_instr);

TEXT();
    SETVAL(comb.vb_opcode1, CC2(BITS(comb.vb_instr, 15, 13), BITS(comb.vb_instr, 1, 0)));
    SWITCH(comb.vb_opcode1);
    CASE (OPCODE_C_ADDI4SPN);
        SETBITONE(comb.vb_isa_type, cfg->ISA_I_type);
        SETBITONE(comb.vb_dec, "Instr_ADDI");
        SETVAL(comb.vb_radr1, CONST("0x2", 6), "rs1 = sp");
        SETVAL(comb.vb_waddr, CC2(CONST("0x1", 3), BITS(comb.vb_instr, 4, 2)), "rd");
        SETBITS(comb.vb_imm, 9, 2, CC4(BITS(comb.vb_instr, 10, 7),
                                       BITS(comb.vb_instr, 12, 11),
                                       BIT(comb.vb_instr, 5),
                                       BIT(comb.vb_instr, 6)));
        ENDCASE();
    CASE (OPCODE_C_NOP_ADDI);
        SETBITONE(comb.vb_isa_type, cfg->ISA_I_type);
        SETBITONE(comb.vb_dec, "Instr_ADDI");
        SETVAL(comb.vb_radr1, BITS(comb.vb_instr, 11, 7), "rs1");
        SETVAL(comb.vb_waddr, BITS(comb.vb_instr, 11, 7), "rd");
        SETBITS(comb.vb_imm, 4, 0, BITS(comb.vb_instr, 6, 2));
        IF (NZ(BIT(comb.vb_instr, 12)));
            SETBITS(comb.vb_imm, DEC(cfg->RISCV_ARCH), CONST("5"), ALLONES());
        ENDIF();
        ENDCASE();
    CASE (OPCODE_C_SLLI);
        SETBITONE(comb.vb_isa_type, cfg->ISA_I_type);
        SETBITONE(comb.vb_dec, "Instr_SLLI");
        SETVAL(comb.vb_radr1, CC2(CONST("0", 1), BITS(comb.vb_instr, 11, 7)), "rs1");
        SETVAL(comb.vb_waddr, CC2(CONST("0", 1), BITS(comb.vb_instr, 11, 7)), "rd");
        SETBITS(comb.vb_imm, 5, 0, CC2(BIT(comb.vb_instr, 12), BITS(comb.vb_instr, 6, 2)));
        ENDCASE();
    CASE (OPCODE_C_JAL_ADDIW);
        TEXT("JAL is the RV32C only instruction");
        SETBITONE(comb.vb_isa_type, cfg->ISA_I_type);
        SETBITONE(comb.vb_dec, "Instr_ADDIW");
        SETVAL(comb.vb_radr1, CC2(CONST("0", 1), BITS(comb.vb_instr, 11, 7)), "rs1");
        SETVAL(comb.vb_waddr, CC2(CONST("0", 1), BITS(comb.vb_instr, 11, 7)), "rd");
        SETBITS(comb.vb_imm, 4, 0, BITS(comb.vb_instr, 6, 2));
        IF (NZ(BIT(comb.vb_instr, 12)));
            SETBITS(comb.vb_imm, DEC(cfg->RISCV_ARCH), CONST("5"), ALLONES());
        ENDIF();
        ENDCASE();
    CASE (OPCODE_C_LW);
        SETBITONE(comb.vb_isa_type, cfg->ISA_I_type);
        SETBITONE(comb.vb_dec, "Instr_LW");
        SETVAL(comb.vb_radr1, CC2(CONST("0x1", 3), BITS(comb.vb_instr, 9, 7)), "rs1");
        SETVAL(comb.vb_waddr, CC2(CONST("0x1", 3), BITS(comb.vb_instr, 4, 2)), "rd");
        SETBITS(comb.vb_imm, 6, 2, CC3(BIT(comb.vb_instr, 5),
                                       BITS(comb.vb_instr, 12, 10),
                                       BIT(comb.vb_instr, 6)));
        ENDCASE();
    CASE (OPCODE_C_LI, "ADDI rd = r0 + imm");
        SETBITONE(comb.vb_isa_type, cfg->ISA_I_type);
        SETBITONE(comb.vb_dec, "Instr_ADDI");
        SETVAL(comb.vb_waddr, CC2(CONST("0", 1), BITS(comb.vb_instr, 11, 7)), "rd");
        SETBITS(comb.vb_imm, 4, 0, BITS(comb.vb_instr, 6, 2));
        IF (NZ(BIT(comb.vb_instr, 12)));
            SETBITS(comb.vb_imm, DEC(cfg->RISCV_ARCH), CONST("5"), ALLONES());
        ENDIF();
        ENDCASE();
    CASE (OPCODE_C_LWSP);
        SETBITONE(comb.vb_isa_type, cfg->ISA_I_type);
        SETBITONE(comb.vb_dec, "Instr_LW");
        SETVAL(comb.vb_radr1, CONST("0x2", 6), "rs1 = sp");
        SETVAL(comb.vb_waddr, BITS(comb.vb_instr, 11, 7), "rd");
        SETBITS(comb.vb_imm, 7, 2, CC3(BITS(comb.vb_instr, 3, 2),
                                       BIT(comb.vb_instr, 12),
                                       BITS(comb.vb_instr, 6, 4)));
        ENDCASE();
    CASE (OPCODE_C_LD);
        SETBITONE(comb.vb_isa_type, cfg->ISA_I_type);
        SETBITONE(comb.vb_dec, "Instr_LD");
        SETVAL(comb.vb_radr1, CC2(CONST("0x1", 3), BITS(comb.vb_instr, 9, 7)));
        SETVAL(comb.vb_waddr, CC2(CONST("0x1", 3), BITS(comb.vb_instr, 4, 2)), "rd");
        SETBITS(comb.vb_imm, 7, 3, CC3(BIT(comb.vb_instr, 6),
                                       BIT(comb.vb_instr, 5),
                                       BITS(comb.vb_instr, 12, 10)));
        ENDCASE();
    CASE (OPCODE_C_ADDI16SP_LUI);
        IF (EQ(BITS(comb.vb_instr, 11, 7), CONST("0x2", 5)));
            SETBITONE(comb.vb_isa_type, cfg->ISA_I_type);
            SETBITONE(comb.vb_dec, "Instr_ADDI");
            SETVAL(comb.vb_radr1, CONST("0x2", 6), "rs1 = sp");
            SETVAL(comb.vb_waddr, CONST("0x2", 6), "rd = sp");
            SETBITS(comb.vb_imm, 8, 4, CC4(BITS(comb.vb_instr, 4, 3),
                                           BIT(comb.vb_instr, 5),
                                           BIT(comb.vb_instr, 2),
                                           BIT(comb.vb_instr, 6)));
            IF (NZ(BIT(comb.vb_instr, 12)));
                SETBITS(comb.vb_imm, DEC(cfg->RISCV_ARCH), CONST("9"), ALLONES());
            ENDIF();
        ELSE();
            SETBITONE(comb.vb_isa_type, cfg->ISA_U_type);
            SETBITONE(comb.vb_dec, "Instr_LUI");
            SETVAL(comb.vb_waddr, CC2(CONST("0", 1), BITS(comb.vb_instr, 11, 7)), "rd");
            SETBITS(comb.vb_imm, 16, 12, BITS(comb.vb_instr, 6, 2));
            IF (NZ(BIT(comb.vb_instr, 12)));
                SETBITS(comb.vb_imm, DEC(cfg->RISCV_ARCH), CONST("17"), ALLONES());
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (OPCODE_C_LDSP);
        SETBITONE(comb.vb_isa_type, cfg->ISA_I_type);
        SETBITONE(comb.vb_dec, "Instr_LD");
        SETVAL(comb.vb_radr1, CONST("0x2", 6), "rs1 = sp");
        SETVAL(comb.vb_waddr, BITS(comb.vb_instr, 11, 7), "rd");
        SETBITS(comb.vb_imm, 8, 3, CC3(BITS(comb.vb_instr, 4, 2),
                                       BIT(comb.vb_instr, 12),
                                       BITS(comb.vb_instr, 6, 5)));
        ENDCASE();
    CASE (OPCODE_C_MATH);
        IF (EQ(BITS(comb.vb_instr, 11, 10), CONST("0", 2)));
            SETBITONE(comb.vb_isa_type, cfg->ISA_I_type);
            SETBITONE(comb.vb_dec, "Instr_SRLI");
            SETVAL(comb.vb_radr1, CC2(CONST("0x1", 3), BITS(comb.vb_instr, 9, 7)), "rs1");
            SETVAL(comb.vb_waddr, CC2(CONST("0x1", 3), BITS(comb.vb_instr, 9, 7)), "rd");
            SETBITS(comb.vb_imm, 5, 0, CC2(BIT(comb.vb_instr, 12), BITS(comb.vb_instr, 6, 2)));
        ELSIF (EQ(BITS(comb.vb_instr, 11, 10), CONST("1", 2)));
            SETBITONE(comb.vb_isa_type, cfg->ISA_I_type);
            SETBITONE(comb.vb_dec, "Instr_SRAI");
            SETVAL(comb.vb_radr1, CC2(CONST("0x1", 3), BITS(comb.vb_instr, 9, 7)), "rs1");
            SETVAL(comb.vb_waddr, CC2(CONST("0x1", 3), BITS(comb.vb_instr, 9, 7)), "rd");
            SETBITS(comb.vb_imm, 5, 0, CC2(BIT(comb.vb_instr, 12), BITS(comb.vb_instr, 6, 2)));
        ELSIF (EQ(BITS(comb.vb_instr, 11, 10), CONST("2", 2)));
            SETBITONE(comb.vb_isa_type, cfg->ISA_I_type);
            SETBITONE(comb.vb_dec, "Instr_ANDI");
            SETVAL(comb.vb_radr1, CC2(CONST("0x1", 3), BITS(comb.vb_instr, 9, 7)), "rs1");
            SETVAL(comb.vb_waddr, CC2(CONST("0x1", 3), BITS(comb.vb_instr, 9, 7)), "rd");
            SETBITS(comb.vb_imm, 4, 0, BITS(comb.vb_instr, 6, 2));
            IF (NZ(BIT(comb.vb_instr, 12)));
                SETBITS(comb.vb_imm, DEC(cfg->RISCV_ARCH), CONST("5"), ALLONES());
            ENDIF();
        ELSIF (EZ(BIT(comb.vb_instr, 12)));
            SETBITONE(comb.vb_isa_type, cfg->ISA_R_type);
            SETVAL(comb.vb_radr1, CC2(CONST("0x1", 3), BITS(comb.vb_instr, 9, 7)), "rs1");
            SETVAL(comb.vb_radr2, CC2(CONST("0x1", 3), BITS(comb.vb_instr, 4, 2)), "rs2");
            SETVAL(comb.vb_waddr, CC2(CONST("0x1", 3), BITS(comb.vb_instr, 9, 7)), "rd");
            SWITCH (BITS(comb.vb_instr, 6, 5));
            CASE (CONST("0", 2));
                SETBITONE(comb.vb_dec, "Instr_SUB");
                ENDCASE();
            CASE (CONST("1", 2));
                SETBITONE(comb.vb_dec, "Instr_XOR");
                ENDCASE();
            CASE (CONST("2", 2));
                SETBITONE(comb.vb_dec, "Instr_OR");
                ENDCASE();
            CASEDEF();
                SETBITONE(comb.vb_dec, "Instr_AND");
                ENDCASE();
            ENDSWITCH();
        ELSE();
            SETBITONE(comb.vb_isa_type, cfg->ISA_R_type);
            SETVAL(comb.vb_radr1, CC2(CONST("0x1", 3), BITS(comb.vb_instr, 9, 7)), "rs1");
            SETVAL(comb.vb_radr2, CC2(CONST("0x1", 3), BITS(comb.vb_instr, 4, 2)), "rs2");
            SETVAL(comb.vb_waddr, CC2(CONST("0x1", 3), BITS(comb.vb_instr, 9, 7)), "rd");
            SWITCH (BITS(comb.vb_instr, 6, 5));
            CASE (CONST("0", 2));
                SETBITONE(comb.vb_dec, "Instr_SUBW");
                ENDCASE();
            CASE (CONST("1", 2));
                SETBITONE(comb.vb_dec, "Instr_ADDW");
                ENDCASE();
            CASEDEF();
                SETONE(comb.v_error);
                ENDCASE();
            ENDSWITCH();
        ENDIF();
        ENDCASE();
    CASE (OPCODE_C_JR_MV_EBREAK_JALR_ADD);
        SETBITONE(comb.vb_isa_type, cfg->ISA_I_type);
        IF (EZ(BIT(comb.vb_instr, 12)));
            IF (EZ(BITS(comb.vb_instr, 6, 2)));
                SETBITONE(comb.vb_dec, "Instr_JALR");
                SETVAL(comb.vb_radr1, CC2(CONST("0", 1), BITS(comb.vb_instr, 11, 7)), "rs1");
            ELSE();
                SETBITONE(comb.vb_dec, "Instr_ADDI");
                SETVAL(comb.vb_radr1, CC2(CONST("0", 1), BITS(comb.vb_instr, 6, 2)), "rs1");
                SETVAL(comb.vb_waddr, CC2(CONST("0", 1), BITS(comb.vb_instr, 11, 7)), "rd");
            ENDIF();
        ELSE();
            IF (AND2(EZ(BITS(comb.vb_instr, 11, 7)), EZ(BITS(comb.vb_instr, 6, 2))));
                SETBITONE(comb.vb_dec, "Instr_EBREAK");
            ELSIF (EZ(BITS(comb.vb_instr, 6, 2)));
                SETBITONE(comb.vb_dec, "Instr_JALR");
                SETVAL(comb.vb_radr1, CC2(CONST("0", 1), BITS(comb.vb_instr, 11, 7)), "rs1");
                SETVAL(comb.vb_waddr, CONST("0x1", 6));
            ELSE();
                SETBITONE(comb.vb_dec, "Instr_ADD");
                SETBITONE(comb.vb_isa_type, cfg->ISA_R_type);
                SETVAL(comb.vb_radr1, CC2(CONST("0", 1), BITS(comb.vb_instr, 11, 7)), "rs1");
                SETVAL(comb.vb_radr2, CC2(CONST("0", 1), BITS(comb.vb_instr, 6, 2)), "rs2");
                SETVAL(comb.vb_waddr, CC2(CONST("0", 1), BITS(comb.vb_instr, 11, 7)), "rd");
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (OPCODE_C_J, "JAL with rd = 0");
        SETBITONE(comb.vb_isa_type, cfg->ISA_UJ_type);
        SETBITONE(comb.vb_dec, "Instr_JAL");
        SETBITS(comb.vb_imm, 10, 1, CCx(7, &BIT(comb.vb_instr, 8),
                                           &BITS(comb.vb_instr, 10, 9),
                                           &BIT(comb.vb_instr, 6),
                                           &BIT(comb.vb_instr, 7),
                                           &BIT(comb.vb_instr, 2),
                                           &BIT(comb.vb_instr, 11),
                                           &BITS(comb.vb_instr, 5, 3)));
        IF (NZ(BIT(comb.vb_instr, 12)));
            SETBITS(comb.vb_imm, DEC(cfg->RISCV_ARCH), CONST("11"), ALLONES());
        ENDIF();
        ENDCASE();
    CASE (OPCODE_C_SW);
        SETBITONE(comb.vb_isa_type, cfg->ISA_S_type);
        SETBITONE(comb.vb_dec, "Instr_SW");
        SETVAL(comb.vb_radr1, CC2(CONST("0x1", 3), BITS(comb.vb_instr, 9, 7)), "rs1");
        SETVAL(comb.vb_radr2, CC2(CONST("0x1", 3), BITS(comb.vb_instr, 4, 2)), "rs2");
        SETBITS(comb.vb_imm, 6, 2, CC4(BIT(comb.vb_instr, 5),
                                       BIT(comb.vb_instr, 12),
                                       BITS(comb.vb_instr, 11, 10),
                                       BIT(comb.vb_instr, 6)));
        ENDCASE();
    CASE (OPCODE_C_BEQZ);
        SETBITONE(comb.vb_isa_type, cfg->ISA_SB_type);
        SETBITONE(comb.vb_dec, "Instr_BEQ");
        SETVAL(comb.vb_radr1, CC2(CONST("0x1", 3), BITS(comb.vb_instr, 9, 7)), "rs1");
        SETBITS(comb.vb_imm, 7, 1, CC4(BITS(comb.vb_instr, 6, 5),
                                       BIT(comb.vb_instr, 2),
                                       BITS(comb.vb_instr, 11, 10),
                                       BITS(comb.vb_instr, 4, 3)));
        IF (NZ(BIT(comb.vb_instr, 12)));
            SETBITS(comb.vb_imm, DEC(cfg->RISCV_ARCH), CONST("8"), ALLONES());
        ENDIF();
        ENDCASE();
    CASE (OPCODE_C_SWSP);
        SETBITONE(comb.vb_isa_type, cfg->ISA_S_type);
        SETBITONE(comb.vb_dec, "Instr_SW");
        SETVAL(comb.vb_radr1, CONST("0x2", 6), "rs1 = sp");
        SETVAL(comb.vb_radr2, CC2(CONST("0", 1), BITS(comb.vb_instr, 6, 2)), "rs2");
        SETBITS(comb.vb_imm, 7, 2, CC3(BITS(comb.vb_instr, 8, 7),
                                       BIT(comb.vb_instr, 12),
                                       BITS(comb.vb_instr, 11, 9)));
        ENDCASE();
    CASE (OPCODE_C_SD);
        SETBITONE(comb.vb_isa_type, cfg->ISA_S_type);
        SETBITONE(comb.vb_dec, "Instr_SD");
        SETVAL(comb.vb_radr1, CC2(CONST("0x1", 3), BITS(comb.vb_instr, 9, 7)), "rs1");
        SETVAL(comb.vb_radr2, CC2(CONST("0x1", 3), BITS(comb.vb_instr, 4, 2)), "rs2");
        SETBITS(comb.vb_imm, 7, 3, CC3(BITS(comb.vb_instr, 6, 5),
                                       BIT(comb.vb_instr, 12),
                                       BITS(comb.vb_instr, 11, 10)));
        ENDCASE();
    CASE (OPCODE_C_BNEZ);
        SETBITONE(comb.vb_isa_type, cfg->ISA_SB_type);
        SETBITONE(comb.vb_dec, "Instr_BNE");
        SETVAL(comb.vb_radr1, CC2(CONST("0x1", 3), BITS(comb.vb_instr, 9, 7)), "rs1");
        SETBITS(comb.vb_imm, 7, 1, CC4(BITS(comb.vb_instr, 6, 5),
                                       BIT(comb.vb_instr, 2),
                                       BITS(comb.vb_instr, 11, 10),
                                       BITS(comb.vb_instr, 4, 3)));
        IF (NZ(BIT(comb.vb_instr, 12)));
            SETBITS(comb.vb_imm, DEC(cfg->RISCV_ARCH), CONST("8"), ALLONES());
        ENDIF();
        ENDCASE();
    CASE (OPCODE_C_SDSP);
        SETBITONE(comb.vb_isa_type, cfg->ISA_S_type);
        SETBITONE(comb.vb_dec, "Instr_SD");
        SETVAL(comb.vb_radr1, CONST("0x2", 6), "rs1 = sp");
        SETVAL(comb.vb_radr2, CC2(CONST("0", 1), BITS(comb.vb_instr, 6, 2)), "rs2");
        SETBITS(comb.vb_imm, 8, 3, CC3(BITS(comb.vb_instr, 9, 7),
                                       BIT(comb.vb_instr, 12),
                                       BITS(comb.vb_instr, 11, 10)));
        ENDCASE();
    CASEDEF();
        SETONE(comb.v_error);
        ENDCASE();
    ENDSWITCH();

TEXT();
    SETVAL(comb.v_memop_store, OR2(BIT(comb.vb_dec, "Instr_SD"), BIT(comb.vb_dec, "Instr_SW")));
    SETVAL(comb.v_memop_load, OR2(BIT(comb.vb_dec, "Instr_LD"), BIT(comb.vb_dec, "Instr_LW")));
    SETVAL(comb.v_memop_sign_ext, OR2(BIT(comb.vb_dec, "Instr_LD"), BIT(comb.vb_dec, "Instr_LW")));
    IF (NZ(OR2(BIT(comb.vb_dec, "Instr_LD"), BIT(comb.vb_dec, "Instr_SD"))));
        SETVAL(comb.vb_memop_size, cfg->MEMOP_8B);
    ELSIF (NZ(OR2(BIT(comb.vb_dec, "Instr_LW"), BIT(comb.vb_dec, "Instr_SW"))));
        SETVAL(comb.vb_memop_size, cfg->MEMOP_4B);
    ELSE();
        SETVAL(comb.vb_memop_size, cfg->MEMOP_8B);
    ENDIF();
    SETVAL(comb.v_rv32, OR3(BIT(comb.vb_dec, "Instr_ADDW"),
                            BIT(comb.vb_dec, "Instr_ADDIW"),
                            BIT(comb.vb_dec, "Instr_SUBW")));
       
TEXT();
    SETVAL(pc, i_f_pc);
    SETVAL(isa_type, comb.vb_isa_type);
    SETVAL(instr_vec, comb.vb_dec);
    SETVAL(instr, BITS(i_f_instr, 15, 0));
    SETVAL(memop_store, comb.v_memop_store);
    SETVAL(memop_load, comb.v_memop_load);
    SETVAL(memop_sign_ext, comb.v_memop_sign_ext);
    SETVAL(memop_size, comb.vb_memop_size);
    SETVAL(rv32, comb.v_rv32);
    SETVAL(instr_load_fault, i_instr_load_fault);
    SETVAL(instr_page_fault_x, i_instr_page_fault_x);
    SETVAL(instr_unimplemented, comb.v_error);
    SETVAL(radr1, comb.vb_radr1);
    SETVAL(radr2, comb.vb_radr2);
    SETVAL(waddr, comb.vb_waddr);
    SETVAL(imm, comb.vb_imm);
    SETVAL(progbuf_ena, i_progbuf_ena);


TEXT();
    SYNC_RESET(*this, &NZ(i_flush_pipeline));

TEXT();
    SETVAL(o_pc, pc);
    SETVAL(o_instr, CC2(CONST("0", 16), instr));
    SETVAL(o_memop_load, memop_load);
    SETVAL(o_memop_store, memop_store);
    SETVAL(o_memop_sign_ext, memop_sign_ext);
    SETVAL(o_memop_size, memop_size);
    SETZERO(o_unsigned_op);
    SETVAL(o_rv32, rv32);
    SETZERO(o_f64);
    SETONE(o_compressed);
    SETZERO(o_amo);
    SETVAL(o_isa_type, isa_type);
    SETVAL(o_instr_vec, instr_vec);
    SETVAL(o_exception, instr_unimplemented);
    SETVAL(o_instr_load_fault, instr_load_fault);
    SETVAL(o_instr_page_fault_x, instr_page_fault_x);
    SETVAL(o_radr1, radr1);
    SETVAL(o_radr2, radr2);
    SETVAL(o_waddr, waddr);
    SETZERO(o_csr_addr);
    SETVAL(o_imm, imm);
    SETVAL(o_progbuf_ena, progbuf_ena);
}
