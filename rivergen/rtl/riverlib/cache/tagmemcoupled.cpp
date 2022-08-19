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

TagMemCoupled::TagMemCoupled(GenObject *parent,
                             const char *name,
                             const char *gen_abus,
                             const char *gen_waybits,
                             const char *gen_ibits, 
                             const char *gen_lnbits, 
                             const char *gen_flbits) :
    ModuleObject(parent, "TagMemCoupled", name),
    abus(this, "abus", gen_abus, "system bus address width (64 or 32 bits)"),
    waybits(this, "waybits", gen_waybits, "log2 of number of ways bits (2 for 4 ways cache; 3 for 8 ways)"),
    ibits(this, "ibits", gen_ibits, "lines memory address width (usually 6..8)"),
    lnbits(this, "lnbits", gen_lnbits, "One line bits: log2(bytes_per_line)"),
    flbits(this, "flbits", gen_flbits, "total flags number saved with address tag"),
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
    TAG_START(this, "TAG_START", "SUB(abus,ADD(ibits,lnbits))"),
    EVEN(this, "EVEN", "0"),
    ODD(this, "ODD", "1"),
    MemTotal(this, "MemTotal", "2"),
    tagmem_in_type_def_(this, -1),
    tagmem_out_type_def_(this, -1),
    linei(this, "linei", "MemTotal"),
    lineo(this, "lineo", "MemTotal"),
    req_addr(this, "req_addr", "abus"),
    // process
    comb(this),
    memx(this, "memx", "MemTotal")
{
    Operation::start(this);
    disableVcd();

    // Create and connet Sub-modules:
    memx.changeTmplParameter("abus", "abus");
    memx.changeTmplParameter("waybits", "waybits"),
    memx.changeTmplParameter("ibits", "SUB(ibits,1)"),
    memx.changeTmplParameter("lnbits", "lnbits");
    memx.changeTmplParameter("flbits", "flbits");
    memx.changeTmplParameter("snoop", "0");
    GenObject &i = FORGEN ("i", CONST("0"), CONST("MemTotal"), "++", new STRING("memgen"));
        NEW(*memx.arr_[0], memx.getName().c_str(), &i);
            CONNECT(memx, &i, memx->i_clk, i_clk);
            CONNECT(memx, &i, memx->i_nrst, i_nrst);
            CONNECT(memx, &i, memx->i_direct_access, ARRITEM(linei, i, linei->direct_access));
            CONNECT(memx, &i, memx->i_invalidate, ARRITEM(linei, i, linei->invalidate));
            CONNECT(memx, &i, memx->i_re, ARRITEM(linei, i, linei->re));
            CONNECT(memx, &i, memx->i_we, ARRITEM(linei, i, linei->we));
            CONNECT(memx, &i, memx->i_addr, ARRITEM(linei, i, linei->addr));
            CONNECT(memx, &i, memx->i_wdata, ARRITEM(linei, i, linei->wdata));
            CONNECT(memx, &i, memx->i_wstrb, ARRITEM(linei, i, linei->wstrb));
            CONNECT(memx, &i, memx->i_wflags, ARRITEM(linei, i, linei->wflags));
            CONNECT(memx, &i, memx->o_raddr, ARRITEM(lineo, i, lineo->raddr));
            CONNECT(memx, &i, memx->o_rdata, ARRITEM(lineo, i, lineo->rdata));
            CONNECT(memx, &i, memx->o_rflags, ARRITEM(lineo, i, lineo->rflags));
            CONNECT(memx, &i, memx->o_hit, ARRITEM(lineo, i, lineo->hit));
            CONNECT(memx, &i, memx->i_snoop_addr, ARRITEM(linei, i, linei->snoop_addr));
            CONNECT(memx, &i, memx->o_snoop_ready, ARRITEM(lineo, i, lineo->snoop_ready));
            CONNECT(memx, &i, memx->o_snoop_flags, ARRITEM(lineo, i, lineo->snoop_flags));
        ENDNEW();
    ENDFORGEN(new STRING("memgen"));

    Operation::start(&comb);
    proc_comb();
}

void TagMemCoupled::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;
    //GenObject *i;

}

