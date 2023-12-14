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

#include "ram_bytes_tech.h"

ram_bytes_tech::ram_bytes_tech(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "ram_bytes_tech", name, comment),
    abits(this, "abits", "16"),
    log2_dbytes(this, "log2_dbytes", "3"),
    dbytes(this, "dbytes", "POW2(1,log2_dbytes)"),
    dbits(this, "dbits", "MUL(8,POW2(1,log2_dbytes))"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_addr(this, "i_addr", "abits"),
    i_wena(this, "i_wena", "1"),
    i_wstrb(this, "i_wstrb", "POW2(1,log2_dbytes)"),
    i_wdata(this, "i_wdata", "MUL(8,POW2(1,log2_dbytes))"),
    o_rdata(this, "o_rdata", "MUL(8,POW2(1,log2_dbytes))"),
    // signals
    wb_addr(this, "wb_addr", "SUB(abits,log2_dbytes)"),
    wb_wena(this, "wb_wena", "1", "dbytes", NO_COMMENT),
    wb_wdata(this, "wb_wdata", "8", "dbytes", NO_COMMENT),
    wb_rdata(this, "wb_rdata", "8", "dbytes", NO_COMMENT),
    // process
    comb(this),
    mem(this, "mem", "dbytes", NO_COMMENT)
{
    Operation::start(this);

    mem.abits.setObjValue(&SUB2(abits, log2_dbytes));
    mem.dbits.setObjValue(&CONST("8"));
    GenObject &i = FORGEN("i", CONST("0"), dbytes, "++", new StringConst("memgen"));
        NEW(mem, mem.getName().c_str(), &i);
            CONNECT(mem, &i, mem.i_clk, i_clk);
            CONNECT(mem, &i, mem.i_addr, wb_addr);
            CONNECT(mem, &i, mem.i_wena, ARRITEM(wb_wena, i, wb_wena));
            CONNECT(mem, &i, mem.i_wdata, ARRITEM(wb_wdata, i, wb_wdata));
            CONNECT(mem, &i, mem.o_rdata, ARRITEM(wb_rdata, i, wb_rdata));
        ENDNEW();
    ENDFORGEN(new StringConst("memgen"));


    Operation::start(&comb);
    proc_comb();
}

void ram_bytes_tech::proc_comb() {
    SETVAL(wb_addr, BITS(i_addr, DEC(abits), log2_dbytes));
    GenObject &i = FOR("i", CONST("0"), dbytes, "++");
        SETARRITEM(wb_wena, i, wb_wena, AND2(i_wena, ARRITEM(i_wstrb, i, i_wstrb)));
        SETARRITEM(wb_wdata, i, wb_wdata, BIG_TO_U64(BITSW(i_wdata, MUL2(CONST("8"), i), CONST("8"))));
        SETBITSW(comb.vb_rdata, MUL2(CONST("8"), i), CONST("8"), ARRITEM(wb_rdata, i, wb_rdata));
    ENDFOR();

TEXT();
    SETVAL(o_rdata, comb.vb_rdata);
}

