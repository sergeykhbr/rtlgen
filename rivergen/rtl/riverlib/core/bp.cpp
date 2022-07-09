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

BranchPredictor::BranchPredictor(GenObject *parent, river_cfg *cfg) :
    ModuleObject(parent, "BranchPredictor"),
    cfg_(cfg),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_flush_pipeline(this, "i_flush_pipeline", "1", "sync reset BTB"),
    i_resp_mem_valid(this, "i_resp_mem_valid", "1", "Memory response from ICache is valid"),
    i_resp_mem_addr(this, "i_resp_mem_addr", "CFG_CPU_ADDR_BITS", "Memory response address"),
    i_resp_mem_data(this, "i_resp_mem_data", "64", "Memory response value"),
    i_e_jmp(this, "i_e_jmp", "1", "jump was executed"),
    i_e_pc(this, "i_e_pc", "CFG_CPU_ADDR_BITS", "Previous 'Executor' instruction"),
    i_e_npc(this, "i_e_npc", "CFG_CPU_ADDR_BITS","Valid instruction value awaited by 'Executor'"),
    i_ra(this, "i_ra", "RISCV_ARCH", "Return address register value"),
    o_f_valid(this, "o_f_valid", "1", "Fetch request is valid"),
    o_f_pc(this, "o_f_pc", "CFG_CPU_ADDR_BITS", "Fetching instruction pointer"),
    i_f_requested_pc(this, "i_f_requested_pc", "CFG_CPU_ADDR_BITS", "already requested but not accepted address"),
    i_f_fetching_pc(this, "i_f_fetching_pc", "CFG_CPU_ADDR_BITS", "currently memory address"),
    i_f_fetched_pc(this, "i_f_fetched_pc", "CFG_CPU_ADDR_BITS", "already requested and fetched address"),
    i_d_pc(this, "i_d_pc", "CFG_CPU_ADDR_BITS", "decoded instructions"),
    // struct declaration
    PreDecTypeDef_(this, -1),
    // Signals
    wb_pd(this, "wb_pd"),
    w_btb_e(this, "w_btb_e", "1"),
    w_btb_we(this, "w_btb_we", "1"),
    wb_btb_we_pc(this, "wb_btb_we_pc", "CFG_CPU_ADDR_BITS"),
    wb_btb_we_npc(this, "wb_btb_we_npc", "CFG_CPU_ADDR_BITS"),
    wb_start_pc(this, "wb_start_pc", "CFG_CPU_ADDR_BITS"),
    wb_npc(this, "wb_npc", "MUL(CFG_BP_DEPTH,CFG_CPU_ADDR_BITS)"),
    wb_bp_exec(this, "wb_bp_exec", "CFG_BP_DEPTH", "Predicted value was jump-executed before"),

    // registers
    // process
    comb(this, cfg)
{
    ModuleObject *p;

    // Create and connet Sub-modules:
    p = static_cast<ModuleObject *>(SCV_get_module("BpBTB"));
    if (p) {
        btb = p->createInstance(this, "btb");
        btb->connect_io("i_clk", &i_clk);
        btb->connect_io("i_nrst", &i_nrst);
        btb->connect_io("i_flush_pipeline", &i_flush_pipeline);
        btb->connect_io("i_e", &w_btb_e);
        btb->connect_io("i_we", &w_btb_we);
        btb->connect_io("i_we_pc", &wb_btb_we_pc);
        btb->connect_io("i_we_npc", &wb_btb_we_npc);
        btb->connect_io("i_bp_pc", &wb_start_pc);
        btb->connect_io("o_bp_npc", &wb_npc);
        btb->connect_io("o_bp_exec", &wb_bp_exec);
    } else {
        SHOW_ERROR("%s", "BpBTB not found");
        btb = 0;
    }

}

void BranchPredictor::CombProcess::proc_comb() {
    BranchPredictor *p = static_cast<BranchPredictor *>(getParent());

    TEXT("Transform address into 2-dimesional array for convinience");
    GenObject *i = &FOR ("i", CONST("0"), cfg_->CFG_BP_DEPTH, "++");
        SETARRITEM(vb_addr, *i, vb_addr, BITS(p->wb_npc, DEC(MUL2(INC(*i), cfg_->CFG_CPU_ADDR_BITS)),
                                                         MUL2(*i, cfg_->CFG_CPU_ADDR_BITS)));
    ENDFOR();

TEXT();
    SETARRITEM(vb_piped, CONST("0"), vb_piped, BITS(p->i_d_pc, DEC(cfg_->CFG_CPU_ADDR_BITS), CONST("2")));
    SETARRITEM(vb_piped, CONST("1"), vb_piped, BITS(p->i_f_fetched_pc, DEC(cfg_->CFG_CPU_ADDR_BITS), CONST("2")));
    SETARRITEM(vb_piped, CONST("2"), vb_piped, BITS(p->i_f_fetching_pc, DEC(cfg_->CFG_CPU_ADDR_BITS), CONST("2")));
    SETARRITEM(vb_piped, CONST("3"), vb_piped, BITS(p->i_f_requested_pc, DEC(cfg_->CFG_CPU_ADDR_BITS), CONST("2")));

    SETVAL(vb_hit, ALLZEROS());
    GenObject &n = FOR ("n", CONST("0"), CONST("4"), "++");
        i = &FOR ("i", n,  CONST("4"), "++");
            IF (EQ(BITS(ARRITEM(vb_addr, n, vb_addr), DEC(cfg_->CFG_CPU_ADDR_BITS), CONST("2")),
                   ARRITEM(vb_piped, *i, vb_piped)));
                SETBIT(vb_hit, n, CONST("1"));
            ENDIF();
        ENDFOR();
    ENDFOR();

TEXT();
    SETVAL(vb_fetch_npc, ARRITEM(vb_addr, DEC(cfg_->CFG_BP_DEPTH), vb_addr));
    i = &FOR ("i", CONST("3"), CONST("0"), "--");
        IF (EZ(BIT(vb_hit, *i)));
            SETVAL(vb_fetch_npc, ARRITEM(vb_addr, *i, vb_addr));
        ENDIF();
    ENDFOR();

TEXT();
    TEXT("Pre-decoder input signals (not used for now)");
    i = &FOR ("i", CONST("0"), CONST("2"), "++");
        SETARRITEM(p->wb_pd, *i, p->wb_pd.arr_[0]->c_valid,
                   INV(AND_REDUCE(BITS(p->i_resp_mem_data,
                                       INC(MUL2(CONST("16"),*i)),
                                       MUL2(CONST("16"),*i)))));
        SETARRITEM(p->wb_pd, *i, p->wb_pd.arr_[0]->addr,
                    ADD2(p->i_resp_mem_addr, MUL2(CONST("2"), *i)));
        SETARRITEM(p->wb_pd, *i, p->wb_pd.arr_[0]->data,
                    BITS(p->i_resp_mem_data, ADD2(MUL2(CONST("16"),*i), CONST("31")),
                                             MUL2(CONST("16"),*i)));
    ENDFOR();
    SETVAL(vb_ignore_pd, ALLZEROS());
    i = &FOR ("i", CONST("0"), CONST("4"), "++");
        IF (EQ(BITS(ARRITEM(p->wb_pd, CONST("0"), p->wb_pd.arr_[0]->npc), DEC(cfg_->CFG_CPU_ADDR_BITS), CONST("2")),
               ARRITEM(vb_piped, *i, vb_piped)));
            SETBIT(vb_ignore_pd, 0, CONST("1", 1));
        ENDIF();
        IF (EQ(BITS(ARRITEM(p->wb_pd, CONST("1"), p->wb_pd.arr_[0]->npc), DEC(cfg_->CFG_CPU_ADDR_BITS), CONST("2")),
               ARRITEM(vb_piped, *i, vb_piped)));
            SETBIT(vb_ignore_pd, 1, CONST("1", 1));
        ENDIF();
    ENDFOR();

TEXT();
    SETVAL(v_btb_we, OR3(p->i_e_jmp,
                         ARRITEM(p->wb_pd, CONST("0"), p->wb_pd.arr_[0]->jmp),
                         ARRITEM(p->wb_pd, CONST("1"), p->wb_pd.arr_[0]->jmp)));
    IF (NZ(p->i_e_jmp));
        SETVAL(vb_btb_we_pc, p->i_e_pc);
        SETVAL(vb_btb_we_npc, p->i_e_npc);
    ELSIF (ARRITEM(p->wb_pd, CONST("0"), p->wb_pd.arr_[0]->jmp));
        SETVAL(vb_btb_we_pc, ARRITEM(p->wb_pd, CONST("0"), p->wb_pd.arr_[0]->pc));
        SETVAL(vb_btb_we_npc, ARRITEM(p->wb_pd, CONST("0"), p->wb_pd.arr_[0]->npc));
        IF (AND3(EQ(BITS(vb_hit, 2, 0), CONST("0x7", 3)),
                 EZ(BIT(p->wb_bp_exec, 2)),
                 EZ(BIT(vb_ignore_pd, 0))));
            SETVAL(vb_fetch_npc, ARRITEM(p->wb_pd, CONST("0"), p->wb_pd.arr_[0]->npc));
        ENDIF();
    ELSIF (ARRITEM(p->wb_pd, CONST("1"), p->wb_pd.arr_[0]->jmp));
        SETVAL(vb_btb_we_pc, ARRITEM(p->wb_pd, CONST("1"), p->wb_pd.arr_[0]->pc));
        SETVAL(vb_btb_we_npc, ARRITEM(p->wb_pd, CONST("1"), p->wb_pd.arr_[0]->npc));
        IF (AND3(EQ(BITS(vb_hit, 2, 0), CONST("0x7", 3)),
                 EZ(BIT(p->wb_bp_exec, 2)),
                 EZ(BIT(vb_ignore_pd, 1))));
            SETVAL(vb_fetch_npc, ARRITEM(p->wb_pd, CONST("1"), p->wb_pd.arr_[0]->npc));
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
    //o_f_pc = (vb_fetch_npc >> 2) << 2;

}

