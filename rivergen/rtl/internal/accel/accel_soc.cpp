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

#include "accel_soc.h"

accel_soc::accel_soc(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "accel_soc", name, comment),
    // simulation parameters
    sim_uart_speedup_rate(this, "sim_uart_speedup_rate", "0", "simulation UART speed-up: 0=no speed up, 1=2x, 2=4x, etc"),
    // Generic parameters
    // Ports
    i_sys_nrst(this, "i_sys_nrst", "1", "Power-on system reset active LOW"),
    i_sys_clk(this, "i_sys_clk", "1", "System Bus (AXI) clock"),
    i_cpu_nrst(this, "i_cpu_nrst", "1", "CPUs/Groups reset active LOW"),
    i_cpu_clk(this, "i_cpu_clk", "1", "CPUs/Groups clock"),
    i_apb_nrst(this, "i_apb_nrst", "1", "APB sub-system reset: active LOW"),
    i_apb_clk(this, "i_apb_clk", "1", "APB Bus clock"),
    i_dbg_nrst(this, "i_dbg_nrst", "1", "Reset from Debug interface (DMI). Reset everything except DMI"),
    i_ddr_nrst(this, "i_ddr_nrst", "1", "DDR related logic reset (AXI clock transformator)"),
    i_ddr_clk(this, "i_ddr_clk", "1", "DDR memoru clock"),
    _gpio0_(this, "GPIO signals:"),
    i_gpio(this, "i_gpio", "12"),
    o_gpio(this, "o_gpio", "12"),
    o_gpio_dir(this, "o_gpio_dir", "12"),
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
    _i2c0_(this, "I2C master interface for HDMI transmitter"),
    o_i2c0_scl(this, "o_i2c0_scl", "1", "Clock up to 400 KHz. Default 100 KHz"),
    o_i2c0_sda(this, "o_i2c0_sda", "1", "I2C output data"),
    o_i2c0_sda_dir(this, "o_i2c0_sda_dir", "1", "output data tri-stte buffer control"),
    i_i2c0_sda(this, "i_i2c0_sda", "1", "I2C input data"),
    o_i2c0_nreset(this, "o_i2c0_nreset", "1", "I2C slave reset. PCA9548 I2C mux must be de-asserted."),
    i_hdmi_nrst(this, "i_hdmi_nrst", "1", "Reset: active LOW. Must be HIGH only after DDR "),
    i_hdmi_clk(this, "i_hdmi_clk", "1", "HDMI Clock depends on resolution: for 1366x768@60Hz is ~83 MHz"),
    o_hdmi_hsync(this, "o_hdmi_hsync", "1", "Horizontal sync. strob"),
    o_hdmi_vsync(this, "o_hdmi_vsync", "1", "Vertical sync. strob"),
    o_hdmi_de(this, "o_hdmi_de", "1", "Data enable strob"),
    o_hdmi_d(this, "o_hdmi_d", "18", "Data in format YCbCr 16-bits"),
    o_hdmi_spdif(this, "o_hdmi_spdif", "1", "Sound channel output"),
    i_hdmi_spdif_out(this, "i_hdmi_spdif_out", "1", "Reverse sound channel"),
    i_hdmi_int(this, "i_hdmi_int", "1", "External interrupt from HDMI transmitter"),
    _prci0_(this, "PLL and Reset interfaces:"),
    o_dmreset(this, "o_dmreset", "1", "Debug reset request. Everything except DMI."),
    o_prci_pmapinfo(this, "o_prci_pmapinfo", "PRCI mapping information"),
    i_prci_pdevcfg(this, "i_prci_pdevcfg", "PRCI device descriptor"),
    o_prci_apbi(this, "o_prci_apbi", "APB: PLL and Reset configuration interface"),
    i_prci_apbo(this, "i_prci_apbo", "APB: PLL and Reset configuration interface"),
    _ddr0_(this, "DDR interfaces:"),
    o_ddr_aw_id(this, "o_ddr_aw_id", "CFG_SYSBUS_ID_BITS", NO_COMMENT),
    o_ddr_aw_addr(this, "o_ddr_aw_addr", "30", "1 GB by default"),
    o_ddr_aw_len(this, "o_ddr_aw_len", "8", NO_COMMENT),
    o_ddr_aw_size(this, "o_ddr_aw_size", "3", NO_COMMENT),
    o_ddr_aw_burst(this, "o_ddr_aw_burst", "2", NO_COMMENT),
    o_ddr_aw_lock(this, "o_ddr_aw_lock", "1", NO_COMMENT),
    o_ddr_aw_cache(this, "o_ddr_aw_cache", "4", NO_COMMENT),
    o_ddr_aw_prot(this, "o_ddr_aw_prot", "3", NO_COMMENT),
    o_ddr_aw_qos(this, "o_ddr_aw_qos", "4", NO_COMMENT),
    o_ddr_aw_valid(this, "o_ddr_aw_valid", "1", NO_COMMENT),
    i_ddr_aw_ready(this, "i_ddr_aw_ready", "1", NO_COMMENT),
    o_ddr_w_data(this, "o_ddr_w_data", "64", NO_COMMENT),
    o_ddr_w_strb(this, "o_ddr_w_strb", "8", NO_COMMENT),
    o_ddr_w_last(this, "o_ddr_w_last", "1", NO_COMMENT),
    o_ddr_w_valid(this, "o_ddr_w_valid", "1", NO_COMMENT),
    i_ddr_w_ready(this, "i_ddr_w_ready", "1", NO_COMMENT),
    o_ddr_b_ready(this, "o_ddr_b_ready", "1", NO_COMMENT),
    i_ddr_b_id(this, "i_ddr_b_id", "CFG_SYSBUS_ID_BITS", NO_COMMENT),
    i_ddr_b_resp(this, "i_ddr_b_resp", "2", NO_COMMENT),
    i_ddr_b_valid(this, "i_ddr_b_valid", "1", NO_COMMENT),
    o_ddr_ar_id(this, "o_ddr_ar_id", "CFG_SYSBUS_ID_BITS", NO_COMMENT),
    o_ddr_ar_addr(this, "o_ddr_ar_addr", "30", "1 GB by default"),
    o_ddr_ar_len(this, "o_ddr_ar_len", "8", NO_COMMENT),
    o_ddr_ar_size(this, "o_ddr_ar_size", "3", NO_COMMENT),
    o_ddr_ar_burst(this, "o_ddr_ar_burst", "2", NO_COMMENT),
    o_ddr_ar_lock(this, "o_ddr_ar_lock", "1", NO_COMMENT),
    o_ddr_ar_cache(this, "o_ddr_ar_cache", "4", NO_COMMENT),
    o_ddr_ar_prot(this, "o_ddr_ar_prot", "3", NO_COMMENT),
    o_ddr_ar_qos(this, "o_ddr_ar_qos", "4", NO_COMMENT),
    o_ddr_ar_valid(this, "o_ddr_ar_valid", "1", NO_COMMENT),
    i_ddr_ar_ready(this, "i_ddr_ar_ready", "1", NO_COMMENT),
    o_ddr_r_ready(this, "o_ddr_r_ready", "1", NO_COMMENT),
    i_ddr_r_id(this, "i_ddr_r_id", "CFG_SYSBUS_ID_BITS", NO_COMMENT),
    i_ddr_r_data(this, "i_ddr_r_data", "64", NO_COMMENT),
    i_ddr_r_resp(this, "i_ddr_r_resp", "2", NO_COMMENT),
    i_ddr_r_last(this, "i_ddr_r_last", "1", NO_COMMENT),
    i_ddr_r_valid(this, "i_ddr_r_valid", "1", NO_COMMENT),
    i_ddr_app_init_calib_done(this, "i_ddr_app_init_calib_done", "1", "DDR initital calibration done"),
    i_ddr_app_temp(this, "i_ddr_app_temp", "12", "Device temperature"),
    o_ddr_app_sr_req(this, "o_ddr_app_sr_req", "1", "Self-refresh request (low-power mode)"),
    o_ddr_app_ref_req(this, "o_ddr_app_ref_req", "1", "Periodic refresh request ~7.8 us"),
    o_ddr_app_zq_req(this, "o_ddr_app_zq_req", "1", "ZQ calibration request. Startup and runtime maintenance"),
    i_ddr_app_sr_active(this, "i_ddr_app_sr_active", "1", "Self-resfresh is active (low-power mode or sleep)"),
    i_ddr_app_ref_ack(this, "i_ddr_app_ref_ack", "1", "Refresh request acknowledged"),
    i_ddr_app_zq_ack(this, "i_ddr_app_zq_ack", "1", "ZQ calibration request acknowledged"),
    _pcie0_(this, "PCIE endpoint interface:"),
    i_pcie_clk(this, "i_pcie_clk", "1", "PCIE clock generated by end-point"),
    i_pcie_nrst(this, "i_pcie_nrst", "1", "PCIE reset active LOW generated inside of PRCI"),
    i_pcie_completer_id(this, "i_pcie_completer_id", "16", "Bus, Device, Function"),
    o_pcie_dmao(this, "o_pcie_dmao", "PCIE DMA enging interface"),
    i_pcie_dmai(this, "i_pcie_dmai", "PCIE DMA enging interface"),
    // param
    _hwid0_(this),
    _hwid1_(this, "Hardware SoC Identificator."),
    _hwid2_(this, "Read Only unique platform identificator that could be read by FW"),
    SOC_HW_ID(this, "SOC_HW_ID", "32", GENCFG_SOC_HW_ID, NO_COMMENT),
    _cfg0_(this),
    _cfg1_(this, "UARTx fifo log2(size) in bytes:"),
    SOC_UART1_LOG2_FIFOSZ(this, "SOC_UART1_LOG2_FIFOSZ", "4"),
    _cfg2_(this),
    _cfg3_(this, "Number of available generic IO pins:"),
    SOC_GPIO0_WIDTH(this, "SOC_GPIO0_WIDTH", "12"),
    _plic0_(this),
    _plic1_(this, "Number of contexts in PLIC controller."),
    _plic2_(this, "Example FU740: S7 Core0 (M) + 4xU74 Cores (M+S)."),
    SOC_PLIC_CONTEXT_TOTAL(this, "SOC_PLIC_CONTEXT_TOTAL", "9"),
    _plic3_(this, "Any number up to 1024. Zero interrupt must be 0."),
    SOC_PLIC_IRQ_TOTAL(this, "SOC_PLIC_IRQ_TOTAL", "73"),
    // Singals:
    acpo(this, "acpo", NO_COMMENT),
    acpi(this, "acpi", NO_COMMENT),
    bus0_mapinfo(this, "bus0_mapinfo"),
    aximi(this, "aximi"),
    aximo(this, "aximo"),
    axisi(this, "axisi"),
    axiso(this, "axiso"),
    bus1_mapinfo(this, "bus1_mapinfo"),
    apbi(this, "apbi"),
    apbo(this, "apbo"),
    dev_pnp(this, "dev_pnp", NO_COMMENT),
    wb_group0_xmsto(this, "wb_group0_xmsto", "CPU clock domain"),
    wb_group0_xmsti(this, "wb_group0_xmsti", "CPU clock domain"),
    wb_ddr_xslvi(this, "wb_ddr_xslvi", "DDR clock domain"),
    wb_ddr_xslvo(this, "wb_ddr_xslvo", "DDR clock domain"),
    wb_pbridge_xslvi(this, "wb_pbridge_xslvi", "APB clock domain"),
    wb_pbridge_xslvo(this, "wb_pbridge_xslvo", "APB clock domain"),
    wb_clint_mtimer(this, "wb_clint_mtimer", "64"),
    wb_clint_msip(this, "wb_clint_msip", "CFG_CPU_MAX"),
    wb_clint_mtip(this, "wb_clint_mtip", "CFG_CPU_MAX"),
    wb_plic_xeip(this, "wb_plic_xeip", "SOC_PLIC_CONTEXT_TOTAL"),
    wb_plic_meip(this, "wb_plic_meip", "CFG_CPU_MAX"),
    wb_plic_seip(this, "wb_plic_seip", "CFG_CPU_MAX"),
    w_irq_uart1(this, "w_irq_uart1", "1"),
    wb_irq_gpio(this, "wb_irq_gpio", "SOC_GPIO0_WIDTH"),
    w_irq_pnp(this, "w_irq_pnp", "1"),
    wb_ext_irqs(this, "wb_ext_irqs", "SOC_PLIC_IRQ_TOTAL"),
    wb_pcie_dma_state(this, "wb_pcie_dma_state", "4"),
    w_dbg_valid(this, "w_dbg_valid", "1"),
    w_dbg_payload(this, "w_dbg_payload", "64"),
    w_irq_i2c0(this, "w_irq_i2c0", "1"),
    // submodules:
    bus0(this, "bus0", NO_COMMENT),
    bus1(this, "bus1", NO_COMMENT),
    rom0(this, "rom0"),
    sram0(this, "sram0"),
    clint0(this, "clint0"),
    plic0(this, "plic0"),
    uart1(this, "uart1"),
    gpio0(this, "gpio0"),
    pddr0(this, "pddr0", NO_COMMENT),
    i2c0(this, "i2c0", NO_COMMENT),
    hdmi0(this, "hdmi0", NO_COMMENT),
    pcidma0(this, "pcidma0"),
    ppcie0(this, "ppcie0", NO_COMMENT),
    pnp0(this, "pnp0"),
    group0(this, "group0"),
    afifo_ddr0(this, "afifo_ddr0", NO_COMMENT),
    afifo_apb0(this, "afifo_apb0", NO_COMMENT),
    afifo_group0(this, "afifo_group0", NO_COMMENT),
    comb(this)
{
    Operation::start(this);

    // Create and connet Sub-modules:
    NEW(bus0, bus0.getName().c_str());
        CONNECT(bus0, 0, bus0.i_clk, i_sys_clk);
        CONNECT(bus0, 0, bus0.i_nrst, i_sys_nrst);
        CONNECT(bus0, 0, bus0.o_cfg, ARRITEM(dev_pnp, *SCV_get_cfg_type(this, "SOC_PNP_XCTRL0"), dev_pnp));
        CONNECT(bus0, 0, bus0.i_xmsto, aximo);
        CONNECT(bus0, 0, bus0.o_xmsti, aximi);
        CONNECT(bus0, 0, bus0.i_xslvo, axiso);
        CONNECT(bus0, 0, bus0.o_xslvi, axisi);
        CONNECT(bus0, 0, bus0.o_mapinfo, bus0_mapinfo);
    ENDNEW();

TEXT();
    NEW(bus1, bus1.getName().c_str());
        CONNECT(bus1, 0, bus1.i_clk, i_apb_clk);
        CONNECT(bus1, 0, bus1.i_nrst, i_apb_nrst);
        CONNECT(bus1, 0, bus1.i_mapinfo, ARRITEM(bus0_mapinfo, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_PBRIDGE"), bus0_mapinfo));
        CONNECT(bus1, 0, bus1.o_cfg, ARRITEM(dev_pnp, *SCV_get_cfg_type(this, "SOC_PNP_PBRIDGE0"), dev_pnp));
        CONNECT(bus1, 0, bus1.i_xslvi, wb_pbridge_xslvi);
        CONNECT(bus1, 0, bus1.o_xslvo, wb_pbridge_xslvo);
        CONNECT(bus1, 0, bus1.i_apbo, apbo);
        CONNECT(bus1, 0, bus1.o_apbi, apbi);
        CONNECT(bus1, 0, bus1.o_mapinfo, bus1_mapinfo);
    ENDNEW();

    TEXT();
    afifo_group0.abits_depth.setObjValue(new DecConst(2));
    afifo_group0.dbits_depth.setObjValue(new DecConst(3));
    NEW(afifo_group0, afifo_group0.getName().c_str());
        CONNECT(afifo_group0, 0, afifo_group0.i_xmst_nrst, i_cpu_nrst);
        CONNECT(afifo_group0, 0, afifo_group0.i_xmst_clk, i_cpu_clk);
        CONNECT(afifo_group0, 0, afifo_group0.i_xmsto, wb_group0_xmsto);
        CONNECT(afifo_group0, 0, afifo_group0.o_xmsti, wb_group0_xmsti);
        CONNECT(afifo_group0, 0, afifo_group0.i_xslv_nrst, i_sys_nrst);
        CONNECT(afifo_group0, 0, afifo_group0.i_xslv_clk, i_sys_clk);
        CONNECT(afifo_group0, 0, afifo_group0.o_xslvi, ARRITEM(aximo, *SCV_get_cfg_type(this, "CFG_BUS0_XMST_GROUP0"), aximo));
        CONNECT(afifo_group0, 0, afifo_group0.i_xslvo, ARRITEM(aximi, *SCV_get_cfg_type(this, "CFG_BUS0_XMST_GROUP0"), aximi));
    ENDNEW();

    TEXT();
    afifo_apb0.abits_depth.setObjValue(new DecConst(2));
    afifo_apb0.dbits_depth.setObjValue(new DecConst(2));
    NEW(afifo_apb0, afifo_apb0.getName().c_str());
        CONNECT(afifo_apb0, 0, afifo_apb0.i_xslv_nrst, i_sys_nrst);
        CONNECT(afifo_apb0, 0, afifo_apb0.i_xslv_clk, i_sys_clk);
        CONNECT(afifo_apb0, 0, afifo_apb0.i_xslvi, ARRITEM(axisi, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_PBRIDGE"), axisi));
        CONNECT(afifo_apb0, 0, afifo_apb0.o_xslvo, ARRITEM(axiso, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_PBRIDGE"), axiso));
        CONNECT(afifo_apb0, 0, afifo_apb0.i_xmst_nrst, i_apb_nrst);
        CONNECT(afifo_apb0, 0, afifo_apb0.i_xmst_clk, i_apb_clk);
        CONNECT(afifo_apb0, 0, afifo_apb0.o_xmsto, wb_pbridge_xslvi);
        CONNECT(afifo_apb0, 0, afifo_apb0.i_xmsti, wb_pbridge_xslvo);
    ENDNEW();

    TEXT();
    afifo_ddr0.abits_depth.setObjValue(new DecConst(2));
    afifo_ddr0.dbits_depth.setObjValue(new DecConst(9));
    NEW(afifo_ddr0, afifo_ddr0.getName().c_str());
        CONNECT(afifo_ddr0, 0, afifo_ddr0.i_xslv_nrst, i_sys_nrst);
        CONNECT(afifo_ddr0, 0, afifo_ddr0.i_xslv_clk, i_sys_clk);
        CONNECT(afifo_ddr0, 0, afifo_ddr0.i_xslvi, ARRITEM(axisi, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_DDR"), axisi));
        CONNECT(afifo_ddr0, 0, afifo_ddr0.o_xslvo, ARRITEM(axiso, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_DDR"), axiso));
        CONNECT(afifo_ddr0, 0, afifo_ddr0.i_xmst_nrst, i_ddr_nrst);
        CONNECT(afifo_ddr0, 0, afifo_ddr0.i_xmst_clk, i_ddr_clk);
        CONNECT(afifo_ddr0, 0, afifo_ddr0.o_xmsto, wb_ddr_xslvi);
        CONNECT(afifo_ddr0, 0, afifo_ddr0.i_xmsti, wb_ddr_xslvo);
    ENDNEW();

    TEXT();
    group0.cpu_num.setObjValue(SCV_get_cfg_type(this, "CFG_CPU_NUM"));
    group0.l2cache_ena.setObjValue(SCV_get_cfg_type(this, "CFG_L2CACHE_ENA"));
    NEW(group0, group0.getName().c_str());
        CONNECT(group0, 0, group0.i_clk, i_cpu_clk);
        CONNECT(group0, 0, group0.i_cores_nrst, i_cpu_nrst);
        CONNECT(group0, 0, group0.i_dmi_nrst, i_dbg_nrst);
        CONNECT(group0, 0, group0.i_trst, i_jtag_trst);
        CONNECT(group0, 0, group0.i_tck, i_jtag_tck);
        CONNECT(group0, 0, group0.i_tms, i_jtag_tms);
        CONNECT(group0, 0, group0.i_tdi, i_jtag_tdi);
        CONNECT(group0, 0, group0.o_tdo, o_jtag_tdo);
        CONNECT(group0, 0, group0.i_msip, wb_clint_msip);
        CONNECT(group0, 0, group0.i_mtip, wb_clint_mtip);
        CONNECT(group0, 0, group0.i_meip, wb_plic_meip);
        CONNECT(group0, 0, group0.i_seip, wb_plic_seip);
        CONNECT(group0, 0, group0.i_mtimer, wb_clint_mtimer);
        CONNECT(group0, 0, group0.i_acpo, acpo);
        CONNECT(group0, 0, group0.o_acpi, acpi);
        CONNECT(group0, 0, group0.o_xmst_cfg, ARRITEM(dev_pnp, *SCV_get_cfg_type(this, "SOC_PNP_GROUP0"), dev_pnp));
        CONNECT(group0, 0, group0.i_msti, wb_group0_xmsti);
        CONNECT(group0, 0, group0.o_msto, wb_group0_xmsto);
        CONNECT(group0, 0, group0.i_dmi_mapinfo, ARRITEM(bus1_mapinfo, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_DMI"), bus1_mapinfo));
        CONNECT(group0, 0, group0.o_dmi_cfg, ARRITEM(dev_pnp, *SCV_get_cfg_type(this, "SOC_PNP_DMI"), dev_pnp));
        CONNECT(group0, 0, group0.i_dmi_apbi, ARRITEM(apbi, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_DMI"), apbi));
        CONNECT(group0, 0, group0.o_dmi_apbo, ARRITEM(apbo, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_DMI"), apbo));
        CONNECT(group0, 0, group0.o_dmreset, o_dmreset);
    ENDNEW();


    TEXT();
    rom0.abits.setObjValue(SCV_get_cfg_type(this, "CFG_BOOTROM_LOG2_SIZE"));
    rom0.filename.setObjValue(SCV_get_cfg_type(this, "CFG_BOOTROM_FILE_HEX"));
    NEW(rom0, rom0.getName().c_str());
        CONNECT(rom0, 0, rom0.i_clk, i_sys_clk);
        CONNECT(rom0, 0, rom0.i_nrst, i_sys_nrst);
        CONNECT(rom0, 0, rom0.i_mapinfo, ARRITEM(bus0_mapinfo, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_BOOTROM"), bus0_mapinfo));
        CONNECT(rom0, 0, rom0.o_cfg, ARRITEM(dev_pnp, *SCV_get_cfg_type(this, "SOC_PNP_BOOTROM"), dev_pnp));
        CONNECT(rom0, 0, rom0.i_xslvi, ARRITEM(axisi, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_BOOTROM"), axisi));
        CONNECT(rom0, 0, rom0.o_xslvo, ARRITEM(axiso, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_BOOTROM"), axiso));
    ENDNEW();

TEXT();
    sram0.abits.setObjValue(SCV_get_cfg_type(this, "CFG_SRAM_LOG2_SIZE"));
    NEW(sram0, sram0.getName().c_str());
        CONNECT(sram0, 0, sram0.i_clk, i_sys_clk);
        CONNECT(sram0, 0, sram0.i_nrst, i_sys_nrst);
        CONNECT(sram0, 0, sram0.i_mapinfo, ARRITEM(bus0_mapinfo, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_SRAM"), bus0_mapinfo));
        CONNECT(sram0, 0, sram0.o_cfg, ARRITEM(dev_pnp, *SCV_get_cfg_type(this, "SOC_PNP_SRAM"), dev_pnp));
        CONNECT(sram0, 0, sram0.i_xslvi, ARRITEM(axisi, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_SRAM"), axisi));
        CONNECT(sram0, 0, sram0.o_xslvo, ARRITEM(axiso, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_SRAM"), axiso));
    ENDNEW();

TEXT();
    plic0.ctxmax.setObjValue(&SOC_PLIC_CONTEXT_TOTAL);
    clint0.cpu_total.setObjValue(SCV_get_cfg_type(this, "CFG_CPU_MAX"));
    NEW(clint0, clint0.getName().c_str());
        CONNECT(clint0, 0, clint0.i_clk, i_sys_clk);
        CONNECT(clint0, 0, clint0.i_nrst, i_sys_nrst);
        CONNECT(clint0, 0, clint0.i_mapinfo, ARRITEM(bus0_mapinfo, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_CLINT"), bus0_mapinfo));
        CONNECT(clint0, 0, clint0.o_cfg, ARRITEM(dev_pnp, *SCV_get_cfg_type(this, "SOC_PNP_CLINT"), dev_pnp));
        CONNECT(clint0, 0, clint0.i_xslvi, ARRITEM(axisi, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_CLINT"), axisi));
        CONNECT(clint0, 0, clint0.o_xslvo, ARRITEM(axiso, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_CLINT"), axiso));
        CONNECT(clint0, 0, clint0.o_mtimer, wb_clint_mtimer);
        CONNECT(clint0, 0, clint0.o_msip, wb_clint_msip);
        CONNECT(clint0, 0, clint0.o_mtip, wb_clint_mtip);
    ENDNEW();

TEXT();
    plic0.irqmax.setObjValue(&SOC_PLIC_IRQ_TOTAL);
    NEW(plic0, plic0.getName().c_str());
        CONNECT(plic0, 0, plic0.i_clk, i_sys_clk);
        CONNECT(plic0, 0, plic0.i_nrst, i_sys_nrst);
        CONNECT(plic0, 0, plic0.i_mapinfo, ARRITEM(bus0_mapinfo, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_PLIC"), bus0_mapinfo));
        CONNECT(plic0, 0, plic0.o_cfg, ARRITEM(dev_pnp, *SCV_get_cfg_type(this, "SOC_PNP_PLIC"), dev_pnp));
        CONNECT(plic0, 0, plic0.i_xslvi, ARRITEM(axisi, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_PLIC"), axisi));
        CONNECT(plic0, 0, plic0.o_xslvo, ARRITEM(axiso, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_PLIC"), axiso));
        CONNECT(plic0, 0, plic0.i_irq_request, wb_ext_irqs);
        CONNECT(plic0, 0, plic0.o_ip, wb_plic_xeip);
    ENDNEW();

TEXT();
    uart1.log2_fifosz.setObjValue(&SOC_UART1_LOG2_FIFOSZ);
    uart1.sim_speedup_rate.setObjValue(&sim_uart_speedup_rate);
    NEW(uart1, uart1.getName().c_str());
        CONNECT(uart1, 0, uart1.i_clk, i_apb_clk);
        CONNECT(uart1, 0, uart1.i_nrst, i_apb_nrst);
        CONNECT(uart1, 0, uart1.i_mapinfo, ARRITEM(bus1_mapinfo, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_UART1"), bus1_mapinfo));
        CONNECT(uart1, 0, uart1.o_cfg, ARRITEM(dev_pnp, *SCV_get_cfg_type(this, "SOC_PNP_UART1"), dev_pnp));
        CONNECT(uart1, 0, uart1.i_apbi, ARRITEM(apbi, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_UART1"), apbi));
        CONNECT(uart1, 0, uart1.o_apbo, ARRITEM(apbo, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_UART1"), apbo));
        CONNECT(uart1, 0, uart1.i_rd, i_uart1_rd);
        CONNECT(uart1, 0, uart1.o_td, o_uart1_td);
        CONNECT(uart1, 0, uart1.o_irq, w_irq_uart1);
    ENDNEW();

TEXT();
    gpio0.width.setObjValue(&SOC_GPIO0_WIDTH);
    NEW(gpio0, gpio0.getName().c_str());
        CONNECT(gpio0, 0, gpio0.i_clk, i_apb_clk);
        CONNECT(gpio0, 0, gpio0.i_nrst, i_apb_nrst);
        CONNECT(gpio0, 0, gpio0.i_mapinfo, ARRITEM(bus1_mapinfo, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_GPIO"), bus1_mapinfo));
        CONNECT(gpio0, 0, gpio0.o_cfg, ARRITEM(dev_pnp, *SCV_get_cfg_type(this, "SOC_PNP_GPIO"), dev_pnp));
        CONNECT(gpio0, 0, gpio0.i_apbi, ARRITEM(apbi, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_GPIO"), apbi));
        CONNECT(gpio0, 0, gpio0.o_apbo, ARRITEM(apbo, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_GPIO"), apbo));
        CONNECT(gpio0, 0, gpio0.i_gpio, i_gpio);
        CONNECT(gpio0, 0, gpio0.o_gpio_dir, o_gpio_dir);
        CONNECT(gpio0, 0, gpio0.o_gpio, o_gpio);
        CONNECT(gpio0, 0, gpio0.o_irq, wb_irq_gpio);
    ENDNEW();

    TEXT();
    NEW(pddr0, pddr0.getName().c_str());
        CONNECT(pddr0, 0, pddr0.i_apb_nrst, i_apb_nrst);
        CONNECT(pddr0, 0, pddr0.i_apb_clk, i_apb_clk);
        CONNECT(pddr0, 0, pddr0.i_ddr_nrst, i_ddr_nrst);
        CONNECT(pddr0, 0, pddr0.i_ddr_clk, i_ddr_clk);
        CONNECT(pddr0, 0, pddr0.i_mapinfo, ARRITEM(bus1_mapinfo, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_DDR"), bus1_mapinfo));
        CONNECT(pddr0, 0, pddr0.o_cfg, ARRITEM(dev_pnp, *SCV_get_cfg_type(this, "SOC_PNP_DDR_APB"), dev_pnp));
        CONNECT(pddr0, 0, pddr0.i_apbi, ARRITEM(apbi, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_DDR"), apbi));
        CONNECT(pddr0, 0, pddr0.o_apbo, ARRITEM(apbo, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_DDR"), apbo));
        CONNECT(pddr0, 0, pddr0.i_init_calib_done, i_ddr_app_init_calib_done);
        CONNECT(pddr0, 0, pddr0.i_device_temp, i_ddr_app_temp);
        CONNECT(pddr0, 0, pddr0.o_sr_req, o_ddr_app_sr_req);
        CONNECT(pddr0, 0, pddr0.o_ref_req, o_ddr_app_ref_req);
        CONNECT(pddr0, 0, pddr0.o_zq_req, o_ddr_app_zq_req);
        CONNECT(pddr0, 0, pddr0.i_sr_active, i_ddr_app_sr_active);
        CONNECT(pddr0, 0, pddr0.i_ref_ack, i_ddr_app_ref_ack);
        CONNECT(pddr0, 0, pddr0.i_zq_ack, i_ddr_app_zq_ack);
    ENDNEW();

    TEXT();
    NEW(i2c0, i2c0.getName().c_str());
        CONNECT(i2c0, 0, i2c0.i_clk, i_sys_clk);
        CONNECT(i2c0, 0, i2c0.i_nrst, i_sys_nrst);
        CONNECT(i2c0, 0, i2c0.i_mapinfo, ARRITEM(bus1_mapinfo, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_I2C0"), bus1_mapinfo));
        CONNECT(i2c0, 0, i2c0.o_cfg, ARRITEM(dev_pnp, *SCV_get_cfg_type(this, "SOC_PNP_I2C"), dev_pnp));
        CONNECT(i2c0, 0, i2c0.i_apbi, ARRITEM(apbi, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_I2C0"), apbi));
        CONNECT(i2c0, 0, i2c0.o_apbo, ARRITEM(apbo, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_I2C0"), apbo));
        CONNECT(i2c0, 0, i2c0.o_scl, o_i2c0_scl);
        CONNECT(i2c0, 0, i2c0.o_sda, o_i2c0_sda);
        CONNECT(i2c0, 0, i2c0.o_sda_dir, o_i2c0_sda_dir);
        CONNECT(i2c0, 0, i2c0.i_sda, i_i2c0_sda);
        CONNECT(i2c0, 0, i2c0.o_irq, w_irq_i2c0);
        CONNECT(i2c0, 0, i2c0.o_nreset, o_i2c0_nreset);
    ENDNEW();

    TEXT();
    NEW(hdmi0, hdmi0.getName().c_str());
        CONNECT(hdmi0, 0, hdmi0.i_nrst, i_sys_nrst);
        CONNECT(hdmi0, 0, hdmi0.i_clk, i_sys_clk);
        CONNECT(hdmi0, 0, hdmi0.i_hdmi_nrst, i_hdmi_nrst);
        CONNECT(hdmi0, 0, hdmi0.i_hdmi_clk, i_hdmi_clk);
        CONNECT(hdmi0, 0, hdmi0.o_hsync, o_hdmi_hsync);
        CONNECT(hdmi0, 0, hdmi0.o_vsync, o_hdmi_vsync);
        CONNECT(hdmi0, 0, hdmi0.o_de, o_hdmi_de);
        CONNECT(hdmi0, 0, hdmi0.o_data, o_hdmi_d);
        CONNECT(hdmi0, 0, hdmi0.o_spdif, o_hdmi_spdif);
        CONNECT(hdmi0, 0, hdmi0.i_spdif_out, i_hdmi_spdif_out);
        CONNECT(hdmi0, 0, hdmi0.i_irq, i_hdmi_int);
        CONNECT(hdmi0, 0, hdmi0.o_xmst_cfg, ARRITEM(dev_pnp, *SCV_get_cfg_type(this, "SOC_PNP_HDMI_DMA"), dev_pnp));
        CONNECT(hdmi0, 0, hdmi0.i_xmsti, ARRITEM(aximi, *SCV_get_cfg_type(this, "CFG_BUS0_XMST_HDMI"), aximi));
        CONNECT(hdmi0, 0, hdmi0.o_xmsto, ARRITEM(aximo, *SCV_get_cfg_type(this, "CFG_BUS0_XMST_HDMI"), aximo));
    ENDNEW();

    TEXT();
    TEXT("See reference: pg054-7series-pcie.pdf");
    NEW(pcidma0, pcidma0.getName().c_str());
        CONNECT(pcidma0, 0, pcidma0.i_nrst, i_pcie_nrst);
        CONNECT(pcidma0, 0, pcidma0.i_clk, i_sys_clk);
        CONNECT(pcidma0, 0, pcidma0.i_pcie_phy_clk, i_pcie_clk);
        CONNECT(pcidma0, 0, pcidma0.i_pcie_dmai, i_pcie_dmai);
        CONNECT(pcidma0, 0, pcidma0.o_pcie_dmao, o_pcie_dmao);
        CONNECT(pcidma0, 0, pcidma0.i_pcie_completer_id, i_pcie_completer_id);
        CONNECT(pcidma0, 0, pcidma0.o_xmst_cfg, ARRITEM(dev_pnp, *SCV_get_cfg_type(this, "SOC_PNP_PCIE_DMA"), dev_pnp));
        CONNECT(pcidma0, 0, pcidma0.i_xmsti, ARRITEM(aximi, *SCV_get_cfg_type(this, "CFG_BUS0_XMST_PCIE"), aximi));
        CONNECT(pcidma0, 0, pcidma0.o_xmsto, ARRITEM(aximo, *SCV_get_cfg_type(this, "CFG_BUS0_XMST_PCIE"), aximo));
        CONNECT(pcidma0, 0, pcidma0.o_dma_state, wb_pcie_dma_state);
        CONNECT(pcidma0, 0, pcidma0.o_dbg_valid, w_dbg_valid);
        CONNECT(pcidma0, 0, pcidma0.o_dbg_payload, w_dbg_payload);
    ENDNEW();

TEXT();
    NEW(ppcie0, ppcie0.getName().c_str());
        CONNECT(ppcie0, 0, ppcie0.i_clk, i_apb_clk);
        CONNECT(ppcie0, 0, ppcie0.i_nrst, i_apb_nrst);
        CONNECT(ppcie0, 0, ppcie0.i_mapinfo, ARRITEM(bus1_mapinfo, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_PCIE"), bus1_mapinfo));
        CONNECT(ppcie0, 0, ppcie0.o_cfg, ARRITEM(dev_pnp, *SCV_get_cfg_type(this, "SOC_PNP_PCIE_APB"), dev_pnp));
        CONNECT(ppcie0, 0, ppcie0.i_apbi, ARRITEM(apbi, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_PCIE"), apbi));
        CONNECT(ppcie0, 0, ppcie0.o_apbo, ARRITEM(apbo, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_PCIE"), apbo));
        CONNECT(ppcie0, 0, ppcie0.i_pcie_completer_id, i_pcie_completer_id);
        CONNECT(ppcie0, 0, ppcie0.i_dma_state, wb_pcie_dma_state);
        CONNECT(ppcie0, 0, ppcie0.i_dbg_valid, w_dbg_valid);
        CONNECT(ppcie0, 0, ppcie0.i_dbg_payload, w_dbg_payload);
    ENDNEW();

TEXT();
    pnp0.cfg_slots.setObjValue(SCV_get_cfg_type(this, "SOC_PNP_TOTAL"));
    pnp0.hw_id.setObjValue(&SOC_HW_ID);
    pnp0.cpu_max.setObjValue(SCV_get_cfg_type(this, "CFG_CPU_NUM"));
    pnp0.l2cache_ena.setObjValue(SCV_get_cfg_type(this, "CFG_L2CACHE_ENA"));
    pnp0.plic_irq_max.setObjValue(&SOC_PLIC_IRQ_TOTAL);
    NEW(pnp0, pnp0.getName().c_str());
        CONNECT(pnp0, 0, pnp0.i_clk, i_apb_clk);
        CONNECT(pnp0, 0, pnp0.i_nrst, i_apb_nrst);
        CONNECT(pnp0, 0, pnp0.i_mapinfo, ARRITEM(bus1_mapinfo, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_PNP"), bus1_mapinfo));
        CONNECT(pnp0, 0, pnp0.i_cfg, dev_pnp);
        CONNECT(pnp0, 0, pnp0.o_cfg, ARRITEM(dev_pnp, *SCV_get_cfg_type(this, "SOC_PNP_PNP"), dev_pnp));
        CONNECT(pnp0, 0, pnp0.i_apbi, ARRITEM(apbi, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_PNP"), apbi));
        CONNECT(pnp0, 0, pnp0.o_apbo, ARRITEM(apbo, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_PNP"), apbo));
        CONNECT(pnp0, 0, pnp0.o_irq, w_irq_pnp);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void accel_soc::proc_comb() {
    TEXT();
    TEXT("assign interrupts:");
    SETBITS(comb.vb_ext_irqs, 22, 0, ALLZEROS());
    SETBITS(comb.vb_ext_irqs, DEC(ADD2(CONST("23"), SOC_GPIO0_WIDTH)),
                              CONST("23"),
                              wb_irq_gpio, "FU740: 16 bits, current 12-bits");
    SETBIT(comb.vb_ext_irqs, 39, w_irq_uart1);
    SETBIT(comb.vb_ext_irqs, 40, w_irq_i2c0);
    SETBITS(comb.vb_ext_irqs, 69, 41, ALLZEROS());
    SETBIT(comb.vb_ext_irqs, 70, w_irq_pnp);
    SETBITS(comb.vb_ext_irqs, DEC(SOC_PLIC_IRQ_TOTAL), CONST("71"), ALLZEROS());
    SETVAL(wb_ext_irqs, comb.vb_ext_irqs);

    TEXT();
    TEXT("FU740 implements 5 cores (we implement only 4):");
    TEXT("    Hart0 - M-mode only (S7 Core RV64IMAC)");
    TEXT("    Hart1..4 - M+S modes (U74 Cores RV64GC)");
    TEXT("Hart4 ignored");
    SETVAL(wb_plic_meip, CCx(4, &BIT(wb_plic_xeip, 5),
                                &BIT(wb_plic_xeip, 3),
                                &BIT(wb_plic_xeip, 1),
                                &BIT(wb_plic_xeip, 0)));
    SETVAL(wb_plic_seip, CCx(4, &BIT(wb_plic_xeip, 6),
                                &BIT(wb_plic_xeip, 4),
                                &BIT(wb_plic_xeip, 2),
                                &comb.v_gnd1));

    TEXT();
    SETONE(o_jtag_vref);

    TEXT();
    TEXT("Nullify emty AXI-slots:");
    SETVAL(acpo, *SCV_get_cfg_type(this, "axi4_master_out_none"));

    TEXT();
    TEXT("PRCI:");
    SETVAL(o_prci_apbi, ARRITEM(apbi, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_PRCI"), apbi));
    SETARRITEM(apbo, *SCV_get_cfg_type(this, "CFG_BUS1_PSLV_PRCI"), apbo, i_prci_apbo);
    SETARRITEM(dev_pnp, *SCV_get_cfg_type(this, "SOC_PNP_PRCI"), dev_pnp, i_prci_pdevcfg);

    TEXT();
    TEXT("DDR:");
    SETVAL(o_ddr_aw_id, wb_ddr_xslvi.aw_id);
    TEXT("Warning: we should do (addr - start_addr)[29:0], but BAR is 0x80000000");
    SETVAL(o_ddr_aw_addr, BITS(wb_ddr_xslvi.aw_bits.addr, 29, 0));
    SETVAL(o_ddr_aw_len, wb_ddr_xslvi.aw_bits.len);
    SETVAL(o_ddr_aw_size, wb_ddr_xslvi.aw_bits.size);
    SETVAL(o_ddr_aw_burst, wb_ddr_xslvi.aw_bits.burst);
    SETVAL(o_ddr_aw_lock, wb_ddr_xslvi.aw_bits.lock);
    SETVAL(o_ddr_aw_cache, wb_ddr_xslvi.aw_bits.cache);
    SETVAL(o_ddr_aw_prot, wb_ddr_xslvi.aw_bits.prot);
    SETVAL(o_ddr_aw_qos, wb_ddr_xslvi.aw_bits.qos);
    SETVAL(o_ddr_aw_valid, wb_ddr_xslvi.aw_valid);
    SETVAL(comb.vb_ddr_xslvo.aw_ready, i_ddr_aw_ready);
    SETVAL(o_ddr_w_data, wb_ddr_xslvi.w_data);
    SETVAL(o_ddr_w_strb, wb_ddr_xslvi.w_strb);
    SETVAL(o_ddr_w_last, wb_ddr_xslvi.w_last);
    SETVAL(o_ddr_w_valid, wb_ddr_xslvi.w_valid);
    SETVAL(comb.vb_ddr_xslvo.w_ready, i_ddr_w_ready);
    SETVAL(o_ddr_b_ready, wb_ddr_xslvi.b_ready);
    SETVAL(comb.vb_ddr_xslvo.b_id, i_ddr_b_id);
    SETVAL(comb.vb_ddr_xslvo.b_resp, i_ddr_b_resp);
    SETVAL(comb.vb_ddr_xslvo.b_valid, i_ddr_b_valid);
    SETZERO(comb.vb_ddr_xslvo.b_user);
    SETVAL(o_ddr_ar_id, wb_ddr_xslvi.ar_id);
    TEXT("Warning: we should do (addr - start_addr)[29:0], but BAR is 0x80000000");
    SETVAL(o_ddr_ar_addr, BITS(wb_ddr_xslvi.ar_bits.addr, 29, 0));
    SETVAL(o_ddr_ar_len, wb_ddr_xslvi.ar_bits.len);
    SETVAL(o_ddr_ar_size, wb_ddr_xslvi.ar_bits.size);
    SETVAL(o_ddr_ar_burst, wb_ddr_xslvi.ar_bits.burst);
    SETVAL(o_ddr_ar_lock, wb_ddr_xslvi.ar_bits.lock);
    SETVAL(o_ddr_ar_cache, wb_ddr_xslvi.ar_bits.cache);
    SETVAL(o_ddr_ar_prot, wb_ddr_xslvi.ar_bits.prot);
    SETVAL(o_ddr_ar_qos, wb_ddr_xslvi.ar_bits.qos);
    SETVAL(o_ddr_ar_valid, wb_ddr_xslvi.ar_valid);
    SETVAL(comb.vb_ddr_xslvo.ar_ready, i_ddr_ar_ready);
    SETVAL(o_ddr_r_ready, wb_ddr_xslvi.r_ready);
    SETVAL(comb.vb_ddr_xslvo.r_id, i_ddr_r_id);
    SETZERO(comb.vb_ddr_xslvo.r_user);
    SETVAL(comb.vb_ddr_xslvo.r_data, i_ddr_r_data);
    SETVAL(comb.vb_ddr_xslvo.r_resp, i_ddr_r_resp);
    SETVAL(comb.vb_ddr_xslvo.r_last, i_ddr_r_last);
    SETVAL(comb.vb_ddr_xslvo.r_valid, i_ddr_r_valid);
    SETVAL(wb_ddr_xslvo, comb.vb_ddr_xslvo, "SystemC compatibility");
    // DDR plug'n'play structure:
    SETVAL(comb.vb_ddr_mapinfo, ARRITEM(bus0_mapinfo, *SCV_get_cfg_type(this, "CFG_BUS0_XSLV_DDR"), bus0_mapinfo));
    SETVAL(comb.vb_ddr_xcfg.descrsize, *SCV_get_cfg_type(this, "PNP_CFG_DEV_DESCR_BYTES"));
    SETVAL(comb.vb_ddr_xcfg.descrtype, *SCV_get_cfg_type(this, "PNP_CFG_TYPE_SLAVE"));
    SETVAL(comb.vb_ddr_xcfg.addr_start, comb.vb_ddr_mapinfo.addr_start);
    SETVAL(comb.vb_ddr_xcfg.addr_end, comb.vb_ddr_mapinfo.addr_end);
    SETVAL(comb.vb_ddr_xcfg.vid, *SCV_get_cfg_type(this, "VENDOR_OPTIMITECH"));
    SETVAL(comb.vb_ddr_xcfg.did, *SCV_get_cfg_type(this, "OPTIMITECH_SRAM"));
    SETARRITEM(dev_pnp, *SCV_get_cfg_type(this, "SOC_PNP_DDR_AXI"), dev_pnp, comb.vb_ddr_xcfg);

    TEXT("SD-controlled disabled:");
    SETARRITEM(dev_pnp, *SCV_get_cfg_type(this, "SOC_PNP_SDCTRL_REG"), dev_pnp, *SCV_get_cfg_type(this, "dev_config_none"));
    SETARRITEM(dev_pnp, *SCV_get_cfg_type(this, "SOC_PNP_SDCTRL_MEM"), dev_pnp, *SCV_get_cfg_type(this, "dev_config_none"));
}
