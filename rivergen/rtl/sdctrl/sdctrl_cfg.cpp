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
    _cache0_(this, "Cache config:"),
    CFG_SDCACHE_ADDR_BITS(this, "CFG_SDCACHE_ADDR_BITS", "48"),
    CFG_LOG2_SDCACHE_LINEBITS(this, "CFG_LOG2_SDCACHE_LINEBITS", "5", "2=2KB (4 x (8x64))"),
    CFG_LOG2_SDCACHE_BYTES_PER_LINE(this, "CFG_LOG2_SDCACHE_BYTES_PER_LINE", "6", "64 Bytes"),
    SDCACHE_BYTES_PER_LINE(this, "SDCACHE_BYTES_PER_LINE", "POW2(1,CFG_LOG2_SDCACHE_BYTES_PER_LINE)"),
    SDCACHE_LINE_BITS(this, "SDCACHE_LINE_BITS", "MUL(8,SDCACHE_BYTES_PER_LINE)"),
    _cache1_(this),
    SDCACHE_FL_VALID(this, "SDCACHE_FL_VALID", "0"),
    SDCACHE_FL_DIRTY(this, "SDCACHE_FL_DIRTY", "1"),
    SDCACHE_FL_TOTAL(this, "SDCACHE_FL_TOTAL", "2"),
    _cmd0_(this, ""),
    _cmd1_(this, ""),
    CMD0(this, "CMD0", "6", "0", "GO_IDLE_STATE: Reset card to idle state. Response - (4.7.4)"),
    CMD2(this, "CMD2", "6", "2", "ALL_SEND_CID: ask to send CID number"),
    CMD3(this, "CMD3", "6", "3", "SEND_RELATIVE_ADDRE: Ask to publish (RCA) relative address"),
    CMD8(this, "CMD8", "6", "8", "SEND_IF_COND: Card interface condition. Response R7 (4.9.6)."),
    CMD11(this, "CMD11", "6", "11", "VOLTAGE_SWITCH: Switch to 1.8V bus signaling level"),
    CMD17(this, "CMD17", "6", "17", "READ_SINGLE_BLOCK: Read block size of SET_BLOCKLEN"),
    CMD24(this, "CMD24", "6", "24", "WRITE_SINGLE_BLOCK: Write block size of SET_BLOCKLEN"),
    ACMD41(this, "ACMD41", "6", "41", NO_COMMENT),
    CMD55(this, "CMD55", "6", "55", "APP_CMD: application specific commands"),
    CMD58(this, "CMD58", "6", "58", "READ_OCR: Read OCR register in SPI mode"),
    _cmd2_(this, ""),
    R1(this, "R1", "3", "1", NO_COMMENT),
    _r20_(this, "4.9.3 R2 (CID, CSD register, page 140)"),
    _r21_(this, "    [135]     Start bit = 1'b0"),
    _r22_(this, "    [134]     Transmission bit = 1'b0"),
    _r23_(this, "    [133:128] reserved = 6'b111111"),
    _r24_(this, "    [127:120] Manufacturer ID = 8'hX"),
    _r25_(this, "    [119:104] OEM/Application ID = 16'hX"),
    _r26_(this, "    [103:64]  Product name = 40'hX"),
    _r27_(this, "    [63:56]   Product revision = 8'hX"),
    _r28_(this, "    [55:24]   Product serial number = 32'hX"),
    _r29_(this, "    [23:20]   reserved = 4'h0"),
    _r2a_(this, "    [19:8]    Manufacturer date = 12'hX"),
    _r2b_(this, "    [7:1]     CRC7 = 7'hXX"),
    _r2c_(this, "    [0]       End bit = 1'b1"),
    R2(this, "R2", "3", "2", NO_COMMENT),
    _r30_(this, "4.9.4 R3 (OCR register, page 140)"),
    _r31_(this, "    [47]    Start bit = 1'b0"),
    _r32_(this, "    [46]    Transmission bit = 1'b0"),
    _r33_(this, "    [45:40] reserved = 6'b111111"),
    _r34_(this, "    [39:8]  OCR register = 32'hX"),
    _r35_(this, "    [7:1]   reserved = 7'b1111111"),
    _r36_(this, "    [0]     End bit = 1'b1"),
    R3(this, "R3", "3", "3", NO_COMMENT),
    _r60_(this, "4.9.5 R3 (Published RCA response, page 141)"),
    _r61_(this, "    [47]    Start bit = 1'b0"),
    _r62_(this, "    [46]    Transmission bit = 1'b0"),
    _r63_(this, "    [45:40] Command index = 6'b000011"),
    _r64_(this, "    [39:25] New published RCA[31:16] of the card"),
    _r65_(this, "    [24:8]  status bits {[23,22,12,[12:0]} see Table 4-42"),
    _r66_(this, "    [7:1]   CRC7 = 7'hXX"),
    _r67_(this, "    [0]     End bit = 1'b1"),
    R6(this, "R6", "3", "6", NO_COMMENT),
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
    _r710_(this, "    [0]     End bit = 1'b1"),
    R7(this, "R7", "3", "7", NO_COMMENT),
    _cmd3_(this, ""),
    DIR_OUTPUT(this, "DIR_OUTPUT", "1", "0", NO_COMMENT),
    DIR_INPUT(this, "DIR_INPUT", "1", "1", NO_COMMENT),
    _sd0_(this),
    _sd1_(this, "Card types detected during identification stage"),
    SDCARD_UNKNOWN(this, "SDCARD_UNKNOWN", "3", "0", NO_COMMENT),
    SDCARD_VER1X(this, "SDCARD_VER1X", "3", "1", "Ver1.X Standard Capacity"),
    SDCARD_VER2X_SC(this, "SDCARD_VER2X_SC", "3", "2", "Ver2.00 or higer Standard Capacity"),
    SDCARD_VER2X_HC(this, "SDCARD_VER2X_HC", "3", "3", "Ver2.00 or higer High or Extended Capacity"),
    SDCARD_UNUSABLE(this, "SDCARD_UNUSABLE", "3", "7", NO_COMMENT),
    _err0_(this, ""),
    CMDERR_NONE(this, "CMDERR_NONE", "4", "0", NO_COMMENT),
    CMDERR_NO_RESPONSE(this, "CMDERR_NO_RESPONSE", "4", "1", NO_COMMENT),
    CMDERR_WRONG_RESP_STARTBIT(this, "CMDERR_WRONG_RESP_STARTBIT", "4", "2", NO_COMMENT),
    CMDERR_WRONG_RESP_STOPBIT(this, "CMDERR_WRONG_RESP_STOPBIT", "4", "3", NO_COMMENT),

    _n_(this)
{
    sdctrl_cfg_ = this;
}

