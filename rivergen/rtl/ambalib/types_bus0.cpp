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
    _xslv0_(this, ""),
    _xslv1_(this, ""),
    _xslv2_(this, ""),
    _xslv3_(this, ""),
    CFG_BUS0_XSLV_TOTAL(this, "CFG_BUS0_XSLV_TOTAL", "9"),
    _xmst0_(this, ""),
    _xmst1_(this, "@defgroup master_id_group AXI4 masters generic IDs."),
    _xmst2_(this, "@details Each master must be assigned to a specific ID that used"),
    _xmst3_(this, "         as an index in the vector array of AXI master bus."),
    _xmst4_(this, ""),
    _xmst5_(this, "Total number of CPU limited by config CFG_TOTAL_CPU_MAX"),
    CFG_BUS0_XMST_CPU0(this, "CFG_BUS0_XMST_CPU0", "0"),
    _xmst6_(this, "DMA master interface generic index."),
    CFG_BUS0_XMST_DMA(this, "CFG_BUS0_XMST_DMA", "1"),
    _xmst7_(this, "Total Number of master devices on system bus."),
    CFG_BUS0_XMST_TOTAL(this, "CFG_BUS0_XMST_TOTAL", "2"),
    _vec0_(this, ""),
    bus0_xslv_cfg_vector_def_(this, ""),
    bus0_xmst_cfg_vector_def_(this, ""),
    bus0_xmst_in_vector_def_(this, ""),
    bus0_xmst_out_vector_def_(this, ""),
    bus0_xslv_in_vector_def_(this, ""),
    bus0_xslv_out_vector_def_(this, ""),
    _n_(this)
{
    glob_bus0_cfg_ = this;
}

