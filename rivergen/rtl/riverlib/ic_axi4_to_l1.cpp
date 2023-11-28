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

#include "ic_axi4_to_l1.h"

ic_axi4_to_l1::ic_axi4_to_l1(GenObject *parent, const char *name) :
    ModuleObject(parent, "ic_axi4_to_l1", name),
    // Ports
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    _axi4_(this, "AXI4 port"),
    i_xmsto(this, "i_xmsto"),
    o_xmsti(this, "o_xmsti"),
    _l1_(this, "L1 port"),
    i_l1i(this, "i_l1i"),
    o_l1o(this, "o_l1o"),
    // params:
    Idle(this, "4", "Idle", "0", "axi ar_ready=1,aw_ready=1"),
    ReadLineRequest(this, "4", "ReadLineRequest", "1", "l1 ar_valid=1"),
    WaitReadLineResponse(this, "4", "WaitReadLineResponse", "2", "l1 r_ready=1"),
    WriteDataAccept(this, "4", "WriteDataAccept", "3", "axi w_ready=1"),
    WriteLineRequest(this, "4", "WriteLineRequest", "4", "l1 w_valid=1"),
    WaitWriteConfirmResponse(this, "4", "WaitWriteConfirmResponse", "5", "l1 b_ready"),
    WaitWriteAccept(this, "4", "WaitWriteAccept", "6", "axi b_valid"),
    WaitReadAccept(this, "4", "WaitReadAccept", "7", "axi r_valid"),
    CheckBurst(this, "4", "CheckBurst", "8"),
    // registers
    state(this, "state", "4", "Idle"),
    req_addr(this, "req_addr", "CFG_SYSBUS_ADDR_BITS"),
    req_id(this, "req_id", "CFG_SYSBUS_ID_BITS"),
    req_user(this, "req_user", "CFG_SYSBUS_USER_BITS"),
    req_wstrb(this, "req_wstrb", "8"),
    req_wdata(this, "req_wdata", "64"),
    req_len(this, "req_len", "8"),
    req_size(this, "req_size", "3"),
    req_prot(this, "req_prot", "3"),
    writing(this, "writing", "1"),
    read_modify_write(this, "read_modify_write", "1"),
    line_data(this, "line_data", "L1CACHE_LINE_BITS"),
    line_wstrb(this, "line_wstrb", "L1CACHE_BYTES_PER_LINE"),
    resp_data(this, "resp_data", "64"),
    comb(this)
{
    Operation::start(this);


    Operation::start(&comb);
    proc_comb();
}

void ic_axi4_to_l1::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;
    types_river *river = glob_types_river_;
    types_amba *amba = glob_types_amba_;
    GenObject *i;

    SETVAL(comb.vb_xmsti, amba->axi4_master_in_none);
    SETVAL(comb.vb_l1o, river->axi4_l1_out_none);
    SETVAL(comb.t_req_addr, req_addr);

TEXT();
    SETVAL(comb.idx, BITS(req_addr, DEC(cfg->CFG_LOG2_L1CACHE_BYTES_PER_LINE), CONST("3")));
    CALLF(&comb.vb_req_xbytes, amba->XSizeToBytes, 1, &req_size);

TEXT();
    SETZERO(comb.vb_req_mask);
    i = &FOR ("i",CONST("0"), CONST("8"), "++");
        IF (NZ(BIT(req_wstrb, *i)));
            SETBITSW(comb.vb_req_mask, MUL2(CONST("8"), *i), CONST("8"), CONST("0xFF", 8));
        ENDIF();
    ENDFOR();

TEXT();
    SETVAL(comb.vb_resp_data, BITSW(i_l1i.r_data, MUL2(comb.idx, CONST("64")), CONST("64")));

TEXT();
    SETVAL(comb.vb_r_data_modified, i_l1i.r_data);
    SETBITSW(comb.vb_r_data_modified, MUL2(comb.idx, CONST("64")), CONST("64"),
                    ORx_L(2, &AND2_L(BITSW(i_l1i.r_data, MUL2(comb.idx, CONST("64")), CONST("64")),
                                     INV_L(comb.vb_req_mask)),
                             &AND2_L(req_wdata, comb.vb_req_mask)));

TEXT();
    SETZERO(comb.vb_line_wstrb);
    SETBITSW(comb.vb_line_wstrb, MUL2(comb.idx,CONST("8")), CONST("8"), req_wstrb);

TEXT();
    SWITCH (state);
    CASE(Idle);
        SETONE(comb.vb_xmsti.ar_ready);
        SETONE(comb.vb_xmsti.aw_ready);
        SETZERO(read_modify_write);
        SETZERO(writing);
        IF (NZ(i_xmsto.aw_valid));
            TEXT("Convert AXI4 to AXI4Lite used in L1");
            SETVAL(req_id, i_xmsto.aw_id);
            SETVAL(req_user, i_xmsto.aw_user);
            SETVAL(req_addr, i_xmsto.aw_bits.addr);
            SETVAL(req_size, i_xmsto.aw_bits.size);
            SETVAL(req_len, i_xmsto.aw_bits.len);
            SETVAL(req_prot, i_xmsto.aw_bits.prot);
            SETONE(writing);
            SETVAL(state, WriteDataAccept);
        ELSIF (NZ(i_xmsto.ar_valid));
            SETVAL(req_id, i_xmsto.ar_id);
            SETVAL(req_user, i_xmsto.ar_user);
            SETVAL(req_addr, i_xmsto.ar_bits.addr);
            SETVAL(req_size, i_xmsto.ar_bits.size);
            SETVAL(req_len, i_xmsto.ar_bits.len);
            SETVAL(req_prot, i_xmsto.ar_bits.prot);
            SETVAL(state, ReadLineRequest);
        ENDIF();
    ENDCASE();
    CASE(WriteDataAccept);
        SETONE(comb.vb_xmsti.w_ready);
        SETVAL(req_wdata, i_xmsto.w_data);
        SETVAL(req_wstrb, i_xmsto.w_strb);
        IF (NZ(i_xmsto.w_valid));
            TEXT("Cachable memory read and make unique to modify line");
            SETONE(read_modify_write);
            SETVAL(state, ReadLineRequest);
        ENDIF();
    ENDCASE();
    CASE(ReadLineRequest);
        SETONE(comb.vb_l1o.ar_valid);
        SETVAL(comb.vb_l1o.ar_bits.addr, LSH(BITS(req_addr, DEC(amba->CFG_SYSBUS_ADDR_BITS), cfg->CFG_LOG2_L1CACHE_BYTES_PER_LINE),
                                            cfg->CFG_LOG2_L1CACHE_BYTES_PER_LINE));
        SETVAL(comb.vb_l1o.ar_bits.cache, amba->ARCACHE_WRBACK_READ_ALLOCATE);
        SETVAL(comb.vb_l1o.ar_bits.size, cfg->CFG_LOG2_L1CACHE_BYTES_PER_LINE);
        SETZERO(comb.vb_l1o.ar_bits.len);
        SETVAL(comb.vb_l1o.ar_bits.prot, req_prot);
        SETVAL(comb.vb_l1o.ar_snoop, amba->ARSNOOP_READ_MAKE_UNIQUE);
        SETVAL(comb.vb_l1o.ar_id, req_id);
        SETVAL(comb.vb_l1o.ar_user, req_user);
        IF (NZ(i_l1i.ar_ready));
            SETVAL(state, WaitReadLineResponse);
        ENDIF();
    ENDCASE();
    CASE(WaitReadLineResponse);
        SETONE(comb.vb_l1o.r_ready);
        SETVAL(line_data, i_l1i.r_data);
        SETVAL(resp_data, comb.vb_resp_data);
        IF (NZ(i_l1i.r_valid));
            IF (NZ(read_modify_write));
                SETVAL(line_data, comb.vb_r_data_modified);
                SETVAL(line_wstrb, ALLONES());
                SETVAL(state, WriteLineRequest);
            ELSE();
                SETVAL(state, WaitReadAccept);
            ENDIF();
        ENDIF();
    ENDCASE();
    CASE(WriteLineRequest);
        SETONE(comb.vb_l1o.aw_valid);
        SETVAL(comb.vb_l1o.aw_bits.addr, LSH(BITS(req_addr, DEC(amba->CFG_SYSBUS_ADDR_BITS), cfg->CFG_LOG2_L1CACHE_BYTES_PER_LINE),
                                            cfg->CFG_LOG2_L1CACHE_BYTES_PER_LINE));
        SETVAL(comb.vb_l1o.aw_bits.cache, amba->AWCACHE_DEVICE_NON_BUFFERABLE);
        SETVAL(comb.vb_l1o.aw_bits.size, cfg->CFG_LOG2_L1CACHE_BYTES_PER_LINE);
        SETZERO(comb.vb_l1o.aw_bits.len);
        SETVAL(comb.vb_l1o.aw_bits.prot, req_prot);
        SETVAL(comb.vb_l1o.aw_snoop, amba->AWSNOOP_WRITE_NO_SNOOP, "offloading non-cached always");
        SETVAL(comb.vb_l1o.aw_id, req_id);
        SETVAL(comb.vb_l1o.aw_user, req_user);
        TEXT("axi lite for L2-cache");
        SETONE(comb.vb_l1o.w_valid);
        SETONE(comb.vb_l1o.w_last);
        SETVAL(comb.vb_l1o.w_data, line_data);
        SETVAL(comb.vb_l1o.w_strb, line_wstrb);
        IF (AND2(NZ(i_l1i.aw_ready), NZ(i_l1i.w_ready)));
            IF (EZ(req_len));
                SETVAL(state, WaitWriteConfirmResponse);
            ELSE();
                SETVAL(state, CheckBurst);
            ENDIF();
        ENDIF();
    ENDCASE();
    CASE(WaitWriteConfirmResponse);
        SETONE(comb.vb_l1o.b_ready);
        IF (NZ(i_l1i.b_valid));
            SETVAL(state, WaitWriteAccept);
        ENDIF();
    ENDCASE();
    CASE(WaitWriteAccept);
        SETONE(comb.vb_xmsti.b_valid);
        SETVAL(comb.vb_xmsti.b_id, req_id);
        SETVAL(comb.vb_xmsti.b_user, req_user);
        IF (NZ(i_xmsto.b_ready));
            SETVAL(state, Idle);
        ENDIF();
    ENDCASE();
    CASE(WaitReadAccept);
        SETONE(comb.vb_xmsti.r_valid);
        SETVAL(comb.vb_xmsti.r_data, resp_data);
        SETVAL(comb.vb_xmsti.r_last, INV_L(OR_REDUCE(req_len)));
        SETVAL(comb.vb_xmsti.r_id, req_id);
        SETVAL(comb.vb_xmsti.r_user, req_user);
        IF (NZ(i_xmsto.r_ready));
            SETVAL(state, CheckBurst);
        ENDIF();
    ENDCASE();
    CASE(CheckBurst);
        IF (EZ(req_len));
            SETVAL(state, Idle);
        ELSE();
            TEXT("Burst transaction to support external DMA engine");
            SETVAL(req_len, DEC(req_len));
            SETBITS(comb.t_req_addr, 11, 0, ADD2(BITS(req_addr, 11, 0), comb.vb_req_xbytes));
            SETVAL(req_addr, comb.t_req_addr);
            SETZERO(read_modify_write);
            IF (NZ(writing));
                SETVAL(state, WriteDataAccept);
            ELSE();
                SETVAL(state, ReadLineRequest);
            ENDIF();
        ENDIF();
    ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    SETVAL(o_xmsti, comb.vb_xmsti);
    SETVAL(o_l1o, comb.vb_l1o);
}
