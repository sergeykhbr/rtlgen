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

#include "types_bus0.h"

types_bus0* glob_bus0_cfg_ = 0;

types_bus0::types_bus0(GenObject *parent) :
    FileObject(parent, "types_bus0"),
    _xslv0_(this, "@defgroup slave_id_group AMBA AXI slaves generic IDs."),
    _xslv1_(this, "@details Each module in a SoC has to be indexed by unique identificator."),
    _xslv2_(this, "         In current implementation it is used sequential indexing for it."),
    _xslv3_(this, "         Indexes are used to specify a device bus item in a vectors."),
    _xslv4_(this),
    _xslv5_(this, "@brief Configuration index of the Boot ROM module visible by the firmware."),
    CFG_BUS0_XSLV_BOOTROM(this, "CFG_BUS0_XSLV_BOOTROM", "0"),
    _xslv7_(this, "Configuration index of the SRAM module visible by the firmware."),
    CFG_BUS0_XSLV_SRAM(this, "CFG_BUS0_XSLV_SRAM", "2"),
    _xslv8_(this, "External DDR"),
    CFG_BUS0_XSLV_DDR(this, "CFG_BUS0_XSLV_DDR", "3"),
    _xslv9_(this, "Configuration index of the APB Bridge."),
    CFG_BUS0_XSLV_BUS1(this, "CFG_BUS0_XSLV_BUS1", "4"),
    _xslv10_(this, "Configuration index of the GPIO (General Purpose In/Out) module."),
    CFG_BUS0_XSLV_GPIO(this, "CFG_BUS0_XSLV_GPIO", "5"),
    _xslv11_(this, "Configuration index of the Core Local Interrupt Controller module."),
    CFG_BUS0_XSLV_CLINT(this, "CFG_BUS0_XSLV_CLINT", "6"),
    _xslv12_(this, "Configuration index of the External Controller module."),
    CFG_BUS0_XSLV_PLIC(this, "CFG_BUS0_XSLV_PLIC", "7"),
    _xslv13_(this, "Configuration index of the Plug-n-Play module."),
    CFG_BUS0_XSLV_PNP(this, "CFG_BUS0_XSLV_PNP", "8"),
    _xslv14_(this, "Total number of the slaves devices."),
    CFG_BUS0_XSLV_TOTAL(this, "CFG_BUS0_XSLV_TOTAL", "9"),
    _xmst0_(this),
    _xmst1_(this, "@defgroup master_id_group AXI4 masters generic IDs."),
    _xmst2_(this, "@details Each master must be assigned to a specific ID that used"),
    _xmst3_(this, "         as an index in the vector array of AXI master bus."),
    _xmst4_(this, ""),
    _xmst5_(this, "Total number of CPU limited by config CFG_TOTAL_CPU_MAX"),
    CFG_BUS0_XMST_GROUP0(this, "CFG_BUS0_XMST_GROUP0", "0"),
    _xmst6_(this, "DMA master interface generic index."),
    CFG_BUS0_XMST_DMA(this, "CFG_BUS0_XMST_DMA", "1"),
    _xmst7_(this, "Total Number of master devices on system bus."),
    CFG_BUS0_XMST_TOTAL(this, "CFG_BUS0_XMST_TOTAL", "2"),
    _vec0_(this),
    bus0_xmst_in_vector_def_(this, ""),
    bus0_xmst_out_vector_def_(this, ""),
    bus0_xslv_in_vector_def_(this, ""),
    bus0_xslv_out_vector_def_(this, ""),
    bus0_mapinfo_vector_def_(this, ""),
    _map0_(this),
    _map1_(this, "Bus 0 device tree"),
    CFG_BUS0_MAP(this, "CFG_BUS0_MAP"),
    _n_(this)
{
    glob_bus0_cfg_ = this;
}

