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
    vec_i_xmsto(this, "vec_i_xmsto", NO_COMMENT),
    vec_o_xmsti(this, "vec_o_xmsti", NO_COMMENT),
    vec_i_xslvo(this, "vec_i_xslvo", NO_COMMENT),
    vec_o_xslvi(this, "vec_o_xslvi", NO_COMMENT),
    vec_o_mapinfo(this, "vec_o_mapinfo", NO_COMMENT),
    wb_s0_cfg(this, "wb_s0_cfg", NO_COMMENT),
    w_s0_req_valid(this, "w_s0_req_valid", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_s0_req_addr(this, "wb_s0_req_addr", "48", RSTVAL_ZERO, NO_COMMENT),
    wb_s0_req_size(this, "wb_s0_req_size", "8", RSTVAL_ZERO, NO_COMMENT),
    w_s0_req_write(this, "w_s0_req_write", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_s0_req_wdata(this, "wb_s0_req_wdata", "64", RSTVAL_ZERO, NO_COMMENT),
    wb_s0_req_wstrb(this, "wb_s0_req_wstrb", "8", RSTVAL_ZERO, NO_COMMENT),
    w_s0_req_last(this, "w_s0_req_last", "1", RSTVAL_ZERO, NO_COMMENT),
    w_s0_req_ready(this, "w_s0_req_ready", "1", RSTVAL_ZERO, NO_COMMENT),
    w_s0_resp_valid(this, "w_s0_resp_valid", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_s0_resp_rdata(this, "wb_s0_resp_rdata", "64", RSTVAL_ZERO, NO_COMMENT),
    w_s0_resp_err(this, "w_s0_resp_err", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_s1_cfg(this, "wb_s1_cfg", NO_COMMENT),
    w_s1_req_valid(this, "w_s1_req_valid", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_s1_req_addr(this, "wb_s1_req_addr", "48", RSTVAL_ZERO, NO_COMMENT),
    wb_s1_req_size(this, "wb_s1_req_size", "8", RSTVAL_ZERO, NO_COMMENT),
    w_s1_req_write(this, "w_s1_req_write", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_s1_req_wdata(this, "wb_s1_req_wdata", "64", RSTVAL_ZERO, NO_COMMENT),
    wb_s1_req_wstrb(this, "wb_s1_req_wstrb", "8", RSTVAL_ZERO, NO_COMMENT),
    w_s1_req_last(this, "w_s1_req_last", "1", RSTVAL_ZERO, NO_COMMENT),
    w_s1_req_ready(this, "w_s1_req_ready", "1", RSTVAL_ZERO, NO_COMMENT),
    w_s1_resp_valid(this, "w_s1_resp_valid", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_s1_resp_rdata(this, "wb_s1_resp_rdata", "64", RSTVAL_ZERO, NO_COMMENT),
    w_s1_resp_err(this, "w_s1_resp_err", "1", RSTVAL_ZERO, NO_COMMENT),
    w_m0_writing(this, "w_m0_writing", "1", RSTVAL_ZERO, NO_COMMENT),
    w_m0_reading(this, "w_m0_reading", "1", RSTVAL_ZERO, NO_COMMENT),
    w_m1_writing(this, "w_m1_writing", "1", RSTVAL_ZERO, NO_COMMENT),
    w_m1_reading(this, "w_m1_reading", "1", RSTVAL_ZERO, NO_COMMENT),
    w_m2_writing(this, "w_m2_writing", "1", RSTVAL_ZERO, NO_COMMENT),
    w_m2_reading(this, "w_m2_reading", "1", RSTVAL_ZERO, NO_COMMENT),
    clk_cnt(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "clk_cnt", "32", "'0", NO_COMMENT),
    err_cnt(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "err_cnt", "32", "'0", NO_COMMENT),
    test_cnt(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "test_cnt", "32", "'0", NO_COMMENT),
    test_pause_cnt(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "test_pause_cnt", "32", "'0", NO_COMMENT),
    m0_start_ena(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "m0_start_ena", "1", RSTVAL_ZERO, NO_COMMENT),
    m0_test_selector(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "m0_test_selector", "32", RSTVAL_ZERO, NO_COMMENT),
    m1_start_ena(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "m1_start_ena", "1", RSTVAL_ZERO, NO_COMMENT),
    m1_test_selector(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "m1_test_selector", "32", RSTVAL_ZERO, NO_COMMENT),
    m2_start_ena(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "m2_start_ena", "1", RSTVAL_ZERO, NO_COMMENT),
    m2_test_selector(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "m2_test_selector", "32", RSTVAL_ZERO, NO_COMMENT),
    s0_state(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "s0_state", "2", "'0", NO_COMMENT),
    req_s0_ready(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "req_s0_ready", "1", RSTVAL_ZERO, NO_COMMENT),
    resp_s0_valid(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "resp_s0_valid", "1", RSTVAL_ZERO, NO_COMMENT),
    resp_s0_rdata(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "resp_s0_rdata", "64", "'0", NO_COMMENT),
    resp_s0_wait_states(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "resp_s0_wait_states", "3", "'0", NO_COMMENT),
    resp_s0_wait_cnt(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW,  "resp_s0_wait_cnt", "3", "'0", NO_COMMENT),
    end_of_test(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "end_of_test", "1", RSTVAL_ZERO, NO_COMMENT),
    end_idle(this, &clk, CLK_POSEDGE, &nrst, ACTIVE_LOW, "end_idle", "1", RSTVAL_ZERO, NO_COMMENT),
    s0_mem0(this, "s0_mem0", "64", "16", NO_COMMENT),
    s0_mem1(this, "s0_mem1", "64", "16", NO_COMMENT),
    // submodules:
    clk0(this, "clk0", NO_COMMENT),
    bus0(this, "bus0", NO_COMMENT),
    xslv0(this, "xslv0", NO_COMMENT),
    xslv1(this, "xslv1", NO_COMMENT),
    mst0(this, "mst0"),
    mst1(this, "mst1"),
    mst2(this, "mst2"),
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
        CONNECT(xslv0, 0, xslv0.o_cfg, wb_s0_cfg);
        CONNECT(xslv0, 0, xslv0.i_xslvi, ARRITEM(vec_o_xslvi, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_DDR"), vec_o_xslvi));
        CONNECT(xslv0, 0, xslv0.o_xslvo, ARRITEM(vec_i_xslvo, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_DDR"), vec_i_xslvo));
        CONNECT(xslv0, 0, xslv0.o_req_valid, w_s0_req_valid);
        CONNECT(xslv0, 0, xslv0.o_req_addr, wb_s0_req_addr);
        CONNECT(xslv0, 0, xslv0.o_req_size, wb_s0_req_size);
        CONNECT(xslv0, 0, xslv0.o_req_write, w_s0_req_write);
        CONNECT(xslv0, 0, xslv0.o_req_wdata, wb_s0_req_wdata);
        CONNECT(xslv0, 0, xslv0.o_req_wstrb, wb_s0_req_wstrb);
        CONNECT(xslv0, 0, xslv0.o_req_last, w_s0_req_last);
        CONNECT(xslv0, 0, xslv0.i_req_ready, w_s0_req_ready);
        CONNECT(xslv0, 0, xslv0.i_resp_valid, w_s0_resp_valid);
        CONNECT(xslv0, 0, xslv0.i_resp_rdata, wb_s0_resp_rdata);
        CONNECT(xslv0, 0, xslv0.i_resp_err, w_s0_resp_err);
    ENDNEW();

    TEXT();
    NEW(xslv1, xslv1.getName().c_str());
        CONNECT(xslv1, 0, xslv1.i_nrst, nrst);
        CONNECT(xslv1, 0, xslv1.i_clk, clk);
        CONNECT(xslv1, 0, xslv1.i_mapinfo, ARRITEM(vec_o_mapinfo, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_SRAM"), vec_o_mapinfo));
        CONNECT(xslv1, 0, xslv1.o_cfg, wb_s1_cfg);
        CONNECT(xslv1, 0, xslv1.i_xslvi, ARRITEM(vec_o_xslvi, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_SRAM"), vec_o_xslvi));
        CONNECT(xslv1, 0, xslv1.o_xslvo, ARRITEM(vec_i_xslvo, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_SRAM"), vec_i_xslvo));
        CONNECT(xslv1, 0, xslv1.o_req_valid, w_s1_req_valid);
        CONNECT(xslv1, 0, xslv1.o_req_addr, wb_s1_req_addr);
        CONNECT(xslv1, 0, xslv1.o_req_size, wb_s1_req_size);
        CONNECT(xslv1, 0, xslv1.o_req_write, w_s1_req_write);
        CONNECT(xslv1, 0, xslv1.o_req_wdata, wb_s1_req_wdata);
        CONNECT(xslv1, 0, xslv1.o_req_wstrb, wb_s1_req_wstrb);
        CONNECT(xslv1, 0, xslv1.o_req_last, w_s1_req_last);
        CONNECT(xslv1, 0, xslv1.i_req_ready, w_s1_req_ready);
        CONNECT(xslv1, 0, xslv1.i_resp_valid, w_s1_resp_valid);
        CONNECT(xslv1, 0, xslv1.i_resp_rdata, wb_s1_resp_rdata);
        CONNECT(xslv1, 0, xslv1.i_resp_err, w_s1_resp_err);
    ENDNEW();

    TEXT();
    mst0.req_bar.setObjValue(new HexLogicConst(new DecConst(48), 0x81000000));
    mst0.unique_id.setObjValue(new HexLogicConst(new DecConst(4), 0x0));
    mst0.read_only.setObjValue(new DecConst(0));
    NEW(mst0, mst0.getName().c_str());
        CONNECT(mst0, 0, mst0.i_nrst, nrst);
        CONNECT(mst0, 0, mst0.i_clk, clk);
        CONNECT(mst0, 0, mst0.i_xmst, ARRITEM(vec_o_xmsti, *SCV_get_cfg_type(this, "CFG_BUS0_XMST_GROUP0"), vec_o_xmsti));
        CONNECT(mst0, 0, mst0.o_xmst, ARRITEM(vec_i_xmsto, *SCV_get_cfg_type(this, "CFG_BUS0_XMST_GROUP0"), vec_i_xmsto));
        CONNECT(mst0, 0, mst0.i_start_test, m0_start_ena);
        CONNECT(mst0, 0, mst0.i_test_selector, m0_test_selector);
        CONNECT(mst0, 0, mst0.i_show_result, end_of_test);
        CONNECT(mst0, 0, mst0.o_writing, w_m0_writing);
        CONNECT(mst0, 0, mst0.o_reading, w_m0_reading);
    ENDNEW();

    TEXT();
    mst1.req_bar.setObjValue(new HexLogicConst(new DecConst(48), 0x82000000));
    mst1.unique_id.setObjValue(new HexLogicConst(new DecConst(4), 0x1));
    mst1.read_only.setObjValue(new DecConst(0));
    NEW(mst1, mst1.getName().c_str());
        CONNECT(mst1, 0, mst1.i_nrst, nrst);
        CONNECT(mst1, 0, mst1.i_clk, clk);
        CONNECT(mst1, 0, mst1.i_xmst, ARRITEM(vec_o_xmsti, *SCV_get_cfg_type(this, "CFG_BUS0_XMST_PCIE"), vec_o_xmsti));
        CONNECT(mst1, 0, mst1.o_xmst, ARRITEM(vec_i_xmsto, *SCV_get_cfg_type(this, "CFG_BUS0_XMST_PCIE"), vec_i_xmsto));
        CONNECT(mst1, 0, mst1.i_start_test, m1_start_ena);
        CONNECT(mst1, 0, mst1.i_test_selector, m1_test_selector);
        CONNECT(mst1, 0, mst1.i_show_result, end_of_test);
        CONNECT(mst1, 0, mst1.o_writing, w_m1_writing);
        CONNECT(mst1, 0, mst1.o_reading, w_m1_reading);
    ENDNEW();

    TEXT();
    mst2.req_bar.setObjValue(new HexLogicConst(new DecConst(48), 0x08000000));
    mst2.unique_id.setObjValue(new HexLogicConst(new DecConst(4), 0x2));
    mst2.read_compare.setObjValue(new HexLogicConst(new DecConst(64), 0xcafef00d33221100));
    mst2.read_only.setObjValue(new DecConst(1));
    NEW(mst2, mst2.getName().c_str());
        CONNECT(mst2, 0, mst2.i_nrst, nrst);
        CONNECT(mst2, 0, mst2.i_clk, clk);
        CONNECT(mst2, 0, mst2.i_xmst, ARRITEM(vec_o_xmsti, *SCV_get_cfg_type(this, "CFG_BUS0_XMST_HDMI"), vec_o_xmsti));
        CONNECT(mst2, 0, mst2.o_xmst, ARRITEM(vec_i_xmsto, *SCV_get_cfg_type(this, "CFG_BUS0_XMST_HDMI"), vec_i_xmsto));
        CONNECT(mst2, 0, mst2.i_start_test, m2_start_ena);
        CONNECT(mst2, 0, mst2.i_test_selector, m2_test_selector);
        CONNECT(mst2, 0, mst2.i_show_result, end_of_test);
        CONNECT(mst2, 0, mst2.o_writing, w_m2_writing);
        CONNECT(mst2, 0, mst2.o_reading, w_m2_reading);
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
    SETVAL(comb.vb_test_cnt_inv, INV_L(test_cnt));
    SETVAL(clk_cnt, INC(clk_cnt));
    SETZERO(end_of_test);
    SETVAL(end_idle, OR2(end_of_test, end_idle));
    SETZERO(m0_start_ena);
    SETZERO(m1_start_ena);
    SETZERO(m2_start_ena);

    TEXT();
    TEXT("ddr simulation with controllable wait states");
    SWITCH(s0_state);
    CASE(CONST("0", 2));
        SETONE(req_s0_ready);
        SETZERO(resp_s0_valid);
        IF (AND2(NZ(w_s0_req_valid), NZ(req_s0_ready)));
            IF (NZ(w_s0_req_write));
                SETVAL(resp_s0_rdata, ALLONES());
            ELSE();
                IF (EZ(BIT(wb_s0_req_addr, 25)));
                    SETVAL(resp_s0_rdata, ARRITEM(s0_mem0, TO_INT(BITS(wb_s0_req_addr, 5, 2)), s0_mem0));
                ELSE();
                    SETVAL(resp_s0_rdata, ARRITEM(s0_mem1, TO_INT(BITS(wb_s0_req_addr, 5, 2)), s0_mem1));
                ENDIF();
            ENDIF();
            IF (EZ(resp_s0_wait_states));
                SETONE(resp_s0_valid);
            ELSE();
                SETZERO(req_s0_ready);
                SETVAL(resp_s0_wait_cnt, DEC(resp_s0_wait_states));
                SETVAL(s0_state, CONST("1", 2));
            ENDIF();
        ENDIF();
    ENDCASE();
    CASE(CONST("1", 2));
        IF (NZ(resp_s0_wait_cnt));
            SETVAL(resp_s0_wait_cnt, DEC(resp_s0_wait_cnt));
        ELSE();
            SETONE(resp_s0_valid);
            SETVAL(s0_state, CONST("2", 2));
        ENDIF();
    ENDCASE();
    CASE(CONST("2", 2));
        SETZERO(resp_s0_valid);
        SETVAL(s0_state, CONST("0", 2));
    ENDCASE();
    ENDSWITCH();

    TEXT();
    IF (NZ(end_idle));
        TEXT("Do nothing");
    ELSIF (ANDx(6, &NZ(test_pause_cnt),
                &EZ(w_m0_writing),
                &EZ(w_m0_reading),
                &EZ(w_m1_writing),
                &EZ(w_m1_reading),
                &EZ(w_m2_writing),
                &EZ(w_m2_reading)));
        SETVAL(test_pause_cnt, DEC(test_pause_cnt));
    ELSIF (EZ(test_pause_cnt));
        SETVAL(test_cnt, INC(test_cnt));
        SETVAL(resp_s0_wait_states, BITS(test_cnt, 1, 0));
        SETVAL(m0_test_selector, CC2(CONST("0", 2), BITS(test_cnt, 31, 2)));
        SETONE(m0_start_ena);
        IF (EZ(BIT(test_cnt, 0)));
            SETVAL(m1_test_selector, CC2(CONST("0", 1), BITS(test_cnt, 31, 1)));
            SETONE(m1_start_ena);
        ENDIF();
        SETVAL(m2_test_selector, CONST("0xC00", 32), "Burst 4, with zero wait states");
        SETONE(m2_start_ena);
        IF(NZ(BIT(test_cnt, 15)));
            TEXT("End of test (show err_cnt)");
            SETONE(end_of_test);
        ENDIF();
        SETVAL(test_pause_cnt, CONST("10", 32));
    ELSE();
        SETVAL(test_pause_cnt, CONST("10", 32));
    ENDIF();
    IF (ANDx(3, &NZ(BIT(test_cnt, 0)),
                &NZ(w_m0_reading),
                &EZ(OR2_L(w_m1_writing, w_m1_reading))));
        TEXT("Check delayed writing after reading");
        SETVAL(m1_test_selector, BITS(test_cnt, 11, 1));
        SETONE(m1_start_ena);
    ENDIF();

    TEXT();
    SETVAL(w_s0_req_ready, req_s0_ready);
    SETVAL(w_s0_resp_valid, resp_s0_valid);
    SETVAL(wb_s0_resp_rdata, resp_s0_rdata);
    SETVAL(w_s0_resp_err, CONST("0", 1));

    TEXT();
    SETVAL(w_s1_req_ready, CONST("1", 1));
    SETVAL(w_s1_resp_valid, CONST("1", 1));
    SETVAL(wb_s1_resp_rdata, CONST("0xcafef00d33221100", 64));
    SETVAL(w_s1_resp_err, CONST("0", 1));

    TEXT();
    SETARRITEM(vec_i_xslvo, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_BOOTROM"), vec_i_xslvo, *axi4_slave_out_none);
    SETARRITEM(vec_i_xslvo, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_CLINT"), vec_i_xslvo, *axi4_slave_out_none);
    SETARRITEM(vec_i_xslvo, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_PLIC"), vec_i_xslvo, *axi4_slave_out_none);
    SETARRITEM(vec_i_xslvo, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_PBRIDGE"), vec_i_xslvo, *axi4_slave_out_none);
}

void accel_axictrl_bus0_tb::test_proc() {
    IF (AND2(NZ(w_s0_req_write), NZ(w_s0_req_valid)));
        IF (EZ(BIT(wb_s0_req_addr, 25)));
            SETARRITEM(s0_mem0, TO_INT(BITS(wb_s0_req_addr, 5, 2)), s0_mem0, wb_s0_req_wdata);
        ELSE();
            SETARRITEM(s0_mem1, TO_INT(BITS(wb_s0_req_addr, 5, 2)), s0_mem1, wb_s0_req_wdata);
        ENDIF();
    ENDIF();
}

