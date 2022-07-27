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

#include "tracer.h"

Tracer::Tracer(GenObject *parent, const char *name) :
    ModuleObject(parent, "Tracer", name),
    trace_file(this, "trace_file", "trace_river_sysc"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_dbg_executed_cnt(this, "i_dbg_executed_cnt", "64"),
    i_e_valid(this, "i_e_valid", "1"),
    i_e_pc(this, "i_e_pc", "CFG_CPU_ADDR_BITS"),
    i_e_instr(this, "i_e_instr", "32"),
    i_e_wena(this, "i_e_wena", "1"),
    i_e_waddr(this, "i_e_waddr", "6"),
    i_e_wdata(this, "i_e_wdata", "RISCV_ARCH"),
    i_e_memop_valid(this, "i_e_memop_valid", "1"),
    i_e_memop_type(this, "i_e_memop_type", "MemopType_Total"),
    i_e_memop_size(this, "i_e_memop_size", "2"),
    i_e_memop_addr(this, "i_e_memop_addr", "CFG_CPU_ADDR_BITS"),
    i_e_memop_wdata(this, "i_e_memop_wdata", "RISCV_ARCH"),
    i_e_flushd(this, "i_e_flushd", "1"),
    i_m_pc(this, "i_m_pc", "CFG_CPU_ADDR_BITS", "executed memory/flush request only"),
    i_m_valid(this, "i_m_valid", "1", "memory/flush operation completed"),
    i_m_memop_ready(this, "i_m_memop_ready", "1"),
    i_m_wena(this, "i_m_wena", "1"),
    i_m_waddr(this, "i_m_waddr", "6"),
    i_m_wdata(this, "i_m_wdata", "RISCV_ARCH"),
    i_reg_ignored(this, "i_reg_ignored", "1"),
    // params
    TRACE_TBL_SZ(this, "TRACE_TBL_SZ", "64"),
    TaskDisassembler(this),
    // struct declaration
    MemopActionTypeDef_(this, -1),
    RegActionTypeDef_(this, -1),
    // registers
    memaction(this, "memaction", "TRACE_TBL_SZ", true),
    // process
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void Tracer::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;

TEXT();
    SYNC_RESET(*this);

}

