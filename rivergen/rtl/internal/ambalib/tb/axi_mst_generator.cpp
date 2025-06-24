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
    // Ports
    i_nrst(this, "i_nrst", "1", NO_COMMENT),
    i_clk(this, "i_clk", "1", NO_COMMENT),
    i_xmst(this, "i_xmst", NO_COMMENT),
    o_xmst(this, "o_xmst", NO_COMMENT),
    i_start_test(this, "i_start_test", "1", NO_COMMENT),
    i_test_selector(this, "i_test_selector", "10", NO_COMMENT),
    i_show_result(this, "i_show_result", "1", NO_COMMENT),
    o_test_busy(this, "o_test_busy", "1", NO_COMMENT),
    msg(this, "msg", "error message", NO_COMMENT),
    err_cnt(this, "err_cnt", "32", "'0", NO_COMMENT),
    run_cnt(this, "run_cnt", "32", "'0", NO_COMMENT),
    state(this, "state", "4", "'0", NO_COMMENT),
    xsize(this, "xsize", "3", "3", NO_COMMENT),
    aw_valid(this, "aw_valid", "1", "0", NO_COMMENT),
    aw_addr(this, "aw_addr", "48", "'0", NO_COMMENT),
    aw_xlen(this, "aw_xlen", "8", "'0", NO_COMMENT),
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
    ar_valid(this, "ar_valid", "1", "0", NO_COMMENT),
    ar_addr(this, "ar_addr", "48", "'0", NO_COMMENT),
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
            SETVAL(state, CONST("1", 4));
            SETVAL(run_cnt, INC(run_cnt));
            SETVAL(w_wait_states, BITS(i_test_selector, 2, 0));//4, 2));
            SETVAL(b_wait_states, BITS(i_test_selector, 4, 3));//6, 5));
            SETVAL(r_wait_states, BITS(i_test_selector, 7, 5));//9, 7));
            SETVAL(aw_xlen, CC2(CONST("0", 6), BITS(i_test_selector, 9, 8)));//11, 10)));
            SETVAL(ar_xlen, CC2(CONST("0", 6), BITS(i_test_selector, 9, 8)));//, 11, 10)));
            SETVAL(xsize, CONST("3", 3), "8-bytes");
        ENDIF();
    ENDCASE();
    CASE(CONST("1", 4), "aw request");
        SETONE(aw_valid);
        SETVAL(aw_addr, ADD2(comb.vb_bar, CC2(BITS(run_cnt, 11, 0), CONST("0", 5))));
        SETZERO(w_burst_cnt);
        IF (AND2(NZ(aw_valid), NZ(i_xmst.aw_ready)));
            SETZERO(aw_valid);
            SETVAL(w_data,  CC3(comb.vb_run_cnt_inv, BITS(run_cnt, 27, 0), w_burst_cnt));
            SETVAL(w_strb, CONST("0xff", 8));
            IF (EZ(w_wait_states));
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
        IF (NZ(w_wait_cnt));
            SETVAL(w_wait_cnt, DEC(w_wait_cnt));
        ELSE();
            SETONE(w_valid);
            SETVAL(w_last, INV_L(OR_REDUCE(aw_xlen)));
            SETVAL(state, CONST("3", 4));
        ENDIF();
    ENDCASE();
    CASE(CONST("3", 4), "w request");
        SETONE(w_valid);
        SETVAL(w_data,  CC3(comb.vb_run_cnt_inv, BITS(run_cnt, 27, 0), w_burst_cnt));
        IF (AND2(NZ(w_valid), NZ(i_xmst.w_ready)));
            SETVAL(w_burst_cnt, comb.vb_w_burst_cnt_next);
            SETVAL(w_data,  CC3(comb.vb_run_cnt_inv, BITS(run_cnt, 27, 0), comb.vb_w_burst_cnt_next));
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
        SETZERO(w_burst_cnt);
        IF (NZ(b_wait_cnt));
            SETVAL(b_wait_cnt, DEC(b_wait_cnt));
        ELSE();
            SETONE(b_ready);
            IF (AND2(NZ(b_ready), NZ(i_xmst.b_valid)));
                SETZERO(b_ready);
                SETVAL(state, CONST("5", 4));
                SETONE(ar_valid);
                SETVAL(ar_addr, ADD2(comb.vb_bar, CC2(BITS(run_cnt, 11, 0), CONST("0", 5))));
            ENDIF();
        ENDIF();
    ENDCASE();
    CASE(CONST("5", 4), "ar request");
        SETONE(ar_valid);
        SETVAL(ar_addr, ADD2(comb.vb_bar, CC2(BITS(run_cnt, 11, 0), CONST("0", 5))));
        IF (AND2(NZ(ar_valid), NZ(i_xmst.ar_ready))); 
            SETZERO(ar_valid);
            SETZERO(r_burst_cnt);
            IF (EZ(r_wait_states));
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
        IF (NZ(r_wait_cnt));
            SETVAL(r_wait_cnt, DEC(r_wait_cnt));
        ELSE();
            SETONE(r_ready);
            SETVAL(state, CONST("7", 4));
        ENDIF();
    ENDCASE();
    CASE(CONST("7", 4), "r response");
        SETONE(r_ready);
        IF (AND2(NZ(r_ready), NZ(i_xmst.r_valid)));
            SETVAL(r_burst_cnt, INC(r_burst_cnt));
            SETZERO(r_ready);
            SETONE(compare_ena);
            SETVAL(compare_a, i_xmst.r_data);
            SETVAL(compare_b, CC3(comb.vb_run_cnt_inv, BITS(run_cnt, 27, 0), r_burst_cnt));
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
    SETVAL(o_test_busy, OR_REDUCE(state));
}

void axi_mst_generator::test_proc() {
    IF (NZ(compare_ena));
        EXPECT_EQ(compare_a, compare_b, "master[0] write/read compare");
    ENDIF();
    IF (NZ(i_show_result));
        DISPLAY_ERROR(err_cnt, "master generator");
    ENDIF();
}

