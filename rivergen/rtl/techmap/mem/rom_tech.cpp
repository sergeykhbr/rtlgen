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

#include "rom_tech.h"

rom_tech::rom_tech(GenObject *parent, const char *name, const char *gen_abits, const char *log2_dbytes) :
    ModuleObject(parent, "rom_tech", name),
    abits(this, "abits", gen_abits),
    log2_dbytes(this, "log2_dbytes", log2_dbytes),
    dbits(this, "dbits", "MUL(8,POW2(1,log2_dbytes))"),
    filename(this, "filename", ""),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_addr(this, "i_addr", "abits"),
    o_rdata(this, "o_rdata", "MUL(8,POW2(1,log2_dbytes))"),
    // signals
    wb_addr(this, "wb_addr", "SUB(abits,log2_dbytes)"),
    // process
    comb(this),
    inf0(this, "inf0", "abits")
{
    Operation::start(this);

    TEXT("if (dbits != 64) begin");
    TEXT("    TODO: GENERATE assert");
    TEXT("else");

    inf0.changeTmplParameter("abits", "SUB(abits,log2_dbytes)");
    NEW(inf0, inf0.getName().c_str());
        CONNECT(inf0, 0, inf0.i_clk, i_clk);
        CONNECT(inf0, 0, inf0.i_addr, wb_addr);
        CONNECT(inf0, 0, inf0.o_rdata, o_rdata);
    ENDNEW();

    TEXT("endif");

    Operation::start(&comb);
    proc_comb();
}

void rom_tech::proc_comb() {
    SETVAL(wb_addr, BITS(i_addr, DEC(abits), log2_dbytes));
}

