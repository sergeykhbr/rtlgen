// 
//  Copyright 2024 Sergey Khabarov, sergeykhbr@gmail.com
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

#include "cdc_afifo.h"

cdc_afifo::cdc_afifo(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "cdc_afifo", name, comment),
    abits(this, "abits", "3", "fifo log2(depth)"),
    dbits(this, "dbits", "65", "payload width"),
    i_nrst(this, "i_nrst", "1", "reset active LOW"),
    i_wclk(this, "i_wclk", "1", "clock write"),
    i_wr(this, "i_wr", "1", "write enable strob"),
    i_wdata(this, "i_wdata", "dbits", "write data"),
    o_wfull(this, "o_wfull", "1", "fifo is full in wclk domain"),
    i_rclk(this, "i_rclk", "1", "read clock"),
    i_rd(this, "i_rd", "1", "read enable strob"),
    o_rdata(this, "o_rdata", "dbits", "fifo payload read"),
    o_rempty(this, "o_rempty", "1", "fifo is empty it rclk domain"),
    w_wr_ena(this, "w_wr_ena", "1"),
    wb_wgray_addr(this, "wb_wgray_addr", "abits"),
    wb_wgray(this, "wgray", "ADD(abits,1)"),
    q1_wgray(this, "q1_wgray", "ADD(abits,1)", "'0", NO_COMMENT),
    q2_wgray(this, "q2_wgray", "ADD(abits,1)", "'0", NO_COMMENT),
    w_wgray_full(this, "w_wgray_full", "1"),
    w_wgray_empty_unused(this, "w_wgray_empty_unused", "1"),
    w_rd_ena(this, "w_rd_ena", "1"),
    wb_rgray_addr(this, "wb_rgray_addr", "abits"),
    wb_rgray(this, "rgray", "ADD(abits,1)"),
    q1_rgray(this, "q1_rgray", "ADD(abits,1)", "'0", NO_COMMENT),
    q2_rgray(this, "q2_rgray", "ADD(abits,1)", "'0", NO_COMMENT),
    w_rgray_full_unused(this, "w_rgray_full_unused", "1"),
    w_rgray_empty(this, "w_rgray_empty", "1"),
    // sub-modules
    mem0(this, "mem0", NO_COMMENT),
    wgray0(this, "wgray0", NO_COMMENT),
    rgray0(this, "rgray0", NO_COMMENT),
    // process
    comb(this, "comb", 0, CLK_ALWAYS, 0, ACTIVE_NONE, NO_COMMENT),
    wff(this, "proc_wff", &i_wclk, CLK_POSEDGE, &i_nrst, ACTIVE_LOW, NO_COMMENT),
    rff(this, "proc_rff", &i_rclk, CLK_POSEDGE, &i_nrst, ACTIVE_LOW, NO_COMMENT)
{
    Operation::start(this);

    wgray0.abits.setObjValue(&abits);
    NEW(wgray0, wgray0.getName().c_str());
        CONNECT(wgray0, 0, wgray0.i_nrst, i_nrst);
        CONNECT(wgray0, 0, wgray0.i_clk, i_wclk);
        CONNECT(wgray0, 0, wgray0.i_ena, w_wr_ena);
        CONNECT(wgray0, 0, wgray0.i_q2_gray, q2_rgray);
        CONNECT(wgray0, 0, wgray0.o_addr, wb_wgray_addr);
        CONNECT(wgray0, 0, wgray0.o_gray, wb_wgray);
        CONNECT(wgray0, 0, wgray0.o_empty, w_wgray_empty_unused);
        CONNECT(wgray0, 0, wgray0.o_full, w_wgray_full);
    ENDNEW();

TEXT();
    rgray0.abits.setObjValue(&abits);
    NEW(rgray0, rgray0.getName().c_str());
        CONNECT(rgray0, 0, rgray0.i_nrst, i_nrst);
        CONNECT(rgray0, 0, rgray0.i_clk, i_rclk);
        CONNECT(rgray0, 0, rgray0.i_ena, w_rd_ena);
        CONNECT(rgray0, 0, rgray0.i_q2_gray, q2_wgray);
        CONNECT(rgray0, 0, rgray0.o_addr, wb_rgray_addr);
        CONNECT(rgray0, 0, rgray0.o_gray, wb_rgray);
        CONNECT(rgray0, 0, rgray0.o_empty, w_rgray_empty);
        CONNECT(rgray0, 0, rgray0.o_full, w_rgray_full_unused);
    ENDNEW();

TEXT();
    mem0.abits.setObjValue(&abits);
    mem0.dbits.setObjValue(&dbits);
    NEW(mem0, mem0.getName().c_str());
        CONNECT(mem0, 0, mem0.i_wclk, i_wclk);
        CONNECT(mem0, 0, mem0.i_wena, w_wr_ena);
        CONNECT(mem0, 0, mem0.i_waddr, wb_wgray_addr);
        CONNECT(mem0, 0, mem0.i_wdata, i_wdata);
        CONNECT(mem0, 0, mem0.i_rclk, i_rclk);
        CONNECT(mem0, 0, mem0.i_raddr, wb_rgray_addr);
        CONNECT(mem0, 0, mem0.o_rdata, o_rdata);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();

    Operation::start(&wff);
    proc_wff();

    Operation::start(&rff);
    proc_rff();
}

void cdc_afifo::proc_comb() {
    ASSIGN(w_wr_ena, AND2_L(i_wr, INV_L(w_wgray_full)));
    ASSIGN(w_rd_ena, AND2_L(i_rd, INV_L(w_rgray_empty)));
    ASSIGN(o_wfull, w_wgray_full);
    ASSIGN(o_rempty, w_rgray_empty);
}

void cdc_afifo::proc_wff() {
    IF (EZ(i_nrst));
        SETVAL_NB(q1_wgray, CONST("'0"));
        SETVAL_NB(q2_wgray, CONST("'0"));
    ELSE();
        SETVAL_NB(q1_wgray, wb_wgray);
        SETVAL_NB(q2_wgray, q1_wgray);
    ENDIF();
}

void cdc_afifo::proc_rff() {
    IF (EZ(i_nrst));
        SETVAL_NB(q1_rgray, CONST("'0"));
        SETVAL_NB(q2_rgray, CONST("'0"));
    ELSE();
        SETVAL_NB(q1_rgray, wb_rgray);
        SETVAL_NB(q2_rgray, q1_rgray);
    ENDIF();
}
