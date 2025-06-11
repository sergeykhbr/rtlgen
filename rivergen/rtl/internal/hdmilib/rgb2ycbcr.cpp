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
    i_rgb565(this, "i_rgb565", "1", "RGB 16-bits input"),
    o_y(this, "o_y", "8", "Intensity component"),
    o_cb(this, "o_cb", "8", "Cb component"),
    o_cr(this, "o_cr", "8", "Cr compnent"),
    // params
    // signals
    // registers
    y(this, "y", "8", "'0", NO_COMMENT),
    cr(this, "cr", "8", "'0", NO_COMMENT),
    cb(this, "cb", "8", "'0", NO_COMMENT),
    //
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void rgb2ycbcr::proc_comb() {

    TEXT();
    SYNC_RESET();

    TEXT();
    SETVAL(o_y, y);
    SETVAL(o_cb, cb);
    SETVAL(o_cr, cr);
}

