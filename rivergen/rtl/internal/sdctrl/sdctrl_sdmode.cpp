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

#include "sdctrl_sdmode.h"

sdctrl_sdmode::sdctrl_sdmode(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "sdctrl_sdmode", name, comment),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_posedge(this, "i_posedge", "1", "SPI clock posedge pulse"),
    i_dat0(this, "i_dat0", "1", "Data Line[0] input; DI in SPI mode"),
    o_dat0(this, "o_dat0", "1", "Data Line[0] output"),
    o_dat0_dir(this, "o_dat0_dir", "1", "Direction bit: 1=input; 0=output"),
    i_dat1(this, "i_dat1", "1", "Data Line[1] input"),
    o_dat1(this, "o_dat1", "1", "Data Line[1] output"),
    o_dat1_dir(this, "o_dat1_dir", "1", "Direction bit: 1=input; 0=output"),
    i_dat2(this, "i_dat2", "1", "Data Line[2] input"),
    o_dat2(this, "o_dat2", "1", "Data Line[2] output"),
    o_dat2_dir(this, "o_dat2_dir", "1", "Direction bit: 1=input; 0=output"),
    i_cd_dat3(this, "i_cd_dat3", "1", "Card Detect / Data Line[3] input"),
    o_dat3(this, "o_dat3", "1", "Data Line[3] output; CS output in SPI mode"),
    o_dat3_dir(this, "o_dat3_dir", "1", "Direction bit: 1=input; 0=output"),
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
    i_cmd_resp_cmd(this, "i_cmd_resp_cmd", "6"),
    i_cmd_resp_arg32(this, "i_cmd_resp_arg32", "32"),
    o_data_req_ready(this, "o_data_req_ready", "1"),
    i_data_req_valid(this, "i_data_req_valid", "1"),
    i_data_req_write(this, "i_data_req_write", "1"),
    i_data_req_addr(this, "i_data_req_addr", "CFG_SDCACHE_ADDR_BITS"),
    i_data_req_wdata(this, "i_data_req_wdata", "512"),
    o_data_resp_valid(this, "o_data_resp_valid", "1"),
    o_data_resp_rdata(this, "o_data_resp_rdata", "512"),
    i_crc16_0(this, "i_crc16_0", "16"),
    i_crc16_1(this, "i_crc16_1", "16"),
    i_crc16_2(this, "i_crc16_2", "16"),
    i_crc16_3(this, "i_crc16_3", "16"),
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
    _sdstate0_(this, "SD-card states see Card Status[12:9] CURRENT_STATE on page 145:"),
    SDSTATE_IDLE(this, "SDSTATE_IDLE", "4", "0", NO_COMMENT),
    SDSTATE_READY(this, "SDSTATE_READY", "4", "1", NO_COMMENT),
    SDSTATE_IDENT(this, "SDSTATE_IDENT", "4", "2", NO_COMMENT),
    SDSTATE_STBY(this, "SDSTATE_STBY", "4", "3", NO_COMMENT),
    SDSTATE_TRAN(this, "SDSTATE_TRAN", "4", "4", NO_COMMENT),
    SDSTATE_DATA(this, "SDSTATE_DATA", "4", "5", NO_COMMENT),
    SDSTATE_RCV(this, "SDSTATE_RCV", "4", "6", NO_COMMENT),
    SDSTATE_PRG(this, "SDSTATE_PRG", "4", "7", NO_COMMENT),
    SDSTATE_DIS(this, "SDSTATE_DIS", "4", "8", NO_COMMENT),
    SDSTATE_INA(this, "SDSTATE_INA", "4", "9", NO_COMMENT),
    _idlestate0_(this, "SD-card 'idle' state substates:"),
    IDLESTATE_CMD0(this, "IDLESTATE_CMD0", "3", "0", NO_COMMENT),
    IDLESTATE_CMD8(this, "IDLESTATE_CMD8", "3", "1", NO_COMMENT),
    IDLESTATE_CMD55(this, "IDLESTATE_CMD55", "3", "2", NO_COMMENT),
    IDLESTATE_ACMD41(this, "IDLESTATE_ACMD41", "3", "3", NO_COMMENT),
    IDLESTATE_CARD_IDENTIFICATION(this, "IDLESTATE_CARD_IDENTIFICATION", "3", "5", NO_COMMENT),
    _readystate0_(this, "SD-card 'ready' state substates:"),
    READYSTATE_CMD11(this, "READYSTATE_CMD11", "2", "0", NO_COMMENT),
    READYSTATE_CMD2(this, "READYSTATE_CMD2", "2", "1", NO_COMMENT),
    READYSTATE_CHECK_CID(this, "READYSTATE_CHECK_CID", "2", "2", "State change: ready -> ident"),
    _identstate0_(this, "SD-card 'ident' state substates:"),
    IDENTSTATE_CMD3(this, "IDENTSTATE_CMD3", "1", "0", NO_COMMENT),
    IDENTSTATE_CHECK_RCA(this, "IDENTSTATE_CHECK_RCA", "1", "1", "State change: ident -> stby"),
    // signals
    // registers
    clkcnt(this, "clkcnt", "7", "'0", NO_COMMENT),
    cmd_req_valid(this, "cmd_req_valid", "1"),
    cmd_req_cmd(this, "cmd_req_cmd", "6", "'0", NO_COMMENT),
    cmd_req_arg(this, "cmd_req_arg", "32", "'0", NO_COMMENT),
    cmd_req_rn(this, "cmd_req_rn", "3", "'0", NO_COMMENT),
    cmd_resp_cmd(this, "cmd_resp_cmd", "6", "'0", NO_COMMENT),
    cmd_resp_arg32(this, "cmd_resp_arg32", "32", "'0", NO_COMMENT),
    data_addr(this, "data_addr", "32", "'0", NO_COMMENT),
    data_data(this, "data_data", "512", "'0", NO_COMMENT),
    data_resp_valid(this, "data_resp_valid", "1"),
    wdog_ena(this, "wdog_ena", "1"),
    crc16_clear(this, "crc16_clear", "1", "1"),
    crc16_calc0(this, "crc16_calc0", "16", "'0", NO_COMMENT),
    crc16_calc1(this, "crc16_calc1", "16", "'0", NO_COMMENT),
    crc16_calc2(this, "crc16_calc2", "16", "'0", NO_COMMENT),
    crc16_calc3(this, "crc16_calc3", "16", "'0", NO_COMMENT),
    crc16_rx0(this, "crc16_rx0", "16", "'0", NO_COMMENT),
    crc16_rx1(this, "crc16_rx1", "16", "'0", NO_COMMENT),
    crc16_rx2(this, "crc16_rx2", "16", "'0", NO_COMMENT),
    crc16_rx3(this, "crc16_rx3", "16", "'0", NO_COMMENT),
    dat_full_ena(this, "dat_full_ena", "1"),
    dat_csn(this, "dat_csn", "1", "1"),
    err_clear(this, "err_clear", "1"),
    err_valid(this, "err_valid", "1"),
    err_code(this, "err_code", "4", "'0", NO_COMMENT),
    sck_400khz_ena(this, "sck_400khz_ena", "1", "1", NO_COMMENT),
    sdstate(this, "sdstate", "4", "SDSTATE_IDLE", NO_COMMENT),
    idlestate(this, "initstate", "3", "IDLESTATE_CMD0", NO_COMMENT),
    readystate(this, "readystate", "2", "READYSTATE_CMD11", NO_COMMENT),
    identstate(this, "identstate", "1", "IDENTSTATE_CMD3", NO_COMMENT),
    wait_cmd_resp(this, "wait_cmd_resp", "1", "0", NO_COMMENT),
    sdtype(this, "sdtype", "3", "SDCARD_UNKNOWN", NO_COMMENT),
    HCS(this, "HCS", "1", "1", "High Capacity Support"),
    S18(this, "S18", "1", "0", "1.8V Low voltage"),
    OCR_VoltageWindow(this, "OCR_VoltageWindow", "24", "0xff8000", "all ranges 2.7 to 3.6 V"),
    bitcnt(this, "bitcnt", "12", "'0", NO_COMMENT),
    //
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void sdctrl_sdmode::proc_comb() {
    SETZERO(err_clear);
    SETZERO(err_valid);
    SETZERO(err_code);
    SETZERO(data_resp_valid);
    SETVAL(comb.vb_cmd_req_arg, cmd_req_arg);

TEXT();
    IF (i_posedge);
        TEXT("Not a full block 4096 bits just a cache line (dat_csn is active LOW):");
        IF (EZ(dat_full_ena));
            SETVAL(data_data, CC2(BITS(data_data, 510, 0), OR2(i_dat0, dat_csn)));
        ELSE();
            SETVAL(data_data, CC2(BITS(data_data, 507, 0), CC4(i_dat0, i_dat1, i_dat2, i_cd_dat3)));
        ENDIF();
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
        SETVAL(cmd_resp_cmd, i_cmd_resp_cmd);
        SETVAL(cmd_resp_arg32, i_cmd_resp_arg32);
        SWITCH(cmd_req_rn);
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
            SETVAL(data_addr, CC2(BITS(i_cmd_resp_arg32, 31, 16), CONST("0", 16)));
            ENDCASE();
        CASEDEF();
            ENDCASE();
        ENDSWITCH();
    ELSIF (NZ(wait_cmd_resp));
        TEXT("do nothing");
    ELSIF (EQ(sdstate, SDSTATE_IDLE));
        SETONE(sck_400khz_ena);
        SWITCH (idlestate);
        CASE (IDLESTATE_CMD0);
            SETVAL(sdtype, sdctrl_cfg_->SDCARD_UNKNOWN);
            SETONE(HCS);
            SETZERO(S18);
            SETZERO(data_addr);
            SETONE(cmd_req_valid);
            SETVAL(cmd_req_cmd, sdctrl_cfg_->CMD0);
            SETVAL(cmd_req_rn, sdctrl_cfg_->R1);
            SETZERO(comb.vb_cmd_req_arg);
            SETVAL(idlestate, IDLESTATE_CMD8);
            ENDCASE();
        CASE (IDLESTATE_CMD8);
            TEXT("See page 113. 4.3.13 Send Interface Condition Command");
            TEXT("  [39:22] reserved 00000h");
            TEXT("  [21]    PCIe 1.2V support 0");
            TEXT("  [20]    PCIe availability 0");
            TEXT("  [19:16] Voltage Supply (VHS) 0001b: 2.7-3.6V");
            TEXT("  [15:8]  Check Pattern 55h");
            IF (EZ(i_err_code));
                SETONE(cmd_req_valid);
            ELSE();
                SETVAL(idlestate, IDLESTATE_CMD0);
                SETONE(err_clear);
            ENDIF();
            SETVAL(cmd_req_cmd, sdctrl_cfg_->CMD8);
            SETVAL(cmd_req_rn, sdctrl_cfg_->R7);
            SETZERO(comb.vb_cmd_req_arg);
            SETBIT(comb.vb_cmd_req_arg, 13, i_cfg_pcie_12V_support);
            SETBIT(comb.vb_cmd_req_arg, 12, i_cfg_pcie_available);
            SETBITS(comb.vb_cmd_req_arg, 11, 8, i_cfg_voltage_supply);
            SETBITS(comb.vb_cmd_req_arg, 7, 0, i_cfg_check_pattern);
            SETVAL(idlestate, IDLESTATE_CMD55);
            ENDCASE();
        CASE (IDLESTATE_CMD55);
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
            SETVAL(idlestate, IDLESTATE_ACMD41);
            ENDCASE();
        CASE (IDLESTATE_ACMD41);
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
            SETBIT(comb.vb_cmd_req_arg, 24, S18);
            SETVAL(cmd_req_rn, sdctrl_cfg_->R3);
            SETVAL(idlestate, IDLESTATE_CARD_IDENTIFICATION);
            ENDCASE();
        CASE (IDLESTATE_CARD_IDENTIFICATION);
            IF (EZ(BIT(cmd_resp_arg32, 31)));
                TEXT("LOW if the card has not finished power-up routine");
                SETVAL(idlestate, IDLESTATE_CMD55);
            ELSIF (NZ(S18));
                TEXT("Voltage switch command to change 3.3V to 1.8V");
                SETVAL(readystate, READYSTATE_CMD11);
            ELSE();
                SETVAL(readystate, READYSTATE_CMD2);
            ENDIF();
            SETVAL(sdstate, SDSTATE_READY);
            ENDCASE();
        CASEDEF();
            SETVAL(idlestate, IDLESTATE_CMD0);
            ENDCASE();
        ENDSWITCH();
    ELSIF (EQ(sdstate, SDSTATE_READY));
        SWITCH(readystate);
        CASE(READYSTATE_CMD11);
            TEXT("CMD11: VOLTAGE_SWITCH siwtch to 1.8V bus signaling.");
            TEXT("  [31:0] reserved all zeros");
            SETONE(cmd_req_valid);
            SETVAL(cmd_req_cmd, sdctrl_cfg_->CMD11);
            SETVAL(cmd_req_rn, sdctrl_cfg_->R1);
            SETZERO(comb.vb_cmd_req_arg);
            SETVAL(readystate, READYSTATE_CMD2);
            ENDCASE();
        CASE(READYSTATE_CMD2);
            TEXT("CMD2: ALL_SEND_CID ask to send CID number.");
            TEXT("  [31:0] stuff bits");
            SETONE(cmd_req_valid);
            SETVAL(cmd_req_cmd, sdctrl_cfg_->CMD2);
            SETVAL(cmd_req_rn, sdctrl_cfg_->R2);
            SETZERO(comb.vb_cmd_req_arg);
            SETVAL(readystate, READYSTATE_CHECK_CID);
            ENDCASE();
        CASE(READYSTATE_CHECK_CID);
            SETVAL(sdstate, SDSTATE_IDENT);
            SETVAL(identstate, IDENTSTATE_CMD3);
            ENDCASE();
        CASEDEF();
            ENDCASE();
        ENDSWITCH();
    ELSIF (EQ(sdstate, SDSTATE_IDENT));
        SWITCH(identstate);
        CASE(IDENTSTATE_CMD3);
            TEXT("CMD3: SEND_RELATIVE_ADDR ask card to publish a new relative address (RCA).");
            TEXT("  [31:0] stuff bits");
            SETONE(cmd_req_valid);
            SETVAL(cmd_req_cmd, sdctrl_cfg_->CMD3);
            SETVAL(cmd_req_rn, sdctrl_cfg_->R6);
            SETZERO(comb.vb_cmd_req_arg);
            SETVAL(identstate, IDENTSTATE_CHECK_RCA);
            ENDCASE();
        CASE(IDENTSTATE_CHECK_RCA);
            SETVAL(sdstate, SDSTATE_STBY);
            SETZERO(sck_400khz_ena);
            ENDCASE();
        CASEDEF();
            ENDCASE();
        ENDSWITCH();
    ELSIF (EQ(sdstate, SDSTATE_STBY));
    ELSE();
    ENDIF();

TEXT();
    SETVAL(cmd_req_arg, comb.vb_cmd_req_arg);
    SETVAL(comb.v_dat0, BIT(data_data, 511));
    SETVAL(comb.v_dat1, BIT(data_data, 510));
    SETVAL(comb.v_dat2, BIT(data_data, 509));
    SETVAL(comb.v_dat3, BIT(data_data, 508));

TEXT();
    SYNC_RESET();

TEXT();
    SETVAL(o_dat0, comb.v_dat0);
    SETVAL(o_dat1, comb.v_dat1);
    SETVAL(o_dat2, comb.v_dat2);
    SETVAL(o_dat3, comb.v_dat3);
    SETVAL(o_dat0_dir, dat_csn);
    SETVAL(o_dat1_dir, dat_csn);
    SETVAL(o_dat2_dir, dat_csn);
    SETVAL(o_dat3_dir, dat_csn);
    SETVAL(o_crc16_clear, crc16_clear);
    SETVAL(o_crc16_next, comb.v_crc16_next);
    SETVAL(o_cmd_req_valid, cmd_req_valid);
    SETVAL(o_cmd_req_cmd, cmd_req_cmd);
    SETVAL(o_cmd_req_arg, cmd_req_arg);
    SETVAL(o_cmd_req_rn, cmd_req_rn);
    SETVAL(o_data_req_ready, comb.v_data_req_ready);
    SETVAL(o_data_resp_valid, data_resp_valid);
    SETVAL(o_data_resp_rdata, data_data);
    SETVAL(o_wdog_ena, wdog_ena);
    SETVAL(o_err_valid, err_valid);
    SETVAL(o_err_clear, err_clear);
    SETVAL(o_err_code, err_code);
    SETVAL(o_400khz_ena, sck_400khz_ena);
    SETVAL(o_sdtype, sdtype);
}
