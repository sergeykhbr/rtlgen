// 
//  Copyright 2025 Sergey Khabarov, sergeykhbr@gmail.com
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

#include "l1_dma_snoop.h"

l1_dma_snoop::l1_dma_snoop(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "l1_dma_snoop", name, comment),
    // Generic parameters
    abits(this, "abits", "48", "adress bits used"),
    userbits(this, "userbits", "1"),
    base_offset(this, "base_offset", "64", "'0", "Address offset for all DMA transactions"),
    coherence_ena(this, "coherence_ena", "false"),
    // Ports
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    o_req_mem_ready(this, "o_req_mem_ready", "1", "Ready to accept next data"),
    i_req_mem_path(this, "i_req_mem_path", "1", "instruction/data"),
    i_req_mem_valid(this, "i_req_mem_valid", "1", "Request data is ready to accept"),
    i_req_mem_type(this, "i_req_mem_type", "REQ_MEM_TYPE_BITS", "read/write/cached"),
    i_req_mem_size(this, "i_req_mem_size", "3", ""),
    i_req_mem_addr(this, "i_req_mem_addr", "abits", "Address to read/write"),
    i_req_mem_strob(this, "i_req_mem_strob", "L1CACHE_BYTES_PER_LINE", "Byte enabling write strob"),
    i_req_mem_data(this, "i_req_mem_data", "L1CACHE_LINE_BITS", "Data to write"),
    o_resp_mem_path(this, "o_resp_mem_path", "1", "instruction/data."),
    o_resp_mem_valid(this, "o_resp_mem_valid", "1", "Read/Write data is valid. All write transaction with valid response."),
    o_resp_mem_load_fault(this, "o_resp_mem_load_fault", "1", "Error on memory access"),
    o_resp_mem_store_fault(this, "o_resp_mem_store_fault", "1", "Error on memory access"),
    o_resp_mem_data(this, "o_resp_mem_data", "L1CACHE_LINE_BITS", "Read data value"),
    _dsnoop0_(this, "D$ Snoop interface"),
    o_req_snoop_valid(this, "o_req_snoop_valid", "1"),
    o_req_snoop_type(this, "o_req_snoop_type", "SNOOP_REQ_TYPE_BITS"),
    i_req_snoop_ready(this, "i_req_snoop_ready", "1"),
    o_req_snoop_addr(this, "o_req_snoop_addr", "abits"),
    o_resp_snoop_ready(this, "o_resp_snoop_ready", "1"),
    i_resp_snoop_valid(this, "i_resp_snoop_valid", "1"),
    i_resp_snoop_data(this, "i_resp_snoop_data", "L1CACHE_LINE_BITS"),
    i_resp_snoop_flags(this, "i_resp_snoop_flags", "DTAG_FL_TOTAL"),
    i_msti(this, "i_msti", "L1-cache master input"),
    o_msto(this, "o_msto", "L1-cache master output"),
    // params
    state_idle(this, "state_idle", "3", "0", NO_COMMENT),
    state_ar(this, "state_ar", "3", "1", NO_COMMENT),
    state_r(this, "state_r", "3", "2", NO_COMMENT),
    state_aw(this, "state_aw", "3", "3", NO_COMMENT),
    state_w(this, "state_w", "3", "4", NO_COMMENT),
    state_b(this, "state_b", "3", "5", NO_COMMENT),
    snoop_idle(this, "snoop_idle", "3", "0", NO_COMMENT),
    snoop_ac_wait_accept(this, "snoop_ac_wait_accept", "3", "1", NO_COMMENT),
    snoop_cr(this, "snoop_cr", "3", "2", NO_COMMENT),
    snoop_cr_wait_accept(this, "snoop_cr_wait_accept", "3", "3", NO_COMMENT),
    snoop_cd(this, "snoop_cd", "3", "4", NO_COMMENT),
    snoop_cd_wait_accept(this, "snoop_cd_wait_accept", "3", "5", NO_COMMENT),
    // Singals:
    // registers
    state(this, "state", "3", "state_idle", NO_COMMENT),
    req_addr(this, "req_addr", "abits", "'0", NO_COMMENT),
    req_path(this, "req_path", "1"),
    req_cached(this, "req_cached", "4", "'0", NO_COMMENT),
    req_wdata(this, "req_wdata", "L1CACHE_LINE_BITS", "'0", NO_COMMENT),
    req_wstrb(this, "req_wstrb", "L1CACHE_BYTES_PER_LINE", "'0", NO_COMMENT),
    req_size(this, "req_size", "3", "'0", NO_COMMENT),
    req_prot(this, "req_prot", "3", "'0", NO_COMMENT),
    req_ar_snoop(this, "req_ar_snoop", "4", "'0", NO_COMMENT),
    req_aw_snoop(this, "req_aw_snoop", "3", "'0", NO_COMMENT),
    snoop_state(this, "snoop_state", "3", "snoop_idle", NO_COMMENT),
    ac_addr(this, "ac_addr", "abits", "'0", NO_COMMENT),
    ac_snoop(this, "ac_snoop", "4", "0", "Table C3-19"),
    cr_resp(this, "cr_resp", "5", "'0", NO_COMMENT),
    req_snoop_type(this, "req_snoop_type", "SNOOP_REQ_TYPE_BITS", "'0", NO_COMMENT),
    resp_snoop_data(this, "resp_snoop_data", "L1CACHE_LINE_BITS", "'0", NO_COMMENT),
    cache_access(this, "cache_access", "1"),
    watchdog(this, "watchdog", "13", "'0", NO_COMMENT),
    // functions
    reqtype2arsnoop(this),
    reqtype2awsnoop(this),
    // submodules:
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

l1_dma_snoop::reqtype2arsnoop_func::reqtype2arsnoop_func(GenObject *parent)
    : FunctionObject(parent, "reqtype2arsnoop"),
    ret(this, "ret", "4"),
    reqtype(this, "reqtype", "REQ_MEM_TYPE_BITS") {
    SETZERO(ret);
    IF (EZ(BIT(reqtype, glob_river_cfg_->REQ_MEM_TYPE_CACHED)));
        SETVAL(ret, glob_types_amba_->ARSNOOP_READ_NO_SNOOP);
    ELSE();
        IF (EZ(BIT(reqtype, glob_river_cfg_->REQ_MEM_TYPE_UNIQUE)));
            SETVAL(ret, glob_types_amba_->ARSNOOP_READ_SHARED);
        ELSE();
            SETVAL(ret, glob_types_amba_->ARSNOOP_READ_MAKE_UNIQUE);
        ENDIF();
    ENDIF();
}

l1_dma_snoop::reqtype2awsnoop_func::reqtype2awsnoop_func(GenObject *parent)
    : FunctionObject(parent, "reqtype2awsnoop"),
    ret(this, "ret", "4"),
    reqtype(this, "reqtype", "REQ_MEM_TYPE_BITS") {
    SETZERO(ret);
    IF (EZ(BIT(reqtype, glob_river_cfg_->REQ_MEM_TYPE_CACHED)));
        SETVAL(ret, glob_types_amba_->AWSNOOP_WRITE_NO_SNOOP);
    ELSE();
        IF (EZ(BIT(reqtype, glob_river_cfg_->REQ_MEM_TYPE_UNIQUE)));
            SETVAL(ret, glob_types_amba_->AWSNOOP_WRITE_BACK);
        ELSE();
            SETVAL(ret, glob_types_amba_->AWSNOOP_WRITE_LINE_UNIQUE);
        ENDIF();
    ENDIF();
}

void l1_dma_snoop::proc_comb() {
    SETVAL(comb.vmsto.ar_bits.burst, *SCV_get_cfg_type(this, "AXI_BURST_INCR"), "INCR (possible any value actually)");
    SETVAL(comb.vmsto.aw_bits.burst, *SCV_get_cfg_type(this, "AXI_BURST_INCR"), "INCR (possible any value actually)");

    SWITCH (state);
    CASE(state_idle);
        SETONE(comb.v_next_ready);
        IF (NZ(i_req_mem_valid));
            SETVAL(req_path, i_req_mem_path);
            SETVAL(req_addr, i_req_mem_addr);
            SETVAL(req_size, i_req_mem_size);
            TEXT("[0] 0=Unpriv/1=Priv;");
            TEXT("[1] 0=Secure/1=Non-secure;");
            TEXT("[2] 0=Data/1=Instruction");
            SETVAL(req_prot, LSH(i_req_mem_path, CONST("2")));
            IF (EZ(BIT(i_req_mem_type, *SCV_get_cfg_type(this, "REQ_MEM_TYPE_WRITE"))));
                SETVAL(state, state_ar);
                SETZERO(req_wdata);
                SETZERO(req_wstrb);
                IF (NZ(BIT(i_req_mem_type, *SCV_get_cfg_type(this, "REQ_MEM_TYPE_CACHED"))));
                    SETVAL(req_cached, *SCV_get_cfg_type(this, "ARCACHE_WRBACK_READ_ALLOCATE"));
                ELSE();
                    SETVAL(req_cached, *SCV_get_cfg_type(this, "ARCACHE_DEVICE_NON_BUFFERABLE"));
                ENDIF();
                IF (NZ(coherence_ena));
                    CALLF(&req_ar_snoop, reqtype2arsnoop, 1, &i_req_mem_type);
                ENDIF();
            ELSE();
                SETVAL(state, state_aw);
                SETVAL(req_wdata, i_req_mem_data);
                SETVAL(req_wstrb, i_req_mem_strob);
                IF (NZ(BIT(i_req_mem_type, *SCV_get_cfg_type(this, "REQ_MEM_TYPE_CACHED"))));
                    SETVAL(req_cached, *SCV_get_cfg_type(this, "AWCACHE_WRBACK_WRITE_ALLOCATE"));
                ELSE();
                    SETVAL(req_cached, *SCV_get_cfg_type(this, "AWCACHE_DEVICE_NON_BUFFERABLE"));
                ENDIF();
                IF (NZ(coherence_ena));
                    CALLF(&req_aw_snoop, reqtype2awsnoop, 1, &i_req_mem_type);
                ENDIF();
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(state_ar);
        SETONE(comb.vmsto.ar_valid);
        SETVAL(comb.vmsto.ar_bits.addr, req_addr);
        SETVAL(comb.vmsto.ar_bits.cache, req_cached);
        SETVAL(comb.vmsto.ar_bits.size, req_size);
        SETVAL(comb.vmsto.ar_bits.prot, req_prot);
        SETVAL(comb.vmsto.ar_snoop, req_ar_snoop);
        IF (OR2(NZ(i_msti.ar_ready), BIT(watchdog, 12)));
            SETVAL(state, state_r);
        ENDIF();
        ENDCASE();
    CASE(state_r);
        SETONE(comb.vmsto.r_ready);
        SETVAL(comb.v_mem_er_load_fault, OR2_L(BIT(i_msti.r_resp, 1), BIT(watchdog, 12)));
        SETVAL(comb.v_resp_mem_valid, i_msti.r_valid);
        TEXT("r_valid and r_last always should be in the same time");
        IF (OR2(AND2(NZ(i_msti.r_valid), NZ(i_msti.r_last)), BIT(watchdog, 12)));
            SETVAL(state, state_idle);
        ENDIF();
        ENDCASE();
    CASE(state_aw);
        SETONE(comb.vmsto.aw_valid);
        SETVAL(comb.vmsto.aw_bits.addr, req_addr);
        SETVAL(comb.vmsto.aw_bits.cache, req_cached);
        SETVAL(comb.vmsto.aw_bits.size, req_size);
        SETVAL(comb.vmsto.aw_bits.prot, req_prot);
        SETVAL(comb.vmsto.aw_snoop, req_aw_snoop);
        TEXT("axi lite to simplify L2-cache");
        SETONE(comb.vmsto.w_valid);
        SETONE(comb.vmsto.w_last);
        SETVAL(comb.vmsto.w_data, req_wdata);
        SETVAL(comb.vmsto.w_strb, req_wstrb);
        IF (OR2(NZ(i_msti.aw_ready), BIT(watchdog, 12)));
            IF (NZ(i_msti.w_ready));
                SETVAL(state, state_b);
            ELSE();
                SETVAL(state, state_w);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(state_w);
        TEXT("Shoudln't get here because of Lite interface:");
        SETONE(comb.vmsto.w_valid);
        SETONE(comb.vmsto.w_last);
        SETVAL(comb.vmsto.w_data, req_wdata);
        SETVAL(comb.vmsto.w_strb, req_wstrb);
        IF (OR2(NZ(i_msti.w_ready), NZ(BIT(watchdog, 12))));
            SETVAL(state, state_b);
        ENDIF();
        ENDCASE();
    CASE(state_b);
        SETONE(comb.vmsto.b_ready);
        SETVAL(comb.v_resp_mem_valid, i_msti.b_valid);
        SETVAL(comb.v_mem_er_store_fault, OR2_L(BIT(i_msti.b_resp, 1), BIT(watchdog, 12)));
        IF (OR2(NZ(i_msti.b_valid), BIT(watchdog, 12)));
            SETVAL(state, state_idle);
        ENDIF();
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

    TEXT();
    IF (EQ(state, state_idle));
        SETZERO(watchdog);
    ELSE();
        SETVAL(watchdog, INC(watchdog));
    ENDIF();

TEXT();
    TEXT("Snoop processing:");
    SWITCH (snoop_state);
    CASE(snoop_idle);
        SETONE(comb.v_snoop_next_ready);
        ENDCASE();
    CASE(snoop_ac_wait_accept);
        SETONE(comb.req_snoop_valid);
        SETVAL(comb.vb_req_snoop_addr, ac_addr);
        SETVAL(comb.vb_req_snoop_type, req_snoop_type);
        IF (NZ(i_req_snoop_ready));
            IF (EZ(cache_access));
                SETVAL(snoop_state, snoop_cr);
            ELSE();
                SETVAL(snoop_state, snoop_cd);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(snoop_cr);
        IF (NZ(i_resp_snoop_valid));
            SETONE(comb.v_cr_valid);
            IF (ANDx(2, &NZ(BIT(i_resp_snoop_flags, *SCV_get_cfg_type(this, "TAG_FL_VALID"))),
                        &ORx(2, &EZ(BIT(i_resp_snoop_flags, *SCV_get_cfg_type(this, "DTAG_FL_SHARED"))),
                                &EQ(ac_snoop, *SCV_get_cfg_type(this, "AC_SNOOP_READ_UNIQUE")))));
                TEXT("Need second request with cache access");
                SETONE(cache_access);
                TEXT("see table C3-21 \"Snoop response bit allocation\"");
                SETBITONE(comb.vb_cr_resp, 0, "will be Data transfer");
                SETBITONE(comb.vb_cr_resp, 4, "WasUnique");
                IF (EQ(ac_snoop, *SCV_get_cfg_type(this, "AC_SNOOP_READ_UNIQUE")));
                    SETBITONE(comb.vb_req_snoop_type, *SCV_get_cfg_type(this, "SNOOP_REQ_TYPE_READCLEAN"));
                ELSE();
                    SETBITONE(comb.vb_req_snoop_type, *SCV_get_cfg_type(this, "SNOOP_REQ_TYPE_READDATA"));
                ENDIF();
                SETVAL(req_snoop_type, comb.vb_req_snoop_type);
                SETVAL(snoop_state, snoop_ac_wait_accept);
                IF (NZ(i_msti.cr_ready));
                    SETVAL(snoop_state, snoop_ac_wait_accept);
                ELSE();
                    SETVAL(snoop_state, snoop_cr_wait_accept);
                ENDIF();
            ELSE();
                SETZERO(comb.vb_cr_resp);
                IF (NZ(i_msti.cr_ready));
                    SETVAL(snoop_state, snoop_idle);
                ELSE();
                    SETVAL(snoop_state, snoop_cr_wait_accept);
                ENDIF();
            ENDIF();
            SETVAL(cr_resp, comb.vb_cr_resp);
        ENDIF();
        ENDCASE();
    CASE(snoop_cr_wait_accept);
        SETONE(comb.v_cr_valid);
        SETVAL(comb.vb_cr_resp, cr_resp);
        IF (NZ(i_msti.cr_ready));
            IF (NZ(cache_access));
                SETVAL(snoop_state, snoop_ac_wait_accept);
            ELSE();
                SETVAL(snoop_state, snoop_idle);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(snoop_cd);
        IF (NZ(i_resp_snoop_valid));
            SETONE(comb.v_cd_valid);
            SETVAL(comb.vb_cd_data, i_resp_snoop_data);
            SETVAL(resp_snoop_data, i_resp_snoop_data);
            IF (NZ(i_msti.cd_ready));
                SETVAL(snoop_state, snoop_idle);
            ELSE();
                SETVAL(snoop_state, snoop_cd_wait_accept);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(snoop_cd_wait_accept);
        SETONE(comb.v_cd_valid);
        SETVAL(comb.vb_cd_data, resp_snoop_data);
        IF (NZ(i_msti.cd_ready));
            SETVAL(snoop_state, snoop_idle);
        ENDIF();
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    IF (ANDx(3, &NZ(coherence_ena),
                &NZ(comb.v_snoop_next_ready),
                &NZ(i_msti.ac_valid)));
        SETONE(comb.req_snoop_valid);
        SETZERO(cache_access);
        SETZERO(comb.vb_req_snoop_type, "First snoop operation always just to read flags");
        SETZERO(req_snoop_type);
        SETVAL(comb.vb_req_snoop_addr, i_msti.ac_addr);
        SETVAL(ac_addr, i_msti.ac_addr);
        SETVAL(ac_snoop, i_msti.ac_snoop);
        IF (NZ(i_req_snoop_ready));
            SETVAL(snoop_state, snoop_cr);
        ELSE();
            SETVAL(snoop_state, snoop_ac_wait_accept);
        ENDIF();
    ELSE();
        SETONE(comb.v_snoop_next_ready);
        SETONE(comb.v_cr_valid);
    ENDIF();

TEXT();
    SYNC_RESET();

TEXT();
    SETVAL(comb.vmsto.ac_ready, comb.v_snoop_next_ready);
    SETVAL(comb.vmsto.cr_valid, comb.v_cr_valid);
    SETVAL(comb.vmsto.cr_resp, comb.vb_cr_resp);
    SETVAL(comb.vmsto.cd_valid, comb.v_cd_valid);
    SETVAL(comb.vmsto.cd_data, comb.vb_cd_data);
    SETVAL(comb.vmsto.cd_last, comb.v_cd_valid);
    SETZERO(comb.vmsto.rack);
    SETZERO(comb.vmsto.wack);

TEXT();
    SETVAL(o_req_mem_ready, comb.v_next_ready);
    SETVAL(o_resp_mem_path, req_path);
    SETVAL(o_resp_mem_valid, comb.v_resp_mem_valid);
    SETVAL(o_resp_mem_data, i_msti.r_data);
    SETVAL(o_resp_mem_load_fault, comb.v_mem_er_load_fault);
    SETVAL(o_resp_mem_store_fault, comb.v_mem_er_store_fault);
    TEXT("AXI Snoop IOs:");
    SETVAL(o_req_snoop_valid, comb.req_snoop_valid);
    SETVAL(o_req_snoop_type, comb.vb_req_snoop_type);
    SETVAL(o_req_snoop_addr, comb.vb_req_snoop_addr);
    SETONE(o_resp_snoop_ready);

TEXT();
    SETVAL(o_msto, comb.vmsto);
}
