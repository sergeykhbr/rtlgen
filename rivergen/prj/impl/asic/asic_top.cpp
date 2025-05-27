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

asic_top::asic_top(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "asic_top", name, comment),
    async_reset(this, "async_reset", "1", "CFG_ASYNC_RESET", NO_COMMENT),
    sim_uart_speedup_rate(this, "sim_uart_speedup_rate", "0", "simulation UART speed-up: 0=no speed up, 1=2x, 2=4x, etc"),
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
#if GENCFG_SD_CTRL_ENABLE
    _sdctrl0_(this, "SD-card signals:"),
    o_sd_sclk(this, "o_sd_sclk", "1"),
    io_sd_cmd(this, "io_sd_cmd", "1", "CMD IO Command/Resonse; Data output in SPI mode"),
    io_sd_dat0(this, "io_sd_dat0", "1", "Data[0] IO; Data input in SPI mode"),
    io_sd_dat1(this, "io_sd_dat1", "1"),
    io_sd_dat2(this, "io_sd_dat2", "1"),
    io_sd_cd_dat3(this, "io_sd_cd_dat3", "1", "CD/DAT3 IO CardDetect/Data[3]; CS output in SPI mode"),
    i_sd_detected(this, "i_sd_detected", "1", "SD-card detected"),
    i_sd_protect(this, "i_sd_protect", "1", "SD-card write protect"),
#endif
#if GENCFG_HDMI_ENABLE
    _i2c0_(this, "I2C master inerface to HDMI transmitter:"),
    o_i2c0_scl(this, "o_i2c0_scl", "1", "I2C clock upto 400 kHz (default 100 kHz)"),
    io_i2c0_sda(this, "io_i2c0_sda", "1", "I2C bi-directional data"),
    o_i2c0_nreset(this, "o_i2c0_nreset", "1", "I2C slave reset. PCA9548 I2C mux must be de-asserted."),
    _i2c1_(this, "Data interface to HDMI transmitter:"),
    o_hdmi_clk(this, "o_hdmi_clk", "1", "HDMI clock depends on resolution for 1366x768@60Hz is ~83MHz"),
    o_hdmi_hsync(this, "o_hdmi_hsync", "1", "Horizontal sync. strob"),
    o_hdmi_vsync(this, "o_hdmi_vsync", "1", "Vertical sync. strob"),
    o_hdmi_de(this, "o_hdmi_de", "1", "Data enable strob"),
    o_hdmi_d(this, "o_hdmi_d", "18", "Data in format YCbCr 16-bits"),
    o_hdmi_spdif(this, "o_hdmi_spdif", "1", "Sound channel output"),
    i_hdmi_spdif_out(this, "i_hdmi_spdif_out", "1", "Reverse sound channel"),
    i_hdmi_int(this, "i_hdmi_int", "1", "External interrupt from HDMI transmitter"),
#endif
    // param
    //gpio_signal_vector_def_(this, ""),
    // Singals:
    ib_clk_tcxo(this, "ib_clk_tcxo", "1"),
    ib_gpio_ipins(this, "ib_gpio_ipins", "12"),
    ob_gpio_opins(this, "ob_gpio_opins", "12"),
    ob_gpio_direction(this, "ob_gpio_direction", "12"),
#if GENCFG_SD_CTRL_ENABLE
    ib_sd_cmd(this, "ib_sd_cmd", "1"),
    ob_sd_cmd(this, "ob_sd_cmd", "1"),
    ob_sd_cmd_direction(this, "ob_sd_cmd_direction", "1"),
    ib_sd_dat0(this, "ib_sd_dat0", "1"),
    ob_sd_dat0(this, "ob_sd_dat0", "1"),
    ob_sd_dat0_direction(this, "ob_sd_dat0_direction", "1"),
    ib_sd_dat1(this, "ib_sd_dat1", "1"),
    ob_sd_dat1(this, "ob_sd_dat1", "1"),
    ob_sd_dat1_direction(this, "ob_sd_dat1_direction", "1"),
    ib_sd_dat2(this, "ib_sd_dat2", "1"),
    ob_sd_dat2(this, "ob_sd_dat2", "1"),
    ob_sd_dat2_direction(this, "ob_sd_dat2_direction", "1"),
    ib_sd_cd_dat3(this, "ib_sd_cd_dat3", "1"),
    ob_sd_cd_dat3(this, "ob_sd_cd_dat3", "1"),
    ob_sd_cd_dat3_direction(this, "ob_sd_cd_dat3_direction", "1"),
#endif
#if GENCFG_HDMI_ENABLE
    ob_i2c0_scl(this, "ob_i2c0_scl", "1"),
    ob_i2c0_sda(this, "ob_i2c0_sda", "1"),
    ob_i2c0_sda_direction(this, "ob_i2c0_sda_direction", "1"),
    ib_i2c0_sda(this, "ib_i2c0_sda", "1"),
    ob_i2c0_nreset(this, "ob_i2c0_nreset", "1"),
    ob_hdmi_hsync(this, "ob_hdmi_hsync", "1"),
    ob_hdmi_vsync(this, "ob_hdmi_vsync", "1"),
    ob_hdmi_de(this, "ob_hdmi_de", "1"),
    ob_hdmi_d(this, "ob_hdmi_d", "18"),
    ob_hdmi_spdif(this, "ob_hdmi_spdif", "1"),
    ib_hdmi_spdif_out(this, "ib_hdmi_spdif_out", "1"),
    ib_hdmi_int(this, "ib_hdmi_int", "1"),
#endif
    w_sys_rst(this, "w_sys_rst", "1"),
    w_sys_nrst(this, "w_sys_nrst", "1"),
    w_dbg_nrst(this, "w_dbg_nrst", "1"),
    w_dmreset(this, "w_dmreset", "1"),
    w_sys_clk(this, "w_sys_clk", "1"),
    w_ddr_clk(this, "w_ddr_clk", "1"),
    w_pcie_clk(this, "w_pcie_clk", "1"),
    w_pll_lock(this, "w_pll_lock", "1"),
    ddr_xmapinfo(this, "ddr_xmapinfo", NO_COMMENT),
    ddr_xdev_cfg(this, "ddr_xdev_cfg", NO_COMMENT),
    ddr_xslvo(this, "ddr_xslvo", NO_COMMENT),
    ddr_xslvi(this, "ddr_xslvi", NO_COMMENT),
    ddr_pmapinfo(this, "ddr_pmapinfo", NO_COMMENT),
    ddr_pdev_cfg(this, "ddr_pdev_cfg", NO_COMMENT),
    ddr_apbi(this, "ddr_apbi", NO_COMMENT),
    ddr_apbo(this, "ddr_apbo", NO_COMMENT),
    w_ddr_ui_nrst(this, "w_ddr_ui_nrst", "1"),
    w_ddr_ui_clk(this, "w_ddr_ui_clk", "1"),
    w_ddr3_init_calib_complete(this, "w_ddr3_init_calib_complete", "1"),
    w_pcie_phy_lnk_up(this, "w_pcie_phy_lnk_up", "1"),
    prci_pmapinfo(this, "prci_pmapinfo", NO_COMMENT),
    prci_dev_cfg(this, "prci_dev_cfg", NO_COMMENT),
    prci_apbi(this, "prci_apbi", NO_COMMENT),
    prci_apbo(this, "prci_apbo", NO_COMMENT),
#if GENCFG_PCIE_ENABLE
    w_pcie_user_clk(this, "w_pcie_user_clk", "1"),
    w_pcie_user_rst(this, "w_pcie_user_rst", "1"),
    w_pcie_nrst(this, "w_pcie_nrst", "1"),
    wb_pcie_completer_id(this, "wb_pcie_completer_id", "16", "Bus, Device, Function"),
    pcie_dmao(this, "pcie_dmao", NO_COMMENT),
    pcie_dmai(this, "pcie_dmai", NO_COMMENT),
#endif
    // submodules:
    iclk0(this, "iclk0", NO_COMMENT),
#if GENCFG_SD_CTRL_ENABLE
    iosdcmd0(this, "iosdcmd0", NO_COMMENT),
    iosddat0(this, "iosddat0", NO_COMMENT),
    iosddat1(this, "iosddat1", NO_COMMENT),
    iosddat2(this, "iosddat2", NO_COMMENT),
    iosddat3(this, "iosddat3", NO_COMMENT),
#endif
#if GENCFG_HDMI_ENABLE
    oi2c0scl(this, "oi2c0scl", NO_COMMENT),
    oi2c0nreset(this, "oi2c0nreset", NO_COMMENT),
    ioi2c0sda(this, "ioi2c0sda", NO_COMMENT),
    ohdmiclk(this, "ohdmiclk", NO_COMMENT),
    ohdmihsync(this, "ohdmihsync", NO_COMMENT),
    ohdmivsync(this, "ohdmivsync", NO_COMMENT),
    ohdmide(this, "ohdmide", NO_COMMENT),
    ohdmid(this, "ohdmid", NO_COMMENT),
    ohdmispdif(this, "ohdmispdif", NO_COMMENT),
    ihdmispdif(this, "ihdmispdif", NO_COMMENT),
    ihdmiint(this, "ihdmiint", NO_COMMENT),
#endif
    pll0(this, "pll0", NO_COMMENT),
    prci0(this, "prci0", NO_COMMENT),
    soc0(this, "soc0", NO_COMMENT)
{
    Operation::start(this);

    // Create and connet Sub-modules:
    NEW(iclk0, iclk0.getName().c_str());
        CONNECT(iclk0, 0, iclk0.i_clk_p, i_sclk_p);
        CONNECT(iclk0, 0, iclk0.i_clk_n, i_sclk_n);
        CONNECT(iclk0, 0, iclk0.o_clk, ib_clk_tcxo);
    ENDNEW();

#if GENCFG_SD_CTRL_ENABLE
TEXT();
    NEW(iosdcmd0, iosdcmd0.getName().c_str());
        CONNECT(iosdcmd0, 0, iosdcmd0.io, io_sd_cmd);
        CONNECT(iosdcmd0, 0, iosdcmd0.o, ib_sd_cmd);
        CONNECT(iosdcmd0, 0, iosdcmd0.i, ob_sd_cmd);
        CONNECT(iosdcmd0, 0, iosdcmd0.t, ob_sd_cmd_direction);
    ENDNEW();

TEXT();
    NEW(iosddat0, iosddat0.getName().c_str());
        CONNECT(iosddat0, 0, iosddat0.io, io_sd_dat0);
        CONNECT(iosddat0, 0, iosddat0.o, ib_sd_dat0);
        CONNECT(iosddat0, 0, iosddat0.i, ob_sd_dat0);
        CONNECT(iosddat0, 0, iosddat0.t, ob_sd_dat0_direction);
    ENDNEW();

TEXT();
    NEW(iosddat1, iosddat1.getName().c_str());
        CONNECT(iosddat1, 0, iosddat1.io, io_sd_dat1);
        CONNECT(iosddat1, 0, iosddat1.o, ib_sd_dat1);
        CONNECT(iosddat1, 0, iosddat1.i, ob_sd_dat1);
        CONNECT(iosddat1, 0, iosddat1.t, ob_sd_dat1_direction);
    ENDNEW();

TEXT();
    NEW(iosddat2, iosddat2.getName().c_str());
        CONNECT(iosddat2, 0, iosddat2.io, io_sd_dat2);
        CONNECT(iosddat2, 0, iosddat2.o, ib_sd_dat2);
        CONNECT(iosddat2, 0, iosddat2.i, ob_sd_dat2);
        CONNECT(iosddat2, 0, iosddat2.t, ob_sd_dat2_direction);
    ENDNEW();

TEXT();
    NEW(iosddat3, iosddat3.getName().c_str());
        CONNECT(iosddat3, 0, iosddat3.io, io_sd_cd_dat3);
        CONNECT(iosddat3, 0, iosddat3.o, ib_sd_cd_dat3);
        CONNECT(iosddat3, 0, iosddat3.i, ob_sd_cd_dat3);
        CONNECT(iosddat3, 0, iosddat3.t, ob_sd_cd_dat3_direction);
    ENDNEW();
#endif

#if GENCFG_HDMI_ENABLE
    TEXT();
    TEXT("======== HDMI I2C interface ========");
    NEW(oi2c0scl, oi2c0scl.getName().c_str());
        CONNECT(oi2c0scl, 0, oi2c0scl.i, ob_i2c0_scl);
        CONNECT(oi2c0scl, 0, oi2c0scl.o, o_i2c0_scl);
    ENDNEW();

    TEXT();
    NEW(oi2c0nreset, oi2c0nreset.getName().c_str());
        CONNECT(oi2c0nreset, 0, oi2c0nreset.i, ob_i2c0_nreset);
        CONNECT(oi2c0nreset, 0, oi2c0nreset.o, o_i2c0_nreset);
    ENDNEW();

    TEXT();
    NEW(ioi2c0sda, ioi2c0sda.getName().c_str());
        CONNECT(ioi2c0sda, 0, ioi2c0sda.io, io_i2c0_sda);
        CONNECT(ioi2c0sda, 0, ioi2c0sda.o, ib_i2c0_sda);
        CONNECT(ioi2c0sda, 0, ioi2c0sda.i, ob_i2c0_sda);
        CONNECT(ioi2c0sda, 0, ioi2c0sda.t, ob_i2c0_sda_direction);
    ENDNEW();

    TEXT();
    TEXT("======== HDMI data buffer ========");
    NEW(ohdmiclk, ohdmiclk.getName().c_str());
        CONNECT(ohdmiclk, 0, ohdmiclk.i, w_sys_clk);
        CONNECT(ohdmiclk, 0, ohdmiclk.o, o_hdmi_clk);
    ENDNEW();

    TEXT();
    NEW(ohdmihsync, ohdmihsync.getName().c_str());
        CONNECT(ohdmihsync, 0, ohdmihsync.i, ob_hdmi_hsync);
        CONNECT(ohdmihsync, 0, ohdmihsync.o, o_hdmi_hsync);
    ENDNEW();

    TEXT();
    NEW(ohdmivsync, ohdmivsync.getName().c_str());
        CONNECT(ohdmivsync, 0, ohdmivsync.i, ob_hdmi_vsync);
        CONNECT(ohdmivsync, 0, ohdmivsync.o, o_hdmi_vsync);
    ENDNEW();

    TEXT();
    NEW(ohdmide, ohdmide.getName().c_str());
        CONNECT(ohdmide, 0, ohdmide.i, ob_hdmi_de);
        CONNECT(ohdmide, 0, ohdmide.o, o_hdmi_de);
    ENDNEW();

    TEXT();
    ohdmid.width.setObjValue(new DecConst(18));
    NEW(ohdmid, ohdmid.getName().c_str());
        CONNECT(ohdmid, 0, ohdmid.i, ob_hdmi_d);
        CONNECT(ohdmid, 0, ohdmid.o, o_hdmi_d);
    ENDNEW();

    TEXT();
    NEW(ohdmispdif, ohdmispdif.getName().c_str());
        CONNECT(ohdmispdif, 0, ohdmispdif.i, ob_hdmi_spdif);
        CONNECT(ohdmispdif, 0, ohdmispdif.o, o_hdmi_spdif);
    ENDNEW();

    TEXT();
    NEW(ihdmispdif, ihdmispdif.getName().c_str());
        CONNECT(ihdmispdif, 0, ihdmispdif.i, i_hdmi_spdif_out);
        CONNECT(ihdmispdif, 0, ihdmispdif.o, ib_hdmi_spdif_out);
    ENDNEW();

    TEXT();
    NEW(ihdmiint, ihdmiint.getName().c_str());
        CONNECT(ihdmiint, 0, ihdmiint.i, i_hdmi_int);
        CONNECT(ihdmiint, 0, ihdmiint.o, ib_hdmi_int);
    ENDNEW();
#endif


TEXT();
    NEW(pll0, pll0.getName().c_str());
        CONNECT(pll0, 0, pll0.i_reset, i_rst);
        CONNECT(pll0, 0, pll0.i_clk_tcxo, ib_clk_tcxo);
        CONNECT(pll0, 0, pll0.o_clk_sys, w_sys_clk);
        CONNECT(pll0, 0, pll0.o_clk_ddr, w_ddr_clk);
        CONNECT(pll0, 0, pll0.o_clk_pcie, w_pcie_clk);
        CONNECT(pll0, 0, pll0.o_locked, w_pll_lock);
    ENDNEW();

TEXT();
    NEW(prci0, prci0.getName().c_str());
        CONNECT(prci0, 0, prci0.i_clk, ib_clk_tcxo);
        CONNECT(prci0, 0, prci0.i_pwrreset, i_rst);
        CONNECT(prci0, 0, prci0.i_dmireset, w_dmreset);
        CONNECT(prci0, 0, prci0.i_sys_locked, w_pll_lock);
        CONNECT(prci0, 0, prci0.i_ddr_locked, w_ddr3_init_calib_complete);
        CONNECT(prci0, 0, prci0.i_pcie_phy_clk, w_pcie_user_clk);
        CONNECT(prci0, 0, prci0.i_pcie_phy_rst, w_pcie_user_rst);
        CONNECT(prci0, 0, prci0.i_pcie_phy_lnk_up, w_pcie_phy_lnk_up);
        CONNECT(prci0, 0, prci0.o_sys_rst, w_sys_rst);
        CONNECT(prci0, 0, prci0.o_sys_nrst, w_sys_nrst);
        CONNECT(prci0, 0, prci0.o_dbg_nrst, w_dbg_nrst);
        CONNECT(prci0, 0, prci0.o_pcie_nrst, w_pcie_nrst);
        CONNECT(prci0, 0, prci0.i_mapinfo, prci_pmapinfo);
        CONNECT(prci0, 0, prci0.o_cfg, prci_dev_cfg);
        CONNECT(prci0, 0, prci0.i_apbi, prci_apbi);
        CONNECT(prci0, 0, prci0.o_apbo, prci_apbo);
    ENDNEW();

TEXT();
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
        CONNECT(soc0, 0, soc0.o_jtag_vref, o_jtag_vref);
        CONNECT(soc0, 0, soc0.i_uart1_rd, i_uart1_rd);
        CONNECT(soc0, 0, soc0.o_uart1_td, o_uart1_td);
#if GENCFG_SD_CTRL_ENABLE
        CONNECT(soc0, 0, soc0.o_sd_sclk, o_sd_sclk);
        CONNECT(soc0, 0, soc0.i_sd_cmd, ib_sd_cmd);
        CONNECT(soc0, 0, soc0.o_sd_cmd, ob_sd_cmd);
        CONNECT(soc0, 0, soc0.o_sd_cmd_dir, ob_sd_cmd_direction);
        CONNECT(soc0, 0, soc0.i_sd_dat0, ib_sd_dat0);
        CONNECT(soc0, 0, soc0.o_sd_dat0, ob_sd_dat0);
        CONNECT(soc0, 0, soc0.o_sd_dat0_dir, ob_sd_dat0_direction);
        CONNECT(soc0, 0, soc0.i_sd_dat1, ib_sd_dat1);
        CONNECT(soc0, 0, soc0.o_sd_dat1, ob_sd_dat1);
        CONNECT(soc0, 0, soc0.o_sd_dat1_dir, ob_sd_dat1_direction);
        CONNECT(soc0, 0, soc0.i_sd_dat2, ib_sd_dat2);
        CONNECT(soc0, 0, soc0.o_sd_dat2, ob_sd_dat2);
        CONNECT(soc0, 0, soc0.o_sd_dat2_dir, ob_sd_dat2_direction);
        CONNECT(soc0, 0, soc0.i_sd_cd_dat3, ib_sd_cd_dat3);
        CONNECT(soc0, 0, soc0.o_sd_cd_dat3, ob_sd_cd_dat3);
        CONNECT(soc0, 0, soc0.o_sd_cd_dat3_dir, ob_sd_cd_dat3_direction);
        CONNECT(soc0, 0, soc0.i_sd_detected, i_sd_detected);
        CONNECT(soc0, 0, soc0.i_sd_protect, i_sd_protect);
#endif
#if GENCFG_HDMI_ENABLE
        CONNECT(soc0, 0, soc0.o_i2c0_scl, ob_i2c0_scl);
        CONNECT(soc0, 0, soc0.i_i2c0_sda, ib_i2c0_sda);
        CONNECT(soc0, 0, soc0.o_i2c0_sda, ob_i2c0_sda);
        CONNECT(soc0, 0, soc0.o_i2c0_sda_dir, ob_i2c0_sda_direction);
        CONNECT(soc0, 0, soc0.o_i2c0_nreset, ob_i2c0_nreset);
        CONNECT(soc0, 0, soc0.i_hdmi_clk, w_sys_clk);
        CONNECT(soc0, 0, soc0.o_hdmi_hsync, ob_hdmi_hsync);
        CONNECT(soc0, 0, soc0.o_hdmi_vsync, ob_hdmi_vsync);
        CONNECT(soc0, 0, soc0.o_hdmi_de, ob_hdmi_de);
        CONNECT(soc0, 0, soc0.o_hdmi_d, ob_hdmi_d);
        CONNECT(soc0, 0, soc0.o_hdmi_spdif, ob_hdmi_spdif);
        CONNECT(soc0, 0, soc0.i_hdmi_spdif_out, ib_hdmi_spdif_out);
        CONNECT(soc0, 0, soc0.i_hdmi_int, ib_hdmi_int);
#endif
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
#if GENCFG_PCIE_ENABLE
        CONNECT(soc0, 0, soc0.i_pcie_nrst, w_pcie_nrst);
        CONNECT(soc0, 0, soc0.i_pcie_clk, w_pcie_user_clk);
        CONNECT(soc0, 0, soc0.i_pcie_completer_id, wb_pcie_completer_id);
        CONNECT(soc0, 0, soc0.o_pcie_dmao, pcie_dmao);
        CONNECT(soc0, 0, soc0.i_pcie_dmai, pcie_dmai);
#endif
    ENDNEW();
}

