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

#include "ram_cache_bwe_tech.h"

ram_cache_bwe_tech::ram_cache_bwe_tech(GenObject *parent, const char *name, const char *gen_abits, const char *gen_dbits) :
    ModuleObject(parent, "ram_cache_bwe_tech", name),
    abits(this, "abits", gen_abits),
    dbits(this, "dbits", gen_dbits),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_addr(this, "i_addr", &abits),
    i_wena(this, "i_wena", "DIV(dbits,8)"),
    i_wdata(this, "i_wdata", &dbits),
    o_rdata(this, "o_rdata", &dbits),
    // signals
    wb_we(this, "wb_we", "1", "DIV(dbits,8)"),
    wb_wdata(this, "wb_wdata", "8", "DIV(dbits,8)"),
    wb_rdata(this, "wb_rdata", "8", "DIV(dbits,8)"),
    // process
    comb(this),
    rx(this, "rx", "DIV(dbits,8)")
{
    Operation::start(this);
    rx.disableReset();
    disableVcd();

    // Create and connet Sub-modules:
    rx.changeTmplParameter("abits", "abits");
    GenObject &i = FORGEN ("i", CONST("0"), CONST("DIV(dbits,8)"), "++", new STRING("rxgen"));
        NEW(*rx.arr_[0], rx.getName().c_str(), &i);
            CONNECT(rx, &i, rx->i_clk, i_clk);
            CONNECT(rx, &i, rx->i_addr, i_addr);
            CONNECT(rx, &i, rx->i_wena, ARRITEM(wb_we, i, wb_we));
            CONNECT(rx, &i, rx->i_wdata, ARRITEM(wb_wdata, i, wb_wdata));
            CONNECT(rx, &i, rx->o_rdata, ARRITEM(wb_rdata, i, wb_rdata));
        ENDNEW();
    ENDFORGEN(new STRING("rxgen"));

    Operation::start(&comb);
    proc_comb();
}

void ram_cache_bwe_tech::proc_comb() {
    GenObject &i = FOR ("i", CONST("0"), CONST("DIV(dbits,8)"), "++");
        SETARRITEM(wb_we, i, wb_we, BIT(i_wena, i));
        SETARRITEM(wb_wdata, i, wb_wdata, BIG_TO_U64(BITSW(i_wdata, MUL2(CONST("8"), i), CONST("8"))));
        SETBITSW(o_rdata, MUL2(CONST("8"), i), CONST("8"), ARRITEM(wb_rdata, i, wb_rdata));
    ENDFOR();
}

