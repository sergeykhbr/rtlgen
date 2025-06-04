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

#include "types_accel_bus0.h"

types_accel_bus0::types_accel_bus0(GenObject *parent) :
    FileObject(parent, "types_accel_bus0"),
    xmst_autoincr_(0),
    xslv_autoincr_(0),
    _xmst0_(this),
    _xmst1_(this, "@defgroup master_id_group AXI4 masters generic IDs."),
    _xmst2_(this, "@details Each master must be assigned to a specific ID that used"),
    _xmst3_(this, "         as an index in the vector array of AXI master bus."),
    _xmst4_(this, ""),
    _xmst5_(this, "Total number of CPU limited by config CFG_TOTAL_CPU_MAX"),
    CFG_BUS0_XMST_GROUP0(this, "CFG_BUS0_XMST_GROUP0", xmst_autoincr_++),
    _xmst6_(this, "PCIE DMA master interface."),
    CFG_BUS0_XMST_PCIE(this, "CFG_BUS0_XMST_PCIE", xmst_autoincr_++),
    _xmst7_(this, "HDMI DMA master interface."),
    CFG_BUS0_XMST_HDMI(this, "CFG_BUS0_XMST_HDMI", xmst_autoincr_++),
    _xmst8_(this, "Total Number of master devices on system bus."),
    CFG_BUS0_XMST_TOTAL(this, "CFG_BUS0_XMST_TOTAL", xmst_autoincr_),
    _xmst9_(this, "Necessary bus width to store index + 1."),
    CFG_BUS0_XMST_LOG2_TOTAL(this, "CFG_BUS0_XMST_LOG2_TOTAL", "2", "$clog2(CFG_BUS0_XMST_TOTAL + 1)"),
    _xslv0_(this),
    _xslv1_(this),
    _xslv2_(this, "@defgroup slave_id_group AMBA AXI slaves generic IDs."),
    _xslv3_(this, "@details Each module in a SoC has to be indexed by unique identificator."),
    _xslv4_(this, "         In current implementation it is used sequential indexing for it."),
    _xslv5_(this, "         Indexes are used to specify a device bus item in a vectors."),
    _xslv6_(this),
    _xslv7_(this, "@brief Configuration index of the Boot ROM module visible by the firmware."),
    CFG_BUS0_XSLV_BOOTROM(this, "CFG_BUS0_XSLV_BOOTROM", xslv_autoincr_++),
    _xslv8_(this, "Configuration index of the Core Local Interrupt Controller module."),
    CFG_BUS0_XSLV_CLINT(this, "CFG_BUS0_XSLV_CLINT", xslv_autoincr_++),
    _xslv9_(this, "Configuration index of the SRAM module visible by the firmware."),
    CFG_BUS0_XSLV_SRAM(this, "CFG_BUS0_XSLV_SRAM", xslv_autoincr_++),
    _xslv10_(this, "Configuration index of the External Controller module."),
    CFG_BUS0_XSLV_PLIC(this, "CFG_BUS0_XSLV_PLIC", xslv_autoincr_++),
    _xslv11_(this, "Configuration index of the APB Bridge."),
    CFG_BUS0_XSLV_PBRIDGE(this, "CFG_BUS0_XSLV_PBRIDGE", xslv_autoincr_++),
    _xslv12_(this, "External DDR"),
    CFG_BUS0_XSLV_DDR(this, "CFG_BUS0_XSLV_DDR", xslv_autoincr_++),
    _xslv14_(this, "Total number of the slaves devices."),
    CFG_BUS0_XSLV_TOTAL(this, "CFG_BUS0_XSLV_TOTAL", xslv_autoincr_),
    _xslv15_(this, "Necessary bus width to store index + 1."),
    CFG_BUS0_XSLV_LOG2_TOTAL(this, "CFG_BUS0_XSLV_LOG2_TOTAL", "3", "$clog2(CFG_BUS0_XSLV_TOTAL + 1)"),
    _vec0_(this),
    bus0_xmst_in_vector_def_(this, "bus0_xmst_in_vector"),
    bus0_xmst_out_vector_def_(this, "bus0_xmst_out_vector"),
    bus0_xslv_in_vector_def_(this, "bus0_xslv_in_vector"),
    bus0_xslv_out_vector_def_(this, "bus0_xslv_out_vector"),
    bus0_mapinfo_vector_def_(this, "bus0_mapinfo_vector"),
    _map0_(this),
    _map1_(this, "Bus 0 device tree"),
    CFG_BUS0_MAP(this),
    _n_(this)
{
}

