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

#include "sdctrl_crc15.h"

sdctrl_crc15::sdctrl_crc15(GenObject *parent, const char *name) :
    ModuleObject(parent, "sdctrl_crc15", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_clear(this, "i_clear", "1", "Clear CRC register;"),
    i_next(this, "i_next", "1", "Shift enable strob"),
    i_dat(this, "i_dat", "1", "Input bit"),
    o_crc15(this, "o_crc15", "15", "Computed value"),
    // params
    // signals
    // registers
    crc15(this, "crc15", "15"),
    //
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void sdctrl_crc15::proc_comb() {
    TEXT("CRC15 = x^16 + x^12 + x^5 + 1");
    SETVAL(comb.v_inv15, XOR2(BIT(crc15, 14), i_dat));
    SETBIT(comb.vb_crc15, 14, BIT(crc15, 13));
    SETBIT(comb.vb_crc15, 13, BIT(crc15, 12));
    SETBIT(comb.vb_crc15, 12, XOR2(BIT(crc15, 11), comb.v_inv15));
    SETBIT(comb.vb_crc15, 11, BIT(crc15, 10));
    SETBIT(comb.vb_crc15, 10, BIT(crc15, 9));
    SETBIT(comb.vb_crc15, 9, BIT(crc15, 8));
    SETBIT(comb.vb_crc15, 8, BIT(crc15, 7));
    SETBIT(comb.vb_crc15, 7, BIT(crc15, 6));
    SETBIT(comb.vb_crc15, 6, BIT(crc15, 5));
    SETBIT(comb.vb_crc15, 5, XOR2(BIT(crc15, 4), comb.v_inv15));
    SETBIT(comb.vb_crc15, 4, BIT(crc15, 3));
    SETBIT(comb.vb_crc15, 3, BIT(crc15, 2));
    SETBIT(comb.vb_crc15, 2, BIT(crc15, 1));
    SETBIT(comb.vb_crc15, 1, BIT(crc15, 0));
    SETBIT(comb.vb_crc15, 0, comb.v_inv15);

TEXT();
    IF (NZ(i_clear));
        SETZERO(crc15);
    ELSIF (NZ(i_next));
        SETVAL(crc15, comb.vb_crc15);
    ENDIF();


TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_crc15, crc15);
}
