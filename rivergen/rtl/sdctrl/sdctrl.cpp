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

sdctrl::sdctrl(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "sdctrl", name, comment),
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
    _mode0_(this, "SD controller modes:"),
    MODE_PRE_INIT(this, "2", "MODE_PRE_INIT", "0"),
    MODE_SPI(this, "2", "MODE_SPI", "1"),
    MODE_SD(this, "2", "MODE_SD", "2"),
    // signals
    w_regs_sck_posedge(this, "w_regs_sck_posedge", "1"),
    w_regs_sck_negedge(this, "w_regs_sck", "1"),
    w_regs_err_clear(this, "w_regs_err_clear", "1"),
    wb_regs_watchdog(this, "wb_regs_watchdog", "16"),
    w_regs_spi_mode(this, "w_regs_spi_mode", "1"),
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
    w_cache_req_ready(this, "w_cache_req_ready", "1"),
    w_cache_resp_valid(this, "w_cache_resp_valid", "1"),
    wb_cache_resp_rdata(this, "wb_cache_resp_rdata", "64"),
    w_cache_resp_err(this, "w_cache_resp_err", "1"),
    w_cache_resp_ready(this, "w_cache_resp_ready", "1"),
    w_req_sdmem_valid(this, "w_req_sdmem_valid", "1"),
    w_req_sdmem_write(this, "w_req_sdmem_write", "1"),
    wb_req_sdmem_addr(this, "wb_req_sdmem_addr", "CFG_SDCACHE_ADDR_BITS"),
    wb_req_sdmem_wdata(this, "wb_req_sdmem_wdata", "SDCACHE_LINE_BITS"),
    w_regs_flush_valid(this, "w_regs_flush_valid", "1"),
    w_cache_flush_end(this, "w_cache_flush_end", "1"),
    w_trx_cmd(this, "w_trx_cmd", "1"),
    w_trx_cmd_dir(this, "w_trx_cmd_dir", "1"),
    w_trx_cmd_csn(this, "w_trx_cmd_csn", "1"),
    w_trx_wdog_ena(this, "w_trx_wdog_ena", "1"),
    w_trx_err_valid(this, "w_trx_err_valid", "1"),
    wb_trx_err_setcode(this, "wb_trx_err_setcode", "4"),
    w_cmd_in(this, "w_cmd_in", "1"),
    w_cmd_req_ready(this, "w_cmd_req_ready", "1"),
    w_cmd_resp_valid(this, "w_cmd_resp_valid", "1"),
    wb_cmd_resp_cmd(this, "wb_cmd_resp_cmd", "6"),
    wb_cmd_resp_reg(this, "wb_cmd_resp_reg", "32"),
    wb_cmd_resp_crc7_rx(this, "wb_cmd_resp_crc7_rx", "7"),
    wb_cmd_resp_crc7_calc(this, "wb_cmd_resp_crc7_calc", "7"),
    wb_cmd_resp_spistatus(this, "wb_cmd_resp_spistatus", "15"),
    w_cmd_resp_ready(this, "w_cmd_resp_ready", "1"),
    wb_crc16_0(this, "wb_crc16_0", "16"),
    wb_crc16_1(this, "wb_crc16_1", "16"),
    wb_crc16_2(this, "wb_crc16_2", "16"),
    wb_crc16_3(this, "wb_crc16_3", "16"),
    w_wdog_trigger(this, "w_wdog_trigger", "1"),
    wb_err_code(this, "wb_err_code", "4"),
    w_err_pending(this, "w_err_pending", "1"),
    _mux0_(this),
    _mux1_(this, "SPI-mode controller signals:"),
    w_spi_dat(this, "w_spi_dat", "1"),
    w_spi_dat_csn(this, "w_spi_dat_csn", "1"),
    w_spi_cmd_req_valid(this, "w_spi_cmd_req_valid", "1"),
    wb_spi_cmd_req_cmd(this, "wb_spi_cmd_req_cmd", "6"),
    wb_spi_cmd_req_arg(this, "wb_spi_cmd_req_arg", "32"),
    wb_spi_cmd_req_rn(this, "wb_spi_cmd_req_rn", "3"),
    w_spi_req_sdmem_ready(this, "w_spi_req_sdmem_ready", "1"),
    w_spi_resp_sdmem_valid(this, "w_spi_resp_sdmem_valid", "1"),
    wb_spi_resp_sdmem_data(this, "wb_spi_resp_sdmem_data", "512"),
    w_spi_err_valid(this, "w_spi_err_valid", "1"),
    w_spi_err_clear(this, "w_spi_err_clear", "1"),
    wb_spi_err_setcode(this, "wb_spi_err_setcode", "4"),
    w_spi_400kHz_ena(this, "w_spi_400kHz_ena", "1"),
    wb_spi_sdtype(this, "wb_spi_sdtype", "3"),
    w_spi_wdog_ena(this, "w_spi_wdog_ena", "1"),
    w_spi_crc16_clear(this, "w_spi_crc16_clear", "1"),
    w_spi_crc16_next(this, "w_spi_crc16_next", "1"),
    _mux2_(this),
    _mux3_(this, "SD-mode controller signals:"),
    w_sd_dat0(this, "w_sd_dat0", "1"),
    w_sd_dat0_dir(this, "w_sd_dat0_dir", "1"),
    w_sd_dat1(this, "w_sd_dat1", "1"),
    w_sd_dat1_dir(this, "w_sd_dat1_dir", "1"),
    w_sd_dat2(this, "w_sd_dat2", "1"),
    w_sd_dat2_dir(this, "w_sd_dat2_dir", "1"),
    w_sd_dat3(this, "w_sd_dat3", "1"),
    w_sd_dat3_dir(this, "w_sd_dat3_dir", "1"),
    w_sd_cmd_req_valid(this, "w_sd_cmd_req_valid", "1"),
    wb_sd_cmd_req_cmd(this, "wb_sd_cmd_req_cmd", "6"),
    wb_sd_cmd_req_arg(this, "wb_sd_cmd_req_arg", "32"),
    wb_sd_cmd_req_rn(this, "wb_sd_cmd_req_rn", "3"),
    w_sd_req_sdmem_ready(this, "w_sd_req_sdmem_ready", "1"),
    w_sd_resp_sdmem_valid(this, "w_sd_resp_sdmem_valid", "1"),
    wb_sd_resp_sdmem_data(this, "wb_sd_resp_sdmem_data", "512"),
    w_sd_err_valid(this, "w_sd_err_valid", "1"),
    w_sd_err_clear(this, "w_sd_err_clear", "1"),
    wb_sd_err_setcode(this, "wb_sd_err_setcode", "4"),
    w_sd_400kHz_ena(this, "w_sd_400kHz_ena", "1"),
    wb_sd_sdtype(this, "wb_sd_sdtype", "3"),
    w_sd_wdog_ena(this, "w_sd_wdog_ena", "1"),
    w_sd_crc16_clear(this, "w_sd_crc16_clear", "1"),
    w_sd_crc16_next(this, "w_sd_crc16_next", "1"),
    _mux4_(this),
    _mux5_(this, "Mode multiplexed signals:"),
    w_cmd_req_valid(this, "w_cmd_req_valid", "1"),
    wb_cmd_req_cmd(this, "wb_cmd_req_cmd", "6"),
    wb_cmd_req_arg(this, "wb_cmd_req_arg", "32"),
    wb_cmd_req_rn(this, "wb_cmd_req_rn", "3"),
    w_req_sdmem_ready(this, "w_req_sdmem_ready", "1"),
    w_resp_sdmem_valid(this, "w_resp_sdmem_valid", "1"),
    wb_resp_sdmem_data(this, "wb_resp_sdmem_data", "512"),
    w_err_valid(this, "w_err_valid", "1"),
    w_err_clear(this, "w_err_clear", "1"),
    wb_err_setcode(this, "wb_err_setcode", "4"),
    w_400kHz_ena(this, "w_400kHz_ena", "1"),
    wb_sdtype(this, "wb_sdtype", "3"),
    w_wdog_ena(this, "w_wdog_ena", "1"),
    w_crc16_clear(this, "w_crc16_clear", "1"),
    w_crc16_next(this, "w_crc16_next", "1"),
    // registers
    nrst_spimode(this, "nrst_spimode", "1"),
    nrst_sdmode(this, "nrst_sdmode", "1"),
    clkcnt(this, "clkcnt", "7"),
    cmd_set_low(this, "cmd_set_low", "1"),
    mode(this, "mode", "2", "MODE_PRE_INIT"),
    //
    comb(this),
    xslv0(this, "xslv0"),
    regs0(this, "regs0"),
    err0(this, "err0"),
    wdog0(this, "wdog0"),
    crcdat0(this, "crcdat0"),
    crcdat1(this, "crcdat1"),
    crcdat2(this, "crcdat2"),
    crcdat3(this, "crcdat3"),
    spimode0(this, "spimode0"),
    sdmode0(this, "sdmode0"),
    cmdtrx0(this, "cmdtrx0"),
    cache0(this, "cache0")
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
        CONNECT(xslv0, 0, xslv0.i_req_ready, w_cache_req_ready);
        CONNECT(xslv0, 0, xslv0.i_resp_valid, w_cache_resp_valid);
        CONNECT(xslv0, 0, xslv0.i_resp_rdata, wb_cache_resp_rdata);
        CONNECT(xslv0, 0, xslv0.i_resp_err, w_cache_resp_err);
    ENDNEW();

TEXT();
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
        CONNECT(regs0, 0, regs0.o_err_clear, w_regs_err_clear);
        CONNECT(regs0, 0, regs0.o_spi_mode, w_regs_spi_mode);
        CONNECT(regs0, 0, regs0.o_pcie_12V_support, w_regs_pcie_12V_support);
        CONNECT(regs0, 0, regs0.o_pcie_available, w_regs_pcie_available);
        CONNECT(regs0, 0, regs0.o_voltage_supply, wb_regs_voltage_supply);
        CONNECT(regs0, 0, regs0.o_check_pattern, wb_regs_check_pattern);
        CONNECT(regs0, 0, regs0.i_400khz_ena, w_400kHz_ena);
        CONNECT(regs0, 0, regs0.i_sdtype, wb_sdtype);
        CONNECT(regs0, 0, regs0.i_sd_cmd, i_cmd);
        CONNECT(regs0, 0, regs0.i_sd_dat0, i_dat0);
        CONNECT(regs0, 0, regs0.i_sd_dat1, i_dat1);
        CONNECT(regs0, 0, regs0.i_sd_dat2, i_dat2);
        CONNECT(regs0, 0, regs0.i_sd_dat3, i_cd_dat3);
        CONNECT(regs0, 0, regs0.i_err_code, wb_err_code);
        CONNECT(regs0, 0, regs0.i_cmd_req_valid, w_cmd_req_valid);
        CONNECT(regs0, 0, regs0.i_cmd_req_cmd, wb_cmd_req_cmd);
        CONNECT(regs0, 0, regs0.i_cmd_resp_valid, w_cmd_resp_valid);
        CONNECT(regs0, 0, regs0.i_cmd_resp_cmd, wb_cmd_resp_cmd);
        CONNECT(regs0, 0, regs0.i_cmd_resp_reg, wb_cmd_resp_reg);
        CONNECT(regs0, 0, regs0.i_cmd_resp_crc7_rx, wb_cmd_resp_crc7_rx);
        CONNECT(regs0, 0, regs0.i_cmd_resp_crc7_calc, wb_cmd_resp_crc7_calc);
    ENDNEW();

TEXT();
    NEW(wdog0, wdog0.getName().c_str());
        CONNECT(wdog0, 0, wdog0.i_clk, i_clk);
        CONNECT(wdog0, 0, wdog0.i_nrst, i_nrst);
        CONNECT(wdog0, 0, wdog0.i_ena, w_wdog_ena);
        CONNECT(wdog0, 0, wdog0.i_period, wb_regs_watchdog);
        CONNECT(wdog0, 0, wdog0.o_trigger, w_wdog_trigger);
    ENDNEW();

TEXT();
    NEW(err0, err0.getName().c_str());
        CONNECT(err0, 0, err0.i_clk, i_clk);
        CONNECT(err0, 0, err0.i_nrst, i_nrst);
        CONNECT(err0, 0, err0.i_err_valid, w_err_valid);
        CONNECT(err0, 0, err0.i_err_code, wb_err_setcode);
        CONNECT(err0, 0, err0.i_err_clear, w_err_clear);
        CONNECT(err0, 0, err0.o_err_code, wb_err_code);
        CONNECT(err0, 0, err0.o_err_pending, w_err_pending);
    ENDNEW();

TEXT();
    NEW(crcdat0, crcdat0.getName().c_str());
        CONNECT(crcdat0, 0, crcdat0.i_clk, i_clk);
        CONNECT(crcdat0, 0, crcdat0.i_nrst, i_nrst);
        CONNECT(crcdat0, 0, crcdat0.i_clear, w_crc16_clear);
        CONNECT(crcdat0, 0, crcdat0.i_next, w_crc16_next);
        CONNECT(crcdat0, 0, crcdat0.i_dat, i_dat0);
        CONNECT(crcdat0, 0, crcdat0.o_crc16, wb_crc16_0);
    ENDNEW();

TEXT();
    NEW(crcdat1, crcdat1.getName().c_str());
        CONNECT(crcdat1, 0, crcdat1.i_clk, i_clk);
        CONNECT(crcdat1, 0, crcdat1.i_nrst, i_nrst);
        CONNECT(crcdat1, 0, crcdat1.i_clear, w_crc16_clear);
        CONNECT(crcdat1, 0, crcdat1.i_next, w_crc16_next);
        CONNECT(crcdat1, 0, crcdat1.i_dat, i_dat1);
        CONNECT(crcdat1, 0, crcdat1.o_crc16, wb_crc16_1);
    ENDNEW();

TEXT();
    NEW(crcdat2, crcdat2.getName().c_str());
        CONNECT(crcdat2, 0, crcdat2.i_clk, i_clk);
        CONNECT(crcdat2, 0, crcdat2.i_nrst, i_nrst);
        CONNECT(crcdat2, 0, crcdat2.i_clear, w_crc16_clear);
        CONNECT(crcdat2, 0, crcdat2.i_next, w_crc16_next);
        CONNECT(crcdat2, 0, crcdat2.i_dat, i_dat2);
        CONNECT(crcdat2, 0, crcdat2.o_crc16, wb_crc16_2);
    ENDNEW();

TEXT();
    NEW(crcdat3, crcdat3.getName().c_str());
        CONNECT(crcdat3, 0, crcdat3.i_clk, i_clk);
        CONNECT(crcdat3, 0, crcdat3.i_nrst, i_nrst);
        CONNECT(crcdat3, 0, crcdat3.i_clear, w_crc16_clear);
        CONNECT(crcdat3, 0, crcdat3.i_next, w_crc16_next);
        CONNECT(crcdat3, 0, crcdat3.i_dat, i_cd_dat3);
        CONNECT(crcdat3, 0, crcdat3.o_crc16, wb_crc16_3);
    ENDNEW();

TEXT();
    NEW(spimode0, spimode0.getName().c_str());
        CONNECT(spimode0, 0, spimode0.i_clk, i_clk);
        CONNECT(spimode0, 0, spimode0.i_nrst, nrst_spimode);
        CONNECT(spimode0, 0, spimode0.i_posedge, w_regs_sck_posedge);
        CONNECT(spimode0, 0, spimode0.i_miso, i_dat0);
        CONNECT(spimode0, 0, spimode0.o_mosi, w_spi_dat);
        CONNECT(spimode0, 0, spimode0.o_csn, w_spi_dat_csn);
        CONNECT(spimode0, 0, spimode0.i_detected, i_detected);
        CONNECT(spimode0, 0, spimode0.i_protect, i_protect);
        CONNECT(spimode0, 0, spimode0.i_cfg_pcie_12V_support, w_regs_pcie_12V_support);
        CONNECT(spimode0, 0, spimode0.i_cfg_pcie_available, w_regs_pcie_available);
        CONNECT(spimode0, 0, spimode0.i_cfg_voltage_supply, wb_regs_voltage_supply);
        CONNECT(spimode0, 0, spimode0.i_cfg_check_pattern, wb_regs_check_pattern);
        CONNECT(spimode0, 0, spimode0.i_cmd_req_ready, w_cmd_req_ready);
        CONNECT(spimode0, 0, spimode0.o_cmd_req_valid, w_spi_cmd_req_valid);
        CONNECT(spimode0, 0, spimode0.o_cmd_req_cmd, wb_spi_cmd_req_cmd);
        CONNECT(spimode0, 0, spimode0.o_cmd_req_arg, wb_spi_cmd_req_arg);
        CONNECT(spimode0, 0, spimode0.o_cmd_req_rn, wb_spi_cmd_req_rn);
        CONNECT(spimode0, 0, spimode0.i_cmd_resp_valid, w_cmd_resp_valid);
        CONNECT(spimode0, 0, spimode0.i_cmd_resp_r1r2, wb_cmd_resp_spistatus);
        CONNECT(spimode0, 0, spimode0.i_cmd_resp_arg32, wb_cmd_resp_reg);
        CONNECT(spimode0, 0, spimode0.o_data_req_ready, w_spi_req_sdmem_ready);
        CONNECT(spimode0, 0, spimode0.i_data_req_valid, w_req_sdmem_valid);
        CONNECT(spimode0, 0, spimode0.i_data_req_write, w_req_sdmem_write);
        CONNECT(spimode0, 0, spimode0.i_data_req_addr, wb_req_sdmem_addr);
        CONNECT(spimode0, 0, spimode0.i_data_req_wdata, wb_req_sdmem_wdata);
        CONNECT(spimode0, 0, spimode0.o_data_resp_valid, w_spi_resp_sdmem_valid);
        CONNECT(spimode0, 0, spimode0.o_data_resp_rdata, wb_spi_resp_sdmem_data);
        CONNECT(spimode0, 0, spimode0.i_crc16_0, wb_crc16_0);
        CONNECT(spimode0, 0, spimode0.o_crc16_clear, w_spi_crc16_clear);
        CONNECT(spimode0, 0, spimode0.o_crc16_next, w_spi_crc16_next);
        CONNECT(spimode0, 0, spimode0.o_wdog_ena, w_spi_wdog_ena);
        CONNECT(spimode0, 0, spimode0.i_wdog_trigger, w_wdog_trigger);
        CONNECT(spimode0, 0, spimode0.i_err_code, wb_err_code);
        CONNECT(spimode0, 0, spimode0.o_err_valid, w_spi_err_valid);
        CONNECT(spimode0, 0, spimode0.o_err_clear, w_spi_err_clear);
        CONNECT(spimode0, 0, spimode0.o_err_code, wb_spi_err_setcode);
        CONNECT(spimode0, 0, spimode0.o_400khz_ena, w_spi_400kHz_ena);
        CONNECT(spimode0, 0, spimode0.o_sdtype, wb_spi_sdtype);
    ENDNEW();

TEXT();
    NEW(sdmode0, sdmode0.getName().c_str());
        CONNECT(sdmode0, 0, sdmode0.i_clk, i_clk);
        CONNECT(sdmode0, 0, sdmode0.i_nrst, nrst_sdmode);
        CONNECT(sdmode0, 0, sdmode0.i_posedge, w_regs_sck_posedge);
        CONNECT(sdmode0, 0, sdmode0.i_dat0, i_dat0);
        CONNECT(sdmode0, 0, sdmode0.o_dat0, w_sd_dat0);
        CONNECT(sdmode0, 0, sdmode0.o_dat0_dir, w_sd_dat0_dir);
        CONNECT(sdmode0, 0, sdmode0.i_dat1, i_dat1);
        CONNECT(sdmode0, 0, sdmode0.o_dat1, w_sd_dat1);
        CONNECT(sdmode0, 0, sdmode0.o_dat1_dir, w_sd_dat1_dir);
        CONNECT(sdmode0, 0, sdmode0.i_dat2, i_dat2);
        CONNECT(sdmode0, 0, sdmode0.o_dat2, w_sd_dat2);
        CONNECT(sdmode0, 0, sdmode0.o_dat2_dir, w_sd_dat2_dir);
        CONNECT(sdmode0, 0, sdmode0.i_cd_dat3, i_cd_dat3);
        CONNECT(sdmode0, 0, sdmode0.o_dat3, w_sd_dat3);
        CONNECT(sdmode0, 0, sdmode0.o_dat3_dir, w_sd_dat3_dir);
        CONNECT(sdmode0, 0, sdmode0.i_detected, i_detected);
        CONNECT(sdmode0, 0, sdmode0.i_protect, i_protect);
        CONNECT(sdmode0, 0, sdmode0.i_cfg_pcie_12V_support, w_regs_pcie_12V_support);
        CONNECT(sdmode0, 0, sdmode0.i_cfg_pcie_available, w_regs_pcie_available);
        CONNECT(sdmode0, 0, sdmode0.i_cfg_voltage_supply, wb_regs_voltage_supply);
        CONNECT(sdmode0, 0, sdmode0.i_cfg_check_pattern, wb_regs_check_pattern);
        CONNECT(sdmode0, 0, sdmode0.i_cmd_req_ready, w_cmd_req_ready);
        CONNECT(sdmode0, 0, sdmode0.o_cmd_req_valid, w_sd_cmd_req_valid);
        CONNECT(sdmode0, 0, sdmode0.o_cmd_req_cmd, wb_sd_cmd_req_cmd);
        CONNECT(sdmode0, 0, sdmode0.o_cmd_req_arg, wb_sd_cmd_req_arg);
        CONNECT(sdmode0, 0, sdmode0.o_cmd_req_rn, wb_sd_cmd_req_rn);
        CONNECT(sdmode0, 0, sdmode0.i_cmd_resp_valid, w_cmd_resp_valid);
        CONNECT(sdmode0, 0, sdmode0.i_cmd_resp_cmd, wb_cmd_resp_cmd);
        CONNECT(sdmode0, 0, sdmode0.i_cmd_resp_arg32, wb_cmd_resp_reg);
        CONNECT(sdmode0, 0, sdmode0.o_data_req_ready, w_sd_req_sdmem_ready);
        CONNECT(sdmode0, 0, sdmode0.i_data_req_valid, w_req_sdmem_valid);
        CONNECT(sdmode0, 0, sdmode0.i_data_req_write, w_req_sdmem_write);
        CONNECT(sdmode0, 0, sdmode0.i_data_req_addr, wb_req_sdmem_addr);
        CONNECT(sdmode0, 0, sdmode0.i_data_req_wdata, wb_req_sdmem_wdata);
        CONNECT(sdmode0, 0, sdmode0.o_data_resp_valid, w_sd_resp_sdmem_valid);
        CONNECT(sdmode0, 0, sdmode0.o_data_resp_rdata, wb_sd_resp_sdmem_data);
        CONNECT(sdmode0, 0, sdmode0.i_crc16_0, wb_crc16_0);
        CONNECT(sdmode0, 0, sdmode0.i_crc16_1, wb_crc16_1);
        CONNECT(sdmode0, 0, sdmode0.i_crc16_2, wb_crc16_2);
        CONNECT(sdmode0, 0, sdmode0.i_crc16_3, wb_crc16_3);
        CONNECT(sdmode0, 0, sdmode0.o_crc16_clear, w_sd_crc16_clear);
        CONNECT(sdmode0, 0, sdmode0.o_crc16_next, w_sd_crc16_next);
        CONNECT(sdmode0, 0, sdmode0.o_wdog_ena, w_sd_wdog_ena);
        CONNECT(sdmode0, 0, sdmode0.i_wdog_trigger, w_wdog_trigger);
        CONNECT(sdmode0, 0, sdmode0.i_err_code, wb_err_code);
        CONNECT(sdmode0, 0, sdmode0.o_err_valid, w_sd_err_valid);
        CONNECT(sdmode0, 0, sdmode0.o_err_clear, w_sd_err_clear);
        CONNECT(sdmode0, 0, sdmode0.o_err_code, wb_sd_err_setcode);
        CONNECT(sdmode0, 0, sdmode0.o_400khz_ena, w_sd_400kHz_ena);
        CONNECT(sdmode0, 0, sdmode0.o_sdtype, wb_sd_sdtype);
    ENDNEW();

TEXT();
    NEW(cmdtrx0, cmdtrx0.getName().c_str());
        CONNECT(cmdtrx0, 0, cmdtrx0.i_clk, i_clk);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_nrst, i_nrst);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_sclk_posedge, w_regs_sck_posedge);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_sclk_negedge, w_regs_sck_negedge);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_cmd, w_cmd_in);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_cmd, w_trx_cmd);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_cmd_dir, w_trx_cmd_dir);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_cmd_cs, w_trx_cmd_csn);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_spi_mode, w_regs_spi_mode);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_err_code, wb_err_code);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_wdog_trigger, w_wdog_trigger);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_cmd_set_low, cmd_set_low);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_req_valid, w_cmd_req_valid);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_req_cmd, wb_cmd_req_cmd);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_req_arg, wb_cmd_req_arg);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_req_rn, wb_cmd_req_rn);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_req_ready, w_cmd_req_ready);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_resp_valid, w_cmd_resp_valid);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_resp_cmd, wb_cmd_resp_cmd);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_resp_reg, wb_cmd_resp_reg);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_resp_crc7_rx, wb_cmd_resp_crc7_rx);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_resp_crc7_calc, wb_cmd_resp_crc7_calc);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_resp_spistatus, wb_cmd_resp_spistatus);
        CONNECT(cmdtrx0, 0, cmdtrx0.i_resp_ready, w_cmd_resp_ready);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_wdog_ena, w_trx_wdog_ena);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_err_valid, w_trx_err_valid);
        CONNECT(cmdtrx0, 0, cmdtrx0.o_err_setcode, wb_trx_err_setcode);
    ENDNEW();

TEXT();
    NEW(cache0, cache0.getName().c_str());
        CONNECT(cache0, 0, cache0.i_clk, i_clk);
        CONNECT(cache0, 0, cache0.i_nrst, i_nrst);
        CONNECT(cache0, 0, cache0.i_req_valid, w_mem_req_valid);
        CONNECT(cache0, 0, cache0.i_req_write, w_mem_req_write);
        CONNECT(cache0, 0, cache0.i_req_addr, wb_mem_req_addr);
        CONNECT(cache0, 0, cache0.i_req_wdata, wb_mem_req_wdata);
        CONNECT(cache0, 0, cache0.i_req_wstrb, wb_mem_req_wstrb);
        CONNECT(cache0, 0, cache0.o_req_ready, w_cache_req_ready);
        CONNECT(cache0, 0, cache0.o_resp_valid, w_cache_resp_valid);
        CONNECT(cache0, 0, cache0.o_resp_data, wb_cache_resp_rdata);
        CONNECT(cache0, 0, cache0.o_resp_err, w_cache_resp_err);
        CONNECT(cache0, 0, cache0.i_resp_ready, w_cache_resp_ready);
        CONNECT(cache0, 0, cache0.i_req_mem_ready, w_req_sdmem_ready);
        CONNECT(cache0, 0, cache0.o_req_mem_valid, w_req_sdmem_valid);
        CONNECT(cache0, 0, cache0.o_req_mem_write, w_req_sdmem_write);
        CONNECT(cache0, 0, cache0.o_req_mem_addr, wb_req_sdmem_addr);
        CONNECT(cache0, 0, cache0.o_req_mem_data, wb_req_sdmem_wdata);
        CONNECT(cache0, 0, cache0.i_mem_data_valid, w_resp_sdmem_valid);
        CONNECT(cache0, 0, cache0.i_mem_data, wb_resp_sdmem_data);
        CONNECT(cache0, 0, cache0.i_mem_fault, w_err_pending);
        CONNECT(cache0, 0, cache0.i_flush_valid, w_regs_flush_valid);
        CONNECT(cache0, 0, cache0.o_flush_end, w_cache_flush_end);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void sdctrl::proc_comb() {

TEXT();
    IF (EQ(mode, MODE_PRE_INIT));
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
            IF (NZ(w_regs_spi_mode));
                SETVAL(mode, MODE_SPI);
                SETONE(nrst_spimode);
            ELSE();
                SETVAL(mode, MODE_SD);
                SETONE(nrst_sdmode);
            ENDIF();
        ENDIF();
    ELSIF (EQ(mode, MODE_SPI));
        TEXT("SPI MOSI:");
        SETVAL(comb.v_cmd_dir, sdctrl_cfg_->DIR_OUTPUT);
        SETVAL(comb.v_cmd_out, INV(ORx(2, &AND2(INV(w_trx_cmd), INV(w_trx_cmd_csn)),
                                          &AND2(INV(w_spi_dat), INV(w_spi_dat_csn)))));
        TEXT("SPI MISO:");
        SETVAL(comb.v_dat0_dir, sdctrl_cfg_->DIR_INPUT);
        SETVAL(comb.v_cmd_in, i_dat0);
        TEXT("SPI CSn:");
        SETVAL(comb.v_dat3_dir, sdctrl_cfg_->DIR_OUTPUT);
        SETVAL(comb.v_dat3_out, AND2(w_trx_cmd_csn, w_spi_dat_csn));
        TEXT("Unused in SPI mode:");
        SETVAL(comb.v_dat2_dir, sdctrl_cfg_->DIR_OUTPUT);
        SETONE(comb.v_dat2_out);
        SETVAL(comb.v_dat1_dir, sdctrl_cfg_->DIR_OUTPUT);
        SETONE(comb.v_dat1_out);

        TEXT();
        SETVAL(comb.v_cmd_req_valid, w_spi_cmd_req_valid);
        SETVAL(comb.vb_cmd_req_cmd, wb_spi_cmd_req_cmd);
        SETVAL(comb.vb_cmd_req_arg, wb_spi_cmd_req_arg);
        SETVAL(comb.vb_cmd_req_rn, wb_spi_cmd_req_rn);
        SETVAL(comb.v_req_sdmem_ready, w_spi_req_sdmem_ready);
        SETVAL(comb.v_resp_sdmem_valid, w_spi_resp_sdmem_valid);
        SETVAL(comb.vb_resp_sdmem_data, wb_spi_resp_sdmem_data);
        SETVAL(comb.v_err_valid, w_spi_err_valid);
        SETVAL(comb.v_err_clear, OR2(w_regs_err_clear, w_spi_err_clear));
        SETVAL(comb.vb_err_setcode, wb_spi_err_setcode);
        SETVAL(comb.v_400kHz_ena, w_spi_400kHz_ena);
        SETVAL(comb.vb_sdtype, wb_spi_sdtype);
        SETVAL(comb.v_wdog_ena, OR2(w_spi_wdog_ena, w_trx_wdog_ena));
        SETVAL(comb.v_crc16_clear, w_spi_crc16_clear);
        SETVAL(comb.v_crc16_next, w_spi_crc16_next);

    ELSE();
        SETVAL(comb.v_cmd_dir, w_trx_cmd_dir);
        SETVAL(comb.v_cmd_in, i_cmd);
        SETVAL(comb.v_cmd_out, w_trx_cmd);
        SETVAL(comb.v_dat0_dir, w_sd_dat0_dir);
        SETVAL(comb.v_dat0_out, w_sd_dat0);
        SETVAL(comb.v_dat1_dir, w_sd_dat1_dir);
        SETVAL(comb.v_dat1_out, w_sd_dat1);
        SETVAL(comb.v_dat2_dir, w_sd_dat2_dir);
        SETVAL(comb.v_dat2_out, w_sd_dat2);
        SETVAL(comb.v_dat3_dir, w_sd_dat3_dir);
        SETVAL(comb.v_dat3_out, w_sd_dat3);

        TEXT();
        SETVAL(comb.v_cmd_req_valid, w_sd_cmd_req_valid);
        SETVAL(comb.vb_cmd_req_cmd, wb_sd_cmd_req_cmd);
        SETVAL(comb.vb_cmd_req_arg, wb_sd_cmd_req_arg);
        SETVAL(comb.vb_cmd_req_rn, wb_sd_cmd_req_rn);
        SETVAL(comb.v_req_sdmem_ready, w_sd_req_sdmem_ready);
        SETVAL(comb.v_resp_sdmem_valid, w_sd_resp_sdmem_valid);
        SETVAL(comb.vb_resp_sdmem_data, wb_sd_resp_sdmem_data);
        SETVAL(comb.v_err_valid, w_sd_err_valid);
        SETVAL(comb.v_err_clear, OR2(w_regs_err_clear, w_sd_err_clear));
        SETVAL(comb.vb_err_setcode, wb_sd_err_setcode);
        SETVAL(comb.v_400kHz_ena, w_sd_400kHz_ena);
        SETVAL(comb.vb_sdtype, wb_sd_sdtype);
        SETVAL(comb.v_wdog_ena, OR2(w_sd_wdog_ena, w_trx_wdog_ena));
        SETVAL(comb.v_crc16_clear, w_sd_crc16_clear);
        SETVAL(comb.v_crc16_next, w_sd_crc16_next);
    ENDIF();

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(w_cmd_in, comb.v_cmd_in);
    SETVAL(o_cmd, comb.v_cmd_out);
    SETVAL(o_cmd_dir, comb.v_cmd_dir);
    SETVAL(o_cd_dat3, comb.v_dat3_out);
    SETVAL(o_cd_dat3_dir, comb.v_dat3_dir);
    SETVAL(o_dat2, comb.v_dat2_out);
    SETVAL(o_dat2_dir, comb.v_dat2_dir);
    SETVAL(o_dat1, comb.v_dat1_out);
    SETVAL(o_dat1_dir, comb.v_dat1_dir);
    SETVAL(o_dat0, comb.v_dat0_out);
    SETVAL(o_dat0_dir, comb.v_dat0_dir);
    SETVAL(w_cmd_req_valid, comb.v_cmd_req_valid);
    SETVAL(wb_cmd_req_cmd, comb.vb_cmd_req_cmd);
    SETVAL(wb_cmd_req_arg, comb.vb_cmd_req_arg);
    SETVAL(wb_cmd_req_rn, comb.vb_cmd_req_rn);
    SETONE(w_cmd_resp_ready);
    SETONE(w_cache_resp_ready);
    SETVAL(w_req_sdmem_ready, comb.v_req_sdmem_ready);
    SETVAL(w_resp_sdmem_valid, comb.v_resp_sdmem_valid);
    SETVAL(wb_resp_sdmem_data, comb.vb_resp_sdmem_data);
    SETVAL(w_err_valid, comb.v_err_valid);
    SETVAL(w_err_clear, comb.v_err_clear);
    SETVAL(wb_err_setcode, comb.vb_err_setcode);
    SETVAL(w_400kHz_ena, comb.v_400kHz_ena);
    SETVAL(wb_sdtype, comb.vb_sdtype);
    SETVAL(w_wdog_ena, comb.v_wdog_ena);
    SETVAL(w_crc16_clear, comb.v_crc16_clear);
    SETVAL(w_crc16_next, comb.v_crc16_next);
}
