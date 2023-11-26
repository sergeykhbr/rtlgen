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

#include "ram_tech.h"

ram_tech::ram_tech(GenObject *parent, const char *name, const char *depth,
                    const char *gen_abits, const char *gen_dbits) :
    ModuleObject(parent, "ram_tech", name, depth),
    abits(this, "abits", gen_abits),
    dbits(this, "dbits", gen_dbits),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_addr(this, "i_addr", &abits),
    i_wena(this, "i_wena", "1"),
    i_wdata(this, "i_wdata", &dbits),
    o_rdata(this, "o_rdata", &dbits),
    DEPTH(this, "DEPTH", "POW2(1,abits)"),
    rdata(this, "rdata", "dbits"),
    mem(this, "mem", "dbits", "DEPTH"),
    // process
    rproc(this)
{
    Operation::start(this);
    mem.disableReset();
    disableVcd();

    Operation::start(&rproc);
    registers();
}

void ram_tech::registers() {
    IF (NZ(i_wena));
        SETARRITEM(mem, TO_INT(i_addr), mem, i_wdata);
    ENDIF();
    SETVAL(rdata, ARRITEM(mem, TO_INT(i_addr), mem));

TEXT();
    SETVAL(o_rdata, rdata);
}

