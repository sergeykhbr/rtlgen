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
    CFG_SDCARD_POWERUP_DONE_DELAY(this, "CFG_SDCARD_POWERUP_DONE_DELAY", "700", "Delay of busy bits in ACMD41 response"),
    CFG_SDCARD_VHS(this, "4", "CFG_SDCARD_VHS", "0x1", "CMD8 Voltage supply mask"),
    CFG_SDCARD_PCIE_1_2V(this, "1", "CFG_SDCARD_PCIE_1_2V", "0"),
    CFG_SDCARD_PCIE_AVAIL(this, "1", "CFG_SDCARD_PCIE_AVAIL", "0"),
    CFG_SDCARD_VDD_VOLTAGE_WINDOW(this, "24", "CFG_SDCARD_VDD_VOLTAGE_WINDOW", "0xff8000"),
    // signals
    w_clk(this, "w_clk", "1"),
    wb_rdata(this, "wb_rdata", "8"),
    w_cmd_in(this, "w_cmd_in", "1"),
    w_cmd_out(this, "w_cmd_out", "1"),
    w_cmd_dir(this, "w_cmd_dir", "1"),
    w_cmd_req_valid(this, "w_cmd_req_valid", "1"),
    wb_cmd_req_cmd(this, "wb_cmd_req_cmd", "6"),
    wb_cmd_req_data(this, "wb_cmd_req_data", "32"),
    w_cmd_req_ready(this, "w_cmd_req_ready", "1"),
    w_cmd_resp_valid(this, "w_cmd_resp_valid", "1"),
    wb_cmd_resp_data32(this, "wb_cmd_resp_data32", "32"),
    w_cmd_resp_ready(this, "w_cmd_resp_ready", "1"),
    // registers
    //
    comb(this),
    iobufcmd0(this, "iobufcmd0"),
    cmdio0(this, "cmdio0"),
    ctrl0(this, "ctrl0")
{
    Operation::start(this);
    NEW(iobufcmd0, iobufcmd0.getName().c_str());
        CONNECT(iobufcmd0, 0, iobufcmd0.io, io_cmd);
        CONNECT(iobufcmd0, 0, iobufcmd0.o, w_cmd_in);
        CONNECT(iobufcmd0, 0, iobufcmd0.i, w_cmd_out);
        CONNECT(iobufcmd0, 0, iobufcmd0.t, w_cmd_dir);
    ENDNEW();

    NEW(cmdio0, cmdio0.getName().c_str());
        CONNECT(cmdio0, 0, cmdio0.i_clk, i_sclk);
        CONNECT(cmdio0, 0, cmdio0.i_nrst, i_nrst);
        CONNECT(cmdio0, 0, cmdio0.i_cmd, w_cmd_in);
        CONNECT(cmdio0, 0, cmdio0.o_cmd, w_cmd_out);
        CONNECT(cmdio0, 0, cmdio0.o_cmd_dir, w_cmd_dir);
        CONNECT(cmdio0, 0, cmdio0.o_cmd_req_valid, w_cmd_req_valid);
        CONNECT(cmdio0, 0, cmdio0.o_cmd_req_cmd, wb_cmd_req_cmd);
        CONNECT(cmdio0, 0, cmdio0.o_cmd_req_data, wb_cmd_req_data);
        CONNECT(cmdio0, 0, cmdio0.i_cmd_req_ready, w_cmd_req_ready);
        CONNECT(cmdio0, 0, cmdio0.i_cmd_resp_valid, w_cmd_resp_valid);
        CONNECT(cmdio0, 0, cmdio0.i_cmd_resp_data32, wb_cmd_resp_data32);
        CONNECT(cmdio0, 0, cmdio0.o_cmd_resp_ready, w_cmd_resp_ready);
    ENDNEW();

    NEW(ctrl0, ctrl0.getName().c_str());
        CONNECT(ctrl0, 0, ctrl0.i_clk, i_sclk);
        CONNECT(ctrl0, 0, ctrl0.i_nrst, i_nrst);
        CONNECT(ctrl0, 0, ctrl0.i_cmd_req_valid, w_cmd_req_valid);
        CONNECT(ctrl0, 0, ctrl0.i_cmd_req_cmd, wb_cmd_req_cmd);
        CONNECT(ctrl0, 0, ctrl0.i_cmd_req_data, wb_cmd_req_data);
        CONNECT(ctrl0, 0, ctrl0.o_cmd_req_ready, w_cmd_req_ready);
        CONNECT(ctrl0, 0, ctrl0.o_cmd_resp_valid, w_cmd_resp_valid);
        CONNECT(ctrl0, 0, ctrl0.o_cmd_resp_data32, wb_cmd_resp_data32);
        CONNECT(ctrl0, 0, ctrl0.i_cmd_resp_ready, w_cmd_resp_ready);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void vip_sdcard_top::proc_comb() {
}
