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

RegIntBank::RegIntBank(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "RegIntBank", name, comment),
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
    o_gp(this, "o_gp", "RISCV_ARCH"),
    o_tp(this, "o_tp", "RISCV_ARCH"),
    o_t0(this, "o_t0", "RISCV_ARCH"),
    o_t1(this, "o_t1", "RISCV_ARCH"),
    o_t2(this, "o_t2", "RISCV_ARCH"),
    o_fp(this, "o_fp", "RISCV_ARCH"),
    o_s1(this, "o_s1", "RISCV_ARCH"),
    o_a0(this, "o_a0", "RISCV_ARCH"),
    o_a1(this, "o_a1", "RISCV_ARCH"),
    o_a2(this, "o_a2", "RISCV_ARCH"),
    o_a3(this, "o_a3", "RISCV_ARCH"),
    o_a4(this, "o_a4", "RISCV_ARCH"),
    o_a5(this, "o_a5", "RISCV_ARCH"),
    o_a6(this, "o_a6", "RISCV_ARCH"),
    o_a7(this, "o_a7", "RISCV_ARCH"),
    o_s2(this, "o_s2", "RISCV_ARCH"),
    o_s3(this, "o_s3", "RISCV_ARCH"),
    o_s4(this, "o_s4", "RISCV_ARCH"),
    o_s5(this, "o_s5", "RISCV_ARCH"),
    o_s6(this, "o_s6", "RISCV_ARCH"),
    o_s7(this, "o_s7", "RISCV_ARCH"),
    o_s8(this, "o_s8", "RISCV_ARCH"),
    o_s9(this, "o_s9", "RISCV_ARCH"),
    o_s10(this, "o_s10", "RISCV_ARCH"),
    o_s11(this, "o_s11", "RISCV_ARCH"),
    o_t3(this, "o_t3", "RISCV_ARCH"),
    o_t4(this, "o_t4", "RISCV_ARCH"),
    o_t5(this, "o_t5", "RISCV_ARCH"),
    o_t6(this, "o_t6", "RISCV_ARCH"),
    // struct declaration
    RegValueTypeDef_(this, "RegValueType", NO_COMMENT),
    // registers
    arr(this, &i_clk, &i_nrst, "arr"),
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
    SETVAL(comb.next_tag, INC(ARRITEM(arr, comb.int_waddr, arr.tag)));
    IF (EQ(comb.next_tag, i_wtag));
        SETONE(comb.v_inordered);
    ENDIF();

TEXT();
    TEXT("Debug port has lower priority to avoid system hangup due the tags error");
    IF (AND3(NZ(i_wena), NZ(i_waddr), NZ(OR2(INV(i_inorder), comb.v_inordered))));
        SETARRITEM(arr, comb.int_waddr, arr.val, i_wdata);
        SETARRITEM(arr, comb.int_waddr, arr.tag, i_wtag);
    ELSIF (NZ(AND2(i_dport_ena, i_dport_write)));
        IF (NZ(i_dport_addr));
            SETARRITEM(arr, comb.int_daddr, arr.val, i_dport_wdata);
        ENDIF();
    ENDIF();

TEXT();
    SYNC_RESET();

TEXT();
    SETVAL(o_ignored, AND4(i_wena, OR_REDUCE(i_waddr), i_inorder, INV(comb.v_inordered)));
    SETVAL(o_rdata1, ARRITEM(arr, comb.int_radr1, arr.val));
    SETVAL(o_rtag1, ARRITEM(arr, comb.int_radr1, arr.tag));
    SETVAL(o_rdata2, ARRITEM(arr, comb.int_radr2, arr.val));
    SETVAL(o_rtag2, ARRITEM(arr, comb.int_radr2, arr.tag));
    SETVAL(o_dport_rdata, ARRITEM(arr, comb.int_daddr, arr.val));
    SETVAL(o_ra, ARRITEM(arr, cfg->REG_RA, arr.val));
    SETVAL(o_sp, ARRITEM(arr, cfg->REG_SP, arr.val));
    SETVAL(o_gp, ARRITEM(arr, cfg->REG_GP, arr.val));
    SETVAL(o_tp, ARRITEM(arr, cfg->REG_TP, arr.val));
    SETVAL(o_t0, ARRITEM(arr, cfg->REG_T0, arr.val));
    SETVAL(o_t1, ARRITEM(arr, cfg->REG_T1, arr.val));
    SETVAL(o_t2, ARRITEM(arr, cfg->REG_T2, arr.val));
    SETVAL(o_fp, ARRITEM(arr, cfg->REG_S0, arr.val));
    SETVAL(o_s1, ARRITEM(arr, cfg->REG_S1, arr.val));
    SETVAL(o_a0, ARRITEM(arr, cfg->REG_A0, arr.val));
    SETVAL(o_a1, ARRITEM(arr, cfg->REG_A1, arr.val));
    SETVAL(o_a2, ARRITEM(arr, cfg->REG_A2, arr.val));
    SETVAL(o_a3, ARRITEM(arr, cfg->REG_A3, arr.val));
    SETVAL(o_a4, ARRITEM(arr, cfg->REG_A4, arr.val));
    SETVAL(o_a5, ARRITEM(arr, cfg->REG_A5, arr.val));
    SETVAL(o_a6, ARRITEM(arr, cfg->REG_A6, arr.val));
    SETVAL(o_a7, ARRITEM(arr, cfg->REG_A7, arr.val));
    SETVAL(o_s2, ARRITEM(arr, cfg->REG_S2, arr.val));
    SETVAL(o_s3, ARRITEM(arr, cfg->REG_S3, arr.val));
    SETVAL(o_s4, ARRITEM(arr, cfg->REG_S4, arr.val));
    SETVAL(o_s5, ARRITEM(arr, cfg->REG_S5, arr.val));
    SETVAL(o_s6, ARRITEM(arr, cfg->REG_S6, arr.val));
    SETVAL(o_s7, ARRITEM(arr, cfg->REG_S7, arr.val));
    SETVAL(o_s8, ARRITEM(arr, cfg->REG_S8, arr.val));
    SETVAL(o_s9, ARRITEM(arr, cfg->REG_S9, arr.val));
    SETVAL(o_s10, ARRITEM(arr, cfg->REG_S10, arr.val));
    SETVAL(o_s11, ARRITEM(arr, cfg->REG_S11, arr.val));
    SETVAL(o_t3, ARRITEM(arr, cfg->REG_T3, arr.val));
    SETVAL(o_t4, ARRITEM(arr, cfg->REG_T4, arr.val));
    SETVAL(o_t5, ARRITEM(arr, cfg->REG_T5, arr.val));
    SETVAL(o_t6, ARRITEM(arr, cfg->REG_T6, arr.val));

}

