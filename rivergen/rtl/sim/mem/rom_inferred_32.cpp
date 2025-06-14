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

#include "rom_inferred_32.h"

rom_inferred_32::rom_inferred_32(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "rom_inferred_32", name, comment),
    abits(this, "abits", "6", NO_COMMENT),
    filename(this, "filename", ""),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_addr(this, "i_addr", "abits"),
    o_rdata(this, "o_rdata", "32"),
    DEPTH(this, "DEPTH", "POW2(1,abits)"),
    mem(this, "mem", "32", "DEPTH", NO_COMMENT),
    // process
    rproc(this, &i_clk)
{
    Operation::start(this);

    INITIAL();
        READMEMH(filename, mem);
    ENDINITIAL();

    Operation::start(&rproc);
    rxegisters();
}

void rom_inferred_32::rxegisters() {
    SETVAL(o_rdata, ARRITEM(mem, TO_INT(i_addr), mem));
}

