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
               const char *gen_waybits, 
               const char *gen_ibits, 
               const char *gen_lnbits, 
               const char *gen_flbits,
               const char *gen_snoop) :
    ModuleObject(parent, "TagMemNWay", name),
    abus(this, "abus", gen_abus, "system bus address width (64 or 32 bits)"),
    waybits(this, "waybits", gen_waybits, "log2 of number of ways bits (2 for 4 ways cache; 3 for 8 ways)"),
    ibits(this, "ibits", gen_ibits, "lines memory address width (usually 6..8)"),
    lnbits(this, "lnbits", gen_lnbits, "One line bits: log2(bytes_per_line)"),
    flbits(this, "flbits", gen_flbits, "total flags number saved with address tag"),
    snoop(this, "snoop", gen_snoop, "0 Snoop port disabled; 1 Enabled (L2 caching)"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_direct_access(this, "i_direct_access", "1", "lsb bits of address forms way index to access"),
    i_invalidate(this, "i_invalidate", "1"),
    i_re(this, "i_re", "1"),
    i_we(this, "i_we", "1"),
    i_addr(this, "i_addr", "abus"),
    i_wdata(this, "i_wdata", "MUL(8,POW2(1,lnbits))"),
    i_wstrb(this, "i_wstrb", "POW2(1,lnbits)"),
    i_wflags(this, "i_wflags", "flbits"),
    o_raddr(this, "o_raddr", "abus"),
    o_rdata(this, "o_rdata", "MUL(8,POW2(1,lnbits))"),
    o_rflags(this, "o_rflags", "flbits"),
    o_hit(this, "o_hit", "1"),
    _snoop0_(this, "L2 snoop port, active when snoop = 1"),
    i_snoop_addr(this, "i_snoop_addr", "abus"),
    o_snoop_ready(this, "o_snoop_ready", "1", "single port memory not used for writing"),
    o_snoop_flags(this, "o_snoop_flags", "flbits"),
    NWAYS(this, "NWAYS", "POW2(1,waybits)"),
    FL_VALID(this, "FL_VALID", "0"),
    WayInTypeDef_(this, "", -1),
    WayOutTypeDef_(this, "", -1),
    w_lrui_init(this, "w_lrui_init", "1"),
    wb_lrui_raddr(this, "wb_lrui_raddr", "ibits"),
    wb_lrui_waddr(this, "wb_lrui_waddr", "ibits"),
    w_lrui_up(this, "w_lrui_up", "1"),
    w_lrui_down(this, "w_lrui_down", "1"),
    wb_lrui_lru(this, "wb_lrui_lru", "waybits"),
    wb_lruo_lru(this, "wb_lruo_lru", "waybits"),
    way_i(this, "", "way_i", "NWAYS"),
    way_o(this, "", "way_o", "NWAYS"),
    req_addr(this, "req_addr", "abus"),
    direct_access(this, "direct_access", "1"),
    invalidate(this, "invalidate", "1"),
    re(this, "re", "1"),
    // process
    comb(this),
    wayx(this, "wayx", "NWAYS"),
    lru0(this, "lru0", "ibits", "waybits")
{
    Operation::start(this);

    // Create and connet Sub-modules:
    wayx.changeTmplParameter("abus", "abus");
    wayx.changeTmplParameter("ibits", "ibits");
    wayx.changeTmplParameter("lnbits", "lnbits");
    wayx.changeTmplParameter("flbits", "flbits");
    wayx.changeTmplParameter("snoop", "snoop");
    GenObject &i = FORGEN ("i", CONST("0"), CONST("NWAYS"), "++", new STRING("waygen"));
        NEW(*wayx.arr_[0], wayx.getName().c_str(), &i);
            CONNECT(wayx, &i, wayx->i_clk, i_clk);
            CONNECT(wayx, &i, wayx->i_nrst, i_nrst);
            CONNECT(wayx, &i, wayx->i_addr, ARRITEM(way_i, i, way_i->addr));
            CONNECT(wayx, &i, wayx->i_wstrb, ARRITEM(way_i, i, way_i->wstrb));
            CONNECT(wayx, &i, wayx->i_wdata, ARRITEM(way_i, i, way_i->wdata));
            CONNECT(wayx, &i, wayx->i_wflags, ARRITEM(way_i, i, way_i->wflags));
            CONNECT(wayx, &i, wayx->o_raddr, ARRITEM(way_o, i, way_o->raddr));
            CONNECT(wayx, &i, wayx->o_rdata, ARRITEM(way_o, i, way_o->rdata));
            CONNECT(wayx, &i, wayx->o_rflags, ARRITEM(way_o, i, way_o->rflags));
            CONNECT(wayx, &i, wayx->o_hit, ARRITEM(way_o, i, way_o->hit));
            CONNECT(wayx, &i, wayx->i_snoop_addr, ARRITEM(way_i, i, way_i->snoop_addr));
            CONNECT(wayx, &i, wayx->o_snoop_flags, ARRITEM(way_o, i, way_o->snoop_flags));

        ENDNEW();
    ENDFORGEN(new STRING("waygen"));

    NEW(lru0, lru0.getName().c_str());
        CONNECT(lru0, 0, lru0.i_clk, i_clk);
        CONNECT(lru0, 0, lru0.i_init, w_lrui_init);
        CONNECT(lru0, 0, lru0.i_raddr, wb_lrui_raddr);
        CONNECT(lru0, 0, lru0.i_waddr, wb_lrui_waddr);
        CONNECT(lru0, 0, lru0.i_up, w_lrui_up);
        CONNECT(lru0, 0, lru0.i_down, w_lrui_down);
        CONNECT(lru0, 0, lru0.i_lru, wb_lrui_lru);
        CONNECT(lru0, 0, lru0.o_lru, wb_lruo_lru);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void TagMemNWay::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;
    GenObject *i;

TEXT();
    SETVAL(direct_access, i_direct_access);
    SETVAL(invalidate, i_invalidate);
    SETVAL(re, i_re);
    SETVAL(req_addr, i_addr);

TEXT();
    SETVAL(comb.vb_hit_idx, wb_lruo_lru);
    IF (NZ(direct_access));
        SETVAL(comb.vb_hit_idx, BITS(req_addr, DEC(waybits), CONST("0")));
    ELSE();
        i = &FOR ("i", CONST("0"), NWAYS, "++");
            IF (NZ(ARRITEM(way_o, *i, way_o->hit)));
                SETVAL(comb.vb_hit_idx, *i);
            ENDIF();
        ENDFOR();
    ENDIF();
    SETVAL(comb.vb_raddr, ARRITEM(way_o, TO_INT(comb.vb_hit_idx), way_o->raddr));
    SETVAL(comb.vb_rdata, ARRITEM(way_o, TO_INT(comb.vb_hit_idx), way_o->rdata));
    SETVAL(comb.vb_rflags, ARRITEM(way_o, TO_INT(comb.vb_hit_idx), way_o->rflags));
    SETVAL(comb.v_hit, ARRITEM(way_o, TO_INT(comb.vb_hit_idx), way_o->hit));

TEXT();
    IF (NZ(invalidate));
        SETZERO(comb.vb_wflags);
        SETVAL(comb.vb_wstrb, ALLONES());
    ELSE();
        SETVAL(comb.vb_wflags, i_wflags);
        SETVAL(comb.vb_wstrb, i_wstrb);
    ENDIF();
    TEXT("");
    TEXT("    Warning: we can write only into previously read line,");
    TEXT("                if the previuosly read line is hit and contains valid flags");
    TEXT("                HIGH we modify it. Otherwise, we write into displacing line.");
    TEXT("");
    i = &FOR ("i", CONST("0"), NWAYS, "++");
        SETARRITEM(way_i, *i, way_i->addr, i_addr);
        SETARRITEM(way_i, *i, way_i->wdata, i_wdata);
        SETARRITEM(way_i, *i, way_i->wstrb, ALLZEROS());
        SETARRITEM(way_i, *i, way_i->wflags, comb.vb_wflags);
        SETARRITEM(way_i, *i, way_i->snoop_addr, i_snoop_addr);
    ENDFOR();

TEXT();
    SETVAL(comb.v_way_we, OR2(i_we, AND2(invalidate, comb.v_hit)));
    IF (NZ(comb.v_way_we));
        SETARRITEM(way_i, TO_INT(comb.vb_hit_idx), way_i->wstrb, comb.vb_wstrb);
    ENDIF();

TEXT();
    SETONE(comb.v_snoop_ready);
    IF (EQ(snoop, CONST("1")));
        i = &FOR("i", CONST("0"), NWAYS, "++");
            TEXT("tagmem already cleared snoop flags if there's no snoop hit");
            IF (NZ(BIT(ARRITEM_B(way_o, *i, way_o->snoop_flags), FL_VALID)));
                SETVAL(comb.vb_snoop_flags, ARRITEM(way_o, *i, way_o->snoop_flags));
            ENDIF();
        ENDFOR();
        TEXT("Writing into snoop tag memory, output value won't be valid on next clock");
        IF (NZ(comb.v_way_we));
            SETZERO(comb.v_snoop_ready);
        ENDIF();
    ENDIF();

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(w_lrui_init, direct_access);
    SETVAL(wb_lrui_raddr, BITS(i_addr, DEC(ADD2(ibits,lnbits)), lnbits));
    SETVAL(wb_lrui_waddr, BITS(req_addr, DEC(ADD2(ibits, lnbits)), lnbits));
    SETVAL(w_lrui_up, OR2(i_we, AND2(comb.v_hit, re)));
    SETVAL(w_lrui_down, AND2(comb.v_hit, invalidate));
    SETVAL(wb_lrui_lru, comb.vb_hit_idx);

TEXT();
    SETVAL(o_raddr, comb.vb_raddr);
    SETVAL(o_rdata, comb.vb_rdata);
    SETVAL(o_rflags, comb.vb_rflags);
    SETVAL(o_hit, comb.v_hit);
    SETVAL(o_snoop_ready, comb.v_snoop_ready);
    SETVAL(o_snoop_flags, comb.vb_snoop_flags);
}

