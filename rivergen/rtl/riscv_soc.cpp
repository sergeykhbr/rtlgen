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

#include "riscv_soc.h"
#include "../prj/impl/asic_full/config_target.h"

riscv_soc::riscv_soc(GenObject *parent, const char *name) :
    ModuleObject(parent, "riscv_soc", name),
    // simulation parameters
    bootfile(this, "bootfile", "", "Project relative HEX-file name to init boot ROM without .hex extension"),
    sim_uart_speedup_rate(this, "sim_uart_speedup_rate", "0", "simulation UART speed-up: 0=no speed up, 1=2x, 2=4x, etc"),
    // Generic parameters
    async_reset(this, "async_reset", "CFG_ASYNC_RESET"),
    // Ports
    i_sys_nrst(this, "i_sys_nrst", "1", "Power-on system reset active LOW"),
    i_sys_clk(this, "i_sys_clk", "1", "System/Bus clock"),
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
    _spi0_(this, "SPI SD-card signals:"),
    o_spi_cs(this, "o_spi_cs", "1"),
    o_spi_sclk(this, "o_spi_sclk", "1"),
    o_spi_mosi(this, "o_spi_mosi", "1", "SPI: Master Output Slave Input"),
    i_spi_miso(this, "i_spi_miso", "1", "SPI: Master Input Slave Output"),
    i_sd_detected(this, "i_sd_detected", "1", "SD-card detected"),
    i_sd_protect(this, "i_sd_protect", "1", "SD-card write protect"),
    _prci0_(this, "PLL and Reset interfaces:"),
    o_dmreset(this, "o_dmreset", "1", "Debug reset request. Everything except DMI."),
    o_prci_pmapinfo(this, "o_prci_pmapinfo", "PRCI mapping information"),
    i_prci_pdevcfg(this, "i_prci_pdevcfg", "PRCI device descriptor"),
    o_prci_apbi(this, "o_prci_apbi", "APB: PLL and Reset configuration interface"),
    i_prci_apbo(this, "i_prci_apbo", "APB: PLL and Reset configuration interface"),
    _ddr0_(this, "DDR interfaces:"),
    o_ddr_pmapinfo(this, "o_ddr_pmapinfo", "DDR configuration mapping information"),
    i_ddr_pdevcfg(this, "i_ddr_pdevcfg", "DDR configuration device descriptor"),
    o_ddr_apbi(this, "o_ddr_apbi", "APB: DDR configuration interface"),
    i_ddr_apbo(this, "i_ddr_apbo", "APB: DDR configuration interface"),
    o_ddr_xmapinfo(this, "o_ddr_xmapinfo", "DDR memory bank mapping information"),
    i_ddr_xdevcfg(this, "i_ddr_xdevcfg", "DDR memory bank descriptor"),
    o_ddr_xslvi(this, "o_ddr_xslvi", "AXI DDR memory interface"),
    i_ddr_xslvo(this, "i_ddr_xslvo", "AXI DDR memory interface"),
    // param
    _hwid0_(this),
    _hwid1_(this, "Hardware SoC Identificator."),
    _hwid2_(this, "Read Only unique platform identificator that could be read by FW"),
    SOC_HW_ID(this, "32", "SOC_HW_ID", "0x20220903"),
    _cfg0_(this),
    _cfg1_(this, "UARTx fifo log2(size) in bytes:"),
    SOC_UART1_LOG2_FIFOSZ(this, "SOC_UART1_LOG2_FIFOSZ", "4"),
    _cfg2_(this),
    _cfg3_(this, "Number of available generic IO pins:"),
    SOC_GPIO0_WIDTH(this, "SOC_GPIO0_WIDTH", "12"),
    _cfg4_(this),
    _cfg5_(this, "SD-card in SPI mode buffer size. It should be at least log2(512) Bytes:"),
    SOC_SPI0_LOG2_FIFOSZ(this, "SOC_SPI0_LOG2_FIFOSZ", "9"),
    _plic0_(this),
    _plic1_(this, "Number of contexts in PLIC controller."),
    _plic2_(this, "Example FU740: S7 Core0 (M) + 4xU74 Cores (M+S)."),
    SOC_PLIC_CONTEXT_TOTAL(this, "SOC_PLIC_CONTEXT_TOTAL", "9"),
    _plic3_(this, "Any number up to 1024. Zero interrupt must be 0."),
    SOC_PLIC_IRQ_TOTAL(this, "SOC_PLIC_IRQ_TOTAL", "73"),
    // Singals:
    acpo(this, "acpo"),
    acpi(this, "acpi"),
    bus0_mapinfo(this, "bus0_mapinfo"),
    aximi(this, "aximi"),
    aximo(this, "aximo"),
    axisi(this, "axisi"),
    axiso(this, "axiso"),
    bus1_mapinfo(this, "bus1_mapinfo"),
    apbi(this, "apbi"),
    apbo(this, "apbo"),
    dev_pnp(this, "dev_pnp"),
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
    // submodules:
    bus0(this, "bus0"),
    bus1(this, "bus1"),
    rom0(this, "rom0"),
    sram0(this, "sram0"),
    clint0(this, "clint0"),
    plic0(this, "plic0"),
    uart1(this, "uart1"),
    gpio0(this, "gpio0"),
    spi0(this, "spi0"),
    pnp0(this, "pnp0"),
    group0(this, "group0"),
    u_cdc_ddr0(this, "u_cdc_ddr0"),
    comb(this)
{
    Operation::start(this);

    // Create and connet Sub-modules:
    NEW(bus0, bus0.getName().c_str());
        CONNECT(bus0, 0, bus0.i_clk, i_sys_clk);
        CONNECT(bus0, 0, bus0.i_nrst, i_sys_nrst);
        CONNECT(bus0, 0, bus0.o_cfg, ARRITEM(dev_pnp, glob_pnp_cfg_->SOC_PNP_XCTRL0, dev_pnp));
        CONNECT(bus0, 0, bus0.i_xmsto, aximo);
        CONNECT(bus0, 0, bus0.o_xmsti, aximi);
        CONNECT(bus0, 0, bus0.i_xslvo, axiso);
        CONNECT(bus0, 0, bus0.o_xslvi, axisi);
        CONNECT(bus0, 0, bus0.o_mapinfo, bus0_mapinfo);
    ENDNEW();

    NEW(bus1, bus1.getName().c_str());
        CONNECT(bus1, 0, bus1.i_clk, i_sys_clk);
        CONNECT(bus1, 0, bus1.i_nrst, i_sys_nrst);
        CONNECT(bus1, 0, bus1.i_mapinfo, ARRITEM(bus0_mapinfo, glob_bus0_cfg_->CFG_BUS0_XSLV_PBRIDGE, bus0_mapinfo));
        CONNECT(bus1, 0, bus1.o_cfg, ARRITEM(dev_pnp, glob_pnp_cfg_->SOC_PNP_PBRIDGE0, dev_pnp));
        CONNECT(bus1, 0, bus1.i_xslvi, ARRITEM(axisi, glob_bus0_cfg_->CFG_BUS0_XSLV_PBRIDGE, axisi));
        CONNECT(bus1, 0, bus1.o_xslvo, ARRITEM(axiso, glob_bus0_cfg_->CFG_BUS0_XSLV_PBRIDGE, axiso));
        CONNECT(bus1, 0, bus1.i_apbo, apbo);
        CONNECT(bus1, 0, bus1.o_apbi, apbi);
        CONNECT(bus1, 0, bus1.o_mapinfo, bus1_mapinfo);
    ENDNEW();

    group0.cpu_num.setObjValue(&prj_cfg_->CFG_CPU_NUM);
    group0.l2cache_ena.setObjValue(&prj_cfg_->CFG_L2CACHE_ENA);
    group0.ilog2_nways.setObjValue(&prj_cfg_->CFG_ILOG2_NWAYS);
    group0.ilog2_lines_per_way.setObjValue(&prj_cfg_->CFG_ILOG2_LINES_PER_WAY);
    group0.dlog2_nways.setObjValue(&prj_cfg_->CFG_DLOG2_NWAYS);
    group0.dlog2_lines_per_way.setObjValue(&prj_cfg_->CFG_DLOG2_LINES_PER_WAY);
    group0.l2log2_nways.setObjValue(&prj_cfg_->CFG_L2_LOG2_NWAYS);
    group0.l2log2_lines_per_way.setObjValue(&prj_cfg_->CFG_L2_LOG2_LINES_PER_WAY);
    NEW(group0, group0.getName().c_str());
        CONNECT(group0, 0, group0.i_clk, i_sys_clk);
        CONNECT(group0, 0, group0.i_cores_nrst, i_sys_nrst);
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
        CONNECT(group0, 0, group0.o_xmst_cfg, ARRITEM(dev_pnp, glob_pnp_cfg_->SOC_PNP_GROUP0, dev_pnp));
        CONNECT(group0, 0, group0.i_msti, ARRITEM(aximi, glob_bus0_cfg_->CFG_BUS0_XMST_GROUP0, aximi));
        CONNECT(group0, 0, group0.o_msto, ARRITEM(aximo, glob_bus0_cfg_->CFG_BUS0_XMST_GROUP0, aximo));
        CONNECT(group0, 0, group0.i_dmi_mapinfo, ARRITEM(bus1_mapinfo, glob_bus1_cfg_->CFG_BUS1_PSLV_DMI, bus1_mapinfo));
        CONNECT(group0, 0, group0.o_dmi_cfg, ARRITEM(dev_pnp, glob_pnp_cfg_->SOC_PNP_DMI, dev_pnp));
        CONNECT(group0, 0, group0.i_dmi_apbi, ARRITEM(apbi, glob_bus1_cfg_->CFG_BUS1_PSLV_DMI, apbi));
        CONNECT(group0, 0, group0.o_dmi_apbo, ARRITEM(apbo, glob_bus1_cfg_->CFG_BUS1_PSLV_DMI, apbo));
        CONNECT(group0, 0, group0.o_dmreset, o_dmreset);
    ENDNEW();

    rom0.abits.setObjValue(&prj_cfg_->CFG_BOOTROM_LOG2_SIZE);
    rom0.filename.setObjValue(&bootfile);
    NEW(rom0, rom0.getName().c_str());
        CONNECT(rom0, 0, rom0.i_clk, i_sys_clk);
        CONNECT(rom0, 0, rom0.i_nrst, i_sys_nrst);
        CONNECT(rom0, 0, rom0.i_mapinfo, ARRITEM(bus0_mapinfo, glob_bus0_cfg_->CFG_BUS0_XSLV_BOOTROM, bus0_mapinfo));
        CONNECT(rom0, 0, rom0.o_cfg, ARRITEM(dev_pnp, glob_pnp_cfg_->SOC_PNP_BOOTROM, dev_pnp));
        CONNECT(rom0, 0, rom0.i_xslvi, ARRITEM(axisi, glob_bus0_cfg_->CFG_BUS0_XSLV_BOOTROM, axisi));
        CONNECT(rom0, 0, rom0.o_xslvo, ARRITEM(axiso, glob_bus0_cfg_->CFG_BUS0_XSLV_BOOTROM, axiso));
    ENDNEW();

    sram0.abits.setObjValue(&prj_cfg_->CFG_SRAM_LOG2_SIZE);
    NEW(sram0, sram0.getName().c_str());
        CONNECT(sram0, 0, sram0.i_clk, i_sys_clk);
        CONNECT(sram0, 0, sram0.i_nrst, i_sys_nrst);
        CONNECT(sram0, 0, sram0.i_mapinfo, ARRITEM(bus0_mapinfo, glob_bus0_cfg_->CFG_BUS0_XSLV_SRAM, bus0_mapinfo));
        CONNECT(sram0, 0, sram0.o_cfg, ARRITEM(dev_pnp, glob_pnp_cfg_->SOC_PNP_SRAM, dev_pnp));
        CONNECT(sram0, 0, sram0.i_xslvi, ARRITEM(axisi, glob_bus0_cfg_->CFG_BUS0_XSLV_SRAM, axisi));
        CONNECT(sram0, 0, sram0.o_xslvo, ARRITEM(axiso, glob_bus0_cfg_->CFG_BUS0_XSLV_SRAM, axiso));
    ENDNEW();

    plic0.ctxmax.setObjValue(&SOC_PLIC_CONTEXT_TOTAL);
    clint0.cpu_total.setObjValue(&glob_river_cfg_->CFG_CPU_MAX);
    NEW(clint0, clint0.getName().c_str());
        CONNECT(clint0, 0, clint0.i_clk, i_sys_clk);
        CONNECT(clint0, 0, clint0.i_nrst, i_sys_nrst);
        CONNECT(clint0, 0, clint0.i_mapinfo, ARRITEM(bus0_mapinfo, glob_bus0_cfg_->CFG_BUS0_XSLV_CLINT, bus0_mapinfo));
        CONNECT(clint0, 0, clint0.o_cfg, ARRITEM(dev_pnp, glob_pnp_cfg_->SOC_PNP_CLINT, dev_pnp));
        CONNECT(clint0, 0, clint0.i_xslvi, ARRITEM(axisi, glob_bus0_cfg_->CFG_BUS0_XSLV_CLINT, axisi));
        CONNECT(clint0, 0, clint0.o_xslvo, ARRITEM(axiso, glob_bus0_cfg_->CFG_BUS0_XSLV_CLINT, axiso));
        CONNECT(clint0, 0, clint0.o_mtimer, wb_clint_mtimer);
        CONNECT(clint0, 0, clint0.o_msip, wb_clint_msip);
        CONNECT(clint0, 0, clint0.o_mtip, wb_clint_mtip);
    ENDNEW();

    plic0.irqmax.setObjValue(&SOC_PLIC_IRQ_TOTAL);
    NEW(plic0, plic0.getName().c_str());
        CONNECT(plic0, 0, plic0.i_clk, i_sys_clk);
        CONNECT(plic0, 0, plic0.i_nrst, i_sys_nrst);
        CONNECT(plic0, 0, plic0.i_mapinfo, ARRITEM(bus0_mapinfo, glob_bus0_cfg_->CFG_BUS0_XSLV_PLIC, bus0_mapinfo));
        CONNECT(plic0, 0, plic0.o_cfg, ARRITEM(dev_pnp, glob_pnp_cfg_->SOC_PNP_PLIC, dev_pnp));
        CONNECT(plic0, 0, plic0.i_xslvi, ARRITEM(axisi, glob_bus0_cfg_->CFG_BUS0_XSLV_PLIC, axisi));
        CONNECT(plic0, 0, plic0.o_xslvo, ARRITEM(axiso, glob_bus0_cfg_->CFG_BUS0_XSLV_PLIC, axiso));
        CONNECT(plic0, 0, plic0.i_irq_request, wb_ext_irqs);
        CONNECT(plic0, 0, plic0.o_ip, wb_plic_xeip);
    ENDNEW();

    NEW(u_cdc_ddr0, u_cdc_ddr0.getName().c_str());
        CONNECT(u_cdc_ddr0, 0, u_cdc_ddr0.i_xslv_clk, i_sys_clk);
        CONNECT(u_cdc_ddr0, 0, u_cdc_ddr0.i_xslv_nrst, i_sys_nrst);
        CONNECT(u_cdc_ddr0, 0, u_cdc_ddr0.i_xslvi, ARRITEM(axisi, glob_bus0_cfg_->CFG_BUS0_XSLV_DDR, axisi));
        CONNECT(u_cdc_ddr0, 0, u_cdc_ddr0.o_xslvo, ARRITEM(axiso, glob_bus0_cfg_->CFG_BUS0_XSLV_DDR, axiso));
        CONNECT(u_cdc_ddr0, 0, u_cdc_ddr0.i_xmst_clk, i_ddr_clk);
        CONNECT(u_cdc_ddr0, 0, u_cdc_ddr0.i_xmst_nrst, i_ddr_nrst);
        CONNECT(u_cdc_ddr0, 0, u_cdc_ddr0.o_xmsto, o_ddr_xslvi);
        CONNECT(u_cdc_ddr0, 0, u_cdc_ddr0.i_xmsti, i_ddr_xslvo);
    ENDNEW();

    uart1.log2_fifosz.setObjValue(&SOC_UART1_LOG2_FIFOSZ);
    uart1.sim_speedup_rate.setObjValue(&sim_uart_speedup_rate);
    NEW(uart1, uart1.getName().c_str());
        CONNECT(uart1, 0, uart1.i_clk, i_sys_clk);
        CONNECT(uart1, 0, uart1.i_nrst, i_sys_nrst);
        CONNECT(uart1, 0, uart1.i_mapinfo, ARRITEM(bus1_mapinfo, glob_bus1_cfg_->CFG_BUS1_PSLV_UART1, bus1_mapinfo));
        CONNECT(uart1, 0, uart1.o_cfg, ARRITEM(dev_pnp, glob_pnp_cfg_->SOC_PNP_UART1, dev_pnp));
        CONNECT(uart1, 0, uart1.i_apbi, ARRITEM(apbi, glob_bus1_cfg_->CFG_BUS1_PSLV_UART1, apbi));
        CONNECT(uart1, 0, uart1.o_apbo, ARRITEM(apbo, glob_bus1_cfg_->CFG_BUS1_PSLV_UART1, apbo));
        CONNECT(uart1, 0, uart1.i_rd, i_uart1_rd);
        CONNECT(uart1, 0, uart1.o_td, o_uart1_td);
        CONNECT(uart1, 0, uart1.o_irq, w_irq_uart1);
    ENDNEW();

    gpio0.width.setObjValue(&SOC_GPIO0_WIDTH);
    NEW(gpio0, gpio0.getName().c_str());
        CONNECT(gpio0, 0, gpio0.i_clk, i_sys_clk);
        CONNECT(gpio0, 0, gpio0.i_nrst, i_sys_nrst);
        CONNECT(gpio0, 0, gpio0.i_mapinfo, ARRITEM(bus1_mapinfo, glob_bus1_cfg_->CFG_BUS1_PSLV_GPIO, bus1_mapinfo));
        CONNECT(gpio0, 0, gpio0.o_cfg, ARRITEM(dev_pnp, glob_pnp_cfg_->SOC_PNP_GPIO, dev_pnp));
        CONNECT(gpio0, 0, gpio0.i_apbi, ARRITEM(apbi, glob_bus1_cfg_->CFG_BUS1_PSLV_GPIO, apbi));
        CONNECT(gpio0, 0, gpio0.o_apbo, ARRITEM(apbo, glob_bus1_cfg_->CFG_BUS1_PSLV_GPIO, apbo));
        CONNECT(gpio0, 0, gpio0.i_gpio, i_gpio);
        CONNECT(gpio0, 0, gpio0.o_gpio_dir, o_gpio_dir);
        CONNECT(gpio0, 0, gpio0.o_gpio, o_gpio);
        CONNECT(gpio0, 0, gpio0.o_irq, wb_irq_gpio);
    ENDNEW();

    spi0.log2_fifosz.setObjValue(&SOC_SPI0_LOG2_FIFOSZ);
    NEW(spi0, spi0.getName().c_str());
        CONNECT(spi0, 0, spi0.i_clk, i_sys_clk);
        CONNECT(spi0, 0, spi0.i_nrst, i_sys_nrst);
        CONNECT(spi0, 0, spi0.i_mapinfo, ARRITEM(bus1_mapinfo, glob_bus1_cfg_->CFG_BUS1_PSLV_SPI, bus1_mapinfo));
        CONNECT(spi0, 0, spi0.o_cfg, ARRITEM(dev_pnp, glob_pnp_cfg_->SOC_PNP_SPI, dev_pnp));
        CONNECT(spi0, 0, spi0.i_apbi, ARRITEM(apbi, glob_bus1_cfg_->CFG_BUS1_PSLV_SPI, apbi));
        CONNECT(spi0, 0, spi0.o_apbo, ARRITEM(apbo, glob_bus1_cfg_->CFG_BUS1_PSLV_SPI, apbo));
        CONNECT(spi0, 0, spi0.o_cs, o_spi_cs);
        CONNECT(spi0, 0, spi0.o_sclk, o_spi_sclk);
        CONNECT(spi0, 0, spi0.o_mosi, o_spi_mosi);
        CONNECT(spi0, 0, spi0.i_miso, i_spi_miso);
        CONNECT(spi0, 0, spi0.i_detected, i_sd_detected);
        CONNECT(spi0, 0, spi0.i_protect, i_sd_protect);
    ENDNEW();

    pnp0.cfg_slots.setObjValue(&glob_pnp_cfg_->SOC_PNP_TOTAL);
    pnp0.hw_id.setObjValue(&SOC_HW_ID);
    pnp0.cpu_max.setObjValue(&prj_cfg_->CFG_CPU_NUM);
    pnp0.l2cache_ena.setObjValue(&prj_cfg_->CFG_L2CACHE_ENA);
    pnp0.plic_irq_max.setObjValue(&SOC_PLIC_IRQ_TOTAL);
    NEW(pnp0, pnp0.getName().c_str());
        CONNECT(pnp0, 0, pnp0.i_clk, i_sys_clk);
        CONNECT(pnp0, 0, pnp0.i_nrst, i_sys_nrst);
        CONNECT(pnp0, 0, pnp0.i_mapinfo, ARRITEM(bus1_mapinfo, glob_bus1_cfg_->CFG_BUS1_PSLV_PNP, bus1_mapinfo));
        CONNECT(pnp0, 0, pnp0.i_cfg, dev_pnp);
        CONNECT(pnp0, 0, pnp0.o_cfg, ARRITEM(dev_pnp, glob_pnp_cfg_->SOC_PNP_PNP, dev_pnp));
        CONNECT(pnp0, 0, pnp0.i_apbi, ARRITEM(apbi, glob_bus1_cfg_->CFG_BUS1_PSLV_PNP, apbi));
        CONNECT(pnp0, 0, pnp0.o_apbo, ARRITEM(apbo, glob_bus1_cfg_->CFG_BUS1_PSLV_PNP, apbo));
        CONNECT(pnp0, 0, pnp0.o_irq, w_irq_pnp);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void riscv_soc::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;
    types_amba *amba = glob_types_amba_;


    TEXT();
    TEXT("assign interrupts:");
    SETBITS(comb.vb_ext_irqs, 22, 0, ALLZEROS());
    SETBITS(comb.vb_ext_irqs, DEC(ADD2(CONST("23"), SOC_GPIO0_WIDTH)),
                              CONST("23"),
                              wb_irq_gpio, "FU740: 16 bits, current 12-bits");
    SETBIT(comb.vb_ext_irqs, 39, w_irq_uart1);
    SETBITS(comb.vb_ext_irqs, 69, 40, ALLZEROS());
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
    SETARRITEM(aximo, glob_bus0_cfg_->CFG_BUS0_XMST_DMA, aximo, amba->axi4_master_out_none);
    SETVAL(acpo, amba->axi4_master_out_none);

    TEXT();
    TEXT("PRCI:");
    SETVAL(o_prci_apbi, ARRITEM(apbi, glob_bus1_cfg_->CFG_BUS1_PSLV_PRCI, apbi));
    SETARRITEM(apbo, glob_bus1_cfg_->CFG_BUS1_PSLV_PRCI, apbo, i_prci_apbo);
    SETARRITEM(dev_pnp, glob_pnp_cfg_->SOC_PNP_PRCI, dev_pnp, i_prci_pdevcfg);

    TEXT();
    TEXT("DDR:");
    SETVAL(o_ddr_xmapinfo, ARRITEM(bus0_mapinfo, glob_bus0_cfg_->CFG_BUS0_XSLV_DDR, bus0_mapinfo));
    SETARRITEM(dev_pnp, glob_pnp_cfg_->SOC_PNP_DDR_AXI, dev_pnp, i_ddr_xdevcfg);
    SETVAL(o_ddr_pmapinfo, ARRITEM(bus1_mapinfo, glob_bus1_cfg_->CFG_BUS1_PSLV_DDR, bus1_mapinfo));
    SETARRITEM(dev_pnp, glob_pnp_cfg_->SOC_PNP_DDR_APB, dev_pnp, i_ddr_pdevcfg);
    SETVAL(o_ddr_apbi, ARRITEM(apbi, glob_bus1_cfg_->CFG_BUS1_PSLV_DDR, apbi));
    SETARRITEM(apbo, glob_bus1_cfg_->CFG_BUS1_PSLV_DDR, apbo, i_ddr_apbo);
}
