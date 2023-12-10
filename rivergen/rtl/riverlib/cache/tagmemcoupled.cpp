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

#include "tagmemcoupled.h"

TagMemCoupled::TagMemCoupled(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "TagMemCoupled", name, comment),
    abus(this, "abus", "64", "system bus address width (64 or 32 bits)"),
    waybits(this, "waybits", "2", "log2 of number of ways bits (2 for 4 ways cache; 3 for 8 ways)"),
    ibits(this, "ibits", "6", "lines memory address width (usually 6..8)"),
    lnbits(this, "lnbits", "5", "One line bits: log2(bytes_per_line)"),
    flbits(this, "flbits", "4", "total flags number saved with address tag"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_direct_access(this, "i_direct_access", "1"),
    i_invalidate(this, "i_invalidate", "1"),
    i_re(this, "i_re", "1"),
    i_we(this, "i_we", "1"),
    i_addr(this, "i_addr", "abus"),
    i_wdata(this, "i_wdata", "MUL(8,POW2(1,lnbits))"),
    i_wstrb(this, "i_wstrb", "POW2(1,lnbits)"),
    i_wflags(this, "i_wflags", "flbits"),
    o_raddr(this, "o_raddr", "abus"),
    o_rdata(this, "o_rdata", "ADD(MUL(8,POW2(1,lnbits)),32)"),
    o_rflags(this, "o_rflags", "flbits"),
    o_hit(this, "o_hit", "1"),
    o_hit_next(this, "o_hit_next", "1"),
    LINE_SZ(this, "LINE_SZ", "POW2(1,lnbits)"),
    TAG_START(this, "TAG_START", "SUB(abus,ADD(ibits,lnbits))"),
    EVEN(this, "EVEN", "0"),
    ODD(this, "ODD", "1"),
    MemTotal(this, "MemTotal", "2"),
    tagmem_in_type_def_(this, "tagmem_in_type", NO_COMMENT),
    tagmem_out_type_def_(this, "tagmem_out_type", NO_COMMENT),
    linei(this, "linei", "MemTotal", NO_COMMENT),
    lineo(this, "lineo", "MemTotal", NO_COMMENT),
    req_addr(this, "req_addr", "abus", "'0", NO_COMMENT),
    // process
    comb(this),
    memx(this, "memx", "MemTotal", NO_COMMENT)
{
    Operation::start(this);
    disableVcd();

    // Create and connet Sub-modules:
    memx.abus.setObjValue(&abus);
    memx.waybits.setObjValue(&waybits),
    memx.ibits.setObjValue(&DEC(ibits)),
    memx.lnbits.setObjValue(&lnbits);
    memx.flbits.setObjValue(&flbits);
    memx.snoop.setObjValue(&CONST("0"));
    GenObject &i = FORGEN ("i", CONST("0"), CONST("MemTotal"), "++", new STRING("memgen"));
        NEW(memx, memx.getName().c_str(), &i);
            CONNECT(memx, &i, memx.i_clk, i_clk);
            CONNECT(memx, &i, memx.i_nrst, i_nrst);
            CONNECT(memx, &i, memx.i_direct_access, ARRITEM(linei, i, linei.direct_access));
            CONNECT(memx, &i, memx.i_invalidate, ARRITEM(linei, i, linei.invalidate));
            CONNECT(memx, &i, memx.i_re, ARRITEM(linei, i, linei.re));
            CONNECT(memx, &i, memx.i_we, ARRITEM(linei, i, linei.we));
            CONNECT(memx, &i, memx.i_addr, ARRITEM(linei, i, linei.addr));
            CONNECT(memx, &i, memx.i_wdata, ARRITEM(linei, i, linei.wdata));
            CONNECT(memx, &i, memx.i_wstrb, ARRITEM(linei, i, linei.wstrb));
            CONNECT(memx, &i, memx.i_wflags, ARRITEM(linei, i, linei.wflags));
            CONNECT(memx, &i, memx.o_raddr, ARRITEM(lineo, i, lineo.raddr));
            CONNECT(memx, &i, memx.o_rdata, ARRITEM(lineo, i, lineo.rdata));
            CONNECT(memx, &i, memx.o_rflags, ARRITEM(lineo, i, lineo.rflags));
            CONNECT(memx, &i, memx.o_hit, ARRITEM(lineo, i, lineo.hit));
            CONNECT(memx, &i, memx.i_snoop_addr, ARRITEM(linei, i, linei.snoop_addr));
            CONNECT(memx, &i, memx.o_snoop_ready, ARRITEM(lineo, i, lineo.snoop_ready));
            CONNECT(memx, &i, memx.o_snoop_flags, ARRITEM(lineo, i, lineo.snoop_flags));
        ENDNEW();
    ENDFORGEN(new STRING("memgen"));

    Operation::start(&comb);
    proc_comb();
}

void TagMemCoupled::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;

TEXT();
    SETVAL(req_addr, i_addr);
    SETVAL(comb.v_addr_sel, BIT(i_addr, lnbits));
    SETVAL(comb.v_addr_sel_r, BIT(req_addr, lnbits));

TEXT();
    SETVAL(comb.vb_addr_next, ADD2(i_addr, LINE_SZ));

TEXT();
    SETVAL(comb.vb_index, BITS(i_addr, DEC(ADD2(ibits, lnbits)), lnbits));
    SETVAL(comb.vb_index_next, BITS(comb.vb_addr_next, DEC(ADD2(ibits, lnbits)), lnbits));

TEXT();
    IF (NZ(AND_REDUCE(BITS(i_addr, DEC(lnbits), CONST("2")))));
        SETONE(comb.v_use_overlay);
    ENDIF();
    IF (NZ(AND_REDUCE(BITS(req_addr, DEC(lnbits), CONST("2")))));
        SETONE(comb.v_use_overlay_r);
    ENDIF();


TEXT();
    TEXT("Change the bit order in the requested address:");
    TEXT("   [tag][line_idx][odd/evenbit][line_bytes] on");
    TEXT("   [tag][1'b0]    [line_idx]   [line_bytes]");
    TEXT(" ");
    TEXT("Example (abus=32; ibits=7; lnbits=5;):");
    TEXT("  [4:0]   byte in line           [4:0]");
    TEXT("  [11:5]  line index             {[1'b0],[11:6]}");
    TEXT("  [31:12] tag                    [31:12]");
    SETVAL(comb.vb_addr_tag_direct, i_addr);
    SETBITS(comb.vb_addr_tag_direct, DEC(ADD2(ibits, lnbits)), lnbits, RSH(comb.vb_index, CONST("1")));

TEXT();
    SETVAL(comb.vb_addr_tag_next, comb.vb_addr_next);
    SETBITS(comb.vb_addr_tag_next, DEC(ADD2(ibits, lnbits)), lnbits, RSH(comb.vb_index_next, CONST("1")));

TEXT();
    IF (EZ(comb.v_addr_sel));
        SETARRITEM(linei, EVEN, linei.addr, comb.vb_addr_tag_direct);
        SETARRITEM(linei, EVEN, linei.wstrb, i_wstrb);
        SETARRITEM(linei, ODD, linei.addr, comb.vb_addr_tag_next);
        SETARRITEM(linei, ODD, linei.wstrb, ALLZEROS());
    ELSE();
        SETARRITEM(linei, EVEN, linei.addr, comb.vb_addr_tag_next);
        SETARRITEM(linei, EVEN, linei.wstrb, ALLZEROS());
        SETARRITEM(linei, ODD, linei.addr,  comb.vb_addr_tag_direct);
        SETARRITEM(linei, ODD, linei.wstrb, i_wstrb);
    ENDIF();

TEXT();
    SETARRITEM(linei, EVEN, linei.direct_access, AND2(i_direct_access, OR2(INV(comb.v_addr_sel), comb.v_use_overlay)));
    SETARRITEM(linei, ODD, linei.direct_access, AND2(i_direct_access, OR2(comb.v_addr_sel, comb.v_use_overlay)));

TEXT();
    SETARRITEM(linei, EVEN, linei.invalidate, AND2(i_invalidate, OR2(INV(comb.v_addr_sel), comb.v_use_overlay)));
    SETARRITEM(linei, ODD, linei.invalidate, AND2(i_invalidate, OR2(comb.v_addr_sel, comb.v_use_overlay)));

TEXT();
    SETARRITEM(linei, EVEN, linei.re, AND2(i_re, OR2(INV(comb.v_addr_sel), comb.v_use_overlay)));
    SETARRITEM(linei, ODD, linei.re, AND2(i_re, OR2(comb.v_addr_sel, comb.v_use_overlay)));

TEXT();
    SETARRITEM(linei, EVEN, linei.we, AND2(i_we, OR2(INV(comb.v_addr_sel), comb.v_use_overlay)));
    SETARRITEM(linei, ODD, linei.we, AND2(i_we, OR2(comb.v_addr_sel, comb.v_use_overlay)));

TEXT();
    SETARRITEM(linei, EVEN, linei.wdata, i_wdata);
    SETARRITEM(linei, ODD, linei.wdata, i_wdata);

TEXT();
    SETARRITEM(linei, EVEN, linei.wflags, i_wflags);
    SETARRITEM(linei, ODD, linei.wflags, i_wflags);

TEXT();
    TEXT("Form output:");
    IF (EZ(comb.v_addr_sel_r));
        SETVAL(comb.vb_o_rdata, CC2(BITS(ARRITEM_B(lineo, ODD, lineo.rdata), 31, 0), ARRITEM(lineo, EVEN, lineo.rdata)));
        SETVAL(comb.vb_raddr_tag, ARRITEM(lineo, EVEN, lineo.raddr));
        SETVAL(comb.vb_o_rflags, ARRITEM(lineo, EVEN, lineo.rflags));

        TEXT();
        SETVAL(comb.v_o_hit, ARRITEM(lineo, EVEN, lineo.hit));
        IF (EZ(comb.v_use_overlay_r));
            SETVAL(comb.v_o_hit_next, ARRITEM(lineo, EVEN, lineo.hit));
        ELSE();
            SETVAL(comb.v_o_hit_next, ARRITEM(lineo, ODD, lineo.hit));
        ENDIF();
    ELSE();
        SETVAL(comb.vb_o_rdata, CC2(BITS(ARRITEM_B(lineo, EVEN, lineo.rdata), 31, 0), ARRITEM(lineo, ODD, lineo.rdata)));
        SETVAL(comb.vb_raddr_tag, ARRITEM(lineo, ODD, lineo.raddr));
        SETVAL(comb.vb_o_rflags, ARRITEM(lineo, ODD, lineo.rflags));

        TEXT();
        SETVAL(comb.v_o_hit, ARRITEM(lineo, ODD, lineo.hit));
        IF (EZ(comb.v_use_overlay_r));
            SETVAL(comb.v_o_hit_next, ARRITEM(lineo, ODD, lineo.hit));
        ELSE();
            SETVAL(comb.v_o_hit_next, ARRITEM(lineo, EVEN, lineo.hit));
        ENDIF();
    ENDIF();

TEXT();
    SETVAL(comb.vb_o_raddr, comb.vb_raddr_tag);
    SETBIT(comb.vb_o_raddr, lnbits, comb.v_addr_sel_r);
    SETBITS(comb.vb_o_raddr, DEC(ADD2(ibits, lnbits)), INC(lnbits),
                    BITS(comb.vb_raddr_tag, SUB2(ADD2(ibits, lnbits), CONST("2")), lnbits));

TEXT();
    SETVAL(o_raddr, comb.vb_o_raddr);
    SETVAL(o_rdata, comb.vb_o_rdata);
    SETVAL(o_rflags, comb.vb_o_rflags);
    SETVAL(o_hit, comb.v_o_hit);
    SETVAL(o_hit_next, comb.v_o_hit_next);
}

