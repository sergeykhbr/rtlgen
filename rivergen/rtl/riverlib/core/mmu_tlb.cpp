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
    mem.changeTmplParameter("abits", "CFG_MMU_TLB_AWIDTH");
    mem.changeTmplParameter("dbits", "8");
    GenObject &i = FORGEN ("i", CONST("0"), CONST("CFG_MMU_PTE_DBYTES"), "++", new STRING("memx"));
        NEW(*mem.arr_[0], mem.getName().c_str(), &i);
            CONNECT(mem, &i, mem->i_clk, i_clk);
            CONNECT(mem, &i, mem->i_adr, i_adr);
            CONNECT(mem, &i, mem->i_wena, i_wena);
            CONNECT(mem, &i, mem->i_wdata, ARRITEM(wb_mem_data, i, wb_mem_data->wdata));
            CONNECT(mem, &i, mem->o_rdata, ARRITEM(wb_mem_data, i, wb_mem_data->rdata));
        ENDNEW();
    ENDFORGEN(new STRING("memx"));
}

void MmuTlb::proc_comb() {
    GenObject &i = FOR ("i", CONST("0"), CONST("CFG_MMU_PTE_DBYTES"), "++");
        SETARRITEM(wb_mem_data, i, wb_mem_data->wdata,
                  BIG_TO_U64(BITSW(i_wdata, MUL2(CONST("8"), i), CONST("8"))));
        SETBITSW(comb.vb_rdata, MUL2(CONST("8"), i),
                                CONST("8"),
                                ARRITEM(wb_mem_data, i, wb_mem_data->rdata));
    ENDFOR();
    SETVAL(o_rdata, comb.vb_rdata);
}
