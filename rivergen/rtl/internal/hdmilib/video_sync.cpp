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

#include "video_sync.h"

video_sync::video_sync(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "video_sync", name, comment),
    H_ACTIVE(this, "H_ACTIVE", "1366"),
    H_FRONT(this, "H_FRONT", "70"),
    H_SYNC(this, "H_SYNC", "143"),
    H_BACK(this, "H_BACK", "213"),
    V_ACTIVE(this, "V_ACTIVE", "768"),
    V_FRONT(this, "V_FRONT", "3"),
    V_SYNC(this, "V_SYNC", "5"),
    V_BACK(this, "V_BACK", "24"),
    // IO
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    o_hsync(this, "o_hsync", "1", "Horisontal sync pulse"),
    o_vsync(this, "o_vsync", "1", "Vertical sync pulse"),
    o_de(this, "o_de", "1", "Data enable, HIGH in active zone 1366x768 screen resolution"),
    o_x(this, "o_x", "11", "Width pixel coordinate"),
    o_y(this, "o_y", "10", "Height pixel coordinate"),
    // params
    H_TOTAL(this, "H_TOTAL", "ADD(ADD(H_ACTIVE,H_SYNC),ADD(H_FRONT,H_BACK))"),
    V_TOTAL(this, "V_TOTAL", "ADD(ADD(V_ACTIVE,V_SYNC),ADD(V_FRONT,V_BACK))"),
    // signals
    // registers
    h_count(this, "h_count", "11", "'0", NO_COMMENT),
    v_count(this, "v_count", "10", "'0", NO_COMMENT),
    h_sync(this, "h_sync", "1", RSTVAL_ZERO, NO_COMMENT),
    v_sync(this, "v_sync", "1", RSTVAL_ZERO, NO_COMMENT),
    de(this, "de", "1", RSTVAL_ZERO, NO_COMMENT),
    x_pix(this, "x_pix", "11", "'0", NO_COMMENT),
    y_pix(this, "y_pix", "10", "'0", NO_COMMENT),
    //
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void video_sync::proc_comb() {
    IF (EQ(h_count, DEC(H_TOTAL)));
        SETZERO(h_count);
        IF (EQ(v_count, DEC(V_TOTAL)));
            SETZERO(v_count);
        ELSE();
            SETVAL(v_count, INC(v_count));
        ENDIF();
    ELSE();
        SETVAL(h_count, INC(h_count));
    ENDIF();


    TEXT();
    TEXT("Re-arrange sequence: active => front porch => sync => back porch");
    TEXT("                     _______________________|------|____________");
    TEXT("Polarity+");
    IF (AND2(GE(h_count, ADD2(H_ACTIVE, H_FRONT)), LS(h_count, ADD2(ADD2(H_ACTIVE, H_FRONT), H_SYNC))));
        SETONE(h_sync);
    ELSE();
        SETZERO(h_sync);
    ENDIF();
    IF (AND2(GE(v_count, ADD2(V_ACTIVE, V_FRONT)), LS(v_count, ADD2(ADD2(V_ACTIVE, V_FRONT), V_SYNC))));
        SETONE(v_sync);
    ELSE();
        SETZERO(v_sync);
    ENDIF();
    IF (AND2(LS(h_count, H_ACTIVE), LS(v_count, V_ACTIVE)));
        SETONE(de);
        SETVAL(x_pix, h_count);
        SETVAL(y_pix, v_count);
    ELSE();
        SETZERO(de);
        SETZERO(x_pix);
        SETZERO(y_pix);
    ENDIF();

    TEXT();
    SYNC_RESET();

    TEXT();
    SETVAL(o_de, de);
    SETVAL(o_hsync, h_sync);
    SETVAL(o_vsync, v_sync);
    SETVAL(o_x, x_pix);
    SETVAL(o_y, y_pix);
}

