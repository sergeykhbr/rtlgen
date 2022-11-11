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

#include "decoder.h"

InstrDecoder::InstrDecoder(GenObject *parent, const char *name) :
    ModuleObject(parent, "InstrDecoder", name),
    fpu_ena(this, "fpu_ena", "true"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_f_pc(this, "i_f_pc", "RISCV_ARCH", "Fetched pc"),
    i_f_instr(this, "i_f_instr", "64", "Fetched instruction value"),
    i_instr_load_fault(this, "i_instr_load_fault", "1", "fault instruction's address"),
    i_instr_page_fault_x(this, "i_instr_page_fault_x", "1", "Instruction MMU page fault"),
    i_e_npc(this, "i_e_npc", "RISCV_ARCH", "executor expected instr pointer"),
    o_radr1(this, "o_radr1", "6", "register bank address 1 (rs1)"),
    o_radr2(this, "o_radr2", "6", "register bank address 2 (rs2)"),
    o_waddr(this, "o_waddr", "6", "register bank output (rd)"),
    o_csr_addr(this, "o_csr_addr", "12", "CSR bank output"),
    o_imm(this, "o_imm", "RISCV_ARCH", "immediate constant decoded from instruction"),
    i_flush_pipeline(this, "i_flush_pipeline", "1", "reset pipeline and cache"),
    i_progbuf_ena(this, "i_progbuf_ena", "1", "executing from progbuf"),
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
    o_progbuf_ena(this, "o_progbuf_ena", "1"),
    // struct declaration
    DecoderDataTypeDef_(this, "", -1),
    // parameters
    DEC_NUM(this, "DEC_NUM", "2"),
    DEC_BLOCK(this, "DEC_BLOCK", "MUL(2,DEC_NUM)", "2 rv + 2 rvc"),
    _1_(this, "shift registers depth to store previous decoded data"),
    FULL_DEC_DEPTH(this, "FULL_DEC_DEPTH", "MUL(DEC_BLOCK,ADD(SUB(CFG_DEC_DEPTH,1),CFG_BP_DEPTH))"),
    // Signals
    wd(this, "", "wd", "ADD(FULL_DEC_DEPTH,DEC_BLOCK)"),
    d(this, "d"),
    wb_f_pc(this, "wb_f_pc", "RISCV_ARCH", "DEC_NUM"),
    wb_f_instr(this, "wb_f_instr", "32", "DEC_NUM"),
    // registers
    // process
    comb(this),
    rv(this, "rv", "DEC_NUM"),
    rvc(this, "rvc", "DEC_NUM")
{
    Operation::start(this);

    // Create and connet Sub-modules:
    {
        GenObject &i = FORGEN ("i", CONST("0"), CONST("DEC_NUM"), "++", new STRING("rvx"));
            NEW(*rv.getItem(0), rv.getName().c_str(), &i);
                CONNECT(rv, &i, rv->i_clk, i_clk);
                CONNECT(rv, &i, rv->i_nrst, i_nrst);
                CONNECT(rv, &i, rv->i_flush_pipeline, i_flush_pipeline);
                CONNECT(rv, &i, rv->i_progbuf_ena, i_progbuf_ena);
                CONNECT(rv, &i, rv->i_f_pc, ARRITEM(wb_f_pc, i, wb_f_pc));
                CONNECT(rv, &i, rv->i_f_instr, ARRITEM(wb_f_instr, i, wb_f_instr));
                CONNECT(rv, &i, rv->i_instr_load_fault, i_instr_load_fault);
                CONNECT(rv, &i, rv->i_instr_page_fault_x, i_instr_page_fault_x);
                CONNECT(rv, &i, rv->o_radr1, ARRITEM(wd, MUL2(CONST("2"), i), wd->radr1));
                CONNECT(rv, &i, rv->o_radr2, ARRITEM(wd, MUL2(CONST("2"), i), wd->radr2));
                CONNECT(rv, &i, rv->o_waddr, ARRITEM(wd, MUL2(CONST("2"), i), wd->waddr));
                CONNECT(rv, &i, rv->o_csr_addr, ARRITEM(wd, MUL2(CONST("2"), i), wd->csr_addr));
                CONNECT(rv, &i, rv->o_imm, ARRITEM(wd, MUL2(CONST("2"), i), wd->imm));
                CONNECT(rv, &i, rv->o_pc, ARRITEM(wd, MUL2(CONST("2"), i), wd->pc));
                CONNECT(rv, &i, rv->o_instr, ARRITEM(wd, MUL2(CONST("2"), i), wd->instr));
                CONNECT(rv, &i, rv->o_memop_store, ARRITEM(wd, MUL2(CONST("2"), i), wd->memop_store));
                CONNECT(rv, &i, rv->o_memop_load, ARRITEM(wd, MUL2(CONST("2"), i), wd->memop_load));
                CONNECT(rv, &i, rv->o_memop_sign_ext, ARRITEM(wd, MUL2(CONST("2"), i), wd->memop_sign_ext));
                CONNECT(rv, &i, rv->o_memop_size, ARRITEM(wd, MUL2(CONST("2"), i), wd->memop_size));
                CONNECT(rv, &i, rv->o_rv32, ARRITEM(wd, MUL2(CONST("2"), i), wd->rv32));
                CONNECT(rv, &i, rv->o_compressed, ARRITEM(wd, MUL2(CONST("2"), i), wd->compressed));
                CONNECT(rv, &i, rv->o_amo, ARRITEM(wd, MUL2(CONST("2"), i), wd->amo));
                CONNECT(rv, &i, rv->o_f64, ARRITEM(wd, MUL2(CONST("2"), i), wd->f64));
                CONNECT(rv, &i, rv->o_unsigned_op, ARRITEM(wd, MUL2(CONST("2"), i), wd->unsigned_op));
                CONNECT(rv, &i, rv->o_isa_type, ARRITEM(wd, MUL2(CONST("2"), i), wd->isa_type));
                CONNECT(rv, &i, rv->o_instr_vec, ARRITEM(wd, MUL2(CONST("2"), i), wd->instr_vec));
                CONNECT(rv, &i, rv->o_exception, ARRITEM(wd, MUL2(CONST("2"), i), wd->instr_unimplemented));
                CONNECT(rv, &i, rv->o_instr_load_fault, ARRITEM(wd, MUL2(CONST("2"), i), wd->instr_load_fault));
                CONNECT(rv, &i, rv->o_instr_page_fault_x, ARRITEM(wd, MUL2(CONST("2"), i), wd->instr_page_fault_x));
                CONNECT(rv, &i, rv->o_progbuf_ena, ARRITEM(wd, MUL2(CONST("2"), i), wd->progbuf_ena));
            ENDNEW();
        ENDFORGEN(new STRING("rvx"));
    }

    {
        GenObject &i = FORGEN ("i", CONST("0"), CONST("DEC_NUM"), "++", new STRING("rvcx"));
            NEW(*rvc.getItem(0), rvc.getName().c_str(), &i);
                CONNECT(rvc, &i, rvc->i_clk, i_clk);
                CONNECT(rvc, &i, rvc->i_nrst, i_nrst);
                CONNECT(rvc, &i, rvc->i_flush_pipeline, i_flush_pipeline);
                CONNECT(rvc, &i, rvc->i_progbuf_ena, i_progbuf_ena);
                CONNECT(rvc, &i, rvc->i_f_pc, ARRITEM(wb_f_pc, i, wb_f_pc));
                CONNECT(rvc, &i, rvc->i_f_instr, ARRITEM(wb_f_instr, i, wb_f_instr));
                CONNECT(rvc, &i, rvc->i_instr_load_fault, i_instr_load_fault);
                CONNECT(rvc, &i, rvc->i_instr_page_fault_x, i_instr_page_fault_x);
                CONNECT(rvc, &i, rvc->o_radr1, ARRITEM(wd, INC(MUL2(CONST("2"), i)), wd->radr1));
                CONNECT(rvc, &i, rvc->o_radr2, ARRITEM(wd, INC(MUL2(CONST("2"), i)), wd->radr2));
                CONNECT(rvc, &i, rvc->o_waddr, ARRITEM(wd, INC(MUL2(CONST("2"), i)), wd->waddr));
                CONNECT(rvc, &i, rvc->o_csr_addr, ARRITEM(wd, INC(MUL2(CONST("2"), i)), wd->csr_addr));
                CONNECT(rvc, &i, rvc->o_imm, ARRITEM(wd, INC(MUL2(CONST("2"), i)), wd->imm));
                CONNECT(rvc, &i, rvc->o_pc, ARRITEM(wd, INC(MUL2(CONST("2"), i)), wd->pc));
                CONNECT(rvc, &i, rvc->o_instr, ARRITEM(wd, INC(MUL2(CONST("2"), i)), wd->instr));
                CONNECT(rvc, &i, rvc->o_memop_store, ARRITEM(wd, INC(MUL2(CONST("2"), i)), wd->memop_store));
                CONNECT(rvc, &i, rvc->o_memop_load, ARRITEM(wd, INC(MUL2(CONST("2"), i)), wd->memop_load));
                CONNECT(rvc, &i, rvc->o_memop_sign_ext, ARRITEM(wd, INC(MUL2(CONST("2"), i)), wd->memop_sign_ext));
                CONNECT(rvc, &i, rvc->o_memop_size, ARRITEM(wd, INC(MUL2(CONST("2"), i)), wd->memop_size));
                CONNECT(rvc, &i, rvc->o_rv32, ARRITEM(wd, INC(MUL2(CONST("2"), i)), wd->rv32));
                CONNECT(rvc, &i, rvc->o_compressed, ARRITEM(wd, INC(MUL2(CONST("2"), i)), wd->compressed));
                CONNECT(rvc, &i, rvc->o_amo, ARRITEM(wd, INC(MUL2(CONST("2"), i)), wd->amo));
                CONNECT(rvc, &i, rvc->o_f64, ARRITEM(wd, INC(MUL2(CONST("2"), i)), wd->f64));
                CONNECT(rvc, &i, rvc->o_unsigned_op, ARRITEM(wd, INC(MUL2(CONST("2"), i)), wd->unsigned_op));
                CONNECT(rvc, &i, rvc->o_isa_type, ARRITEM(wd, INC(MUL2(CONST("2"), i)), wd->isa_type));
                CONNECT(rvc, &i, rvc->o_instr_vec, ARRITEM(wd, INC(MUL2(CONST("2"), i)), wd->instr_vec));
                CONNECT(rvc, &i, rvc->o_exception, ARRITEM(wd, INC(MUL2(CONST("2"), i)), wd->instr_unimplemented));
                CONNECT(rvc, &i, rvc->o_instr_load_fault, ARRITEM(wd, INC(MUL2(CONST("2"), i)), wd->instr_load_fault));
                CONNECT(rvc, &i, rvc->o_instr_page_fault_x, ARRITEM(wd, INC(MUL2(CONST("2"), i)), wd->instr_page_fault_x));
                CONNECT(rvc, &i, rvc->o_progbuf_ena, ARRITEM(wd, INC(MUL2(CONST("2"), i)), wd->progbuf_ena));
            ENDNEW();
        ENDFORGEN(new STRING("rvcx"));
    }
}

void InstrDecoder::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;

    GenObject *i = &FOR("i", CONST("0"), FULL_DEC_DEPTH, "++");
        SETARRITEM(wd, ADD2(DEC_BLOCK,*i), wd, ARRITEM(d, *i, d));
    ENDFOR();

TEXT();
    IF (NE(i_f_pc, ARRITEM(wd, CONST("0"), wd->pc)));
        SETONE(comb.shift_ena);
    ENDIF();

TEXT();
    TEXT("Shift decoder buffer when new instruction available");
    IF (NZ(comb.shift_ena));
        i = &FOR("i", CONST("0"), DEC_BLOCK, "++");
            SETARRITEM(d, *i, d, ARRITEM(wd, *i, wd));
        ENDFOR();
        i = &FOR("i", CONST("DEC_BLOCK"), FULL_DEC_DEPTH, "++");
            SETARRITEM(d, *i, d, ARRITEM(d, SUB2(*i, DEC_BLOCK), d));
        ENDFOR();
    ENDIF();

TEXT();
    TEXT("Select output decoder:");
    i = &FOR("i", CONST("0"), DIV2(ADD2(FULL_DEC_DEPTH, DEC_BLOCK), CONST("2")), "++");
        IF (EQ(i_e_npc, ARRITEM(wd, MUL2(CONST("2"), *i), wd->pc)));
            IF (EZ(ARRITEM(wd, MUL2(CONST("2"), *i), wd->compressed)));
                SETVAL(comb.selidx, MUL2(CONST("2"), *i));
            ELSE();
                SETVAL(comb.selidx, INC(MUL2(CONST("2"), *i)));
            ENDIF();
        ENDIF();
    ENDFOR();

TEXT();
    TEXT("generate decoders inputs with offset");
    i = &FOR("i", CONST("0"), DEC_NUM, "++");
        SETARRITEM(wb_f_pc, *i, wb_f_pc, ADD2(i_f_pc, MUL2(CONST("2"), *i)));
        SETARRITEM(wb_f_instr, *i, wb_f_instr, BITSW(i_f_instr, 
                                                     MUL2(CONST("16"), *i),
                                                     CONST("32")));
    ENDFOR();

TEXT();
    SYNC_RESET(*this, &NZ(i_flush_pipeline));

TEXT();
    SETVAL(o_pc, ARRITEM(wd, comb.selidx, wd->pc));
    SETVAL(o_instr, ARRITEM(wd, comb.selidx, wd->instr));
    SETVAL(o_memop_load, ARRITEM(wd, comb.selidx, wd->memop_load));
    SETVAL(o_memop_store, ARRITEM(wd, comb.selidx, wd->memop_store));
    SETVAL(o_memop_sign_ext, ARRITEM(wd, comb.selidx, wd->memop_sign_ext));
    SETVAL(o_memop_size, ARRITEM(wd, comb.selidx, wd->memop_size));
    SETVAL(o_unsigned_op, ARRITEM(wd, comb.selidx, wd->unsigned_op));
    SETVAL(o_rv32, ARRITEM(wd, comb.selidx, wd->rv32));
    SETVAL(o_f64, ARRITEM(wd, comb.selidx, wd->f64));
    SETVAL(o_compressed, ARRITEM(wd, comb.selidx, wd->compressed));
    SETVAL(o_amo, ARRITEM(wd, comb.selidx, wd->amo));
    SETVAL(o_isa_type, ARRITEM(wd, comb.selidx, wd->isa_type));
    SETVAL(o_instr_vec, ARRITEM(wd, comb.selidx, wd->instr_vec));
    SETVAL(o_exception, ARRITEM(wd, comb.selidx, wd->instr_unimplemented));
    SETVAL(o_instr_load_fault, ARRITEM(wd, comb.selidx, wd->instr_load_fault));
    SETVAL(o_instr_page_fault_x, ARRITEM(wd, comb.selidx, wd->instr_page_fault_x));
    SETVAL(o_radr1, ARRITEM(wd, comb.selidx, wd->radr1));
    SETVAL(o_radr2, ARRITEM(wd, comb.selidx, wd->radr2));
    SETVAL(o_waddr, ARRITEM(wd, comb.selidx, wd->waddr));
    SETVAL(o_csr_addr, ARRITEM(wd, comb.selidx, wd->csr_addr));
    SETVAL(o_imm, ARRITEM(wd, comb.selidx, wd->imm));
    SETVAL(o_progbuf_ena, ARRITEM(wd, comb.selidx, wd->progbuf_ena));
}

