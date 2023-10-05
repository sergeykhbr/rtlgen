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
    i_cs(this, "i_cs", "1", "dat3 in SPI mode."),
    o_spi_mode(this, "o_spi_mode", "1", "Detected SPI mode on CMD0"),
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
    i_cmd_resp_r1b(this, "i_cmd_resp_r1b", "1", "Same as R1 with zero line (any number of bits) card is busy, non-zero is ready"),
    i_cmd_resp_r2(this, "i_cmd_resp_r2", "1", "2-Bytes status"),
    i_cmd_resp_r3(this, "i_cmd_resp_r3", "1", "Read OCR 32 bits"),
    i_cmd_resp_r7(this, "i_cmd_resp_r7", "1", "CMD8 interface condition response"),
    i_stat_idle_state(this, "i_stat_idle_state", "1", "Card in idle state and running initialization process"),
    i_stat_erase_reset(this, "i_stat_erase_reset", "1", "Erase sequence was cleared before executing"),
    i_stat_illegal_cmd(this, "i_stat_illegal_cmd", "1", "Illegal command was detected"),
    i_stat_err_erase_sequence(this, "i_stat_err_erase_sequence", "1", "An error ini the sequence of erase commands occured"),
    i_stat_err_address(this, "i_stat_err_address", "1", "A misaligned adddres that didnot mathc block length"),
    i_stat_err_parameter(this, "i_stat_err_parameter", "1", "The command argument was otuside the allows range"),
    i_stat_locked(this, "i_stat_locked", "1", "Is set when card is locked by the user"),
    i_stat_wp_erase_skip(this, "i_stat_wp_erase_skip", "1", "Erase wp-sector or password error during card lock/unlock"),
    i_stat_err(this, "i_stat_err", "1", "A general or an unknown error occured during operation"),
    i_stat_err_cc(this, "i_stat_err_cc", "1", "Internal card controller error"),
    i_stat_ecc_failed(this, "i_stat_ecc_failed", "1", "Card internal ECC eas applied but failed to correct data"),
    i_stat_wp_violation(this, "i_stat_wp_violation", "1", "The command tried to write wp block"),
    i_stat_erase_param(this, "i_stat_erase_param", "1", "An invalid selection for erase, sectors or groups"),
    i_stat_out_of_range(this, "i_stat_out_of_range", "1"),
    o_busy(this, "o_busy", "1"),
    _cmdstate0_(this, ""),
    _cmdstate1_(this, "Receiver CMD state:"),
    CMDSTATE_REQ_STARTBIT(this, "4", "CMDSTATE_REQ_STARTBIT", "0"),
    CMDSTATE_REQ_TXBIT(this, "4", "CMDSTATE_REQ_TXBIT", "1"),
    CMDSTATE_REQ_CMD(this, "4", "CMDSTATE_REQ_CMD", "2"),
    CMDSTATE_REQ_ARG(this, "4", "CMDSTATE_REQ_ARG", "3"),
    CMDSTATE_REQ_CRC7(this, "4", "CMDSTATE_REQ_CRC7", "4"),
    CMDSTATE_REQ_STOPBIT(this, "4", "CMDSTATE_REQ_STOPBIT", "5"),
    CMDSTATE_REQ_VALID(this, "4", "CMDSTATE_REQ_VALID", "6"),
    CMDSTATE_WAIT_RESP(this, "4", "CMDSTATE_WAIT_RESP", "7"),
    CMDSTATE_RESP(this, "4", "CMDSTATE_RESP", "8"),
    CMDSTATE_RESP_CRC7(this, "4", "CMDSTATE_RESP_CRC7", "9"),
    CMDSTATE_RESP_STOPBIT(this, "4", "CMDSTATE_RESP_STOPBIT", "10"),
    CMDSTATE_INIT(this, "4", "CMDSTATE_INIT", "15"),
    // signals
    w_cmd_out(this, "w_cmd_out", "1"),
    w_crc7_clear(this, "w_crc7_clear", "1"),
    w_crc7_next(this, "w_crc7_next", "1"),
    w_crc7_dat(this, "w_crc7_dat", "1"),
    wb_crc7(this, "wb_crc7", "7"),
    // registers
    clkcnt(this, "clkcnt", "8"),
    cs(this, "cs", "1"),
    spi_mode(this, "spi_mode", "1"),
    cmdz(this, "cmdz", "1", "1"),
    cmd_dir(this, "cmd_dir", "1", "1"),
    cmd_rxshift(this, "cmd_rxshift", "48", "-1"),
    cmd_txshift(this, "cmd_txshift", "48", "-1"),
    cmd_state(this, "cmd_state", "4", "CMDSTATE_INIT"),
    cmd_req_crc_err(this, "cmd_req_crc_err", "1"),
    bitcnt(this, "bitcnt", "6"),
    txbit(this, "txbit", "1"),
    crc_calc(this, "crc_calc", "7"),
    crc_rx(this, "crc_rx", "7"),
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

TEXT();
    IF (NZ(i_cmd_req_ready));
        SETZERO(cmd_req_valid);
    ENDIF();
    SETVAL(clkcnt, INC(clkcnt));

TEXT();
    SWITCH(cmd_state);
    CASE (CMDSTATE_INIT);
        SETZERO(spi_mode);
        SETONE(cmd_dir);
        SETONE(comb.v_crc7_clear);
        SETZERO(cmd_req_crc_err);
        TEXT("Wait several (72) clocks to switch into idle state");
        IF (EQ(clkcnt, CONST("70")));
            SETVAL(cmd_state, CMDSTATE_REQ_STARTBIT);
        ENDIF();
        ENDCASE();
    CASE (CMDSTATE_REQ_STARTBIT);
        SETZERO(comb.v_busy);
        SETONE(comb.v_crc7_clear);
        IF (NZ(AND2(spi_mode, i_cs)));
            TEXT("Do nothing");
        ELSIF (AND2(NZ(cmdz), EZ(i_cmd)));
            SETVAL(cs, i_cs);
            SETVAL(cmd_state, CMDSTATE_REQ_TXBIT);
        ENDIF();
        ENDCASE();
    CASE (CMDSTATE_REQ_TXBIT);
        SETVAL(cmd_state, CMDSTATE_REQ_CMD);
        SETVAL(bitcnt, CONST("5", 6));
        SETONE(comb.v_crc7_next);
        SETVAL(txbit, i_cmd);
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
            SETVAL(crc_calc, wb_crc7);
        ELSE();
            SETVAL(bitcnt, DEC(bitcnt));
        ENDIF();
        ENDCASE();
    CASE (CMDSTATE_REQ_CRC7);
        SETVAL(crc_rx, CC2(BITS(crc_rx, 5, 0), i_cmd));
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
        IF (NE(crc_calc, crc_rx));
            SETONE(cmd_req_crc_err);
        ENDIF();
        IF (NZ(txbit));
            SETVAL(cmd_state, CMDSTATE_WAIT_RESP);
            SETONE(cmd_req_valid);
            IF (AND2(EZ(BITS(cmd_rxshift, 45, 40)), EZ(cs)));
                TEXT("CMD0 with CS = 0 (CD_DAT3)");
                SETONE(spi_mode);
            ENDIF();
        ELSE();
            SETVAL(cmd_state, CMDSTATE_REQ_STARTBIT);
            SETONE(cmd_dir);
        ENDIF();
        SETVAL(cmd_req_cmd, BITS(cmd_rxshift, 45, 40));
        SETVAL(cmd_req_data, BITS(cmd_rxshift, 39, 8));
        ENDCASE();
    CASE (CMDSTATE_WAIT_RESP);
        SETONE(cmd_resp_ready);
        IF (NZ(i_cmd_resp_valid));
            SETZERO(cmd_resp_ready);
            SETVAL(cmd_state, CMDSTATE_RESP);
            IF (EZ(spi_mode));
                SETVAL(bitcnt, CONST("39", 6));
                SETZERO(comb.vb_cmd_txshift);
                SETBITS(comb.vb_cmd_txshift, 45, 40, BITS(cmd_rxshift, 45, 40));
                SETBITS(comb.vb_cmd_txshift, 39, 8, i_cmd_resp_data32);
                SETBITS(comb.vb_cmd_txshift, 7, 0, CONST("0xFF", 8));
            ELSE();
                TEXT("Default R1 response in SPI mode:");
                SETVAL(bitcnt, CONST("7", 6));
                SETVAL(comb.vb_cmd_txshift, ALLONES());
                SETBIT(comb.vb_cmd_txshift, 47, CONST("0", 1));
                SETBIT(comb.vb_cmd_txshift, 46, i_stat_err_parameter);
                SETBIT(comb.vb_cmd_txshift, 45, i_stat_err_address);
                SETBIT(comb.vb_cmd_txshift, 44, i_stat_err_erase_sequence);
                SETBIT(comb.vb_cmd_txshift, 43, cmd_req_crc_err);
                SETBIT(comb.vb_cmd_txshift, 42, i_stat_illegal_cmd);
                SETBIT(comb.vb_cmd_txshift, 41, i_stat_erase_reset);
                SETBIT(comb.vb_cmd_txshift, 40, i_stat_idle_state);
                IF (NZ(i_cmd_resp_r2));
                    SETVAL(bitcnt, CONST("15", 6));
                    SETBIT(comb.vb_cmd_txshift, 39, i_stat_out_of_range);
                    SETBIT(comb.vb_cmd_txshift, 38, i_stat_erase_param);
                    SETBIT(comb.vb_cmd_txshift, 37, i_stat_wp_violation);
                    SETBIT(comb.vb_cmd_txshift, 36, i_stat_ecc_failed);
                    SETBIT(comb.vb_cmd_txshift, 35, i_stat_err_cc);
                    SETBIT(comb.vb_cmd_txshift, 34, i_stat_err);
                    SETBIT(comb.vb_cmd_txshift, 33, i_stat_wp_erase_skip);
                    SETBIT(comb.vb_cmd_txshift, 32, i_stat_locked);
                ELSIF (OR2(NZ(i_cmd_resp_r3), NZ(i_cmd_resp_r7)));
                    SETVAL(bitcnt, CONST("39", 6));
                    SETBITS(comb.vb_cmd_txshift, 39, 8, i_cmd_resp_data32);
                ENDIF();
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (CMDSTATE_RESP);
        SETVAL(comb.v_crc7_in, BIT(cmd_txshift, 47));
        IF (EZ(bitcnt));
            IF (EZ(spi_mode));
                SETVAL(bitcnt, CONST("6", 6));
                SETVAL(cmd_state, CMDSTATE_RESP_CRC7);
                SETBITS(comb.vb_cmd_txshift, 47, 40, CC2(wb_crc7, CONST("1", 1)));
                SETVAL(crc_calc, wb_crc7);
            ELSE();
                SETVAL(cmd_state, CMDSTATE_RESP_STOPBIT);
            ENDIF();
        ELSE();
            SETONE(comb.v_crc7_next);
            SETVAL(bitcnt, DEC(bitcnt));
        ENDIF();
    ENDCASE();
    CASE (CMDSTATE_RESP_CRC7);
        IF (EZ(bitcnt));
            SETVAL(cmd_state, CMDSTATE_RESP_STOPBIT);
        ELSE();
            SETVAL(bitcnt, DEC(bitcnt));
        ENDIF();
    ENDCASE();
    CASE (CMDSTATE_RESP_STOPBIT);
        SETVAL(cmd_state, CMDSTATE_REQ_STARTBIT);
        SETONE(cmd_dir);
        SETONE(comb.v_crc7_clear);
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    IF (LS(cmd_state, CMDSTATE_REQ_VALID));
        SETVAL(cmd_rxshift, CC2(BITS(cmd_rxshift, 46, 0), i_cmd));
        SETVAL(cmd_txshift, ALLONES());
    ELSE();
        IF (AND2(EQ(cmd_state, CMDSTATE_RESP_STOPBIT), EZ(bitcnt)));
            SETVAL(cmd_rxshift, ALLONES());
        ENDIF();
        SETVAL(cmd_txshift, comb.vb_cmd_txshift);
    ENDIF();

TEXT();
    IF (EZ(cmd_dir));
        TEXT("Output:");
        SETVAL(cmdz, BIT(cmd_txshift, 47));
    ELSE();
        TEXT("Input:");
        SETVAL(cmdz, i_cmd);
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
    SETVAL(o_spi_mode, spi_mode);
    SETVAL(o_busy, comb.v_busy);
}
