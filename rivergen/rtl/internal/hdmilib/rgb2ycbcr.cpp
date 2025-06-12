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

#include "rgb2ycbcr.h"

rgb2ycbcr::rgb2ycbcr(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "rgb2ycbcr", name, comment),
    // IO
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_rgb565(this, "i_rgb565", "16", "RGB 16-bits input"),
    i_hsync(this, "i_hsync", "1", "Horizontal sync"),
    i_vsync(this, "i_vsync", "1", "Vertical sync"),
    i_de(this, "i_de", "1", "data enable"),
    o_ycbcr422(this, "o_ycbcr422", "18", "YCbCr 16-bits 4:2:2 format plus 2 extra bits"),
    o_hsync(this, "o_hsync", "1", "delayed horizontal sync"),
    o_vsync(this, "o_vsync", "1", "delayed vertical sync"),
    o_de(this, "o_de", "1", "delayed data enable"),
    // params
    // signals
    w_signed(this, "w_signed", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_v0(this, "wb_v0", "8", RSTVAL_ZERO, NO_COMMENT),
    wb_v1(this, "wb_v1", "8", RSTVAL_ZERO, NO_COMMENT),
    wb_v2(this, "wb_v2", "8", RSTVAL_ZERO, NO_COMMENT),
    wb_v3(this, "wb_v3", "8", RSTVAL_ZERO, NO_COMMENT),
    wb_m(this, "wb_m", "128", RSTVAL_ZERO, NO_COMMENT),
    wb_res0(this, "wb_res0", "18", RSTVAL_ZERO, NO_COMMENT),
    wb_res1(this, "wb_res1", "18", RSTVAL_ZERO, NO_COMMENT),
    wb_res2(this, "wb_res2", "18", RSTVAL_ZERO, NO_COMMENT),
    wb_res3(this, "wb_res3", "18", RSTVAL_ZERO, NO_COMMENT),
    // registers
    hsync(this, "hsync", "8", "'0", NO_COMMENT),
    vsync(this, "vsync", "8", "'0", NO_COMMENT),
    de(this, "de", "8", "'0", NO_COMMENT),
    even(this, "even", "7", "'0", "HIGH on samples 0, 2, 4, ..."),
    ycbcr422(this, "ycbcr422", "16", "'0", NO_COMMENT),
    //
    im8(this, "im8", NO_COMMENT),
    comb(this)
{
    Operation::start(this);

    NEW(im8, im8.getName().c_str());
        CONNECT(im8, 0, im8.i_nrst, i_nrst);
        CONNECT(im8, 0, im8.i_clk, i_clk);
        CONNECT(im8, 0, im8.i_signed, w_signed);
        CONNECT(im8, 0, im8.i_v0, wb_v0);
        CONNECT(im8, 0, im8.i_v1, wb_v1);
        CONNECT(im8, 0, im8.i_v2, wb_v2);
        CONNECT(im8, 0, im8.i_v3, wb_v3);
        CONNECT(im8, 0, im8.i_m, wb_m);
        CONNECT(im8, 0, im8.o_res0, wb_res0);
        CONNECT(im8, 0, im8.o_res1, wb_res1);
        CONNECT(im8, 0, im8.o_res2, wb_res2);
        CONNECT(im8, 0, im8.o_res3, wb_res3);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void rgb2ycbcr::proc_comb() {
    SETVAL(wb_m, CCx(16, &comb.vb_m33,
                         &comb.vb_m32,
                         &comb.vb_m31,
                         &comb.vb_m30,
                         &comb.vb_m23,
                         &comb.vb_m22,
                         &comb.vb_m21,
                         &comb.vb_m20,
                         &comb.vb_m13,
                         &comb.vb_m12,
                         &comb.vb_m11,
                         &comb.vb_m10,
                         &comb.vb_m03,
                         &comb.vb_m02,
                         &comb.vb_m01,
                         &comb.vb_m00));


    TEXT();
    SETVAL(w_signed, CONST("1", 1), "Signed operation -127..+127");
    SETVAL(wb_v0, CC3(CONST("0", 1), BITS(i_rgb565, 15, 11), CONST("0", 2)), "Red, normilizgin to 0..127");
    SETVAL(wb_v1, CC3(CONST("0", 1), BITS(i_rgb565, 10, 5), CONST("0", 1)), "Green, normilizgin to 0..127");
    SETVAL(wb_v2, CC3(CONST("0", 1), BITS(i_rgb565, 4, 0), CONST("0", 2)), "Blue, normilizgin to 0..127");
    SETVAL(wb_v3, CONST("127", 8));

    TEXT();
    SETVAL(even, CC2(BITS(even, 5, 0), AND2_L(i_de, INV_L(BIT(even, 0)))));
    SETVAL(hsync, CC2(BITS(hsync, 6, 0), i_hsync));
    SETVAL(vsync, CC2(BITS(vsync, 6, 0), i_vsync));
    SETVAL(de, CC2(BITS(de, 6, 0), i_de));
    IF (NZ(BIT(even, 6)));
        TEXT("{Cb, Y} scaled down to 64 (multiplier gives 14 bits unsigned scale to 8-bits)");
        SETVAL(ycbcr422, CC2(BITS(wb_res1, 13, 6), BITS(wb_res0, 13, 6)));
    ELSE();
        TEXT("Cr, Y scaled down to 64 (multiplier gives 14 bits unsigned scale to 8-bits)");
        SETVAL(ycbcr422, CC2(BITS(wb_res2, 13, 6), BITS(wb_res0, 13, 6)));
    ENDIF();

    TEXT();
    SYNC_RESET();

    SETVAL(o_ycbcr422, CC2(CONST("0", 2), ycbcr422));
    SETVAL(o_hsync, BIT(hsync, 7));
    SETVAL(o_vsync, BIT(vsync, 7));
    SETVAL(o_de, BIT(de, 7));
}

