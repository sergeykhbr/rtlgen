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

#include "types_bus1.h"

types_bus1* glob_bus1_cfg_ = 0;

types_bus1::types_bus1(GenObject *parent) :
    FileObject(parent, "types_bus1"),
    _pslv0_(this, "@defgroup slave_id_group AMBA APB slaves generic IDs."),
    _pslv1_(this, "@details Each module in a SoC has to be indexed by unique identificator."),
    _pslv2_(this, "         In current implementation it is used sequential indexing for it."),
    _pslv3_(this, "         Indexes are used to specify a device bus item in a vectors."),
    _pslv4_(this),
    _pslv5_(this, "@brief Worjgroup DMI interface."),
    CFG_BUS1_PSLV0_DMI(this, "CFG_BUS1_PSLV0_DMI", "0"),
    _pslv6_(this, "@brief UART0 APB device."),
    CFG_BUS1_PSLV1_UART1(this, "CFG_BUS1_PSLV1_UART1", "1"),
    _pslv7_(this, "Total number of the APB slaves devices on Bus[1]."),
    CFG_BUS1_PSLV_TOTAL(this, "CFG_BUS1_PSLV_TOTAL", "2"),
    _pmst0_(this),
    _pmst1_(this, "@defgroup master_id_group APB masters generic IDs."),
    _pmst2_(this, "@details Each master must be assigned to a specific ID that used"),
    _pmst3_(this, "         as an index in the vector array of APB master bus."),
    _pmst4_(this, ""),
    _pmst5_(this, "Bus[0] master interface"),
    CFG_BUS1_PMST_BUS0(this, "CFG_BUS1_PMST_BUS0", "0"),
    _pmst6_(this, "Total Number of master devices that have access to APB Bus[1]."),
    CFG_BUS1_PMST_TOTAL(this, "CFG_BUS1_PMST_TOTAL", "1"),
    _vec0_(this),
    bus1_pslv_in_vector_def_(this, ""),
    bus1_pslv_out_vector_def_(this, ""),
    bus1_pmst_in_vector_def_(this, ""),
    bus1_pmst_out_vector_def_(this, ""),
    _n_(this)
{
    glob_bus1_cfg_ = this;
}

