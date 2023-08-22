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
    i_nrst(this, "i_nrst", "1", "To avoid undefined states of registers (xxx)"),
    i_sclk(this, "i_sclk", "1"),
    io_cmd(this, "io_cmd", "1"),
    io_dat0(this, "io_dat0", "1"),
    io_dat1(this, "io_dat1", "1"),
    io_dat2(this, "io_dat2", "1"),
    io_cd_dat3(this, "io_cd_dat3", "1"),
    // params
    _cfg0_(this, "Generic config parameters"),
    CFG_SDCARD_VHS(this, "4", "CFG_SDCARD_VHS", "0x1", "CMD8 Voltage supply mask"),
    CFG_SDCARD_PCIE_1_2V(this, "1", "CFG_SDCARD_PCIE_1_2V", "0"),
    CFG_SDCARD_PCIE_AVAIL(this, "1", "CFG_SDCARD_PCIE_AVAIL", "0"),
    _cmdstate0_(this, ""),
    _cmdstate1_(this, "Receiver CMD state:"),
    CMDSTATE_IDLE(this, "4", "CMDSTATE_IDLE", "0"),
    CMDSTATE_REQ_STARTBIT(this, "4", "CMDSTATE_REQ_STARTBIT", "1"),
    CMDSTATE_REQ_CMD(this, "4", "CMDSTATE_REQ_CMD", "2"),
    CMDSTATE_REQ_ARG(this, "4", "CMDSTATE_REQ_ARG", "3"),
    CMDSTATE_REQ_CRC7(this, "4", "CMDSTATE_REQ_CRC7", "4"),
    CMDSTATE_REQ_STOPBIT(this, "4", "CMDSTATE_REQ_STOPBIT", "5"),
    CMDSTATE_WAIT_RESP(this, "4", "CMDSTATE_WAIT_RESP", "6"),
    CMDSTATE_RESP(this, "4", "CMDSTATE_RESP", "7"),
    CMDSTATE_RESP_CRC7(this, "4", "CMDSTATE_RESP_CRC7", "8"),
    // signals
    w_clk(this, "w_clk", "1"),
    wb_rdata(this, "wb_rdata", "8"),
    w_cmd_in(this, "w_cmd_in", "1"),
    w_cmd_out(this, "w_cmd_out", "1"),
    w_crc7_clear(this, "w_crc7_clear", "1"),
    w_crc7_next(this, "w_crc7_next", "1"),
    w_crc7_dat(this, "w_crc7_dat", "1"),
    wb_crc7(this, "wb_crc7", "7"),
    // registers
    cmd_dir(this, "cmd_dir", "1", "1"),
    cmd_rxshift(this, "cmd_rxshift", "48", "-1"),
    cmd_txshift(this, "cmd_txshift", "48", "-1"),
    cmd_state(this, "cmd_state", "4", "CMDSTATE_IDLE"),
    bitcnt(this, "bitcnt", "6"),
    //
    comb(this),
    iobufcmd0(this, "iobufcmd0"),
    crccmd0(this, "crccmd0")
{
    Operation::start(this);
    NEW(iobufcmd0, iobufcmd0.getName().c_str());
        CONNECT(iobufcmd0, 0, iobufcmd0.io, io_cmd);
        CONNECT(iobufcmd0, 0, iobufcmd0.o, w_cmd_in);
        CONNECT(iobufcmd0, 0, iobufcmd0.i, w_cmd_out);
        CONNECT(iobufcmd0, 0, iobufcmd0.t, cmd_dir);
    ENDNEW();

    NEW(crccmd0, crccmd0.getName().c_str());
        CONNECT(crccmd0, 0, crccmd0.i_clk, i_sclk);
        CONNECT(crccmd0, 0, crccmd0.i_nrst, i_nrst);
        CONNECT(crccmd0, 0, crccmd0.i_clear, w_crc7_clear);
        CONNECT(crccmd0, 0, crccmd0.i_next, w_crc7_next);
        CONNECT(crccmd0, 0, crccmd0.i_dat, w_crc7_dat);
        CONNECT(crccmd0, 0, crccmd0.o_crc7, wb_crc7);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void vip_sdcard_top::proc_comb() {
    SETVAL(comb.vb_cmd_txshift, CC2(BITS(cmd_txshift, 46, 0), CONST("1", 1)));
    SETVAL(comb.v_crc7_in, w_cmd_in);;

TEXT();
    SWITCH(cmd_state);
    CASE (CMDSTATE_IDLE);
        SETONE(cmd_dir);
        IF (EZ(w_cmd_in));
            SETONE(comb.v_crc7_next);
            SETVAL(cmd_state, CMDSTATE_REQ_STARTBIT);
        ELSE();
            SETONE(comb.v_crc7_clear);
        ENDIF();
        ENDCASE();
    CASE (CMDSTATE_REQ_STARTBIT);
        IF (NZ(w_cmd_in));
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
            SETVAL(bitcnt, CONST("10", 6));
        ELSE();
            SETVAL(bitcnt, DEC(bitcnt));
        ENDIF();
        ENDCASE();
    CASE (CMDSTATE_REQ_STOPBIT);
        SETVAL(cmd_state, CMDSTATE_WAIT_RESP);
        SETZERO(cmd_dir);
        SETONE(comb.v_crc7_clear);
        ENDCASE();
    CASE (CMDSTATE_WAIT_RESP);
        TEXT("Preparing output with some delay (several clocks):");
        IF (EZ(bitcnt));
            SETVAL(cmd_state, CMDSTATE_RESP);
            SETVAL(bitcnt, CONST("39", 6));
            SETZERO(comb.vb_cmd_txshift);
            SETBITS(comb.vb_cmd_txshift, 45, 40, BITS(cmd_rxshift, 45, 40));
            SETBITS(comb.vb_cmd_txshift, 7, 0, CONST("0xFF", 8));

            TEXT();
            TEXT("Commands response arguments:");
            SWITCH (BITS(cmd_rxshift, 45, 40));
            CASE(CONST("8", 6), "CMD8: SEND_IF_COND. Send memory Card interface condition");
                TEXT("[21] PCIe 1.2V support");
                TEXT("[20] PCIe availability");
                TEXT("[19:16] Voltage supply");
                TEXT("[15:8] check pattern");
                SETBIT(comb.vb_cmd_txshift, 21,
                        AND2_L(BIT(cmd_rxshift, 21), CFG_SDCARD_PCIE_1_2V));
                SETBIT(comb.vb_cmd_txshift, 20,
                        AND2_L(BIT(cmd_rxshift, 20), CFG_SDCARD_PCIE_AVAIL));
                SETBITS(comb.vb_cmd_txshift, 19, 16,
                        AND2_L(BITS(cmd_rxshift, 19, 16), CFG_SDCARD_VHS));
                SETBITS(comb.vb_cmd_txshift, 15, 8, BITS(cmd_rxshift, 15, 8));
                ENDCASE();
            CASE(CONST("41", 6), "ACMD41: SD_SEND_OP_COND. Send host capacity info");
                TEXT("[31] HCS (OCR[30]) Host Capacity");
                TEXT("[28] XPC");
                TEXT("[24] S18R");
                TEXT("[23:0] VDD Voltage Window (OCR[23:0])");
                ENDCASE();
            CASEDEF();    
                SETBITS(comb.vb_cmd_txshift, 39, 8, CONST("0", 32));
                ENDCASE();
            ENDSWITCH();
        ELSE();
            SETVAL(bitcnt, DEC(bitcnt));
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
    IF (LE(cmd_state, CMDSTATE_REQ_STOPBIT));
        TEXT("This will includes clock with the stopbit itself");
        SETVAL(cmd_rxshift, CC2(BITS(cmd_rxshift, 46, 0), w_cmd_in));
        SETVAL(cmd_txshift, ALLONES());
    ELSE();
        IF (AND2(EQ(cmd_state, CMDSTATE_RESP_CRC7), EZ(bitcnt)));
            SETVAL(cmd_rxshift, ALLONES());
        ENDIF();
        SETVAL(cmd_txshift, comb.vb_cmd_txshift);
    ENDIF();


TEXT();
    SETVAL(w_cmd_out, BIT(cmd_txshift, 47));
    SETVAL(w_crc7_clear, comb.v_crc7_clear);
    SETVAL(w_crc7_next, comb.v_crc7_next);
    SETVAL(w_crc7_dat, comb.v_crc7_in);
}
