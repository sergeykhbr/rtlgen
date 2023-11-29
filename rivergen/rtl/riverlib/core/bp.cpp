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

#include "bp.h"

BranchPredictor::BranchPredictor(GenObject *parent, const char *name) :
    ModuleObject(parent, "BranchPredictor", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_flush_pipeline(this, "i_flush_pipeline", "1", "sync reset BTB"),
    i_resp_mem_valid(this, "i_resp_mem_valid", "1", "Memory response from ICache is valid"),
    i_resp_mem_addr(this, "i_resp_mem_addr", "RISCV_ARCH", "Memory response address"),
    i_resp_mem_data(this, "i_resp_mem_data", "64", "Memory response value"),
    i_e_jmp(this, "i_e_jmp", "1", "jump was executed"),
    i_e_pc(this, "i_e_pc", "RISCV_ARCH", "Previous 'Executor' instruction"),
    i_e_npc(this, "i_e_npc", "RISCV_ARCH","Valid instruction value awaited by 'Executor'"),
    i_ra(this, "i_ra", "RISCV_ARCH", "Return address register value"),
    o_f_valid(this, "o_f_valid", "1", "Fetch request is valid"),
    o_f_pc(this, "o_f_pc", "RISCV_ARCH", "Fetching instruction pointer"),
    i_f_requested_pc(this, "i_f_requested_pc", "RISCV_ARCH", "already requested but not accepted address"),
    i_f_fetching_pc(this, "i_f_fetching_pc", "RISCV_ARCH", "currently memory address"),
    i_f_fetched_pc(this, "i_f_fetched_pc", "RISCV_ARCH", "already requested and fetched address"),
    i_d_pc(this, "i_d_pc", "RISCV_ARCH", "decoded instructions"),
    // struct declaration
    PreDecTypeDef_(this, "PreDecType", NO_COMMENT),
    // Signals
    wb_pd(this, "wb_pd", "2", NO_COMMENT),
    w_btb_e(this, "w_btb_e", "1"),
    w_btb_we(this, "w_btb_we", "1"),
    wb_btb_we_pc(this, "wb_btb_we_pc", "RISCV_ARCH"),
    wb_btb_we_npc(this, "wb_btb_we_npc", "RISCV_ARCH"),
    wb_start_pc(this, "wb_start_pc", "RISCV_ARCH"),
    wb_npc(this, "wb_npc", "MUL(CFG_BP_DEPTH,RISCV_ARCH)"),
    wb_bp_exec(this, "wb_bp_exec", "CFG_BP_DEPTH", "0", "Predicted value was jump-executed before"),
    predec(this, "predec", "2"),
    btb(this, "btb"),
    // registers
    // process
    comb(this)
{
    Operation::start(this);

    // Create and connet Sub-modules:
    GenObject &i = FORGEN ("i", CONST("0"), CONST("2"), "++", new STRING("predecx"));
        NEW(predec, predec.getName().c_str(), &i);
            CONNECT(predec, &i, predec.i_c_valid, ARRITEM(wb_pd, i, wb_pd.c_valid));
            CONNECT(predec, &i, predec.i_addr, ARRITEM(wb_pd, i, wb_pd.addr));
            CONNECT(predec, &i, predec.i_data, ARRITEM(wb_pd, i, wb_pd.data));
            CONNECT(predec, &i, predec.i_ra, i_ra);
            CONNECT(predec, &i, predec.o_jmp, ARRITEM(wb_pd, i, wb_pd.jmp));
            CONNECT(predec, &i, predec.o_pc, ARRITEM(wb_pd, i, wb_pd.pc));
            CONNECT(predec, &i, predec.o_npc, ARRITEM(wb_pd, i, wb_pd.npc));
        ENDNEW();
    ENDFORGEN(new STRING("predecx"));

TEXT();
    NEW(btb, btb.getName().c_str());
        CONNECT(btb, 0, btb.i_clk, i_clk);
        CONNECT(btb, 0, btb.i_nrst, i_nrst);
        CONNECT(btb, 0, btb.i_flush_pipeline, i_flush_pipeline);
        CONNECT(btb, 0, btb.i_e, w_btb_e);
        CONNECT(btb, 0, btb.i_we, w_btb_we);
        CONNECT(btb, 0, btb.i_we_pc, wb_btb_we_pc);
        CONNECT(btb, 0, btb.i_we_npc, wb_btb_we_npc);
        CONNECT(btb, 0, btb.i_bp_pc, wb_start_pc);
        CONNECT(btb, 0, btb.o_bp_npc, wb_npc);
        CONNECT(btb, 0, btb.o_bp_exec, wb_bp_exec);
    ENDNEW();
}

void BranchPredictor::CombProcess::proc_comb() {
    BranchPredictor *p = static_cast<BranchPredictor *>(getParent());
    Operation::start(this);
    river_cfg *cfg = glob_river_cfg_;

    TEXT("Transform address into 2-dimesional array for convinience");
    GenObject *i = &FOR ("i", CONST("0"), cfg->CFG_BP_DEPTH, "++");
        SETARRITEM(vb_addr, *i, vb_addr, BITSW(p->wb_npc, 
                                               MUL2(*i, cfg->RISCV_ARCH),
                                               cfg->RISCV_ARCH));
    ENDFOR();

TEXT();
    SETARRITEM(vb_piped, CONST("0"), vb_piped, BITS(p->i_d_pc, DEC(cfg->RISCV_ARCH), CONST("2")));
    SETARRITEM(vb_piped, CONST("1"), vb_piped, BITS(p->i_f_fetched_pc, DEC(cfg->RISCV_ARCH), CONST("2")));
    SETARRITEM(vb_piped, CONST("2"), vb_piped, BITS(p->i_f_fetching_pc, DEC(cfg->RISCV_ARCH), CONST("2")));
    SETARRITEM(vb_piped, CONST("3"), vb_piped, BITS(p->i_f_requested_pc, DEC(cfg->RISCV_ARCH), CONST("2")));

    TEXT("Check availablity of pc in pipeline");
    SETVAL(vb_hit, ALLZEROS());
    GenObject &n = FOR ("n", CONST("0"), CONST("4"), "++");
        i = &FOR ("i", n,  CONST("4"), "++");
            IF (EQ(BITS(ARRITEM(vb_addr, n, vb_addr), DEC(cfg->RISCV_ARCH), CONST("2")),
                   ARRITEM(vb_piped, *i, vb_piped)));
                SETBIT(vb_hit, n, CONST("1", 1));
            ENDIF();
        ENDFOR();
    ENDFOR();

TEXT();
    SETVAL(vb_fetch_npc, ARRITEM(vb_addr, DEC(cfg->CFG_BP_DEPTH), vb_addr));
    i = &FOR ("i", CONST("3"), CONST("0"), "--");
        IF (EZ(BIT(vb_hit, *i)));
            SETVAL(vb_fetch_npc, ARRITEM(vb_addr, *i, vb_addr));
        ENDIF();
    ENDFOR();

TEXT();
    TEXT("Pre-decoder input signals (not used for now)");
    i = &FOR ("i", CONST("0"), CONST("2"), "++");
        SETARRITEM(p->wb_pd, *i, p->wb_pd.c_valid,
                   INV(AND_REDUCE(BITSW(p->i_resp_mem_data,
                                       MUL2(CONST("16"),*i),
                                       CONST("2")))));
        SETARRITEM(p->wb_pd, *i, p->wb_pd.addr,
                    ADD2(p->i_resp_mem_addr, MUL2(CONST("2"), *i)));
        SETARRITEM(p->wb_pd, *i, p->wb_pd.data,
                    BITSW(p->i_resp_mem_data, MUL2(CONST("16"),*i),
                                              CONST("32")));
    ENDFOR();
    SETVAL(vb_ignore_pd, ALLZEROS());
    i = &FOR ("i", CONST("0"), CONST("4"), "++");
        IF (EQ(BITS(ARRITEM_B(p->wb_pd, CONST("0"), p->wb_pd.npc), DEC(cfg->RISCV_ARCH), CONST("2")),
               ARRITEM(vb_piped, *i, vb_piped)));
            SETBIT(vb_ignore_pd, 0, CONST("1", 1));
        ENDIF();
        IF (EQ(BITS(ARRITEM_B(p->wb_pd, CONST("1"), p->wb_pd.npc), DEC(cfg->RISCV_ARCH), CONST("2")),
               ARRITEM(vb_piped, *i, vb_piped)));
            SETBIT(vb_ignore_pd, 1, CONST("1", 1));
        ENDIF();
    ENDFOR();

TEXT();
    SETVAL(v_btb_we, OR3(p->i_e_jmp,
                         ARRITEM(p->wb_pd, CONST("0"), p->wb_pd.jmp),
                         ARRITEM(p->wb_pd, CONST("1"), p->wb_pd.jmp)));
    IF (NZ(p->i_e_jmp));
        SETVAL(vb_btb_we_pc, p->i_e_pc);
        SETVAL(vb_btb_we_npc, p->i_e_npc);
    ELSIF (ARRITEM(p->wb_pd, CONST("0"), p->wb_pd.jmp));
        SETVAL(vb_btb_we_pc, ARRITEM(p->wb_pd, CONST("0"), p->wb_pd.pc));
        SETVAL(vb_btb_we_npc, ARRITEM(p->wb_pd, CONST("0"), p->wb_pd.npc));
        IF (AND3(EQ(BITS(vb_hit, 2, 0), CONST("0x7", 3)),
                 EZ(BIT(p->wb_bp_exec, 2)),
                 EZ(BIT(vb_ignore_pd, 0))));
            SETVAL(vb_fetch_npc, ARRITEM(p->wb_pd, CONST("0"), p->wb_pd.npc));
        ENDIF();
    ELSIF (ARRITEM(p->wb_pd, CONST("1"), p->wb_pd.jmp));
        SETVAL(vb_btb_we_pc, ARRITEM(p->wb_pd, CONST("1"), p->wb_pd.pc));
        SETVAL(vb_btb_we_npc, ARRITEM(p->wb_pd, CONST("1"), p->wb_pd.npc));
        IF (AND3(EQ(BITS(vb_hit, 2, 0), CONST("0x7", 3)),
                 EZ(BIT(p->wb_bp_exec, 2)),
                 EZ(BIT(vb_ignore_pd, 1))));
            SETVAL(vb_fetch_npc, ARRITEM(p->wb_pd, CONST("1"), p->wb_pd.npc));
        ENDIF();
    ELSE();
        SETVAL(vb_btb_we_pc, p->i_e_pc);
        SETVAL(vb_btb_we_npc, p->i_e_npc);
    ENDIF();

TEXT();
    SETVAL(p->wb_start_pc, p->i_e_npc);
    SETVAL(p->w_btb_e, p->i_e_jmp);
    SETVAL(p->w_btb_we, v_btb_we);
    SETVAL(p->wb_btb_we_pc, vb_btb_we_pc);
    SETVAL(p->wb_btb_we_npc, vb_btb_we_npc);

TEXT();
    SETVAL(p->o_f_valid, CONST("1", 1));
    SETVAL(p->o_f_pc, CC2(BITS(vb_fetch_npc, DEC(cfg->RISCV_ARCH), CONST("2")), CONST("0", 2)));
}

