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

#include "mpu.h"

MPU::MPU(GenObject *parent, const char *name) :
    ModuleObject(parent, "MPU", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_iaddr(this, "i_iaddr", "CFG_CPU_ADDR_BITS"),
    i_daddr(this, "i_daddr", "CFG_CPU_ADDR_BITS"),
    i_region_we(this, "i_region_we", "1"),
    i_region_idx(this, "i_region_idx", "CFG_MPU_TBL_WIDTH"),
    i_region_addr(this, "i_region_addr", "CFG_CPU_ADDR_BITS"),
    i_region_mask(this, "i_region_mask", "CFG_CPU_ADDR_BITS"),
    i_region_flags(this, "i_region_flags", "CFG_MPU_FL_TOTAL", "{ena, cachable, r, w, x}"),
    o_iflags(this, "o_iflags", "CFG_MPU_FL_TOTAL"),
    o_dflags(this, "o_dflags", "CFG_MPU_FL_TOTAL"),
    MpuTableItemTypeDef_(this, -1),
    // registers
    tbl(this, "tbl", "CFG_MPU_TBL_SIZE", true),
    // process
    comb(this)
{
    Operation::start(&comb);
    proc_comb();
}

void MPU::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;
    GenObject *i;

    SETVAL(comb.v_iflags, ALLONES());
    SETVAL(comb.v_dflags, ALLONES());

TEXT();
    SETVAL(comb.vb_flags, i_region_flags);
    IF (NZ(BIT(i_region_flags, cfg->CFG_MPU_FL_ENA)));
        SETVAL(comb.vb_addr, i_region_addr);
        SETVAL(comb.vb_mask, i_region_mask);
    ELSE();
        SETVAL(comb.vb_addr, ALLONES());
        SETVAL(comb.vb_mask, ALLONES());
    ENDIF();

TEXT();
    i = &FOR ("i", CONST("0"), cfg->CFG_MPU_TBL_SIZE, "++");
        IF (EQ(ARRITEM_B(tbl, *i, tbl->addr), AND2_L(i_iaddr, ARRITEM_B(tbl, *i, tbl->mask))));
            IF (NZ(BIT(ARRITEM_B(tbl, *i, tbl->flags), cfg->CFG_MPU_FL_ENA)));
                SETVAL(comb.v_iflags, ARRITEM(tbl, *i, tbl->flags));
            ENDIF();
        ENDIF();

        TEXT();
        IF (EQ(ARRITEM_B(tbl, *i, tbl->addr), AND2_L(i_daddr, ARRITEM_B(tbl, *i, tbl->mask))));
            IF (NZ(BIT(ARRITEM_B(tbl, *i, tbl->flags), cfg->CFG_MPU_FL_ENA)));
                SETVAL(comb.v_dflags, ARRITEM(tbl, *i, tbl->flags));
            ENDIF();
        ENDIF();
    ENDFOR();

TEXT();
    IF (NZ(i_region_we));
        SETARRITEM(tbl, TO_INT(i_region_idx), tbl->addr, comb.vb_addr);
        SETARRITEM(tbl, TO_INT(i_region_idx), tbl->mask, comb.vb_mask);
        SETARRITEM(tbl, TO_INT(i_region_idx), tbl->flags, comb.vb_flags);
    ENDIF();

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_iflags, comb.v_iflags);
    SETVAL(o_dflags, comb.v_dflags);
}
