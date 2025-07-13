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

#include "axi_mst_generator.h"

axi_mst_generator::axi_mst_generator(GenObject *parent, const char *name) :
    ModuleObject(parent, "axi_mst_generator", name, NO_COMMENT),
    // parameters
    req_bar(this, "req_bar", "48", "0x81000000", NO_COMMENT),
    unique_id(this, "unique_id", "4", "'1", NO_COMMENT),
    read_compare(this, "read_compare", "64", "0xFFFFFFFFFFFFFFFF", NO_COMMENT),
    read_only(this, "read_only", "0", NO_COMMENT),
    burst_disable(this, "burst_disable", "0", NO_COMMENT),
    // Ports
    i_nrst(this, "i_nrst", "1", NO_COMMENT),
    i_clk(this, "i_clk", "1", NO_COMMENT),
    i_xmst(this, "i_xmst", NO_COMMENT),
    o_xmst(this, "o_xmst", NO_COMMENT),
    i_start_test(this, "i_start_test", "1", NO_COMMENT),
    i_test_selector(this, "i_test_selector", "32", NO_COMMENT),
    i_show_result(this, "i_show_result", "1", NO_COMMENT),
    o_writing(this, "o_writing", "1", NO_COMMENT),
    o_reading(this, "o_reading", "1", NO_COMMENT),
    msg(this, "msg", "error message", NO_COMMENT),
    err_cnt(this, "err_cnt", "32", "'0", NO_COMMENT),
    compare_cnt(this, "compare_cnt", "32", "'0", NO_COMMENT),
    run_cnt(this, "run_cnt", "32", "'0", NO_COMMENT),
    state(this, "state", "4", "'0", NO_COMMENT),
    xsize(this, "xsize", "3", "3", NO_COMMENT),
    aw_wait_cnt(this, "aw_wait_cnt", "2", "'0", NO_COMMENT),
    aw_valid(this, "aw_valid", "1", "0", NO_COMMENT),
    aw_addr(this, "aw_addr", "48", "'0", NO_COMMENT),
    aw_unmap(this, "aw_unmap", "1", "0", NO_COMMENT),
    aw_xlen(this, "aw_xlen", "8", "'0", NO_COMMENT),
    w_use_axi_light(this, "w_use_axi_light", "1", RSTVAL_ZERO, NO_COMMENT),
    w_wait_states(this, "w_wait_states", "3", "'0", NO_COMMENT),
    w_wait_cnt(this, "w_wait_cnt", "3", "'0", NO_COMMENT),
    w_valid(this, "w_valid", "1", "0", NO_COMMENT),
    w_data(this, "w_data", "64", "'0", NO_COMMENT),
    w_strb(this, "w_strb", "8", "'0", NO_COMMENT),
    w_last(this, "w_last", "8", "'0", NO_COMMENT),
    w_burst_cnt(this, "w_burst_cnt", "4", "'0", NO_COMMENT),
    b_wait_states(this, "b_wait_states", "2", "'0", NO_COMMENT),
    b_wait_cnt(this, "b_wait_cnt", "2", "'0", NO_COMMENT),
    b_ready(this, "b_ready", "1", "0", NO_COMMENT),
    ar_wait_cnt(this, "ar_wait_cnt", "2", "'0", NO_COMMENT),
    ar_valid(this, "ar_valid", "1", "0", NO_COMMENT),
    ar_addr(this, "ar_addr", "48", "'0", NO_COMMENT),
    ar_unmap(this, "ar_unmap", "1", "0", NO_COMMENT),
    ar_xlen(this, "ar_xlen", "8", "'0", NO_COMMENT),
    r_wait_states(this, "r_wait_states", "3", "'0", NO_COMMENT),
    r_wait_cnt(this, "r_wait_cnt", "3", "'0", NO_COMMENT),
    r_ready(this, "r_ready", "1", "0", NO_COMMENT),
    r_burst_cnt(this, "r_burst_cnt", "4", "'0", NO_COMMENT),
    compare_ena(this, "compare_ena", "1", "0", NO_COMMENT),
    compare_a(this, "compare_a", "64", "'0", NO_COMMENT),
    compare_b(this, "compare_b", "64", "'0", NO_COMMENT),
    // submodules:
    // processes:
    comb(this),
    test(this, &i_clk)
{
    Operation::start(this);

    Operation::start(&comb);
    comb_proc();

    Operation::start(&test);
    test_proc();
}

void axi_mst_generator::comb_proc() {
    SETVAL(comb.vb_run_cnt_inv, INV_L(run_cnt));
    SETVAL(comb.vb_w_burst_cnt_next, INC(w_burst_cnt));
    SETVAL(comb.vb_bar, req_bar);
    SETZERO(compare_ena);

    TEXT();
    TEXT("AXI master request state machines");
    SWITCH(state);
    CASE(CONST("0", 4));
        IF (NZ(i_start_test));
            IF (NZ(read_only));
                IF (EZ(BITS(i_test_selector, 1, 0)));
                    SETVAL(state, CONST("5", 4), "ar_request");
                ELSE();
                    SETVAL(state, CONST("14", 4), "wait to ar_request");
                ENDIF();
            ELSE();
                IF (EZ(BITS(i_test_selector, 1, 0)));
                    SETVAL(state, CONST("1", 4), "aw_request");
                ELSE();
                    SETVAL(state, CONST("13", 4), "wait to aw_request");
                ENDIF();
            ENDIF();
            SETVAL(run_cnt, INC(run_cnt));
            SETZERO(aw_unmap);
            SETVAL(ar_unmap, BIT(i_test_selector, 0));
            SETVAL(aw_wait_cnt, BITS(i_test_selector, 1, 0));
            SETVAL(ar_wait_cnt, BITS(i_test_selector, 1, 0));
            SETVAL(w_wait_states, BITS(i_test_selector, 4, 2));
            SETVAL(b_wait_states, BITS(i_test_selector, 6, 5));
            SETVAL(r_wait_states, BITS(i_test_selector, 9, 7));
            IF (EZ(burst_disable));
                SETVAL(aw_xlen, CC2(CONST("0", 6), BITS(i_test_selector, 11, 10)));
                SETVAL(ar_xlen, CC2(CONST("0", 6), BITS(i_test_selector, 11, 10)));
            ELSE();
                SETZERO(aw_xlen);
                SETZERO(ar_xlen);
            ENDIF();
            IF (AND2(EZ(BITS(i_test_selector, 11, 10)), EQ(BITS(i_test_selector, 4, 2), CONST("7", 3))));
                SETONE(w_use_axi_light);
            ELSE();
                SETZERO(w_use_axi_light);
            ENDIF();
            SETVAL(xsize, CONST("3", 3), "8-bytes");
            IF (NZ(BIT(i_test_selector, 12)));
                SETVAL(xsize, CONST("2", 3), "4-bytes");
            ENDIF();
        ENDIF();
    ENDCASE();
    CASE(CONST("13", 4), "wait to aw request");
        IF (NZ(aw_wait_cnt));
            SETVAL(aw_wait_cnt, DEC(aw_wait_cnt));
        ELSE();
            SETVAL(state, CONST("1", 4), "aw_request");
        ENDIF();
    ENDCASE();
    CASE(CONST("14", 4), "wait to ar request");
        IF (NZ(ar_wait_cnt));
            SETVAL(ar_wait_cnt, DEC(ar_wait_cnt));
        ELSE();
            SETVAL(state, CONST("5", 4), "ar_request");
        ENDIF();
    ENDCASE();
    CASE(CONST("1", 4), "aw request");
        SETONE(comb.v_writing);
        SETONE(aw_valid);
        IF (EZ(aw_unmap));
            SETVAL(aw_addr, ADD2(comb.vb_bar, CC2(BITS(run_cnt, 6, 0), CONST("0", 5))));
        ELSE();
            SETVAL(aw_addr, CONST("0xFFFFFFFFFC00", 48));
        ENDIF();
        SETZERO(w_burst_cnt);
        IF (NZ(w_use_axi_light));
            SETONE(w_valid);
            SETONE(w_last);
            SETVAL(w_data,  CC4(unique_id, BITS(comb.vb_run_cnt_inv, 27, 0), BITS(run_cnt, 27, 0), w_burst_cnt));
            SETVAL(w_strb, CONST("0xff", 8));
        ENDIF();
        IF (AND2(NZ(aw_valid), NZ(i_xmst.aw_ready)));
            SETZERO(aw_valid);
            SETVAL(w_data,  CC4(unique_id, BITS(comb.vb_run_cnt_inv, 27, 0), BITS(run_cnt, 27, 0), w_burst_cnt));
            SETVAL(w_strb, CONST("0xff", 8));
            IF (AND2(NZ(w_valid), NZ(i_xmst.w_ready)));
                SETZERO(w_valid);
                SETZERO(w_last);
                IF (EZ(b_wait_states));
                    SETZERO(b_wait_cnt);
                    SETONE(b_ready);
                ELSE();
                    SETVAL(b_wait_cnt, b_wait_states);
                ENDIF();
                SETVAL(state, CONST("4", 4));
            ELSIF (OR3(EZ(w_wait_states), NZ(aw_xlen), NZ(w_valid)));
                TEXT("1. Generate first w_valid just after aw in burst transaction to check buffering");
                TEXT("2. Cannot inject waits for AXI Light requests");
                SETZERO(w_wait_cnt);
                SETONE(w_valid);
                SETVAL(w_last, INV_L(OR_REDUCE(aw_xlen)));
                SETVAL(state, CONST("3", 4));
            ELSE();
                SETVAL(state, CONST("2", 4));
                SETVAL(w_wait_cnt, w_wait_states);
            ENDIF();
        ENDIF();
    ENDCASE();
    CASE(CONST("2", 4), "w wait request");
        SETONE(comb.v_writing);
        IF (NZ(w_wait_cnt));
            SETVAL(w_wait_cnt, DEC(w_wait_cnt));
        ELSE();
            SETONE(w_valid);
            SETVAL(w_last, INV_L(OR_REDUCE(aw_xlen)));
            SETVAL(state, CONST("3", 4));
        ENDIF();
    ENDCASE();
    CASE(CONST("3", 4), "w request");
        SETONE(comb.v_writing);
        SETONE(w_valid);
        SETVAL(w_data,  CC4(unique_id, BITS(comb.vb_run_cnt_inv, 27, 0), BITS(run_cnt, 27, 0), w_burst_cnt));
        IF (AND2(NZ(w_valid), NZ(i_xmst.w_ready)));
            SETVAL(w_burst_cnt, comb.vb_w_burst_cnt_next);
            SETVAL(w_data,  CC4(unique_id, BITS(comb.vb_run_cnt_inv, 27, 0), BITS(run_cnt, 27, 0), comb.vb_w_burst_cnt_next));
            SETZERO(w_valid);
            SETZERO(w_last);
            SETVAL(w_wait_cnt, w_wait_states);
            IF (NZ(aw_xlen));
                SETVAL(aw_xlen, DEC(aw_xlen));
                IF (EZ(w_wait_states));
                    SETONE(w_valid);
                    SETVAL(w_last, INV_L(OR_REDUCE(BITS(aw_xlen, 7, 1))));
                ELSE();
                    SETVAL(state, CONST("2", 4));
                ENDIF();
            ELSE();
                IF (EZ(b_wait_states));
                    SETZERO(b_wait_cnt);
                    SETONE(b_ready);
                ELSE();
                    SETVAL(b_wait_cnt, b_wait_states);
                ENDIF();
                SETVAL(state, CONST("4", 4));
            ENDIF();
        ENDIF();
    ENDCASE();
    CASE(CONST("4", 4), "b response");
        SETONE(comb.v_writing);
        SETZERO(w_burst_cnt);
        IF (NZ(b_wait_cnt));
            SETVAL(b_wait_cnt, DEC(b_wait_cnt));
        ELSE();
            SETONE(b_ready);
            IF (AND2(NZ(b_ready), NZ(i_xmst.b_valid)));
                SETZERO(b_ready);
                SETVAL(state, CONST("5", 4));
                SETONE(ar_valid);
                IF (EZ(ar_unmap));
                    SETVAL(ar_addr, ADD2(comb.vb_bar, CC2(BITS(run_cnt, 6, 0), CONST("0", 5))));
                ELSE();
                    SETVAL(ar_addr, CONST("0xFFFFFFFFFC00", 48));
                ENDIF();
            ENDIF();
        ENDIF();
    ENDCASE();
    CASE(CONST("5", 4), "ar request");
        SETONE(comb.v_reading);
        SETONE(ar_valid);
        IF (EZ(ar_unmap));
            SETVAL(ar_addr, ADD2(comb.vb_bar, CC2(BITS(run_cnt, 6, 0), CONST("0", 5))));
        ELSE();
            SETVAL(ar_addr, CONST("0xFFFFFFFFFC00", 48));
        ENDIF();
        IF (AND2(NZ(ar_valid), NZ(i_xmst.ar_ready))); 
            SETZERO(ar_valid);
            SETZERO(r_burst_cnt);
            IF (OR2(EZ(r_wait_states), NZ(ar_xlen)));
                SETZERO(r_wait_cnt);
                SETONE(r_ready);
                SETVAL(state, CONST("7", 4));
            ELSE();
                SETVAL(state, CONST("6", 4));
                SETVAL(r_wait_cnt, r_wait_states);
            ENDIF();
        ENDIF();
    ENDCASE();
    CASE(CONST("6", 4));
        SETONE(comb.v_reading);
        IF (NZ(r_wait_cnt));
            SETVAL(r_wait_cnt, DEC(r_wait_cnt));
        ELSE();
            SETONE(r_ready);
            SETVAL(state, CONST("7", 4));
        ENDIF();
    ENDCASE();
    CASE(CONST("7", 4), "r response");
        SETONE(comb.v_reading);
        SETONE(r_ready);
        IF (AND2(NZ(r_ready), NZ(i_xmst.r_valid)));
            SETVAL(r_burst_cnt, INC(r_burst_cnt));
            SETZERO(r_ready);
            SETONE(compare_ena);
            SETVAL(compare_a, i_xmst.r_data);
            IF (EZ(ar_unmap));
                IF (EZ(read_only));
                    SETVAL(compare_b, CC4(unique_id, BITS(comb.vb_run_cnt_inv, 27, 0), BITS(run_cnt, 27, 0), r_burst_cnt));
                ELSE();
                    SETVAL(compare_b, read_compare);
                ENDIF();
            ELSE();
                SETVAL(compare_b, CONST("0xFFFFFFFFFFFFFFFF", 64));
            ENDIF();
            IF (NZ(i_xmst.r_last));
                TEXT("Goto idle");
                SETVAL(state, CONST("0", 4));
            ELSE();
                IF (EZ(r_wait_states));
                    SETONE(r_ready);
                ELSE();
                    SETVAL(r_wait_cnt, r_wait_states);
                    SETVAL(state, CONST("6", 4));
                ENDIF();
            ENDIF();
        ENDIF();
    ENDCASE();
    CASE(CONST("15", 4), "do nothing");
    ENDCASE();
    ENDSWITCH();

    TEXT();
    IF (NZ(compare_ena));
        SETVAL(compare_cnt, INC(compare_cnt));
        IF (NE(compare_a, compare_b));
            SETVAL(err_cnt, INC(err_cnt));
        ENDIF();
    ENDIF();

    TEXT();
    SETVAL(comb.vb_xmsto.ar_valid, ar_valid);
    SETVAL(comb.vb_xmsto.ar_bits.addr, ar_addr);
    SETVAL(comb.vb_xmsto.ar_bits.len, ar_xlen);
    SETVAL(comb.vb_xmsto.ar_bits.size, xsize);
    SETVAL(comb.vb_xmsto.ar_bits.burst, CONST("1", 2));
    SETVAL(comb.vb_xmsto.ar_bits.lock, CONST("0", 1));
    SETVAL(comb.vb_xmsto.ar_bits.cache, CONST("0", 1));
    SETVAL(comb.vb_xmsto.ar_bits.prot, CONST("0", 3));
    SETVAL(comb.vb_xmsto.ar_bits.qos, CONST("0", 4));
    SETVAL(comb.vb_xmsto.ar_bits.region, CONST("0", 4));
    SETVAL(comb.vb_xmsto.ar_id, ALLONES());
    SETVAL(comb.vb_xmsto.ar_user, ALLONES());
    SETVAL(comb.vb_xmsto.aw_valid, aw_valid);
    SETVAL(comb.vb_xmsto.aw_bits.addr, aw_addr);
    SETVAL(comb.vb_xmsto.aw_bits.len, aw_xlen);
    SETVAL(comb.vb_xmsto.aw_bits.size, xsize);
    SETVAL(comb.vb_xmsto.aw_bits.burst, CONST("1", 2));
    SETVAL(comb.vb_xmsto.aw_bits.lock, CONST("0", 1));
    SETVAL(comb.vb_xmsto.aw_bits.cache, CONST("0", 1));
    SETVAL(comb.vb_xmsto.aw_bits.prot, CONST("0", 3));
    SETVAL(comb.vb_xmsto.aw_bits.qos, CONST("0", 4));
    SETVAL(comb.vb_xmsto.aw_bits.region, CONST("0", 4));
    SETVAL(comb.vb_xmsto.aw_id, ALLONES());
    SETVAL(comb.vb_xmsto.aw_user, ALLONES());
    SETVAL(comb.vb_xmsto.w_valid, w_valid);
    SETVAL(comb.vb_xmsto.w_data, w_data);
    SETVAL(comb.vb_xmsto.w_last, w_last);
    SETVAL(comb.vb_xmsto.w_strb, w_strb);
    SETVAL(comb.vb_xmsto.w_user, ALLONES());
    SETVAL(comb.vb_xmsto.b_ready, b_ready);
    SETVAL(comb.vb_xmsto.r_ready, r_ready);
    SETVAL(o_xmst, comb.vb_xmsto);
    SETVAL(o_writing, comb.v_writing);
    SETVAL(o_reading, comb.v_reading);
}

void axi_mst_generator::test_proc() {
    IF (NZ(compare_ena));
        EXPECT_EQ(compare_a, compare_b, "master[0] write/read compare");
    ENDIF();
    IF (NZ(i_show_result));
        DISPLAY_ERROR(compare_cnt, err_cnt, "master generator");
    ENDIF();
}

