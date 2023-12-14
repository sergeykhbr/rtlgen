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

#include "lrunway.h"

lrunway::lrunway(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "lrunway", name, comment),
    abits(this, "abits", "6", "Cache line address bus (usually 6..8)"),
    waybits(this, "waybits", "2", "Number of way bitwidth (=2 for 4 ways cache)"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_init(this, "i_init", "1"),
    i_raddr(this, "i_raddr", "abits"),
    i_waddr(this, "i_waddr", "abits"),
    i_up(this, "i_up", "1"),
    i_down(this, "i_down", "1"),
    i_lru(this, "i_lru", "waybits"),
    o_lru(this, "o_lru", "waybits"),
    LINES_TOTAL(this, "LINES_TOTAL", "POW2(1,abits)"),
    WAYS_TOTAL(this, "WAYS_TOTAL", "POW2(1,waybits)"),
    LINE_WIDTH(this, "LINE_WIDTH", "MUL(WAYS_TOTAL,waybits)"),
    radr(this, "radr", "abits", "0"),
    mem(this, "mem", "LINE_WIDTH", "LINES_TOTAL", NO_COMMENT),
    // process
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void lrunway::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;
    GenObject *i;

    SETVAL(radr, i_raddr);
    SETVAL(comb.vb_tbl_rdata, ARRITEM(mem, TO_INT(radr), mem));

TEXT();
    SETVAL(comb.v_we, OR3(i_up, i_down, i_init));

TEXT();
    TEXT("init table value");
    i = &FOR ("i", CONST("0"), WAYS_TOTAL, "++");
        SETBITSW(comb.vb_tbl_wdata_init, MUL2(*i, waybits), waybits, *i);
    ENDFOR();

TEXT();
    TEXT("LRU next value, last used goes on top");
    SETVAL(comb.vb_tbl_wdata_up, comb.vb_tbl_rdata);
    IF (NE(BITSW(comb.vb_tbl_rdata, SUB2(LINE_WIDTH, waybits), waybits), i_lru));
        SETBITSW(comb.vb_tbl_wdata_up, SUB2(LINE_WIDTH, waybits), waybits, i_lru);
        SETONE(comb.shift_ena_up);

TEXT();
        i= &FOR ("i", SUB2(WAYS_TOTAL, CONST("2")), CONST("0"), "--");
            IF (NZ(comb.shift_ena_up));
                SETBITSW(comb.vb_tbl_wdata_up, MUL2(*i, waybits), waybits,
                        BITSW(comb.vb_tbl_rdata, MUL2(INC(*i), waybits), waybits));
                IF (EQ(BITSW(comb.vb_tbl_rdata, MUL2(*i, waybits), waybits), i_lru));
                    SETZERO(comb.shift_ena_up);
                ENDIF();
            ENDIF();
        ENDFOR();
    ENDIF();

TEXT();
    TEXT("LRU next value when invalidate, marked as 'invalid' goes down");
    SETVAL(comb.vb_tbl_wdata_down, comb.vb_tbl_rdata);
    IF (NE(BITS(comb.vb_tbl_rdata, DEC(waybits), CONST("0")), i_lru));
        SETBITS(comb.vb_tbl_wdata_down, DEC(waybits), CONST("0"), i_lru);
        SETONE(comb.shift_ena_down);

        TEXT();
        i = &FOR ("i", CONST("1"), WAYS_TOTAL, "++");
            IF (NZ(comb.shift_ena_down));
                SETBITSW(comb.vb_tbl_wdata_down, MUL2(*i, waybits), waybits,
                        BITSW(comb.vb_tbl_rdata, MUL2(DEC(*i), waybits), waybits));
                IF (EQ(BITSW(comb.vb_tbl_rdata, MUL2(*i, waybits), waybits), i_lru));
                    SETZERO(comb.shift_ena_down);
                ENDIF();
            ENDIF();
        ENDFOR();
    ENDIF();

TEXT();
    IF (NZ(i_init));
        SETVAL(comb.vb_tbl_wdata, comb.vb_tbl_wdata_init);
    ELSIF (NZ(i_up));
        SETVAL(comb.vb_tbl_wdata, comb.vb_tbl_wdata_up);
    ELSIF (NZ(i_down));
        SETVAL(comb.vb_tbl_wdata, comb.vb_tbl_wdata_down);
    ELSE();
        SETZERO(comb.vb_tbl_wdata);
    ENDIF();

TEXT();
    IF (NZ(comb.v_we));
        SETARRITEM(mem, TO_INT(i_waddr), mem, comb.vb_tbl_wdata);
    ENDIF();

    SETVAL(o_lru, BITS(comb.vb_tbl_rdata, DEC(waybits), CONST("0")));
}

