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

#include "accel_axi2apb_bus1_tb.h"

accel_axi2apb_bus1_tb::accel_axi2apb_bus1_tb(GenObject *parent, const char *name) :
    ModuleObject(parent, "accel_axi2apb_bus1_tb", name, NO_COMMENT),
    // parameters
    // Ports
    nrst(this, "nrst", "1", "1", "Power-on system reset active LOW"),
    clk(this, "clk", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_xslv_mapinfo(this, "wb_xslv_mapinfo", "Mapped address of the axi2apb bridge"),
    wb_pslv_mapinfo(this, "wb_pslv_mapinfo", "Mapped address of the testing APB device (UART)"),
    vec_o_mapinfo(this, "vec_o_mapinfo", NO_COMMENT),
    wb_xslv_cfg(this, "wb_xslv_cfg", NO_COMMENT),
    wb_pslv_cfg(this, "wb_pslv_cfg", NO_COMMENT),
    wb_i_xslvi(this, "wb_i_xslvi", NO_COMMENT),
    wb_o_xslvo(this, "wb_o_xslvo", NO_COMMENT),
    vec_apbi(this, "vec_apbi", NO_COMMENT),
    vec_apbo(this, "vec_apbo", NO_COMMENT),
    w_req_valid(this, "w_req_valid", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_req_addr(this, "wb_req_addr", "32", RSTVAL_ZERO, NO_COMMENT),
    w_req_write(this, "w_req_write", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_req_wdata(this, "wb_req_wdata", "32", RSTVAL_ZERO, NO_COMMENT),
    w_resp_valid(this, "w_resp_valid", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_resp_rdata(this, "wb_resp_rdata", "32", RSTVAL_ZERO, NO_COMMENT),
    w_resp_err(this, "w_resp_err", "1", RSTVAL_ZERO, NO_COMMENT),
    msg(this, "msg", "error message", NO_COMMENT),
    clk_cnt(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "clk_cnt", "32", "'0", NO_COMMENT),
    err_cnt(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "err_cnt", "32", "'0", NO_COMMENT),
    test_cnt(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "test_cnt", "32", "'0", NO_COMMENT),
    test_state(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "test_state", "3", "'0", NO_COMMENT),
    apb_wait_states(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "apb_wait_states", "2", "'0", NO_COMMENT),
    test_pause_cnt(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "test_pause_cnt", "32", "'0", NO_COMMENT),
    xsize(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "xsize", "3", "2", NO_COMMENT),
    aw_valid(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "aw_valid", "1", "0", NO_COMMENT),
    aw_addr(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "aw_addr", "48", "'0", NO_COMMENT),
    w_wait_states(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "w_wait_states", "4", "'0", NO_COMMENT),
    w_wait_cnt(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "w_wait_cnt", "4", "'0", NO_COMMENT),
    w_valid(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "w_valid", "1", "0", NO_COMMENT),
    w_data(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "w_data", "64", "'0", NO_COMMENT),
    w_strb(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "w_strb", "8", "'0", NO_COMMENT),
    b_wait_states(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "b_wait_states", "2", "'0", NO_COMMENT),
    b_wait_cnt(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "b_wait_cnt", "2", "'0", NO_COMMENT),
    b_ready(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "b_ready", "1", "0", NO_COMMENT),
    ar_valid(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "ar_valid", "1", "0", NO_COMMENT),
    ar_addr(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "ar_addr", "48", "'0", NO_COMMENT),
    r_wait_states(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "r_wait_states", "4", "'0", NO_COMMENT),
    r_wait_cnt(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "r_wait_cnt", "4", "'0", NO_COMMENT),
    r_ready(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "r_ready", "1", "0", NO_COMMENT),
    compare_ena(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "compare_ena", "1", "0", NO_COMMENT),
    compare_a(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "compare_a", "64", "'0", NO_COMMENT),
    compare_b(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "compare_b", "64", "'0", NO_COMMENT),
    end_of_test(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "end_of_test", "1", RSTVAL_ZERO, NO_COMMENT),
    apbstate(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "apbstate", "2", "'0", NO_COMMENT),
    apbram(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "apbram", "64", "0xcccccccccccccccc", NO_COMMENT),
    preq_ready(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "preq_ready", "1", RSTVAL_ZERO, NO_COMMENT),
    presp_valid(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "presp_valid", "1", RSTVAL_ZERO, NO_COMMENT),
    presp_rdata(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "presp_rdata", "32", "'0", NO_COMMENT),
    presp_delay(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "presp_delay", "2", "'0", NO_COMMENT),
    // submodules:
    clk0(this, "clk0", NO_COMMENT),
    bus1(this, "bus1", NO_COMMENT),
    pslv0(this, "pslv0", NO_COMMENT),
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
    NEW(bus1, bus1.getName().c_str());
        CONNECT(bus1, 0, bus1.i_clk, clk);
        CONNECT(bus1, 0, bus1.i_nrst, nrst);
        CONNECT(bus1, 0, bus1.i_mapinfo, wb_xslv_mapinfo);
        CONNECT(bus1, 0, bus1.o_cfg, wb_xslv_cfg);
        CONNECT(bus1, 0, bus1.i_xslvi, wb_i_xslvi);
        CONNECT(bus1, 0, bus1.o_xslvo, wb_o_xslvo);
        CONNECT(bus1, 0, bus1.i_apbo, vec_apbo);
        CONNECT(bus1, 0, bus1.o_apbi, vec_apbi);
        CONNECT(bus1, 0, bus1.o_mapinfo, vec_o_mapinfo);
    ENDNEW();

    TEXT();
    NEW(pslv0, pslv0.getName().c_str());
        CONNECT(pslv0, 0, pslv0.i_nrst, nrst);
        CONNECT(pslv0, 0, pslv0.i_clk, clk);
        CONNECT(pslv0, 0, pslv0.i_mapinfo, wb_pslv_mapinfo);
        CONNECT(pslv0, 0, pslv0.o_cfg, wb_pslv_cfg);
        CONNECT(pslv0, 0, pslv0.i_apbi, ARRITEM(vec_apbi, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_UART1"), vec_apbi));
        CONNECT(pslv0, 0, pslv0.o_apbo, ARRITEM(vec_apbo, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_UART1"), vec_apbo));
        CONNECT(pslv0, 0, pslv0.o_req_valid, w_req_valid);
        CONNECT(pslv0, 0, pslv0.o_req_addr, wb_req_addr);
        CONNECT(pslv0, 0, pslv0.o_req_write, w_req_write);
        CONNECT(pslv0, 0, pslv0.o_req_wdata, wb_req_wdata);
        CONNECT(pslv0, 0, pslv0.i_resp_valid, w_resp_valid);
        CONNECT(pslv0, 0, pslv0.i_resp_rdata, wb_resp_rdata);
        CONNECT(pslv0, 0, pslv0.i_resp_err, w_resp_err);
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

void accel_axi2apb_bus1_tb::comb_proc() {
    GenObject *apb_out_none = SCV_get_cfg_type(this, "apb_out_none");
    SETVAL(comb.vb_xslv_mapinfo.addr_start, CONST("0x08000000", 48));
    SETVAL(comb.vb_xslv_mapinfo.addr_end, CONST("0x09000000", 48));
    SETVAL(wb_xslv_mapinfo, comb.vb_xslv_mapinfo);

    TEXT();
    SETVAL(comb.vb_pslv_mapinfo.addr_start, CONST("0x010000", 48));
    SETVAL(comb.vb_pslv_mapinfo.addr_end, CONST("0x011000", 48));
    SETVAL(wb_pslv_mapinfo, comb.vb_pslv_mapinfo);

    TEXT();
    SETVAL(comb.vb_test_cnt_inv, INV_L(test_cnt));
    SETVAL(comb.vb_bar, CONST("0x08100000"));
    SETVAL(clk_cnt, INC(clk_cnt));
    SETZERO(compare_ena);
    SETZERO(end_of_test);

    TEXT();
    TEXT("APB memory simulation with controllable wait states");
    SWITCH(apbstate);
    CASE(CONST("0", 2));
        SETONE(preq_ready);
        SETZERO(presp_valid);
        IF (AND2(NZ(w_req_valid), NZ(preq_ready)));
            IF (NZ(w_req_write));
                IF (EZ(BIT(wb_req_addr, 2)));
                    SETVAL(apbram, CC2(BITS(apbram, 63, 32), wb_req_wdata));
                ELSE();
                    SETVAL(apbram, CC2(wb_req_wdata, BITS(apbram, 31, 0)));
                ENDIF();
                SETVAL(presp_rdata, ALLONES());
            ELSE();
                IF (EZ(BIT(wb_req_addr, 2)));
                    SETVAL(presp_rdata, BITS(apbram, 31, 0));
                ELSE();
                    SETVAL(presp_rdata, BITS(apbram, 63, 32));
                ENDIF();
            ENDIF();
            IF (EZ(apb_wait_states));
                SETONE(presp_valid);
            ELSE();
                SETZERO(preq_ready);
                SETVAL(presp_delay, DEC(apb_wait_states));
                SETVAL(apbstate, CONST("1", 2));
            ENDIF();
        ENDIF();
    ENDCASE();
    CASE(CONST("1", 2));
        IF (NZ(presp_delay));
            SETVAL(presp_delay, DEC(presp_delay));
        ELSE();
            SETONE(presp_valid);
            SETVAL(apbstate, CONST("2", 2));
        ENDIF();
    ENDCASE();
    CASE(CONST("2", 2));
        SETZERO(presp_valid);
        SETVAL(apbstate, CONST("0", 2));
    ENDCASE();
    ENDSWITCH();

    TEXT();
    TEXT("AXI request state machines");
    SWITCH(test_state);
    CASE(CONST("0", 3));
        IF (NZ(test_pause_cnt));
            SETVAL(test_pause_cnt, DEC(test_pause_cnt));
        ELSE();
            SETVAL(test_cnt, INC(test_cnt));
            SETVAL(apb_wait_states, BITS(test_cnt, 1, 0));
            SETVAL(w_wait_states, BITS(test_cnt, 5, 2));
            SETVAL(b_wait_states, BITS(test_cnt, 7, 6));
            SETVAL(r_wait_states, BITS(test_cnt, 11, 8));
            IF(NZ(BIT(test_cnt, 12)));
                SETVAL(xsize, CONST("3", 3), "8-bytes");
            ELSE();
                SETVAL(xsize, CONST("2", 3), "4-bytes");
            ENDIF();
            IF(NZ(BIT(test_cnt, 13)));
                TEXT("End of test (show err_cnt)");
                SETONE(end_of_test);
                SETVAL(test_state, CONST("7", 3));
            ELSE();
                SETVAL(test_state, CONST("1", 3));
            ENDIF();
        ENDIF();
    ENDCASE();
    CASE(CONST("1", 3), "aw request");
        SETONE(aw_valid);
        SETVAL(aw_addr, ADD2(comb.vb_bar, CC2(BITS(test_cnt, 11, 0), CONST("0", 2))));
        IF (AND2(NZ(aw_valid), NZ(wb_o_xslvo.aw_ready)));
            SETZERO(aw_valid);
            SETVAL(test_state, CONST("2", 3));
            SETVAL(w_data, CC2(comb.vb_test_cnt_inv, test_cnt));
            IF (EQ(xsize, CONST("2", 3)));
                IF (EZ(BIT(test_cnt, 0)));
                    SETVAL(w_strb, CONST("0x0f",8));
                ELSE();
                    SETVAL(w_strb, CONST("0xf0",8));
                ENDIF();
            ELSE();
                SETVAL(w_strb, CONST("0xff",8));
            ENDIF();
            IF (EZ(w_wait_states));
                SETZERO(w_wait_cnt);
                SETONE(w_valid);
            ELSE();
                SETVAL(w_wait_cnt, w_wait_states);
            ENDIF();
        ENDIF();
    ENDCASE();
    CASE(CONST("2", 3), "w request");
        IF (NZ(w_wait_cnt));
            SETVAL(w_wait_cnt, DEC(w_wait_cnt));
        ELSE();
            SETONE(w_valid);
            SETVAL(w_data, CC2(comb.vb_test_cnt_inv, test_cnt));
            IF (AND2(NZ(w_valid), NZ(wb_o_xslvo.w_ready)));
                SETZERO(w_valid);
                SETVAL(test_state, CONST("3", 3));
                IF (EZ(b_wait_states));
                    SETZERO(b_wait_cnt);
                    SETONE(b_ready);
                ELSE();
                    SETVAL(b_wait_cnt, b_wait_states);
                ENDIF();
            ENDIF();
        ENDIF();
    ENDCASE();
    CASE(CONST("3", 3), "b response");
        IF (NZ(b_wait_cnt));
            SETVAL(b_wait_cnt, DEC(b_wait_cnt));
        ELSE();
            SETONE(b_ready);
            IF (AND2(NZ(b_ready), NZ(wb_o_xslvo.b_valid)));
                SETZERO(b_ready);
                SETVAL(test_state, CONST("4", 3));
                SETONE(ar_valid);
                SETVAL(ar_addr, ADD2(comb.vb_bar, CC2(BITS(test_cnt, 11, 0), CONST("0", 2))));
            ENDIF();
        ENDIF();
    ENDCASE();
    CASE(CONST("4", 3), "ar request");
        SETONE(ar_valid);
        SETVAL(ar_addr, ADD2(comb.vb_bar, CC2(BITS(test_cnt, 11, 0), CONST("0", 2))));
        IF (AND2(NZ(ar_valid), NZ(wb_o_xslvo.ar_ready))); 
            SETZERO(ar_valid);
            SETVAL(test_state, CONST("5", 3));
            IF (EZ(r_wait_states));
                SETZERO(r_wait_cnt);
                SETONE(r_ready);
            ELSE();
                SETVAL(r_wait_cnt, r_wait_states);
            ENDIF();
        ENDIF();
    ENDCASE();
    CASE(CONST("5", 3), "r response");
        IF (NZ(r_wait_cnt));
            SETVAL(r_wait_cnt, DEC(r_wait_cnt));
        ELSE();
            SETONE(r_ready);
            IF (AND2(NZ(r_ready), NZ(wb_o_xslvo.r_valid)));
                SETZERO(r_ready);
                SETONE(compare_ena);
                SETVAL(compare_a, wb_o_xslvo.r_data);
                IF (EQ(xsize, CONST("2", 3)));
                    IF (EZ(BIT(test_cnt, 0)));
                        SETVAL(compare_b, CC2(BITS(w_data, 31, 0), BITS(w_data, 31, 0)));
                    ELSE();
                        SETVAL(compare_b, CC2(BITS(w_data, 63, 32), BITS(w_data, 63, 32)));
                    ENDIF();
                ELSIF(EZ(BIT(test_cnt, 0)));
                    SETVAL(compare_b, w_data);
                ELSE();
                    TEXT("Error response");
                    SETVAL(compare_b, ALLONES());
                ENDIF();
                IF (NZ(wb_o_xslvo.r_last));
                    TEXT("Goto idle");
                    SETVAL(test_pause_cnt, CONST("10", 32));
                    SETVAL(test_state, CONST("0", 3));
                ELSE();
                    IF (EZ(r_wait_states));
                        SETONE(r_ready);
                    ELSE();
                        SETVAL(r_wait_cnt, r_wait_states);
                    ENDIF();
                ENDIF();
            ENDIF();
        ENDIF();
    ENDCASE();
    CASE(CONST("7", 3), "do nothing");
    ENDCASE();
    ENDSWITCH();

    TEXT();
    IF (NZ(compare_ena));
        IF (NE(compare_a, compare_b));
            SETVAL(err_cnt, INC(err_cnt));
        ENDIF();
    ENDIF();

    TEXT();
    SETVAL(comb.vb_xslvi.ar_valid, ar_valid);
    SETVAL(comb.vb_xslvi.ar_bits.addr, ar_addr);
    SETVAL(comb.vb_xslvi.ar_bits.len, CONST("0", 8));
    SETVAL(comb.vb_xslvi.ar_bits.size, xsize);
    SETVAL(comb.vb_xslvi.ar_bits.burst, CONST("1", 2));
    SETVAL(comb.vb_xslvi.ar_bits.lock, CONST("0", 1));
    SETVAL(comb.vb_xslvi.ar_bits.cache, CONST("0", 1));
    SETVAL(comb.vb_xslvi.ar_bits.prot, CONST("0", 3));
    SETVAL(comb.vb_xslvi.ar_bits.qos, CONST("0", 4));
    SETVAL(comb.vb_xslvi.ar_bits.region, CONST("0", 4));
    SETVAL(comb.vb_xslvi.ar_id, ALLZEROS());
    SETVAL(comb.vb_xslvi.ar_user, ALLZEROS());

    TEXT();
    SETVAL(comb.vb_xslvi.aw_valid, aw_valid);
    SETVAL(comb.vb_xslvi.aw_bits.addr, aw_addr);
    SETVAL(comb.vb_xslvi.aw_bits.len, CONST("0", 8));
    SETVAL(comb.vb_xslvi.aw_bits.size, xsize);
    SETVAL(comb.vb_xslvi.aw_bits.burst, CONST("1", 2));
    SETVAL(comb.vb_xslvi.aw_bits.lock, CONST("0", 1));
    SETVAL(comb.vb_xslvi.aw_bits.cache, CONST("0", 1));
    SETVAL(comb.vb_xslvi.aw_bits.prot, CONST("0", 3));
    SETVAL(comb.vb_xslvi.aw_bits.qos, CONST("0", 4));
    SETVAL(comb.vb_xslvi.aw_bits.region, CONST("0", 4));
    SETVAL(comb.vb_xslvi.aw_id, ALLZEROS());
    SETVAL(comb.vb_xslvi.aw_user, ALLZEROS());

    TEXT();
    SETVAL(comb.vb_xslvi.w_valid, w_valid);
    SETVAL(comb.vb_xslvi.w_data, w_data);
    SETVAL(comb.vb_xslvi.w_last, CONST("1", 1));
    SETVAL(comb.vb_xslvi.w_strb, w_strb);
    SETVAL(comb.vb_xslvi.w_user, ALLZEROS());
    SETVAL(comb.vb_xslvi.b_ready, b_ready);
    SETVAL(comb.vb_xslvi.r_ready, r_ready);
    SETVAL(wb_i_xslvi, comb.vb_xslvi);

    TEXT();
    SETARRITEM(vec_apbo, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_PRCI"), vec_apbo, *apb_out_none);
    SETARRITEM(vec_apbo, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_DMI"), vec_apbo, *apb_out_none);
    SETARRITEM(vec_apbo, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_I2C0"), vec_apbo, *apb_out_none);
    SETARRITEM(vec_apbo, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_GPIO"), vec_apbo, *apb_out_none);
    SETARRITEM(vec_apbo, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_DDR"), vec_apbo, *apb_out_none);
    SETARRITEM(vec_apbo, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_PCIE"), vec_apbo, *apb_out_none);
    SETARRITEM(vec_apbo, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_PNP"), vec_apbo, *apb_out_none);
    SETVAL(w_resp_valid, presp_valid);
    SETVAL(wb_resp_rdata, presp_rdata);
    SETVAL(w_resp_err, CONST("0", 1));
}


void accel_axi2apb_bus1_tb::test_proc() {
    IF (NZ(compare_ena));
        EXPECT_EQ(compare_a, compare_b, "APB write/read compare");
    ENDIF();
    IF (NZ(end_of_test));
        DISPLAY_ERROR(err_cnt);
    ENDIF();
}

