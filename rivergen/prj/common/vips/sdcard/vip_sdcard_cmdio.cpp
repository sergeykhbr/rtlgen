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

#include "vip_sdcard_cmdio.h"

vip_sdcard_cmdio::vip_sdcard_cmdio(GenObject *parent, const char *name) :
    ModuleObject(parent, "vip_sdcard_cmdio", name),
    i_nrst(this, "i_nrst", "1"),
    i_clk(this, "i_clk", "1"),
    i_cmd(this, "i_cmd", "1"),
    o_cmd(this, "o_cmd", "1"),
    o_cmd_dir(this, "o_cmd_dir", "1"),
    o_cmd_req_valid(this, "o_cmd_req_valid", "1"),
    o_cmd_req_cmd(this, "o_cmd_req_cmd", "6"),
    o_cmd_req_data(this, "o_cmd_req_data", "32"),
    i_cmd_req_ready(this, "i_cmd_req_ready", "1"),
    i_cmd_resp_valid(this, "i_cmd_resp_valid", "1"),
    i_cmd_resp_data32(this, "i_cmd_resp_data32", "32"),
    o_cmd_resp_ready(this, "o_cmd_resp_ready", "1"),
    _cmdstate0_(this, ""),
    _cmdstate1_(this, "Receiver CMD state:"),
    CMDSTATE_IDLE(this, "4", "CMDSTATE_IDLE", "0"),
    CMDSTATE_REQ_STARTBIT(this, "4", "CMDSTATE_REQ_STARTBIT", "1"),
    CMDSTATE_REQ_CMD(this, "4", "CMDSTATE_REQ_CMD", "2"),
    CMDSTATE_REQ_ARG(this, "4", "CMDSTATE_REQ_ARG", "3"),
    CMDSTATE_REQ_CRC7(this, "4", "CMDSTATE_REQ_CRC7", "4"),
    CMDSTATE_REQ_STOPBIT(this, "4", "CMDSTATE_REQ_STOPBIT", "5"),
    CMDSTATE_REQ_VALID(this, "4", "CMDSTATE_REQ_VALID", "6"),
    CMDSTATE_WAIT_RESP(this, "4", "CMDSTATE_WAIT_RESP", "7"),
    CMDSTATE_RESP(this, "4", "CMDSTATE_RESP", "8"),
    CMDSTATE_RESP_CRC7(this, "4", "CMDSTATE_RESP_CRC7", "9"),
    // signals
    w_cmd_out(this, "w_cmd_out", "1"),
    w_crc7_clear(this, "w_crc7_clear", "1"),
    w_crc7_next(this, "w_crc7_next", "1"),
    w_crc7_dat(this, "w_crc7_dat", "1"),
    wb_crc7(this, "wb_crc7", "7"),
    // registers
    cmdz(this, "cmdz", "1", "1"),
    cmd_dir(this, "cmd_dir", "1", "1"),
    cmd_rxshift(this, "cmd_rxshift", "48", "-1"),
    cmd_txshift(this, "cmd_txshift", "48", "-1"),
    cmd_state(this, "cmd_state", "4", "CMDSTATE_IDLE"),
    bitcnt(this, "bitcnt", "6"),
    cmd_req_valid(this, "cmd_req_valid", "1"),
    cmd_req_cmd(this, "cmd_req_cmd", "6"),
    cmd_req_data(this, "cmd_req_data", "32"),
    cmd_resp_ready(this, "cmd_resp_ready", "1"),
    //
    comb(this),
    crccmd0(this, "crccmd0")
{
    Operation::start(this);

    NEW(crccmd0, crccmd0.getName().c_str());
        CONNECT(crccmd0, 0, crccmd0.i_clk, i_clk);
        CONNECT(crccmd0, 0, crccmd0.i_nrst, i_nrst);
        CONNECT(crccmd0, 0, crccmd0.i_clear, w_crc7_clear);
        CONNECT(crccmd0, 0, crccmd0.i_next, w_crc7_next);
        CONNECT(crccmd0, 0, crccmd0.i_dat, w_crc7_dat);
        CONNECT(crccmd0, 0, crccmd0.o_crc7, wb_crc7);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void vip_sdcard_cmdio::proc_comb() {
    SETVAL(comb.vb_cmd_txshift, CC2(BITS(cmd_txshift, 46, 0), CONST("1", 1)));
    SETVAL(comb.v_crc7_in, i_cmd);
    SETVAL(cmdz, i_cmd);

TEXT();
    IF (NZ(i_cmd_req_ready));
        SETZERO(cmd_req_valid);
    ENDIF();

TEXT();
    SWITCH(cmd_state);
    CASE (CMDSTATE_IDLE);
        SETONE(cmd_dir);
        IF (NZ(i_cmd));
            SETONE(comb.v_crc7_next);
            SETVAL(cmd_state, CMDSTATE_REQ_STARTBIT);
        ELSE();
            SETONE(comb.v_crc7_clear);
        ENDIF();
        ENDCASE();
    CASE (CMDSTATE_REQ_STARTBIT);
        IF (NZ(i_cmd));
            SETONE(comb.v_crc7_next);
            SETVAL(cmd_state, CMDSTATE_REQ_CMD);
            SETVAL(bitcnt, CONST("5", 6));
        ELSE();
            SETONE(comb.v_crc7_clear);
            SETVAL(cmd_state, CMDSTATE_IDLE);
        ENDIF();
        ENDCASE();
    CASE (CMDSTATE_REQ_CMD);
        SETONE(comb.v_crc7_next);
        IF (EZ(bitcnt));
            SETVAL(bitcnt, CONST("31", 6));
            SETVAL(cmd_state, CMDSTATE_REQ_ARG);
        ELSE();
            SETVAL(bitcnt, DEC(bitcnt));
        ENDIF();
        ENDCASE();
    CASE (CMDSTATE_REQ_ARG);
        SETONE(comb.v_crc7_next);
        IF (EZ(bitcnt));
            SETVAL(cmd_state, CMDSTATE_REQ_CRC7);
            SETVAL(bitcnt, CONST("6", 6));
        ELSE();
            SETVAL(bitcnt, DEC(bitcnt));
        ENDIF();
        ENDCASE();
    CASE (CMDSTATE_REQ_CRC7);
        IF (EZ(bitcnt));
            SETVAL(cmd_state, CMDSTATE_REQ_STOPBIT);
        ELSE();
            SETVAL(bitcnt, DEC(bitcnt));
        ENDIF();
        ENDCASE();
    CASE (CMDSTATE_REQ_STOPBIT);
        SETVAL(cmd_state, CMDSTATE_REQ_VALID);
        SETZERO(cmd_dir);
        SETONE(comb.v_crc7_clear);
        ENDCASE();
    CASE (CMDSTATE_REQ_VALID);
        SETVAL(cmd_state, CMDSTATE_WAIT_RESP);
        SETONE(cmd_req_valid);
        SETVAL(cmd_req_cmd, BITS(cmd_rxshift, 45, 40));
        SETVAL(cmd_req_data, BITS(cmd_rxshift, 39, 8));
        SETONE(cmd_resp_ready);
        ENDCASE();
    CASE (CMDSTATE_WAIT_RESP);
        IF (NZ(i_cmd_resp_valid));
            SETZERO(cmd_resp_ready);
            SETVAL(cmd_state, CMDSTATE_RESP);
            SETVAL(bitcnt, CONST("39", 6));
            SETZERO(comb.vb_cmd_txshift);
            SETBITS(comb.vb_cmd_txshift, 45, 40, BITS(cmd_rxshift, 45, 40));
            SETBITS(comb.vb_cmd_txshift, 39, 8, i_cmd_resp_data32);
            SETBITS(comb.vb_cmd_txshift, 7, 0, CONST("0xFF", 8));
        ENDIF();
        ENDCASE();
    CASE (CMDSTATE_RESP);
        SETVAL(comb.v_crc7_in, BIT(cmd_txshift, 47));
        IF (EZ(bitcnt));
            SETVAL(bitcnt, CONST("7", 6));
            SETVAL(cmd_state, CMDSTATE_RESP_CRC7);
            SETBITS(comb.vb_cmd_txshift, 47, 40, CC2(wb_crc7, CONST("1", 1)));
        ELSE();
            SETONE(comb.v_crc7_next);
            SETVAL(bitcnt, DEC(bitcnt));
        ENDIF();
    ENDCASE();
    CASE (CMDSTATE_RESP_CRC7);
        IF (EZ(bitcnt));
            SETVAL(cmd_state, CMDSTATE_IDLE);
            SETONE(cmd_dir);
        ELSE();
            SETVAL(bitcnt, DEC(bitcnt));
        ENDIF();
    ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    IF (LS(cmd_state, CMDSTATE_REQ_VALID));
        SETVAL(cmd_rxshift, CC2(BITS(cmd_rxshift, 46, 0), i_cmd));
        SETVAL(cmd_txshift, ALLONES());
    ELSE();
        IF (AND2(EQ(cmd_state, CMDSTATE_RESP_CRC7), EZ(bitcnt)));
            SETVAL(cmd_rxshift, ALLONES());
        ENDIF();
        SETVAL(cmd_txshift, comb.vb_cmd_txshift);
    ENDIF();


TEXT();
    SETVAL(w_crc7_clear, comb.v_crc7_clear);
    SETVAL(w_crc7_next, comb.v_crc7_next);
    SETVAL(w_crc7_dat, comb.v_crc7_in);
    SETVAL(o_cmd, BIT(cmd_txshift, 47));
    SETVAL(o_cmd_dir, cmd_dir);
    SETVAL(o_cmd_req_valid, cmd_req_valid);
    SETVAL(o_cmd_req_cmd, cmd_req_cmd);
    SETVAL(o_cmd_req_data, cmd_req_data);
    SETVAL(o_cmd_resp_ready, cmd_resp_ready);
}
