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

#include "axi_dma.h"

axi_dma::axi_dma(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "axi_dma", name, comment),
    // Generic parameters
    abits(this, "abits", "48", "adress bits used"),
    userbits(this, "userbits", "1"),
    // Ports
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    o_req_mem_ready(this, "o_req_mem_ready", "1", "Ready to accept next data"),
    i_req_mem_valid(this, "i_req_mem_valid", "1", "Request data is ready to accept"),
    i_req_mem_write(this, "i_req_mem_write", "1", "0=read; 1=write operation"),
    i_req_mem_bytes(this, "i_req_mem_bytes", "10", "0=1024 B; 4=DWORD; 8=QWORD; ..."),
    i_req_mem_addr(this, "i_req_mem_addr", "abits", "Address to read/write"),
    i_req_mem_strob(this, "i_req_mem_strob", "8", "Byte enabling write strob"),
    i_req_mem_data(this, "i_req_mem_data", "64", "Data to write"),
    i_req_mem_last(this, "i_req_mem_last", "1", "Last data payload in a sequence"),
    o_resp_mem_valid(this, "o_resp_mem_valid", "1", "Read/Write data is valid. All write transaction with valid response."),
    o_resp_mem_last(this, "o_resp_mem_last", "1", "Last response in a sequence."),
    o_resp_mem_fault(this, "o_resp_mem_fault", "1", "Error on memory access"),
    o_resp_mem_addr(this, "o_resp_mem_addr", "abits", "Read address value"),
    o_resp_mem_data(this, "o_resp_mem_data", "64", "Read data value"),
    i_resp_mem_ready(this, "i_resp_mem_ready", "1", "Ready to accept response"),
    i_msti(this, "i_msti", "AXI master input"),
    o_msto(this, "o_msto", "AXI master output"),
    o_dbg_valid(this, "o_dbg_valid", "1"),
    o_dbg_payload(this, "o_dbg_payload", "64"),
    // params
    state_idle(this, "state_idle", "3", "0", NO_COMMENT),
    state_ar(this, "state_ar", "3", "1", NO_COMMENT),
    state_r(this, "state_r", "3", "2", NO_COMMENT),
    state_r_wait_accept(this, "state_r_wait_accept", "3", "3", NO_COMMENT),
    state_aw(this, "state_aw", "3", "4", NO_COMMENT),
    state_w(this, "state_w", "3", "5", NO_COMMENT),
    state_w_wait_accept(this, "state_w_wait_accept", "3", "6", NO_COMMENT),
    state_b(this, "state_b", "3", "7", NO_COMMENT),
    // Singals:
    // registers
    state(this, "state", "3", "state_idle", NO_COMMENT),
    ar_valid(this, "ar_valid", "1", RSTVAL_ZERO, NO_COMMENT),
    aw_valid(this, "aw_valid", "1", RSTVAL_ZERO, NO_COMMENT),
    w_valid(this, "w_valid", "1", RSTVAL_ZERO, NO_COMMENT),
    r_ready(this, "r_ready", "1", RSTVAL_ZERO, NO_COMMENT),
    b_ready(this, "b_ready", "1", RSTVAL_ZERO, NO_COMMENT),
    req_addr(this, "req_addr", "CFG_SYSBUS_ADDR_BITS", "'0", NO_COMMENT),
    req_wdata(this, "req_wdata", "CFG_SYSBUS_DATA_BITS", "'0", NO_COMMENT),
    req_wstrb(this, "req_wstrb", "CFG_SYSBUS_DATA_BYTES", "'0", NO_COMMENT),
    req_size(this, "req_size", "3", "'0", NO_COMMENT),
    req_len(this, "req_len", "8", "'0", NO_COMMENT),
    req_last(this, "req_last", "1", RSTVAL_ZERO, NO_COMMENT),
    req_ready(this, "req_ready", "1", "1", NO_COMMENT),
    resp_valid(this, "resp_valid", "1", "'0", NO_COMMENT),
    resp_last(this, "resp_last", "1", "'0", NO_COMMENT),
    resp_addr(this, "resp_addr", "CFG_SYSBUS_ADDR_BITS", "'0", NO_COMMENT),
    resp_data(this, "resp_data", "CFG_SYSBUS_DATA_BITS", "'0", NO_COMMENT),
    resp_error(this, "resp_error", "1", RSTVAL_ZERO, NO_COMMENT),
    user_count(this, "user_count", "CFG_SYSBUS_USER_BITS", "'0", NO_COMMENT),
    dbg_valid(this, "dbg_valid", "1", "0", NO_COMMENT),
    dbg_payload(this, "dbg_payload", "64", "'0", NO_COMMENT),
    // functions
    // submodules:
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void axi_dma::proc_comb() {
    SETVAL(comb.vb_req_mem_bytes_m1, DEC(i_req_mem_bytes));
    SETVAL(comb.vb_req_addr_inc, req_addr);

    TEXT();
    TEXT("Byte swapping:");
    IF (EQ(req_size, CONST("0", 3)));
        SETBITS(comb.vb_req_addr_inc, 9, 0, ADD2(BITS(req_addr, 9, 0), CONST("0x1", 10)));
        IF (EQ(BITS(req_addr, 2, 0), CONST("0", 3)));
            SETBITS(comb.vb_r_data_swap, 31, 0, CC4(BITS(i_msti.r_data, 7, 0),
                                             BITS(i_msti.r_data, 7, 0),
                                             BITS(i_msti.r_data, 7, 0),
                                             BITS(i_msti.r_data, 7, 0)));
        ELSIF (EQ(BITS(req_addr, 2, 0), CONST("1", 3)));
            SETBITS(comb.vb_r_data_swap, 31, 0, CC4(BITS(i_msti.r_data, 15, 8),
                                             BITS(i_msti.r_data, 15, 8),
                                             BITS(i_msti.r_data, 15, 8),
                                             BITS(i_msti.r_data, 15, 8)));
        ELSIF (EQ(BITS(req_addr, 2, 0), CONST("2", 3)));
            SETBITS(comb.vb_r_data_swap, 31, 0, CC4(BITS(i_msti.r_data, 23, 16),
                                             BITS(i_msti.r_data, 23, 16),
                                             BITS(i_msti.r_data, 23, 16),
                                             BITS(i_msti.r_data, 23, 16)));
        ELSIF (EQ(BITS(req_addr, 2, 0), CONST("3", 3)));
            SETBITS(comb.vb_r_data_swap, 31, 0, CC4(BITS(i_msti.r_data, 31, 24),
                                             BITS(i_msti.r_data, 31, 24),
                                             BITS(i_msti.r_data, 31, 24),
                                             BITS(i_msti.r_data, 31, 24)));
        ELSIF (EQ(BITS(req_addr, 2, 0), CONST("4", 3)));
            SETBITS(comb.vb_r_data_swap, 31, 0, CC4(BITS(i_msti.r_data, 39, 32),
                                             BITS(i_msti.r_data, 39, 32),
                                             BITS(i_msti.r_data, 39, 32),
                                             BITS(i_msti.r_data, 39, 32)));
        ELSIF (EQ(BITS(req_addr, 2, 0), CONST("5", 3)));
            SETBITS(comb.vb_r_data_swap, 31, 0, CC4(BITS(i_msti.r_data, 47, 40),
                                             BITS(i_msti.r_data, 47, 40),
                                             BITS(i_msti.r_data, 47, 40),
                                             BITS(i_msti.r_data, 47, 40)));
        ELSIF (EQ(BITS(req_addr, 2, 0), CONST("6", 3)));
            SETBITS(comb.vb_r_data_swap, 31, 0, CC4(BITS(i_msti.r_data, 55, 48),
                                             BITS(i_msti.r_data, 55, 48),
                                             BITS(i_msti.r_data, 55, 48),
                                             BITS(i_msti.r_data, 55, 48)));
        ELSE();
            SETBITS(comb.vb_r_data_swap, 31, 0, CC4(BITS(i_msti.r_data, 63, 56),
                                             BITS(i_msti.r_data, 63, 56),
                                             BITS(i_msti.r_data, 63, 56),
                                             BITS(i_msti.r_data, 63, 56)));
        ENDIF();
        SETBITS(comb.vb_r_data_swap, 63, 32, BITS(comb.vb_r_data_swap, 31, 0));
    ELSIF (EQ(req_size, CONST("1", 3)));
        SETBITS(comb.vb_req_addr_inc, 9, 0, ADD2(BITS(req_addr, 9, 0), CONST("0x2", 10)));
        IF (EQ(BITS(req_addr, 2, 1), CONST("0", 2)));
            SETVAL(comb.vb_r_data_swap, CC4(BITS(i_msti.r_data, 15, 0),
                                            BITS(i_msti.r_data, 15, 0),
                                            BITS(i_msti.r_data, 15, 0),
                                            BITS(i_msti.r_data, 15, 0)));
        ELSIF (EQ(BITS(req_addr, 2, 1), CONST("1", 2)));
            SETVAL(comb.vb_r_data_swap, CC4(BITS(i_msti.r_data, 31, 16),
                                            BITS(i_msti.r_data, 31, 16),
                                            BITS(i_msti.r_data, 31, 16),
                                            BITS(i_msti.r_data, 31, 16)));
        ELSIF (EQ(BITS(req_addr, 2, 1), CONST("2", 2)));
            SETVAL(comb.vb_r_data_swap, CC4(BITS(i_msti.r_data, 47, 32),
                                            BITS(i_msti.r_data, 47, 32),
                                            BITS(i_msti.r_data, 47, 32),
                                            BITS(i_msti.r_data, 47, 32)));
        ELSE();
            SETVAL(comb.vb_r_data_swap, CC4(BITS(i_msti.r_data, 63, 48),
                                            BITS(i_msti.r_data, 63, 48),
                                            BITS(i_msti.r_data, 63, 48),
                                            BITS(i_msti.r_data, 63, 48)));
        ENDIF();
    ELSIF (EQ(req_size, CONST("2", 3)));
        SETBITS(comb.vb_req_addr_inc, 9, 0, ADD2(BITS(req_addr, 9, 0), CONST("0x4", 10)));
        IF (EZ(BIT(req_addr, 2)));
            SETVAL(comb.vb_r_data_swap, CC2(BITS(i_msti.r_data, 31, 0),
                                            BITS(i_msti.r_data, 31, 0)));
        ELSE();
            SETVAL(comb.vb_r_data_swap, CC2(BITS(i_msti.r_data, 63, 32),
                                            BITS(i_msti.r_data, 63, 32)));
        ENDIF();
    ELSE();
        SETBITS(comb.vb_req_addr_inc, 9, 0, ADD2(BITS(req_addr, 9, 0), CONST("0x8", 10)));
        SETVAL(comb.vb_r_data_swap, i_msti.r_data);
    ENDIF();

TEXT();
    SETZERO(dbg_valid);
    SWITCH (state);
    CASE(state_idle);
        SETONE(req_ready);
        SETZERO(resp_valid);
        SETZERO(resp_last);
        IF (NZ(i_req_mem_valid));
            SETZERO(req_ready);
            SETVAL(req_addr, CC2(ALLZEROS(), i_req_mem_addr));
            IF (EQ(i_req_mem_bytes, CONST("1", 10)));
                SETVAL(req_size, CONST("0", 3));
                SETZERO(req_len);
            ELSIF (EQ(i_req_mem_bytes, CONST("2", 10)));
                SETVAL(req_size, CONST("1", 3));
                SETZERO(req_len);
            ELSIF (EQ(i_req_mem_bytes, CONST("4", 10)));
                SETVAL(req_size, CONST("2", 3));
                SETZERO(req_len);
            ELSE();
                SETVAL(req_size, CONST("3", 3));
                SETVAL(req_len, CC2(CONST("0", 1), BITS(comb.vb_req_mem_bytes_m1, 9, 3)));
            ENDIF();
            IF (EZ(i_req_mem_write));
                SETONE(ar_valid);
                SETVAL(state, state_ar);
                SETZERO(req_wdata);
                SETZERO(req_wstrb);
                SETZERO(req_last);
            ELSE();
                SETONE(aw_valid);
                SETVAL(w_valid, i_req_mem_last, "Try to use AXI Lite");
                SETVAL(req_wdata, i_req_mem_data);
                SETVAL(req_wstrb, i_req_mem_strob);
                SETVAL(req_last, i_req_mem_last);
                SETVAL(state, state_aw);
                SETONE(dbg_valid);
            ENDIF();
            TEXT("debug interface:");
            SETVAL(dbg_payload, CCx(6, &CONST("0x1", 1),            // [63]
                                    &BITS(i_req_mem_addr, 10, 0),   // [62:52]
                                    &CONST("0x0", 2),               // [51:50]
                                    &i_req_mem_bytes,               // [49:40]
                                    &i_req_mem_strob,               // [39:32]
                                    &BITS(i_req_mem_data, 31, 0)));
        ENDIF();
        ENDCASE();
    CASE(state_ar);
        IF (NZ(i_msti.ar_ready));
            SETVAL(resp_addr, req_addr);
            SETZERO(ar_valid);
            SETONE(r_ready);
            SETVAL(state, state_r);
        ENDIF();
        ENDCASE();
    CASE(state_r);
        IF (NZ(i_msti.r_valid));
            SETONE(resp_valid);
            SETVAL(resp_addr, req_addr);
            SETVAL(resp_data, comb.vb_r_data_swap);
            SETVAL(resp_last, i_msti.r_last);
            SETVAL(resp_error, BIT(i_msti.r_resp, 1));
            SETVAL(req_addr, comb.vb_req_addr_inc);

            TEXT();
            IF (OR2(EZ(i_resp_mem_ready), NZ(i_msti.r_last)));
                SETZERO(r_ready);
                SETVAL(state, state_r_wait_accept);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(state_r_wait_accept);
        IF (NZ(i_resp_mem_ready));
            SETZERO(resp_valid);
            TEXT("debug interface:");
            SETONE(dbg_valid);
            SETVAL(dbg_payload, CC3(CONST("0x0", 1),                // [63]
                                    BITS(dbg_payload, 62, 32),      // [62:32]
                                    BITS(resp_data, 31, 0)));

            TEXT();
            IF (NZ(resp_last));
                SETZERO(resp_last);
                SETVAL(user_count, INC(user_count));
                SETONE(req_ready);
                SETVAL(state, state_idle);
            ELSE();
                SETONE(r_ready);
                SETVAL(state, state_r);
            ENDIF();
        ENDIF();
        ENDCASE();

    TEXT();
    CASE(state_aw);
        IF (NZ(i_msti.aw_ready));
            SETZERO(aw_valid);
            SETVAL(state, state_w);
            SETVAL(resp_addr, req_addr);

            TEXT();
            IF (AND2(w_valid, NZ(i_msti.w_ready)));
                TEXT("AXI Lite accepted");
                SETZERO(w_valid);
                SETVAL(b_ready, i_resp_mem_ready);
                SETVAL(state, state_b);
            ELSE();
                SETONE(w_valid);
                SETVAL(req_ready, INV_L(req_last));
                SETVAL(state, state_w);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(state_w);
        IF (NZ(i_msti.w_ready));
            TEXT("Burst write:");
            SETVAL(w_valid, i_req_mem_valid);
            SETVAL(req_last, i_req_mem_last);
            SETVAL(req_wstrb, i_req_mem_strob);
            SETVAL(req_wdata, i_req_mem_data);
            SETVAL(req_addr, comb.vb_req_addr_inc);

            IF (NZ(req_last));
                SETZERO(req_last);
                SETZERO(w_valid);
                SETVAL(b_ready, i_resp_mem_ready);
                SETZERO(req_ready);
                SETVAL(state, state_b);
            ENDIF();
        ELSIF(NZ(w_valid));
            SETZERO(req_ready);
            SETVAL(state, state_w_wait_accept);
        ENDIF();
        ENDCASE();
    CASE(state_w_wait_accept);
        IF (NZ(i_msti.w_ready));
            SETZERO(w_valid);
            IF (NZ(req_last));
                SETZERO(req_last);
                SETVAL(b_ready, i_resp_mem_ready);
                SETVAL(state, state_b);
            ELSE();
                SETONE(req_ready);
                SETVAL(state, state_w);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(state_b);
        SETVAL(b_ready, i_resp_mem_ready);
        IF (AND2(NZ(b_ready), NZ(i_msti.b_valid)));
            SETZERO(b_ready);
            SETVAL(state, state_idle);
            SETVAL(user_count, INC(user_count));
            SETVAL(resp_error, BIT(i_msti.b_resp, 1));
            SETONE(resp_valid);
            SETONE(resp_last);
        ENDIF();
        ENDCASE();
    ENDSWITCH();


TEXT();
    SYNC_RESET();

TEXT();
    SETVAL(o_resp_mem_valid, resp_valid);
    SETVAL(o_resp_mem_last, resp_last);
    SETVAL(o_resp_mem_fault, resp_error);
    SETVAL(o_resp_mem_addr, resp_addr);
    SETVAL(o_resp_mem_data, resp_data);
    SETVAL(o_req_mem_ready, req_ready);

TEXT();
    SETVAL(comb.vmsto.ar_valid, ar_valid);
    SETVAL(comb.vmsto.ar_bits.addr, req_addr);
    SETVAL(comb.vmsto.ar_bits.size, req_size);
    SETVAL(comb.vmsto.ar_bits.len, req_len);
    SETVAL(comb.vmsto.ar_user, user_count);
    SETVAL(comb.vmsto.ar_bits.burst, glob_types_amba_->AXI_BURST_INCR);
    SETVAL(comb.vmsto.r_ready, r_ready);

    TEXT();
    SETVAL(comb.vmsto.aw_valid, aw_valid);
    SETVAL(comb.vmsto.aw_bits.addr, req_addr);
    SETVAL(comb.vmsto.aw_bits.size, req_size);
    SETVAL(comb.vmsto.aw_bits.len, req_len);
    SETVAL(comb.vmsto.aw_user, user_count);
    SETVAL(comb.vmsto.aw_bits.burst, glob_types_amba_->AXI_BURST_INCR);
    SETVAL(comb.vmsto.w_valid, w_valid);
    SETVAL(comb.vmsto.w_last, req_last);
    SETVAL(comb.vmsto.w_data, req_wdata);
    SETVAL(comb.vmsto.w_strb, req_wstrb);
    SETVAL(comb.vmsto.w_user, user_count);
    SETVAL(comb.vmsto.b_ready, b_ready);

TEXT();
    SETVAL(o_msto, comb.vmsto);
    SETVAL(o_dbg_valid, dbg_valid);
    SETVAL(o_dbg_payload, dbg_payload);
}
