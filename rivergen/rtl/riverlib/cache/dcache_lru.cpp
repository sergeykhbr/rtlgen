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

#include "dcache_lru.h"

DCacheLru::DCacheLru(GenObject *parent, const char *name) :
    ModuleObject(parent, "DCacheLru", name),
    waybits(this, "waybits", "2", "Log2 of number of ways. Default 2: 4 ways"),
    ibits(this, "ibits", "7", "Log2 of number of lines per way: 7=16KB; 8=32KB; .. (if bytes per line = 32 B)"),
    coherence_ena(this, "coherence_ena", "false"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    _ctrl0_(this, "Data path:"),
    i_req_valid(this, "i_req_valid", "1"),
    i_req_type(this, "i_req_type", "MemopType_Total"),
    i_req_addr(this, "i_req_addr", "CFG_CPU_ADDR_BITS"),
    i_req_wdata(this, "i_req_wdata", "64"),
    i_req_wstrb(this, "i_req_wstrb", "8"),
    i_req_size(this, "i_req_size", "2"),
    o_req_ready(this, "o_req_ready", "1"),
    o_resp_valid(this, "o_resp_valid", "1"),
    o_resp_addr(this, "o_resp_addr", "CFG_CPU_ADDR_BITS"),
    o_resp_data(this, "o_resp_data", "64"),
    o_resp_er_load_fault(this, "o_resp_er_load_fault", "1"),
    o_resp_er_store_fault(this, "o_resp_er_store_fault", "1"),
    i_resp_ready(this, "i_resp_ready", "1"),
    _mem0_(this, "Memory interface:"),
    i_req_mem_ready(this, "i_req_mem_ready", "1"),
    o_req_mem_valid(this, "o_req_mem_valid", "1"),
    o_req_mem_type(this, "o_req_mem_type", "REQ_MEM_TYPE_BITS"),
    o_req_mem_size(this, "o_req_mem_size", "3"),
    o_req_mem_addr(this, "o_req_mem_addr", "CFG_CPU_ADDR_BITS"),
    o_req_mem_strob(this, "o_req_mem_strob", "L1CACHE_BYTES_PER_LINE"),
    o_req_mem_data(this, "o_req_mem_data", "L1CACHE_LINE_BITS"),
    i_mem_data_valid(this, "i_mem_data_valid", "1"),
    i_mem_data(this, "i_mem_data", "L1CACHE_LINE_BITS"),
    i_mem_load_fault(this, "i_mem_load_fault", "1"),
    i_mem_store_fault(this, "i_mem_store_fault", "1"),
    _mpu0(this, "Mpu interface"),
    o_mpu_addr(this, "o_mpu_addr", "CFG_CPU_ADDR_BITS"),
    i_pma_cached(this, "i_pma_cached", "1"),
    i_pmp_r(this, "i_pmp_r", "1", "PMP Read access"),
    i_pmp_w(this, "i_pmp_w", "1", "PMP Write access"),
    _dsnop0_(this, "D$ Snoop interface"),
    i_req_snoop_valid(this, "i_req_snoop_valid", "1"),
    i_req_snoop_type(this, "i_req_snoop_type", "SNOOP_REQ_TYPE_BITS"),
    o_req_snoop_ready(this, "o_req_snoop_ready", "1"),
    i_req_snoop_addr(this, "i_req_snoop_addr", "CFG_CPU_ADDR_BITS"),
    i_resp_snoop_ready(this, "i_resp_snoop_ready", "1"),
    o_resp_snoop_valid(this, "o_resp_snoop_valid", "1"),
    o_resp_snoop_data(this, "o_resp_snoop_data", "L1CACHE_LINE_BITS"),
    o_resp_snoop_flags(this, "o_resp_snoop_flags", "DTAG_FL_TOTAL"),
    _dbg0_(this, "Debug interface"),
    i_flush_address(this, "i_flush_address", "CFG_CPU_ADDR_BITS"),
    i_flush_valid(this, "i_flush_valid", "1"),
    o_flush_end(this, "o_flush_end", "1"),
    // params
    abus(this, "abus", "CFG_CPU_ADDR_BITS"),
//    waybits(this, "waybits", "CFG_DLOG2_NWAYS"),
//    ibits(this, "ibits", "CFG_DLOG2_LINES_PER_WAY"),
    lnbits(this, "lnbits", "CFG_LOG2_L1CACHE_BYTES_PER_LINE"),
    flbits(this, "flbits", "DTAG_FL_TOTAL"),
    ways(this, "ways", "POW2(1,waybits)"),
    State_Idle(this, "4", "State_Idle", "0"),
    State_CheckHit(this, "4", "State_CheckHit", "1"),
    State_TranslateAddress(this, "4", "State_TranslateAddress", "2"),
    State_WaitGrant(this, "4", "State_WaitGrant", "3"),
    State_WaitResp(this, "4", "State_WaitResp", "4"),
    State_CheckResp(this, "4", "State_CheckResp", "5"),
    State_SetupReadAdr(this, "4", "State_SetupReadAdr", "6"),
    State_WriteBus(this, "4", "State_WriteBus", "7"),
    State_FlushAddr(this, "4", "State_FlushAddr", "8"),
    State_FlushCheck(this, "4", "State_FlushCheck", "9"),
    State_Reset(this, "4", "State_Reset", "10"),
    State_ResetWrite(this, "4", "State_ResetWrite", "11"),
    State_SnoopSetupAddr(this, "4", "State_SnoopSetupAddr", "12"),
    State_SnoopReadData(this, "4", "State_SnoopReadData", "13"),
    LINE_BYTES_MASK(this, "CFG_CPU_ADDR_BITS", "LINE_BYTES_MASK", "SUB(POW2(1,CFG_LOG2_L1CACHE_BYTES_PER_LINE),1)"),
    // signals
    line_direct_access_i(this, "line_direct_access_i", "1"),
    line_invalidate_i(this, "line_invalidate_i", "1"),
    line_re_i(this, "line_re_i", "1"),
    line_we_i(this, "line_we_i", "1"),
    line_addr_i(this, "line_addr_i", "CFG_CPU_ADDR_BITS"),
    line_wdata_i(this, "line_wdata_i", "L1CACHE_LINE_BITS"),
    line_wstrb_i(this, "line_wstrb_i", "L1CACHE_BYTES_PER_LINE"),
    line_wflags_i(this, "line_wflags_i", "DTAG_FL_TOTAL"),
    line_raddr_o(this, "line_raddr_o", "CFG_CPU_ADDR_BITS"),
    line_rdata_o(this, "line_rdata_o", "L1CACHE_LINE_BITS"),
    line_rflags_o(this, "line_rflags_o", "DTAG_FL_TOTAL"),
    line_hit_o(this, "line_hit_o", "1"),
    _snoop1_(this, "Snoop signals:"),
    line_snoop_addr_i(this, "line_snoop_addr_i", "CFG_CPU_ADDR_BITS"),
    line_snoop_ready_o(this, "line_snoop_ready_o", "1"),
    line_snoop_flags_o(this, "line_snoop_flags_o", "DTAG_FL_TOTAL"),
    // registers
    req_type(this, "req_type", "MemopType_Total"),
    req_addr(this, "req_addr", "CFG_CPU_ADDR_BITS"),
    req_wdata(this, "req_wdata", "64"),
    req_wstrb(this, "req_wstrb", "8"),
    req_size(this, "req_size", "3"),
    state(this, "state", "4", "State_Reset"),
    req_mem_valid(this, "req_mem_valid", "1"),
    req_mem_type(this, "req_mem_type", "REQ_MEM_TYPE_BITS"),
    req_mem_size(this, "req_mem_size", "3"),
    mem_addr(this, "mem_addr", "CFG_CPU_ADDR_BITS"),
    load_fault(this, "load_fault", "1"),
    write_first(this, "write_first", "1"),
    write_flush(this, "write_flush", "1"),
    write_share(this, "write_share", "1"),
    mem_wstrb(this, "mem_wstrb", "L1CACHE_BYTES_PER_LINE"),
    req_flush(this, "req_flush", "1", "0", "init flush request"),
    req_flush_all(this, "req_flush_all", "1"),
    req_flush_addr(this, "req_flush_addr", "CFG_CPU_ADDR_BITS", "0", "[0]=1 flush all"),
    req_flush_cnt(this, "req_flush_cnt", "ADD(ibits,waybits)"),
    flush_cnt(this, "flush_cnt", "ADD(ibits,waybits)", "-1"),
    cache_line_i(this, "cache_line_i", "L1CACHE_LINE_BITS"),
    cache_line_o(this, "cache_line_o", "L1CACHE_LINE_BITS"),
    req_snoop_type(this, "req_snoop_type", "SNOOP_REQ_TYPE_BITS"),
    snoop_flags_valid(this, "snoop_flags_valid", "1"),
    snoop_restore_wait_resp(this, "snoop_restore_wait_resp", "1"),
    snoop_restore_write_bus(this, "snoop_restore_write_bus", "1"),
    req_addr_restore(this, "req_addr_restore", "CFG_CPU_ADDR_BITS"),
    // process
    comb(this),
    mem0(this, "mem0", "abus", "waybits", "ibits", "lnbits", "flbits", "1")
{
    Operation::start(this);

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
        CONNECT(mem0, 0, mem0.i_snoop_addr, line_snoop_addr_i);
        CONNECT(mem0, 0, mem0.o_snoop_ready, line_snoop_ready_o);
        CONNECT(mem0, 0, mem0.o_snoop_flags, line_snoop_flags_o);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void DCacheLru::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;
    GenObject *i;

    SETVAL(comb.t_req_type, req_type);
    SETVAL(comb.v_resp_snoop_valid, snoop_flags_valid);
    SETVAL(comb.ridx, BITS(req_addr, DEC(cfg->CFG_LOG2_L1CACHE_BYTES_PER_LINE), CONST("3")));

TEXT();
    SETVAL(comb.vb_cached_data, BITSW(line_rdata_o, MUL2(CONST("64"), TO_INT(comb.ridx)), CONST("64")));
    SETVAL(comb.vb_uncached_data, BIG_TO_U64(BITS(cache_line_i, 63, 0)));

TEXT();
    IF (EQ(BITS(req_addr, DEC(cfg->CFG_CPU_ADDR_BITS), cfg->CFG_LOG2_L1CACHE_BYTES_PER_LINE),
            BITS(i_req_addr, DEC(cfg->CFG_CPU_ADDR_BITS), cfg-> CFG_LOG2_L1CACHE_BYTES_PER_LINE)));
        SETONE(comb.v_req_same_line);
    ENDIF();

TEXT();
    IF (NZ(i_flush_valid));
        SETONE(req_flush);
        SETVAL(req_flush_all, BIT(i_flush_address, 0));
        IF (NZ(BIT(i_flush_address, 0)));
            SETVAL(req_flush_cnt, ALLONES());
            SETZERO(req_flush_addr);
        ELSE();
            SETZERO(req_flush_cnt);
            SETVAL(req_flush_addr, i_flush_address);
        ENDIF();
    ENDIF();

TEXT();
    i = &FOR ("i", CONST("0"), CONST("8"), "++");
        IF (NZ(BIT(req_wstrb, *i)));
            SETBITSW(comb.vb_req_mask, MUL2(CONST("8"), *i), CONST("8"), CONST("0xFF", 8));
        ENDIF();
    ENDFOR();

TEXT();
    SETVAL(comb.vb_line_rdata_o_modified, line_rdata_o);
    SETVAL(comb.vb_cache_line_i_modified, cache_line_i);
    i = &FOR("i", CONST("0"), DIV2(cfg->L1CACHE_BYTES_PER_LINE, CONST("8")), "++");
        IF (EQ(*i, TO_INT(comb.ridx)));
            SETBITSW(comb.vb_line_rdata_o_modified, MUL2(CONST("64"), *i), CONST("64"),
                ORx_L(2, &ANDx_L(2, &BITSW(comb.vb_line_rdata_o_modified, MUL2(CONST("64"), *i), CONST("64")),
                                    &INV_L(comb.vb_req_mask)),
                         &AND2_L(req_wdata, comb.vb_req_mask)));

            SETBITSW(comb.vb_cache_line_i_modified, MUL2(CONST("64"), *i), CONST("64"),
                ORx_L(2, &ANDx_L(2, &BITSW(comb.vb_cache_line_i_modified, MUL2(CONST("64"), *i), CONST("64")),
                                    &INV_L(comb.vb_req_mask)),
                         &AND2_L(req_wdata, comb.vb_req_mask)));

            SETBITSW(comb.vb_line_rdata_o_wstrb, MUL2(CONST("8"), *i), CONST("8"), req_wstrb);
        ENDIF();
    ENDFOR();

TEXT();
    TEXT("Flush counter when direct access");
    IF (EQ(BITS(req_addr, DEC(waybits), CONST("0")), DEC(ways)));
        SETVAL(comb.vb_addr_direct_next, ANDx_L(2, &ADD2(req_addr, cfg->L1CACHE_BYTES_PER_LINE),
                                                   &INV_L(LINE_BYTES_MASK)));
    ELSE();
        SETVAL(comb.vb_addr_direct_next, INC(req_addr));
    ENDIF();

TEXT();
    SETVAL(comb.vb_line_addr, req_addr);
    SETVAL(comb.vb_line_wdata, cache_line_i);

TEXT();
    TEXT("System Bus access state machine");
    SWITCH (state);
    CASE(State_Idle);
        SETZERO(load_fault);
        SETONE(comb.v_ready_next);
        ENDCASE();
    CASE(State_CheckHit);
        SETVAL(comb.vb_resp_data, comb.vb_cached_data);
        IF (NZ(line_hit_o));
            TEXT("Hit");
            SETONE(comb.v_resp_valid);
            IF (NZ(i_resp_ready));
                IF (NZ(BIT(req_type, cfg->MemopType_Store)));
                    TEXT("Modify tagged mem output with request and write back");
                    SETONE(comb.v_line_cs_write);
                    SETBITONE(comb.v_line_wflags, cfg->TAG_FL_VALID);
                    SETBITONE(comb.v_line_wflags, cfg->DTAG_FL_DIRTY);
                    SETBITZERO(comb.v_line_wflags, cfg->DTAG_FL_RESERVED);
                    SETBITZERO(comb.t_req_type, cfg->MemopType_Store);
                    SETVAL(req_type, comb.t_req_type, "clear MemopType_Store bit");
                    SETVAL(comb.vb_line_wstrb, comb.vb_line_rdata_o_wstrb);
                    SETVAL(comb.vb_line_wdata, comb.vb_line_rdata_o_modified);
                    IF (NZ(BIT(req_type, cfg->MemopType_Release)));
                        SETZERO(comb.vb_resp_data);
                    ENDIF();
                    IF (ANDx(2, &NZ(BIT(req_type, cfg->MemopType_Release)),
                                &EZ(BIT(line_rflags_o, cfg->DTAG_FL_RESERVED))));
                        TEXT("ignore writing if cacheline wasn't reserved before:");
                        SETZERO(comb.v_line_cs_write);
                        SETZERO(comb.vb_line_wstrb);
                        SETONE(comb.vb_resp_data, "return error");
                        SETVAL(state, State_Idle);
                    ELSE();
                        IF (AND2(coherence_ena, NZ(BIT(line_rflags_o, cfg->DTAG_FL_SHARED))));
                            TEXT("Make line: 'shared' -> 'unique' using write request");
                            SETONE(write_share);
                            SETVAL(state, State_TranslateAddress);
                        ELSE();
                            IF (NZ(comb.v_req_same_line));
                                TEXT("Write address is the same as the next requested, so use it to write");
                                TEXT("value and update state machine");
                                SETONE(comb.v_ready_next);
                            ENDIF();
                            SETVAL(state, State_Idle);
                        ENDIF();
                    ENDIF();
                ELSIF (ANDx(2, &EZ(BIT(req_type, cfg->MemopType_Store)),
                               &NZ(BIT(req_type, cfg->MemopType_Reserve))));
                    TEXT("Load with reserve");
                    SETONE(comb.v_line_cs_write);
                    SETVAL(comb.v_line_wflags, line_rflags_o);
                    SETBITONE(comb.v_line_wflags, cfg->DTAG_FL_RESERVED);
                    SETVAL(comb.vb_line_wdata, comb.vb_cached_data);
                    SETVAL(comb.vb_line_wstrb, ALLONES(), "flags will be modified only if wstrb != 0");
                    SETVAL(state, State_Idle);
                ELSE();
                    SETONE(comb.v_ready_next);
                    SETVAL(state, State_Idle);
                ENDIF();
            ENDIF();
        ELSE();
            TEXT("Miss");
            IF (ANDx(2, &NZ(BIT(req_type, cfg->MemopType_Store)),
                        &NZ(BIT(req_type, cfg->MemopType_Release))));
                SETONE(comb.vb_resp_data, "return error. Cannot store into unreserved cache line");
                SETVAL(state, State_Idle);
            ELSE();
                SETVAL(state, State_TranslateAddress);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(State_TranslateAddress);
        IF (AND2(NZ(BIT(req_type, cfg->MemopType_Store)), EZ(i_pmp_w)));
            SETONE(load_fault);
            SETZERO(comb.t_cache_line_i);
            SETVAL(cache_line_i, INV_L(comb.t_cache_line_i));
            SETVAL(state, State_CheckResp);
        ELSIF (AND2(EZ(BIT(req_type, cfg->MemopType_Store)), EZ(i_pmp_r)));
            SETONE(load_fault);
            SETZERO(comb.t_cache_line_i);
            SETVAL(cache_line_i, INV_L(comb.t_cache_line_i));
            SETVAL(state, State_CheckResp);
        ELSE();
            SETONE(req_mem_valid);
            SETZERO(load_fault);
            SETVAL(state, State_WaitGrant);
            IF (NZ(i_pma_cached));
                TEXT("Cached:");
                IF (NZ(write_share));
                    CALLF(&req_mem_type, cfg->WriteLineUnique, 0);
                    SETVAL(mem_addr, LSH(BITS(line_raddr_o, DEC(cfg->CFG_CPU_ADDR_BITS),
                                cfg->CFG_LOG2_L1CACHE_BYTES_PER_LINE), cfg->CFG_LOG2_L1CACHE_BYTES_PER_LINE));
                ELSIF (ANDx(2, &NZ(BIT(line_rflags_o, cfg->TAG_FL_VALID)),
                               &NZ(BIT(line_rflags_o, cfg->DTAG_FL_DIRTY))));
                    SETONE(write_first);
                    CALLF(&req_mem_type, cfg->WriteBack, 0);
                    SETVAL(mem_addr, LSH(BITS(line_raddr_o, DEC(cfg->CFG_CPU_ADDR_BITS),
                                cfg->CFG_LOG2_L1CACHE_BYTES_PER_LINE), cfg->CFG_LOG2_L1CACHE_BYTES_PER_LINE));
                ELSE();
                    TEXT("1. Read -> Save cache");
                    TEXT("2. Read -> Modify -> Save cache");
                    SETVAL(mem_addr, LSH(BITS(req_addr, DEC(cfg->CFG_CPU_ADDR_BITS),
                                cfg->CFG_LOG2_L1CACHE_BYTES_PER_LINE), cfg->CFG_LOG2_L1CACHE_BYTES_PER_LINE));
                    IF (NZ(BIT(req_type, cfg->MemopType_Store)));
                        CALLF(&req_mem_type, cfg->ReadMakeUnique, 0);
                    ELSE();
                        CALLF(&req_mem_type, cfg->ReadShared, 0);
                    ENDIF();
                ENDIF();
                SETVAL(req_mem_size, cfg->CFG_LOG2_L1CACHE_BYTES_PER_LINE);
                SETVAL(mem_wstrb, ALLONES());
                SETVAL(cache_line_o, line_rdata_o);
            ELSE();
                TEXT("Uncached read/write");
                SETVAL(mem_addr, req_addr);
                SETVAL(mem_wstrb, CC2(ALLZEROS(), req_wstrb));
                SETVAL(req_mem_size, req_size);
                IF (NZ(BIT(req_type, cfg->MemopType_Store)));
                    CALLF(&req_mem_type, cfg->WriteNoSnoop, 0);
                ELSE();
                    CALLF(&req_mem_type, cfg->ReadNoSnoop, 0);
                ENDIF();
                SETZERO(comb.t_cache_line_i);
                SETBITS(comb.t_cache_line_i, 63, 0, req_wdata);
                SETVAL(cache_line_o, comb.t_cache_line_i);
            ENDIF();
        ENDIF();
        SETZERO(cache_line_i);
        ENDCASE();
    CASE(State_WaitGrant);
        IF (NZ(i_req_mem_ready));
            IF (ORx(4, &NZ(write_flush),
                       &NZ(write_first),
                       &NZ(write_share),
                       &ANDx(2, &NZ(BIT(req_type, cfg->MemopType_Store)),
                                &EZ(BIT(req_mem_type, cfg->REQ_MEM_TYPE_CACHED)))));
                SETVAL(state, State_WriteBus);
            ELSE();
                TEXT("1. uncached read");
                TEXT("2. cached read or write");
                SETVAL(state, State_WaitResp);
            ENDIF();
            SETZERO(req_mem_valid);
        ENDIF();
        ENDCASE();
    CASE(State_WaitResp);
        IF (NZ(i_mem_data_valid));
            SETVAL(cache_line_i, i_mem_data);
            SETVAL(state, State_CheckResp);
            IF (NZ(i_mem_load_fault));
                SETONE(load_fault);
            ENDIF();
        ELSIF (ANDx(2, &coherence_ena,
                       &AND2(NZ(i_req_snoop_valid), NZ(i_req_snoop_type))));
            TEXT("Access cache data");
            SETONE(comb.v_req_snoop_ready_on_wait);
            SETONE(snoop_restore_wait_resp);
            SETVAL(req_addr_restore, req_addr);
            SETVAL(req_addr, i_req_snoop_addr);
            SETVAL(req_snoop_type, i_req_snoop_type);
            SETVAL(state, State_SnoopSetupAddr);
        ENDIF();
        ENDCASE();
    CASE(State_CheckResp);
        IF (ORx(2, &EZ(BIT(req_mem_type, cfg->REQ_MEM_TYPE_CACHED)),
                   &NZ(load_fault)));
            TEXT("uncached read only (write goes to WriteBus) or cached load-modify fault");
            SETONE(comb.v_resp_valid);
            SETVAL(comb.vb_resp_data, comb.vb_uncached_data);
            SETVAL(comb.v_resp_er_store_fault, AND2(load_fault, BIT(req_type, cfg->MemopType_Store)));
            SETVAL(comb.v_resp_er_load_fault, AND2(load_fault, INV(BIT(req_type, cfg->MemopType_Store))));
            IF (NZ(i_resp_ready));
                SETVAL(state, State_Idle);
            ENDIF();
        ELSE();
            SETVAL(state, State_SetupReadAdr);
            SETONE(comb.v_line_cs_write);
            SETBITONE(comb.v_line_wflags, cfg->TAG_FL_VALID);
            SETBITONE(comb.v_line_wflags, cfg->DTAG_FL_SHARED);
            SETBIT(comb.v_line_wflags, cfg->DTAG_FL_RESERVED, BIT(req_type, cfg->MemopType_Reserve));
            SETVAL(comb.vb_line_wstrb, ALLONES(), "write full line");
            IF (NZ(BIT(req_type, cfg->MemopType_Store)));
                TEXT("Modify tagged mem output with request before write");
                SETBITZERO(comb.t_req_type, cfg->MemopType_Store);
                SETVAL(req_type, comb.t_req_type, "clear MemopType_Store bit");
                SETBITONE(comb.v_line_wflags, cfg->DTAG_FL_DIRTY);
                SETBITZERO(comb.v_line_wflags, cfg->DTAG_FL_SHARED);
                SETBITZERO(comb.v_line_wflags, cfg->DTAG_FL_RESERVED);
                SETVAL(comb.vb_line_wdata, comb.vb_cache_line_i_modified);
                SETONE(comb.v_resp_valid);
                SETVAL(state, State_Idle);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(State_SetupReadAdr);
        SETVAL(state, State_CheckHit);
        ENDCASE();
    CASE(State_WriteBus);
        IF (NZ(i_mem_data_valid));
            IF (NZ(write_share));
                SETZERO(write_share);
                SETVAL(state, State_Idle);
            ELSIF (NZ(write_flush));
                TEXT("Offloading Cache line on flush request");
                SETVAL(state, State_FlushAddr);
            ELSIF (NZ(write_first));
                TEXT("Obsolete line was offloaded, now read new line");
                SETVAL(mem_addr, LSH(BITS(req_addr, DEC(cfg->CFG_CPU_ADDR_BITS), cfg->CFG_LOG2_L1CACHE_BYTES_PER_LINE),
                                cfg->CFG_LOG2_L1CACHE_BYTES_PER_LINE));
                SETONE(req_mem_valid);
                SETZERO(write_first);
                IF (NZ(BIT(req_type, cfg->MemopType_Store)));
                    TEXT("read request: read-modify-save cache line");
                    CALLF(&req_mem_type, cfg->ReadMakeUnique, 0);
                ELSE();
                    CALLF(&req_mem_type, cfg->ReadShared, 0);
                ENDIF();
                SETVAL(state, State_WaitGrant);
            ELSE();
                TEXT("Non-cached write");
                SETVAL(state, State_Idle);
                SETONE(comb.v_resp_valid);
                SETVAL(comb.v_resp_er_store_fault, i_mem_store_fault);
            ENDIF();
        ELSIF (ANDx(2, &coherence_ena,
                       &AND2(NZ(i_req_snoop_valid), NZ(i_req_snoop_type))));
            TEXT("Access cache data cannot be in the same clock as i_mem_data_valid");
            SETONE(comb.v_req_snoop_ready_on_wait);
            SETONE(snoop_restore_write_bus);
            SETVAL(req_addr_restore, req_addr);
            SETVAL(req_addr, i_req_snoop_addr);
            SETVAL(req_snoop_type, i_req_snoop_type);
            SETVAL(state, State_SnoopSetupAddr);
        ENDIF();
        ENDCASE();
    CASE(State_FlushAddr);
        SETVAL(state, State_FlushCheck);
        SETVAL(comb.v_direct_access, req_flush_all, "0=only if hit; 1=will be applied ignoring hit");
        SETONE(comb.v_invalidate, "generate: wstrb='1; wflags='0");
        SETZERO(write_flush);
        SETZERO(cache_line_i);
        ENDCASE();
    CASE(State_FlushCheck);
        SETVAL(cache_line_o, line_rdata_o);
        SETVAL(comb.v_direct_access, req_flush_all);
        SETVAL(comb.v_line_cs_write, req_flush_all);
        IF (ANDx(2, &NZ(BIT(line_rflags_o, cfg->TAG_FL_VALID)),
                    &NZ(BIT(line_rflags_o, cfg->DTAG_FL_DIRTY))));
            TEXT("Off-load valid line");
            SETONE(write_flush);
            SETVAL(mem_addr, line_raddr_o);
            SETONE(req_mem_valid);
            CALLF(&req_mem_type, cfg->WriteBack, 0);
            SETVAL(mem_wstrb, ALLONES());
            SETVAL(state, State_WaitGrant);
        ELSE();
            TEXT("Write clean line");
            SETVAL(state, State_FlushAddr);
            IF (EZ(flush_cnt));
                SETVAL(state, State_Idle);
                SETONE(comb.v_flush_end);
            ENDIF();
        ENDIF();
        IF (NZ(flush_cnt));
            SETVAL(flush_cnt, DEC(flush_cnt));
            IF (NZ(req_flush_all));
                SETVAL(req_addr, comb.vb_addr_direct_next);
            ELSE();
                SETVAL(req_addr, ADD2(req_addr, cfg->L1CACHE_BYTES_PER_LINE));
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(State_Reset);
        TEXT("Write clean line");
        SETONE(comb.v_direct_access);
        SETONE(comb.v_invalidate, "generate: wstrb='1; wflags='0");
        SETVAL(state, State_ResetWrite);
        ENDCASE();
    CASE(State_ResetWrite);
        SETONE(comb.v_direct_access);
        SETONE(comb.v_line_cs_write);
        SETVAL(state, State_Reset);
        IF (NZ(flush_cnt));
            SETVAL(flush_cnt, DEC(flush_cnt));
            SETVAL(req_addr, comb.vb_addr_direct_next);
        ELSE();
            SETVAL(state, State_Idle);
        ENDIF();
        ENDCASE();
    CASE(State_SnoopSetupAddr);
        SETVAL(state, State_SnoopReadData);
        SETVAL(comb.v_invalidate, BIT(req_snoop_type, cfg->SNOOP_REQ_TYPE_READCLEAN)); 
        ENDCASE();
    CASE(State_SnoopReadData);
        SETONE(comb.v_resp_snoop_valid);
        IF (EZ(BIT(req_snoop_type, cfg->SNOOP_REQ_TYPE_READCLEAN)));
            SETONE(comb.v_line_cs_write);
            SETVAL(comb.vb_line_wdata, line_rdata_o);
            SETVAL(comb.vb_line_wstrb, ALLONES());
            SETVAL(comb.v_line_wflags, line_rflags_o);
            SETBITZERO(comb.v_line_wflags, cfg->DTAG_FL_DIRTY);
            SETBITONE(comb.v_line_wflags, cfg->DTAG_FL_SHARED);
            SETBITZERO(comb.v_line_wflags, cfg->DTAG_FL_RESERVED);
        ENDIF();
        TEXT("restore state");
        SETZERO(snoop_restore_wait_resp);
        SETZERO(snoop_restore_write_bus);
        IF (NZ(snoop_restore_wait_resp));
            SETVAL(req_addr, req_addr_restore);
            SETVAL(state, State_WaitResp);
        ELSIF (NZ(snoop_restore_write_bus));
            SETVAL(req_addr, req_addr_restore);
            SETVAL(state, State_WriteBus);
        ELSE();
            SETVAL(state, State_Idle);
        ENDIF();
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    SETVAL(comb.v_req_snoop_ready,
            ORx(3, &AND2(line_snoop_ready_o, INV(OR_REDUCE(i_req_snoop_type))),
                   &AND3(coherence_ena, comb.v_ready_next, OR_REDUCE(i_req_snoop_type)),
                   &comb.v_req_snoop_ready_on_wait));

TEXT();
    SETVAL(snoop_flags_valid, ANDx(3, &i_req_snoop_valid,
                                      &line_snoop_ready_o,
                                      &INV(OR_REDUCE(i_req_snoop_type))));

TEXT();
    IF (NZ(comb.v_ready_next));
        IF (ANDx(2, &coherence_ena,
                    &AND2(NZ(i_req_snoop_valid), NZ(i_req_snoop_type))));
            TEXT("Access cache data");
            SETVAL(req_addr, i_req_snoop_addr);
            SETVAL(req_snoop_type, i_req_snoop_type);
            SETVAL(state, State_SnoopSetupAddr);
        ELSIF (NZ(req_flush));
            SETVAL(state, State_FlushAddr);
            SETZERO(req_flush);
            SETZERO(cache_line_i);
            SETVAL(req_addr, AND2_L(req_flush_addr, INV_L(LINE_BYTES_MASK)));
            SETVAL(req_mem_size, cfg->CFG_LOG2_L1CACHE_BYTES_PER_LINE);
            SETVAL(flush_cnt, req_flush_cnt);
        ELSE();
            SETONE(comb.v_req_ready);
            SETVAL(comb.v_line_cs_read, i_req_valid);
            SETVAL(comb.vb_line_addr, i_req_addr);
            IF (NZ(i_req_valid));
                SETVAL(req_addr, i_req_addr);
                SETVAL(req_wstrb, i_req_wstrb);
                SETVAL(req_size, CC2(CONST("0", 1), i_req_size));
                SETVAL(req_wdata, i_req_wdata);
                SETVAL(req_type, i_req_type);
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
    SETVAL(line_snoop_addr_i, i_req_snoop_addr);

TEXT();
    SETVAL(o_req_ready, comb.v_req_ready);
    SETVAL(o_req_mem_valid, req_mem_valid);
    SETVAL(o_req_mem_addr, mem_addr);
    SETVAL(o_req_mem_type, req_mem_type);
    SETVAL(o_req_mem_size, req_mem_size);
    SETVAL(o_req_mem_strob, mem_wstrb);
    SETVAL(o_req_mem_data, cache_line_o);

TEXT();
    SETVAL(o_resp_valid, comb.v_resp_valid);
    SETVAL(o_resp_data, comb.vb_resp_data);
    SETVAL(o_resp_addr, req_addr);
    SETVAL(o_resp_er_load_fault, comb.v_resp_er_load_fault);
    SETVAL(o_resp_er_store_fault, comb.v_resp_er_store_fault);
    SETVAL(o_mpu_addr, req_addr);

TEXT();
    SETVAL(o_req_snoop_ready, comb.v_req_snoop_ready);
    SETVAL(o_resp_snoop_valid, comb.v_resp_snoop_valid);
    SETVAL(o_resp_snoop_data, line_rdata_o);
    SETVAL(o_resp_snoop_flags, line_snoop_flags_o);

TEXT();
    SETVAL(o_flush_end, comb.v_flush_end);
}