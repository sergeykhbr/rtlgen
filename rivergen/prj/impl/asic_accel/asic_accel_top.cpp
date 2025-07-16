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

#include "asic_accel_top.h"

asic_accel_top::asic_accel_top(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "asic_accel_top", name, comment),
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
    _ddr0_(this, "DDR signals:"),
    o_ddr3_reset_n(this, "o_ddr3_reset_n", "1", NO_COMMENT),
    o_ddr3_ck_n(this, "o_ddr3_ck_n", "1", NO_COMMENT),
    o_ddr3_ck_p(this, "o_ddr3_ck_p", "1", NO_COMMENT),
    o_ddr3_cke(this, "o_ddr3_cke", "1", NO_COMMENT),
    o_ddr3_cs_n(this, "o_ddr3_cs_n", "1", "Chip select active LOW"),
    o_ddr3_ras_n(this, "o_ddr3_ras_n", "1", NO_COMMENT),
    o_ddr3_cas_n(this, "o_ddr3_cas_n", "1", NO_COMMENT),
    o_ddr3_we_n(this, "o_ddr3_we_n", "1", "Write enable active LOW"),
    o_ddr3_dm(this, "o_ddr3_dm", "8", "Data mask"),
    o_ddr3_ba(this, "o_ddr3_ba", "3", "Bank address"),
    o_ddr3_addr(this, "o_ddr3_addr", "14", NO_COMMENT),
    io_ddr3_dq(this, "io_ddr3_dq", "64", NO_COMMENT),
    io_ddr3_dqs_n(this, "io_ddr3_dqs_n", "8", "Data strob positive"),
    io_ddr3_dqs_p(this, "io_ddr3_dqs_p", "8", "Data strob negative"),
    o_ddr3_odt(this, "o_ddr3_odt", "1", "on-die termination"),
    // param
    //gpio_signal_vector_def_(this, ""),
    // Singals:
    ib_clk_tcxo(this, "ib_clk_tcxo", "1"),
    ib_gpio_ipins(this, "ib_gpio_ipins", "12"),
    ob_gpio_opins(this, "ob_gpio_opins", "12"),
    ob_gpio_direction(this, "ob_gpio_direction", "12"),
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
    w_sys_rst(this, "w_sys_rst", "1"),
    w_sys_nrst(this, "w_sys_nrst", "1"),
    w_dbg_nrst(this, "w_dbg_nrst", "1"),
    w_hdmi_nrst(this, "w_hdmi_nrst", "1"),
    w_dmreset(this, "w_dmreset", "1"),
    w_sys_clk(this, "w_sys_clk", "1"),
    w_ddr_clk(this, "w_ddr_clk", "1"),
    w_pcie_clk(this, "w_pcie_clk", "1"),
    w_pll_lock(this, "w_pll_lock", "1"),
    _t5_(this, "DDR AXI slave interface:"),
    wb_ddr_aw_id(this, "wb_ddr_aw_id", "CFG_SYSBUS_ID_BITS", NO_COMMENT),
    wb_ddr_aw_addr(this, "wb_ddr_aw_addr", "30", "1 GB by default"),
    wb_ddr_aw_len(this, "wb_ddr_aw_len", "8", NO_COMMENT),
    wb_ddr_aw_size(this, "wb_ddr_aw_size", "3", NO_COMMENT),
    wb_ddr_aw_burst(this, "wb_ddr_aw_burst", "2", NO_COMMENT),
    w_ddr_aw_lock(this, "w_ddr_aw_lock", "1", NO_COMMENT),
    wb_ddr_aw_cache(this, "wb_ddr_aw_cache", "4", NO_COMMENT),
    wb_ddr_aw_prot(this, "wb_ddr_aw_prot", "3", NO_COMMENT),
    wb_ddr_aw_qos(this, "wb_ddr_aw_qos", "4", NO_COMMENT),
    w_ddr_aw_valid(this, "w_ddr_aw_valid", "1", NO_COMMENT),
    w_ddr_aw_ready(this, "w_ddr_aw_ready", "1", NO_COMMENT),
    wb_ddr_w_data(this, "wb_ddr_w_data", "64", NO_COMMENT),
    wb_ddr_w_strb(this, "wb_ddr_w_strb", "8", NO_COMMENT),
    w_ddr_w_last(this, "w_ddr_w_last", "1", NO_COMMENT),
    w_ddr_w_valid(this, "w_ddr_w_valid", "1", NO_COMMENT),
    w_ddr_w_ready(this, "w_ddr_w_ready", "1", NO_COMMENT),
    w_ddr_b_ready(this, "w_ddr_b_ready", "1", NO_COMMENT),
    wb_ddr_b_id(this, "wb_ddr_b_id", "CFG_SYSBUS_ID_BITS", NO_COMMENT),
    wb_ddr_b_resp(this, "wb_ddr_b_resp", "2", NO_COMMENT),
    w_ddr_b_valid(this, "w_ddr_b_valid", "1", NO_COMMENT),
    wb_ddr_ar_id(this, "wb_ddr_ar_id", "CFG_SYSBUS_ID_BITS", NO_COMMENT),
    wb_ddr_ar_addr(this, "wb_ddr_ar_addr", "30", "1 GB by default"),
    wb_ddr_ar_len(this, "wb_ddr_ar_len", "8", NO_COMMENT),
    wb_ddr_ar_size(this, "wb_ddr_ar_size", "3", NO_COMMENT),
    wb_ddr_ar_burst(this, "wb_ddr_ar_burst", "2", NO_COMMENT),
    w_ddr_ar_lock(this, "w_ddr_ar_lock", "1", NO_COMMENT),
    wb_ddr_ar_cache(this, "wb_ddr_ar_cache", "4", NO_COMMENT),
    wb_ddr_ar_prot(this, "wb_ddr_ar_prot", "3", NO_COMMENT),
    wb_ddr_ar_qos(this, "wb_ddr_ar_qos", "4", NO_COMMENT),
    w_ddr_ar_valid(this, "w_ddr_ar_valid", "1", NO_COMMENT),
    w_ddr_ar_ready(this, "w_ddr_ar_ready", "1", NO_COMMENT),
    w_ddr_r_ready(this, "w_ddr_r_ready", "1", NO_COMMENT),
    wb_ddr_r_id(this, "wb_ddr_r_id", "CFG_SYSBUS_ID_BITS", NO_COMMENT),
    wb_ddr_r_data(this, "wb_ddr_r_data", "64", NO_COMMENT),
    wb_ddr_r_resp(this, "wb_ddr_r_resp", "2", NO_COMMENT),
    w_ddr_r_last(this, "w_ddr_r_last", "1", NO_COMMENT),
    w_ddr_r_valid(this, "w_ddr_r_valid", "1", NO_COMMENT),
    w_ddr_app_sr_req(this, "w_ddr_app_sr_req", "1", "Self-refresh request (low-power mode)"),
    w_ddr_app_ref_req(this, "w_ddr_app_ref_req", "1", "Periodic refresh request ~7.8 us"),
    w_ddr_app_zq_req(this, "w_ddr_app_zq_req", "1", "ZQ calibration request. Startup and runtime maintenance"),
    w_ddr_app_sr_active(this, "w_ddr_app_sr_active", "1", "Self-resfresh is active (low-power mode or sleep)"),
    w_ddr_app_ref_ack(this, "w_ddr_app_ref_ack", "1", "Refresh request acknoledged"),
    w_ddr_app_zq_ack(this, "w_ddr_app_zq_ack", "1", "ZQ calibration request acknowledged"),
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
    w_pcie_user_clk(this, "w_pcie_user_clk", "1"),
    w_pcie_user_rst(this, "w_pcie_user_rst", "1"),
    w_pcie_nrst(this, "w_pcie_nrst", "1"),
    wb_pcie_completer_id(this, "wb_pcie_completer_id", "16", "Bus, Device, Function"),
    pcie_dmao(this, "pcie_dmao", NO_COMMENT),
    pcie_dmai(this, "pcie_dmai", NO_COMMENT),
    // submodules:
    iclk0(this, "iclk0", NO_COMMENT),
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
    pll0(this, "pll0", NO_COMMENT),
    prci0(this, "prci0", NO_COMMENT),
    ddr3(this, "ddr3", NO_COMMENT),
    soc0(this, "soc0", NO_COMMENT)
{
    Operation::start(this);

    // Create and connet Sub-modules:
    NEW(iclk0, iclk0.getName().c_str());
        CONNECT(iclk0, 0, iclk0.i_clk_p, i_sclk_p);
        CONNECT(iclk0, 0, iclk0.i_clk_n, i_sclk_n);
        CONNECT(iclk0, 0, iclk0.o_clk, ib_clk_tcxo);
    ENDNEW();

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
        CONNECT(prci0, 0, prci0.o_hdmi_nrst, w_hdmi_nrst);
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
        CONNECT(soc0, 0, soc0.i_cpu_nrst, w_sys_nrst);
        CONNECT(soc0, 0, soc0.i_cpu_clk, w_sys_clk);
        CONNECT(soc0, 0, soc0.i_apb_nrst, w_sys_nrst);
        CONNECT(soc0, 0, soc0.i_apb_clk, w_sys_clk);
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
        CONNECT(soc0, 0, soc0.o_i2c0_scl, ob_i2c0_scl);
        CONNECT(soc0, 0, soc0.i_i2c0_sda, ib_i2c0_sda);
        CONNECT(soc0, 0, soc0.o_i2c0_sda, ob_i2c0_sda);
        CONNECT(soc0, 0, soc0.o_i2c0_sda_dir, ob_i2c0_sda_direction);
        CONNECT(soc0, 0, soc0.o_i2c0_nreset, ob_i2c0_nreset);
        CONNECT(soc0, 0, soc0.i_hdmi_nrst, w_hdmi_nrst);
        CONNECT(soc0, 0, soc0.i_hdmi_clk, w_sys_clk);
        CONNECT(soc0, 0, soc0.o_hdmi_hsync, ob_hdmi_hsync);
        CONNECT(soc0, 0, soc0.o_hdmi_vsync, ob_hdmi_vsync);
        CONNECT(soc0, 0, soc0.o_hdmi_de, ob_hdmi_de);
        CONNECT(soc0, 0, soc0.o_hdmi_d, ob_hdmi_d);
        CONNECT(soc0, 0, soc0.o_hdmi_spdif, ob_hdmi_spdif);
        CONNECT(soc0, 0, soc0.i_hdmi_spdif_out, ib_hdmi_spdif_out);
        CONNECT(soc0, 0, soc0.i_hdmi_int, ib_hdmi_int);
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
        CONNECT(soc0, 0, soc0.i_pcie_nrst, w_pcie_nrst);
        CONNECT(soc0, 0, soc0.i_pcie_clk, w_pcie_user_clk);
        CONNECT(soc0, 0, soc0.i_pcie_completer_id, wb_pcie_completer_id);
        CONNECT(soc0, 0, soc0.o_pcie_dmao, pcie_dmao);
        CONNECT(soc0, 0, soc0.i_pcie_dmai, pcie_dmai);
    ENDNEW();

    TEXT();
    TEXT("----------------------------------------");
    TEXT("External IPs");
    NEW(ddr3, ddr3.getName().c_str());
        CONNECT(ddr3, 0, ddr3.i_nrst, w_sys_nrst);
        /*CONNECT(ddr3, 0, ddr3.i_apb_clk, w_sys_clk);
        CONNECT(ddr3, 0, ddr3.i_xslv_nrst, w_sys_nrst);
        CONNECT(ddr3, 0, ddr3.i_xslv_clk, w_sys_clk);
        CONNECT(ddr3, 0, ddr3.i_xmapinfo, ddr_xmapinfo);
        CONNECT(ddr3, 0, ddr3.o_xcfg, ddr_xdev_cfg);
        CONNECT(ddr3, 0, ddr3.i_xslvi, ddr_xslvi);
        CONNECT(ddr3, 0, ddr3.o_xslvo, ddr_xslvo);
        CONNECT(ddr3, 0, ddr3.i_pmapinfo, ddr_pmapinfo);
        CONNECT(ddr3, 0, ddr3.o_pcfg, ddr_pdev_cfg);
        CONNECT(ddr3, 0, ddr3.i_apbi, ddr_apbi);
        CONNECT(ddr3, 0, ddr3.o_apbo, ddr_apbo);*/
        CONNECT(ddr3, 0, ddr3.o_ui_nrst, w_ddr_ui_nrst);
        CONNECT(ddr3, 0, ddr3.o_ui_clk, w_ddr_ui_clk);
        CONNECT(ddr3, 0, ddr3.o_ddr3_reset_n, o_ddr3_reset_n);
        CONNECT(ddr3, 0, ddr3.o_ddr3_ck_n, o_ddr3_ck_n);
        CONNECT(ddr3, 0, ddr3.o_ddr3_ck_p, o_ddr3_ck_p);
        CONNECT(ddr3, 0, ddr3.o_ddr3_cke, o_ddr3_cke);
        CONNECT(ddr3, 0, ddr3.o_ddr3_cs_n, o_ddr3_cs_n);
        CONNECT(ddr3, 0, ddr3.o_ddr3_ras_n, o_ddr3_ras_n);
        CONNECT(ddr3, 0, ddr3.o_ddr3_cas_n, o_ddr3_cas_n);
        CONNECT(ddr3, 0, ddr3.o_ddr3_we_n, o_ddr3_we_n);
        CONNECT(ddr3, 0, ddr3.o_ddr3_dm, o_ddr3_dm);
        CONNECT(ddr3, 0, ddr3.o_ddr3_ba, o_ddr3_ba);
        CONNECT(ddr3, 0, ddr3.o_ddr3_addr, o_ddr3_addr);
        CONNECT(ddr3, 0, ddr3.io_ddr3_dq, io_ddr3_dq);
        CONNECT(ddr3, 0, ddr3.io_ddr3_dqs_n, io_ddr3_dqs_n);
        CONNECT(ddr3, 0, ddr3.io_ddr3_dqs_p, io_ddr3_dqs_p);
        CONNECT(ddr3, 0, ddr3.o_ddr3_odt, o_ddr3_odt);
        CONNECT(ddr3, 0, ddr3.o_init_calib_done, w_ddr3_init_calib_complete);
        CONNECT(ddr3, 0, ddr3.i_aw_id, wb_ddr_aw_id);
        CONNECT(ddr3, 0, ddr3.i_aw_addr, wb_ddr_aw_addr);
        CONNECT(ddr3, 0, ddr3.i_aw_len, wb_ddr_aw_len);
        CONNECT(ddr3, 0, ddr3.i_aw_size, wb_ddr_aw_size);
        CONNECT(ddr3, 0, ddr3.i_aw_burst, wb_ddr_aw_burst);
        CONNECT(ddr3, 0, ddr3.i_aw_lock, w_ddr_aw_lock);
        CONNECT(ddr3, 0, ddr3.i_aw_cache, wb_ddr_aw_cache);
        CONNECT(ddr3, 0, ddr3.i_aw_prot, wb_ddr_aw_prot);
        CONNECT(ddr3, 0, ddr3.i_aw_qos, wb_ddr_aw_qos);
        CONNECT(ddr3, 0, ddr3.i_aw_valid, w_ddr_aw_valid);
        CONNECT(ddr3, 0, ddr3.o_aw_ready, w_ddr_aw_ready);
        CONNECT(ddr3, 0, ddr3.i_w_data, wb_ddr_w_data);
        CONNECT(ddr3, 0, ddr3.i_w_strb, wb_ddr_w_strb);
        CONNECT(ddr3, 0, ddr3.i_w_last, w_ddr_w_last);
        CONNECT(ddr3, 0, ddr3.i_w_valid, w_ddr_w_valid);
        CONNECT(ddr3, 0, ddr3.o_w_ready, w_ddr_w_ready);
        CONNECT(ddr3, 0, ddr3.i_b_ready, w_ddr_b_ready);
        CONNECT(ddr3, 0, ddr3.o_b_id, wb_ddr_b_id);
        CONNECT(ddr3, 0, ddr3.o_b_resp, wb_ddr_b_resp);
        CONNECT(ddr3, 0, ddr3.o_b_valid, w_ddr_b_valid);
        CONNECT(ddr3, 0, ddr3.i_ar_id, wb_ddr_ar_id);
        CONNECT(ddr3, 0, ddr3.i_ar_addr, wb_ddr_ar_addr);
        CONNECT(ddr3, 0, ddr3.i_ar_len, wb_ddr_ar_len);
        CONNECT(ddr3, 0, ddr3.i_ar_size, wb_ddr_ar_size);
        CONNECT(ddr3, 0, ddr3.i_ar_burst, wb_ddr_ar_burst);
        CONNECT(ddr3, 0, ddr3.i_ar_lock, w_ddr_ar_lock);
        CONNECT(ddr3, 0, ddr3.i_ar_cache, wb_ddr_ar_cache);
        CONNECT(ddr3, 0, ddr3.i_ar_prot, wb_ddr_ar_prot);
        CONNECT(ddr3, 0, ddr3.i_ar_qos, wb_ddr_ar_qos);
        CONNECT(ddr3, 0, ddr3.i_ar_valid, w_ddr_ar_valid);
        CONNECT(ddr3, 0, ddr3.o_ar_ready, w_ddr_ar_ready);
        CONNECT(ddr3, 0, ddr3.i_r_ready, w_ddr_r_ready);
        CONNECT(ddr3, 0, ddr3.o_r_id, wb_ddr_r_id);
        CONNECT(ddr3, 0, ddr3.o_r_data, wb_ddr_r_data);
        CONNECT(ddr3, 0, ddr3.o_r_resp, wb_ddr_r_resp);
        CONNECT(ddr3, 0, ddr3.o_r_last, w_ddr_r_last);
        CONNECT(ddr3, 0, ddr3.o_r_valid, w_ddr_r_valid);
        CONNECT(ddr3, 0, ddr3.i_sr_req, w_ddr_app_sr_req);
        CONNECT(ddr3, 0, ddr3.i_ref_req, w_ddr_app_ref_req);
        CONNECT(ddr3, 0, ddr3.i_zq_req, w_ddr_app_zq_req);
        CONNECT(ddr3, 0, ddr3.o_sr_active, w_ddr_app_sr_active);
        CONNECT(ddr3, 0, ddr3.o_ref_ack, w_ddr_app_ref_ack);
        CONNECT(ddr3, 0, ddr3.o_zq_ack, w_ddr_app_zq_ack);
    ENDNEW();
}

