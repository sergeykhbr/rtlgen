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
    _sdstate0_(this, "SD-card global state:"),
    SDSTATE_RESET(this, "2", "SDSTATE_RESET", "0"),
    _initstate0_(this, "SD-card initalization state:"),
    INITSTATE_CMD0(this, "4", "INITSTATE_CMD0", "0"),
    INITSTATE_CMD0_RESP(this, "4", "INITSTATE_CMD0_RESP", "1"),
    INITSTATE_CMD8(this, "4", "INITSTATE_CMD8", "2"),
    INITSTATE_CMD41(this, "4", "INITSTATE_CMD41", "3"),
    INITSTATE_CMD11(this, "4", "INITSTATE_CMD11", "4"),
    INITSTATE_CMD2(this, "4", "INITSTATE_CMD2", "5"),
    INITSTATE_CMD3(this, "4", "INITSTATE_CMD3", "6"),
    INITSTATE_ERROR(this, "4", "INITSTATE_ERROR", "7"),
    INITSTATE_DONE(this, "4", "INITSTATE_DONE", "8"),
    // signals
    w_regs_sck_posedge(this, "w_regs_sck_posedge", "1"),
    w_regs_sck_negedge(this, "w_regs_sck", "1"),
    w_regs_clear_cmderr(this, "w_regs_clear_cmderr", "1"),
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
    wb_cmd_resp_r1(this, "wb_cmd_resp_r1", "8"),
    wb_cmd_resp_r3r6(this, "wb_cmd_resp_r3r6", "32"),
    w_cmd_resp_ready(this, "w_cmd_resp_ready", "1"),
    wb_cmdstate(this, "wb_cmdstate", "4"),
    wb_cmderr(this, "wb_cmderr", "4"),
    w_crc7_clear(this, "w_crc7_clear", "1"),
    w_crc7_next(this, "w_crc7_next", "1"),
    w_crc7_dat(this, "w_crc7_dat", "1"),
    wb_crc7(this, "wb_crc7", "7"),
    w_crc16_next(this, "w_crc16_next", "1"),
    wb_crc16_dat(this, "wb_crc16_dat", "4"),
    wb_crc16(this, "wb_crc16", "16"),
    // registers
    cmd_req_ena(this, "cmd_req_ena", "1"),
    cmd_req_type(this, "cmd_req_type", "6"),
    cmd_req_arg(this, "cmd_req_arg", "32"),
    cmd_req_resp(this, "cmd_req_resp", "3"),
    cmd_resp_r1(this, "cmd_resp_r1", "8"),
    cmd_resp_r3r6(this, "cmd_resp_r3r6", "32"),
    crc16_clear(this, "crc16_clear", "1", "1"),
    dat(this, "dat", "4", "-1"),
    dat_dir(this, "dat_dir", "1", "DIR_INPUT"),
    sdstate(this, "sdstate", "2", "SDSTATE_RESET"),
    initstate(this, "initstate", "4", "INITSTATE_CMD0"),
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
        CONNECT(cmdtrx0, 0, cmdtrx0.i_req_valid, cmd_req_ena);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_req_type, cmd_req_type);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_req_arg, cmd_req_arg);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_req_resp, cmd_req_resp);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_req_ready, w_cmd_req_ready);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_crc7, wb_crc7);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_crc7_clear, w_crc7_clear);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_crc7_next, w_crc7_next);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_crc7_dat, w_crc7_dat);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_resp_valid, w_cmd_resp_valid);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_resp_r1, wb_cmd_resp_r1);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_resp_r3r6, wb_cmd_resp_r3r6);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_resp_ready, w_cmd_resp_ready);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_clear_cmderr, w_regs_clear_cmderr);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_cmdstate, wb_cmdstate);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_cmderr, wb_cmderr);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void sdctrl::proc_comb() {
TEXT();
    TEXT("SD-card global state:");
    SWITCH (sdstate);
    CASE (SDSTATE_RESET);
        SWITCH (initstate);
        CASE (INITSTATE_CMD0);
            SETONE(cmd_req_ena);
            SETVAL(cmd_req_type, sdctrl_cfg_->CMD0);
            SETVAL(cmd_req_arg, CONST("0", 32));
            SETVAL(cmd_req_resp, sdctrl_cfg_->R1);
            SETVAL(initstate, INC(initstate));
            ENDCASE();
        CASE (INITSTATE_CMD0_RESP);
            IF (NZ(w_cmd_resp_valid));
                SETVAL(cmd_resp_r1, wb_cmd_resp_r1);
                SETVAL(cmd_resp_r3r6, wb_cmd_resp_r3r6);
                SETVAL(initstate, INC(initstate));
            ENDIF();
            ENDCASE();
        CASE (INITSTATE_CMD8);
            ENDCASE();
        CASE (INITSTATE_CMD41);
            ENDCASE();
        CASE (INITSTATE_CMD11);
            ENDCASE();
        CASE (INITSTATE_CMD2);
            ENDCASE();
        CASE (INITSTATE_CMD3);
            ENDCASE();
        CASE (INITSTATE_ERROR);
            ENDCASE();
        CASE (INITSTATE_DONE);
            ENDCASE();
        CASEDEF();
            ENDCASE();
        ENDSWITCH();
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    IF (AND2(NZ(cmd_req_ena), NZ(w_cmd_req_ready)));
        SETZERO(cmd_req_ena);
    ENDIF();

TEXT();
    SYNC_RESET(*this);


TEXT();
    SETONE(w_cmd_resp_ready);
    SETVAL(w_crc16_next, comb.v_crc16_next);

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

}
