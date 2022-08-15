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

lrunway::lrunway(GenObject *parent,
                 const char *name,
                 const char *gen_abits,
                 const char *gen_waybits) :
    ModuleObject(parent, "lrunway", name),
    abits(this, "abits", gen_abits, "Cache line address bus (usually 6..8)"),
    waybits(this, "waybits", gen_waybits, "Number of way bitwidth (=2 for 4 ways cache)"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_init(this, "i_init", "1"),
    i_raddr(this, "i_raddr", &abits),
    i_waddr(this, "i_waddr", &abits),
    i_up(this, "i_up", "1"),
    i_down(this, "i_down", "1"),
    i_lru(this, "i_lru", &waybits),
    o_lru(this, "o_lru", &waybits),
    LINES_TOTAL(this, "LINES_TOTAL", "POW2(1,abits)"),
    WAYS_TOTAL(this, "WAYS_TOTAL", "POW2(1,waybits)"),
    LINE_WIDTH(this, "LINE_WIDTH", "MUL(WAYS_TOTAL,waybits)"),
    radr(this, "radr", "abits", "0"),
    tbl(this, "mem", "LINE_WIDTH", "LINES_TOTAL", true),
    // process
    comb(this)
{
    Operation::start(this);
    tbl.disableReset();
    disableVcd();

    Operation::start(&comb);
    proc_comb();
}

void lrunway::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;
    GenObject *i;

}

