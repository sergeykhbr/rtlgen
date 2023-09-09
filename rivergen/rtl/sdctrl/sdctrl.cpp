// 
//  Copyright 2022 Sergey Khabarov, sergeykhbr@gmail.com
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

#include "sdctrl.h"

sdctrl::sdctrl(GenObject *parent, const char *name) :
    ModuleObject(parent, "sdctrl", name),
    log2_fifosz(this, "log2_fifosz", "9"),
    fifo_dbits(this, "fifo_dbits", "8"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_xmapinfo(this, "i_xmapinfo", "APB interconnect slot information"),
    o_xcfg(this, "o_xcfg", "APB Device descriptor"),
    i_xslvi(this, "i_xslvi", "AXI input interface to access SD-card memory"),
    o_xslvo(this, "o_xslvo", "AXI output interface to access SD-card memory"),
    i_pmapinfo(this, "i_pmapinfo", "APB interconnect slot information"),
    o_pcfg(this, "o_pcfg", "APB sd-controller configuration registers descriptor"),
    i_apbi(this, "i_apbi", "APB Slave to Bridge interface"),
    o_apbo(this, "o_apbo", "APB Bridge to Slave interface"),
    o_sclk(this, "o_sclk", "1", "Clock up to 50 MHz"),
    i_cmd(this, "i_cmd", "1", "Command response;"),
    o_cmd(this, "o_cmd", "1", "Command request; DO in SPI mode"),
    o_cmd_dir(this, "o_cmd_dir", "1", "Direction bit: 1=input; 0=output"),
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
    o_cd_dat3(this, "o_cd_dat3", "1", "Card Detect / Data Line[3] output; CS output in SPI mode"),
    o_cd_dat3_dir(this, "o_cd_dat3_dir", "1", "Direction bit: 1=input; 0=output"),
    i_detected(this, "i_detected", "1"),
    i_protect(this, "i_protect", "1"),
    // params
    _sdstate0_(this, "SD-card states see Card Status[12:9] CURRENT_STATE on page 145:"),
    SDSTATE_PRE_INIT(this, "4", "SDSTATE_PRE_INIT", "0xF"),
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
    _idlestate0_(this, "SD-card 'idle' state substates:"),
    IDLESTATE_CMD0(this, "3", "IDLESTATE_CMD0", "0"),
    IDLESTATE_CMD8(this, "3", "IDLESTATE_CMD8", "1"),
    IDLESTATE_CMD55(this, "3", "IDLESTATE_CMD55", "2"),
    IDLESTATE_ACMD41(this, "3", "IDLESTATE_ACMD41", "3"),
    IDLESTATE_CARD_IDENTIFICATION(this, "3", "IDLESTATE_CARD_IDENTIFICATION", "4"),
    _readystate0_(this, "SD-card 'ready' state substates:"),
    READYSTATE_CMD11(this, "2", "READYSTATE_CMD11", "0"),
    READYSTATE_CMD2(this, "2", "READYSTATE_CMD2", "1"),
    READYSTATE_CHECK_CID(this, "2", "READYSTATE_CHECK_CID", "2", "State change: ready -> ident"),
    _identstate0_(this, "SD-card 'ident' state substates:"),
    IDENTSTATE_CMD3(this, "1", "IDENTSTATE_CMD3", "0"),
    IDENTSTATE_CHECK_RCA(this, "1", "IDENTSTATE_CHECK_RCA", "1", "State change: ident -> stby"),
    // signals
    w_regs_sck_posedge(this, "w_regs_sck_posedge", "1"),
    w_regs_sck_negedge(this, "w_regs_sck", "1"),
    w_regs_clear_cmderr(this, "w_regs_clear_cmderr", "1"),
    wb_regs_watchdog(this, "wb_regs_watchdog", "16"),
    w_regs_pcie_12V_support(this, "w_regs_pcie_12V_support", "1"),
    w_regs_pcie_available(this, "w_regs_pcie_available", "1"),
    wb_regs_voltage_supply(this, "wb_regs_voltage_supply", "4"),
    wb_regs_check_pattern(this, "wb_regs_check_pattern", "8"),
    w_mem_req_valid(this, "w_mem_req_valid", "1"),
    wb_mem_req_addr(this, "wb_mem_req_addr", "CFG_SYSBUS_ADDR_BITS"),
    wb_mem_req_size(this, "wb_mem_req_size", "8"),
    w_mem_req_write(this, "w_mem_req_write", "1"),
    wb_mem_req_wdata(this, "wb_mem_req_wdata", "CFG_SYSBUS_DATA_BITS"),
    wb_mem_req_wstrb(this, "wb_mem_req_wstrb", "CFG_SYSBUS_DATA_BYTES"),
    w_mem_req_last(this, "w_mem_req_last", "1"),
    w_mem_req_ready(this, "w_mem_req_ready", "1"),
    w_mem_resp_valid(this, "w_mem_resp_valid", "1"),
    wb_mem_resp_rdata(this, "wb_mem_resp_rdata", "CFG_SYSBUS_DATA_BITS"),
    wb_mem_resp_err(this, "wb_mem_resp_err", "1"),
    w_cmd_req_ready(this, "w_cmd_req_ready", "1"),
    w_cmd_resp_valid(this, "w_cmd_resp_valid", "1"),
    wb_cmd_resp_cmd(this, "wb_cmd_resp_cmd", "6"),
    wb_cmd_resp_reg(this, "wb_cmd_resp_reg", "32"),
    wb_cmd_resp_crc7_rx(this, "wb_cmd_resp_crc7_rx", "7"),
    wb_cmd_resp_crc7_calc(this, "wb_cmd_resp_crc7_calc", "7"),
    w_cmd_resp_ready(this, "w_cmd_resp_ready", "1"),
    wb_trx_cmdstate(this, "wb_trx_cmdstate", "4"),
    wb_trx_cmderr(this, "wb_trx_cmderr", "4"),
    w_clear_cmderr(this, "w_clear_cmderr", "1"),
    w_400kHz_ena(this, "w_400kHz_ena", "1"),
    w_crc7_clear(this, "w_crc7_clear", "1"),
    w_crc7_next(this, "w_crc7_next", "1"),
    w_crc7_dat(this, "w_crc7_dat", "1"),
    wb_crc7(this, "wb_crc7", "7"),
    w_crc16_next(this, "w_crc16_next", "1"),
    wb_crc16_dat(this, "wb_crc16_dat", "4"),
    wb_crc16(this, "wb_crc16", "16"),
    // registers
    clkcnt(this, "clkcnt", "7"),
    cmd_set_low(this, "cmd_set_low", "1"),
    cmd_req_valid(this, "cmd_req_valid", "1"),
    cmd_req_cmd(this, "cmd_req_cmd", "6"),
    cmd_req_arg(this, "cmd_req_arg", "32"),
    cmd_req_rn(this, "cmd_req_rn", "3"),
    cmd_resp_cmd(this, "cmd_resp_r1", "6"),
    cmd_resp_reg(this, "cmd_resp_reg", "32"),
    crc16_clear(this, "crc16_clear", "1", "1"),
    dat(this, "dat", "4", "-1"),
    dat_dir(this, "dat_dir", "1", "DIR_OUTPUT"),
    sdstate(this, "sdstate", "4", "SDSTATE_PRE_INIT"),
    idlestate(this, "initstate", "3", "IDLESTATE_CMD0"),
    readystate(this, "readystate", "2", "READYSTATE_CMD11"),
    identstate(this, "identstate", "1", "IDENTSTATE_CMD3"),
    wait_cmd_resp(this, "wait_cmd_resp", "1"),
    sdtype(this, "sdtype", "3", "SDCARD_UNKNOWN"),
    HCS(this, "HCS", "1", "1", "High Capacity Support"),
    S18(this, "S18", "1", "0", "1.8V Low voltage"),
    RCA(this, "RCA", "32", "0", "Relative Address"),
    OCR_VoltageWindow(this, "OCR_VoltageWindow", "24", "0xff8000", "all ranges 2.7 to 3.6 V"),
    //
    comb(this),
    xslv0(this, "xslv0"),
    regs0(this, "regs0"),
    crccmd0(this, "crccmd0"),
    crcdat0(this, "crcdat0"),
    cmdtrx0(this, "cmdtrx0")
{
    Operation::start(this);

    xslv0.vid.setObjValue(&glob_pnp_cfg_->VENDOR_OPTIMITECH);
    xslv0.did.setObjValue(&glob_pnp_cfg_->OPTIMITECH_SDCTRL_MEM);
    NEW(xslv0, xslv0.getName().c_str());
        CONNECT(xslv0, 0, xslv0.i_clk, i_clk);
        CONNECT(xslv0, 0, xslv0.i_nrst, i_nrst);
        CONNECT(xslv0, 0, xslv0.i_mapinfo, i_xmapinfo);
        CONNECT(xslv0, 0, xslv0.o_cfg, o_xcfg);
        CONNECT(xslv0, 0, xslv0.i_xslvi, i_xslvi);
        CONNECT(xslv0, 0, xslv0.o_xslvo, o_xslvo);
        CONNECT(xslv0, 0, xslv0.o_req_valid, w_mem_req_valid);
        CONNECT(xslv0, 0, xslv0.o_req_addr, wb_mem_req_addr);
        CONNECT(xslv0, 0, xslv0.o_req_size, wb_mem_req_size);
        CONNECT(xslv0, 0, xslv0.o_req_write, w_mem_req_write);
        CONNECT(xslv0, 0, xslv0.o_req_wdata, wb_mem_req_wdata);
        CONNECT(xslv0, 0, xslv0.o_req_wstrb, wb_mem_req_wstrb);
        CONNECT(xslv0, 0, xslv0.o_req_last, w_mem_req_last);
        CONNECT(xslv0, 0, xslv0.i_req_ready, w_mem_req_ready);
        CONNECT(xslv0, 0, xslv0.i_resp_valid, w_mem_resp_valid);
        CONNECT(xslv0, 0, xslv0.i_resp_rdata, wb_mem_resp_rdata);
        CONNECT(xslv0, 0, xslv0.i_resp_err, wb_mem_resp_err);
    ENDNEW();

    NEW(regs0, regs0.getName().c_str());
        CONNECT(regs0, 0, regs0.i_clk, i_clk);
        CONNECT(regs0, 0, regs0.i_nrst, i_nrst);
        CONNECT(regs0, 0, regs0.i_pmapinfo, i_pmapinfo);
        CONNECT(regs0, 0, regs0.o_pcfg, o_pcfg);
        CONNECT(regs0, 0, regs0.i_apbi, i_apbi);
        CONNECT(regs0, 0, regs0.o_apbo, o_apbo);
        CONNECT(regs0, 0, regs0.o_sck, o_sclk);
        CONNECT(regs0, 0, regs0.o_sck_posedge, w_regs_sck_posedge);
        CONNECT(regs0, 0, regs0.o_sck_negedge, w_regs_sck_negedge);
        CONNECT(regs0, 0, regs0.o_watchdog, wb_regs_watchdog);
        CONNECT(regs0, 0, regs0.o_clear_cmderr, w_regs_clear_cmderr);
        CONNECT(regs0, 0, regs0.o_pcie_12V_support, w_regs_pcie_12V_support);
        CONNECT(regs0, 0, regs0.o_pcie_available, w_regs_pcie_available);
        CONNECT(regs0, 0, regs0.o_voltage_supply, wb_regs_voltage_supply);
        CONNECT(regs0, 0, regs0.o_check_pattern, wb_regs_check_pattern);
        CONNECT(regs0, 0, regs0.i_400khz_ena, w_400kHz_ena);
        CONNECT(regs0, 0, regs0.i_sdtype, sdtype);
        CONNECT(regs0, 0, regs0.i_sdstate, sdstate);
        CONNECT(regs0, 0, regs0.i_cmd_state, wb_trx_cmdstate);
        CONNECT(regs0, 0, regs0.i_cmd_err, wb_trx_cmderr);
        CONNECT(regs0, 0, regs0.i_cmd_req_valid, cmd_req_valid);
        CONNECT(regs0, 0, regs0.i_cmd_req_cmd, cmd_req_cmd);
        CONNECT(regs0, 0, regs0.i_cmd_resp_valid, w_cmd_resp_valid);
        CONNECT(regs0, 0, regs0.i_cmd_resp_cmd, wb_cmd_resp_cmd);
        CONNECT(regs0, 0, regs0.i_cmd_resp_reg, wb_cmd_resp_reg);
        CONNECT(regs0, 0, regs0.i_cmd_resp_crc7_rx, wb_cmd_resp_crc7_rx);
        CONNECT(regs0, 0, regs0.i_cmd_resp_crc7_calc, wb_cmd_resp_crc7_calc);
    ENDNEW();

    NEW(crccmd0, crccmd0.getName().c_str());
        CONNECT(crccmd0, 0, crccmd0.i_clk, i_clk);
        CONNECT(crccmd0, 0, crccmd0.i_nrst, i_nrst);
        CONNECT(crccmd0, 0, crccmd0.i_clear, w_crc7_clear);
        CONNECT(crccmd0, 0, crccmd0.i_next, w_crc7_next);
        CONNECT(crccmd0, 0, crccmd0.i_dat, w_crc7_dat);
        CONNECT(crccmd0, 0, crccmd0.o_crc7, wb_crc7);
    ENDNEW();

    NEW(crcdat0, crcdat0.getName().c_str());
        CONNECT(crcdat0, 0, crcdat0.i_clk, i_clk);
        CONNECT(crcdat0, 0, crcdat0.i_nrst, i_nrst);
        CONNECT(crcdat0, 0, crcdat0.i_clear, crc16_clear);
        CONNECT(crcdat0, 0, crcdat0.i_next, w_crc16_next);
        CONNECT(crcdat0, 0, crcdat0.i_dat, wb_crc16_dat);
        CONNECT(crcdat0, 0, crcdat0.o_crc16, wb_crc16);
    ENDNEW();

    NEW(cmdtrx0, cmdtrx0.getName().c_str());
        CONNECT(cmdtrx0, 0, cmdtrx0.i_clk, i_clk);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_nrst, i_nrst);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_sclk_posedge, w_regs_sck_posedge);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_sclk_negedge, w_regs_sck_negedge);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_cmd, i_cmd);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_cmd, o_cmd);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_cmd_dir, o_cmd_dir);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_watchdog, wb_regs_watchdog);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_cmd_set_low, cmd_set_low);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_req_valid, cmd_req_valid);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_req_cmd, cmd_req_cmd);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_req_arg, cmd_req_arg);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_req_rn, cmd_req_rn);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_req_ready, w_cmd_req_ready);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_crc7, wb_crc7);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_crc7_clear, w_crc7_clear);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_crc7_next, w_crc7_next);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_crc7_dat, w_crc7_dat);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_resp_valid, w_cmd_resp_valid);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_resp_cmd, wb_cmd_resp_cmd);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_resp_reg, wb_cmd_resp_reg);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_resp_crc7_rx, wb_cmd_resp_crc7_rx);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_resp_crc7_calc, wb_cmd_resp_crc7_calc);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_resp_ready, w_cmd_resp_ready);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_clear_cmderr, w_clear_cmderr);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_cmdstate, wb_trx_cmdstate);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_cmderr, wb_trx_cmderr);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void sdctrl::proc_comb() {
    SETVAL(comb.vb_cmd_req_arg, cmd_req_arg);

TEXT();
    IF (NZ(wait_cmd_resp));
        SETONE(comb.v_cmd_resp_ready);
        IF (NZ(w_cmd_resp_valid));
            SETZERO(wait_cmd_resp);
            SETVAL(cmd_resp_cmd, wb_cmd_resp_cmd);
            SETVAL(cmd_resp_reg, wb_cmd_resp_reg);

TEXT();
            IF (ANDx(2, &EQ(cmd_req_cmd, sdctrl_cfg_->CMD8),
                        &EQ(wb_trx_cmderr, sdctrl_cfg_->CMDERR_NO_RESPONSE)));
                SETVAL(sdtype, sdctrl_cfg_->SDCARD_VER1X);
                SETZERO(HCS, "Standard Capacity only");
                SETVAL(idlestate, IDLESTATE_CMD55);
                SETONE(comb.v_clear_cmderr);
            ELSIF (NE(wb_trx_cmderr, sdctrl_cfg_->CMDERR_NONE));
                IF (EQ(cmd_req_cmd, sdctrl_cfg_->CMD0));
                    TEXT("Re-send CMD0");
                    SETVAL(idlestate, IDLESTATE_CMD0);
                    SETONE(comb.v_clear_cmderr);
                ELSE();
                    SETVAL(sdstate, SDSTATE_INA);
                    SETVAL(sdtype, sdctrl_cfg_->SDCARD_UNUSABLE);
                ENDIF();
            ELSE();
                TEXT("Parse Rx response:");
                SWITCH(cmd_req_rn);
                CASE (sdctrl_cfg_->R1);
                    ENDCASE();
                CASE (sdctrl_cfg_->R3);
                    TEXT("Table 5-1: OCR Register definition, page 246");
                    TEXT("    [23:0]  Voltage window can be requested by CMD58");
                    TEXT("    [24]    Switching to 1.8V accepted (S18A)");
                    TEXT("    [27]    Over 2TB support status (CO2T)");
                    TEXT("    [29]    UHS-II Card status");
                    TEXT("    [30]    Card Capacity Status (CCS)");
                    TEXT("    [31]    Card power-up status (busy is LOW if the card not finished the power-up routine)");
                    IF (NZ(BIT(wb_cmd_resp_reg, 31)));
                        SETVAL(OCR_VoltageWindow, BITS(wb_cmd_resp_reg, 23, 0));
                        SETVAL(HCS, BIT(wb_cmd_resp_reg, 30));
                        SETVAL(S18, BIT(wb_cmd_resp_reg, 24));
                    ENDIF();
                    ENDCASE();
                CASE (sdctrl_cfg_->R6);
                    SETVAL(RCA, CC2(BITS(wb_cmd_resp_reg, 31, 16), CONST("0", 16)));
                    ENDCASE();
                CASEDEF();
                    ENDCASE();
                ENDSWITCH();
            ENDIF();
        ENDIF();
    ELSIF(NZ(cmd_req_valid));
        TEXT("Do nothing wait to accept");
    ELSE();
        TEXT("SD-card global state:");
        SWITCH (sdstate);
        CASE (SDSTATE_PRE_INIT);
            TEXT("Page 222, Fig.4-96 State Diagram (Pre-Init mode)");
            TEXT("1. No commands were sent to the card after POW (except CMD0):");
            TEXT("    CMD line held High for at least 1 ms (by SW), then SDCLK supplied");
            TEXT("    at least 74 clocks with keeping CMD line High");
            TEXT("2. CMD High to Low transition && CMD=Low < 74 clocks then go idle,");
            TEXT("    if Low >= 74 clocks then Fast boot in CV-mode");
            IF (NZ(w_regs_sck_posedge));
                SETVAL(clkcnt, INC(clkcnt));
            ENDIF();
            IF (GE(clkcnt, CONST("73", 7)));
                SETVAL(sdstate, SDSTATE_IDLE);
            ENDIF();
            IF (LE(clkcnt, CONST("63", 7)));
                //SETONE(cmd_set_low);
            ELSE();
                SETZERO(cmd_set_low);
            ENDIF();
            ENDCASE();
        CASE (SDSTATE_IDLE);
            SWITCH (idlestate);
            CASE (IDLESTATE_CMD0);
                SETVAL(sdtype, sdctrl_cfg_->SDCARD_UNKNOWN);
                SETONE(HCS);
                SETZERO(S18);
                SETZERO(RCA);
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
                SETONE(cmd_req_valid);
                SETVAL(cmd_req_cmd, sdctrl_cfg_->CMD8);
                SETVAL(cmd_req_rn, sdctrl_cfg_->R7);
                SETZERO(comb.vb_cmd_req_arg);
                SETBIT(comb.vb_cmd_req_arg, 13, w_regs_pcie_12V_support);
                SETBIT(comb.vb_cmd_req_arg, 12, w_regs_pcie_available);
                SETBITS(comb.vb_cmd_req_arg, 11, 8, wb_regs_voltage_supply);
                SETBITS(comb.vb_cmd_req_arg, 7, 0, wb_regs_check_pattern);
                SETVAL(idlestate, IDLESTATE_CMD55);
                ENDCASE();
            CASE (IDLESTATE_CMD55);
                TEXT("Page 64: APP_CMD (CMD55) shall always precede ACMD41.");
                TEXT("  [31:16] RCA (Relative Adrress should be set 0)");
                TEXT("  [15:0] stuff bits");
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
                SETVAL(cmd_req_rn, sdctrl_cfg_->R3);
                SETZERO(comb.vb_cmd_req_arg);
                SETBIT(comb.vb_cmd_req_arg, 30, HCS);
                SETBIT(comb.vb_cmd_req_arg, 24, S18);
                SETBITS(comb.vb_cmd_req_arg, 23, 0, OCR_VoltageWindow);
                SETVAL(idlestate, IDLESTATE_CARD_IDENTIFICATION);
                ENDCASE();
            CASE (IDLESTATE_CARD_IDENTIFICATION);
                IF (EZ(BIT(cmd_resp_reg, 31)));
                    TEXT("LOW if the card has not finished power-up routine");
                    SETVAL(idlestate, IDLESTATE_CMD55);
                ELSE();
                    IF (NZ(HCS));
                        SETVAL(sdtype, sdctrl_cfg_->SDCARD_VER2X_HC);
                    ELSIF (EQ(sdtype, sdctrl_cfg_->SDCARD_UNKNOWN));
                        SETVAL(sdtype, sdctrl_cfg_->SDCARD_VER2X_SC);
                    ENDIF();
                    IF (NZ(S18));
                        TEXT("Voltage switch command to change 3.3V to 1.8V");
                        SETVAL(readystate, READYSTATE_CMD11);
                    ELSE();
                        SETVAL(readystate, READYSTATE_CMD2);
                    ENDIF();
                    SETVAL(sdstate, SDSTATE_READY);
                ENDIF();
                ENDCASE();
            CASEDEF();
                SETVAL(idlestate, IDLESTATE_CMD0);
                ENDCASE();
            ENDSWITCH();
            ENDCASE();

        CASE (SDSTATE_READY);
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
            ENDCASE();

        CASE (SDSTATE_IDENT);
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
                ENDCASE();
            CASEDEF();
                ENDCASE();
            ENDSWITCH();
            ENDCASE();

        CASEDEF();
            ENDCASE();
        ENDSWITCH();
    ENDIF();
    SETVAL(cmd_req_arg, comb.vb_cmd_req_arg);

TEXT();
    IF (AND2(NZ(cmd_req_valid), NZ(w_cmd_req_ready)));
        SETZERO(cmd_req_valid);
        SETONE(wait_cmd_resp);
    ENDIF();

TEXT();
    SYNC_RESET(*this);


TEXT();
    SETVAL(w_cmd_resp_ready, comb.v_cmd_resp_ready);
    SETVAL(w_crc16_next, comb.v_crc16_next);
    TEXT("Page 222, Table 4-81 Overview of Card States vs Operation Modes table");
    IF (ORx(3, &LE(sdstate, SDSTATE_IDENT),
               &EQ(sdstate, SDSTATE_INA),
               &EQ(sdstate, SDSTATE_PRE_INIT)));
        SETONE(w_400kHz_ena);
    ELSE();
        TEXT("data transfer mode:");
        TEXT("Stand-By, Transfer, Sending, Receive, Programming, Disconnect states");
        SETZERO(w_400kHz_ena);
    ENDIF();

TEXT();
    SETVAL(o_cd_dat3, BIT(dat, 3));
    SETVAL(o_dat2, BIT(dat, 2));
    SETVAL(o_dat1, BIT(dat, 1));
    SETVAL(o_dat0, BIT(dat, 0));

    TEXT("Direction bits:");
    SETVAL(o_dat0_dir, dat_dir);
    SETVAL(o_dat1_dir, dat_dir);
    SETVAL(o_dat2_dir, dat_dir);
    SETVAL(o_cd_dat3_dir, dat_dir);
    TEXT("Memory request:");
    SETONE(w_mem_req_ready);
    SETONE(w_mem_resp_valid);
    SETVAL(wb_mem_resp_rdata, ALLONES());
    SETZERO(wb_mem_resp_err);
    SETVAL(w_clear_cmderr, OR2(w_regs_clear_cmderr, comb.v_clear_cmderr));
}
