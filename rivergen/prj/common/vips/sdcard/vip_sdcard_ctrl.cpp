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

vip_sdcard_ctrl::vip_sdcard_ctrl(GenObject *parent, const char *name) :
    ModuleObject(parent, "vip_sdcard_ctrl", name),
    CFG_SDCARD_POWERUP_DONE_DELAY(this, "CFG_SDCARD_POWERUP_DONE_DELAY", "700", "Delay of busy bits in ACMD41 response"),
    CFG_SDCARD_VHS(this, "CFG_SDCARD_VHS", "4", "0x1", "CMD8 Voltage supply mask"),
    CFG_SDCARD_PCIE_1_2V(this, "CFG_SDCARD_PCIE_1_2V", "1", "0"),
    CFG_SDCARD_PCIE_AVAIL(this, "CFG_SDCARD_PCIE_AVAIL", "1", "0"),
    CFG_SDCARD_VDD_VOLTAGE_WINDOW(this, "CFG_SDCARD_VDD_VOLTAGE_WINDOW", "24", "0xff8000"),
    i_nrst(this, "i_nrst", "1"),
    i_clk(this, "i_clk", "1"),
    i_cmd_req_valid(this, "i_cmd_req_valid", "1"),
    i_cmd_req_cmd(this, "i_cmd_req_cmd", "6"),
    i_cmd_req_data(this, "i_cmd_req_data", "32"),
    o_cmd_req_ready(this, "o_cmd_req_ready", "1"),
    o_cmd_resp_valid(this, "o_cmd_resp_valid", "1"),
    o_cmd_resp_data32(this, "o_cmd_resp_data32", "32"),
    i_cmd_resp_ready(this, "i_cmd_resp_ready", "1"),
    // params
    _sdstate0_(this, ""),
    _sdstate1_(this, "SD-card states (see Card Status[12:9] CURRENT_STATE on page 145)"),
    SDSTATE_IDLE(this, "4", "SDSTATE_IDLE", "0"),
    SDSTATE_READY(this, "4", "SDSTATE_READY", "1"),
    SDSTATE_IDENT(this, "4", "SDSTATE_IDENT", "2"),
    SDSTATE_STBY(this, "4", "SDSTATE_STBY", "3"),
    SDSTATE_TRAN(this, "4", "SDSTATE_TRAN", "4"),
    SDSTATE_DATA(this, "4", "SDSTATE_DATA", "5"),
    SDSTATE_RCV(this, "4", "SDSTATE_RCV", "6"),
    SDSTATE_PRG(this, "4", "SDSTATE_PRG", "7"),
    SDSTATE_DIS(this, "4", "SDSTATE_DIS", "8"),
    SDSTATE_INA(this, "4", "SDSTATE_INA", "9"),
    // signals
    // registers
    sdstate(this, "sdstate", "4", "SDSTATE_IDLE"),
    powerup_cnt(this, "powerup_cnt", "32"),
    preinit_cnt(this, "preinit_cnt", "8"),
    delay_cnt(this, "delay_cnt", "32"),
    powerup_done(this, "powerup_done", "1"),
    cmd_req_ready(this, "cmd_req_ready", "1"),
    cmd_resp_valid(this, "cmd_resp_valid", "1"),
    cmd_resp_valid_delayed(this, "cmd_resp_valid_delayed", "1"),
    cmd_resp_data32(this, "cmd_resp_data32", "32"),
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
                SETONE(cmd_resp_valid);
                SETVAL(delay_cnt, CONST("20", 32));
                SETBIT(comb.vb_resp_data32, 31, powerup_done);
                SETBITS(comb.vb_resp_data32, 23, 0,
                    AND2_L(BITS(i_cmd_req_data, 23, 0), CFG_SDCARD_VDD_VOLTAGE_WINDOW));
                IF (EQ(AND2_L(BITS(i_cmd_req_data, 23, 0), CFG_SDCARD_VDD_VOLTAGE_WINDOW), CONST("0", 24)));
                    TEXT("OCR check failed:");
                    SETVAL(sdstate, SDSTATE_INA);
                ELSIF (NZ(powerup_done));
                    SETVAL(sdstate, SDSTATE_READY);
                ENDIF();
                ENDCASE();
            CASEDEF();
                TEXT("Illegal commands in 'idle' state:");
                SETONE(cmd_resp_valid);
                SETVAL(comb.vb_resp_data32, ALLONES());
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
}
