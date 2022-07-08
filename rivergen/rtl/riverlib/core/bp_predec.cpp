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

#include "bp_predec.h"

BpPreDecoder::BpPreDecoder(GenObject *parent, river_cfg *cfg) :
    ModuleObject(parent, "BpPreDecoder"),
    cfg_(cfg),
    i_c_valid(this, "i_c_valid", "1", "Use compressed for prediction"),
    i_addr(this, "i_addr", "CFG_CPU_ADDR_BITS", "Memory response address"),
    i_data(this, "i_data", "32", "Memory response value"),
    i_ra(this, "i_ra", "RISCV_ARCH", "Return address register value"),
    o_jmp(this, "o_jmp", "1", "Jump detected"),
    o_pc(this, "o_pc", "CFG_CPU_ADDR_BITS", "Fetching instruction pointer"),
    o_npc(this, "o_npc", "CFG_CPU_ADDR_BITS", "Fetching instruction pointer"),
    // Signals
    vb_npc(this, "vb_npc", "CFG_CPU_ADDR_BITS"),
    v_jal(this, "v_jal", "1", "JAL instruction"),
    v_branch(this, "v_branch", "1", "One of branch instructions (only negative offset)"),
    v_c_j(this, "v_c_j", "1", "compressed J instruction"),
    v_c_ret(this, "v_c_ret", "1", "compressed RET pseudo-instruction"),
    // process
    comb(this)
{
}

void BpPreDecoder::proc_comb() {
    SETVAL(comb.vb_pc, i_addr);
    SETVAL(comb.vb_tmp, i_data);

TEXT();
    TEXT("Unconditional jump \"J\"");
    IF (NZ(BIT(comb.vb_tmp, 31)));
        SETBITS(comb.vb_jal_off, DEC(cfg_->CFG_CPU_ADDR_BITS), CONST("20"), ALLONES());
    ELSE();
        SETBITS(comb.vb_jal_off, DEC(cfg_->CFG_CPU_ADDR_BITS), CONST("20"), ALLZEROS());
    ENDIF();
    SETBITS(comb.vb_jal_off, 19, 12, BITS(comb.vb_tmp, 19, 12));
    SETBIT(comb.vb_jal_off, 11, BIT(comb.vb_tmp, 20));
    SETBITS(comb.vb_jal_off, 10, 1, BITS(comb.vb_tmp, 30, 21));
    SETBIT(comb.vb_jal_off, 0, ALLZEROS());
    SETVAL(comb.vb_jal_addr, ADD2(comb.vb_pc, comb.vb_jal_off));

TEXT();
    SETVAL(v_jal, ALLZEROS());
    IF (EQ(BITS(comb.vb_tmp, 6, 0), CONST("0x6F", 7)));
        SETVAL(v_jal, ALLONES());
    ENDIF();

TEXT();
    TEXT("Conditional branches \"BEQ\", \"BNE\", \"BLT\", \"BGE\", \"BLTU\", \"BGEU\"");
    TEXT("Only negative offset leads to predicted jumps");
    IF (BIT(comb.vb_tmp, 31));
        SETBITS(comb.vb_branch_off, DEC(cfg_->CFG_CPU_ADDR_BITS), CONST("12"), ALLONES());
    ELSE();
        SETBITS(comb.vb_branch_off, DEC(cfg_->CFG_CPU_ADDR_BITS), CONST("12"), ALLZEROS());
    ENDIF();
    SETBIT(comb.vb_branch_off, 11, BIT(comb.vb_tmp, 7));
    SETBITS(comb.vb_branch_off, 10, 5,  BITS(comb.vb_tmp, 30, 25));
    SETBITS(comb.vb_branch_off, 4, 1,  BITS(comb.vb_tmp, 11, 8));
    SETBIT(comb.vb_branch_off, 0, ALLZEROS());
    SETVAL(comb.vb_branch_addr, ADD2(comb.vb_pc, comb.vb_branch_off));

TEXT();
    SETVAL(v_branch, ALLZEROS());
    IF (NZ(AND2(EQ(BITS(comb.vb_tmp, 6, 0), CONST("0x63", 7)),
                BIT(comb.vb_tmp, 31))));
        SETVAL(v_branch, ALLONES());
    ENDIF();

TEXT();
    TEXT("Check Compressed \"C_J\" unconditional jump");
    IF (BIT(comb.vb_tmp, 12));
        SETBITS(comb.vb_c_j_off, DEC(cfg_->CFG_CPU_ADDR_BITS), CONST("11"), ALLONES());
    ELSE();
        SETBITS(comb.vb_c_j_off, DEC(cfg_->CFG_CPU_ADDR_BITS), CONST("11"), ALLZEROS());
    ENDIF();
    SETBIT(comb.vb_c_j_off, 10, BIT(comb.vb_tmp, 8));
    SETBITS(comb.vb_c_j_off, 9, 8, BITS(comb.vb_tmp, 10, 9));
    SETBIT(comb.vb_c_j_off, 7, BIT(comb.vb_tmp, 6));
    SETBIT(comb.vb_c_j_off, 6, BIT(comb.vb_tmp, 7));
    SETBIT(comb.vb_c_j_off, 5, BIT(comb.vb_tmp, 2));
    SETBIT(comb.vb_c_j_off, 4, BIT(comb.vb_tmp, 11));
    SETBITS(comb.vb_c_j_off, 3, 1, BITS(comb.vb_tmp, 5, 3));
    SETBIT(comb.vb_c_j_off, 0, ALLZEROS());
    SETVAL(comb.vb_c_j_addr, ADD2(comb.vb_pc, comb.vb_c_j_off));

TEXT();
    SETVAL(v_c_j, ALLZEROS());
    IF (AND2(EQ(BITS(comb.vb_tmp, 15, 13), CONST("0x5", 3)),
             EQ(BITS(comb.vb_tmp, 1, 0), CONST("0x1", 2))));
        SETVAL(v_c_j, i_c_valid);
    ENDIF();

TEXT();
    TEXT("Compressed RET pseudo-instruction");
    SETVAL(v_c_ret, ALLZEROS());
    IF (EQ(BITS(comb.vb_tmp, 15, 0), CONST("0x8082", 16)));
        SETVAL(v_c_ret, i_c_valid);
    ENDIF();

TEXT();
    IF (NZ(v_jal));
        SETVAL(vb_npc, comb.vb_jal_addr);
    ELSIF (NZ(v_branch));
        SETVAL(vb_npc, comb.vb_branch_addr);
    ELSIF (NZ(v_c_j));
        SETVAL(vb_npc, comb.vb_c_j_addr);
    ELSIF (NZ(v_c_ret));
        SETVAL(vb_npc, i_ra);
    ELSE();
        SETVAL(vb_npc, ADD2(comb.vb_pc, CONST("4")));
    ENDIF();

TEXT();
    SETVAL(o_jmp, OR4(v_jal, v_branch, v_c_j, v_c_ret));
    SETVAL(o_pc, comb.vb_pc);
    SETVAL(o_npc, comb.vb_npc);
}

