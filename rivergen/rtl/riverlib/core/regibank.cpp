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

#include "regibank.h"

RegIntBank::RegIntBank(GenObject *parent, const char *name) :
    ModuleObject(parent, "RegIntBank", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_radr1(this, "i_radr1", "6", "Port 1 read address"),
    o_rdata1(this, "o_rdata1", "RISCV_ARCH", "Port 1 read value"),
    o_rtag1(this, "o_rtag1", "CFG_REG_TAG_WIDTH", "Port 1 read tag value"),
    i_radr2(this, "i_radr2", "6", "Port 2 read address"),
    o_rdata2(this, "o_rdata2", "RISCV_ARCH", "Port 2 read value"),
    o_rtag2(this, "o_rtag2", "CFG_REG_TAG_WIDTH", "Port 2 read tag value"),
    i_waddr(this, "i_waddr", "6", "Writing value"),
    i_wena(this, "i_wena", "1", "Writing is enabled"),
    i_wtag(this, "i_wtag", "CFG_REG_TAG_WIDTH", "Writing register tag"),
    i_wdata(this, "i_wdata", "RISCV_ARCH", "Writing value"),
    i_inorder(this, "i_inorder", "1", "Writing only if tag sequenced"),
    o_ignored(this, "o_ignored", "1", "Sequenced writing is ignored because it was overwritten by executor (need for tracer)"),
    i_dport_addr(this, "i_dport_addr", "6", "Debug port address"),
    i_dport_ena(this, "i_dport_ena", "1", "Debug port is enabled"),
    i_dport_write(this, "i_dport_write", "1", "Debug port write is enabled"),
    i_dport_wdata(this, "i_dport_wdata", "RISCV_ARCH", "Debug port write value"),
    o_dport_rdata(this, "o_dport_rdata", "RISCV_ARCH", "Debug port read value"),
    o_ra(this, "o_ra", "RISCV_ARCH", "Return address for branch predictor"),
    o_sp(this, "o_sp", "RISCV_ARCH", "Stack Pointer for border control"),
    // struct declaration
    RegValueTypeDef_(this, -1),
    // registers
    reg(this, "reg", "REGS_TOTAL", true),
    // process
    comb(this)
{
}

void RegIntBank::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;

    SETVAL(comb.int_daddr, TO_INT(i_dport_addr));
    SETVAL(comb.int_waddr, TO_INT(i_waddr));
    SETVAL(comb.int_radr1, TO_INT(i_radr1));
    SETVAL(comb.int_radr2, TO_INT(i_radr2));

TEXT();
    SETVAL(comb.next_tag, INC(ARRITEM_B(reg, comb.int_waddr, reg->tag)));
    IF (EQ(comb.next_tag, i_wtag));
        SETONE(comb.v_inordered);
    ENDIF();

TEXT();
    TEXT("Debug port has lower priority to avoid system hangup due the tags error");
    IF (AND3(NZ(i_wena), NZ(i_waddr), OR2(INV(i_inorder), comb.v_inordered)));
        SETARRITEM(reg, comb.int_waddr, reg->val, i_wdata);
        SETARRITEM(reg, comb.int_waddr, reg->tag, i_wtag);
    ELSIF (AND2(i_dport_ena, i_dport_write));
        IF (NZ(i_dport_addr));
            SETARRITEM(reg, comb.int_daddr, reg->val, i_dport_wdata);
        ENDIF();
    ENDIF();

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_ignored, AND4(i_wena, NZ(i_waddr), i_inorder, INV(comb.v_inordered)));
    SETVAL(o_rdata1, ARRITEM(reg, comb.int_radr1, reg->val));
    SETVAL(o_rtag1, ARRITEM(reg, comb.int_radr1, reg->tag));
    SETVAL(o_rdata2, ARRITEM(reg, comb.int_radr2, reg->val));
    SETVAL(o_rtag2, ARRITEM(reg, comb.int_radr2, reg->tag));
    SETVAL(o_dport_rdata, ARRITEM(reg, comb.int_daddr, reg->val));
    SETVAL(o_ra, ARRITEM(reg, cfg->REG_RA, reg->val));
    SETVAL(o_sp, ARRITEM(reg, cfg->REG_SP, reg->val));

}

