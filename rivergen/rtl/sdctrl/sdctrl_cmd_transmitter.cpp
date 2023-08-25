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

#include "sdctrl_cfg.h"
#include "sdctrl_cmd_transmitter.h"

sdctrl_cmd_transmitter::sdctrl_cmd_transmitter(GenObject *parent, const char *name) :
    ModuleObject(parent, "sdctrl_cmd_transmitter", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_sclk_posedge(this, "i_sclk_posedge", "1"),
    i_sclk_negedge(this, "i_sclk_negedge", "1"),
    i_cmd(this, "i_cmd", "1"),
    o_cmd(this, "o_cmd", "1"),
    o_cmd_dir(this, "o_cmd_dir", "1"),
    i_watchdog(this, "i_watchdog", "16", "Max number of sclk to receive start bit"),
    i_cmd_set_low(this, "i_cmd_set_low", "1", "Set forcibly o_cmd output to LOW"),
    i_req_valid(this, "i_req_valid", "1"),
    i_req_cmd(this, "i_req_cmd", "6"),
    i_req_arg(this, "i_req_arg", "32"),
    i_req_rn(this, "i_req_rn", "3", "R1, R3,R6 or R2"),
    o_req_ready(this, "o_req_ready", "1"),
    i_crc7(this, "i_crc7", "7"),
    o_crc7_clear(this, "o_crc7_clear", "1"),
    o_crc7_next(this, "o_crc7_next", "1"),
    o_crc7_dat(this, "o_crc7_dat", "1"),
    o_resp_valid(this, "o_resp_valid", "1"),
    o_resp_cmd(this, "o_resp_cmd", "6", "Mirrored command"),
    o_resp_reg(this, "o_resp_reg", "32", "Card Status, OCR register (R3) or RCA register (R6)"),
    o_resp_crc7_rx(this, "o_resp_crc7_rx", "7", "Received CRC7"),
    o_resp_crc7_calc(this, "o_resp_crc7_calc", "7", "Calculated CRC7"),
    i_resp_ready(this, "i_resp_ready", "1"),
    i_clear_cmderr(this, "i_clear_cmderr", "1"),
    o_cmdstate(this, "o_cmdstate", "4"),
    o_cmderr(this, "o_cmderr", "4"),
    // params
    _cmdstate0_(this, "Command request states:"),
    CMDSTATE_IDLE(this, "4", "CMDSTATE_IDLE", "0"),
    CMDSTATE_REQ_CONTENT(this, "4", "CMDSTATE_REQ_CONTENT", "1"),
    CMDSTATE_REQ_CRC7(this, "4", "CMDSTATE_REQ_CRC7", "2"),
    CMDSTATE_REQ_STOPBIT(this, "4", "CMDSTATE_REQ_STOPBIT", "3"),
    CMDSTATE_RESP_WAIT(this, "4", "CMDSTATE_RESP_WAIT", "4"),
    CMDSTATE_RESP_TRANSBIT(this, "4", "CMDSTATE_RESP_TRANSBIT", "5"),
    CMDSTATE_RESP_CMD_MIRROR(this, "4", "CMDSTATE_RESP_CMD_MIRROR", "6"),
    CMDSTATE_RESP_R1(this, "4", "CMDSTATE_RESP_R1", "7"),
    CMDSTATE_RESP_REG(this, "4", "CMDSTATE_RESP_REG", "8"),
    CMDSTATE_RESP_CID_CSD(this, "4", "CMDSTATE_RESP_CID_CSD", "9"),
    CMDSTATE_RESP_CRC7(this, "4", "CMDSTATE_RESP_CRC7", "10"),
    CMDSTATE_RESP_STOPBIT(this, "4", "CMDSTATE_RESP_STOPBIT", "11"),
    // signals
    // registers
    req_cmd(this, "req_cmd", "6"),
    req_rn(this, "req_rn", "3"),
    resp_valid(this, "resp_valid", "1"),
    resp_cmd(this, "resp_cmd", "6"),
    resp_reg(this, "resp_arg", "32"),
    cmdshift(this, "cmdshift", "40", "-1"),
    cmdmirror(this, "cmdmirror", "6"),
    regshift(this, "regshift", "32"),
    cidshift(this, "cidshift", "120"),
    crc_calc(this, "crc_calc", "7"),
    crc_rx(this, "crc_rx", "7"),
    cmdbitcnt(this, "cmdbitcnt", "7"),
    crc7_clear(this, "crc7_clear", "1", "1"),
    cmdstate(this, "cmdstate", "4", "CMDSTATE_IDLE"),
    cmderr(this, "cmderr", "4", "CMDERR_NONE"),
    watchdog(this, "watchdog", "16"),
    //
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void sdctrl_cmd_transmitter::proc_comb() {
    SETVAL(comb.vb_cmdshift, cmdshift);
    IF (NZ(i_clear_cmderr));
        SETZERO(cmderr);
    ENDIF();
    IF (NZ(i_resp_ready));
        SETZERO(resp_valid);
    ENDIF();

TEXT();
    TEXT("command state:");
    IF (NZ(i_sclk_negedge));
        TEXT("CMD Request:");
        IF (EQ(cmdstate, CMDSTATE_IDLE));
            IF (NZ(i_cmd_set_low));
                TEXT("Used during p-init state (power-up)");
                SETZERO(comb.vb_cmdshift);
            ELSE();
                SETVAL(comb.vb_cmdshift, ALLONES());
            ENDIF();
            SETONE(crc7_clear);
            SETONE(comb.v_req_ready);
            IF (NE(cmderr, sdctrl_cfg_->CMDERR_NONE));
                SETZERO(comb.v_req_ready);
            ELSIF (NZ(i_req_valid));
                SETVAL(req_cmd, i_req_cmd);
                SETVAL(req_rn, i_req_rn);
                SETVAL(comb.vb_cmdshift, CC3(CONST("0x1", 2), i_req_cmd, i_req_arg));
                SETVAL(cmdbitcnt, CONST("39", 7));
                SETZERO(crc7_clear);
                SETVAL(cmdstate, CMDSTATE_REQ_CONTENT);
            ENDIF();
        ELSIF (EQ(cmdstate, CMDSTATE_REQ_CONTENT));
            SETONE(comb.v_crc7_next);
            SETVAL(comb.vb_cmdshift, CC2(BITS(cmdshift, 38, 0), CONST("1", 1)));
            IF (NZ(cmdbitcnt));
                SETVAL(cmdbitcnt, DEC(cmdbitcnt));
            ELSE();
                SETVAL(cmdstate, CMDSTATE_REQ_CRC7);
                SETBITS(comb.vb_cmdshift, 39, 32, CC2(i_crc7, CONST("1", 1)));
                SETVAL(cmdbitcnt, CONST("6", 7));
                SETONE(crc7_clear);
            ENDIF();
        ELSIF (EQ(cmdstate, CMDSTATE_REQ_CRC7));
            SETVAL(comb.vb_cmdshift, CC2(BITS(cmdshift, 38, 0), CONST("1", 1)));
            IF (NZ(cmdbitcnt));
                SETVAL(cmdbitcnt, DEC(cmdbitcnt));
            ELSE();
                SETVAL(cmdstate, CMDSTATE_REQ_STOPBIT);
            ENDIF();
        ELSIF (EQ(cmdstate, CMDSTATE_REQ_STOPBIT));
            SETVAL(cmdstate, CMDSTATE_RESP_WAIT);
            SETVAL(watchdog, i_watchdog);
            SETZERO(crc7_clear);
        ENDIF();

    ELSIF (NZ(i_sclk_posedge));
        TEXT("CMD Response (see page 140. '4.9 Responses'):");
        TEXT();
        IF (EQ(cmdstate, CMDSTATE_RESP_WAIT));
            TEXT("[47] start bit; [135] for R2");
            SETVAL(watchdog, DEC(watchdog));
            IF (EZ(i_cmd));
                SETONE(comb.v_crc7_next);
                SETVAL(cmdstate, CMDSTATE_RESP_TRANSBIT);
            ELSIF(EZ(watchdog));
                SETVAL(cmderr, sdctrl_cfg_->CMDERR_NO_RESPONSE);
                SETVAL(cmdstate, CMDSTATE_IDLE);
            ENDIF();
        ELSIF(EQ(cmdstate, CMDSTATE_RESP_TRANSBIT));
            TEXT("[46](134) transmission bit (R2);");
            SETONE(comb.v_crc7_next);
            IF (EZ(i_cmd));
                SETVAL(cmdstate, CMDSTATE_RESP_CMD_MIRROR);
                SETZERO(cmdmirror);
                SETVAL(cmdbitcnt, CONST("5", 7));
            ELSE();
                SETVAL(cmderr, sdctrl_cfg_->CMDERR_WRONG_RESP_STARTBIT);
                SETVAL(cmdstate, CMDSTATE_IDLE);
            ENDIF();
        ELSIF(EQ(cmdstate, CMDSTATE_RESP_CMD_MIRROR));
            TEXT("[45:40] [133:128] command index mirrored: 111111 for R2 and R3 (OCR)");
            SETONE(comb.v_crc7_next);
            SETVAL(cmdmirror, CC2(BITS(cmdmirror, 4, 0), i_cmd));
            IF (NZ(cmdbitcnt));
                SETVAL(cmdbitcnt, DEC(cmdbitcnt));
            ELSE();
                IF (EQ(req_rn, sdctrl_cfg_->R2));
                    SETVAL(cmdbitcnt, CONST("119", 7));
                    SETVAL(cmdstate, CMDSTATE_RESP_CID_CSD);
                ELSE();
                    SETVAL(cmdbitcnt, CONST("31", 7));
                    SETVAL(cmdstate, CMDSTATE_RESP_REG);
                ENDIF();
            ENDIF();
        ELSIF(EQ(cmdstate, CMDSTATE_RESP_REG));
            TEXT("[39:8] Card status (R1), OCR (R3) or RCA (R6) register");
            SETONE(comb.v_crc7_next);
            SETVAL(regshift, CC2(BITS(regshift, 30, 0), i_cmd));
            IF (NZ(cmdbitcnt));
                SETVAL(cmdbitcnt, DEC(cmdbitcnt));
            ELSE();
                SETVAL(crc_calc, i_crc7);
                SETVAL(cmdbitcnt, CONST("6", 7));
                SETVAL(cmdstate, CMDSTATE_RESP_CRC7);
            ENDIF();
        ELSIF (EQ(cmdstate, CMDSTATE_RESP_CID_CSD));
            TEXT("[127:8] CID or CSD register incl. internal CRC7 R2 response on CMD2 and CMD10 (CID) or CMD9 (CSD)");
            SETVAL(cidshift, CC2(BITS(cidshift, 118, 0), i_cmd));
            IF (NZ(cmdbitcnt));
                SETVAL(cmdbitcnt, DEC(cmdbitcnt));
            ELSE();
                SETVAL(crc_calc, i_crc7);
                SETVAL(cmdbitcnt, CONST("6", 7));
                SETVAL(cmdstate, CMDSTATE_RESP_CRC7);
            ENDIF();
        ELSIF (EQ(cmdstate, CMDSTATE_RESP_CRC7));
            TEXT("[7:1] CRC7: 1111111 for R3 (OCR) no proteection");
            SETVAL(crc_rx, CC2(BITS(crc_rx, 5, 0), i_cmd));
            IF (NZ(cmdbitcnt));
                SETVAL(cmdbitcnt, DEC(cmdbitcnt));
            ELSE();
                SETVAL(cmdstate, CMDSTATE_RESP_STOPBIT);
            ENDIF();
        ELSIF (EQ(cmdstate, CMDSTATE_RESP_STOPBIT));
            TEXT("[7:1] End bit");
            IF (EZ(i_cmd));
                SETVAL(cmderr, sdctrl_cfg_->CMDERR_WRONG_RESP_STOPBIT);
            ENDIF();
            SETVAL(cmdstate, CMDSTATE_IDLE);
            SETONE(resp_valid);
        ENDIF();
    ENDIF();
    SETVAL(cmdshift, comb.vb_cmdshift);

TEXT();
    IF (LS(cmdstate, CMDSTATE_RESP_WAIT));
        SETVAL(comb.v_cmd_dir, sdctrl_cfg_->DIR_OUTPUT);
        SETVAL(comb.v_crc7_dat, BIT(cmdshift, 39));
    ELSE();
        SETVAL(comb.v_cmd_dir, sdctrl_cfg_->DIR_INPUT);
        SETVAL(comb.v_crc7_dat, i_cmd);
    ENDIF();

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_cmd, BIT(cmdshift, 39));
    SETVAL(o_cmd_dir, comb.v_cmd_dir);
    SETVAL(o_req_ready, comb.v_req_ready);
    SETVAL(o_crc7_clear, crc7_clear);
    SETVAL(o_crc7_next, comb.v_crc7_next);
    SETVAL(o_crc7_dat, comb.v_crc7_dat);
    SETVAL(o_resp_valid, resp_valid);
    SETVAL(o_resp_cmd, cmdmirror);
    SETVAL(o_resp_reg, regshift);
    SETVAL(o_resp_crc7_rx, crc_rx);
    SETVAL(o_resp_crc7_calc, crc_calc);
    SETVAL(o_cmdstate, cmdstate);
    SETVAL(o_cmderr, cmderr);
}
