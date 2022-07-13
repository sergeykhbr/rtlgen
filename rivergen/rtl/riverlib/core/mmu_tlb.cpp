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

#include "mmu_tlb.h"

MmuTlb::MmuTlb(GenObject *parent, const char *name) :
    ModuleObject(parent, "MmuTlb", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_adr(this, "i_adr", "CFG_MMU_TLB_AWIDTH"),
    i_wena(this, "i_wena", "1"),
    i_wdata(this, "i_wdata", "CFG_MMU_PTE_DWIDTH"),
    o_rdata(this, "o_rdata", "CFG_MMU_PTE_DWIDTH"),
    // struct declaration
    MemTypeDef_(this, -1),
    // Signals
    wb_mem_data(this, "wb_mem_data", "CFG_MMU_PTE_DBYTES"),
    // sub-modules
    mem(this, "mem", "CFG_MMU_PTE_DWIDTH"),
    // process
    comb(this)
{
    mem.disableVcd();
    Operation::start(this);

    // Create and connet Sub-modules:
    GenObject &i = FOR ("i", CONST("0"), CONST("CFG_MMU_PTE_DBYTES"), "++");
        NEW(*mem.arr_[0], mem.getName().c_str(), &i);
            CONNECT(mem, &i, mem->abits, glob_river_cfg_->CFG_MMU_TLB_AWIDTH);
            CONNECT(mem, &i, mem->dbits, CONST("8"));
            CONNECT(mem, &i, mem->i_clk, i_clk);
            CONNECT(mem, &i, mem->i_adr, i_adr);
            CONNECT(mem, &i, mem->i_wena, i_wena);
            CONNECT(mem, &i, mem->i_wdata, ARRITEM(wb_mem_data, i, wb_mem_data->wdata));
            CONNECT(mem, &i, mem->o_rdata, ARRITEM(wb_mem_data, i, wb_mem_data->rdata));
        ENDNEW();
    ENDFOR();
}

void MmuTlb::proc_comb() {
    GenObject &i = FOR ("i", CONST("0"), CONST("CFG_MMU_PTE_DBYTES"), "++");
        SETVAL(ARRITEM(wb_mem_data, i, wb_mem_data->wdata), BITS(i_wdata, DEC(MUL2(CONST("8"), INC(i))),
                                                                          MUL2(CONST("8"), i)));
        SETBITS(o_rdata, DEC(MUL2(CONST("8"), INC(i))),
                         MUL2(CONST("8"), i),
                         ARRITEM(wb_mem_data, i, wb_mem_data->rdata));
    ENDFOR();
}
