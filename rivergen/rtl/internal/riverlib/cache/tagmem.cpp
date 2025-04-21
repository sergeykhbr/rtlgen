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

#include "tagmem.h"

TagMem::TagMem(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "TagMem", name, comment),
    abus(this, "abus", "64", "system bus address width (64 or 32 bits)"),
    ibits(this, "ibits", "6", "lines memory address width (usually 6..8)"),
    lnbits(this, "lnbits", "5", "One line bits: log2(bytes_per_line)"),
    flbits(this, "flbits", "4", "total flags number saved with address tag"),
    snoop(this, "snoop", "0", "0 Snoop port disabled; 1 Enabled (L2 caching)"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_addr(this, "i_addr", "abus"),
    i_wstrb(this, "i_wstrb", "POW2(1,lnbits)"),
    i_wdata(this, "i_wdata", "MUL(8,POW2(1,lnbits))"),
    i_wflags(this, "i_wflags", "flbits"),
    o_raddr(this, "o_raddr", "abus"),
    o_rdata(this, "o_rdata", "MUL(8,POW2(1,lnbits))"),
    o_rflags(this, "o_rflags", "flbits"),
    o_hit(this, "o_hit", "1"),
    _snoop0_(this, "L2 snoop port, active when snoop = 1"),
    i_snoop_addr(this, "i_snoop_addr", "abus"),
    o_snoop_flags(this, "o_snoop_flags", "flbits"),
    TAG_BITS(this, "TAG_BITS", "SUB(SUB(abus,ibits),lnbits))"),
    TAG_WITH_FLAGS(this, "TAG_WITH_FLAGS", "ADD(TAG_BITS,flbits)"),
    wb_index(this, "wb_index", "ibits"),
    wb_tago_rdata(this, "wb_tago_rdata", "TAG_WITH_FLAGS"),
    wb_tagi_wdata(this, "wb_tagi_wdata", "TAG_WITH_FLAGS"),
    w_tagi_we(this, "w_tagi_we", "1"),
    wb_snoop_index(this, "wb_snoop_index", "ibits"),
    wb_snoop_tagaddr(this, "wb_snoop_tagaddr", "TAG_BITS"),
    wb_tago_snoop_rdata(this, "wb_tago_snoop_rdata", "TAG_WITH_FLAGS"),
    tagaddr(this, "tagaddr", "TAG_BITS", "0"),
    index(this, "index", "ibits", "0"),
    snoop_tagaddr(this, "snoop_tagaddr", "TAG_BITS", "0"),
    // process
    comb(this),
    data0(this, "data0", NO_COMMENT),
    tag0(this, "tag0", NO_COMMENT),
    tagsnoop0(this, "tagsnoop0", NO_COMMENT)
{
    Operation::start(this);

    // Create and connet Sub-modules:
    TEXT("bwe = byte write enable");
    data0.abits.setObjValue(&ibits);
    data0.dbits.setObjValue(&CONST("MUL(8,POW2(1,lnbits))"));
    NEW(data0, data0.getName().c_str());
        CONNECT(data0, 0, data0.i_clk, i_clk);
        CONNECT(data0, 0, data0.i_addr, wb_index);
        CONNECT(data0, 0, data0.i_wena, i_wstrb);
        CONNECT(data0, 0, data0.i_wdata, i_wdata);
        CONNECT(data0, 0, data0.o_rdata, o_rdata);
    ENDNEW();

TEXT();
    tag0.abits.setObjValue(&ibits);
    tag0.dbits.setObjValue(&TAG_WITH_FLAGS);
    NEW(tag0, tag0.getName().c_str());
        CONNECT(tag0, 0, tag0.i_clk, i_clk);
        CONNECT(tag0, 0, tag0.i_addr, wb_index);
        CONNECT(tag0, 0, tag0.i_wena, w_tagi_we);
        CONNECT(tag0, 0, tag0.i_wdata, wb_tagi_wdata);
        CONNECT(tag0, 0, tag0.o_rdata, wb_tago_rdata);
    ENDNEW();

TEXT();
    GENERATE("snoop_gen");
        IFGEN (snoop, new StringConst("snoop_en"));
        tagsnoop0.abits.setObjValue(&ibits);
        tagsnoop0.dbits.setObjValue(&TAG_WITH_FLAGS);
            NEW(tagsnoop0, tagsnoop0.getName().c_str());
                CONNECT(tagsnoop0, 0, tagsnoop0.i_clk, i_clk);
                CONNECT(tagsnoop0, 0, tagsnoop0.i_addr, wb_snoop_index);
                CONNECT(tagsnoop0, 0, tagsnoop0.i_wena, w_tagi_we);
                CONNECT(tagsnoop0, 0, tagsnoop0.i_wdata, wb_tagi_wdata);
                CONNECT(tagsnoop0, 0, tagsnoop0.o_rdata, wb_tago_snoop_rdata);
            ENDNEW();
        ELSEGEN(new StringConst("snoop"));
            ASSIGNZERO(wb_tago_snoop_rdata);
        ENDIFGEN(new StringConst("snoop_dis"));
    ENDGENERATE("snoop_gen");

    Operation::start(&comb);
    proc_comb();
}

void TagMem::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;

TEXT();
    IF (EQ(tagaddr, BITS(wb_tago_rdata, DEC(TAG_BITS), CONST("0"))));
        SETVAL(comb.v_hit, BIT(wb_tago_rdata, TAG_BITS), "valid bit");
    ENDIF();

TEXT();
    SETBITS(comb.vb_raddr, DEC(abus), ADD2(ibits, lnbits), BITS(wb_tago_rdata, DEC(TAG_BITS), CONST("0")));
    SETBITS(comb.vb_raddr, DEC(ADD2(ibits, lnbits)), lnbits, index);

TEXT();
    SETVAL(comb.vb_index, BITS(i_addr, DEC(ADD2(ibits, lnbits)), lnbits));
    SETBITS(comb.vb_tagi_wdata, DEC(TAG_BITS), CONST("0"),
                        BITS(i_addr, DEC(abus), ADD2(ibits, lnbits)));
    SETBITS(comb.vb_tagi_wdata, DEC(TAG_WITH_FLAGS), TAG_BITS, i_wflags);

TEXT();
    IF (EQ(snoop, CONST("1")));
        SETVAL(comb.vb_snoop_flags, BITS(wb_tago_snoop_rdata, DEC(TAG_WITH_FLAGS), TAG_BITS));
        SETVAL(comb.vb_snoop_index, BITS(i_snoop_addr, DEC(ADD2(ibits, lnbits)), lnbits));
        SETVAL(comb.vb_snoop_tagaddr, BITS(i_snoop_addr, DEC(abus), ADD2(ibits, lnbits)));

        IF (NZ(i_wstrb));
            SETVAL(comb.vb_snoop_index, comb.vb_index);
        ENDIF();

        IF (NE(snoop_tagaddr, BITS(wb_tago_snoop_rdata, DEC(TAG_BITS), CONST("0"))));
            SETZERO(comb.vb_snoop_flags);
        ENDIF();
    ENDIF();

TEXT();
    SETVAL(tagaddr, BITS(comb.vb_tagi_wdata, DEC(TAG_BITS), CONST("0")));
    SETVAL(index, comb.vb_index);
    SETVAL(snoop_tagaddr, comb.vb_snoop_tagaddr);

TEXT();
    SYNC_RESET();

TEXT();
    SETVAL(wb_index, comb.vb_index);
    SETVAL(w_tagi_we, OR_REDUCE(i_wstrb));
    SETVAL(wb_tagi_wdata, comb.vb_tagi_wdata);

TEXT();
    SETVAL(o_raddr, comb.vb_raddr);
    SETVAL(o_rflags, BITS(wb_tago_rdata, DEC(TAG_WITH_FLAGS), TAG_BITS));
    SETVAL(o_hit, comb.v_hit);

TEXT();
    SETVAL(wb_snoop_index, comb.vb_snoop_index);
    SETVAL(wb_snoop_tagaddr, comb.vb_snoop_tagaddr);
    SETVAL(o_snoop_flags, comb.vb_snoop_flags);
}

