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

#include "vip_sdcard_ctrl.h"

vip_sdcard_ctrl::vip_sdcard_ctrl(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "vip_sdcard_ctrl", name, comment),
    CFG_SDCARD_POWERUP_DONE_DELAY(this, "CFG_SDCARD_POWERUP_DONE_DELAY", "450", "Delay of busy bits in ACMD41 response"),
    CFG_SDCARD_HCS(this, "CFG_SDCARD_HCS", "1", "0x1", "High Capacity Support"),
    CFG_SDCARD_VHS(this, "CFG_SDCARD_VHS", "4", "0x1", "CMD8 Voltage supply mask"),
    CFG_SDCARD_PCIE_1_2V(this, "CFG_SDCARD_PCIE_1_2V", "1", "0", NO_COMMENT),
    CFG_SDCARD_PCIE_AVAIL(this, "CFG_SDCARD_PCIE_AVAIL", "1", "0", NO_COMMENT),
    CFG_SDCARD_VDD_VOLTAGE_WINDOW(this, "CFG_SDCARD_VDD_VOLTAGE_WINDOW", "24", "0xff8000", NO_COMMENT),
    i_nrst(this, "i_nrst", "1"),
    i_clk(this, "i_clk", "1"),
    i_spi_mode(this, "i_spi_mode", "1"),
    i_cs(this, "i_cs", "1"),
    i_cmd_req_valid(this, "i_cmd_req_valid", "1"),
    i_cmd_req_cmd(this, "i_cmd_req_cmd", "6"),
    i_cmd_req_data(this, "i_cmd_req_data", "32"),
    o_cmd_req_ready(this, "o_cmd_req_ready", "1"),
    o_cmd_resp_valid(this, "o_cmd_resp_valid", "1"),
    o_cmd_resp_data32(this, "o_cmd_resp_data32", "32"),
    i_cmd_resp_ready(this, "i_cmd_resp_ready", "1"),
    o_cmd_resp_r1b(this, "o_cmd_resp_r1b", "1"),
    o_cmd_resp_r2(this, "o_cmd_resp_r2", "1"),
    o_cmd_resp_r3(this, "o_cmd_resp_r3", "1"),
    o_cmd_resp_r7(this, "o_cmd_resp_r7", "1"),
    o_stat_idle_state(this, "o_stat_idle_state", "1"),
    o_stat_illegal_cmd(this, "o_stat_illegal_cmd", "1"),
    o_mem_addr(this, "o_mem_addr", "41"),
    i_mem_rdata(this, "i_mem_rdata", "8"),
    o_crc16_clear(this, "o_crc16_clear", "1"),
    o_crc16_next(this, "o_crc16_next", "1"),
    i_crc16(this, "i_crc16", "16"),
    o_dat_trans(this, "o_dat_trans", "1"),
    o_dat(this, "o_dat", "4"),
    i_cmdio_busy(this, "i_cmdio_busy", "1"),
    // params
    _sdstate0_(this, ""),
    _sdstate1_(this, "SD-card states (see Card Status[12:9] CURRENT_STATE on page 145)"),
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
    _sdstate2_(this),
    _sdstate3_(this, "Data block access state machine:"),
    DATASTATE_IDLE(this, "DATASTATE_IDLE", "3", "0", NO_COMMENT),
    DATASTATE_START(this, "DATASTATE_START", "3", "1", NO_COMMENT),
    DATASTATE_CRC15(this, "DATASTATE_CRC15", "3", "2", NO_COMMENT),
    DATASTATE_STOP(this, "DATASTATE_STOP", "3", "3", NO_COMMENT),
    // signals
    // registers
    sdstate(this, "sdstate", "4", "SDSTATE_IDLE", NO_COMMENT),
    datastate(this, "datastate", "3", "DATASTATE_IDLE", NO_COMMENT),
    powerup_cnt(this, "powerup_cnt", "32", "'0", NO_COMMENT),
    preinit_cnt(this, "preinit_cnt", "8", "'0", NO_COMMENT),
    delay_cnt(this, "delay_cnt", "32", "'0", NO_COMMENT),
    powerup_done(this, "powerup_done", "1"),
    cmd_req_ready(this, "cmd_req_ready", "1"),
    cmd_resp_valid(this, "cmd_resp_valid", "1"),
    cmd_resp_valid_delayed(this, "cmd_resp_valid_delayed", "1"),
    cmd_resp_data32(this, "cmd_resp_data32", "32", "'0", NO_COMMENT),
    cmd_resp_r1b(this, "cmd_resp_r1b", "1"),
    cmd_resp_r2(this, "cmd_resp_r2", "1"),
    cmd_resp_r3(this, "cmd_resp_r3", "1"),
    cmd_resp_r7(this, "cmd_resp_r7", "1"),
    illegal_cmd(this, "illegal_cmd", "1"),
    ocr_hcs(this, "ocr_hcs", "1"),
    ocr_vdd_window(this, "ocr_vdd_window", "24", "'0", NO_COMMENT),
    req_mem_valid(this, "req_mem_valid", "1"),
    req_mem_addr(this, "req_mem_addr", "41", "'0", NO_COMMENT),
    shiftdat(this, "shiftdat", "16", "'1", NO_COMMENT),
    bitcnt(this, "bitcnt", "13", "'0", NO_COMMENT),
    crc16_clear(this, "crc16_clear", "1"),
    crc16_next(this, "crc16_next", "1"),
    dat_trans(this, "dat_trans", "1"),
    //
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void vip_sdcard_ctrl::proc_comb() {
    SETVAL(comb.vb_resp_data32, cmd_resp_data32);

TEXT();
    IF (AND2(NZ(cmd_resp_valid_delayed), NZ(i_cmd_resp_ready)));
        SETZERO(cmd_resp_valid_delayed);
        SETZERO(cmd_resp_r1b);
        SETZERO(cmd_resp_r2);
        SETZERO(cmd_resp_r3);
        SETZERO(cmd_resp_r7);
        SETZERO(illegal_cmd);
    ENDIF();
    TEXT("Power-up counter emulates 'busy' bit in ACMD41 response:");
    IF (AND2(EZ(powerup_done), LS(powerup_cnt, CFG_SDCARD_POWERUP_DONE_DELAY)));
        SETVAL(powerup_cnt, INC(powerup_cnt));
    ELSE();
        SETONE(powerup_done);
    ENDIF();

TEXT();
    IF (NZ(i_cmd_req_valid));
        SWITCH (sdstate);
        CASE(SDSTATE_IDLE);
            SWITCH (i_cmd_req_cmd);
            CASE (CONST("0", 6), "CMD0: GO_IDLE_STATE.");
                SETONE(cmd_resp_valid);
                SETZERO(comb.vb_resp_data32);
                SETVAL(delay_cnt, CONST("20", 32));
                ENDCASE();
            CASE (CONST("8", 6), "CMD8: SEND_IF_COND.");
                TEXT("Send memory Card interface condition:");
                TEXT("[21] PCIe 1.2V support");
                TEXT("[20] PCIe availability");
                TEXT("[19:16] Voltage supply");
                TEXT("[15:8] check pattern");
                SETONE(cmd_resp_valid);
                SETONE(cmd_resp_r7);
                SETVAL(delay_cnt, CONST("20", 32));
                SETBIT(comb.vb_resp_data32, 13,
                        AND2_L(BIT(i_cmd_req_data, 13), CFG_SDCARD_PCIE_1_2V));
                SETBIT(comb.vb_resp_data32, 12,
                        AND2_L(BIT(i_cmd_req_data, 12), CFG_SDCARD_PCIE_AVAIL));
                SETBITS(comb.vb_resp_data32, 11, 8,
                        AND2_L(BITS(i_cmd_req_data, 11, 8), CFG_SDCARD_VHS));
                SETBITS(comb.vb_resp_data32, 7, 0, BITS(i_cmd_req_data, 7, 0));
                ENDCASE();
            CASE (CONST("55", 6), "CMD55: APP_CMD.");
                SETONE(cmd_resp_valid);
                SETZERO(comb.vb_resp_data32);
                ENDCASE();
            CASE (CONST("41", 6), "ACMD41: SD_SEND_OP_COND.");
                TEXT("Send host capacity info:");
                TEXT("[39] BUSY, active LOW");
                TEXT("[38] HCS (OCR[30]) Host Capacity");
                TEXT("[36] XPC");
                TEXT("[32] S18R");
                TEXT("[31:8] VDD Voltage Window (OCR[23:0])");
                SETVAL(ocr_hcs, AND2_L(BIT(i_cmd_req_data, 30), CFG_SDCARD_HCS));
                SETVAL(ocr_vdd_window, AND2_L(BITS(i_cmd_req_data, 23, 0), CFG_SDCARD_VDD_VOLTAGE_WINDOW));
                SETONE(cmd_resp_valid);
                SETVAL(delay_cnt, CONST("20", 32));
                SETBIT(comb.vb_resp_data32, 31, powerup_done);
                SETBIT(comb.vb_resp_data32, 30, AND2_L(BIT(i_cmd_req_data, 30), CFG_SDCARD_HCS));
                SETBITS(comb.vb_resp_data32, 23, 0,
                    AND2_L(BITS(i_cmd_req_data, 23, 0), CFG_SDCARD_VDD_VOLTAGE_WINDOW));
                IF (EQ(AND2_L(BITS(i_cmd_req_data, 23, 0), CFG_SDCARD_VDD_VOLTAGE_WINDOW), CONST("0", 24)));
                    TEXT("OCR check failed:");
                    SETVAL(sdstate, SDSTATE_INA);
                ELSIF (AND2(EZ(i_spi_mode), NZ(powerup_done)));
                    TEXT("SD mode only");
                    SETVAL(sdstate, SDSTATE_READY);
                ENDIF();
                ENDCASE();
            CASE (CONST("58", 6), "CMD58: READ_OCR.");
                SETONE(cmd_resp_valid);
                SETONE(cmd_resp_r7);
                SETVAL(delay_cnt, CONST("20", 32));
                IF (NZ(i_spi_mode));
                    SETZERO(comb.vb_resp_data32);
                    SETBIT(comb.vb_resp_data32, 31, powerup_done);
                    SETBIT(comb.vb_resp_data32, 30, ocr_hcs);
                    SETBITS(comb.vb_resp_data32, 23, 0, ocr_vdd_window);
                ELSE();
                    SETONE(illegal_cmd);
                ENDIF();
                ENDCASE();
            CASE (CONST("17", 6), "CMD17: READ_SINGLE_BLOCK.");
                SETONE(cmd_resp_valid);
                SETVAL(delay_cnt, CONST("20", 32));
                IF (NZ(i_spi_mode));
                    SETONE(req_mem_valid);
                    SETVAL(req_mem_addr, CC2(i_cmd_req_data, CONST("0", 9)));
                    SETZERO(comb.vb_resp_data32);
                ELSE();
                    SETONE(illegal_cmd);
                ENDIF();
                ENDCASE();
            CASEDEF();
                TEXT("Illegal commands in 'idle' state:");
                SETONE(cmd_resp_valid);
                SETVAL(comb.vb_resp_data32, ALLONES());
                SETONE(illegal_cmd);
                ENDCASE();
            ENDSWITCH();
            ENDCASE();
        CASE(SDSTATE_READY);
            SWITCH (i_cmd_req_cmd);
            CASE (CONST("0", 6), "CMD0: GO_IDLE_STATE.");
                SETONE(cmd_resp_valid);
                SETZERO(comb.vb_resp_data32);
                SETVAL(delay_cnt, CONST("2", 32));
                SETVAL(sdstate, SDSTATE_IDLE);
                ENDCASE();
            CASE (CONST("2", 6), "CMD2: .");
                SETONE(cmd_resp_valid);
                SETVAL(delay_cnt, CONST("1", 32));
                SETVAL(sdstate, SDSTATE_IDENT);
                ENDCASE();
            CASE (CONST("11", 6), "CMD11: .");
                SETONE(cmd_resp_valid);
                SETVAL(delay_cnt, CONST("1", 32));
                ENDCASE();
            CASEDEF();
                TEXT("Illegal commands in 'ready' state:");
                SETONE(cmd_resp_valid);
                SETVAL(comb.vb_resp_data32, ALLONES());
                SETONE(illegal_cmd);
                ENDCASE();
            ENDSWITCH();
            ENDCASE();
        CASE(SDSTATE_IDENT);
            SWITCH (i_cmd_req_cmd);
            CASE (CONST("0", 6), "CMD0: GO_IDLE_STATE.");
                SETONE(cmd_resp_valid);
                SETZERO(comb.vb_resp_data32);
                SETVAL(delay_cnt, CONST("2", 32));
                SETVAL(sdstate, SDSTATE_IDLE);
                ENDCASE();
            CASE (CONST("3", 6), "CMD3: .");
                SETONE(cmd_resp_valid);
                SETVAL(delay_cnt, CONST("1", 32));
                SETVAL(sdstate, SDSTATE_STBY);
                ENDCASE();
            CASEDEF();
                TEXT("Illegal commands in 'stby' state:");
                SETONE(cmd_resp_valid);
                SETVAL(comb.vb_resp_data32, ALLONES());
                SETONE(illegal_cmd);
                ENDCASE();
            ENDSWITCH();
            ENDCASE();
        CASE(SDSTATE_STBY);
            ENDCASE();
        CASE(SDSTATE_TRAN);
            ENDCASE();
        CASE(SDSTATE_DATA);
            ENDCASE();
        CASE(SDSTATE_RCV);
            ENDCASE();
        CASE(SDSTATE_PRG);
            ENDCASE();
        CASE(SDSTATE_DIS);
            ENDCASE();
        CASE(SDSTATE_INA);
            ENDCASE();
        CASEDEF();
            ENDCASE();
        ENDSWITCH();
    ENDIF();

TEXT();
    SETVAL(shiftdat, CC2(BITS(shiftdat, 14, 0), CONST("1", 1)));
    SWITCH(datastate);
    CASE (DATASTATE_IDLE);
        SETONE(crc16_clear);
        SETZERO(crc16_next);
        SETZERO(dat_trans);
        IF (AND3(NZ(req_mem_valid), EZ(i_cmdio_busy), EZ(i_cs)));
            SETZERO(req_mem_valid);
            SETVAL(datastate, DATASTATE_START);
            SETVAL(shiftdat, CONST("0xFE00", 16));
            SETZERO(bitcnt);
            SETONE(dat_trans);
        ENDIF();
        ENDCASE();
    CASE (DATASTATE_START);
        SETVAL(bitcnt, INC(bitcnt));
        IF (NZ(AND_REDUCE(BITS(bitcnt, 2, 0))));
            SETZERO(crc16_clear);
            SETONE(crc16_next);
            IF (EQ(BITS(bitcnt, 12, 3), CONST("512", 10)));
                SETVAL(datastate, DATASTATE_CRC15);
                SETVAL(shiftdat, i_crc16);
                SETZERO(bitcnt);
                SETZERO(crc16_next);
            ELSE();
                TEXT("Read memory byte:");
                SETVAL(shiftdat, CC2(i_mem_rdata, BITS(shiftdat, 7, 0)));
                SETVAL(req_mem_addr, INC(req_mem_addr));
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (DATASTATE_CRC15);
        SETVAL(bitcnt, INC(bitcnt));
        IF (NZ(AND_REDUCE(BITS(bitcnt, 3, 0))));
            SETVAL(datastate, DATASTATE_IDLE);
            SETZERO(dat_trans);
        ENDIF();
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    SETVAL(cmd_resp_data32, comb.vb_resp_data32);
    SETVAL(cmd_req_ready, INV(OR_REDUCE(delay_cnt)));
    IF (NZ(cmd_resp_valid));
        IF (EZ(delay_cnt));
            SETVAL(cmd_resp_valid_delayed, cmd_resp_valid);
            SETZERO(cmd_resp_valid);
        ELSE();
            SETVAL(delay_cnt, DEC(delay_cnt));
        ENDIF();
    ENDIF();
    

TEXT();
    SETVAL(o_cmd_req_ready, cmd_req_ready);
    SETVAL(o_cmd_resp_valid, cmd_resp_valid_delayed);
    SETVAL(o_cmd_resp_data32, cmd_resp_data32);
    SETVAL(o_cmd_resp_r1b, cmd_resp_r1b);
    SETVAL(o_cmd_resp_r2, cmd_resp_r2);
    SETVAL(o_cmd_resp_r3, cmd_resp_r3);
    SETVAL(o_cmd_resp_r7, cmd_resp_r7);
    SETVAL(o_stat_illegal_cmd, illegal_cmd);
    SETVAL(o_stat_idle_state, powerup_done);
    SETVAL(o_mem_addr, req_mem_addr);
    SETVAL(o_crc16_clear,  crc16_clear);
    SETVAL(o_crc16_next,  crc16_next);
    SETVAL(o_dat_trans, dat_trans);
    SETVAL(o_dat, BITS(shiftdat, 15, 12));
}
