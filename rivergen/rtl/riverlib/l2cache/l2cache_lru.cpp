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

#include "l2cache_lru.h"

L2CacheLru::L2CacheLru(GenObject *parent, const char *name) :
    ModuleObject(parent, "L2CacheLru", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_req_valid(this, "i_req_valid", "1"),
    i_req_type(this, "i_req_type", "L2_REQ_TYPE_BITS"),
    i_req_size(this, "i_req_size", "3"),
    i_req_prot(this, "i_req_prot", "3"),
    i_req_addr(this, "i_req_addr", "CFG_CPU_ADDR_BITS"),
    i_req_wdata(this, "i_req_wdata", "L1CACHE_LINE_BITS"),
    i_req_wstrb(this, "i_req_wstrb", "L1CACHE_BYTES_PER_LINE"),
    o_req_ready(this, "o_req_ready", "1"),
    o_resp_valid(this, "o_resp_valid", "1"),
    o_resp_rdata(this, "o_resp_rdata", "L1CACHE_LINE_BITS"),
    o_resp_status(this, "o_resp_status", "2"),
    _mem0_(this, "Memory interface:"),
    i_req_mem_ready(this, "i_req_mem_ready", "1"),
    o_req_mem_valid(this, "o_req_mem_valid", "1"),
    o_req_mem_type(this, "o_req_mem_type", "REQ_MEM_TYPE_BITS"),
    o_req_mem_size(this, "o_req_mem_size", "3"),
    o_req_mem_prot(this, "o_req_mem_prot", "3"),
    o_req_mem_addr(this, "o_req_mem_addr", "CFG_CPU_ADDR_BITS"),
    o_req_mem_strob(this, "o_req_mem_strob", "L2CACHE_BYTES_PER_LINE"),
    o_req_mem_data(this, "o_req_mem_data", "L2CACHE_LINE_BITS"),
    i_mem_data_valid(this, "i_mem_data_valid", "1"),
    i_mem_data(this, "i_mem_data", "L2CACHE_LINE_BITS"),
    i_mem_data_ack(this, "i_mem_data_ack", "1"),
    i_mem_load_fault(this, "i_mem_load_fault", "1"),
    i_mem_store_fault(this, "i_mem_store_fault", "1"),
    _flush0_(this, "Flush interface"),
    i_flush_address(this, "i_flush_address", "CFG_CPU_ADDR_BITS"),
    i_flush_valid(this, "i_flush_valid", "1"),
    o_flush_end(this, "o_flush_end", "1"),
    // params
    abus(this, "abus", "CFG_CPU_ADDR_BITS"),
    waybits(this, "waybits", "CFG_L2_LOG2_NWAYS"),
    ibits(this, "ibits", "CFG_L2_LOG2_LINES_PER_WAY"),
    lnbits(this, "lnbits", "CFG_L2_LOG2_BYTES_PER_LINE"),
    flbits(this, "flbits", "L2TAG_FL_TOTAL"),
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
    LINE_BYTES_MASK(this, "CFG_CPU_ADDR_BITS", "LINE_BYTES_MASK", "SUB(POW2(1,CFG_L2_LOG2_BYTES_PER_LINE),1)"),
    // signals
    line_direct_access_i(this, "line_direct_access_i", "1"),
    line_invalidate_i(this, "line_invalidate_i", "1"),
    line_re_i(this, "line_re_i", "1"),
    line_we_i(this, "line_we_i", "1"),
    line_addr_i(this, "line_addr_i", "CFG_CPU_ADDR_BITS"),
    line_wdata_i(this, "line_wdata_i", "L2CACHE_LINE_BITS"),
    line_wstrb_i(this, "line_wstrb_i", "L2CACHE_BYTES_PER_LINE"),
    line_wflags_i(this, "line_wflags_i", "L2TAG_FL_TOTAL"),
    line_raddr_o(this, "line_raddr_o", "CFG_CPU_ADDR_BITS"),
    line_rdata_o(this, "line_rdata_o", "L2CACHE_LINE_BITS"),
    line_rflags_o(this, "line_rflags_o", "L2TAG_FL_TOTAL"),
    line_hit_o(this, "line_hit_o", "1"),
    _snoop1_(this, "Snoop signals:"),
    line_snoop_addr_i(this, "line_snoop_addr_i", "CFG_CPU_ADDR_BITS"),
    line_snoop_ready_o(this, "line_snoop_ready_o", "1"),
    line_snoop_flags_o(this, "line_snoop_flags_o", "L2TAG_FL_TOTAL"),
    // registers
    req_type(this, "req_type", "L2_REQ_TYPE_BITS"),
    req_size(this, "req_size", "3"),
    req_prot(this, "req_prot", "3"),
    req_addr(this, "req_addr", "CFG_CPU_ADDR_BITS"),
    req_wdata(this, "req_wdata", "L1CACHE_LINE_BITS"),
    req_wstrb(this, "req_wstrb", "L1CACHE_BYTES_PER_LINE"),
    state(this, "state", "4", "State_Reset"),
    req_mem_valid(this, "req_mem_valid", "1"),
    req_mem_type(this, "req_mem_type", "REQ_MEM_TYPE_BITS"),
    mem_addr(this, "mem_addr", "CFG_CPU_ADDR_BITS"),
    rb_resp(this, "rb_resp", "2", "0", "r_resp, b_resp"),
    write_first(this, "write_first", "1"),
    write_flush(this, "write_flush", "1"),
    mem_wstrb(this, "mem_wstrb", "L2CACHE_BYTES_PER_LINE"),
    req_flush(this, "req_flush", "1", "0", "init flush request"),
    req_flush_all(this, "req_flush_all", "1"),
    req_flush_addr(this, "req_flush_addr", "CFG_CPU_ADDR_BITS", "0", "[0]=1 flush all"),
    req_flush_cnt(this, "req_flush_cnt", "ADD(CFG_L2_LOG2_LINES_PER_WAY,CFG_L2_LOG2_NWAYS)"),
    flush_cnt(this, "flush_cnt", "ADD(CFG_L2_LOG2_LINES_PER_WAY,CFG_L2_LOG2_NWAYS)", "-1"),
    cache_line_i(this, "cache_line_i", "L2CACHE_LINE_BITS"),
    cache_line_o(this, "cache_line_o", "L2CACHE_LINE_BITS"),
    // process
    comb(this),
    mem0(this, "mem0", "abus", "waybits", "ibits", "lnbits", "flbits", "0")
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

void L2CacheLru::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;
    GenObject *i;

    SETVAL(comb.vb_req_type, req_type, "systemc specific");
    IF (NE(cfg->L2CACHE_LINE_BITS, cfg->L1CACHE_LINE_BITS));
        SETVAL(comb.ridx, TO_INT(BITS(req_addr, DEC(cfg->CFG_L2_LOG2_BYTES_PER_LINE),
                                      SUB2(cfg->CFG_L2_LOG2_BYTES_PER_LINE, cfg->CFG_DLOG2_BYTES_PER_LINE))));
    ENDIF();

TEXT();
    SETVAL(comb.vb_cached_data, BITSW(line_rdata_o, MUL2(comb.ridx, cfg->L1CACHE_LINE_BITS),
                                                    cfg->L1CACHE_LINE_BITS));
    SETVAL(comb.vb_uncached_data, BITS(cache_line_i, DEC(cfg->L1CACHE_LINE_BITS), CONST("0")));

TEXT();
    IF (EQ(BITS(req_addr, DEC(cfg->CFG_CPU_ADDR_BITS), cfg->CFG_L2_LOG2_BYTES_PER_LINE),
            BITS(i_req_addr, DEC(cfg->CFG_CPU_ADDR_BITS), cfg->CFG_L2_LOG2_BYTES_PER_LINE)));
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
    i = &FOR ("i", CONST("0"), cfg->L1CACHE_BYTES_PER_LINE, "++");
        IF (NZ(BIT(req_wstrb, *i)));
            SETBITSW(comb.vb_req_mask, MUL2(CONST("8"), *i), CONST("8"), CONST("0xFF", 8));
        ENDIF();
    ENDFOR();

TEXT();
    SETVAL(comb.vb_line_rdata_o_modified, line_rdata_o);
    SETVAL(comb.vb_cache_line_i_modified, cache_line_i);
    i = &FOR ("i", CONST("0"), DIV2(cfg->L2CACHE_BYTES_PER_LINE, cfg->L1CACHE_BYTES_PER_LINE), "++");
        IF (EQ(*i, comb.ridx));
            SETBITSW(comb.vb_line_rdata_o_modified, MUL2(cfg->L1CACHE_LINE_BITS, *i), cfg->L1CACHE_LINE_BITS,
                ORx_L(2, &ANDx_L(2, &BITSW(comb.vb_line_rdata_o_modified, MUL2(cfg->L1CACHE_LINE_BITS, *i), cfg->L1CACHE_LINE_BITS),
                                    &INV_L(comb.vb_req_mask)),
                         &AND2_L(req_wdata, comb.vb_req_mask)));

            SETBITSW(comb.vb_cache_line_i_modified, MUL2(cfg->L1CACHE_LINE_BITS, *i), cfg->L1CACHE_LINE_BITS,
                ORx_L(2, &ANDx_L(2, &BITSW(comb.vb_cache_line_i_modified, MUL2(cfg->L1CACHE_LINE_BITS, *i), cfg->L1CACHE_LINE_BITS),
                                    &INV_L(comb.vb_req_mask)),
                         &AND2_L(req_wdata, comb.vb_req_mask)));

            SETBITSW(comb.vb_line_rdata_o_wstrb, MUL2(cfg->L1CACHE_BYTES_PER_LINE, *i), cfg->L1CACHE_BYTES_PER_LINE, req_wstrb);
        ENDIF();
    ENDFOR();

TEXT();
    TEXT("Flush counter when direct access");
    IF (EQ(BITS(req_addr, DEC(cfg->CFG_L2_LOG2_NWAYS), CONST("0")), DEC(cfg->L2CACHE_WAYS)));
        SETVAL(comb.vb_addr_direct_next, ANDx_L(2, &ADD2(req_addr, cfg->L2CACHE_BYTES_PER_LINE),
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
        SETONE(comb.v_ready_next);
        ENDCASE();
    CASE(State_CheckHit);
        IF (NZ(line_hit_o));
            TEXT("Hit");
            SETONE(comb.v_resp_valid);
            SETVAL(comb.vb_resp_rdata, comb.vb_cached_data);
            SETVAL(comb.vb_resp_status, rb_resp);
            IF (EZ(BIT(req_type, cfg->L2_REQ_TYPE_CACHED)));
                TEXT("Warning: This is a wrong case possible if MPU region changed");
                TEXT("         without proper cache flushing.");
                SETZERO(comb.v_line_cs_write);
                SETONE(comb.v_invalidate);
                SETVAL(state, State_TranslateAddress);
            ELSIF (NZ(BIT(req_type, cfg->L2_REQ_TYPE_UNIQUE)));
                SETZERO(comb.v_line_cs_write);
                SETONE(comb.v_invalidate);
            ELSIF (NZ(BIT(req_type, cfg->L2_REQ_TYPE_WRITE)));
                TEXT("Modify tagged mem output with request and write back");
                SETONE(comb.v_line_cs_write);
                SETBITONE(comb.v_line_wflags, cfg->TAG_FL_VALID);
                SETBITONE(comb.v_line_wflags, cfg->L2TAG_FL_DIRTY);
                SETBITZERO(comb.vb_req_type, cfg->L2_REQ_TYPE_WRITE);
                SETVAL(req_type, comb.vb_req_type);
                SETVAL(comb.vb_line_wstrb, comb.vb_line_rdata_o_wstrb);
                SETVAL(comb.vb_line_wdata, comb.vb_line_rdata_o_modified);
                IF (NZ(comb.v_req_same_line));
                    TEXT("Write address is the same as the next requested, so use it to write");
                    TEXT("value and update state machine");
                    SETONE(comb.v_ready_next);
                ENDIF();
            ELSE();
                SETONE(comb.v_ready_next);
            ENDIF();
            SETVAL(state, State_Idle);
        ELSE();
            TEXT("Miss");
            IF (ANDx(2, &NZ(BIT(req_type, cfg->L2_REQ_TYPE_WRITE)),
                        &NZ(BIT(req_type, cfg->L2_REQ_TYPE_UNIQUE))));
                TEXT("This command analog of invalidate line");
                TEXT("no need to read it form memory");
                SETVAL(state, State_Idle);
                SETONE(comb.v_resp_valid);
            ELSIF (ANDx(3, &EZ(BIT(req_type, cfg->L2_REQ_TYPE_WRITE)),
                           &EZ(BIT(req_type, cfg->L2_REQ_TYPE_UNIQUE)),
                           &NZ(BIT(req_type, cfg->L2_REQ_TYPE_SNOOP))));
                SETVAL(state, State_Idle);
                SETONE(comb.v_resp_valid);

                TEXT();
                TEXT("Save into cache read via Snoop channel data");
                SETONE(comb.v_line_cs_write);
                SETBITONE(comb.v_line_wflags, cfg->TAG_FL_VALID);
                SETBITONE(comb.v_line_wflags, cfg->L2TAG_FL_DIRTY);
                SETVAL(comb.vb_line_wstrb, comb.vb_line_rdata_o_wstrb);
                SETVAL(comb.vb_line_wdata, comb.vb_line_rdata_o_modified);
            ELSE();
                SETVAL(state, State_TranslateAddress);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(State_TranslateAddress);
        SETONE(req_mem_valid);
        SETVAL(state, State_WaitGrant);

        TEXT();
        IF (NZ(BIT(req_type, cfg->L2_REQ_TYPE_CACHED)));
            IF (ANDx(2, &NZ(BIT(line_rflags_o, cfg->TAG_FL_VALID)),
                        &NZ(BIT(line_rflags_o, cfg->L2TAG_FL_DIRTY))));
                SETONE(write_first);
                CALLF(&req_mem_type, cfg->WriteBack, 0);
                SETVAL(mem_addr, LSH(BITS(line_raddr_o, DEC(cfg->CFG_CPU_ADDR_BITS),
                            cfg->CFG_L2_LOG2_BYTES_PER_LINE), cfg->CFG_L2_LOG2_BYTES_PER_LINE));
            ELSE();
                SETVAL(mem_addr, LSH(BITS(req_addr, DEC(cfg->CFG_CPU_ADDR_BITS),
                            cfg->CFG_L2_LOG2_BYTES_PER_LINE), cfg->CFG_L2_LOG2_BYTES_PER_LINE));
                IF (NZ(BIT(req_type, cfg->L2_REQ_TYPE_WRITE)));
                    CALLF(&req_mem_type, cfg->ReadMakeUnique, 0);
                ELSE();
                    CALLF(&req_mem_type, cfg->ReadShared, 0);
                ENDIF();
            ENDIF();
            SETVAL(mem_wstrb, ALLONES());
            SETVAL(cache_line_o, line_rdata_o);
        ELSE();
            SETVAL(mem_addr, req_addr);
            SETVAL(mem_wstrb, CC2(ALLZEROS(), req_wstrb));
            IF (NZ(BIT(req_type, cfg->L2_REQ_TYPE_WRITE)));
                CALLF(&req_mem_type, cfg->WriteNoSnoop, 0);
            ELSE();
                CALLF(&req_mem_type, cfg->ReadNoSnoop, 0);
            ENDIF();
            SETBITS(comb.t_cache_line_i, 63, 0, req_wdata);
            SETVAL(cache_line_o, comb.t_cache_line_i);
        ENDIF();

        TEXT();
        SETZERO(cache_line_i);
        SETZERO(rb_resp);
        ENDCASE();
    CASE(State_WaitGrant);
        IF (NZ(i_req_mem_ready));
            IF (ORx(3, &NZ(write_flush),
                       &NZ(write_first),
                       &ANDx(2, &NZ(BIT(req_type, cfg->L2_REQ_TYPE_WRITE)),
                                &EZ(BIT(req_type, cfg->L2_REQ_TYPE_CACHED)))));
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
                SETVAL(rb_resp, CONST("0x2", 2), "SLVERR");
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(State_CheckResp);
        IF (ORx(2, &EZ(BIT(req_mem_type, cfg->REQ_MEM_TYPE_CACHED)),
                   &NZ(BIT(rb_resp, 1))));
            TEXT("uncached read only (write goes to WriteBus) or cached load-modify fault");
            SETONE(comb.v_resp_valid);
            SETVAL(comb.vb_resp_rdata, comb.vb_uncached_data);
            SETVAL(comb.vb_resp_status, rb_resp);
            SETVAL(state, State_Idle);
        ELSE();
            SETVAL(state, State_SetupReadAdr);
            SETONE(comb.v_line_cs_write);
            SETBITONE(comb.v_line_wflags, cfg->TAG_FL_VALID);
            SETVAL(comb.vb_line_wstrb, ALLONES(), "write full line");
            IF (NZ(BIT(req_type, cfg->L2_REQ_TYPE_WRITE)));
                TEXT("Modify tagged mem output with request before write");
                SETBITZERO(comb.vb_req_type, cfg->L2_REQ_TYPE_WRITE);
                SETVAL(req_type, comb.vb_req_type);
                SETBITONE(comb.v_line_wflags, cfg->L2TAG_FL_DIRTY);
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
            IF (NZ(write_flush));
                TEXT("Offloading Cache line on flush request");
                SETVAL(state, State_FlushAddr);
            ELSIF (NZ(write_first));
                SETVAL(mem_addr, LSH(BITS(req_addr, DEC(cfg->CFG_CPU_ADDR_BITS), cfg->CFG_L2_LOG2_BYTES_PER_LINE),
                                     cfg->CFG_L2_LOG2_BYTES_PER_LINE));
                SETONE(req_mem_valid);
                SETZERO(write_first);
                IF (NZ(BIT(req_type, cfg->L2_REQ_TYPE_WRITE)));
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
                SETBIT(comb.vb_resp_status, 1, i_mem_store_fault, "rb_resp");
            ENDIF();
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
                    &NZ(BIT(line_rflags_o, cfg->L2TAG_FL_DIRTY))));
            TEXT("Off-load valid line");
            SETONE(write_flush);
            SETVAL(mem_addr, line_raddr_o);
            SETONE(req_mem_valid);
            CALLF(&req_mem_type, cfg->WriteBack, 0);
            SETVAL(req_size, cfg->CFG_L2_LOG2_BYTES_PER_LINE);
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
                SETVAL(req_addr, ADD2(req_addr, cfg->L2CACHE_BYTES_PER_LINE));
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
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    IF (NZ(comb.v_ready_next));
        IF (NZ(req_flush));
            SETVAL(state, State_FlushAddr);
            SETZERO(req_flush);
            SETZERO(cache_line_i);
            SETVAL(req_addr, AND2_L(req_flush_addr, INV(LINE_BYTES_MASK)));
            SETVAL(req_size, cfg->CFG_L2_LOG2_BYTES_PER_LINE);
            SETVAL(flush_cnt, req_flush_cnt);
        ELSE();
            SETVAL(comb.v_line_cs_read, i_req_valid);
            SETONE(comb.v_req_ready);
            SETVAL(comb.vb_line_addr, i_req_addr);
            IF (NZ(i_req_valid));
                SETVAL(req_addr, i_req_addr);
                SETVAL(req_wstrb, i_req_wstrb);
                SETVAL(req_wdata, i_req_wdata);
                SETVAL(req_type, i_req_type);
                SETVAL(req_size, i_req_size);
                SETVAL(req_prot, i_req_prot);
                SETZERO(rb_resp, "RESP OK");
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
    SETVAL(o_req_mem_type, req_mem_type);
    SETVAL(o_req_mem_size, req_size);
    SETVAL(o_req_mem_prot, req_prot);
    SETVAL(o_req_mem_addr, mem_addr);
    SETVAL(o_req_mem_strob, mem_wstrb);
    SETVAL(o_req_mem_data, cache_line_o);

TEXT();
    TEXT("always 1 clock messages");
    SETVAL(o_resp_valid, comb.v_resp_valid);
    SETVAL(o_resp_rdata, comb.vb_resp_rdata);
    SETVAL(o_resp_status, comb.vb_resp_status);
    SETVAL(o_flush_end, comb.v_flush_end);
}