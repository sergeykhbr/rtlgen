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

#include "bp_btb.h"

BpBTB::BpBTB(GenObject *parent, const char *name) :
    ModuleObject(parent, "BpBTB", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_flush_pipeline(this, "i_flush_pipeline", "1", "sync reset BTB"),
    i_e(this, "i_e", "1", "executed jump"),
    i_we(this, "i_we", "1", "Write enable into BTB"),
    i_we_pc(this, "i_we_pc", "CFG_CPU_ADDR_BITS", "Jump start instruction address"),
    i_we_npc(this, "i_we_npc", "CFG_CPU_ADDR_BITS", "Jump target address"),
    i_bp_pc(this, "i_bp_pc", "CFG_CPU_ADDR_BITS", "Start address of the prediction sequence"),
    o_bp_npc(this, "o_bp_npc", "MUL(CFG_BP_DEPTH,CFG_CPU_ADDR_BITS)", "Predicted sequence"),
    o_bp_exec(this, "o_bp_exec", "CFG_BP_DEPTH", "Predicted value was jump-executed before"),
    // struct declaration
    BtbEntryTypeDef_(this, -1),
    // Signals
    btb(this, "btb", "CFG_BTB_SIZE", true),
    dbg_npc(this, "dbg_npc", "CFG_CPU_ADDR_BITS", "CFG_BP_DEPTH"),
    // registers
    // process
    comb(this)
{
}

void BpBTB::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;

    SETBITS(comb.vb_addr, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0"), i_bp_pc);
    SETBIT(comb.vb_bp_exec, 0, i_e);

TEXT();
    GenObject *i = &FOR("i", CONST("1"), cfg->CFG_BP_DEPTH, "++");
        SETVAL(comb.t_addr, BITS(comb.vb_addr, MUL2(*i, DEC(cfg->CFG_CPU_ADDR_BITS)),
                                               MUL2(DEC(*i), cfg->CFG_CPU_ADDR_BITS)));
        GenObject &n = FOR("n", DEC(cfg->CFG_BTB_SIZE), CONST("0"), "--");
            IF (EQ(comb.t_addr, ARRITEM(btb, n, btb->pc)));
                SETBITS(comb.vb_addr, DEC(MUL2(INC(*i), cfg->CFG_CPU_ADDR_BITS)),
                                      MUL2(*i, cfg->CFG_CPU_ADDR_BITS), ARRITEM(btb, n, btb->npc));
                SETBIT(comb.vb_hit, *i, CONST("1", 1));
                SETBIT(comb.vb_bp_exec, *i, btb->exec,  "Used for: Do not override by pre-decoded jumps");
            ELSIF(EZ(BIT(comb.vb_hit, *i)));
                SETBITS(comb.vb_addr, DEC(MUL2(INC(*i), cfg->CFG_CPU_ADDR_BITS)),
                                      MUL2(*i, cfg->CFG_CPU_ADDR_BITS), ADD2(comb.t_addr, CONST("4")));
            ENDIF();
        ENDFOR();
    ENDFOR();

TEXT();
    SETVAL(comb.v_dont_update, ALLZEROS());
    SETVAL(comb.vb_pc_equal, ALLZEROS());
    i = &FOR("i", CONST("0"), cfg->CFG_BTB_SIZE, "++");
        IF (EQ(ARRITEM(btb, *i, btb->pc), i_we_pc));
            SETBIT(comb.vb_pc_equal, *i, CONST("1"));
            SETVAL(comb.v_dont_update, AND2(ARRITEM(btb, *i, btb->exec), INV(i_e)));
        ENDIF();
    ENDFOR();
    SETVAL(comb.vb_pc_nshift, ALLZEROS());
    FOR ("i", CONST("1"), cfg->CFG_BTB_SIZE, "++");
        SETBIT(comb.vb_pc_nshift, *i, OR2(BIT(comb.vb_pc_equal, DEC(*i)), 
                                         BIT(comb.vb_pc_nshift, DEC(*i))));
    ENDFOR();

TEXT();
    IF (NZ(AND2(i_we, INV(comb.v_dont_update))));
        SETARRITEM(btb, CONST("0"), btb->exec, i_e);
        SETARRITEM(btb, CONST("0"), btb->pc, i_we_pc);
        SETARRITEM(btb, CONST("0"), btb->npc, i_we_npc);
        i = &FOR ("i", CONST("1"), cfg->CFG_BTB_SIZE, "++");
            IF (EZ(BIT(comb.vb_pc_nshift, *i)));
                SETARRITEM(btb, *i, btb, ARRITEM(btb, DEC(*i), btb));
            ELSE();
                SETARRITEM(btb, *i, btb, ARRITEM(btb, *i, btb));
            ENDIF();
        ENDFOR();
    ENDIF();

TEXT();
    SYNC_RESET(*this, &i_flush_pipeline);

TEXT();
    i = &FOR("i", CONST("0"), cfg->CFG_BP_DEPTH, "++");
        SETARRITEM(dbg_npc, *i, dbg_npc, BIG_TO_U64(BITS(comb.vb_addr, DEC(MUL2(INC(*i), cfg->CFG_CPU_ADDR_BITS)),
                                                                       MUL2(*i, cfg->CFG_CPU_ADDR_BITS))));
    ENDFOR();
    SETVAL(o_bp_npc, comb.vb_addr);
    SETVAL(o_bp_exec, comb.vb_bp_exec);

}

