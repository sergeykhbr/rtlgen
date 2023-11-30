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

#include "../../../prj/impl/asic/target_cfg.h"
#include "icache_lru.h"

ICacheLru::ICacheLru(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "ICacheLru", name, comment),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    _ctrl0_(this, "Control path:"),
    i_req_valid(this, "i_req_valid", "1"),
    i_req_addr(this, "i_req_addr", "CFG_CPU_ADDR_BITS"),
    o_req_ready(this, "o_req_ready", "1"),
    o_resp_valid(this, "o_resp_valid", "1"),
    o_resp_addr(this, "o_resp_addr", "CFG_CPU_ADDR_BITS"),
    o_resp_data(this, "o_resp_data", "64"),
    o_resp_load_fault(this, "o_resp_load_fault", "1"),
    i_resp_ready(this, "i_resp_ready", "1"),
    _mem0_(this, "Memory interface:"),
    i_req_mem_ready(this, "i_req_mem_ready", "1"),
    o_req_mem_valid(this, "o_req_mem_valid", "1"),
    o_req_mem_type(this, "o_req_mem_type", "REQ_MEM_TYPE_BITS"),
    o_req_mem_size(this, "o_req_mem_size", "3"),
    o_req_mem_addr(this, "o_req_mem_addr", "CFG_CPU_ADDR_BITS"),
    o_req_mem_strob(this, "o_req_mem_strob", "L1CACHE_BYTES_PER_LINE", "unused"),
    o_req_mem_data(this, "o_req_mem_data", "L1CACHE_LINE_BITS"),
    i_mem_data_valid(this, "i_mem_data_valid", "1"),
    i_mem_data(this, "i_mem_data", "L1CACHE_LINE_BITS"),
    i_mem_load_fault(this, "i_mem_load_fault", "1"),
    _mpu0(this, "Mpu interface"),
    o_mpu_addr(this, "o_mpu_addr", "CFG_CPU_ADDR_BITS"),
    i_pma_cached(this, "i_pma_cached", "1"),
    i_pmp_x(this, "i_pmp_x", "1", "PMP eXecute access"),
    _flush0_(this, "Flush interface"),
    i_flush_address(this, "i_flush_address", "CFG_CPU_ADDR_BITS"),
    i_flush_valid(this, "i_flush_valid", "1"),
    // params
    abus(this, "abus", "CFG_CPU_ADDR_BITS"),
    lnbits(this, "lnbits", "CFG_LOG2_L1CACHE_BYTES_PER_LINE"),
    flbits(this, "flbits", "ITAG_FL_TOTAL"),
    ways(this, "ways", "POW2(1,CFG_ILOG2_NWAYS)"),
    _1_(this),
    _2_(this, "State machine states:"),
    State_Idle(this, "4", "State_Idle", "0"),
    State_CheckHit(this, "4", "State_CheckHit", "1"),
    State_TranslateAddress(this, "4", "State_TranslateAddress", "2"),
    State_WaitGrant(this, "4", "State_WaitGrant", "3"),
    State_WaitResp(this, "4", "State_WaitResp", "4"),
    State_CheckResp(this, "4", "State_CheckResp", "5"),
    State_SetupReadAdr(this, "4", "State_SetupReadAdr", "6"),
    State_FlushAddr(this, "4", "State_FlushAddr", "8"),
    State_FlushCheck(this, "4", "State_FlushCheck", "9"),
    State_Reset(this, "4", "State_Reset", "10"),
    State_ResetWrite(this, "4", "State_ResetWrite", "11"),
    _3_(this),
    LINE_BYTES_MASK(this, "CFG_CPU_ADDR_BITS", "LINE_BYTES_MASK", "SUB(POW2(1,CFG_LOG2_L1CACHE_BYTES_PER_LINE),1)"),
    FLUSH_ALL_VALUE(this, "32", "FLUSH_ALL_VALUE", "SUB(POW2(1,ADD(CFG_ILOG2_LINES_PER_WAY,CFG_ILOG2_NWAYS)),1)", "Actual bitwidth is (ibits + waybits) but to avoid sc template generation use 32-bits"),
    // signals
    line_direct_access_i(this, "line_direct_access_i", "1"),
    line_invalidate_i(this, "line_invalidate_i", "1"),
    line_re_i(this, "line_re_i", "1"),
    line_we_i(this, "line_we_i", "1"),
    line_addr_i(this, "line_addr_i", "CFG_CPU_ADDR_BITS"),
    line_wdata_i(this, "line_wdata_i", "L1CACHE_LINE_BITS"),
    line_wstrb_i(this, "line_wstrb_i", "POW2(1,CFG_LOG2_L1CACHE_BYTES_PER_LINE)"),
    line_wflags_i(this, "line_wflags_i", "ITAG_FL_TOTAL"),
    line_raddr_o(this, "line_raddr_o", "CFG_CPU_ADDR_BITS"),
    line_rdata_o(this, "line_rdata_o", "ADD(L1CACHE_LINE_BITS,32)"),
    line_rflags_o(this, "line_rflags_o", "ITAG_FL_TOTAL"),
    line_hit_o(this, "line_hit_o", "1"),
    line_hit_next_o(this, "line_hit_next_o", "1"),
    // registers
    req_addr(this, "req_addr", "CFG_CPU_ADDR_BITS"),
    req_addr_next(this, "req_addr_next", "CFG_CPU_ADDR_BITS"),
    write_addr(this, "write_addr", "CFG_CPU_ADDR_BITS"),
    state(this, "state", "4", "State_Reset"),
    req_mem_valid(this, "req_mem_valid", "1"),
    mem_addr(this, "mem_addr", "CFG_CPU_ADDR_BITS"),
    req_mem_type(this, "req_mem_type", "REQ_MEM_TYPE_BITS"),
    req_mem_size(this, "req_mem_size", "3"),
    load_fault(this, "load_fault", "1"),
    req_flush(this, "req_flush", "1", "1", "init flush request"),
    req_flush_all(this, "req_flush_all", "1"),
    req_flush_addr(this, "req_flush_addr", "CFG_CPU_ADDR_BITS", "0", "[0]=1 flush all"),
    req_flush_cnt(this, "req_flush_cnt", "32"),
    flush_cnt(this, "flush_cnt", "32"),
    cache_line_i(this, "cache_line_i", "L1CACHE_LINE_BITS"),
    // process
    comb(this),
    mem0(this, "mem0")
{
    Operation::start(this);

    // Generic paramters to template parameters assignment
    mem0.abus.setObjValue(&abus);
    mem0.waybits.setObjValue(&glob_target_cfg_->CFG_ILOG2_NWAYS);
    mem0.ibits.setObjValue(&glob_target_cfg_->CFG_ILOG2_LINES_PER_WAY);
    mem0.lnbits.setObjValue(&lnbits);
    mem0.flbits.setObjValue(&flbits);
    NEW(mem0, mem0.getName().c_str());
        CONNECT(mem0, 0, mem0.i_clk, i_clk);
        CONNECT(mem0, 0, mem0.i_nrst, i_nrst);
        CONNECT(mem0, 0, mem0.i_direct_access, line_direct_access_i);
        CONNECT(mem0, 0, mem0.i_invalidate, line_invalidate_i);
        CONNECT(mem0, 0, mem0.i_re, line_re_i);
        CONNECT(mem0, 0, mem0.i_we, line_we_i);
        CONNECT(mem0, 0, mem0.i_addr, line_addr_i);
        CONNECT(mem0, 0, mem0.i_wdata, line_wdata_i);
        CONNECT(mem0, 0, mem0.i_wstrb, line_wstrb_i);
        CONNECT(mem0, 0, mem0.i_wflags, line_wflags_i);
        CONNECT(mem0, 0, mem0.o_raddr, line_raddr_o);
        CONNECT(mem0, 0, mem0.o_rdata, line_rdata_o);
        CONNECT(mem0, 0, mem0.o_rflags, line_rflags_o);
        CONNECT(mem0, 0, mem0.o_hit, line_hit_o);
        CONNECT(mem0, 0, mem0.o_hit_next, line_hit_next_o);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void ICacheLru::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;

    SETVAL(comb.sel_cached, TO_INT(BITS(req_addr, DEC(cfg->CFG_LOG2_L1CACHE_BYTES_PER_LINE), CONST("2"))));
    SETVAL(comb.sel_uncached, TO_INT(BITS(req_addr, 2, 2)));

    SETVAL(comb.vb_cached_data, BITSW(line_rdata_o, MUL2(CONST("32"), comb.sel_cached),
                                         CONST("64")));
    SETVAL(comb.vb_uncached_data, BITSW(cache_line_i, MUL2(CONST("32"), comb.sel_uncached),
                                             CONST("64")));

TEXT();
    TEXT("flush request via debug interface");
    IF (NZ(i_flush_valid));
        SETONE(req_flush);
        SETVAL(req_flush_all, BIT(i_flush_address, 0));
        IF (NZ(BIT(i_flush_address, 0)));
            SETVAL(req_flush_cnt, FLUSH_ALL_VALUE);
            SETZERO(req_flush_addr);
        ELSE();
            SETZERO(req_flush_cnt);
            SETVAL(req_flush_addr, i_flush_address);
        ENDIF();
    ENDIF();

TEXT();
    TEXT("Flush counter when direct access");
    IF (EQ(BITS(req_addr, DEC(glob_target_cfg_->CFG_ILOG2_NWAYS), CONST("0")), DEC(ways)));
        SETVAL(comb.vb_addr_direct_next, AND2_L(ADD2(req_addr, cfg->L1CACHE_BYTES_PER_LINE),
                                                INV_L(LINE_BYTES_MASK)));
    ELSE();
        SETVAL(comb.vb_addr_direct_next, INC(req_addr));
    ENDIF();

TEXT();
    SETVAL(comb.vb_line_addr, req_addr);
    SETVAL(comb.vb_line_wdata, cache_line_i);

TEXT();
    SWITCH (state);
    CASE(State_Idle);
        SETONE(comb.v_ready_next);
        ENDCASE();
    CASE(State_CheckHit);
        SETVAL(comb.vb_resp_data, comb.vb_cached_data);
        IF (AND2(NZ(line_hit_o), NZ(line_hit_next_o)));
            TEXT("Hit");
            SETONE(comb.v_resp_valid);
            IF (NZ(i_resp_ready));
                SETONE(comb.v_ready_next);
                SETVAL(state, State_Idle);
            ENDIF();
        ELSE();
            TEXT("Miss");
            SETVAL(state, State_TranslateAddress);
        ENDIF();
        ENDCASE();
    CASE(State_TranslateAddress);
        IF (EZ(i_pmp_x));
            SETZERO(comb.t_cache_line_i);
            SETVAL(cache_line_i, INV_L(comb.t_cache_line_i));
            SETVAL(state, State_CheckResp);
            SETONE(load_fault);
        ELSE();
            SETONE(req_mem_valid);
            SETVAL(state, State_WaitGrant);
            SETVAL(write_addr, req_addr);
            SETZERO(load_fault);

            TEXT();
            IF (NZ(i_pma_cached));
                IF (EZ(line_hit_o));
                    SETVAL(mem_addr, LSH(BITS(req_addr, DEC(cfg->CFG_CPU_ADDR_BITS),
                            cfg->CFG_LOG2_L1CACHE_BYTES_PER_LINE), cfg->CFG_LOG2_L1CACHE_BYTES_PER_LINE));
                ELSE();
                    SETVAL(write_addr, req_addr_next);
                    SETVAL(mem_addr, LSH(BITS(req_addr_next, DEC(cfg->CFG_CPU_ADDR_BITS),
                            cfg->CFG_LOG2_L1CACHE_BYTES_PER_LINE), cfg->CFG_LOG2_L1CACHE_BYTES_PER_LINE));
                ENDIF();
                CALLF(&req_mem_type, cfg->ReadShared, 0);
                SETVAL(req_mem_size, cfg->CFG_LOG2_L1CACHE_BYTES_PER_LINE);
            ELSE();
                SETVAL(mem_addr, CC2(BITS(req_addr, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("3")), CONST("0", 3)));
                CALLF(&req_mem_type, cfg->ReadNoSnoop, 0);
                SETVAL(req_mem_size, CONST("4", 3), "uncached, 16 B");
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(State_WaitGrant);
        IF (NZ(i_req_mem_ready));
            SETVAL(state, State_WaitResp);
            SETZERO(req_mem_valid);
        ENDIF();
        ENDCASE();
    CASE(State_WaitResp);
        IF (NZ(i_mem_data_valid));
            SETVAL(cache_line_i, i_mem_data);
            SETVAL(state, State_CheckResp);
            SETVAL(write_addr, req_addr, "Swap addres for 1 clock to write line");
            SETVAL(req_addr, write_addr);
            IF (NZ(i_mem_load_fault));
                SETONE(load_fault);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(State_CheckResp);
        SETVAL(req_addr, write_addr, "Restore req_addr after line write");
        IF (ORx(2, &EZ(BIT(req_mem_type, cfg->REQ_MEM_TYPE_CACHED)),
                   &NZ(load_fault)));
            SETONE(comb.v_resp_valid);
            SETVAL(comb.vb_resp_data, comb.vb_uncached_data);
            SETVAL(comb.v_resp_er_load_fault, load_fault);
            IF (NZ(i_resp_ready));
                SETVAL(state, State_Idle);
            ENDIF();
        ELSE();
            SETVAL(state, State_SetupReadAdr);
            SETONE(comb.v_line_cs_write);
            SETBITONE(comb.v_line_wflags, cfg->TAG_FL_VALID);
            SETVAL(comb.vb_line_wstrb, ALLONES(), "write full line");
        ENDIF();
        ENDCASE();
    CASE(State_SetupReadAdr);
        SETVAL(state, State_CheckHit);
        ENDCASE();
    CASE(State_FlushAddr);
        SETVAL(state, State_FlushCheck);
        SETVAL(comb.v_direct_access, req_flush_all, "0=only if hit; 1=will be applied ignoring hit");
        SETONE(comb.v_invalidate, "generate: wstrb='1; wflags='0");
        SETZERO(cache_line_i);
        ENDCASE();
    CASE(State_FlushCheck);
        SETVAL(state, State_FlushAddr);
        SETVAL(comb.v_direct_access, req_flush_all);
        SETVAL(comb.v_line_cs_write, req_flush_all);
        IF (NZ(flush_cnt));
            SETVAL(flush_cnt, DEC(flush_cnt));
            IF (NZ(req_flush_all));
                SETVAL(req_addr, comb.vb_addr_direct_next);
            ELSE();
                SETVAL(req_addr, ADD2(req_addr, cfg->L1CACHE_BYTES_PER_LINE));
            ENDIF();
        ELSE();
            SETVAL(state, State_Idle);
        ENDIF();
        ENDCASE();
    CASE(State_Reset);
        TEXT("Write clean line");
        IF (NZ(req_flush));
            SETZERO(req_flush);
            SETVAL(flush_cnt, FLUSH_ALL_VALUE, "Init after power-on-reset");
        ENDIF();
        SETONE(comb.v_direct_access);
        SETONE(comb.v_invalidate, "generate: wstrb='1; wflags='0");
        SETVAL(state, State_ResetWrite);
        ENDCASE();
    CASE(State_ResetWrite);
        SETONE(comb.v_direct_access);
        SETONE(comb.v_line_cs_write);
        SETVAL(state, State_Reset);

        TEXT();
        IF (NZ(flush_cnt));
            SETVAL(flush_cnt, DEC(flush_cnt));
            SETVAL(req_addr, comb.vb_addr_direct_next);
        ELSE();
            SETVAL(state, State_Idle);
        ENDIF();
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    IF (NZ(comb.v_ready_next));
        IF (NZ(req_flush));
            SETVAL(state, State_FlushAddr);
            SETZERO(req_flush);
            SETZERO(cache_line_i);
            SETVAL(req_addr, AND2_L(req_flush_addr, INV_L(LINE_BYTES_MASK)));
            SETVAL(flush_cnt, req_flush_cnt);
        ELSE();
            SETONE(comb.v_req_ready);
            SETVAL(comb.v_line_cs_read, i_req_valid);
            SETVAL(comb.vb_line_addr, i_req_addr);
            IF (NZ(i_req_valid));
                SETVAL(req_addr, i_req_addr);
                SETVAL(req_addr_next, ADD2(i_req_addr, cfg->L1CACHE_BYTES_PER_LINE));
                SETVAL(state, State_CheckHit);
            ENDIF();
        ENDIF();
    ENDIF();

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(line_direct_access_i, comb.v_direct_access);
    SETVAL(line_invalidate_i, comb.v_invalidate);
    SETVAL(line_re_i, comb.v_line_cs_read);
    SETVAL(line_we_i, comb.v_line_cs_write);
    SETVAL(line_addr_i, comb.vb_line_addr);
    SETVAL(line_wdata_i, comb.vb_line_wdata);
    SETVAL(line_wstrb_i, comb.vb_line_wstrb);
    SETVAL(line_wflags_i, comb.v_line_wflags);

TEXT();
    SETVAL(o_req_ready, comb.v_req_ready);
    SETVAL(o_req_mem_valid, req_mem_valid);
    SETVAL(o_req_mem_addr, mem_addr);
    SETVAL(o_req_mem_type, req_mem_type);
    SETVAL(o_req_mem_size, req_mem_size);
    SETZERO(o_req_mem_strob);
    SETZERO(o_req_mem_data);

TEXT();
    SETVAL(o_resp_valid, comb.v_resp_valid);
    SETVAL(o_resp_data, comb.vb_resp_data);
    SETVAL(o_resp_addr, req_addr);
    SETVAL(o_resp_load_fault, comb.v_resp_er_load_fault);
    SETVAL(o_mpu_addr, req_addr);
}