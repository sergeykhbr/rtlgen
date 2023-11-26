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
    _xmst8_(this, "Necessary bus width to store index + 1."),
    CFG_BUS0_XMST_LOG2_TOTAL(this, "CFG_BUS0_XMST_LOG2_TOTAL", "2", "$clog2(CFG_BUS0_XMST_TOTAL + 1)"),
    _xslv0_(this),
    _xslv1_(this),
    _xslv2_(this, "@defgroup slave_id_group AMBA AXI slaves generic IDs."),
    _xslv3_(this, "@details Each module in a SoC has to be indexed by unique identificator."),
    _xslv4_(this, "         In current implementation it is used sequential indexing for it."),
    _xslv5_(this, "         Indexes are used to specify a device bus item in a vectors."),
    _xslv6_(this),
    _xslv7_(this, "@brief Configuration index of the Boot ROM module visible by the firmware."),
    CFG_BUS0_XSLV_BOOTROM(this, "CFG_BUS0_XSLV_BOOTROM", "0"),
    _xslv8_(this, "Configuration index of the Core Local Interrupt Controller module."),
    CFG_BUS0_XSLV_CLINT(this, "CFG_BUS0_XSLV_CLINT", "1"),
    _xslv9_(this, "Configuration index of the SRAM module visible by the firmware."),
    CFG_BUS0_XSLV_SRAM(this, "CFG_BUS0_XSLV_SRAM", "2"),
    _xslv10_(this, "Configuration index of the External Controller module."),
    CFG_BUS0_XSLV_PLIC(this, "CFG_BUS0_XSLV_PLIC", "3"),
    _xslv11_(this, "Configuration index of the APB Bridge."),
    CFG_BUS0_XSLV_PBRIDGE(this, "CFG_BUS0_XSLV_PBRIDGE", "4"),
    _xslv12_(this, "External DDR"),
    CFG_BUS0_XSLV_DDR(this, "CFG_BUS0_XSLV_DDR", "5"),
    _xslv13_(this, "SD-card direct memory access."),
    CFG_BUS0_XSLV_SDCTRL_MEM(this, "CFG_BUS0_XSLV_SDCTRL_MEM", "6"),
    _xslv14_(this, "Total number of the slaves devices."),
    CFG_BUS0_XSLV_TOTAL(this, "CFG_BUS0_XSLV_TOTAL", "7"),
    _xslv15_(this, "Necessary bus width to store index + 1."),
    CFG_BUS0_XSLV_LOG2_TOTAL(this, "CFG_BUS0_XSLV_LOG2_TOTAL", "3", "$clog2(CFG_BUS0_XSLV_TOTAL + 1)"),
    _vec0_(this),
    bus0_xmst_in_vector_def_(this, "axi4_master_in_type"),
    bus0_xmst_out_vector_def_(this, "axi4_master_out_type"),
    bus0_xslv_in_vector_def_(this, "axi4_slave_in_type"),
    bus0_xslv_out_vector_def_(this, "axi4_slave_out_type"),
    bus0_mapinfo_vector_def_(this, "mapinfo_type"),
    _map0_(this),
    _map1_(this, "Bus 0 device tree"),
    CFG_BUS0_MAP(this),
    _n_(this)
{
    glob_bus0_cfg_ = this;
}

