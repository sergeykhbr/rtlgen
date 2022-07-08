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

BpBTB::BpBTB(GenObject *parent, river_cfg *cfg) :
    ModuleObject(parent, "BpBTB"),
    cfg_(cfg),
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
    btb(this, "btb"),
    dbg_npc(this, "dbg_npc"),
    // registers
    // process
    comb(this)
{
}

void BpBTB::proc_comb() {

TEXT();
    SYNC_RESET(*this);
}

