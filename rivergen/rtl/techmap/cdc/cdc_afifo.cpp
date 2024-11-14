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
    dbits(this, "dbits", "32", "payload width"),
    i_wclk(this, "i_wclk", "1", "clock write"),
    i_wrstn(this, "i_wrstn", "1", "write reset active LOW"),
    i_wr(this, "i_wr", "1", "write enable strob"),
    i_wdata(this, "i_wdata", "dbits", "write data"),
    o_wfull(this, "o_wfull", "1", "fifo is full in wclk domain"),
    i_rclk(this, "i_rclk", "1", "read clock"),
    i_rrstn(this, "i_rrstn", "1", "read reset active LOW"),
    i_rd(this, "i_rd", "1", "read enable strob"),
    o_rdata(this, "o_rdata", "dbits", "fifo payload read"),
    o_rempty(this, "o_rempty", "1", "fifo is empty it rclk domain"),
    DEPTH(this, "DEPTH", "POW2(1,abits)"),
    wgray(this, &i_wclk, CLK_POSEDGE, &i_wrstn, ACTIVE_LOW, "wgray", "ADD(abits,1)", RSTVAL_ZERO, NO_COMMENT),
    wbin(this, &i_wclk, CLK_POSEDGE, &i_wrstn, ACTIVE_LOW, "wbin", "ADD(abits,1)", RSTVAL_ZERO, NO_COMMENT),
    wq2_rgray(this, &i_wclk, CLK_POSEDGE, &i_wrstn, ACTIVE_LOW, "wq2_rgray", "ADD(abits,1)", RSTVAL_ZERO, NO_COMMENT),
    wq1_rgray(this, &i_wclk, CLK_POSEDGE, &i_wrstn, ACTIVE_LOW, "wq1_rgray", "ADD(abits,1)", RSTVAL_ZERO, NO_COMMENT),
    wfull(this, &i_wclk, CLK_POSEDGE, &i_wrstn, ACTIVE_LOW, "wfull", "1", RSTVAL_ZERO, NO_COMMENT),
    rgray(this, &i_rclk, CLK_POSEDGE, &i_rrstn, ACTIVE_LOW, "rgray", "ADD(abits,1)", RSTVAL_ZERO, NO_COMMENT),
    rbin(this, &i_rclk, CLK_POSEDGE, &i_rrstn, ACTIVE_LOW, "rbin", "ADD(abits,1)", RSTVAL_ZERO, NO_COMMENT),
    rq2_wgray(this, &i_rclk, CLK_POSEDGE, &i_rrstn, ACTIVE_LOW, "rq2_wgray", "ADD(abits,1)", RSTVAL_ZERO, NO_COMMENT),
    rq1_wgray(this, &i_rclk, CLK_POSEDGE, &i_rrstn, ACTIVE_LOW, "rq1_wgray", "ADD(abits,1)", RSTVAL_ZERO, NO_COMMENT),
    rempty(this, &i_rclk, CLK_POSEDGE, &i_rrstn, ACTIVE_LOW, "rempty", "1", "1", NO_COMMENT),
    mem(this, &i_wclk, CLK_POSEDGE, 0, ACTIVE_NONE, "mem", "dbits", "DEPTH", RSTVAL_NONE, NO_COMMENT),
    // process
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void cdc_afifo::proc_comb() {
    TEXT("Cross the Gray pointer to write clock domain:");
    SETVAL(wq1_rgray, rgray);
    SETVAL(wq2_rgray, wq1_rgray);

TEXT();
    TEXT("Next write address and Gray write pointer");
    SETVAL(comb.vb_wbinnext, ADD2(wbin, CC2(CONST("0", "abits"), AND2(i_wr, INV(wfull)))));
    SETVAL(comb.vb_wgraynext, XOR2(RSH(comb.vb_wbinnext, CONST("1")), comb.vb_wbinnext));
    SETVAL(comb.vb_waddr, BITS(wbin, DEC(abits), CONST("0")));
    SETVAL(wgray, comb.vb_wgraynext);
    SETVAL(wbin, comb.vb_wbinnext);

TEXT();
    IF(EQ(comb.vb_wgraynext, CC2(INV_L(BITS(wq2_rgray, abits, DEC(abits))),
                                 BITS(wq2_rgray, SUB2(abits, CONST("2")), CONST("0")))));
        SETONE(comb.v_wfull_next);
    ENDIF();
    SETVAL(wfull, comb.v_wfull_next);

TEXT();
    IF (NZ(AND2(i_wr, INV(wfull))));
        SETARRITEM(mem, TO_INT(comb.vb_waddr), mem, i_wdata);
    ENDIF();

TEXT();
    TEXT("Write Gray pointer into read clock domain");
    SETVAL(rq1_wgray, wgray);
    SETVAL(rq2_wgray, rq1_wgray);
    SETVAL(comb.vb_rbinnext, ADD2(rbin, CC2(CONST("0", "abits"), AND2(i_rd, INV(rempty)))));
    SETVAL(comb.vb_rgraynext, XOR2(RSH(comb.vb_rbinnext, CONST("1")), comb.vb_rbinnext));
    SETVAL(rgray, comb.vb_rgraynext);
    SETVAL(rbin, comb.vb_rbinnext);
    SETVAL(comb.vb_raddr, BITS(rbin, DEC(abits), CONST("0")));

TEXT();
    IF (EQ(comb.vb_rgraynext, rq2_wgray));
        SETONE(comb.v_rempty_next);
    ENDIF();
    SETVAL(rempty, comb.v_rempty_next);

TEXT();
    SETVAL(o_wfull, wfull);
    SETVAL(o_rempty, rempty);
    SETVAL(o_rdata, ARRITEM(mem, TO_INT(comb.vb_raddr), mem));
}

