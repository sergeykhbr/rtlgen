// 
//  Copyright 2023 Sergey Khabarov, sergeykhbr@gmail.com
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

#include "ddr3_tech.h"

ddr3_tech::ddr3_tech(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "ddr3_tech", name, comment),
    ROW_BITS(this, "ROW_BITS", "14", "Row address width"),
    BA_BITS(this, "BA_BITS", "3", "Bank address width"),
    LANE_TOTAL(this, "LANE_TOTAL", "8", "Lane is: 8 dq (byte) + 1 dm (mask) + 1 dqs_n/dqs_p (strob)"),
    DUAL_RANK(this, "DUAL_RANK", "0", "1=Enable dual DDR; 0=disable."),
    AXI_SIZE_LOG2(this, "AXI_SIZE_LOG2", "30", "30 = 1GB (default). Bus address width"),
    AXI_ID_BITS(this, "AXI_ID_BITS", "5", "Custom ID field width"),
    AXI_USER_BITS(this, "AXI_USER_BITS", "1", "Custom user field width (unused/nullified)"),
    /*i_apb_nrst(this, "i_apb_nrst", "1", NO_COMMENT),
    i_apb_clk(this, "i_apb_clk", "1", NO_COMMENT),
    i_xslv_nrst(this, "i_xslv_nrst", "1", NO_COMMENT),
    i_xslv_clk(this, "i_xslv_clk", "1", NO_COMMENT),
    _t0_(this, "AXI memory access (ddr clock)"),
    i_xmapinfo(this, "i_xmapinfo"),
    o_xcfg(this, "o_xcfg"),
    i_xslvi(this, "i_xslvi"),
    o_xslvo(this, "o_xslvo"),
    _t1_(this, "APB control interface (sys clock):"),
    i_pmapinfo(this, "i_pmapinfo"),
    o_pcfg(this, "o_pcfg"),
    i_apbi(this, "i_apbi"),
    o_apbo(this, "o_apbo"),*/
    i_nrst(this, "i_nrst", "1", "DDR reset active LOW. Connected to IODELAY"),
    _t3_(this, "to debug PIN:"),
    o_ui_nrst(this, "o_ui_nrst", "1", "xilinx generte ddr clock inside ddr controller"),
    o_ui_clk(this, "o_ui_clk", "1", "xilinx generte ddr clock inside ddr controller"),
    _t4_(this, "DDR signals:"),
    o_ddr3_reset_n(this, "o_ddr3_reset_n", "1", NO_COMMENT),
    o_ddr3_ck_n(this, "o_ddr3_ck_n", "DUAL_RANK", NO_COMMENT),
    o_ddr3_ck_p(this, "o_ddr3_ck_p", "DUAL_RANK", NO_COMMENT),
    o_ddr3_cke(this, "o_ddr3_cke", "DUAL_RANK", NO_COMMENT),
    o_ddr3_cs_n(this, "o_ddr3_cs_n", "DUAL_RANK", "Chip select active LOW"),
    o_ddr3_ras_n(this, "o_ddr3_ras_n", "1", NO_COMMENT),
    o_ddr3_cas_n(this, "o_ddr3_cas_n", "1", NO_COMMENT),
    o_ddr3_we_n(this, "o_ddr3_we_n", "1", "Write enable active LOW"),
    o_ddr3_dm(this, "o_ddr3_dm", "LANE_TOTAL", "Data mask"),
    o_ddr3_ba(this, "o_ddr3_ba", "BA_BITS", "Bank address"),
    o_ddr3_addr(this, "o_ddr3_addr", "ROW_BITS", NO_COMMENT),
    io_ddr3_dq(this, "io_ddr3_dq", "MUL(8,LANE_TOTAL)", NO_COMMENT),
    io_ddr3_dqs_n(this, "io_ddr3_dqs_n", "LANE_TOTAL", "Data strob positive"),
    io_ddr3_dqs_p(this, "io_ddr3_dqs_p", "LANE_TOTAL", "Data strob negative"),
    o_ddr3_odt(this, "o_ddr3_odt", "DUAL_RANK", "on-die termination"),
    o_init_calib_done(this, "o_init_calib_done", "1", "DDR calibration done, active HIGH"),
    i_sr_req(this, "i_sr_req", "1", "Self-refresh request (low-power mode)"),
    i_ref_req(this, "i_ref_req", "1", "Periodic refresh request ~7.8 us"),
    i_zq_req(this, "i_zq_req", "1", "ZQ calibration request. Startup and runtime maintenance"),
    o_sr_active(this, "o_sr_active", "1", "Self-resfresh is active (low-power mode or sleep)"),
    o_ref_ack(this, "o_ref_ack", "1", "Refresh request acknoledged"),
    o_zq_ack(this, "o_zq_ack", "1", "ZQ calibration request acknowledged"),
    _t5_(this, "AXI slave interface:"),
    i_aw_id(this, "i_aw_id", "AXI_ID_BITS", NO_COMMENT),
    i_aw_addr(this, "i_aw_addr", "POW2(1,AXI_SIZE_LOG2)", NO_COMMENT),
    i_aw_len(this, "i_aw_len", "8", NO_COMMENT),
    i_aw_size(this, "i_aw_size", "3", NO_COMMENT),
    i_aw_burst(this, "i_aw_burst", "2", NO_COMMENT),
    i_aw_lock(this, "i_aw_lock", "1", NO_COMMENT),
    i_aw_cache(this, "i_aw_cache", "4", NO_COMMENT),
    i_aw_prot(this, "i_aw_prot", "3", NO_COMMENT),
    i_aw_qos(this, "i_aw_qos", "4", NO_COMMENT),
    i_aw_valid(this, "i_aw_valid", "1", NO_COMMENT),
    o_aw_ready(this, "o_aw_ready", "1", NO_COMMENT),
    i_w_data(this, "i_w_data", "64", NO_COMMENT),
    i_w_strb(this, "i_w_strb", "8", NO_COMMENT),
    i_w_last(this, "i_w_last", "1", NO_COMMENT),
    i_w_valid(this, "i_w_valid", "1", NO_COMMENT),
    o_w_ready(this, "o_w_ready", "1", NO_COMMENT),
    i_b_ready(this, "i_b_ready", "1", NO_COMMENT),
    o_b_id(this, "o_b_id", "AXI_ID_BITS", NO_COMMENT),
    o_b_resp(this, "o_b_resp", "2", NO_COMMENT),
    o_b_valid(this, "o_b_valid", "1", NO_COMMENT),
    i_ar_id(this, "i_ar_id", "AXI_ID_BITS", NO_COMMENT),
    i_ar_addr(this, "i_ar_addr", "POW2(1,AXI_SIZE_LOG2)", NO_COMMENT),
    i_ar_len(this, "i_ar_len", "8", NO_COMMENT),
    i_ar_size(this, "i_ar_size", "3", NO_COMMENT),
    i_ar_burst(this, "i_ar_burst", "2", NO_COMMENT),
    i_ar_lock(this, "i_ar_lock", "1", NO_COMMENT),
    i_ar_cache(this, "i_ar_cache", "4", NO_COMMENT),
    i_ar_prot(this, "i_ar_prot", "3", NO_COMMENT),
    i_ar_qos(this, "i_ar_qos", "4", NO_COMMENT),
    i_ar_valid(this, "i_ar_valid", "1", NO_COMMENT),
    o_ar_ready(this, "o_ar_ready", "1", NO_COMMENT),
    i_r_ready(this, "i_r_ready", "1", NO_COMMENT),
    o_r_id(this, "o_r_id", "AXI_ID_BITS", NO_COMMENT),
    o_r_data(this, "o_r_data", "64", NO_COMMENT),
    o_r_resp(this, "o_r_resp", "2", NO_COMMENT),
    o_r_last(this, "o_r_last", "1", NO_COMMENT),
    o_r_valid(this, "o_r_valid", "1", NO_COMMENT),
    // signals:
    w_ui_nrst(this, "w_ui_nrst", "1", RSTVAL_ZERO, NO_COMMENT),
    w_ui_clk(this, "w_ui_clk", "1", RSTVAL_ZERO, NO_COMMENT),
    w_init_calib_done(this, "w_init_calib_done", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_device_temp(this, "wb_device_temp", "12", RSTVAL_ZERO, NO_COMMENT),
    w_sr_active(this, "w_sr_active", "1", RSTVAL_ZERO, NO_COMMENT),
    w_ref_ack(this, "w_ref_ack", "1", RSTVAL_ZERO, NO_COMMENT),
    w_zq_ack(this, "w_zq_ack", "1", RSTVAL_ZERO, NO_COMMENT),
    //wb_xcfg_unused(this, "wb_xcfg_unused", NO_COMMENT),
    // registers:
    ddr_calib(this, &w_ui_clk, CLK_POSEDGE, &w_ui_nrst, ACTIVE_LOW, "ddr_calib", "8", RSTVAL_ZERO, NO_COMMENT),
    mem0(this, "mem0", "64", "16", NO_COMMENT),
    // submodule
    clk0(this, "clk0", NO_COMMENT),
    //pctrl0(this, "pctrl0", NO_COMMENT),
    //sram0(this, "sram0", NO_COMMENT),
    // process
    comb(this),
    ff(parent, "ff", &w_ui_clk, CLK_POSEDGE, 0, ACTIVE_NONE, NO_COMMENT)
{
    io_ddr3_dq.setAttribute(ATTR_UNCHECKED_WRITERS);
    io_ddr3_dqs_n.setAttribute(ATTR_UNCHECKED_WRITERS);
    io_ddr3_dqs_p.setAttribute(ATTR_UNCHECKED_WRITERS);

    Operation::start(this);

    clk0.period.setObjValue(new FloatConst(5.0));  // 200 MHz
    NEW(clk0, clk0.getName().c_str());
        CONNECT(clk0, 0, clk0.o_clk, w_ui_clk);
    ENDNEW();

    /*TEXT();
    NEW(pctrl0, pctrl0.getName().c_str());
        CONNECT(pctrl0, 0, pctrl0.i_clk, i_apb_clk);
        CONNECT(pctrl0, 0, pctrl0.i_nrst, i_apb_nrst);
        CONNECT(pctrl0, 0, pctrl0.i_mapinfo, i_pmapinfo);
        CONNECT(pctrl0, 0, pctrl0.o_cfg, o_pcfg);
        CONNECT(pctrl0, 0, pctrl0.i_apbi, i_apbi);
        CONNECT(pctrl0, 0, pctrl0.o_apbo, o_apbo);
        CONNECT(pctrl0, 0, pctrl0.i_pll_locked, w_ui_nrst);
        CONNECT(pctrl0, 0, pctrl0.i_init_calib_done, w_init_calib_done);
        CONNECT(pctrl0, 0, pctrl0.i_device_temp, wb_device_temp);
        CONNECT(pctrl0, 0, pctrl0.i_sr_active, w_sr_active);
        CONNECT(pctrl0, 0, pctrl0.i_ref_ack, w_ref_ack);
        CONNECT(pctrl0, 0, pctrl0.i_zq_ack, w_zq_ack);
    ENDNEW();*/

    /*TEXT();
    sram0.abits.setObjValue(new DecConst(12));
    NEW(sram0, sram0.getName().c_str());
        CONNECT(sram0, 0, sram0.i_clk, w_ui_clk);
        CONNECT(sram0, 0, sram0.i_nrst, w_ui_nrst);
        CONNECT(sram0, 0, sram0.i_mapinfo, i_xmapinfo);
        CONNECT(sram0, 0, sram0.o_cfg, wb_xcfg_unused);
        CONNECT(sram0, 0, sram0.i_xslvi, i_xslvi);
        CONNECT(sram0, 0, sram0.o_xslvo, o_xslvo);
    ENDNEW();*/

    INITIAL();
        SETZERO(w_ui_nrst);
        SETVAL_DELAY(w_ui_nrst, CONST("1", 1), *new FloatConst(250.0));
    ENDINITIAL();

    Operation::start(&comb);
    proc_comb();

    Operation::start(&ff);
    proc_ff();
}

void ddr3_tech::proc_comb() {
    IF (EZ(BIT(ddr_calib, 7)));
        SETVAL(ddr_calib, INC(ddr_calib));
    ENDIF();

    TEXT();
    SETVAL(w_init_calib_done, BIT(ddr_calib, 7));
    SETVAL(o_ui_nrst, w_ui_nrst);
    SETVAL(o_ui_clk, w_ui_clk);
    SETVAL(o_init_calib_done, w_init_calib_done);
}

void ddr3_tech::proc_ff() {
}