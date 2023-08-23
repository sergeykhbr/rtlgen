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

#include "sdctrl_cfg.h"

sdctrl_cfg *sdctrl_cfg_ = 0;

sdctrl_cfg::sdctrl_cfg(GenObject *parent) :
    FileObject(parent, "sdctrl_cfg"),
    _cmd0_(this, ""),
    _cmd1_(this, ""),
    CMD0(this, "6", "CMD0", "0", "GO_IDLE_STATE: Reset card to idle state. Response - (4.7.4)"),
    CMD8(this, "6", "CMD8", "8", "SEND_IF_COND: Card interface condition. Response R7 (4.9.6)."),
    ACMD41(this, "6", "ACMD41", "41"),
    CMD55(this, "6", "CMD55", "55", "APP_CMD: application specific commands"),
    _cmd2_(this, ""),
    R1(this, "3", "R1", "1"),
    R2(this, "3", "R2", "2"),
    R3(this, "3", "R3", "3"),
    R6(this, "3", "R6", "6"),
    _r70_(this, "4.9.6 R7 (Card interface condition, page 142)"),
    _r71_(this, "    [47]    Start bit = 1'b0"),
    _r72_(this, "    [46]    Transmission bit = 1'b0"),
    _r73_(this, "    [45:40] Command index = 6'b001000"),
    _r74_(this, "    [39:22] Reserved bits = 18'h0"),
    _r75_(this, "    [21]    PCIe 1.2V support = 1'bX"),
    _r76_(this, "    [20]    PCIe Response = 1'bX"),
    _r77_(this, "    [19:16] Voltage accepted = 4'hX"),
    _r78_(this, "    [15:8]  Echo-back of check pattern = 8'hXX"),
    _r79_(this, "    [7:1]   CRC7 = 7'hXX"),
    _r710_(this, "[0]     End bit = 1'b1"),
    R7(this, "3", "R7", "7"),
    _cmd3_(this, ""),
    DIR_OUTPUT(this, "1", "DIR_OUTPUT", "0"),
    DIR_INPUT(this, "1", "DIR_INPUT", "1"),
    _sd0_(this),
    _sd1_(this, "Card types detected during identification stage"),
    SDCARD_UNKNOWN(this, "3", "SDCARD_UNKNOWN", "0"),
    SDCARD_VER1X(this, "3", "SDCARD_VER1X", "1", "Ver1.X Standard Capacity"),
    SDCARD_VER2X_SC(this, "3", "SDCARD_VER2X_SC", "2", "Ver2.00 or higer Standard Capacity"),
    SDCARD_VER2X_HC(this, "3", "SDCARD_VER2X_HC", "3", "Ver2.00 or higer High or Extended Capacity"),
    SDCARD_UNUSABLE(this, "3", "SDCARD_UNUSABLE", "7"),
    _err0_(this, ""),
    CMDERR_NONE(this, "4", "CMDERR_NONE", "0"),
    CMDERR_NO_RESPONSE(this, "4", "CMDERR_NO_RESPONSE", "1"),
    CMDERR_WRONG_RESP_STARTBIT(this, "4", "CMDERR_WRONG_RESP_STARTBIT", "2"),
    CMDERR_WRONG_RESP_STOPBIT(this, "4", "CMDERR_WRONG_RESP_STOPBIT", "3"),

    _n_(this)
{
    sdctrl_cfg_ = this;
}

