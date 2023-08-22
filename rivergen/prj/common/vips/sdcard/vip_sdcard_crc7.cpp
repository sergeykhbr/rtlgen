// 
//  Copyright 2023 Sergey Khabarov, sergeykhbr@gmail.com
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

#include "vip_sdcard_crc7.h"

vip_sdcard_crc7::vip_sdcard_crc7(GenObject *parent, const char *name) :
    ModuleObject(parent, "vip_sdcard_crc7", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_clear(this, "i_clear", "1", "Clear CRC register;"),
    i_next(this, "i_next", "1", "Shift enable strob"),
    i_dat(this, "i_dat", "1", "Input bit"),
    o_crc7(this, "o_crc7", "7", "Computed value"),
    // params
    // signals
    // registers
    crc7(this, "crc7", "7"),
    //
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void vip_sdcard_crc7::proc_comb() {
    TEXT("CRC7 = x^7 + x^3 + 1");
    TEXT("CMD0 -> 01 000000 0000..000000000000000000000000 1001010 1 -> 0x4A (0x95)");
    TEXT("CMD17-> 01 010001 0000..000000000000000000000000 0101010 1 -> 0x2A (0x55)");
    TEXT("CMD17<- 00 010001 0000..000000000010010000000000 0110011 1 -> 0x33 (0x67)");
    SETVAL(comb.v_inv7, XOR2(BIT(crc7, 6), i_dat));
    SETBIT(comb.vb_crc7, 6, BIT(crc7, 5));
    SETBIT(comb.vb_crc7, 5, BIT(crc7, 4));
    SETBIT(comb.vb_crc7, 4, BIT(crc7, 3));
    SETBIT(comb.vb_crc7, 3, XOR2(BIT(crc7, 2), comb.v_inv7));
    SETBIT(comb.vb_crc7, 2, BIT(crc7, 1));
    SETBIT(comb.vb_crc7, 1, BIT(crc7, 0));
    SETBIT(comb.vb_crc7, 0, comb.v_inv7);

TEXT();
    IF (NZ(i_clear));
        SETZERO(crc7);
    ELSIF (NZ(i_next));
        SETVAL(crc7, comb.vb_crc7);
    ENDIF();

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_crc7, comb.vb_crc7);
}
