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
    i_apb_nrst(this, "i_apb_nrst", "1", NO_COMMENT),
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
    o_apbo(this, "o_apbo"),
    _t3_(this, "to debug PIN:"),
    o_ui_nrst(this, "o_ui_nrst", "1", "xilinx generte ddr clock inside ddr controller"),
    o_ui_clk(this, "o_ui_clk", "1", "xilinx generte ddr clock inside ddr controller"),
    o_init_calib_done(this, "o_init_calib_done", "1", NO_COMMENT),
    // signals:
    w_ui_nrst(this, "w_ui_nrst", "1", RSTVAL_ZERO, NO_COMMENT),
    w_ui_clk(this, "w_ui_clk", "1", RSTVAL_ZERO, NO_COMMENT),
    w_init_calib_done(this, "w_init_calib_done", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_device_temp(this, "wb_device_temp", "12", RSTVAL_ZERO, NO_COMMENT),
    w_sr_active(this, "w_sr_active", "1", RSTVAL_ZERO, NO_COMMENT),
    w_ref_ack(this, "w_ref_ack", "1", RSTVAL_ZERO, NO_COMMENT),
    w_zq_ack(this, "w_zq_ack", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_xcfg_unused(this, "wb_xcfg_unused", NO_COMMENT),
    // registers:
    ddr_calib(this, &i_xslv_clk, CLK_POSEDGE, &i_xslv_nrst, ACTIVE_LOW, "ddr_calib", "8", RSTVAL_ZERO, NO_COMMENT),
    // submodule
    clk0(this, "clk0", NO_COMMENT),
    pctrl0(this, "pctrl0", NO_COMMENT),
    sram0(this, "sram0", NO_COMMENT),
    // process
    comb(this)
{
    Operation::start(this);

    clk0.period.setObjValue(new FloatConst(5.0));  // 200 MHz
    NEW(clk0, clk0.getName().c_str());
        CONNECT(clk0, 0, clk0.o_clk, w_ui_clk);
    ENDNEW();

    TEXT();
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
    ENDNEW();

    TEXT();
    sram0.abits.setObjValue(new DecConst(12));
    NEW(sram0, sram0.getName().c_str());
        CONNECT(sram0, 0, sram0.i_clk, w_ui_clk);
        CONNECT(sram0, 0, sram0.i_nrst, w_ui_nrst);
        CONNECT(sram0, 0, sram0.i_mapinfo, i_xmapinfo);
        CONNECT(sram0, 0, sram0.o_cfg, wb_xcfg_unused);
        CONNECT(sram0, 0, sram0.i_xslvi, i_xslvi);
        CONNECT(sram0, 0, sram0.o_xslvo, o_xslvo);
    ENDNEW();

    INITIAL();
        SETZERO(w_ui_nrst);
        SETVAL_DELAY(w_ui_nrst, CONST("1", 1), *new FloatConst(250.0));
    ENDINITIAL();

    Operation::start(&comb);
    proc_comb();
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
