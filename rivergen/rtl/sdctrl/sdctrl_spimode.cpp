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

#include "sdctrl_spimode.h"

sdctrl_spimode::sdctrl_spimode(GenObject *parent, const char *name) :
    ModuleObject(parent, "sdctrl_spimode", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_posedge(this, "i_posedge", "1", "SPI clock posedge pulse"),
    i_miso(this, "i_miso", "1", "SPI data input"),
    o_mosi(this, "o_mosi", "1", "SPI master output slave input"),
    o_csn(this, "o_csn", "1", "Chip select active LOW"),
    i_detected(this, "i_detected", "1"),
    i_protect(this, "i_protect", "1"),
    i_cfg_pcie_12V_support(this, "i_cfg_pcie_12V_support", "1"),
    i_cfg_pcie_available(this, "i_cfg_pcie_available", "1"),
    i_cfg_voltage_supply(this, "i_cfg_voltage_supply", "4"),
    i_cfg_check_pattern(this, "i_cfg_check_pattern", "8"),
    i_cmd_req_ready(this, "i_cmd_req_ready", "1"),
    o_cmd_req_valid(this, "o_cmd_req_valid", "1"),
    o_cmd_req_cmd(this, "o_cmd_req_cmd", "6"),
    o_cmd_req_arg(this, "o_cmd_req_arg", "32"),
    o_cmd_req_rn(this, "o_cmd_req_rn", "3"),
    i_cmd_resp_valid(this, "i_cmd_resp_valid", "1"),
    i_cmd_resp_r1r2(this, "i_cmd_resp_r1r2", "15"),
    i_cmd_resp_arg32(this, "i_cmd_resp_arg32", "32"),
    o_data_req_ready(this, "o_data_req_ready", "1"),
    i_data_req_valid(this, "i_data_req_valid", "1"),
    i_data_req_write(this, "i_data_req_write", "1"),
    i_data_req_addr(this, "i_data_req_addr", "CFG_SDCACHE_ADDR_BITS"),
    i_data_req_wdata(this, "i_data_req_wdata", "512"),
    o_data_resp_valid(this, "o_data_resp_valid", "1"),
    o_data_resp_rdata(this, "o_data_resp_rdata", "512"),
    i_crc16_0(this, "i_crc16_0", "16"),
    o_crc16_clear(this, "o_crc16_clear", "1"),
    o_crc16_next(this, "o_crc16_next", "1"),
    o_wdog_ena(this, "o_wdog_ena", "1"),
    i_wdog_trigger(this, "i_wdog_trigger", "1"),
    i_err_code(this, "i_err_code", "4"),
    o_err_valid(this, "o_err_valid", "1"),
    o_err_clear(this, "o_err_clear", "1"),
    o_err_code(this, "o_err_code", "4"),
    o_400khz_ena(this, "o_400khz_ena", "1"),
    o_sdtype(this, "o_sdtype", "3"),
    // params
    _state0_(this, "Controller state"),
    STATE_CMD0(this, "4", "STATE_CMD0", "0"),
    STATE_CMD8(this, "4", "STATE_CMD8", "1"),
    STATE_CMD55(this, "4", "STATE_CMD55", "2"),
    STATE_ACMD41(this, "4", "STATE_ACMD41", "3"),
    STATE_CMD58(this, "4", "STATE_CMD58", "4"),
    STATE_WAIT_DATA_REQ(this, "4", "STATE_WAIT_DATA_REQ", "5"),
    STATE_CMD17_READ_SINGLE_BLOCK(this, "4", "STATE_CMD17_READ_SINGLE_BLOCK", "6"),
    STATE_CMD24_WRITE_SINGLE_BLOCK(this, "4", "STATE_CMD24_WRITE_SINGLE_BLOCK", "7"),
    STATE_WAIT_DATA_START(this, "4", "STATE_WAIT_DATA_START", "8"),
    STATE_READING_DATA(this, "4", "STATE_READING_DATA", "9"),
    STATE_READING_CRC15(this, "4", "STATE_READING_CRC15", "10"),
    STATE_READING_END(this, "4", "STATE_READING_END", "11"),
    // signals
    // registers
    clkcnt(this, "clkcnt", "7"),
    cmd_req_valid(this, "cmd_req_valid", "1"),
    cmd_req_cmd(this, "cmd_req_cmd", "6"),
    cmd_req_arg(this, "cmd_req_arg", "32"),
    cmd_req_rn(this, "cmd_req_rn", "3"),
    cmd_resp_cmd(this, "cmd_resp_cmd", "6"),
    cmd_resp_arg32(this, "cmd_resp_arg32", "32"),
    cmd_resp_r1(this, "cmd_resp_r1", "7"),
    cmd_resp_r2(this, "cmd_resp_r2", "8"),
    data_addr(this, "data_addr", "32"),
    data_data(this, "data_data", "512"),
    data_resp_valid(this, "data_resp_valid", "1"),
    wdog_ena(this, "wdog_ena", "1"),
    crc16_clear(this, "crc16_clear", "1", "1"),
    crc16_calc0(this, "crc16_calc0", "16"),
    crc16_rx0(this, "crc16_rx0", "16"),
    dat_csn(this, "dat_csn", "1", "1"),
    dat_reading(this, "dat_reading", "1"),
    err_clear(this, "err_clear", "1"),
    err_valid(this, "err_valid", "1"),
    err_code(this, "err_code", "4"),
    sck_400khz_ena(this, "sck_400khz_ena", "1", "1"),
    state(this, "state", "4", "STATE_CMD0"),
    wait_cmd_resp(this, "wait_cmd_resp", "1"),
    sdtype(this, "sdtype", "3", "SDCARD_UNKNOWN"),
    HCS(this, "HCS", "1", "1", "High Capacity Support"),
    S18(this, "S18", "1", "0", "1.8V Low voltage"),
    OCR_VoltageWindow(this, "OCR_VoltageWindow", "24", "0xff8000", "all ranges 2.7 to 3.6 V"),
    bitcnt(this, "bitcnt", "12"),
    //
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void sdctrl_spimode::proc_comb() {
    SETZERO(err_clear);
    SETZERO(err_valid);
    SETZERO(err_code);
    SETZERO(data_resp_valid);
    SETVAL(comb.vb_cmd_req_arg, cmd_req_arg);

TEXT();
    IF (i_posedge);
        TEXT("Not a full block 4096 bits just a cache line (dat_csn is active LOW):");
        SETVAL(data_data, CC2(BITS(data_data, 510, 0), OR2(i_miso, dat_csn)));
        SETVAL(bitcnt, INC(bitcnt));
    ENDIF();

TEXT();
    IF (NZ(cmd_req_valid));
        IF (NZ(i_cmd_req_ready));
            SETZERO(cmd_req_valid);
            SETONE(wait_cmd_resp);
        ENDIF();
    ELSIF (AND2(NZ(i_cmd_resp_valid), NZ(wait_cmd_resp)));
        TEXT("Parse Rx response:");
        SETZERO(wait_cmd_resp);
        SETVAL(cmd_resp_r1, BITS(i_cmd_resp_r1r2, 14, 8));
        SETVAL(cmd_resp_arg32, i_cmd_resp_arg32);
        SWITCH(cmd_req_rn);
        CASE (sdctrl_cfg_->R2);
            SETVAL(cmd_resp_r2, BITS(i_cmd_resp_r1r2, 7, 0));
            ENDCASE();
        CASE (sdctrl_cfg_->R3);
            TEXT("Table 5-1: OCR Register definition, page 246");
            TEXT("    [23:0]  Voltage window can be requested by CMD58");
            TEXT("    [24]    Switching to 1.8V accepted (S18A)");
            TEXT("    [27]    Over 2TB support status (CO2T)");
            TEXT("    [29]    UHS-II Card status");
            TEXT("    [30]    Card Capacity Status (CCS)");
            TEXT("    [31]    Card power-up status (busy is LOW if the card not finished the power-up routine)");
            IF (NZ(BIT(i_cmd_resp_arg32, 31)));
                SETVAL(OCR_VoltageWindow, BITS(i_cmd_resp_arg32, 23, 0));
                SETVAL(HCS, BIT(i_cmd_resp_arg32, 30));
                IF (NZ(BIT(i_cmd_resp_arg32, 30)));
                    SETVAL(sdtype, sdctrl_cfg_->SDCARD_VER2X_HC);
                ELSIF (EQ(sdtype, sdctrl_cfg_->SDCARD_UNKNOWN));
                    SETVAL(sdtype, sdctrl_cfg_->SDCARD_VER2X_SC);
                ENDIF();
                SETVAL(S18, BIT(i_cmd_resp_arg32, 24));
            ENDIF();
            ENDCASE();
        CASE (sdctrl_cfg_->R6);
            TEXT("Relative Address (RCA):");
            SETVAL(data_addr, CC2(BITS(i_cmd_resp_arg32, 31, 16), CONST("0", 16)));
            ENDCASE();
        CASEDEF();
            ENDCASE();
        ENDSWITCH();
    ELSIF (NZ(wait_cmd_resp));
        TEXT("do nothing");
    ELSIF (EQ(state, STATE_CMD0));
        SETVAL(sdtype, sdctrl_cfg_->SDCARD_UNKNOWN);
        SETONE(HCS);
        SETZERO(S18);
        SETZERO(data_addr);
        SETVAL(OCR_VoltageWindow, CONST("0xff8000"));
        SETONE(cmd_req_valid);
        SETVAL(cmd_req_cmd, sdctrl_cfg_->CMD0);
        SETVAL(cmd_req_rn, sdctrl_cfg_->R1);
        SETZERO(comb.vb_cmd_req_arg);
        SETVAL(state, STATE_CMD8);
    ELSIF (EQ(state, STATE_CMD8));
        TEXT("See page 113. 4.3.13 Send Interface Condition Command");
        TEXT("  [39:22] reserved 00000h");
        TEXT("  [21]    PCIe 1.2V support 0");
        TEXT("  [20]    PCIe availability 0");
        TEXT("  [19:16] Voltage Supply (VHS) 0001b: 2.7-3.6V");
        TEXT("  [15:8]  Check Pattern 55h");
        IF (EZ(i_err_code));
            SETONE(cmd_req_valid);
        ELSE();
            SETVAL(state, STATE_CMD0);
            SETONE(err_clear);
        ENDIF();
        SETVAL(cmd_req_cmd, sdctrl_cfg_->CMD8);
        SETVAL(cmd_req_rn, sdctrl_cfg_->R7);
        SETZERO(comb.vb_cmd_req_arg);
        SETBIT(comb.vb_cmd_req_arg, 13, i_cfg_pcie_12V_support);
        SETBIT(comb.vb_cmd_req_arg, 12, i_cfg_pcie_available);
        SETBITS(comb.vb_cmd_req_arg, 11, 8, i_cfg_voltage_supply);
        SETBITS(comb.vb_cmd_req_arg, 7, 0, i_cfg_check_pattern);
        SETVAL(state, STATE_CMD55);
    ELSIF (EQ(state, STATE_CMD55));
        TEXT("Page 64: APP_CMD (CMD55) shall always precede ACMD41.");
        TEXT("  [31:16] RCA (Relative Adrress should be set 0)");
        TEXT("  [15:0] stuff bits");
        IF(EQ(i_err_code, sdctrl_cfg_->CMDERR_NO_RESPONSE));
            SETVAL(sdtype, sdctrl_cfg_->SDCARD_VER1X);
            SETZERO(HCS, "Standard Capacity only");
            SETONE(err_clear);
        ENDIF();
        SETONE(cmd_req_valid);
        SETVAL(cmd_req_cmd, sdctrl_cfg_->CMD55);
        SETVAL(cmd_req_rn, sdctrl_cfg_->R1);
        SETZERO(comb.vb_cmd_req_arg);
        SETVAL(state, STATE_ACMD41);
    ELSIF (EQ(state, STATE_ACMD41));
        TEXT("Page 131: SD_SEND_OP_COND. ");
        TEXT("  [31] reserved bit");
        TEXT("  [30] HCS (high capacity support)");
        TEXT("  [29] reserved for eSD");
        TEXT("  [28] XPC (maximum power in default speed)");
        TEXT("  [27:25] reserved bits");
        TEXT("  [24] S18R Send request to switch to 1.8V");
        TEXT("  [23:0] VDD voltage window (OCR[23:0])");
        SETONE(cmd_req_valid);
        SETVAL(cmd_req_cmd, sdctrl_cfg_->ACMD41);
        SETZERO(comb.vb_cmd_req_arg);
        SETBIT(comb.vb_cmd_req_arg, 30, HCS);
        SETBITS(comb.vb_cmd_req_arg, 23, 0, OCR_VoltageWindow);
        SETVAL(cmd_req_rn, sdctrl_cfg_->R1);
        SETVAL(state, STATE_CMD58);
    ELSIF (EQ(state, STATE_CMD58));
        TEXT("READ_OCR: Reads OCR register. Used in SPI mode only.");
        TEXT("  [31] reserved bit");
        TEXT("  [30] HCS (high capacity support)");
        TEXT("  [29:0] reserved");
        IF (NE(cmd_resp_r1, CONST("0x1", 7)));
            TEXT("SD card not in idle state");
            SETVAL(state, STATE_CMD55);
        ELSE();
            SETONE(cmd_req_valid);
            SETVAL(cmd_req_cmd, sdctrl_cfg_->CMD58);
            SETZERO(comb.vb_cmd_req_arg);
            SETVAL(cmd_req_rn, sdctrl_cfg_->R3);
            SETVAL(state, STATE_WAIT_DATA_REQ);
            SETZERO(sck_400khz_ena);
        ENDIF();
    ELSIF (EQ(state, STATE_WAIT_DATA_REQ));
        SETZERO(wdog_ena);
        SETONE(comb.v_data_req_ready);
        IF (NZ(i_data_req_valid));
            SETVAL(data_addr, BITS(i_data_req_addr, DEC(sdctrl_cfg_->CFG_SDCACHE_ADDR_BITS), CONST("9")));
            SETVAL(data_data, i_data_req_wdata);
            IF (NZ(i_data_req_write));
                SETVAL(state, STATE_CMD24_WRITE_SINGLE_BLOCK);
            ELSE();
                SETVAL(state, STATE_CMD17_READ_SINGLE_BLOCK);
            ENDIF();
        ENDIF();
    ELSIF (EQ(state, STATE_CMD17_READ_SINGLE_BLOCK));
        TEXT("CMD17: READ_SINGLE_BLOCK. Reads a block of the size SET_BLOCKLEN");
        TEXT("  [31:0] data address");
        SETONE(cmd_req_valid);
        SETVAL(cmd_req_cmd, sdctrl_cfg_->CMD17);
        SETVAL(cmd_req_rn, sdctrl_cfg_->R1);
        SETVAL(comb.vb_cmd_req_arg, data_addr);
        SETVAL(state, STATE_WAIT_DATA_START);
        SETZERO(bitcnt);
    ELSIF (EQ(state, STATE_CMD24_WRITE_SINGLE_BLOCK));
        TEXT("TODO:");

    ELSIF (EQ(state, STATE_WAIT_DATA_START));
        SETZERO(dat_csn);
        SETONE(dat_reading);
        SETONE(crc16_clear);
        SETONE(wdog_ena);
        IF (NE(i_err_code, sdctrl_cfg_->CMDERR_NONE));
            SETVAL(state, STATE_WAIT_DATA_REQ);
        ELSIF (EQ(BITS(data_data, 7, 0), CONST("0xFE", 8)));
            SETVAL(state, STATE_READING_DATA);
            SETZERO(bitcnt);
            SETZERO(crc16_clear);
        ELSIF(NZ(i_wdog_trigger));
            SETVAL(state, STATE_WAIT_DATA_REQ);
            SETONE(err_valid);
            SETVAL(err_code, sdctrl_cfg_->CMDERR_NO_RESPONSE);
        ENDIF();
    ELSIF (EQ(state, STATE_READING_DATA));
        IF (NZ(i_posedge));
            SETONE(comb.v_crc16_next);
            IF (NZ(AND_REDUCE(bitcnt)));
                SETVAL(state, STATE_READING_CRC15);
                SETVAL(crc16_calc0, i_crc16_0);
            ENDIF();
            IF (NZ(AND_REDUCE(BITS(bitcnt, 8, 0))));
                SETONE(data_resp_valid);
            ENDIF();
        ENDIF();
    ELSIF (EQ(state, STATE_READING_CRC15));
        IF (NZ(i_posedge));
            IF (NZ(AND_REDUCE(BITS(bitcnt, 3, 0))));
                SETVAL(state, STATE_READING_END);
                SETONE(dat_csn);
                SETZERO(dat_reading);
            ENDIF();
        ENDIF();
    ELSIF (EQ(state, STATE_READING_END));
        SETVAL(crc16_rx0, TO_U32(BITS(data_data, 15, 0)));
        SETVAL(state, STATE_WAIT_DATA_REQ);
    ENDIF();

TEXT();
    SETVAL(cmd_req_arg, comb.vb_cmd_req_arg);
    SETVAL(comb.v_dat, OR2(dat_reading, BIT(data_data, 511)));

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_mosi, comb.v_dat);
    SETVAL(o_cmd_req_valid, cmd_req_valid);
    SETVAL(o_cmd_req_cmd, cmd_req_cmd);
    SETVAL(o_cmd_req_arg, cmd_req_arg);
    SETVAL(o_cmd_req_rn, cmd_req_rn);
    SETVAL(o_data_req_ready, comb.v_data_req_ready);
    SETVAL(o_data_resp_valid, data_resp_valid);
    SETVAL(o_data_resp_rdata, data_data);
    SETVAL(o_csn, dat_csn);
    SETVAL(o_crc16_clear, crc16_clear);
    SETVAL(o_crc16_next, comb.v_crc16_next);
    SETVAL(o_wdog_ena, wdog_ena);
    SETVAL(o_err_valid, err_valid);
    SETVAL(o_err_clear, err_clear);
    SETVAL(o_err_code, err_code);
    SETVAL(o_400khz_ena, sck_400khz_ena);
    SETVAL(o_sdtype, sdtype);
}
