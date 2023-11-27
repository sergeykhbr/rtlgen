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

#include "pmp.h"

PMP::PMP(GenObject *parent, const char *name) :
    ModuleObject(parent, "PMP", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_ena(this, "i_ena", "1", "PMP is active in S or U modes or if L/MPRV bit is set in M-mode"),
    i_iaddr(this, "i_iaddr", "CFG_CPU_ADDR_BITS"),
    i_daddr(this, "i_daddr", "CFG_CPU_ADDR_BITS"),
    i_we(this, "i_we", "1", "write enable into PMP"),
    i_region(this, "i_region", "CFG_PMP_TBL_WIDTH", "selected PMP region"),
    i_start_addr(this, "i_start_addr", "RISCV_ARCH", "PMP region start address"),
    i_end_addr(this, "i_end_addr", "RISCV_ARCH", "PMP region end address (inclusive)"),
    i_flags(this, "i_flags", "CFG_PMP_FL_TOTAL", "{ena, lock, r, w, x}"),
    o_r(this, "o_r", "1"),
    o_w(this, "o_w", "1"),
    o_x(this, "o_x", "1"),
    PmpTableItemTypeDef_(this, "PmpTableItemType"),
    // registers
    tbl(this, "tbl"),
    // process
    comb(this)
{
    Operation::start(&comb);
    proc_comb();
}

void PMP::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;
    GenObject *i;

    TEXT("PMP is active for S,U modes or in M-mode when L-bit is set:");
    SETVAL(comb.v_r, INV(i_ena));
    SETVAL(comb.v_w, INV(i_ena));
    SETVAL(comb.v_x, INV(i_ena));

TEXT();
    SETVAL(comb.vb_flags, i_flags);
    IF (NZ(BIT(i_flags, cfg->CFG_PMP_FL_V)));
        SETVAL(comb.vb_start_addr, i_start_addr);
        SETVAL(comb.vb_end_addr, i_end_addr);
    ELSE();
        SETVAL(comb.vb_start_addr, ALLZEROS());
        SETVAL(comb.vb_end_addr, ALLZEROS());
    ENDIF();

TEXT();
    i = &FOR ("i", DEC(cfg->CFG_PMP_TBL_SIZE), CONST("0"), "--");
        IF (ANDx(2, &GE(i_iaddr, BITS(ARRITEM_B(tbl, *i, tbl.start_addr), DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0"))),
                    &LE(i_iaddr, BITS(ARRITEM_B(tbl, *i, tbl.end_addr), DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0")))));
            IF (ANDx(2, &NZ(BIT(ARRITEM_B(tbl, *i, tbl.flags), cfg->CFG_PMP_FL_V)),
                        &OR2(i_ena, BIT(ARRITEM_B(tbl, *i, tbl.flags), cfg->CFG_PMP_FL_L))));
                SETVAL(comb.v_x, BIT(ARRITEM_B(tbl, *i, tbl.flags), cfg->CFG_PMP_FL_X));
            ENDIF();
        ENDIF();

        TEXT();
        IF (ANDx(2, &GE(i_daddr, BITS(ARRITEM_B(tbl, *i, tbl.start_addr), DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0"))),
                    &LE(i_daddr, BITS(ARRITEM_B(tbl, *i, tbl.end_addr), DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0")))));
            IF (ANDx(2, &NZ(BIT(ARRITEM_B(tbl, *i, tbl.flags), cfg->CFG_PMP_FL_V)),
                        &OR2(i_ena, BIT(ARRITEM_B(tbl, *i, tbl.flags), cfg->CFG_PMP_FL_L))));
                SETVAL(comb.v_r, BIT(ARRITEM_B(tbl, *i, tbl.flags), cfg->CFG_PMP_FL_R));
                SETVAL(comb.v_w, BIT(ARRITEM_B(tbl, *i, tbl.flags), cfg->CFG_PMP_FL_W));
            ENDIF();
        ENDIF();
    ENDFOR();

TEXT();
    IF (NZ(i_we));
        SETARRITEM(tbl, TO_INT(i_region), tbl.start_addr, comb.vb_start_addr);
        SETARRITEM(tbl, TO_INT(i_region), tbl.end_addr, comb.vb_end_addr);
        SETARRITEM(tbl, TO_INT(i_region), tbl.flags, comb.vb_flags);
    ENDIF();

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_r, comb.v_r);
    SETVAL(o_w, comb.v_w);
    SETVAL(o_x, comb.v_x);
}
