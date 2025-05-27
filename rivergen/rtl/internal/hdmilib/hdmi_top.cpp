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

#include "hdmi_top.h"

hdmi_top::hdmi_top(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "hdmi_top", name, comment),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_hdmi_clk(this, "i_hdmi_clk", "1", "HDMI clock depends on resolution for 1366x768@60Hz ~83MHz"),
    o_hsync(this, "o_hsync", "1", "Horizontal sync strob"),
    o_vsync(this, "o_vsync", "1", "Vertical sync. strob"),
    o_de(this, "o_de", "1", "Data enable strob"),
    o_data(this, "o_data", "18", "Output data in YCbCr format"),
    o_spdif(this, "o_spdif", "1", "Sound channel"),
    i_spdif_out(this, "i_spdif_out", "1", "Reverse sound"),
    i_irq(this, "i_irq", "1", "Interrupt request from HDMI transmitter"),
    // params
    // signals
    w_sync_hsync(this, "w_sync_hsync", "1", RSTVAL_ZERO, NO_COMMENT),
    w_sync_vsync(this, "w_sync_vsync", "1", RSTVAL_ZERO, NO_COMMENT),
    w_sync_de(this, "w_sync_de", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_sync_x(this, "wb_sync_x", "11", RSTVAL_ZERO, NO_COMMENT),
    wb_sync_y(this, "wb_sync_y", "10", RSTVAL_ZERO, NO_COMMENT),
    // registers
    //
    sync0(this, "sync0", NO_COMMENT),
    fb0(this, "fb0", NO_COMMENT),
    comb(this)
{
    Operation::start(this);

    NEW(sync0, sync0.getName().c_str());
        CONNECT(sync0, 0, sync0.i_nrst, i_nrst);
        CONNECT(sync0, 0, sync0.i_clk, i_hdmi_clk);
        CONNECT(sync0, 0, sync0.o_hsync, w_sync_hsync);
        CONNECT(sync0, 0, sync0.o_vsync, w_sync_vsync);
        CONNECT(sync0, 0, sync0.o_de, w_sync_de);
        CONNECT(sync0, 0, sync0.o_x, wb_sync_x);
        CONNECT(sync0, 0, sync0.o_y, wb_sync_y);
    ENDNEW();

    TEXT();
    NEW(fb0, fb0.getName().c_str());
        CONNECT(fb0, 0, fb0.i_nrst, i_nrst);
        CONNECT(fb0, 0, fb0.i_clk, i_hdmi_clk);
        CONNECT(fb0, 0, fb0.i_hsync, w_sync_hsync);
        CONNECT(fb0, 0, fb0.i_vsync, w_sync_vsync);
        CONNECT(fb0, 0, fb0.i_de, w_sync_de);
        CONNECT(fb0, 0, fb0.i_x, wb_sync_x);
        CONNECT(fb0, 0, fb0.i_y, wb_sync_y);
        CONNECT(fb0, 0, fb0.o_hsync, o_hsync);
        CONNECT(fb0, 0, fb0.o_vsync, o_vsync);
        CONNECT(fb0, 0, fb0.o_de, o_de);
        CONNECT(fb0, 0, fb0.o_YCbCr, o_data);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void hdmi_top::proc_comb() {
    ASSIGN(o_spdif, CONST("0", 1));
}


