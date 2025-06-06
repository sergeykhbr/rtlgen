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

#include "jtag_app.h"

jtag_app::jtag_app(GenObject *parent, const char *name) :
    ModuleObject(parent, "jtag_app", name, NO_COMMENT),
    // parameters
    // Ports
    i_nrst(this, "i_nrst", "1", "1", "Power-on system reset active LOW"),
    w_clk1(this, "w_clk1", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_clk1_cnt(this, "wb_clk1_cnt", "32", "'0", NO_COMMENT),
    w_slv_o_req_valid(this, "w_slv_o_req_valid", "1"),
    wb_slv_o_req_addr(this, "wb_slv_o_req_addr", "CFG_SYSBUS_ADDR_BITS"),
    wb_slv_o_req_size(this, "wb_slv_o_req_size", "8"),
    w_slv_o_req_write(this, "w_slv_o_req_write", "1"),
    wb_slv_o_req_wdata(this, "wb_slv_o_req_wdata", "CFG_SYSBUS_DATA_BITS"),
    wb_slv_o_req_wstrb(this, "wb_slv_o_req_wstrb", "CFG_SYSBUS_DATA_BYTES"),
    w_slv_o_req_last(this, "w_slv_o_req_last", "1"),
    w_slv_i_req_ready(this, "w_slv_i_req_ready", "1"),
    w_slv_i_resp_valid(this, "w_slv_i_resp_valid", "1"),
    wb_slv_i_resp_rdata(this, "wb_slv_i_resp_rdata", "CFG_SYSBUS_DATA_BITS"),
    w_slv_i_resp_err(this, "w_slv_i_resp_err", "1"),
    // submodules:
    clk1(this, "clk1", NO_COMMENT),
    // processes:
    comb(this),
    test_clk1(this, &w_clk1)
{
    Operation::start(this);

    // Create and connet Sub-modules:
    clk1.period.setObjValue(new FloatConst(50.0));  // 20 MHz
    NEW(clk1, clk1.getName().c_str());
        CONNECT(clk1, 0, clk1.o_clk, w_clk1);
    ENDNEW();

    /*TEXT();
    NEW(tt, tt.getName().c_str());
        CONNECT(tt, 0, tt.i_nrst, i_nrst);
        CONNECT(tt, 0, tt.i_clk, w_clk1);
        CONNECT(tt, 0, tt.i_hdmi_clk, w_clk2);
        CONNECT(tt, 0, tt.o_hsync, w_hdmi_hsync);
        CONNECT(tt, 0, tt.o_vsync, w_hdmi_vsync);
        CONNECT(tt, 0, tt.o_de, w_hdmi_de);
        CONNECT(tt, 0, tt.o_data, wb_hdmi_data);
        CONNECT(tt, 0, tt.o_spdif, w_hdmi_spdif);
        CONNECT(tt, 0, tt.i_spdif_out, w_hdmi_spdif_out);
        CONNECT(tt, 0, tt.i_irq, w_hdmi_irq);
        CONNECT(tt, 0, tt.o_xmst_cfg, wb_mst_o_cfg);
        CONNECT(tt, 0, tt.i_xmsti, wb_xmsti);
        CONNECT(tt, 0, tt.o_xmsto, wb_xmsto);
    ENDNEW();*/


    Operation::start(&comb);
    proc_comb();

    Operation::start(&test_clk1);
    proc_test_clk1();

}

void jtag_app::proc_comb() {
}


void jtag_app::proc_test_clk1() {
    SETVAL(wb_clk1_cnt, INC(wb_clk1_cnt));
    IF (LS(wb_clk1_cnt, CONST("10")));
        SETZERO(i_nrst);
    ELSE();
        SETONE(i_nrst);
    ENDIF();

    TEXT();
}

