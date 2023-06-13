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

#include "types_pnp.h"

types_pnp* glob_pnp_cfg_ = 0;

types_pnp::types_pnp(GenObject *parent) :
    FileObject(parent, "types_pnp"),
    _vid1_(this, "@name Vendor IDs defintion."),
    VENDOR_GNSSSENSOR(this, "16", "VENDOR_GNSSSENSOR", "0x00F1"),
    VENDOR_OPTIMITECH(this, "16", "VENDOR_OPTIMITECH", "0x00F2"),
    _didmst0_(this),
    _didmst1_(this, "@name Master Device IDs definition:"),
    _didmst2_(this, "Empty master slot device"),
    MST_DID_EMPTY(this, "16", "MST_DID_EMPTY", "0x7755"),
    _didmst5_(this, "\"River\" CPU Device ID."),
    RISCV_RIVER_CPU(this, "16", "RISCV_RIVER_CPU", "0x0505"),
    _didmst6_(this, "\"Wasserfall\" CPU Device ID."),
    RISCV_RIVER_WORKGROUP(this, "16", "RISCV_RIVER_WORKGROUP", "0x0506"),
    _didmst7_(this, "UART with DMA: Test Access Point (TAP)"),
    GNSSSENSOR_UART_TAP(this, "16", "GNSSSENSOR_UART_TAP", "0x050A"),
    _didmst8_(this, "JTAG Test Access Point (TAP) with SBA interface (DMA without progbuf)"),
    OPTIMITECH_JTAG_SBA(this, "16", "OPTIMITECH_JTAG_SBA", "0x050B"),
    _didslv0_(this),
    _didslv1_(this, "@name Slave Device IDs definition:"),
    _didslv2_(this, "Empty slave slot device"),
    SLV_DID_EMPTY(this, "16", "SLV_DID_EMPTY", "0x5577"),
    _didslv3_(this, "Boot ROM Device ID"),
    OPTIMITECH_ROM(this, "16", "OPTIMITECH_ROM", "0x0071"),
    _didslv4_(this, "Internal SRAM block Device ID"),
    OPTIMITECH_SRAM(this, "16", "OPTIMITECH_SRAM", "0x0073"),
    _didslv5_(this, "Configuration Registers Module"),
    OPTIMITECH_PNP(this, "16", "OPTIMITECH_PNP", "0x0074"),
    _didslv6_(this, "SD-card controller"),
    OPTIMITECH_SPI_FLASH(this, "16", "OPTIMITECH_SPI_FLASH", "0x0075"),
    _didslv7_(this, "General purpose IOs"),
    OPTIMITECH_GPIO(this, "16", "OPTIMITECH_GPIO", "0x0076"),
    _didslv8_(this, "rs-232 UART Device ID"),
    OPTIMITECH_UART(this, "16", "OPTIMITECH_UART", "0x007a"),
    _didslv9_(this, "Core local interrupt controller"),
    OPTIMITECH_CLINT(this, "16", "OPTIMITECH_CLINT", "0x0083"),
    _didslv10_(this, "External interrupt controller"),
    OPTIMITECH_PLIC(this, "16", "OPTIMITECH_PLIC", "0x0084"),
    _didslv11_(this, "AXI to APB Brdige"),
    OPTIMITECH_AXI2APB_BRIDGE(this, "16", "OPTIMITECH_AXI2APB_BRIDGE", "0x0085"),
    _didslv12_(this, "AXI interconnect"),
    OPTIMITECH_AXI_INTERCONNECT(this, "16", "OPTIMITECH_AXI_INTERCONNECT", "0x0086"),
    _didslv13_(this, "APB PLL and Register Interface"),
    OPTIMITECH_PRCI(this, "16", "OPTIMITECH_PRCI", "0x0087"),
    _didslv14_(this, "DDR controller status registers"),
    OPTIMITECH_DDRCTRL(this, "16", "OPTIMITECH_DDRCTRL", "0x0088"),
    _didslv15_(this, "SPI controller (SD-card in SPI mode)"),
    OPTIMITECH_SPI(this, "16", "OPTIMITECH_SPI", "0x0089"),
    _didslv16_(this, "RIVER debug registers:"),
    OPTIMITECH_RIVER_DMI(this, "16", "OPTIMITECH_RIVER_DMI", "0x008a"),
    _pnpcfg0_(this),
    _pnpcfg1_(this, "Plug'n'Play descriptor localparams."),
    _pnpcfg2_(this, "Undefined type of the descriptor (empty device)."),
    PNP_CFG_TYPE_INVALID(this, "2", "PNP_CFG_TYPE_INVALID", "0x0"),
    _pnpcfg3_(this, "AXI slave device standard descriptor."),
    PNP_CFG_TYPE_MASTER(this, "2", "PNP_CFG_TYPE_MASTER", "0x1"),
    _pnpcfg4_(this, "AXI master device standard descriptor."),
    PNP_CFG_TYPE_SLAVE(this, "2", "PNP_CFG_TYPE_SLAVE", "0x2"),
    _pnpcfg5_(this, "@brief Size in bytes of the standard slave descriptor.."),
    _pnpcfg6_(this, "@details Firmware uses this value instead of sizeof(slave_config_type)."),
    PNP_CFG_DEV_DESCR_BYTES(this, "8", "PNP_CFG_DEV_DESCR_BYTES", "0x10"),
    _pnp0_(this),
    _pnp1_(this, "Plug-and-Play device descriptors array connected to pnp module:"),
    SOC_PNP_XCTRL0(this, "SOC_PNP_XCTRL0", "0"),
    SOC_PNP_GROUP0(this, "SOC_PNP_GROUP0", "1"),
    SOC_PNP_BOOTROM(this, "SOC_PNP_BOOTROM", "2"),
    SOC_PNP_SRAM(this, "SOC_PNP_SRAM", "3"),
    SOC_PNP_DDR_AXI(this, "SOC_PNP_DDR_AXI", "4"),
    SOC_PNP_DDR_APB(this, "SOC_PNP_DDR_APB", "5"),
    SOC_PNP_PRCI(this, "SOC_PNP_PRCI", "6"),
    SOC_PNP_GPIO(this, "SOC_PNP_GPIO", "7"),
    SOC_PNP_CLINT(this, "SOC_PNP_CLINT", "8"),
    SOC_PNP_PLIC(this, "SOC_PNP_PLIC", "9"),
    SOC_PNP_PNP(this, "SOC_PNP_PNP", "10"),
    SOC_PNP_PBRIDGE0(this, "SOC_PNP_PBRIDGE0", "11"),
    SOC_PNP_DMI(this, "SOC_PNP_DMI", "12"),
    SOC_PNP_UART1(this, "SOC_PNP_UART1", "13"),
    SOC_PNP_SPI(this, "SOC_PNP_SPI", "14"),
    SOC_PNP_TOTAL(this, "SOC_PNP_TOTAL", "15"),
    _pnp2_(this),
    _pnp3_(this, "@brief   Plug-n-play descriptor structure for connected device."),
    _pnp4_(this, "@details Each device must generates this datatype output that"),
    _pnp5_(this, "         is connected directly to the 'pnp' slave module on system bus."),
    dev_config_type_def_(this, ""),
    _pnp6_(this),
    _pnp7_(this, "@brief Default config value for empty slot."),
    dev_config_none(this, "dev_config_none"),
    _pnp8_(this),
    soc_pnp_vector_def_(this, ""),
    _n_(this)
{
    glob_pnp_cfg_ = this;
}

