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
    // Generic parameters
    async_reset(this, "async_reset", "CFG_ASYNC_RESET"),
    // Ports
    i_rst(this, "i_rst", "1", "System reset active HIGH"),
    i_clk(this, "i_clk", "1", "CPU clock"),
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
    // param
    _map0_(this),
    _pnp0_(this),
    SOC_PNP_XCTRL0(this, "SOC_PNP_XCTRL0", "0"),
    SOC_PNP_GROUP0(this, "SOC_PNP_GROUP0", "1"),
    SOC_PNP_BOOTROM(this, "SOC_PNP_BOOTROM", "2"),
    SOC_PNP_SRAM(this, "SOC_PNP_SRAM", "3"),
    SOC_PNP_DDR(this, "SOC_PNP_DDR", "4"),
    SOC_PNP_GPIO(this, "SOC_PNP_GPIO", "5"),
    SOC_PNP_CLINT(this, "SOC_PNP_CLINT", "6"),
    SOC_PNP_PLIC(this, "SOC_PNP_PLIC", "7"),
    SOC_PNP_PNP(this, "SOC_PNP_PNP", "8"),
    SOC_PNP_PBRIDGE0(this, "SOC_PNP_PBRIDGE0", "9"),
    SOC_PNP_DMI(this, "SOC_PNP_DMI", "10"),
    SOC_PNP_UART1(this, "SOC_PNP_UART1", "11"),
    SOC_PNP_TOTAL(this, "SOC_PNP_TOTAL", "12"),
    _cfg0_(this),
    CFG_SOC_UART1_LOG2_FIFOSZ(this, "CFG_SOC_UART1_LOG2_FIFOSZ", "4"),
    soc_pnp_vector_def_(this, ""),
    // Singals:
    w_sys_nrst(this, "w_sys_nrst", "1", "0", "System reset of whole system"),
    w_dbg_nrst(this, "w_dbg_nrst", "1", "0", "Reset workgroup debug interface"),
    w_dmreset(this, "w_dmreset", "1", "0", "Reset request from workgroup debug interface"),
    acpo(this, "acpo"),
    acpi(this, "acpi"),
    bus0_mapinfo(this, "bus0_mapinfo"),
    aximi(this, "aximi"),
    aximo(this, "aximo"),
    axisi(this, "axisi"),
    axiso(this, "axiso"),
    bus1_mapinfo(this, "bus1_mapinfo"),
    apbmi(this, "apbmi"),
    apbmo(this, "apbmo"),
    apbsi(this, "apbsi"),
    apbso(this, "apbso"),
    dev_pnp(this, "dev_pnp"),
    wb_clint_mtimer(this, "wb_clint_mtimer", "64"),
    wb_clint_msip(this, "wb_clint_msip", "CFG_CPU_MAX"),
    wb_clint_mtip(this, "wb_clint_mtip", "CFG_CPU_MAX"),
    wb_plic_xeip(this, "wb_plic_xeip", "CFG_PLIC_CONTEXT_TOTAL"),
    wb_plic_meip(this, "wb_plic_meip", "CFG_CPU_MAX"),
    wb_plic_seip(this, "wb_plic_seip", "CFG_CPU_MAX"),
    w_irq_uart1(this, "w_irq_uart1", "1"),
    wb_irq_gpio(this, "wb_irq_gpio", "16"),
    w_irq_pnp(this, "w_irq_pnp", "1"),
    wb_ext_irqs(this, "wb_ext_irqs", "CFG_PLIC_IRQ_TOTAL"),
    // submodules:
    apbrdg0(this, "apbrdg0"),
    uart1(this, "uart1"),
    group0(this, "group0"),
    comb(this)
{
    Operation::start(this);

    // Create and connet Sub-modules:
    NEW(apbrdg0, apbrdg0.getName().c_str());
        CONNECT(apbrdg0, 0, apbrdg0.i_clk, i_clk);
        CONNECT(apbrdg0, 0, apbrdg0.i_nrst, w_sys_nrst);
        CONNECT(apbrdg0, 0, apbrdg0.i_mapinfo, ARRITEM(bus0_mapinfo, glob_bus0_cfg_->CFG_BUS0_XSLV_PBRIDGE, bus0_mapinfo));
        CONNECT(apbrdg0, 0, apbrdg0.o_cfg, ARRITEM(dev_pnp, SOC_PNP_PBRIDGE0, dev_pnp));
        CONNECT(apbrdg0, 0, apbrdg0.i_xslvi, ARRITEM(axisi, glob_bus0_cfg_->CFG_BUS0_XSLV_PBRIDGE, axisi));
        CONNECT(apbrdg0, 0, apbrdg0.o_xslvo, ARRITEM(axiso, glob_bus0_cfg_->CFG_BUS0_XSLV_PBRIDGE, axiso));
        CONNECT(apbrdg0, 0, apbrdg0.i_apbmi, ARRITEM(apbmi, glob_bus1_cfg_->CFG_BUS1_PMST_PARENT, apbmi));
        CONNECT(apbrdg0, 0, apbrdg0.o_apbmo, ARRITEM(apbmo, glob_bus1_cfg_->CFG_BUS1_PMST_PARENT, apbmo));
    ENDNEW();

    group0.cpu_num.setObjValue(&prj_cfg_->CFG_CPU_NUM);
    group0.l2cache_ena.setObjValue(&prj_cfg_->CFG_L2CACHE_ENA);
    NEW(group0, group0.getName().c_str());
        CONNECT(group0, 0, group0.i_clk, i_clk);
        CONNECT(group0, 0, group0.i_cores_nrst, w_sys_nrst);
        CONNECT(group0, 0, group0.i_dmi_nrst, w_dbg_nrst);
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
        CONNECT(group0, 0, group0.o_xmst_cfg, ARRITEM(dev_pnp, SOC_PNP_GROUP0, dev_pnp));
        CONNECT(group0, 0, group0.i_msti, ARRITEM(aximi, glob_bus0_cfg_->CFG_BUS0_XMST_GROUP0, aximi));
        CONNECT(group0, 0, group0.o_msto, ARRITEM(aximo, glob_bus0_cfg_->CFG_BUS0_XMST_GROUP0, aximo));
        CONNECT(group0, 0, group0.i_dmi_mapinfo, ARRITEM(bus1_mapinfo, glob_bus1_cfg_->CFG_BUS1_PSLV_DMI, bus1_mapinfo));
        CONNECT(group0, 0, group0.o_dmi_cfg, ARRITEM(dev_pnp, SOC_PNP_DMI, dev_pnp));
        CONNECT(group0, 0, group0.i_dmi_apbi, ARRITEM(apbsi, glob_bus1_cfg_->CFG_BUS1_PSLV_DMI, apbsi));
        CONNECT(group0, 0, group0.o_dmi_apbo, ARRITEM(apbso, glob_bus1_cfg_->CFG_BUS1_PSLV_DMI, apbso));
        CONNECT(group0, 0, group0.o_dmreset, w_dmreset);
    ENDNEW();

    uart1.log2_fifosz.setObjValue(&CFG_SOC_UART1_LOG2_FIFOSZ);
    NEW(uart1, uart1.getName().c_str());
        CONNECT(uart1, 0, uart1.i_clk, i_clk);
        CONNECT(uart1, 0, uart1.i_nrst, w_sys_nrst);
        CONNECT(uart1, 0, uart1.i_mapinfo, ARRITEM(bus1_mapinfo, glob_bus1_cfg_->CFG_BUS1_PSLV_UART1, bus1_mapinfo));
        CONNECT(uart1, 0, uart1.o_cfg, ARRITEM(dev_pnp, SOC_PNP_UART1, dev_pnp));
        CONNECT(uart1, 0, uart1.i_apbi, ARRITEM(apbsi, glob_bus1_cfg_->CFG_BUS1_PSLV_UART1, apbsi));
        CONNECT(uart1, 0, uart1.o_apbo, ARRITEM(apbso, glob_bus1_cfg_->CFG_BUS1_PSLV_UART1, apbso));
        CONNECT(uart1, 0, uart1.i_rd, i_uart1_rd);
        CONNECT(uart1, 0, uart1.o_td, o_uart1_td);
        CONNECT(uart1, 0, uart1.o_irq, w_irq_uart1);
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
    SETBITS(comb.vb_ext_irqs, 38, 23, wb_irq_gpio, "FU740: 16 bits, current 12-bits");
    SETBIT(comb.vb_ext_irqs, 39, w_irq_uart1);
    SETBITS(comb.vb_ext_irqs, 69, 40, ALLZEROS());
    SETBIT(comb.vb_ext_irqs, 70, w_irq_pnp);
    SETBITS(comb.vb_ext_irqs, DEC(target->CFG_PLIC_IRQ_TOTAL), 71, ALLZEROS());;
    SETVAL(wb_ext_irqs, comb.vb_ext_irqs);

    TEXT();
    SETONE(o_jtag_vref);

    TEXT();
    TEXT("Nullify emty AXI-slots:");
    SETARRITEM(aximo, glob_bus0_cfg_->CFG_BUS0_XMST_DMA, aximo, amba->axi4_master_out_none);
    SETVAL(acpo, amba->axi4_master_out_none);

    TEXT("TODO: APB interconnect");
    SETARRITEM(apbsi, glob_bus1_cfg_->CFG_BUS1_PSLV_DMI, apbsi, glob_types_amba_->apb_in_none);
    SETARRITEM(apbsi, glob_bus1_cfg_->CFG_BUS1_PSLV_UART1, apbsi, ARRITEM(apbmo, glob_bus1_cfg_->CFG_BUS1_PMST_PARENT, apbmo));
    SETARRITEM(apbmi, glob_bus1_cfg_->CFG_BUS1_PMST_PARENT, apbmi, ARRITEM(apbso, glob_bus1_cfg_->CFG_BUS1_PSLV_UART1, apbso));
}
