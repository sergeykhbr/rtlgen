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

#include "accel_axictrl_bus0_tb.h"

accel_axictrl_bus0_tb::accel_axictrl_bus0_tb(GenObject *parent, const char *name) :
    ModuleObject(parent, "accel_axictrl_bus0_tb", name, NO_COMMENT),
    // parameters
    // Ports
    nrst(this, "nrst", "1", "1", "Power-on system reset active LOW"),
    clk(this, "clk", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_bus0_cfg(this, "wb_bus0_cfg", NO_COMMENT),
    wb_xslv0_cfg(this, "wb_xslv0_cfg", NO_COMMENT),
    vec_i_xmsto(this, "vec_i_xmsto", NO_COMMENT),
    vec_o_xmsti(this, "vec_o_xmsti", NO_COMMENT),
    vec_i_xslvo(this, "vec_i_xslvo", NO_COMMENT),
    vec_o_xslvi(this, "vec_o_xslvi", NO_COMMENT),
    vec_o_mapinfo(this, "vec_o_mapinfo", NO_COMMENT),
    w_req_valid(this, "w_req_valid", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_req_addr(this, "wb_req_addr", "48", RSTVAL_ZERO, NO_COMMENT),
    wb_req_size(this, "wb_req_size", "8", RSTVAL_ZERO, NO_COMMENT),
    w_req_write(this, "w_req_write", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_req_wdata(this, "wb_req_wdata", "64", RSTVAL_ZERO, NO_COMMENT),
    wb_req_wstrb(this, "wb_req_wstrb", "8", RSTVAL_ZERO, NO_COMMENT),
    w_req_last(this, "w_req_last", "1", RSTVAL_ZERO, NO_COMMENT),
    w_req_ready(this, "w_req_ready", "1", RSTVAL_ZERO, NO_COMMENT),
    w_resp_valid(this, "w_resp_valid", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_resp_rdata(this, "wb_resp_rdata", "64", RSTVAL_ZERO, NO_COMMENT),
    w_resp_err(this, "w_resp_err", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_m0_xmsti(this, "wb_m0_xmsti", NO_COMMENT),
    wb_m1_xmsti(this, "wb_m1_xmsti", NO_COMMENT),
    msg(this, "msg", "error message", NO_COMMENT),
    clk_cnt(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "clk_cnt", "32", "'0", NO_COMMENT),
    err_cnt(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "err_cnt", "32", "'0", NO_COMMENT),
    test_cnt(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "test_cnt", "32", "'0", NO_COMMENT),
    test_pause_cnt(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "test_pause_cnt", "32", "'0", NO_COMMENT),
    m0_state(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "m0_state", "4", "'0", NO_COMMENT),
    m0_xsize(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "m0_xsize", "3", "3", NO_COMMENT),
    m0_aw_valid(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "m0_aw_valid", "1", "0", NO_COMMENT),
    m0_aw_addr(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "m0_aw_addr", "48", "'0", NO_COMMENT),
    m0_aw_xlen(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "m0_aw_xlen", "8", "'0", NO_COMMENT),
    m0_w_wait_states(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "m0_w_wait_states", "3", "'0", NO_COMMENT),
    m0_w_wait_cnt(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "m0_w_wait_cnt", "3", "'0", NO_COMMENT),
    m0_w_valid(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "m0_w_valid", "1", "0", NO_COMMENT),
    m0_w_data(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "m0_w_data", "64", "'0", NO_COMMENT),
    m0_w_strb(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "m0_w_strb", "8", "'0", NO_COMMENT),
    m0_w_last(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "m0_w_last", "8", "'0", NO_COMMENT),
    m0_b_wait_states(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "m0_b_wait_states", "2", "'0", NO_COMMENT),
    m0_b_wait_cnt(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "m0_b_wait_cnt", "2", "'0", NO_COMMENT),
    m0_b_ready(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "m0_b_ready", "1", "0", NO_COMMENT),
    m0_ar_valid(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "m0_ar_valid", "1", "0", NO_COMMENT),
    m0_ar_addr(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "m0_ar_addr", "48", "'0", NO_COMMENT),
    m0_ar_xlen(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "m0_ar_xlen", "8", "'0", NO_COMMENT),
    m0_r_wait_states(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "m0_r_wait_states", "3", "'0", NO_COMMENT),
    m0_r_wait_cnt(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "m0_r_wait_cnt", "3", "'0", NO_COMMENT),
    m0_r_ready(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "m0_r_ready", "1", "0", NO_COMMENT),
    m1_state(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "m1_state", "3", "'0", NO_COMMENT),
    compare_ena(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "compare_ena", "1", "0", NO_COMMENT),
    compare_a(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "compare_a", "64", "'0", NO_COMMENT),
    compare_b(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "compare_b", "64", "'0", NO_COMMENT),
    end_of_test(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "end_of_test", "1", RSTVAL_ZERO, NO_COMMENT),
    slvstate(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "slvstate", "2", "'0", NO_COMMENT),
    slvram(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "slvram", "64", "0xcccccccccccccccc", NO_COMMENT),
    req_ready(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "req_ready", "1", RSTVAL_ZERO, NO_COMMENT),
    resp_valid(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "resp_valid", "1", RSTVAL_ZERO, NO_COMMENT),
    resp_rdata(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "resp_rdata", "64", "'0", NO_COMMENT),
    resp_wait_states(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "resp_wait_states", "3", "'0", NO_COMMENT),
    resp_wait_cnt(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "resp_wait_cnt", "3", "'0", NO_COMMENT),
    // submodules:
    clk0(this, "clk0", NO_COMMENT),
    bus0(this, "bus0", NO_COMMENT),
    xslv0(this, "xslv0", NO_COMMENT),
    // processes:
    comb(this),
    test(this, &clk)
{
    Operation::start(this);

    // Create and connet Sub-modules:
    clk0.period.setObjValue(new FloatConst(10.0));  // 100 MHz
    NEW(clk0, clk0.getName().c_str());
        CONNECT(clk0, 0, clk0.o_clk, clk);
    ENDNEW();

    TEXT();
    NEW(bus0, bus0.getName().c_str());
        CONNECT(bus0, 0, bus0.i_clk, clk);
        CONNECT(bus0, 0, bus0.i_nrst, nrst);
        CONNECT(bus0, 0, bus0.o_cfg, wb_bus0_cfg);
        CONNECT(bus0, 0, bus0.i_xmsto, vec_i_xmsto);
        CONNECT(bus0, 0, bus0.o_xmsti, vec_o_xmsti);
        CONNECT(bus0, 0, bus0.i_xslvo, vec_i_xslvo);
        CONNECT(bus0, 0, bus0.o_xslvi, vec_o_xslvi);
        CONNECT(bus0, 0, bus0.o_mapinfo, vec_o_mapinfo);
    ENDNEW();

    TEXT();
    NEW(xslv0, xslv0.getName().c_str());
        CONNECT(xslv0, 0, xslv0.i_nrst, nrst);
        CONNECT(xslv0, 0, xslv0.i_clk, clk);
        CONNECT(xslv0, 0, xslv0.i_mapinfo, ARRITEM(vec_o_mapinfo, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_DDR"), vec_o_mapinfo));
        CONNECT(xslv0, 0, xslv0.o_cfg, wb_xslv0_cfg);
        CONNECT(xslv0, 0, xslv0.i_xslvi, ARRITEM(vec_o_xslvi, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_DDR"), vec_o_xslvi));
        CONNECT(xslv0, 0, xslv0.o_xslvo, ARRITEM(vec_i_xslvo, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_DDR"), vec_i_xslvo));
        CONNECT(xslv0, 0, xslv0.o_req_valid, w_req_valid);
        CONNECT(xslv0, 0, xslv0.o_req_addr, wb_req_addr);
        CONNECT(xslv0, 0, xslv0.o_req_size, wb_req_size);
        CONNECT(xslv0, 0, xslv0.o_req_write, w_req_write);
        CONNECT(xslv0, 0, xslv0.o_req_wdata, wb_req_wdata);
        CONNECT(xslv0, 0, xslv0.o_req_wstrb, wb_req_wstrb);
        CONNECT(xslv0, 0, xslv0.o_req_last, w_req_last);
        CONNECT(xslv0, 0, xslv0.i_req_ready, w_req_ready);
        CONNECT(xslv0, 0, xslv0.i_resp_valid, w_resp_valid);
        CONNECT(xslv0, 0, xslv0.i_resp_rdata, wb_resp_rdata);
        CONNECT(xslv0, 0, xslv0.i_resp_err, w_resp_err);
    ENDNEW();

    INITIAL();
        SETZERO(nrst);
        SETVAL_DELAY(nrst, CONST("1", 1), *new FloatConst(200.0));
    ENDINITIAL();

    Operation::start(&comb);
    comb_proc();

    Operation::start(&test);
    test_proc();
}

void accel_axictrl_bus0_tb::comb_proc() {
    GenObject *axi4_master_out_none = SCV_get_cfg_type(this, "axi4_master_out_none");
    GenObject *axi4_slave_out_none = SCV_get_cfg_type(this, "axi4_slave_out_none");
    TEXT();
    SETVAL(wb_m0_xmsti, ARRITEM(vec_o_xmsti, *SCV_get_cfg_type(this, "CFG_BUS0_XMST_GROUP0"), vec_o_xmsti));
    SETVAL(wb_m1_xmsti, ARRITEM(vec_o_xmsti, *SCV_get_cfg_type(this, "CFG_BUS0_XMST_HDMI"), vec_o_xmsti));
    SETVAL(comb.vb_test_cnt_inv, INV_L(test_cnt));
    SETVAL(comb.vb_bar, CONST("0x81000000"));
    SETVAL(clk_cnt, INC(clk_cnt));
    SETZERO(compare_ena);
    SETZERO(end_of_test);

    TEXT();
    TEXT("ddr simulation with controllable wait states");
    SWITCH(slvstate);
    CASE(CONST("0", 2));
        SETONE(req_ready);
        SETZERO(resp_valid);
        IF (AND2(NZ(w_req_valid), NZ(req_ready)));
            IF (NZ(w_req_write));
                SETVAL(slvram, wb_req_wdata);
                SETVAL(resp_rdata, ALLONES());
            ELSE();
                SETVAL(resp_rdata, slvram);
            ENDIF();
            IF (EZ(resp_wait_states));
                SETONE(resp_valid);
            ELSE();
                SETZERO(req_ready);
                SETVAL(resp_wait_cnt, DEC(resp_wait_states));
                SETVAL(slvstate, CONST("1", 2));
            ENDIF();
        ENDIF();
    ENDCASE();
    CASE(CONST("1", 2));
        IF (NZ(resp_wait_cnt));
            SETVAL(resp_wait_cnt, DEC(resp_wait_cnt));
        ELSE();
            SETONE(resp_valid);
            SETVAL(slvstate, CONST("2", 2));
        ENDIF();
    ENDCASE();
    CASE(CONST("2", 2));
        SETZERO(resp_valid);
        SETVAL(slvstate, CONST("0", 2));
    ENDCASE();
    ENDSWITCH();

    TEXT();
    TEXT("AXI master[0] request state machines");
    SWITCH(m0_state);
    CASE(CONST("0", 4));
        IF (EZ(test_pause_cnt));
            IF (NZ(end_of_test));
                SETVAL(m0_state, CONST("0xF", 4));
            ELSE();
                SETVAL(m0_state, CONST("1", 4));
            ENDIF();
        ENDIF();
    ENDCASE();
    CASE(CONST("1", 4), "aw request");
        SETONE(m0_aw_valid);
        SETVAL(m0_aw_addr, ADD2(comb.vb_bar, CC2(BITS(test_cnt, 11, 0), CONST("0", 5))));
        IF (AND2(NZ(m0_aw_valid), NZ(wb_m0_xmsti.aw_ready)));
            SETZERO(m0_aw_valid);
            SETVAL(m0_w_data, CC2(comb.vb_test_cnt_inv, test_cnt));
            SETVAL(m0_w_strb, CONST("0xff", 8));
            IF (EZ(m0_w_wait_states));
                SETZERO(m0_w_wait_cnt);
                SETONE(m0_w_valid);
                SETVAL(m0_w_last, INV_L(OR_REDUCE(m0_aw_xlen)));
                SETVAL(m0_state, CONST("3", 4));
            ELSE();
                SETVAL(m0_state, CONST("2", 4));
                SETVAL(m0_w_wait_cnt, m0_w_wait_states);
            ENDIF();
        ENDIF();
    ENDCASE();
    CASE(CONST("2", 4), "w request");
        IF (NZ(m0_w_wait_cnt));
            SETVAL(m0_w_wait_cnt, DEC(m0_w_wait_cnt));
        ELSE();
            SETONE(m0_w_valid);
            SETVAL(m0_w_last, INV_L(OR_REDUCE(m0_aw_xlen)));
            SETVAL(m0_state, CONST("3", 4));
        ENDIF();
    ENDCASE();
    CASE(CONST("3", 4), "w request");
        SETONE(m0_w_valid);
        SETVAL(m0_w_data, CC2(comb.vb_test_cnt_inv, test_cnt));
        IF (AND2(NZ(m0_w_valid), NZ(wb_m0_xmsti.w_ready)));
            SETZERO(m0_w_valid);
            SETZERO(m0_w_last);
            SETVAL(m0_w_wait_cnt, m0_w_wait_states);
            IF (NZ(m0_aw_xlen));
                SETVAL(m0_aw_xlen, DEC(m0_aw_xlen));
                IF (EZ(m0_w_wait_states));
                    SETONE(m0_w_valid);
                    SETVAL(m0_w_last, INV_L(OR_REDUCE(BITS(m0_aw_xlen, 7, 1))));
                ELSE();
                    SETVAL(m0_state, CONST("2", 4));
                ENDIF();
            ELSE();
                IF (EZ(m0_b_wait_states));
                    SETZERO(m0_b_wait_cnt);
                    SETONE(m0_b_ready);
                ELSE();
                    SETVAL(m0_b_wait_cnt, m0_b_wait_states);
                ENDIF();
                SETVAL(m0_state, CONST("4", 4));
            ENDIF();
        ENDIF();
    ENDCASE();
    CASE(CONST("4", 4), "b response");
        IF (NZ(m0_b_wait_cnt));
            SETVAL(m0_b_wait_cnt, DEC(m0_b_wait_cnt));
        ELSE();
            SETONE(m0_b_ready);
            IF (AND2(NZ(m0_b_ready), NZ(wb_m0_xmsti.b_valid)));
                SETZERO(m0_b_ready);
                SETVAL(m0_state, CONST("5", 4));
                SETONE(m0_ar_valid);
                SETVAL(m0_ar_addr, ADD2(comb.vb_bar, CC2(BITS(test_cnt, 11, 0), CONST("0", 5))));
            ENDIF();
        ENDIF();
    ENDCASE();
    CASE(CONST("5", 4), "ar request");
        SETONE(m0_ar_valid);
        SETVAL(m0_ar_addr, ADD2(comb.vb_bar, CC2(BITS(test_cnt, 11, 0), CONST("0", 2))));
        IF (AND2(NZ(m0_ar_valid), NZ(wb_m0_xmsti.ar_ready))); 
            SETZERO(m0_ar_valid);
            IF (EZ(m0_r_wait_states));
                SETZERO(m0_r_wait_cnt);
                SETONE(m0_r_ready);
                SETVAL(m0_state, CONST("7", 4));
            ELSE();
                SETVAL(m0_state, CONST("6", 4));
                SETVAL(m0_r_wait_cnt, m0_r_wait_states);
            ENDIF();
        ENDIF();
    ENDCASE();
    CASE(CONST("6", 4));
        IF (NZ(m0_r_wait_cnt));
            SETVAL(m0_r_wait_cnt, DEC(m0_r_wait_cnt));
        ELSE();
            SETONE(m0_r_ready);
            SETVAL(m0_state, CONST("7", 4));
        ENDIF();
    ENDCASE();
    CASE(CONST("7", 4), "r response");
        SETONE(m0_r_ready);
        IF (AND2(NZ(m0_r_ready), NZ(wb_m0_xmsti.r_valid)));
            SETZERO(m0_r_ready);
            SETONE(compare_ena);
            SETVAL(compare_a, wb_m0_xmsti.r_data);
            SETVAL(compare_b, m0_w_data);
            IF (NZ(wb_m0_xmsti.r_last));
                TEXT("Goto idle");
                SETVAL(m0_state, CONST("0", 4));
            ELSE();
                IF (EZ(m0_r_wait_states));
                    SETONE(m0_r_ready);
                ELSE();
                    SETVAL(m0_r_wait_cnt, m0_r_wait_states);
                    SETVAL(m0_state, CONST("6", 4));
                ENDIF();
            ENDIF();
        ENDIF();
    ENDCASE();
    CASE(CONST("15", 4), "do nothing");
    ENDCASE();
    ENDSWITCH();

    TEXT();
    IF (ANDx(3, &NZ(test_pause_cnt),
                &EZ(m0_state),
                &EZ(m1_state)));
        SETVAL(test_pause_cnt, DEC(test_pause_cnt));
    ELSIF (EZ(test_pause_cnt));
        SETVAL(test_cnt, INC(test_cnt));
        SETVAL(resp_wait_states, BITS(test_cnt, 1, 0));
        SETVAL(m0_w_wait_states, BITS(test_cnt, 4, 2));
        SETVAL(m0_b_wait_states, BITS(test_cnt, 6, 5));
        SETVAL(m0_r_wait_states, BITS(test_cnt, 9, 7));
        SETVAL(m0_aw_xlen, CC2(CONST("0", 6), BITS(test_cnt, 11, 10)));
        SETVAL(m0_ar_xlen, CC2(CONST("0", 6), BITS(test_cnt, 11, 10)));
        SETVAL(m0_xsize, CONST("3", 3), "8-bytes");
        IF(NZ(BIT(test_cnt, 13)));
            TEXT("End of test (show err_cnt)");
            SETONE(end_of_test);
        ENDIF();
        SETVAL(test_pause_cnt, CONST("10", 32));
    ELSE();
        SETVAL(test_pause_cnt, CONST("10", 32));
    ENDIF();

    TEXT();
    IF (NZ(compare_ena));
        IF (NE(compare_a, compare_b));
            SETVAL(err_cnt, INC(err_cnt));
        ENDIF();
    ENDIF();

    TEXT();
    SETVAL(comb.vb_m0_xmsto.ar_valid, m0_ar_valid);
    SETVAL(comb.vb_m0_xmsto.ar_bits.addr, m0_ar_addr);
    SETVAL(comb.vb_m0_xmsto.ar_bits.len, m0_ar_xlen);
    SETVAL(comb.vb_m0_xmsto.ar_bits.size, m0_xsize);
    SETVAL(comb.vb_m0_xmsto.ar_bits.burst, CONST("1", 2));
    SETVAL(comb.vb_m0_xmsto.ar_bits.lock, CONST("0", 1));
    SETVAL(comb.vb_m0_xmsto.ar_bits.cache, CONST("0", 1));
    SETVAL(comb.vb_m0_xmsto.ar_bits.prot, CONST("0", 3));
    SETVAL(comb.vb_m0_xmsto.ar_bits.qos, CONST("0", 4));
    SETVAL(comb.vb_m0_xmsto.ar_bits.region, CONST("0", 4));
    SETVAL(comb.vb_m0_xmsto.ar_id, ALLONES());
    SETVAL(comb.vb_m0_xmsto.ar_user, ALLONES());
    SETVAL(comb.vb_m0_xmsto.aw_valid, m0_aw_valid);
    SETVAL(comb.vb_m0_xmsto.aw_bits.addr, m0_aw_addr);
    SETVAL(comb.vb_m0_xmsto.aw_bits.len, m0_aw_xlen);
    SETVAL(comb.vb_m0_xmsto.aw_bits.size, m0_xsize);
    SETVAL(comb.vb_m0_xmsto.aw_bits.burst, CONST("1", 2));
    SETVAL(comb.vb_m0_xmsto.aw_bits.lock, CONST("0", 1));
    SETVAL(comb.vb_m0_xmsto.aw_bits.cache, CONST("0", 1));
    SETVAL(comb.vb_m0_xmsto.aw_bits.prot, CONST("0", 3));
    SETVAL(comb.vb_m0_xmsto.aw_bits.qos, CONST("0", 4));
    SETVAL(comb.vb_m0_xmsto.aw_bits.region, CONST("0", 4));
    SETVAL(comb.vb_m0_xmsto.aw_id, ALLONES());
    SETVAL(comb.vb_m0_xmsto.aw_user, ALLONES());
    SETVAL(comb.vb_m0_xmsto.w_valid, m0_w_valid);
    SETVAL(comb.vb_m0_xmsto.w_data, m0_w_data);
    SETVAL(comb.vb_m0_xmsto.w_last, m0_w_last);
    SETVAL(comb.vb_m0_xmsto.w_strb, m0_w_strb);
    SETVAL(comb.vb_m0_xmsto.w_user, ALLONES());
    SETVAL(comb.vb_m0_xmsto.b_ready, m0_b_ready);
    SETVAL(comb.vb_m0_xmsto.r_ready, m0_r_ready);

    TEXT();
    SETVAL(w_req_ready, req_ready);
    SETVAL(w_resp_valid, resp_valid);
    SETVAL(wb_resp_rdata, resp_rdata);
    SETVAL(w_resp_err, CONST("0", 1));


    TEXT();
    SETARRITEM(vec_i_xmsto, *SCV_get_cfg_type(this, "CFG_BUS0_XMST_GROUP0"), vec_i_xmsto, comb.vb_m0_xmsto);
    SETARRITEM(vec_i_xmsto, *SCV_get_cfg_type(this, "CFG_BUS0_XMST_PCIE"), vec_i_xmsto, *axi4_master_out_none);
    SETARRITEM(vec_i_xmsto, *SCV_get_cfg_type(this, "CFG_BUS0_XMST_HDMI"), vec_i_xmsto, comb.vb_m1_xmsto);
    SETARRITEM(vec_i_xslvo, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_BOOTROM"), vec_i_xslvo, *axi4_slave_out_none);
    SETARRITEM(vec_i_xslvo, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_CLINT"), vec_i_xslvo, *axi4_slave_out_none);
    SETARRITEM(vec_i_xslvo, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_SRAM"), vec_i_xslvo, *axi4_slave_out_none);
    SETARRITEM(vec_i_xslvo, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_PLIC"), vec_i_xslvo, *axi4_slave_out_none);
    SETARRITEM(vec_i_xslvo, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_PBRIDGE"), vec_i_xslvo, *axi4_slave_out_none);
    SETVAL(w_resp_valid, resp_valid);
    SETVAL(wb_resp_rdata, resp_rdata);
    SETVAL(w_resp_err, CONST("0", 1));
}


void accel_axictrl_bus0_tb::test_proc() {
    IF (NZ(compare_ena));
        EXPECT_EQ(compare_a, compare_b);
    ENDIF();
    IF (NZ(end_of_test));
        DISPLAY_ERROR(err_cnt);
    ENDIF();
}

