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

#include "asic_top.h"
#include "config_target.h"

asic_top::asic_top(GenObject *parent, const char *name) :
    ModuleObject(parent, "asic_top", name),
    // simulation parameters
    bootfile(this, "bootfile", "", "Project relative HEX-file name to init boot ROM without .hex extension"),
    sim_uart_speedup_rate(this, "sim_uart_speedup_rate", "0", "simulation UART speed-up: 0=no speed up, 1=2x, 2=4x, etc"),
    // Generic parameters
    async_reset(this, "async_reset", "CFG_ASYNC_RESET"),
    // Ports
    i_rst(this, "i_rst", "1", "Power-on system reset active HIGH"),
    _clk0_(this, "Differential clock (LVDS) positive/negaive signal."),
    i_sclk_p(this, "i_sclk_p", "1"),
    i_sclk_n(this, "i_sclk_n", "1"),
    _gpio0_(this, "GPIO: [11:4] LEDs; [3:0] DIP switch"),
    io_gpio(this, "io_gpio", "12"),
    _jtag0_(this, "JTAG signals:"),
    i_jtag_trst(this, "i_jtag_trst", "1"),
    i_jtag_tck(this, "i_jtag_tck", "1"),
    i_jtag_tms(this, "i_jtag_tms", "1"),
    i_jtag_tdi(this, "i_jtag_tdi", "1"),
    o_jtag_tdo(this, "o_jtag_tdo", "1"),
    o_jtag_vref(this, "o_jtag_vref", "1"),
    _uart1_(this, "UART1 signals"),
    i_uart1_rd(this, "i_uart1_rd", "1"),
    o_uart1_td(this, "o_uart1_td", "1"),
    _spi0_(this, "SPI SD-card signals:"),
    o_spi_cs(this, "o_spi_cs", "1"),
    o_spi_sclk(this, "o_spi_sclk", "1"),
    o_spi_mosi(this, "o_spi_mosi", "1", "SPI: Master Output Slave Input"),
    i_spi_miso(this, "i_spi_miso", "1", "SPI: Master Input Slave Output"),
    i_sd_detected(this, "i_sd_detected", "1", "SD-card detected"),
    i_sd_protect(this, "i_sd_protect", "1", "SD-card write protect"),
    // param
    // Singals:
    ib_clk_tcxo(this, "ib_clk_tcxo", "1"),
    ib_gpio_ipins(this, "ib_gpio_ipins", "12"),
    ob_gpio_opins(this, "ob_gpio_opins", "12"),
    ob_gpio_direction(this, "ob_gpio_direction", "12"),
    w_sys_rst(this, "w_sys_rst", "1"),
    w_sys_nrst(this, "w_sys_nrst", "1"),
    w_dbg_nrst(this, "w_dbg_nrst", "1"),
    w_dmreset(this, "w_dmreset", "1"),
    w_sys_clk(this, "w_sys_clk", "1"),
    w_ddr_clk(this, "w_ddr_clk", "1"),
    w_pll_lock(this, "w_pll_lock", "1"),
    ddr_xmapinfo(this, "ddr_xmapinfo"),
    ddr_xdev_cfg(this, "ddr_xdev_cfg"),
    ddr_xslvo(this, "ddr_xslvo"),
    ddr_xslvi(this, "ddr_xslvi"),
    ddr_pmapinfo(this, "ddr_pmapinfo"),
    ddr_pdev_cfg(this, "ddr_pdev_cfg"),
    ddr_apbi(this, "ddr_apbi"),
    ddr_apbo(this, "ddr_apbo"),
    w_ddr_ui_nrst(this, "w_ddr_ui_nrst", "1"),
    w_ddr_ui_clk(this, "w_ddr_ui_clk", "1"),
    w_ddr3_init_calib_complete(this, "w_ddr3_init_calib_complete", "1"),
    prci_pmapinfo(this, "prci_pmapinfo"),
    prci_dev_cfg(this, "prci_dev_cfg"),
    prci_apbi(this, "prci_apbi"),
    prci_apbo(this, "prci_apbo"),
    // submodules:
    prci0(this, "prci0"),
    soc0(this, "soc0"),
    comb(this)
{
    Operation::start(this);

    // Create and connet Sub-modules:
    NEW(prci0, prci0.getName().c_str());
        CONNECT(prci0, 0, prci0.i_clk, ib_clk_tcxo);
        CONNECT(prci0, 0, prci0.i_pwrreset, i_rst);
        CONNECT(prci0, 0, prci0.i_dmireset, w_dmreset);
        CONNECT(prci0, 0, prci0.i_sys_locked, w_pll_lock);
        CONNECT(prci0, 0, prci0.i_ddr_locked, w_ddr3_init_calib_complete);
        CONNECT(prci0, 0, prci0.o_sys_rst, w_sys_rst);
        CONNECT(prci0, 0, prci0.o_sys_nrst, w_sys_nrst);
        CONNECT(prci0, 0, prci0.o_dbg_nrst, w_dbg_nrst);
        CONNECT(prci0, 0, prci0.i_mapinfo, prci_pmapinfo);
        CONNECT(prci0, 0, prci0.o_cfg, prci_dev_cfg);
        CONNECT(prci0, 0, prci0.i_apbi, prci_apbi);
        CONNECT(prci0, 0, prci0.o_apbo, prci_apbo);
    ENDNEW();


    soc0.bootfile.setObjValue(&bootfile);
    soc0.sim_uart_speedup_rate.setObjValue(&sim_uart_speedup_rate);
    NEW(soc0, soc0.getName().c_str());
        CONNECT(soc0, 0, soc0.i_sys_nrst, w_sys_nrst);
        CONNECT(soc0, 0, soc0.i_sys_clk, w_sys_clk);
        CONNECT(soc0, 0, soc0.i_dbg_nrst, w_dbg_nrst);
        CONNECT(soc0, 0, soc0.i_ddr_nrst, w_ddr_ui_nrst);
        CONNECT(soc0, 0, soc0.i_ddr_clk, w_ddr_ui_clk);
        CONNECT(soc0, 0, soc0.i_gpio, ib_gpio_ipins);
        CONNECT(soc0, 0, soc0.o_gpio, ob_gpio_opins);
        CONNECT(soc0, 0, soc0.o_gpio_dir, ob_gpio_direction);
        CONNECT(soc0, 0, soc0.i_jtag_trst, i_jtag_trst);
        CONNECT(soc0, 0, soc0.i_jtag_tck, i_jtag_tck);
        CONNECT(soc0, 0, soc0.i_jtag_tms, i_jtag_tms);
        CONNECT(soc0, 0, soc0.i_jtag_tdi, i_jtag_tdi);
        CONNECT(soc0, 0, soc0.o_jtag_tdo, o_jtag_tdo);
        CONNECT(soc0, 0, soc0.i_uart1_rd, i_uart1_rd);
        CONNECT(soc0, 0, soc0.o_uart1_td, o_uart1_td);
        CONNECT(soc0, 0, soc0.o_spi_cs, o_spi_cs);
        CONNECT(soc0, 0, soc0.o_spi_sclk, o_spi_sclk);
        CONNECT(soc0, 0, soc0.o_spi_mosi, o_spi_mosi);
        CONNECT(soc0, 0, soc0.i_spi_miso, i_spi_miso);
        CONNECT(soc0, 0, soc0.i_sd_detected, i_sd_detected);
        CONNECT(soc0, 0, soc0.i_sd_protect, i_sd_protect);
        CONNECT(soc0, 0, soc0.o_dmreset, w_dmreset);
        CONNECT(soc0, 0, soc0.o_prci_pmapinfo, prci_pmapinfo);
        CONNECT(soc0, 0, soc0.i_prci_pdevcfg, prci_dev_cfg);
        CONNECT(soc0, 0, soc0.o_prci_apbi, prci_apbi);
        CONNECT(soc0, 0, soc0.i_prci_apbo, prci_apbo);
        CONNECT(soc0, 0, soc0.o_ddr_pmapinfo, ddr_pmapinfo);
        CONNECT(soc0, 0, soc0.i_ddr_pdevcfg, ddr_pdev_cfg);
        CONNECT(soc0, 0, soc0.o_ddr_apbi, ddr_apbi);
        CONNECT(soc0, 0, soc0.i_ddr_apbo, ddr_apbo);
        CONNECT(soc0, 0, soc0.o_ddr_xmapinfo, ddr_xmapinfo);
        CONNECT(soc0, 0, soc0.i_ddr_xdevcfg, ddr_xdev_cfg);
        CONNECT(soc0, 0, soc0.o_ddr_xslvi, ddr_xslvi);
        CONNECT(soc0, 0, soc0.i_ddr_xslvo, ddr_xslvo);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void asic_top::proc_comb() {
}