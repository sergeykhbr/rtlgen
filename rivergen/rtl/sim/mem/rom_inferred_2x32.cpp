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
    wb_rdata0(this, "wb_rdata0", "32"),
    wb_rdata1(this, "wb_rdata1", "32"),
    rom0(this, "rom0", NO_COMMENT),
    rom1(this, "rom1", NO_COMMENT),
    // process
    proc_comb(this)
{
    Operation::start(this);
    /*INITIAL();
        DECLARE_TSTR();
        SETSTRF(hexname0, "%s_lo.hex", 1, &filename);
        READMEMH(hexname0, mem0);

        TEXT();
        SETSTRF(hexname1, "%s_hi.hex", 1, &filename);
        READMEMH(hexname1, mem1);
    ENDINITIAL();*/
    rom0.abits.setObjValue(&abits);
    rom0.filename.setObjValue(&STRCAT(filename, *new StringConst("_lo.hex")));
    NEW(rom0, rom0.getName().c_str());
        CONNECT(rom0, 0, rom0.i_clk, i_clk);
        CONNECT(rom0, 0, rom0.i_addr, i_addr);
        CONNECT(rom0, 0, rom0.o_rdata, wb_rdata0);
    ENDNEW();

    TEXT();
    rom1.abits.setObjValue(&abits);
    rom1.filename.setObjValue(&STRCAT(filename, *new StringConst("_hi.hex")));
    NEW(rom1, rom1.getName().c_str());
        CONNECT(rom1, 0, rom1.i_clk, i_clk);
        CONNECT(rom1, 0, rom1.i_addr, i_addr);
        CONNECT(rom1, 0, rom1.o_rdata, wb_rdata1);
    ENDNEW();

    Operation::start(&proc_comb);
    comb();
}

void rom_inferred_2x32::comb() {
    ASSIGN(o_rdata, CC2(wb_rdata1, wb_rdata0));
}

