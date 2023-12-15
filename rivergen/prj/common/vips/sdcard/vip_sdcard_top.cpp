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

vip_sdcard_top::vip_sdcard_top(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "vip_sdcard_top", name, comment),
    i_nrst(this, "i_nrst", "1", "To avoid undefined states of registers (xxx)"),
    i_sclk(this, "i_sclk", "1"),
    io_cmd(this, "io_cmd", "1"),
    io_dat0(this, "io_dat0", "1"),
    io_dat1(this, "io_dat1", "1"),
    io_dat2(this, "io_dat2", "1"),
    io_cd_dat3(this, "io_cd_dat3", "1"),
    // params
    _cfg0_(this, "Generic config parameters"),
    CFG_SDCARD_POWERUP_DONE_DELAY(this, "CFG_SDCARD_POWERUP_DONE_DELAY", "450", "Delay of busy bits in ACMD41 response"),
    CFG_SDCARD_HCS(this, "CFG_SDCARD_HCS", "1", "1", "High Capacity Support"),
    CFG_SDCARD_VHS(this, "CFG_SDCARD_VHS", "4", "0x1", "CMD8 Voltage supply mask"),
    CFG_SDCARD_PCIE_1_2V(this, "CFG_SDCARD_PCIE_1_2V", "1", "0", NO_COMMENT),
    CFG_SDCARD_PCIE_AVAIL(this, "CFG_SDCARD_PCIE_AVAIL", "1", "0", NO_COMMENT),
    CFG_SDCARD_VDD_VOLTAGE_WINDOW(this, "CFG_SDCARD_VDD_VOLTAGE_WINDOW", "24", "0xff8000", NO_COMMENT),
    // signals
    w_clk(this, "w_clk", "1"),
    wb_rdata(this, "wb_rdata", "8"),
    w_spi_mode(this, "w_spi_mode", "1"),
    w_cmd_in(this, "w_cmd_in", "1"),
    w_cmd_out(this, "w_cmd_out", "1"),
    w_cmd_dir(this, "w_cmd_dir", "1"),
    w_dat0_in(this, "w_dat0_in", "1"),
    w_dat1_in(this, "w_dat1_in", "1"),
    w_dat2_in(this, "w_dat2_in", "1"),
    w_dat3_in(this, "w_dat3_in", "1"),
    w_dat0_out(this, "w_dat0_out", "1"),
    w_dat1_out(this, "w_dat1_out", "1"),
    w_dat2_out(this, "w_dat2_out", "1"),
    w_dat3_out(this, "w_dat3_out", "1"),
    w_dat0_dir(this, "w_dat0_dir", "1"),
    w_dat1_dir(this, "w_dat1_dir", "1"),
    w_dat2_dir(this, "w_dat2_dir", "1"),
    w_dat3_dir(this, "w_dat3_dir", "1"),
    w_cmd_req_valid(this, "w_cmd_req_valid", "1"),
    wb_cmd_req_cmd(this, "wb_cmd_req_cmd", "6"),
    wb_cmd_req_data(this, "wb_cmd_req_data", "32"),
    w_cmd_req_ready(this, "w_cmd_req_ready", "1"),
    w_cmd_resp_valid(this, "w_cmd_resp_valid", "1"),
    wb_cmd_resp_data32(this, "wb_cmd_resp_data32", "32"),
    w_cmd_resp_ready(this, "w_cmd_resp_ready", "1"),
    w_cmd_resp_r1b(this, "w_cmd_resp_r1b", "1"),
    w_cmd_resp_r2(this, "w_cmd_resp_r2", "1"),
    w_cmd_resp_r3(this, "w_cmd_resp_r3", "1"),
    w_cmd_resp_r7(this, "w_cmd_resp_r7", "1"),
    w_cmdio_cmd_dir(this, "w_cmdio_cmd_dir", "1"),
    w_cmdio_cmd_out(this, "w_cmdio_cmd_out", "1"),
    _status0_(this, "Status signals:"),
    w_stat_idle_state(this, "w_stat_idle_state", "1"),
    w_stat_erase_reset(this, "w_stat_erase_reset", "1"),
    w_stat_illegal_cmd(this, "w_stat_illegal_cmd", "1"),
    w_stat_err_erase_sequence(this, "w_stat_err_erase_sequence", "1"),
    w_stat_err_address(this, "w_stat_err_address", "1"),
    w_stat_err_parameter(this, "w_stat_err_parameter", "1"),
    w_stat_locked(this, "w_stat_locked", "1"),
    w_stat_wp_erase_skip(this, "w_stat_wp_erase_skip", "1"),
    w_stat_err(this, "w_stat_err", "1"),
    w_stat_err_cc(this, "w_stat_err_cc", "1"),
    w_stat_ecc_failed(this, "w_stat_ecc_failed", "1"),
    w_stat_wp_violation(this, "w_stat_wp_violation", "1"),
    w_stat_erase_param(this, "w_stat_erase_param", "1"),
    w_stat_out_of_range(this, "w_stat_out_of_range", "1"),
    wb_mem_addr(this, "wb_mem_addr", "41"),
    wb_mem_rdata(this, "wb_mem_rdata", "8"),
    w_crc16_clear(this, "w_crc15_clear", "1"),
    w_crc16_next(this, "w_crc15_next", "1"),
    wb_crc16(this, "wb_crc16", "16"),
    w_dat_trans(this, "w_dat_trans", "1"),
    wb_dat(this, "wb_dat", "4"),
    w_cmdio_busy(this, "w_cmdio_busy", "1"),
    // registers
    //
    comb(this),
    iobufcmd0(this, "iobufcmd0"),
    iobufdat0(this, "iobufdat0"),
    iobufdat1(this, "iobufdat1"),
    iobufdat2(this, "iobufdat2"),
    iobufdat3(this, "iobufdat3"),
    cmdio0(this, "cmdio0", NO_COMMENT),
    ctrl0(this, "ctrl0", NO_COMMENT)
{
    Operation::start(this);
    NEW(iobufcmd0, iobufcmd0.getName().c_str());
        CONNECT(iobufcmd0, 0, iobufcmd0.io, io_cmd);
        CONNECT(iobufcmd0, 0, iobufcmd0.o, w_cmd_in);
        CONNECT(iobufcmd0, 0, iobufcmd0.i, w_cmd_out);
        CONNECT(iobufcmd0, 0, iobufcmd0.t, w_cmd_dir);
    ENDNEW();

TEXT();
    NEW(iobufdat0, iobufdat0.getName().c_str());
        CONNECT(iobufdat0, 0, iobufdat0.io, io_dat0);
        CONNECT(iobufdat0, 0, iobufdat0.o, w_dat0_in);
        CONNECT(iobufdat0, 0, iobufdat0.i, w_dat0_out);
        CONNECT(iobufdat0, 0, iobufdat0.t, w_dat0_dir);
    ENDNEW();

TEXT();
    NEW(iobufdat1, iobufdat1.getName().c_str());
        CONNECT(iobufdat1, 0, iobufdat1.io, io_dat1);
        CONNECT(iobufdat1, 0, iobufdat1.o, w_dat1_in);
        CONNECT(iobufdat1, 0, iobufdat1.i, w_dat1_out);
        CONNECT(iobufdat1, 0, iobufdat1.t, w_dat1_dir);
    ENDNEW();

TEXT();
    NEW(iobufdat2, iobufdat2.getName().c_str());
        CONNECT(iobufdat2, 0, iobufdat2.io, io_dat2);
        CONNECT(iobufdat2, 0, iobufdat2.o, w_dat2_in);
        CONNECT(iobufdat2, 0, iobufdat2.i, w_dat2_out);
        CONNECT(iobufdat2, 0, iobufdat2.t, w_dat2_dir);
    ENDNEW();

TEXT();
    NEW(iobufdat3, iobufdat3.getName().c_str());
        CONNECT(iobufdat3, 0, iobufdat3.io, io_cd_dat3);
        CONNECT(iobufdat3, 0, iobufdat3.o, w_dat3_in);
        CONNECT(iobufdat3, 0, iobufdat3.i, w_dat3_out);
        CONNECT(iobufdat3, 0, iobufdat3.t, w_dat3_dir);
    ENDNEW();

TEXT();
    NEW(cmdio0, cmdio0.getName().c_str());
        CONNECT(cmdio0, 0, cmdio0.i_clk, i_sclk);
        CONNECT(cmdio0, 0, cmdio0.i_nrst, i_nrst);
        CONNECT(cmdio0, 0, cmdio0.i_cs, w_dat3_in);
        CONNECT(cmdio0, 0, cmdio0.o_spi_mode, w_spi_mode);
        CONNECT(cmdio0, 0, cmdio0.i_cmd, w_cmd_in);
        CONNECT(cmdio0, 0, cmdio0.o_cmd, w_cmdio_cmd_out);
        CONNECT(cmdio0, 0, cmdio0.o_cmd_dir, w_cmdio_cmd_dir);
        CONNECT(cmdio0, 0, cmdio0.o_cmd_req_valid, w_cmd_req_valid);
        CONNECT(cmdio0, 0, cmdio0.o_cmd_req_cmd, wb_cmd_req_cmd);
        CONNECT(cmdio0, 0, cmdio0.o_cmd_req_data, wb_cmd_req_data);
        CONNECT(cmdio0, 0, cmdio0.i_cmd_req_ready, w_cmd_req_ready);
        CONNECT(cmdio0, 0, cmdio0.i_cmd_resp_valid, w_cmd_resp_valid);
        CONNECT(cmdio0, 0, cmdio0.i_cmd_resp_data32, wb_cmd_resp_data32);
        CONNECT(cmdio0, 0, cmdio0.o_cmd_resp_ready, w_cmd_resp_ready);
        CONNECT(cmdio0, 0, cmdio0.i_cmd_resp_r1b, w_cmd_resp_r1b);
        CONNECT(cmdio0, 0, cmdio0.i_cmd_resp_r2, w_cmd_resp_r2);
        CONNECT(cmdio0, 0, cmdio0.i_cmd_resp_r3, w_cmd_resp_r3);
        CONNECT(cmdio0, 0, cmdio0.i_cmd_resp_r7, w_cmd_resp_r7);
        CONNECT(cmdio0, 0, cmdio0.i_stat_idle_state, w_stat_idle_state);
        CONNECT(cmdio0, 0, cmdio0.i_stat_erase_reset, w_stat_erase_reset);
        CONNECT(cmdio0, 0, cmdio0.i_stat_illegal_cmd, w_stat_illegal_cmd);
        CONNECT(cmdio0, 0, cmdio0.i_stat_err_erase_sequence, w_stat_err_erase_sequence);
        CONNECT(cmdio0, 0, cmdio0.i_stat_err_address, w_stat_err_address);
        CONNECT(cmdio0, 0, cmdio0.i_stat_err_parameter, w_stat_err_parameter);
        CONNECT(cmdio0, 0, cmdio0.i_stat_locked, w_stat_locked);
        CONNECT(cmdio0, 0, cmdio0.i_stat_wp_erase_skip, w_stat_wp_erase_skip);
        CONNECT(cmdio0, 0, cmdio0.i_stat_err, w_stat_err);
        CONNECT(cmdio0, 0, cmdio0.i_stat_err_cc, w_stat_err_cc);
        CONNECT(cmdio0, 0, cmdio0.i_stat_ecc_failed, w_stat_ecc_failed);
        CONNECT(cmdio0, 0, cmdio0.i_stat_wp_violation, w_stat_wp_violation);
        CONNECT(cmdio0, 0, cmdio0.i_stat_erase_param, w_stat_erase_param);
        CONNECT(cmdio0, 0, cmdio0.i_stat_out_of_range, w_stat_out_of_range);
        CONNECT(cmdio0, 0, cmdio0.o_busy, w_cmdio_busy);
    ENDNEW();

TEXT();
    ctrl0.CFG_SDCARD_POWERUP_DONE_DELAY.setObjValue(&CFG_SDCARD_POWERUP_DONE_DELAY);
    ctrl0.CFG_SDCARD_HCS.setObjValue(&CFG_SDCARD_HCS);
    ctrl0.CFG_SDCARD_VHS.setObjValue(&CFG_SDCARD_VHS);
    ctrl0.CFG_SDCARD_PCIE_1_2V.setObjValue(&CFG_SDCARD_PCIE_1_2V);
    ctrl0.CFG_SDCARD_PCIE_AVAIL.setObjValue(&CFG_SDCARD_PCIE_AVAIL);
    ctrl0.CFG_SDCARD_VDD_VOLTAGE_WINDOW.setObjValue(&CFG_SDCARD_VDD_VOLTAGE_WINDOW);
    NEW(ctrl0, ctrl0.getName().c_str());
        CONNECT(ctrl0, 0, ctrl0.i_clk, i_sclk);
        CONNECT(ctrl0, 0, ctrl0.i_nrst, i_nrst);
        CONNECT(ctrl0, 0, ctrl0.i_spi_mode, w_spi_mode);
        CONNECT(ctrl0, 0, ctrl0.i_cs, w_dat3_in);
        CONNECT(ctrl0, 0, ctrl0.i_cmd_req_valid, w_cmd_req_valid);
        CONNECT(ctrl0, 0, ctrl0.i_cmd_req_cmd, wb_cmd_req_cmd);
        CONNECT(ctrl0, 0, ctrl0.i_cmd_req_data, wb_cmd_req_data);
        CONNECT(ctrl0, 0, ctrl0.o_cmd_req_ready, w_cmd_req_ready);
        CONNECT(ctrl0, 0, ctrl0.o_cmd_resp_valid, w_cmd_resp_valid);
        CONNECT(ctrl0, 0, ctrl0.o_cmd_resp_data32, wb_cmd_resp_data32);
        CONNECT(ctrl0, 0, ctrl0.i_cmd_resp_ready, w_cmd_resp_ready);
        CONNECT(ctrl0, 0, ctrl0.o_cmd_resp_r1b, w_cmd_resp_r1b);
        CONNECT(ctrl0, 0, ctrl0.o_cmd_resp_r2, w_cmd_resp_r2);
        CONNECT(ctrl0, 0, ctrl0.o_cmd_resp_r3, w_cmd_resp_r3);
        CONNECT(ctrl0, 0, ctrl0.o_cmd_resp_r7, w_cmd_resp_r7);
        CONNECT(ctrl0, 0, ctrl0.o_stat_idle_state, w_stat_idle_state);
        CONNECT(ctrl0, 0, ctrl0.o_stat_illegal_cmd, w_stat_illegal_cmd);
        CONNECT(ctrl0, 0, ctrl0.o_mem_addr, wb_mem_addr);
        CONNECT(ctrl0, 0, ctrl0.i_mem_rdata, wb_mem_rdata);
        CONNECT(ctrl0, 0, ctrl0.o_crc16_clear, w_crc16_clear);
        CONNECT(ctrl0, 0, ctrl0.o_crc16_next, w_crc16_next);
        CONNECT(ctrl0, 0, ctrl0.i_crc16, wb_crc16);
        CONNECT(ctrl0, 0, ctrl0.o_dat_trans, w_dat_trans);
        CONNECT(ctrl0, 0, ctrl0.o_dat, wb_dat);
        CONNECT(ctrl0, 0, ctrl0.i_cmdio_busy, w_cmdio_busy);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void vip_sdcard_top::proc_comb() {
    IF (NZ(w_spi_mode));
        SETONE(w_cmd_dir, "in: din");
        SETZERO(w_dat0_dir, "out: dout");
        SETONE(w_dat1_dir, "in: reserved");
        SETONE(w_dat2_dir, "in: reserved");
        SETONE(w_dat3_dir, "in: cs");

        TEXT();
        SETVAL(w_dat0_out, ORx_L(2, &AND2_L(INV(w_dat_trans), w_cmdio_cmd_out),
                                    &AND2_L(w_dat_trans, BIT(wb_dat, 3))));
        SETONE(w_dat1_out);
        SETONE(w_dat2_out);
        SETONE(w_dat3_out);
    ELSE();
        SETVAL(w_cmd_dir, w_cmdio_cmd_dir);
        SETONE(w_dat0_dir, "in:");
        SETONE(w_dat1_dir, "in:");
        SETONE(w_dat2_dir, "in:");
        SETONE(w_dat3_dir, "in:");
        
        TEXT();
        SETVAL(w_cmd_out, w_cmdio_cmd_out);
        SETVAL(w_dat0_out, BIT(wb_dat, 0));
        SETVAL(w_dat1_out, BIT(wb_dat, 1));
        SETVAL(w_dat2_out, BIT(wb_dat, 2));
        SETVAL(w_dat3_out, BIT(wb_dat, 3));
    ENDIF();

TEXT();
    TEXT("Not implemented yet:");
    SETZERO(w_stat_erase_reset);
    SETZERO(w_stat_err_erase_sequence);
    SETZERO(w_stat_err_address);
    SETZERO(w_stat_err_parameter);
    SETZERO(w_stat_locked);
    SETZERO(w_stat_wp_erase_skip);
    SETZERO(w_stat_err);
    SETZERO(w_stat_err_cc);
    SETZERO(w_stat_ecc_failed);
    SETZERO(w_stat_wp_violation);
    SETZERO(w_stat_erase_param);
    SETZERO(w_stat_out_of_range);
    SETVAL(wb_mem_rdata, CONST("0xFF", 8));
    SETVAL(wb_crc16, CONST("0x7fa1", 16));
}
