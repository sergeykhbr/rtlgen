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

#include "framebuf.h"

framebuf::framebuf(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "framebuf", name, comment),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_hsync(this, "i_hsync", "1", "Horizontal sync"),
    i_vsync(this, "i_vsync", "1", "Vertical sync"),
    i_de(this, "i_de", "1", "data enable"),
    i_x(this, "i_x", "11", "x-pixel"),
    i_y(this, "i_y", "10", "y-pixel"),
    o_hsync(this, "o_hsync", "1", "delayed horizontal sync"),
    o_vsync(this, "o_vsync", "1", "delayed vertical sync"),
    o_de(this, "o_de", "1", "delayed data enable"),
    o_YCbCr(this, "o_YCbCr", "18", "YCbCr multiplexed odd/even pixels"),
    _dma0_(this, "DMA engine compatible interface (always read). Get pixels array:"),
    i_req_2d_ready(this, "i_req_2d_ready", "1", "2D pixels ready to accept request"),
    o_req_2d_valid(this, "o_req_2d_valid", "1", "2D pixels request is valid"),
    o_req_2d_bytes(this, "o_req_2d_bytes", "12", "0=4096 Bytes; 4=DWORD; 8=QWORD; ..."),
    o_req_2d_addr(this, "o_req_2d_addr", "24", "16 MB allocated for framebuffer"),
    i_resp_2d_valid(this, "i_resp_2d_valid", "1", "2D pixels buffer response is valid"),
    i_resp_2d_last(this, "i_resp_2d_last", "1", "Last data in burst read"),
    i_resp_2d_addr(this, "i_resp_2d_addr", "24", "16 MB allocated for framebuffer"),
    i_resp_2d_data(this, "i_resp_2d_data", "64", "Read data"),
    o_resp_2d_ready(this, "o_resp_2d_ready", "1", "Ready to accept 2D pixels response"),
    // params
    // signals
    wb_ping_addr(this, "wb_ping_addr", "8", "'0", NO_COMMENT),
    w_ping_wena(this, "w_ping_wena", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_ping_wdata(this, "wb_ping_wdata", "64", "'0", NO_COMMENT),
    wb_ping_rdata(this, "wb_ping_rdata", "64", "'0", NO_COMMENT),
    wb_pong_addr(this, "wb_pong_addr", "8", "'0", NO_COMMENT),
    w_pong_wena(this, "w_pong_wena", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_pong_wdata(this, "wb_pong_wdata", "64", "'0", NO_COMMENT),
    wb_pong_rdata(this, "wb_pong_rdata", "64", "'0", NO_COMMENT),
    // registers
    pix_x0(this, "pix_x0", "1", RSTVAL_ZERO, NO_COMMENT),
    h_sync(this, "h_sync", "2", RSTVAL_ZERO, NO_COMMENT),
    v_sync(this, "v_sync", "2", RSTVAL_ZERO, NO_COMMENT),
    de(this, "de", "2", RSTVAL_ZERO, NO_COMMENT),
    Y0(this, "Y0", "8", RSTVAL_ZERO, NO_COMMENT),
    Y1(this, "Y1", "8", RSTVAL_ZERO, NO_COMMENT),
    Cb(this, "Cb", "8", RSTVAL_ZERO, NO_COMMENT),
    Cr(this, "Cr", "8", RSTVAL_ZERO, NO_COMMENT),
    YCbCr(this, "YCbCr", "16", "'0", NO_COMMENT),
    // modules
    ping(this, "ping", NO_COMMENT),
    pong(this, "pong", NO_COMMENT),
    // processes
    comb(this)
{
    Operation::start(this);

    ping.abits.setObjValue(new DecConst(8));
    ping.dbits.setObjValue(new DecConst(64));
    NEW(ping, ping.getName().c_str());
        CONNECT(ping, 0, ping.i_clk, i_clk);
        CONNECT(ping, 0, ping.i_addr, wb_ping_addr);
        CONNECT(ping, 0, ping.i_wena, w_ping_wena);
        CONNECT(ping, 0, ping.i_wdata, wb_ping_wdata);
        CONNECT(ping, 0, ping.o_rdata, wb_ping_rdata);
    ENDNEW();

    TEXT();
    pong.abits.setObjValue(new DecConst(8));
    pong.dbits.setObjValue(new DecConst(64));
    NEW(pong, pong.getName().c_str());
        CONNECT(pong, 0, pong.i_clk, i_clk);
        CONNECT(pong, 0, pong.i_addr, wb_pong_addr);
        CONNECT(pong, 0, pong.i_wena, w_pong_wena);
        CONNECT(pong, 0, pong.i_wdata, wb_pong_wdata);
        CONNECT(pong, 0, pong.o_rdata, wb_pong_rdata);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void framebuf::proc_comb() {
    TEXT("delayed signals:");
    SETVAL(de, CC2(BIT(de, 0), i_de));
    SETVAL(h_sync, CC2(BIT(h_sync, 0), i_hsync));
    SETVAL(v_sync, CC2(BIT(v_sync, 0), i_vsync));
    SETVAL(pix_x0, BIT(i_x, 0));

    TEXT();
    SETVAL(comb.fb_addr, CC2(i_y, BITS(i_x, 10, 2)));

    TEXT();
    IF (EQ(BITS(i_x, 1, 0), CONST("0",2)));
    ELSIF (EQ(BITS(i_x, 1, 0), CONST("1",2)));
    ELSIF (EQ(BITS(i_x, 1, 0), CONST("2",2)));
    ELSE();
    ENDIF();

    /*
    IF (LS(i_x, CONST("170", 11)));
        TEXT("White");
        SETVAL(Y0, CONST("235", 8));
        SETVAL(Y1, CONST("235", 8));
        SETVAL(Cb, CONST("128", 8));
        SETVAL(Cr, CONST("128", 8));
    ELSIF (LS(i_x, CONST("340", 11)));
        TEXT("Black");
        SETVAL(Y0, CONST("16", 8));
        SETVAL(Y1, CONST("16", 8));
        SETVAL(Cb, CONST("128", 8));
        SETVAL(Cr, CONST("128", 8));
    ELSIF (LS(i_x, CONST("510", 11)));
        TEXT("Red");
        SETVAL(Y0, CONST("82", 8));
        SETVAL(Y1, CONST("82", 8));
        SETVAL(Cb, CONST("90", 8));
        SETVAL(Cr, CONST("240", 8));
    ELSIF (LS(i_x, CONST("680", 11)));
        TEXT("Green");
        SETVAL(Y0, CONST("145", 8));
        SETVAL(Y1, CONST("145", 8));
        SETVAL(Cb, CONST("54", 8));
        SETVAL(Cr, CONST("34", 8));
    ELSIF (LS(i_x, CONST("850", 11)));
        TEXT("Blue");
        SETVAL(Y0, CONST("41", 8));
        SETVAL(Y1, CONST("41", 8));
        SETVAL(Cb, CONST("240", 8));
        SETVAL(Cr, CONST("110", 8));
    ELSIF (LS(i_x, CONST("1020", 11)));
        TEXT("Yellow");
        SETVAL(Y0, CONST("200", 8));
        SETVAL(Y1, CONST("200", 8));
        SETVAL(Cb, CONST("16", 8));
        SETVAL(Cr, CONST("146", 8));
    ELSIF (LS(i_x, CONST("1190", 11)));
        TEXT("Cyan");
        SETVAL(Y0, CONST("170", 8));
        SETVAL(Y1, CONST("170", 8));
        SETVAL(Cb, CONST("166", 8));
        SETVAL(Cr, CONST("16", 8));
    ELSE();
        TEXT("Magneta");
        SETVAL(Y0, CONST("106", 8));
        SETVAL(Y1, CONST("106", 8));
        SETVAL(Cb, CONST("102", 8));
        SETVAL(Cr, CONST("222", 8));
    ENDIF();*/

    TEXT();
    TEXT("See style 1 output:");
    IF (EZ(pix_x0));
        SETVAL(YCbCr, CC2(Cb, Y1));
    ELSE();
        SETVAL(YCbCr, CC2(Cr, Y1));
    ENDIF();

    TEXT();
    SYNC_RESET();

    TEXT();
    SETVAL(o_hsync, h_sync);
    SETVAL(o_vsync, v_sync);
    SETVAL(o_de, de);
    SETVAL(o_YCbCr, CC2(CONST("0", 2), YCbCr));
}

