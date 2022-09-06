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

#include "river_amba.h"

RiverAmba::RiverAmba(GenObject *parent, const char *name) :
    ModuleObject(parent, "RiverAmba", name),
    // Generic parameters
    hartid(this, "hartid", "0"),
    fpu_ena(this, "fpu_ena", "true"),
    coherence_ena(this, "coherence_ena", "false"),
    tracer_ena(this, "tracer_ena", "true"),
    // Ports
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_msti(this, "i_msti"),
    o_msto(this, "o_msto"),
    o_xcfg(this, "o_xcfg"),
    i_dport(this, "i_dport"),
    o_dport(this, "o_dport"),
    i_msip(this, "i_msip", "1", "machine software pending interrupt"),
    i_mtip(this, "i_mtip", "1", "machine timer pending interrupt"),
    i_meip(this, "i_meip", "1", "machine external pending interrupt"),
    i_seip(this, "i_seip", "1", "supervisor external pending interrupt"),
    o_flush_l2(this, "o_flush_l2", "1", "Flush L2 after D$ has been finished"),
    o_halted(this, "o_halted", "1", "CPU halted via debug interface"),
    o_available("1", "o_available", "1", this, "CPU was instantitated of stubbed"),
    i_progbuf(this, "i_progbuf", "MUL(32,CFG_PROGBUF_REG_TOTAL)", "progam buffer"),
    // params
    state_idle(this, "3", "state_idle", "0"),
    state_ar(this, "3", "state_ar", "1"),
    state_r(this, "3", "state_r", "2"),
    state_aw(this, "3", "state_aw", "3"),
    state_w(this, "3", "state_w", "4"),
    state_b(this, "3", "state_b", "5"),
    snoop_idle(this, "3", "snoop_idle", "0"),
    snoop_ac_wait_accept(this, "3", "snoop_ac_wait_accept", "1"),
    snoop_cr(this, "3", "snoop_cr", "2"),
    snoop_cr_wait_accept(this, "3", "snoop_cr_wait_accept", "3"),
    snoop_cd(this, "3", "snoop_cd", "4"),
    snoop_cd_wait_accept(this, "3", "snoop_cd_wait_accept", "5"),
    // Singals:
    req_mem_ready_i(this, "req_mem_ready_i", "1"),
    req_mem_path_o(this, "req_mem_path_o", "1"),
    req_mem_valid_o(this, "req_mem_valid_o", "1"),
    req_mem_type_o(this, "req_mem_type_o", "REQ_MEM_TYPE_BITS"),
    req_mem_size_o(this, "req_mem_size_o", "3"),
    req_mem_addr_o(this, "req_mem_addr_o", "CFG_CPU_ADDR_BITS"),
    req_mem_strob_o(this, "req_mem_strob_o", "L1CACHE_BYTES_PER_LINE"),
    req_mem_data_o(this, "req_mem_data_o", "L1CACHE_LINE_BITS"),
    resp_mem_data_i(this, "resp_mem_data_i", "L1CACHE_LINE_BITS"),
    resp_mem_valid_i(this, "resp_mem_valid_i", "1"),
    resp_mem_load_fault_i(this, "resp_mem_load_fault_i", "1"),
    resp_mem_store_fault_i(this, "resp_mem_store_fault_i", "1"),
    _dsnoop0_(this, "D$ Snoop interface"),
    req_snoop_valid_i(this, "req_snoop_valid_i", "1"),
    req_snoop_type_i(this, "req_snoop_type_i", "SNOOP_REQ_TYPE_BITS"),
    req_snoop_ready_o(this, "req_snoop_ready_o", "1"),
    req_snoop_addr_i(this, "req_snoop_addr_i", "CFG_CPU_ADDR_BITS"),
    resp_snoop_ready_i(this, "resp_snoop_ready_i", "1"),
    resp_snoop_valid_o(this, "resp_snoop_valid_o", "1"),
    resp_snoop_data_o(this, "resp_snoop_data_o", "L1CACHE_LINE_BITS"),
    resp_snoop_flags_o(this, "resp_snoop_flags_o", "DTAG_FL_TOTAL"),
    wb_ip(this, "wb_ip", "IRQ_PER_HART_TOTAL", "Interrupt pending bits"),
    wb_xcfg(this, "wb_xcfg"),
    // registers
    state(this, "state", "3"),
    req_addr(this, "req_addr", "CFG_CPU_ADDR_BITS", "state_idle"),
    req_path(this, "req_path", "1"),
    req_cached(this, "req_cached", "4"),
    req_wdata(this, "req_wdata", "L1CACHE_LINE_BITS"),
    req_wstrb(this, "req_wstrb", "L1CACHE_BYTES_PER_LINE"),
    req_size(this, "req_size", "3"),
    req_prot(this, "req_prot", "3"),
    req_ar_snoop(this, "req_ar_snoop", "4"),
    req_aw_snoop(this, "req_aw_snoop", "3"),
    snoop_state(this, "snoop_state", "3", "snoop_idle"),
    ac_addr(this, "ac_addr", "CFG_CPU_ADDR_BITS"),
    ac_snoop(this, "ac_snoop", "4", "0", "Table C3-19"),
    cr_resp(this, "cr_resp", "5"),
    req_snoop_type(this, "req_snoop_type", "SNOOP_REQ_TYPE_BITS"),
    resp_snoop_data(this, "resp_snoop_data", "L1CACHE_LINE_BITS"),
    cache_access(this, "cache_access", "1"),
    // functions
    reqtype2arsnoop(this),
    reqtype2awsnoop(this),
    // submodules:
    river0(this, "river0"),
    comb(this)
{
    Operation::start(this);

    // Create and connet Sub-modules:
    NEW(river0, river0.getName().c_str());
        CONNECT(river0, 0, river0.i_clk, i_clk);
        CONNECT(river0, 0, river0.i_nrst, i_nrst);
        CONNECT(river0, 0, river0.i_req_mem_ready, req_mem_ready_i);
        CONNECT(river0, 0, river0.o_req_mem_path, req_mem_path_o);
        CONNECT(river0, 0, river0.o_req_mem_valid, req_mem_valid_o);
        CONNECT(river0, 0, river0.o_req_mem_type, req_mem_type_o);
        CONNECT(river0, 0, river0.o_req_mem_size, req_mem_size_o);
        CONNECT(river0, 0, river0.o_req_mem_addr, req_mem_addr_o);
        CONNECT(river0, 0, river0.o_req_mem_strob, req_mem_strob_o);
        CONNECT(river0, 0, river0.o_req_mem_data, req_mem_data_o);
        CONNECT(river0, 0, river0.i_resp_mem_valid, resp_mem_valid_i);
        CONNECT(river0, 0, river0.i_resp_mem_path, req_path);
        CONNECT(river0, 0, river0.i_resp_mem_data, resp_mem_data_i);
        CONNECT(river0, 0, river0.i_resp_mem_load_fault, resp_mem_load_fault_i);
        CONNECT(river0, 0, river0.i_resp_mem_store_fault, resp_mem_store_fault_i);
        CONNECT(river0, 0, river0.i_req_snoop_valid, req_snoop_valid_i);
        CONNECT(river0, 0, river0.i_req_snoop_type, req_snoop_type_i);
        CONNECT(river0, 0, river0.o_req_snoop_ready, req_snoop_ready_o);
        CONNECT(river0, 0, river0.i_req_snoop_addr, req_snoop_addr_i);
        CONNECT(river0, 0, river0.i_resp_snoop_ready, resp_snoop_ready_i);
        CONNECT(river0, 0, river0.o_resp_snoop_valid, resp_snoop_valid_o);
        CONNECT(river0, 0, river0.o_resp_snoop_data, resp_snoop_data_o);
        CONNECT(river0, 0, river0.o_resp_snoop_flags, resp_snoop_flags_o);
        CONNECT(river0, 0, river0.o_flush_l2, o_flush_l2);
        CONNECT(river0, 0, river0.i_irq_pending, wb_ip);
        CONNECT(river0, 0, river0.i_haltreq, i_dport->haltreq);
        CONNECT(river0, 0, river0.i_resumereq, i_dport->resumereq);
        CONNECT(river0, 0, river0.i_dport_req_valid, i_dport->req_valid);
        CONNECT(river0, 0, river0.i_dport_type, i_dport->dtype);
        CONNECT(river0, 0, river0.i_dport_addr, i_dport->addr);
        CONNECT(river0, 0, river0.i_dport_wdata, i_dport->wdata);
        CONNECT(river0, 0, river0.i_dport_size, i_dport->size);
        CONNECT(river0, 0, river0.o_dport_req_ready, o_dport->req_ready);
        CONNECT(river0, 0, river0.i_dport_resp_ready, i_dport->resp_ready);
        CONNECT(river0, 0, river0.o_dport_resp_valid, o_dport->resp_valid);
        CONNECT(river0, 0, river0.o_dport_resp_error, o_dport->resp_error);
        CONNECT(river0, 0, river0.o_dport_rdata, o_dport->rdata);
        CONNECT(river0, 0, river0.i_progbuf, i_progbuf);
        CONNECT(river0, 0, river0.o_halted, o_halted);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

RiverAmba::reqtype2arsnoop_func::reqtype2arsnoop_func(GenObject *parent)
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

RiverAmba::reqtype2awsnoop_func::reqtype2awsnoop_func(GenObject *parent)
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

void RiverAmba::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;

    SETVAL(wb_xcfg.descrsize, glob_types_amba_->PNP_CFG_MASTER_DESCR_BYTES);
    SETVAL(wb_xcfg.descrtype, glob_types_amba_->PNP_CFG_TYPE_MASTER);
    SETVAL(wb_xcfg.vid, glob_types_amba_->VENDOR_OPTIMITECH);
    SETVAL(wb_xcfg.did, glob_types_amba_->RISCV_RIVER_CPU);

TEXT();
    SETBIT(comb.vb_ip, cfg->IRQ_HART_MSIP, i_msip);
    SETBIT(comb.vb_ip, cfg->IRQ_HART_MTIP, i_mtip);
    SETBIT(comb.vb_ip, cfg->IRQ_HART_MEIP, i_meip);
    SETBIT(comb.vb_ip, cfg->IRQ_HART_SEIP, i_seip);
    SETVAL(comb.vmsto, glob_types_river_->axi4_l1_out_none);
    SETVAL(comb.vmsto.ar_bits.burst, glob_types_amba_->AXI_BURST_INCR, "INCR (possible any value actually)");
    SETVAL(comb.vmsto.aw_bits.burst, glob_types_amba_->AXI_BURST_INCR, "INCR (possible any value actually)");

    SWITCH (state);
    CASE(state_idle);
        SETONE(comb.v_next_ready);
        IF (NZ(req_mem_valid_o));
            SETVAL(req_path, req_mem_path_o);
            SETVAL(req_addr, req_mem_addr_o);
            SETVAL(req_size, req_mem_size_o);
            TEXT("[0] 0=Unpriv/1=Priv;");
            TEXT("[1] 0=Secure/1=Non-secure;");
            TEXT("[2] 0=Data/1=Instruction");
            SETVAL(req_prot, LSH(req_mem_path_o, CONST("2")));
            IF (EZ(BIT(req_mem_type_o, cfg->REQ_MEM_TYPE_WRITE)));
                SETVAL(state, state_ar);
                SETZERO(req_wdata);
                SETZERO(req_wstrb);
                IF (NZ(BIT(req_mem_type_o, cfg->REQ_MEM_TYPE_CACHED)));
                    SETVAL(req_cached, glob_types_amba_->ARCACHE_WRBACK_READ_ALLOCATE);
                ELSE();
                    SETVAL(req_cached, glob_types_amba_->ARCACHE_DEVICE_NON_BUFFERABLE);
                ENDIF();
                IF (NZ(coherence_ena));
                    CALLF(&req_ar_snoop, reqtype2arsnoop, 1, &req_mem_type_o);
                ENDIF();
            ELSE();
                SETVAL(state, state_aw);
                SETVAL(req_wdata, req_mem_data_o);
                SETVAL(req_wstrb, req_mem_strob_o);
                IF (NZ(BIT(req_mem_type_o, cfg->REQ_MEM_TYPE_CACHED)));
                    SETVAL(req_cached, glob_types_amba_->AWCACHE_WRBACK_WRITE_ALLOCATE);
                ELSE();
                    SETVAL(req_cached, glob_types_amba_->AWCACHE_DEVICE_NON_BUFFERABLE);
                ENDIF();
                IF (NZ(coherence_ena));
                    CALLF(&req_aw_snoop, reqtype2awsnoop, 1, &req_mem_type_o);
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
        IF (NZ(i_msti->ar_ready));
            SETVAL(state, state_r);
        ENDIF();
        ENDCASE();
    CASE(state_r);
        SETONE(comb.vmsto.r_ready);
        SETVAL(comb.v_mem_er_load_fault, BIT(i_msti->r_resp, 1));
        SETVAL(comb.v_resp_mem_valid, i_msti->r_valid);
        TEXT("r_valid and r_last always should be in the same time");
        IF (AND2(NZ(i_msti->r_valid), NZ(i_msti->r_last)));
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
        IF (NZ(i_msti->aw_ready));
            IF (NZ(i_msti->w_ready));
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
        IF (NZ(i_msti->w_ready));
            SETVAL(state, state_b);
        ENDIF();
        ENDCASE();
    CASE(state_b);
        SETONE(comb.vmsto.b_ready);
        SETVAL(comb.v_resp_mem_valid, i_msti->b_valid);
        SETVAL(comb.v_mem_er_store_fault, BIT(i_msti->b_resp, 1));
        IF (NZ(i_msti->b_valid));
            SETVAL(state, state_idle);
        ENDIF();
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

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
        IF (NZ(req_snoop_ready_o));
            IF (EZ(cache_access));
                SETVAL(snoop_state, snoop_cr);
            ELSE();
                SETVAL(snoop_state, snoop_cd);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(snoop_cr);
        IF (NZ(resp_snoop_valid_o));
            SETONE(comb.v_cr_valid);
            IF (ANDx(2, &NZ(BIT(resp_snoop_flags_o, cfg->TAG_FL_VALID)),
                        &ORx(2, &EZ(BIT(resp_snoop_flags_o, cfg->DTAG_FL_SHARED)),
                                &EQ(ac_snoop, glob_types_amba_->AC_SNOOP_READ_UNIQUE))));
                TEXT("Need second request with cache access");
                SETONE(cache_access);
                TEXT("see table C3-21 \"Snoop response bit allocation\"");
                SETBITONE(comb.vb_cr_resp, 0, "will be Data transfer");
                SETBITONE(comb.vb_cr_resp, 4, "WasUnique");
                IF (EQ(ac_snoop, glob_types_amba_->AC_SNOOP_READ_UNIQUE));
                    SETBITONE(comb.vb_req_snoop_type, cfg->SNOOP_REQ_TYPE_READCLEAN);
                ELSE();
                    SETBITONE(comb.vb_req_snoop_type, cfg->SNOOP_REQ_TYPE_READDATA);
                ENDIF();
                SETVAL(req_snoop_type, comb.vb_req_snoop_type);
                SETVAL(snoop_state, snoop_ac_wait_accept);
                IF (NZ(i_msti->cr_ready));
                    SETVAL(snoop_state, snoop_ac_wait_accept);
                ELSE();
                    SETVAL(snoop_state, snoop_cr_wait_accept);
                ENDIF();
            ELSE();
                SETZERO(comb.vb_cr_resp);
                IF (NZ(i_msti->cr_ready));
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
        IF (NZ(i_msti->cr_ready));
            IF (NZ(cache_access));
                SETVAL(snoop_state, snoop_ac_wait_accept);
            ELSE();
                SETVAL(snoop_state, snoop_idle);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(snoop_cd);
        IF (NZ(resp_snoop_valid_o));
            SETONE(comb.v_cd_valid);
            SETVAL(comb.vb_cd_data, resp_snoop_data_o);
            SETVAL(resp_snoop_data, resp_snoop_data_o);
            IF (NZ(i_msti->cd_ready));
                SETVAL(snoop_state, snoop_idle);
            ELSE();
                SETVAL(snoop_state, snoop_cd_wait_accept);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(snoop_cd_wait_accept);
        SETONE(comb.v_cd_valid);
        SETVAL(comb.vb_cd_data, resp_snoop_data);
        IF (NZ(i_msti->cd_ready));
            SETVAL(snoop_state, snoop_idle);
        ENDIF();
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    IF (ANDx(3, &NZ(coherence_ena),
                &NZ(comb.v_snoop_next_ready),
                &NZ(i_msti->ac_valid)));
        SETONE(comb.req_snoop_valid);
        SETZERO(cache_access);
        SETZERO(comb.vb_req_snoop_type, "First snoop operation always just to read flags");
        SETZERO(req_snoop_type);
        SETVAL(comb.vb_req_snoop_addr, i_msti->ac_addr);
        SETVAL(ac_addr, i_msti->ac_addr);
        SETVAL(ac_snoop, i_msti->ac_snoop);
        IF (NZ(req_snoop_ready_o));
            SETVAL(snoop_state, snoop_cr);
        ELSE();
            SETVAL(snoop_state, snoop_ac_wait_accept);
        ENDIF();
    ELSE();
        SETONE(comb.v_snoop_next_ready);
        SETONE(comb.v_cr_valid);
    ENDIF();

TEXT();
    SYNC_RESET(*this);

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
    SETVAL(wb_ip, comb.vb_ip);
    SETVAL(req_mem_ready_i, comb.v_next_ready);
    SETVAL(resp_mem_valid_i, comb.v_resp_mem_valid);
    SETVAL(resp_mem_data_i, i_msti->r_data);
    SETVAL(resp_mem_load_fault_i, comb.v_mem_er_load_fault);
    SETVAL(resp_mem_store_fault_i, comb.v_mem_er_store_fault);
    TEXT("AXI Snoop IOs:");
    SETVAL(req_snoop_valid_i, comb.req_snoop_valid);
    SETVAL(req_snoop_type_i, comb.vb_req_snoop_type);
    SETVAL(req_snoop_addr_i, comb.vb_req_snoop_addr);
    SETONE(resp_snoop_ready_i);

TEXT();
    SETVAL(o_msto, comb.vmsto);
    SETVAL(o_xcfg, wb_xcfg);
    SETVAL(o_available, CONST("1", 1));
}
