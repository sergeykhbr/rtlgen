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
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_hsync(this, "i_hsync", "1", "Horizontal sync"),
    i_vsync(this, "i_vsync", "1", "Vertical sync"),
    i_de(this, "i_de", "1", "data enable"),
    i_x(this, "", "11", "x-pixel"),
    i_y(this, "", "10", "y-pixel"),
    o_hsync(this, "o_hsync", "1", "delayed horizontal sync"),
    o_vsync(this, "o_vsync", "1", "delayed vertical sync"),
    o_de(this, "o_de", "1", "delayed data enable"),
    o_YCbCr0(this, "o_YCbCr0", "16", "YCbCr odd pixel"),
    o_YCbCr1(this, "o_YCbCr1", "16", "YCbCr even pixel"),
    // params
    // signals
    // registers
    h_sync(this, "h_sync", "1", RSTVAL_ZERO, NO_COMMENT),
    v_sync(this, "v_sync", "1", RSTVAL_ZERO, NO_COMMENT),
    de(this, "de", "1", RSTVAL_ZERO, NO_COMMENT),
    Y0(this, "Y0", "8", RSTVAL_ZERO, NO_COMMENT),
    Y1(this, "Y1", "8", RSTVAL_ZERO, NO_COMMENT),
    Cb(this, "Cb", "8", RSTVAL_ZERO, NO_COMMENT),
    Cr(this, "Cr", "8", RSTVAL_ZERO, NO_COMMENT),
    //
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void framebuf::proc_comb() {
    TEXT("delayed signals:");
    SETVAL(de, i_de);
    SETVAL(h_sync, i_hsync);
    SETVAL(v_sync, i_vsync);

    IF (LS(i_x, CONST("170", 11)));
        TEXT("White");
        SETVAL(Y0, CONST("235", 8));
        SETVAL(Y1, CONST("235", 8));
        SETVAL(Cb, CONST("128", 8));
        SETVAL(Cr, CONST("128", 8));
    ELSIF (LS(i_x, CONST("340", 11)));
        TEXT("Black");
        SETVAL(Y0, CONST("219", 8));
        SETVAL(Y1, CONST("219", 8));
        SETVAL(Cb, CONST("16", 8));
        SETVAL(Cr, CONST("16", 8));
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
        SETVAL(Cr, CONST("234", 8));
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
        SETVAL(Y0, CONST("105", 8));
        SETVAL(Y1, CONST("105", 8));
        SETVAL(Cb, CONST("212", 8));
        SETVAL(Cr, CONST("234", 8));
    ELSE();
        TEXT("Magneta");
        SETVAL(Y0, CONST("170", 8));
        SETVAL(Y1, CONST("170", 8));
        SETVAL(Cb, CONST("166", 8));
        SETVAL(Cr, CONST("16", 8));
    ENDIF();

    TEXT();
    SYNC_RESET(this);

    TEXT();
    SETVAL(o_hsync, h_sync);
    SETVAL(o_vsync, v_sync);
    SETVAL(o_de, de);
    SETVAL(o_YCbCr0, CC2(Y0, Cb));
    SETVAL(o_YCbCr1, CC2(Y1, Cr));
}

