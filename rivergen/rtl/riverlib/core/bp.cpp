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
    ModuleObject(parent, "bp"),
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
    PreDecTypeDef_(this, "PreDecType"),
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
    comb(this)
{
}

void BranchPredictor::proc_comb() {

TEXT();
    SYNC_RESET(*this);
}

