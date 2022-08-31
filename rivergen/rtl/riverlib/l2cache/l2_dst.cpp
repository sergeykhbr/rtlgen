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

#include "l2_dst.h"

L2Destination::L2Destination(GenObject *parent, const char *name) :
    ModuleObject(parent, "L2Destination", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_resp_valid(this, "i_resp_valid", "1"),
    i_resp_rdata(this, "i_resp_rdata", "L1CACHE_LINE_BITS"),
    i_resp_status(this, "i_resp_status", "2"),
    i_l1o0(this, "i_l1o0"),
    o_l1i0(this, "o_l1i0"),
    i_l1o1(this, "i_l1o1"),
    o_l1i1(this, "o_l1i1"),
    i_l1o2(this, "i_l1o2"),
    o_l1i2(this, "o_l1i2"),
    i_l1o3(this, "i_l1o3"),
    o_l1i3(this, "o_l1i3"),
    i_acpo(this, "i_acpo"),
    o_acpi(this, "o_acpi"),
    _0_(this, "cache interface"),
    i_req_ready(this, "i_req_ready", "1"),
    o_req_valid(this, "o_req_valid", "1"),
    o_req_type(this, "o_req_type", "L2_REQ_TYPE_BITS"),
    o_req_addr(this, "o_req_addr", "CFG_CPU_ADDR_BITS"),
    o_req_size(this, "o_req_size", "3"),
    o_req_prot(this, "o_req_prot", "3"),
    o_req_wdata(this, "o_req_wdata", "L1CACHE_LINE_BITS"),
    o_req_wstrb(this, "o_req_wstrb", "L1CACHE_BYTES_PER_LINE"),
    // params
    Idle(this, "3", "Idle", "0"),
    CacheReadReq(this, "3", "CacheReadReq", "1"),
    CacheWriteReq(this, "3", "CacheWriteReq", "2"),
    ReadMem(this, "3", "ReadMem", "3"),
    WriteMem(this, "3", "WriteMem", "4"),
    snoop_ac(this, "3", "snoop_ac", "5"),
    snoop_cr(this, "3", "snoop_cr", "6"),
    snoop_cd(this, "3", "snoop_c", "7"),
    // signals
    // registers
    state(this, "state", "3", "Idle"),
    srcid(this, "srcid", "3", "CFG_SLOT_L1_TOTAL"),
    req_addr(this, "req_addr", "CFG_CPU_ADDR_BITS"),
    req_size(this, "req_size", "3"),
    req_prot(this, "req_prot", "3"),
    req_src(this, "req_src", "5"),
    req_type(this, "req_type", "L2_REQ_TYPE_BITS"),
    req_wdata(this, "req_wdata", "L1CACHE_LINE_BITS"),
    req_wstrb(this, "req_wstrb", "L1CACHE_BYTES_PER_LINE"),
    ac_valid(this, "ac_valid", "ADD(CFG_SLOT_L1_TOTAL,1)"),
    cr_ready(this, "cr_ready", "ADD(CFG_SLOT_L1_TOTAL,1)"),
    cd_ready(this, "cd_ready", "ADD(CFG_SLOT_L1_TOTAL,1)"),
    // functions
    // process
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}


void L2Destination::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;
    GenObject *i;

    SETVAL(comb.vb_req_type, req_type);

TEXT();
    SETARRITEM(comb.vcoreo, 0, i_acpo);
    SETARRITEM(comb.vcoreo, 1, i_l1o0);
    SETARRITEM(comb.vcoreo, 2, i_l1o1);
    SETARRITEM(comb.vcoreo, 3, i_l1o2);
    SETARRITEM(comb.vcoreo, 4, i_l1o3);
    SETARRITEM(comb.vcoreo, 5, cfg->axi4_l1_out_none);

TEXT();
    SETVAL(comb.vb_srcid, cfg->CFG_SLOT_L1_TOTAL);
    i = &FOR ("i", CONST("0"), cfg->CFG_SLOT_L1_TOTAL, "++");
        SETARRITEM(comb.vlxi, *i, comb.vlxi, cfg->axi4_l1_in_none);

        TEXT();
        SETBIT(comb.vb_src_aw, *i, ARRITEM(comb.vcoreo, *i, comb.vcoreo->aw_valid));
        SETBIT(comb.vb_src_ar, *i, ARRITEM(comb.vcoreo, *i, comb.vcoreo->ar_valid));
    ENDFOR();

TEXT();
    TEXT("select source (aw has higher priority):");
    IF (EZ(comb.vb_src_aw));
        i = &FOR ("i", CONST("0"), cfg->CFG_SLOT_L1_TOTAL, "++");
            IF (AND2(EQ(comb.vb_srcid, cfg->CFG_SLOT_L1_TOTAL), NZ(BIT(comb.vb_src_ar, *i))));
                SETVAL(comb.vb_srcid, *i);
            ENDIF();
        ENDFOR();
    ELSE();
        i = &FOR ("i", CONST("0"), cfg->CFG_SLOT_L1_TOTAL, "++");
            IF (AND2(EQ(comb.vb_srcid, cfg->CFG_SLOT_L1_TOTAL), NZ(BIT(comb.vb_src_aw, *i))));
                SETVAL(comb.vb_srcid, *i);
            ENDIF();
        ENDFOR();
    ENDIF();

TEXT();
    SETVAL(comb.vb_ac_valid, ac_valid);
    SETVAL(comb.vb_cr_ready, cr_ready);
    SETVAL(comb.vb_cd_ready, cd_ready);

TEXT();
    SETVAL(comb.vb_broadband_mask, CONST("0x1E", 5), "exclude acp");
    SETBITZERO(comb.vb_broadband_mask, TO_INT(comb.vb_srcid), "exclude source");

TEXT();
    SWITCH (state);
    CASE(Idle);
        SETZERO(comb.vb_req_type);
        IF (NZ(comb.vb_src_aw));
            SETVAL(state, CacheWriteReq);
            SETARRITEM(comb.vlxi, TO_INT(comb.vb_srcid), comb.vlxi->aw_ready, CONST("1", 1));
            SETARRITEM(comb.vlxi, TO_INT(comb.vb_srcid), comb.vlxi->w_ready , CONST("1", 1), "Lite-interface");

            TEXT();
            SETVAL(srcid, comb.vb_srcid);
            SETVAL(req_addr, ARRITEM(comb.vcoreo, TO_INT(comb.vb_srcid), comb.vcoreo->aw_bits.addr));
            SETVAL(req_size, ARRITEM(comb.vcoreo, TO_INT(comb.vb_srcid), comb.vcoreo->aw_bits.size));
            SETVAL(req_prot, ARRITEM(comb.vcoreo, TO_INT(comb.vb_srcid), comb.vcoreo->aw_bits.prot));
            SETBITONE(comb.vb_req_type, cfg->L2_REQ_TYPE_WRITE);
            IF (NZ(BIT(ARRITEM(comb.vcoreo, TO_INT(comb.vb_srcid), comb.vcoreo->aw_bits.cache), 0)));
                SETBITONE(comb.vb_req_type, cfg->L2_REQ_TYPE_CACHED);
                IF (EQ(ARRITEM(comb.vcoreo, TO_INT(comb.vb_srcid), comb.vcoreo->aw_snoop), cfg->AWSNOOP_WRITE_LINE_UNIQUE));
                    SETBITONE(comb.vb_req_type, cfg->L2_REQ_TYPE_UNIQUE);
                    SETVAL(ac_valid, comb.vb_broadband_mask);
                    SETZERO(cr_ready);
                    SETZERO(cd_ready);
                    SETVAL(state, snoop_ac);
                ENDIF();
            ENDIF();
        ELSIF (NZ(comb.vb_src_ar));
            SETVAL(state, CacheReadReq);
            vlxi[vb_srcid.to_int()].ar_ready = 1;

            TEXT();
            SETVAL(srcid, vb_srcid;
            SETVAL(req_addr, ARRITEM(comb.vcoreo, TO_INT(comb.vb_srcid), comb.vcoreo->ar_bits.addr));
            SETVAL(req_size, ARRITEM(comb.vcoreo, TO_INT(comb.vb_srcid), comb.vcoreo->ar_bits.size));
            SETVAL(req_prot, ARRITEM(comb.vcoreo, TO_INT(comb.vb_srcid), comb.vcoreo->ar_bits.prot));
            IF (NZ(BIT(ARRITEM(comb.vcoreo, TO_INT(comb.vb_srcid), comb.vcoreo->ar_bits.cache), 0)));
                SETBITONE(comb.vb_req_type, cfg->L2_REQ_TYPE_CACHED);
                IF (EQ(ARRITEM(comb.vcoreo, TO_INT(comb.vb_srcid), comb.vcoreo->ar_snoop), cfg->ARSNOOP_READ_MAKE_UNIQUE));
                    SETBITONE(comb.vb_req_type, cfg->L2_REQ_TYPE_UNIQUE);
                ENDIF();
                SETVAL(ac_valid, comb.vb_broadband_mask);
                SETZERO(cr_ready);
                SETZERO(cd_ready);
                SETVAL(state, snoop_ac);
            ENDIF();
        ENDIF();
        SETVAL(req_type, comb.vb_req_type);
        TEXT("Lite-interface");
        SETVAL(req_wdata, ARRITEM(comb.vcoreo, TO_INT(comb.vb_srcid), comb.vcoreo->w_data));
        SETVAL(req_wstrb, ARRITEM(comb.vcoreo, TO_INT(comb.vb_srcid), comb.vcoreo->w_strb));
        ENDCASE();
    CASE(CacheReadReq);
        SETONE(comb.v_req_valid);
        IF (NZ(i_req_ready));
            SETVAL(state, ReadMem);
        ENDIF();
        ENDCASE();
    CASE(CacheWriteReq);
        SETONE(comb.v_req_valid);
        IF (NZ(i_req_ready));
            SETVAL(state, WriteMem);
        ENDIF();
        ENDCASE();
    CASE(ReadMem);
        vlxi[r.srcid.read().to_int()].r_valid = i_resp_valid;
        vlxi[r.srcid.read().to_int()].r_last = i_resp_valid;    // Lite interface
        IF (r.req_type.read()[L2_REQ_TYPE_SNOOP] == 1);
            vlxi[r.srcid.read().to_int()].r_data = r.req_wdata;
        ELSE();
            vlxi[r.srcid.read().to_int()].r_data = i_resp_rdata;
        ENDIF();
        IF (i_resp_status.read() == 0);
            vlxi[r.srcid.read().to_int()].r_resp = 0;
        ELSE();
            vlxi[r.srcid.read().to_int()].r_resp = 0x2;    // SLVERR
        ENDIF();
        if (i_resp_valid.read() == 1) {
            SETVAL(state, Idle, "Wouldn't implement wait to accept because L1 is always ready");
        ENDIF();
        ENDCASE();
    CASE(WriteMem);
        vlxi[r.srcid.read().to_int()].b_valid = i_resp_valid;
        IF (i_resp_status.read() == 0);
            vlxi[r.srcid.read().to_int()].b_resp = 0;
        ELSE();
            vlxi[r.srcid.read().to_int()].b_resp = 0x2;    // SLVERR
        ENDIF();
        IF (i_resp_valid.read() == 1);
            SETVAL(state, Idle, "Wouldn't implement wait to accept because L1 is always ready");
        ENDIF();
        ENDCASE();
    CASE(snoop_ac);
        i = &FOR ("i", CONST("1"), cfg->CFG_SLOT_L1_TOTAL, "++");
            vlxi[i].ac_valid = r.ac_valid.read()[i];
            vlxi[i].ac_addr = r.req_addr;
            IF (r.req_type.read()[L2_REQ_TYPE_UNIQUE] == 1);
                vlxi[i].ac_snoop = AC_SNOOP_READ_UNIQUE;
            ELSE();
                vlxi[i].ac_snoop = 0;
            ENDIF();
            IF (AND2(NZ(BIT(ac_valid, *i)), NZ(ARRITEM(comb.vcoreo, *i, comb.vcoreo->ac_ready))));
                SETBITZERO(comb.vb_ac_valid, *i);
                SETBITONE(comb.vb_cr_ready, *i);
            ENDIF();
        ENDFOR();
        SETVAL(ac_valid, comb.vb_ac_valid);
        SETVAL(cr_ready, comb.vb_cr_ready);
        IF (EZ(comb.vb_ac_valid));
            SETVAL(state, snoop_cr);
        ENDIF();
        ENDCASE();
    CASE(snoop_cr);
        for (int i = 1; i < cfg->CFG_SLOT_L1_TOTAL; i++) {
            vlxi[i].cr_ready = r.cr_ready.read()[i];
            if (r.cr_ready.read()[i] == 1 && vcoreo[i].cr_valid == 1) {
                vb_cr_ready[i] = 0;
                if (vcoreo[i].cr_resp[0] == 1) {  // data transaction flag ACE spec
                    vb_cd_ready[i] = 1;
                }
            }
        }
        v.cr_ready = vb_cr_ready;
        v.cd_ready = vb_cd_ready;
        if (vb_cr_ready.or_reduce() == 0) {
            if (vb_cd_ready.or_reduce() == 1) {
                SETVAL(state, snoop_cd);
            } else if (r.req_type.read()[L2_REQ_TYPE_WRITE] == 1) {
                SETVAL(state, CacheWriteReq);
            } else {
                SETVAL(state, CacheReadReq);
            }
        }
        ENDCASE();
    CASE(snoop_cd);
        // Here only to read Unique data from L1
        for (int i = 1; i < cfg->CFG_SLOT_L1_TOTAL; i++) {
            vlxi[i].cd_ready = r.cd_ready.read()[i];
            if (r.cd_ready.read()[i] == 1 && vcoreo[i].cd_valid == 1) {
                vb_cd_ready[i] = 0;
                v.req_wdata = vcoreo[i].cd_data;
            }
        }
        v.cd_ready = vb_cd_ready;
        if (vb_cd_ready.or_reduce() == 0) {
            if (r.req_type.read()[L2_REQ_TYPE_WRITE] == 1) {
                SETVAL(state, CacheWriteReq);
            } else {
                SETVAL(state, CacheReadReq);
                v.req_wstrb = ~0ul;
            }
            TEXT("write to L2 for Read and Write requests");
            vb_req_type[L2_REQ_TYPE_WRITE] = 1;
            vb_req_type[L2_REQ_TYPE_SNOOP] = 1;
            v.req_type = vb_req_type;
        }
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_acpi, ARRITEM(comb.vlxi, 0));
    SETVAL(o_l1i0, ARRITEM(comb.vlxi, 1));
    SETVAL(o_l1i1, ARRITEM(comb.vlxi, 2));
    SETVAL(o_l1i2, ARRITEM(comb.vlxi, 3));
    SETVAL(o_l1i3, ARRITEM(comb.vlxi, 4));

TEXT();
    SETVAL(o_req_valid, comb.v_req_valid);
    SETVAL(o_req_type, req_type);
    SETVAL(o_req_addr, req_addr);
    SETVAL(o_req_size, req_size);
    SETVAL(o_req_prot, req_prot);
    SETVAL(o_req_wdata, req_wdata);
    SETVAL(o_req_wstrb, req_wstrb);
}
