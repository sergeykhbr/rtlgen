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

#include "vip_sdcard_top.h"

vip_sdcard_top::vip_sdcard_top(GenObject *parent, const char *name) :
    ModuleObject(parent, "vip_sdcard_top", name),
    i_sclk(this, "i_sclk", "1"),
    io_cmd(this, "io_cmd", "1"),
    io_dat0(this, "io_dat0", "1"),
    io_dat1(this, "io_dat1", "1"),
    io_dat2(this, "io_dat2", "1"),
    io_cd_dat3(this, "io_cd_dat3", "1"),
    // params
    _cmdstate0_(this, "Receiver CMD state:"),
    CMDSTATE_IDLE(this, "3", "CMDSTATE_IDLE", "0"),
    CMDSTATE_REQ_ARG(this, "3", "CMDSTATE_REQ_ARG", "1"),
    CMDSTATE_REQ_CRC7(this, "3", "CMDSTATE_REQ_CRC7", "2"),
    CMDSTATE_WAIT_RESP(this, "3", "CMDSTATE_WAIT_RESP", "3"),
    CMDSTATE_RESP(this, "3", "CMDSTATE_RESP", "4"),
    // signals
    w_clk(this, "w_clk", "1"),
    wb_rdata(this, "wb_rdata", "8"),
    w_cmd_in(this, "w_cmd_in", "1"),
    w_cmd_out(this, "w_cmd_out", "1"),
    // registers
    cmd_dir(this, "cmd_dir", "1", "1"),
    cmd_rxshift(this, "cmd_rxshift", "48", "-1"),
    cmd_txshift(this, "cmd_txshift", "48", "-1"),
    cmd_state(this, "cmd_state", "2", "CMDSTATE_IDLE"),
    bitcnt(this, "bitcnt", "6"),
    //
    comb(this),
    iobufcmd0(this, "iobufcmd0")
{
    Operation::start(this);
    NEW(iobufcmd0, iobufcmd0.getName().c_str());
        CONNECT(iobufcmd0, 0, iobufcmd0.io, io_cmd);
        CONNECT(iobufcmd0, 0, iobufcmd0.o, w_cmd_in);
        CONNECT(iobufcmd0, 0, iobufcmd0.i, w_cmd_out);
        CONNECT(iobufcmd0, 0, iobufcmd0.t, cmd_dir);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void vip_sdcard_top::proc_comb() {
    SETVAL(comb.vb_cmd_txshift, cmd_txshift);

TEXT();
    SWITCH(cmd_state);
    CASE (CMDSTATE_IDLE);
        IF (EQ(BITS(cmd_rxshift, 7, 6), CONST("1", 2)));
            SETVAL(cmd_state, CMDSTATE_REQ_ARG);
            SETVAL(bitcnt, CONST("31", 6));
        ENDIF();
        ENDCASE();
    CASE (CMDSTATE_REQ_ARG);
        IF (EZ(bitcnt));
            SETVAL(cmd_state, CMDSTATE_REQ_CRC7);
            SETVAL(bitcnt, CONST("7", 6));
        ELSE();
            SETVAL(bitcnt, DEC(bitcnt));
        ENDIF();
        ENDCASE();
    CASE (CMDSTATE_REQ_CRC7);
        IF (EZ(bitcnt));
            SETVAL(cmd_state, CMDSTATE_WAIT_RESP);
            SETVAL(bitcnt, CONST("10", 6));
        ELSE();
            SETVAL(bitcnt, DEC(bitcnt));
        ENDIF();
        ENDCASE();
    CASE (CMDSTATE_WAIT_RESP);
        TEXT("Preparing output with some delay (several clocks):");
        IF (EZ(bitcnt));
            SETVAL(cmd_state, CMDSTATE_RESP);
            SETVAL(bitcnt, CONST("47", 6));
            SETZERO(cmd_dir);
            SETBITS(comb.vb_cmd_txshift, 47, 46, CONST("0", 2));
            SETBITS(comb.vb_cmd_txshift, 45, 40, BITS(cmd_rxshift, 45, 0));
            SETBITS(comb.vb_cmd_txshift, 39, 8, CONST("0x55555555", 32));
            SETBITS(comb.vb_cmd_txshift, 7, 0, CONST("0xFF", 8));
        ELSE();
            SETVAL(bitcnt, DEC(bitcnt));
        ENDIF();
        ENDCASE();
    CASE (CMDSTATE_RESP);
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
    SETVAL(cmd_txshift, comb.vb_cmd_txshift);

TEXT();
    IF (LS(cmd_state, CMDSTATE_WAIT_RESP));
        SETVAL(cmd_rxshift, CC2(BITS(cmd_rxshift, 46, 0), w_cmd_in));
    ELSE();
        SETVAL(cmd_txshift, CC2(BITS(cmd_txshift, 46, 0), CONST("1", 1)));
    ENDIF();


TEXT();
    SETVAL(w_cmd_out, BIT(cmd_txshift, 47));
}
