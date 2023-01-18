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

#include "sfifo.h"

sfifo::sfifo(GenObject *parent, const char *name) :
    ModuleObject(parent, "sfifo", name),
    dbits(this, "dbits", "8", "Data width bits"),
    log2_depth(this, "log2_depth", "4", "Fifo depth"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_we(this, "i_we", "1"),
    i_wdata(this, "i_wdata", "dbits"),
    i_re(this, "i_re", "1"),
    o_rdata(this, "o_rdata", "dbits"),
    o_count(this, "o_count", "ADD(log2_depth,1)", "Number of words in FIFO"),
    // params
    DEPTH(this, "DEPTH", "POW2(1,log2_depth)"),
    // signals
    // registers
    databuf(this, "databuf", "dbits", "DEPTH", true),
    wr_cnt(this, "wr_cnt", "log2_depth"),
    rd_cnt(this, "rd_cnt", "log2_depth"),
    total_cnt(this, "total_cnt", "ADD(log2_depth,1)"),
    //
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void sfifo::proc_comb() {
TEXT();
    TEXT("Check FIFO counter:");
    TEXT();
    IF (EZ(total_cnt));
        SETONE(comb.v_empty);
    ENDIF();

    TEXT();
    SETVAL(comb.v_full, BIT(total_cnt, log2_depth));


TEXT();
    IF (AND2(NZ(i_we), OR2(EZ(comb.v_full), NZ(i_re))));
        SETVAL(wr_cnt, INC(wr_cnt));
        SETARRITEM(databuf, TO_INT(wr_cnt), databuf, i_wdata);
        IF (EZ(i_re));
            SETVAL(total_cnt, INC(total_cnt));
        ENDIF();
    ENDIF();

TEXT();
    IF (AND2(NZ(i_re), EZ(comb.v_empty)));
        SETVAL(rd_cnt, INC(rd_cnt));
        IF (EZ(i_we));
            SETVAL(total_cnt, DEC(total_cnt));
        ENDIF();
    ENDIF();

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_rdata, ARRITEM(databuf, TO_INT(rd_cnt), databuf));
    SETVAL(o_count, total_cnt);
}
