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

#include "tagmemnway.h"

TagMemNWay::TagMemNWay(GenObject *parent,
               const char *name,
               const char *gen_abus, 
               const char *gen_ibits, 
               const char *gen_lnbits, 
               const char *gen_flbits,
               const char *gen_snoop) :
    ModuleObject(parent, "TagMemNWay", name),
    abus(this, "abus", gen_abus, "system bus address width (64 or 32 bits)"),
    ibits(this, "ibits", gen_ibits, "lines memory address width (usually 6..8)"),
    lnbits(this, "lnbits", gen_lnbits, "One line bits: log2(bytes_per_line)"),
    flbits(this, "flbits", gen_flbits, "total flags number saved with address tag"),
    snoop(this, "snoop", gen_snoop, "0 Snoop port disabled; 1 Enabled (L2 caching)"),
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
    wb_datao_rdata(this, "wb_datao_rdata", "8", "POW2(1,lnbits)"),
    wb_datai_wdata(this, "wb_datai_wdata", "8", "POW2(1,lnbits)"),
    w_datai_we(this, "w_datai_we", "8", "POW2(1,lnbits)"),
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
    datax(this, "datax", "POW2(1,lnbits)"),
    tag0(this, "tag0", "ibits", "TAG_WITH_FLAGS"),
    tagsnoop0(this, "tagsnoop0", "ibits", "TAG_WITH_FLAGS")
{
    Operation::start(this);
    disableVcd();

    // Create and connet Sub-modules:
    datax.changeTmplParameter("abits", "ibits");
    GenObject &i = FORGEN ("i", CONST("0"), CONST("POW2(1,lnbits)"), "++", new STRING("datagen"));
        NEW(*datax.arr_[0], datax.getName().c_str(), &i);
            CONNECT(datax, &i, datax->i_clk, i_clk);
            CONNECT(datax, &i, datax->i_adr, wb_index);
            CONNECT(datax, &i, datax->i_wena, ARRITEM(w_datai_we, i, w_datai_we));
            CONNECT(datax, &i, datax->i_wdata, ARRITEM(wb_datai_wdata, i, wb_datai_wdata));
            CONNECT(datax, &i, datax->o_rdata, ARRITEM(wb_datao_rdata, i, wb_datao_rdata));
        ENDNEW();
    ENDFORGEN(new STRING("datagen"));

    NEW(tag0, tag0.getName().c_str());
        CONNECT(tag0, 0, tag0.i_clk, i_clk);
        CONNECT(tag0, 0, tag0.i_adr, wb_index);
        CONNECT(tag0, 0, tag0.i_wena, w_tagi_we);
        CONNECT(tag0, 0, tag0.i_wdata, wb_tagi_wdata);
        CONNECT(tag0, 0, tag0.o_rdata, wb_tago_rdata);
    ENDNEW();

    GENERATE("snoop_gen");
        IFGEN (snoop, new STRING("snoop_en"));
            NEW(tagsnoop0 , tagsnoop0.getName().c_str());
                CONNECT(tagsnoop0, 0, tagsnoop0.i_clk, i_clk);
                CONNECT(tagsnoop0, 0, tagsnoop0.i_adr, wb_snoop_index);
                CONNECT(tagsnoop0, 0, tagsnoop0.i_wena, w_tagi_we);
                CONNECT(tagsnoop0, 0, tagsnoop0.i_wdata, wb_tagi_wdata);
                CONNECT(tagsnoop0, 0, tagsnoop0.o_rdata, wb_tago_snoop_rdata);
            ENDNEW();
        ELSEGEN(new STRING("snoop"));
            SETZERO(wb_tago_snoop_rdata);
        ENDIFGEN(new STRING("snoop_dis"));
    ENDGENERATE("snoop_gen");

    Operation::start(&comb);
    proc_comb();
}

void TagMemNWay::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;
    GenObject *i;

}

