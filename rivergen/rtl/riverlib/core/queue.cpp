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

#include "queue.h"

Queue::Queue(GenObject *parent, const char *name, const char *gen_abits, const char *gen_dbits) :
    ModuleObject(parent, "Queue", name),
    abits(this, "abits", gen_abits),
    dbits(this, "dbits", gen_dbits),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_re(this, "i_re", "1"),
    i_we(this, "i_we", "1"),
    i_wdata(this, "i_wdata", "dbits"),
    o_rdata(this, "o_rdata", "dbits"),
    o_full(this, "o_full", "1"),
    o_nempty(this, "o_nempty", "1"),
    DEPTH(this, "DEPTH", "POW2(1,abits)"),
    wcnt(this, "wcnt", "ADD(abits,1)", "0"),
    mem(this, "mem", "dbits", "DEPTH"),
    // process
    comb(this)
{
    mem.disableReset();
    disableVcd();
}

void Queue::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;
    GenObject *i;

    IF (EQ(wcnt, DEPTH));
        SETONE(comb.full);
    ENDIF();
    IF (GE(wcnt, DEC(DEPTH)));
        SETONE(comb.show_full);
    ENDIF();

TEXT();
    IF (AND2(NZ(i_re), NZ(i_we)));
        i = &FOR("i", CONST("1"), DEPTH, "++");
            SETARRITEM(mem, DEC(*i), mem, ARRITEM(mem, *i, mem));
        ENDFOR();
        IF (NZ(wcnt));
            SETARRITEM(mem, DEC(TO_INT(wcnt)), mem, i_wdata);
        ELSE();
            TEXT("do nothing, it will directly pass to output");
        ENDIF();
    ELSIF (AND2(EZ(i_re), NZ(i_we)));
        IF (EZ(comb.full));
            SETVAL(wcnt, INC(wcnt));
            SETARRITEM(mem, TO_INT(wcnt), mem, i_wdata);
        ENDIF();
    ELSIF (AND2(NZ(i_re), EZ(i_we)));
        IF (NZ(wcnt));
            SETVAL(wcnt, DEC(wcnt));
        ENDIF();
        i = &FOR("i", CONST("1"), DEPTH, "++");
            SETARRITEM(mem, DEC(*i), mem, ARRITEM(mem, *i, mem));
        ENDFOR();
    ENDIF();

TEXT();
    IF (EZ(wcnt));
        SETVAL(comb.vb_data_o, i_wdata);
    ELSE();
        SETVAL(comb.vb_data_o, ARRITEM(mem, CONST("0"), mem));
    ENDIF();

TEXT();
    IF (OR2(NZ(i_we), NZ(wcnt)));
        SETONE(comb.nempty);
    ENDIF();

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_nempty, comb.nempty);
    SETVAL(o_full, comb.show_full);
    SETVAL(o_rdata, comb.vb_data_o);
}

