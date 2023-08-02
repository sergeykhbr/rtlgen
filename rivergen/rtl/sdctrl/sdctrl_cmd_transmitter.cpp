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
    i_req_valid(this, "i_req_valid", "1"),
    i_req_type(this, "i_req_type", "6"),
    i_req_arg(this, "i_req_arg", "32"),
    i_req_resp(this, "i_req_resp", "3", "R1, R3,R6 or R2"),
    o_req_ready(this, "o_req_ready", "1"),
    i_crc7(this, "i_crc7", "7"),
    o_crc7_clear(this, "o_crc7_clear", "1"),
    o_crc7_next(this, "o_crc7_next", "1"),
    o_crc7_dat(this, "o_crc7_dat", "1"),
    o_resp_valid(this, "o_resp_valid", "1"),
    o_resp_r1(this, "o_resp_r1", "8"),
    o_resp_r3r6(this, "o_resp_r3r6", "32", "OCR register (R3) or RCA register (R6)"),
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
    CMDSTATE_RESP_CONTENT(this, "4", "CMDSTATE_RESP_CONTENT", "6"),
    CMDSTATE_RESP_CRC7(this, "4", "CMDSTATE_RESP_CRC7", "7"),
    CMDSTATE_RESP_STOPBIT(this, "4", "CMDSTATE_RESP_STOPBIT", "8"),
    // signals
    // registers
    cmd_req_type(this, "cmd_req_type", "6"),
    cmd_req_resp(this, "cmd_req_resp", "3"),
    cmd_resp_ena(this, "cmd_resp_ena", "1"),
    cmd_resp_arg(this, "cmd_resp_arg", "32"),
    cmdshift(this, "cmdshift", "40", "-1"),
    cmdbitcnt(this, "cmdbitcnt", "6"),
    crc7_clear(this, "crc7_clear", "1", "1"),
    cmdstate(this, "cmdstate", "4", "CMDSTATE_IDLE"),
    cmderr(this, "cmderr", "4", "CMDERR_NONE"),
    //
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void sdctrl_cmd_transmitter::proc_comb() {
    SETVAL(comb.vb_cmdshift, cmdshift);

TEXT();
    TEXT("command state:");
    IF (NZ(i_sclk_negedge));
        TEXT("CMD Request:");
        IF (EQ(cmdstate, CMDSTATE_IDLE));
            SETVAL(comb.vb_cmdshift, ALLONES());
            SETONE(crc7_clear);
            SETONE(comb.v_req_ready);
            IF (NZ(i_req_valid));
                SETVAL(cmd_req_type, i_req_type);
                SETVAL(comb.vb_cmdshift, CC3(CONST("0x1", 2), i_req_type, i_req_arg));
                SETVAL(cmdbitcnt, CONST("39", 6));
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
                SETVAL(cmdbitcnt, CONST("6", 6));
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
            SETZERO(crc7_clear);
            IF (EQ(cmd_req_type, sdctrl_cfg_->R1));
                SETVAL(cmdbitcnt, CONST("6", 6));
            ELSIF (EQ(cmd_req_type, sdctrl_cfg_->R2));
                SETVAL(cmdbitcnt, CONST("37", 6));
            ENDIF();
        ENDIF();

    ELSIF (NZ(i_sclk_posedge));
        TEXT("CMD Response:");
        IF (EQ(cmdstate, CMDSTATE_RESP_WAIT));
            SETVAL(comb.vb_cmdshift, CC2(BITS(cmdshift, 38, 0), i_cmd));
            IF (EZ(i_cmd));
                SETONE(comb.v_crc7_next);
                SETVAL(cmdstate, CMDSTATE_RESP_TRANSBIT);
                SETVAL(cmdbitcnt, CONST("5", 6));
            ELSE();
                TEXT("TODO: watchdog no response");
            ENDIF();
        ELSIF(EQ(cmdstate, CMDSTATE_RESP_TRANSBIT));
            SETONE(comb.v_crc7_next);
            SETVAL(comb.vb_cmdshift, CC2(BITS(cmdshift, 38, 0), i_cmd));
            IF (EZ(i_cmd));
                SETVAL(cmdstate, CMDSTATE_RESP_CONTENT);
            ELSE();
                SETVAL(cmderr, sdctrl_cfg_->CMDERR_WRONG_RESPONSE);
                SETVAL(cmdstate, CMDSTATE_IDLE);
            ENDIF();
        ELSIF(EQ(cmdstate, CMDSTATE_RESP_CONTENT));
            SETONE(comb.v_crc7_next);
            SETVAL(comb.vb_cmdshift, CC2(BITS(cmdshift, 38, 0), i_cmd));
            IF (NZ(cmdbitcnt));
                SETVAL(cmdbitcnt, DEC(cmdbitcnt));
            ELSE();
                SETVAL(cmdstate, CMDSTATE_RESP_STOPBIT);
            ENDIF();
        ELSIF (EQ(cmdstate, CMDSTATE_RESP_STOPBIT));
            SETVAL(cmdstate, CMDSTATE_IDLE);
            SETONE(cmd_resp_ena);
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
    SETVAL(o_cmdstate, cmdstate);
    SETVAL(o_cmderr, cmderr);
}
