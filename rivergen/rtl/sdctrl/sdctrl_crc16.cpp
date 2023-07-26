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

#include "sdctrl_crc16.h"

sdctrl_crc16::sdctrl_crc16(GenObject *parent, const char *name) :
    ModuleObject(parent, "sdctrl_crc16", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_clear(this, "i_clear", "1", "Clear CRC register;"),
    i_next(this, "i_next", "1", "Shift enable strob"),
    i_dat(this, "i_dat", "4", "Input bit"),
    o_crc16(this, "o_crc16", "16", "Computed value"),
    // params
    // signals
    // registers
    crc16(this, "crc16", "16"),
    //
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void sdctrl_crc16::proc_comb() {
    TEXT("CRC16 = x^16 + x^12 + x^5 + 1");
    SETVAL(comb.v_inv16, XOR2(BIT(crc16, 15), BIT(i_dat, 3)));
    SETBIT(comb.vb_crc16, 15, BIT(crc16, 14));
    SETBIT(comb.vb_crc16, 14, BIT(crc16, 13));
    SETBIT(comb.vb_crc16, 13, BIT(crc16, 12));
    SETBIT(comb.vb_crc16, 12, XOR2(BIT(crc16, 11), comb.v_inv16));
    SETBIT(comb.vb_crc16, 11, BIT(crc16, 10));
    SETBIT(comb.vb_crc16, 10, BIT(crc16, 9));
    SETBIT(comb.vb_crc16, 9, BIT(crc16, 8));
    SETBIT(comb.vb_crc16, 8, BIT(crc16, 7));
    SETBIT(comb.vb_crc16, 7, BIT(crc16, 6));
    SETBIT(comb.vb_crc16, 6, BIT(crc16, 5));
    SETBIT(comb.vb_crc16, 5, XOR2(BIT(crc16, 4), comb.v_inv16));
    SETBIT(comb.vb_crc16, 4, BIT(crc16, 3));
    SETBIT(comb.vb_crc16, 3, BIT(crc16, 2));
    SETBIT(comb.vb_crc16, 2, BIT(crc16, 1));
    SETBIT(comb.vb_crc16, 1, BIT(crc16, 0));
    SETBIT(comb.vb_crc16, 0, comb.v_inv16);

TEXT();
    IF (NZ(i_clear));
        SETZERO(crc16);
    ELSIF (NZ(i_next));
        SETVAL(crc16, comb.vb_crc16);
    ENDIF();


TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_crc16, crc16);
}
