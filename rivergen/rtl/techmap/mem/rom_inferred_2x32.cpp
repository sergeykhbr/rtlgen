// 
//  Copyright 2023 Sergey Khabarov, sergeykhbr@gmail.com
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

#include "rom_inferred_2x32.h"

rom_inferred_2x32::rom_inferred_2x32(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "rom_inferred_2x32", name, comment),
    abits(this, "abits", "6", NO_COMMENT),
    filename(this, "filename", ""),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_addr(this, "i_addr", "abits"),
    o_rdata(this, "o_rdata", "64"),
    DEPTH(this, "DEPTH", "POW2(1,abits)"),
    hexname0(this, "hexname0", "", NO_COMMENT),
    hexname1(this, "hexname1", "", NO_COMMENT),
    wb_rdata0(this, "wb_rdata0", "32"),
    wb_rdata1(this, "wb_rdata1", "32"),
    //mem(this, &i_clk, CLK_POSEDGE, 0, ACTIVE_NONE, "mem", "dbits", "DEPTH", RSTVAL_NONE, NO_COMMENT),
    mem0(this, "mem0", "32", "DEPTH", NO_COMMENT),
    mem1(this, "mem1", "32", "DEPTH", NO_COMMENT),
    // process
    rproc(this)
{
    Operation::start(this);
    INITIAL();
        DECLARE_TSTR();
        SETSTRF(hexname0, "%s_lo.hex", 1, &filename);
        READMEMH(hexname0, mem0);

        TEXT();
        SETSTRF(hexname1, "%s_hi.hex", 1, &filename);
        READMEMH(hexname1, mem1);
    ENDINITIAL();

    Operation::start(&rproc);
    rxegisters();
}

void rom_inferred_2x32::rxegisters() {
    SETVAL(wb_rdata0, ARRITEM(mem0, TO_INT(i_addr), mem0));
    SETVAL(wb_rdata1, ARRITEM(mem1, TO_INT(i_addr), mem1));

TEXT();
    SETVAL(o_rdata, CC2(wb_rdata1, wb_rdata0));
}

