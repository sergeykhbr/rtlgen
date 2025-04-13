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

L2Destination::L2Destination(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "L2Destination", name, comment),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_resp_valid(this, "i_resp_valid", "1"),
    i_resp_rdata(this, "i_resp_rdata", "L1CACHE_LINE_BITS"),
    i_resp_status(this, "i_resp_status", "2"),
    i_l1o(this, "i_l1o"),
    o_l1i(this, "o_l1i"),
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
    Idle(this, "Idle", "3", "0", NO_COMMENT),
    CacheReadReq(this, "CacheReadReq", "3", "1", NO_COMMENT),
    CacheWriteReq(this, "CacheWriteReq", "3", "2", NO_COMMENT),
    ReadMem(this, "ReadMem", "3", "3", NO_COMMENT),
    WriteMem(this, "WriteMem", "3", "4", NO_COMMENT),
    snoop_ac(this, "snoop_ac", "3", "5", NO_COMMENT),
    snoop_cr(this, "snoop_cr", "3", "6", NO_COMMENT),
    snoop_cd(this, "snoop_cd", "3", "7", NO_COMMENT),
    // signals
    // registers
    state(this, "state", "3", "Idle"),
    srcid(this, "srcid", "3", "CFG_SLOT_L1_TOTAL"),
    req_addr(this, "req_addr", "CFG_CPU_ADDR_BITS", "'0", NO_COMMENT),
    req_size(this, "req_size", "3", "'0", NO_COMMENT),
    req_prot(this, "req_prot", "3", "'0", NO_COMMENT),
    req_src(this, "req_src", "5", "'0", NO_COMMENT),
    req_type(this, "req_type", "L2_REQ_TYPE_BITS", "'0", NO_COMMENT),
    req_wdata(this, "req_wdata", "L1CACHE_LINE_BITS", "'0", NO_COMMENT),
    req_wstrb(this, "req_wstrb", "L1CACHE_BYTES_PER_LINE", "'0", NO_COMMENT),
    ac_valid(this, "ac_valid", "ADD(CFG_SLOT_L1_TOTAL,1)", "'0", NO_COMMENT),
    cr_ready(this, "cr_ready", "ADD(CFG_SLOT_L1_TOTAL,1)", "'0", NO_COMMENT),
    cd_ready(this, "cd_ready", "ADD(CFG_SLOT_L1_TOTAL,1)", "'0", NO_COMMENT),
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
    types_amba *amba = glob_types_amba_;
    GenObject *i;

    SETVAL(comb.vb_req_type, req_type);

TEXT();
    SETVAL(comb.vb_srcid, cfg->CFG_SLOT_L1_TOTAL);
    i = &FOR ("i", CONST("0"), cfg->CFG_SLOT_L1_TOTAL, "++");
        SETARRITEM(comb.vcoreo, *i, comb.vcoreo, ARRITEM(i_l1o, *i, i_l1o), "Cannot read vector item from port in systemc");
        SETARRITEM(comb.vlxi, *i, comb.vlxi, glob_types_river_->axi4_l1_in_none);

        TEXT();
        SETBIT(comb.vb_src_aw, *i, ARRITEM(comb.vcoreo, *i, comb.vcoreo.aw_valid));
        SETBIT(comb.vb_src_ar, *i, ARRITEM(comb.vcoreo, *i, comb.vcoreo.ar_valid));
    ENDFOR();
    SETARRITEM(comb.vcoreo, cfg->CFG_SLOT_L1_TOTAL, comb.vcoreo, glob_types_river_->axi4_l1_out_none);

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
    SETVAL(comb.vb_broadband_mask_full, ALLONES());
    SETBITZERO(comb.vb_broadband_mask_full, cfg->CFG_SLOT_L1_TOTAL, "exclude empty slot");
    SETVAL(comb.vb_broadband_mask, comb.vb_broadband_mask_full);
    SETBITZERO(comb.vb_broadband_mask, TO_INT(comb.vb_srcid), "exclude source");

TEXT();
    SWITCH (state);
    CASE(Idle);
        SETZERO(comb.vb_req_type);
        IF (NZ(comb.vb_src_aw));
            SETVAL(state, CacheWriteReq);
            SETARRITEM(comb.vlxi, TO_INT(comb.vb_srcid), comb.vlxi.aw_ready, CONST("1", 1));
            SETARRITEM(comb.vlxi, TO_INT(comb.vb_srcid), comb.vlxi.w_ready , CONST("1", 1), "Lite-interface");

            TEXT();
            SETVAL(srcid, comb.vb_srcid);
            SETVAL(req_addr, ARRITEM(comb.vcoreo, TO_INT(comb.vb_srcid), comb.vcoreo.aw_bits.addr));
            SETVAL(req_size, ARRITEM(comb.vcoreo, TO_INT(comb.vb_srcid), comb.vcoreo.aw_bits.size));
            SETVAL(req_prot, ARRITEM(comb.vcoreo, TO_INT(comb.vb_srcid), comb.vcoreo.aw_bits.prot));
            SETBITONE(comb.vb_req_type, cfg->L2_REQ_TYPE_WRITE);
            IF (NZ(BIT(ARRITEM(comb.vcoreo, TO_INT(comb.vb_srcid), comb.vcoreo.aw_bits.cache), 0)));
                SETBITONE(comb.vb_req_type, cfg->L2_REQ_TYPE_CACHED);
                IF (EQ(ARRITEM(comb.vcoreo, TO_INT(comb.vb_srcid), comb.vcoreo.aw_snoop), amba->AWSNOOP_WRITE_LINE_UNIQUE));
                    SETBITONE(comb.vb_req_type, cfg->L2_REQ_TYPE_UNIQUE);
                    SETVAL(ac_valid, comb.vb_broadband_mask);
                    SETZERO(cr_ready);
                    SETZERO(cd_ready);
                    SETVAL(state, snoop_ac);
                ENDIF();
            ENDIF();
        ELSIF (NZ(comb.vb_src_ar));
            SETVAL(state, CacheReadReq);
            SETARRITEM(comb.vlxi, TO_INT(comb.vb_srcid), comb.vlxi.ar_ready, CONST("1", 1));

            TEXT();
            SETVAL(srcid, comb.vb_srcid);
            SETVAL(req_addr, ARRITEM(comb.vcoreo, TO_INT(comb.vb_srcid), comb.vcoreo.ar_bits.addr));
            SETVAL(req_size, ARRITEM(comb.vcoreo, TO_INT(comb.vb_srcid), comb.vcoreo.ar_bits.size));
            SETVAL(req_prot, ARRITEM(comb.vcoreo, TO_INT(comb.vb_srcid), comb.vcoreo.ar_bits.prot));
            IF (NZ(BIT(ARRITEM(comb.vcoreo, TO_INT(comb.vb_srcid), comb.vcoreo.ar_bits.cache), 0)));
                SETBITONE(comb.vb_req_type, cfg->L2_REQ_TYPE_CACHED);
                IF (EQ(ARRITEM(comb.vcoreo, TO_INT(comb.vb_srcid), comb.vcoreo.ar_snoop), amba->ARSNOOP_READ_MAKE_UNIQUE));
                    SETBITONE(comb.vb_req_type, cfg->L2_REQ_TYPE_UNIQUE);
                ENDIF();
                TEXT("prot[2]: 0=Data, 1=Instr.");
                TEXT("If source is I$ then request D$ of the same CPU");
                IF (NZ(BIT(ARRITEM(comb.vcoreo, TO_INT(comb.vb_srcid), comb.vcoreo.ar_bits.prot), 2)));
                    SETVAL(ac_valid, comb.vb_broadband_mask_full);
                ELSE();
                    SETVAL(ac_valid, comb.vb_broadband_mask);
                ENDIF();
                SETZERO(cr_ready);
                SETZERO(cd_ready);
                SETVAL(state, snoop_ac);
            ENDIF();
        ENDIF();
        SETVAL(req_type, comb.vb_req_type);
        TEXT("Lite-interface");
        SETVAL(req_wdata, ARRITEM(comb.vcoreo, TO_INT(comb.vb_srcid), comb.vcoreo.w_data));
        SETVAL(req_wstrb, ARRITEM(comb.vcoreo, TO_INT(comb.vb_srcid), comb.vcoreo.w_strb));
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
        SETARRITEM(comb.vlxi, TO_INT(srcid), comb.vlxi.r_valid, i_resp_valid);
        SETARRITEM(comb.vlxi, TO_INT(srcid), comb.vlxi.r_last, i_resp_valid, "Lite interface");
        IF (NZ(BIT(req_type, cfg->L2_REQ_TYPE_SNOOP)));
            SETARRITEM(comb.vlxi, TO_INT(srcid), comb.vlxi.r_data, req_wdata);
        ELSE();
            SETARRITEM(comb.vlxi, TO_INT(srcid), comb.vlxi.r_data, i_resp_rdata);
        ENDIF();
        IF (EZ(i_resp_status));
            SETARRITEM(comb.vlxi, TO_INT(srcid), comb.vlxi.r_resp, amba->AXI_RESP_OKAY);
        ELSE();
            SETARRITEM(comb.vlxi, TO_INT(srcid), comb.vlxi.r_resp, amba->AXI_RESP_SLVERR);
        ENDIF();
        IF (NZ(i_resp_valid));
            SETVAL(state, Idle, "Wouldn't implement wait to accept because L1 is always ready");
        ENDIF();
        ENDCASE();
    CASE(WriteMem);
        SETARRITEM(comb.vlxi, TO_INT(srcid), comb.vlxi.b_valid, i_resp_valid);
        IF (EZ(i_resp_status));
            SETARRITEM(comb.vlxi, TO_INT(srcid), comb.vlxi.b_resp, amba->AXI_RESP_OKAY);
        ELSE();
            SETARRITEM(comb.vlxi, TO_INT(srcid), comb.vlxi.b_resp, amba->AXI_RESP_SLVERR);
        ENDIF();
        IF (NZ(i_resp_valid));
            SETVAL(state, Idle, "Wouldn't implement wait to accept because L1 is always ready");
        ENDIF();
        ENDCASE();
    CASE(snoop_ac);
        i = &FOR ("i", CONST("0"), cfg->CFG_SLOT_L1_TOTAL, "++");
            SETARRITEM(comb.vlxi, *i, comb.vlxi.ac_valid, BIT(ac_valid, *i));
            SETARRITEM(comb.vlxi, *i, comb.vlxi.ac_addr, req_addr);
            IF (NZ(BIT(req_type, cfg->L2_REQ_TYPE_UNIQUE)));
                SETARRITEM(comb.vlxi, *i, comb.vlxi.ac_snoop, amba->AC_SNOOP_READ_UNIQUE);
            ELSE();
                SETARRITEM(comb.vlxi, *i, comb.vlxi.ac_snoop, CONST("0", 4));
            ENDIF();
            IF (AND2(NZ(BIT(ac_valid, *i)), NZ(ARRITEM(comb.vcoreo, *i, comb.vcoreo.ac_ready))));
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
        i = &FOR ("i", CONST("0"), cfg->CFG_SLOT_L1_TOTAL, "++");
            SETARRITEM(comb.vlxi, *i, comb.vlxi.cr_ready, BIT(cr_ready, *i));
            IF (AND2(NZ(BIT(cr_ready, *i)), NZ(ARRITEM(comb.vcoreo, *i, comb.vcoreo.cr_valid))));
                SETBITZERO(comb.vb_cr_ready, *i);
                IF (NZ(BIT(ARRITEM(comb.vcoreo, *i, comb.vcoreo.cr_resp), 0)), "data transaction flag ACE spec");
                    SETBITONE(comb.vb_cd_ready, *i);
                ENDIF();
            ENDIF();
        ENDFOR();
        SETVAL(cr_ready, comb.vb_cr_ready);
        SETVAL(cd_ready, comb.vb_cd_ready);
        IF (EZ(comb.vb_cr_ready));
            IF (NZ(comb.vb_cd_ready));
                SETVAL(state, snoop_cd);
            ELSIF (NZ(BIT(req_type, cfg->L2_REQ_TYPE_WRITE)));
                SETVAL(state, CacheWriteReq);
            ELSE();
                SETVAL(state, CacheReadReq);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(snoop_cd);
        TEXT("Here only to read Unique data from L1 and write to L2");
        i = &FOR ("i", CONST("0"), cfg->CFG_SLOT_L1_TOTAL, "++");
            SETARRITEM(comb.vlxi, *i, comb.vlxi.cd_ready, BIT(cd_ready, *i));
            IF (AND2(NZ(BIT(cd_ready, *i)), NZ(ARRITEM(comb.vcoreo, *i, comb.vcoreo.cd_valid))));
                SETBITZERO(comb.vb_cd_ready, *i);
                SETVAL(req_wdata, ARRITEM(comb.vcoreo, *i, comb.vcoreo.cd_data));
            ENDIF();
        ENDFOR();
        SETVAL(cd_ready, comb.vb_cd_ready);
        IF (EZ(comb.vb_cd_ready));
            IF (NZ(BIT(req_type, cfg->L2_REQ_TYPE_WRITE)));
                SETVAL(state, CacheWriteReq);
            ELSE();
                SETVAL(state, CacheReadReq);
                SETVAL(req_wstrb, ALLONES());
            ENDIF();
            TEXT("write to L2 for Read and Write requests");
            SETBITONE(comb.vb_req_type, cfg->L2_REQ_TYPE_WRITE);
            SETBITONE(comb.vb_req_type, cfg->L2_REQ_TYPE_SNOOP);
            SETVAL(req_type, comb.vb_req_type);
        ENDIF();
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    SYNC_RESET(*this);

TEXT();
    i = &FOR ("i", CONST("0"), cfg->CFG_SLOT_L1_TOTAL, "++");
        SETARRITEM(o_l1i, *i, o_l1i, ARRITEM(comb.vlxi, *i, comb.vlxi), "vector should be assigned in cycle in systemc");
    ENDFOR();

TEXT();
    SETVAL(o_req_valid, comb.v_req_valid);
    SETVAL(o_req_type, req_type);
    SETVAL(o_req_addr, req_addr);
    SETVAL(o_req_size, req_size);
    SETVAL(o_req_prot, req_prot);
    SETVAL(o_req_wdata, req_wdata);
    SETVAL(o_req_wstrb, req_wstrb);
}
