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
    soc_pnp_vector_def_(this, ""),
    _n_(this)
{
    glob_pnp_cfg_ = this;
}

