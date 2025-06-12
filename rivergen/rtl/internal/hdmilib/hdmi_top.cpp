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
    i_hdmi_nrst(this, "i_hdmi_nrst", "1", "Reset: active LOW. Must be HIGH only after DDR "),
    i_hdmi_clk(this, "i_hdmi_clk", "1", "HDMI clock depends on resolution for 1366x768@60Hz ~83MHz"),
    o_hsync(this, "o_hsync", "1", "Horizontal sync strob"),
    o_vsync(this, "o_vsync", "1", "Vertical sync. strob"),
    o_de(this, "o_de", "1", "Data enable strob"),
    o_data(this, "o_data", "18", "Output data in YCbCr format"),
    o_spdif(this, "o_spdif", "1", "Sound channel"),
    i_spdif_out(this, "i_spdif_out", "1", "Reverse sound"),
    i_irq(this, "i_irq", "1", "Interrupt request from HDMI transmitter"),
    _text1_(this, "DMA engine interface - System on Chip interface"),
    o_xmst_cfg(this, "o_xmst_cfg", "HDMI DMA master interface descriptor"),
    i_xmsti(this, "i_xmsti"),
    o_xmsto(this, "o_xmsto"),
    // params
    // signals
    w_sync_hsync(this, "w_sync_hsync", "1", RSTVAL_ZERO, NO_COMMENT),
    w_sync_vsync(this, "w_sync_vsync", "1", RSTVAL_ZERO, NO_COMMENT),
    w_sync_de(this, "w_sync_de", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_sync_x(this, "wb_sync_x", "11", RSTVAL_ZERO, NO_COMMENT),
    wb_sync_y(this, "wb_sync_y", "10", RSTVAL_ZERO, NO_COMMENT),
    wb_sync_xy_total(this, "wb_sync_xy_total", "24", RSTVAL_ZERO, NO_COMMENT),
    w_fb_hsync(this, "w_fb_hsync", "1", RSTVAL_ZERO, NO_COMMENT),
    w_fb_vsync(this, "w_fb_vsync", "1", RSTVAL_ZERO, NO_COMMENT),
    w_fb_de(this, "w_fb_de", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_fb_rgb565(this, "wb_fb_rgb565", "16", RSTVAL_ZERO, NO_COMMENT),
    w_req_mem_ready(this, "w_req_mem_ready", "1", RSTVAL_ZERO, NO_COMMENT),
    w_req_mem_valid(this, "w_req_mem_valid", "1", RSTVAL_ZERO, NO_COMMENT),
    w_req_mem_write(this, "w_req_mem_write", "1", RSTVAL_ZERO, "0=read; 1=write operation"),
    wb_req_mem_bytes(this, "wb_req_mem_bytes", "12", RSTVAL_ZERO, "0=4096 Bytes; 4=DWORD; 8=QWORD; ..."),
    wb_req_mem_addr(this, "wb_req_mem_addr", "24", "'0", "16 MB allocated for framebuffer"),
    wb_req_mem_strob(this, "wb_req_mem_strob", "8", "'0", NO_COMMENT),
    wb_req_mem_data(this, "wb_req_mem_data", "64", "'0", NO_COMMENT),
    w_req_mem_last(this, "w_req_mem_last", "1", RSTVAL_ZERO, NO_COMMENT),
    w_resp_mem_valid(this, "w_resp_mem_valid", "1", RSTVAL_ZERO, NO_COMMENT),
    w_resp_mem_last(this, "w_resp_mem_last", "1", RSTVAL_ZERO, NO_COMMENT),
    w_resp_mem_fault_unused(this, "w_resp_mem_fault_unused", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_resp_mem_addr(this, "wb_resp_mem_addr", "24", "'0", "16 MB allocated for framebuffer"),
    wb_resp_mem_data(this, "wb_resp_mem_data", "64", "'0", NO_COMMENT),
    w_resp_mem_ready(this, "w_resp_mem_ready", "1", RSTVAL_ZERO, NO_COMMENT),
    w_dbg_valid_unused(this, "w_dbg_valid_unused", "1"),
    wb_dbg_payload_unused(this, "wb_dbg_payload_unused", "64"),
    // registers
    //
    sync0(this, "sync0", NO_COMMENT),
    fb0(this, "fb0", NO_COMMENT),
    rgb2y0(this, "rgb2y0", NO_COMMENT),
    xdma0(this, "xdma0", NO_COMMENT),
    comb(this)
{
    Operation::start(this);

    NEW(sync0, sync0.getName().c_str());
        CONNECT(sync0, 0, sync0.i_nrst, i_hdmi_nrst);
        CONNECT(sync0, 0, sync0.i_clk, i_hdmi_clk);
        CONNECT(sync0, 0, sync0.o_hsync, w_sync_hsync);
        CONNECT(sync0, 0, sync0.o_vsync, w_sync_vsync);
        CONNECT(sync0, 0, sync0.o_de, w_sync_de);
        CONNECT(sync0, 0, sync0.o_x, wb_sync_x);
        CONNECT(sync0, 0, sync0.o_y, wb_sync_y);
        CONNECT(sync0, 0, sync0.o_xy_total, wb_sync_xy_total);
    ENDNEW();

    TEXT();
    NEW(fb0, fb0.getName().c_str());
        CONNECT(fb0, 0, fb0.i_nrst, i_hdmi_nrst);
        CONNECT(fb0, 0, fb0.i_clk, i_hdmi_clk);
        CONNECT(fb0, 0, fb0.i_hsync, w_sync_hsync);
        CONNECT(fb0, 0, fb0.i_vsync, w_sync_vsync);
        CONNECT(fb0, 0, fb0.i_de, w_sync_de);
        CONNECT(fb0, 0, fb0.i_x, wb_sync_x);
        CONNECT(fb0, 0, fb0.i_y, wb_sync_y);
        CONNECT(fb0, 0, fb0.i_xy_total, wb_sync_xy_total);
        CONNECT(fb0, 0, fb0.o_hsync, w_fb_hsync);
        CONNECT(fb0, 0, fb0.o_vsync, w_fb_vsync);
        CONNECT(fb0, 0, fb0.o_de, w_fb_de);
        CONNECT(fb0, 0, fb0.o_rgb565, wb_fb_rgb565);
        CONNECT(fb0, 0, fb0.i_req_2d_ready, w_req_mem_ready);
        CONNECT(fb0, 0, fb0.o_req_2d_valid, w_req_mem_valid);
        CONNECT(fb0, 0, fb0.o_req_2d_bytes, wb_req_mem_bytes);
        CONNECT(fb0, 0, fb0.o_req_2d_addr, wb_req_mem_addr);
        CONNECT(fb0, 0, fb0.i_resp_2d_valid, w_resp_mem_valid);
        CONNECT(fb0, 0, fb0.i_resp_2d_last, w_resp_mem_last);
        CONNECT(fb0, 0, fb0.i_resp_2d_addr, wb_resp_mem_addr);
        CONNECT(fb0, 0, fb0.i_resp_2d_data, wb_resp_mem_data);
        CONNECT(fb0, 0, fb0.o_resp_2d_ready, w_resp_mem_ready);
    ENDNEW();

    TEXT();
    NEW(rgb2y0, rgb2y0.getName().c_str());
        CONNECT(rgb2y0, 0, rgb2y0.i_nrst, i_hdmi_nrst);
        CONNECT(rgb2y0, 0, rgb2y0.i_clk, i_hdmi_clk);
        CONNECT(rgb2y0, 0, rgb2y0.i_rgb565, wb_fb_rgb565);
        CONNECT(rgb2y0, 0, rgb2y0.i_hsync, w_fb_hsync);
        CONNECT(rgb2y0, 0, rgb2y0.i_vsync, w_fb_vsync);
        CONNECT(rgb2y0, 0, rgb2y0.i_de, w_fb_de);
        CONNECT(rgb2y0, 0, rgb2y0.o_ycbcr422, o_data);
        CONNECT(rgb2y0, 0, rgb2y0.o_hsync, o_hsync);
        CONNECT(rgb2y0, 0, rgb2y0.o_vsync, o_vsync);
        CONNECT(rgb2y0, 0, rgb2y0.o_de, o_de);
    ENDNEW();

    TEXT();
    xdma0.abits.setObjValue(new DecConst(24));
    xdma0.userbits.setObjValue(new DecConst(1));
    xdma0.base_offset.setObjValue(new HexLogicConst(SCV_get_cfg_type(this, "CFG_SYSBUS_ADDR_BITS"), 0x80000000));
    NEW(xdma0, xdma0.getName().c_str());
        CONNECT(xdma0, 0, xdma0.i_nrst, i_nrst);
        CONNECT(xdma0, 0, xdma0.i_clk, i_clk);
        CONNECT(xdma0, 0, xdma0.o_req_mem_ready, w_req_mem_ready);
        CONNECT(xdma0, 0, xdma0.i_req_mem_valid, w_req_mem_valid);
        CONNECT(xdma0, 0, xdma0.i_req_mem_write, w_req_mem_write);
        CONNECT(xdma0, 0, xdma0.i_req_mem_bytes, wb_req_mem_bytes);
        CONNECT(xdma0, 0, xdma0.i_req_mem_addr, wb_req_mem_addr);
        CONNECT(xdma0, 0, xdma0.i_req_mem_strob, wb_req_mem_strob);
        CONNECT(xdma0, 0, xdma0.i_req_mem_data, wb_req_mem_data);
        CONNECT(xdma0, 0, xdma0.i_req_mem_last, w_req_mem_last);
        CONNECT(xdma0, 0, xdma0.o_resp_mem_valid, w_resp_mem_valid);
        CONNECT(xdma0, 0, xdma0.o_resp_mem_last, w_resp_mem_last);
        CONNECT(xdma0, 0, xdma0.o_resp_mem_fault, w_resp_mem_fault_unused);
        CONNECT(xdma0, 0, xdma0.o_resp_mem_addr, wb_resp_mem_addr);
        CONNECT(xdma0, 0, xdma0.o_resp_mem_data, wb_resp_mem_data);
        CONNECT(xdma0, 0, xdma0.i_resp_mem_ready, w_resp_mem_ready);
        CONNECT(xdma0, 0, xdma0.i_msti, i_xmsti);
        CONNECT(xdma0, 0, xdma0.o_msto, o_xmsto);
        CONNECT(xdma0, 0, xdma0.o_dbg_valid, w_dbg_valid_unused);
        CONNECT(xdma0, 0, xdma0.o_dbg_payload, wb_dbg_payload_unused);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void hdmi_top::proc_comb() {
    SETVAL(comb.vb_xmst_cfg.descrsize, *SCV_get_cfg_type(this, "PNP_CFG_DEV_DESCR_BYTES"));
    SETVAL(comb.vb_xmst_cfg.descrtype, *SCV_get_cfg_type(this, "PNP_CFG_TYPE_MASTER"));
    SETVAL(comb.vb_xmst_cfg.vid, *SCV_get_cfg_type(this, "VENDOR_OPTIMITECH"));
    SETVAL(comb.vb_xmst_cfg.did, *SCV_get_cfg_type(this, "OPTIMITECH_HDMI_DMA"));

    SETVAL(o_xmst_cfg, comb.vb_xmst_cfg);

    ASSIGN(o_spdif, CONST("0", 1));
    ASSIGN(w_req_mem_write, CONST("0", 1), "Always read");
    ASSIGN(wb_req_mem_strob, CONST("'0", 8));
    ASSIGN(wb_req_mem_data, ALLZEROS());
    ASSIGN(w_req_mem_last, CONST("0", 1));
}


