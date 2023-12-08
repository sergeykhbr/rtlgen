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

#include "l2dummy.h"

L2Dummy::L2Dummy(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "L2Dummy", name, comment),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_l1o(this, "i_l1o"),
    o_l1i(this, "o_l1i"),
    i_l2i(this, "i_l2i"),
    o_l2o(this, "o_l2o"),
    i_flush_valid(this, "i_flush_valid", "1"),
    // params
    Idle(this, "3", "Idle", "0"),
    state_ar(this, "3", "state_ar", "1"),
    state_r(this, "3", "state_r", "2"),
    l1_r_resp(this, "3", "l1_r_resp", "3"),
    state_aw(this, "3", "state_aw", "4"),
    state_w(this, "3", "state_w", "5"),
    state_b(this, "3", "state_b", "6"),
    l1_w_resp(this, "3", "l1_w_resp", "7"),
    // registers
    state(this, "state", "3", "Idle"),
    srcid(this, "srcid", "3", "CFG_SLOT_L1_TOTAL"),
    req_addr(this, "req_addr", "CFG_SYSBUS_ADDR_BITS"),
    req_size(this, "req_size", "3"),
    req_prot(this, "req_prot", "3"),
    req_lock(this, "req_lock", "1"),
    req_id(this, "req_id", "CFG_CPU_ID_BITS"),
    req_user(this, "req_user", "CFG_CPU_USER_BITS", "'0", NO_COMMENT),
    req_wdata(this, "req_wdata", "L1CACHE_LINE_BITS"),
    req_wstrb(this, "req_wstrb", "L1CACHE_BYTES_PER_LINE"),
    rdata(this, "rdata", "L1CACHE_LINE_BITS"),
    resp(this, "resp", "2"),
    // process
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void L2Dummy::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;
    types_river *river = glob_types_river_;
    GenObject *i;

    i = &FOR ("i", CONST("0"), cfg->CFG_SLOT_L1_TOTAL, "++");
        SETARRITEM(comb.vl1o, *i, comb.vl1o, ARRITEM(i_l1o, *i, i_l1o));
        SETARRITEM(comb.vlxi, *i, comb.vlxi, river->axi4_l1_in_none);

        TEXT();
        SETBIT(comb.vb_src_aw, *i, ARRITEM(comb.vl1o, *i, comb.vl1o.aw_valid));
        SETBIT(comb.vb_src_ar, *i, ARRITEM(comb.vl1o, *i, comb.vl1o.ar_valid));
    ENDFOR();
    SETVAL(comb.vl2o, river->axi4_l2_out_none);

TEXT();
    TEXT("select source (aw has higher priority):");
    IF (EZ(comb.vb_src_aw));
        i = &FOR ("i", CONST("0"), cfg->CFG_SLOT_L1_TOTAL, "++");
            IF (AND2(EZ(comb.v_selected), NZ(BIT(comb.vb_src_ar, *i))));
                SETVAL(comb.vb_srcid, *i);
                SETONE(comb.v_selected);
            ENDIF();
        ENDFOR();
    ELSE();
        i = &FOR ("i", CONST("0"), cfg->CFG_SLOT_L1_TOTAL, "++");
            IF (AND2(EZ(comb.v_selected), NZ(BIT(comb.vb_src_aw, *i))));
                SETVAL(comb.vb_srcid, *i);
                SETONE(comb.v_selected);
            ENDIF();
        ENDFOR();
    ENDIF();

    SWITCH(state);
    CASE(Idle);
         IF (NZ(comb.vb_src_aw));
            SETVAL(state, state_aw);
            SETARRITEM(comb.vlxi, comb.vb_srcid, comb.vlxi.aw_ready, CONST("1", 1));
            SETARRITEM(comb.vlxi, comb.vb_srcid, comb.vlxi.w_ready, CONST("1", 1), "AXI-Lite-interface");

            TEXT();
            SETVAL(srcid, comb.vb_srcid);
            SETVAL(req_addr, ARRITEM(comb.vl1o, comb.vb_srcid, comb.vl1o.aw_bits.addr));
            SETVAL(req_size, ARRITEM(comb.vl1o, comb.vb_srcid, comb.vl1o.aw_bits.size));
            SETVAL(req_lock, ARRITEM(comb.vl1o, comb.vb_srcid, comb.vl1o.aw_bits.lock));
            SETVAL(req_prot, ARRITEM(comb.vl1o, comb.vb_srcid, comb.vl1o.aw_bits.prot));
            SETVAL(req_id, ARRITEM(comb.vl1o, comb.vb_srcid, comb.vl1o.aw_id));
            SETVAL(req_user, ARRITEM(comb.vl1o, comb.vb_srcid, comb.vl1o.aw_user));
            TEXT("AXI-Lite-interface");
            SETVAL(req_wdata, ARRITEM(comb.vl1o, comb.vb_srcid, comb.vl1o.w_data));
            SETVAL(req_wstrb, ARRITEM(comb.vl1o, comb.vb_srcid, comb.vl1o.w_strb));
        ELSIF (NZ(comb.vb_src_ar));
            SETVAL(state, state_ar);
            SETARRITEM(comb.vlxi, comb.vb_srcid, comb.vlxi.ar_ready, CONST("1", 1));

            TEXT();
            SETVAL(srcid, comb.vb_srcid);
            SETVAL(req_addr, ARRITEM(comb.vl1o, comb.vb_srcid, comb.vl1o.ar_bits.addr));
            SETVAL(req_size, ARRITEM(comb.vl1o, comb.vb_srcid, comb.vl1o.ar_bits.size));
            SETVAL(req_lock, ARRITEM(comb.vl1o, comb.vb_srcid, comb.vl1o.ar_bits.lock));
            SETVAL(req_prot, ARRITEM(comb.vl1o, comb.vb_srcid, comb.vl1o.ar_bits.prot));
            SETVAL(req_id, ARRITEM(comb.vl1o, comb.vb_srcid, comb.vl1o.ar_id));
            SETVAL(req_user, ARRITEM(comb.vl1o, comb.vb_srcid, comb.vl1o.ar_user));
        ENDIF();
        ENDCASE();
    CASE(state_ar);
        SETONE(comb.vl2o.ar_valid);
        SETVAL(comb.vl2o.ar_bits.addr, req_addr);
        SETVAL(comb.vl2o.ar_bits.size, req_size);
        SETVAL(comb.vl2o.ar_bits.lock, req_lock);
        SETVAL(comb.vl2o.ar_bits.prot, req_prot);
        SETVAL(comb.vl2o.ar_id, req_id);
        SETVAL(comb.vl2o.ar_user, req_user);
        IF (NZ(i_l2i.ar_ready));
            SETVAL(state, state_r);
        ENDIF();
        ENDCASE();
    CASE(state_r);
        SETONE(comb.vl2o.r_ready);
        IF (NZ(i_l2i.r_valid));
            SETVAL(rdata, i_l2i.r_data);
            SETVAL(resp, i_l2i.r_resp);
            SETVAL(state, l1_r_resp);
        ENDIF();
        ENDCASE();
    CASE(l1_r_resp);
        SETARRITEM(comb.vlxi, TO_INT(srcid), comb.vlxi.r_valid,CONST("1", 1));
        SETARRITEM(comb.vlxi, TO_INT(srcid), comb.vlxi.r_last, CONST("1", 1));
        SETARRITEM(comb.vlxi, TO_INT(srcid), comb.vlxi.r_data, rdata);
        SETARRITEM(comb.vlxi, TO_INT(srcid), comb.vlxi.r_resp, CC2(CONST("0", 2), resp));
        SETARRITEM(comb.vlxi, TO_INT(srcid), comb.vlxi.r_id, req_id);
        SETARRITEM(comb.vlxi, TO_INT(srcid), comb.vlxi.r_user, req_user);
        IF (NZ(ARRITEM(comb.vl1o, TO_INT(srcid), comb.vl1o.r_ready)));
            SETVAL(state, Idle);
        ENDIF();
        ENDCASE();
    CASE(state_aw);
        SETONE(comb.vl2o.aw_valid);
        SETVAL(comb.vl2o.aw_bits.addr, req_addr);
        SETVAL(comb.vl2o.aw_bits.size, req_size);
        SETVAL(comb.vl2o.aw_bits.lock, req_lock);
        SETVAL(comb.vl2o.aw_bits.prot, req_prot);
        SETVAL(comb.vl2o.aw_id, req_id);
        SETVAL(comb.vl2o.aw_user, req_user);
        SETONE(comb.vl2o.w_valid, "AXI-Lite request");
        SETONE(comb.vl2o.w_last);
        SETVAL(comb.vl2o.w_data, req_wdata);
        SETVAL(comb.vl2o.w_strb, req_wstrb);
        SETVAL(comb.vl2o.w_user, req_user);
        IF (NZ(i_l2i.aw_ready));
            IF (NZ(i_l2i.w_ready));
                SETVAL(state, state_b);
            ELSE();
                SETVAL(state, state_w);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(state_w);
        SETONE(comb.vl2o.w_valid);
        SETONE(comb.vl2o.w_last);
        SETVAL(comb.vl2o.w_data, req_wdata);
        SETVAL(comb.vl2o.w_strb, req_wstrb);
        SETVAL(comb.vl2o.w_user, req_user);
        IF (NZ(i_l2i.w_ready));
            SETVAL(state, state_b);
        ENDIF();
        ENDCASE();
    CASE(state_b);
        SETONE(comb.vl2o.b_ready);
        IF (NZ(i_l2i.b_valid));
            SETVAL(resp, i_l2i.b_resp);
            SETVAL(state, l1_w_resp);
        ENDIF();
        ENDCASE();
    CASE(l1_w_resp);
        SETARRITEM(comb.vlxi, TO_INT(srcid), comb.vlxi.b_valid, CONST("1", 1));
        SETARRITEM(comb.vlxi, TO_INT(srcid), comb.vlxi.b_resp, resp);
        SETARRITEM(comb.vlxi, TO_INT(srcid), comb.vlxi.b_id, req_id);
        SETARRITEM(comb.vlxi, TO_INT(srcid), comb.vlxi.b_user, req_user);
        IF (NZ(ARRITEM(comb.vl1o, TO_INT(srcid), comb.vl1o.b_ready)));
            SETVAL(state, Idle);
        ENDIF();
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    SYNC_RESET(*this);

TEXT();
    i = &FOR("i", CONST("0"), cfg->CFG_SLOT_L1_TOTAL, "++");
        SETARRITEM(o_l1i, *i, o_l1i, ARRITEM(comb.vlxi, *i, comb.vlxi));
    ENDFOR();
    SETVAL(o_l2o, comb.vl2o);

}

