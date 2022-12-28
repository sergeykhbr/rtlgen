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
    _pslv5_(this, "@brief UART0 APB device."),
    CFG_BUS1_PSLV_UART1(this, "CFG_BUS1_PSLV_UART1", "0"),
    _pslv6_(this, "@brief System status and control registers device."),
    CFG_BUS1_PSLV_PRCI(this, "CFG_BUS1_PSLV_PRCI", "1"),
    _pslv7_(this, "@brief Worjgroup DMI interface."),
    CFG_BUS1_PSLV_DMI(this, "CFG_BUS1_PSLV_DMI", "2"),
    _pslv8_(this, "Configuration index of the SPI SD-card."),
    CFG_BUS1_PSLV_SPI(this, "CFG_BUS1_PSLV_SPI", "3"),
    _pslv9_(this, "Configuration index of the GPIO (General Purpose In/Out) module."),
    CFG_BUS1_PSLV_GPIO(this, "CFG_BUS1_PSLV_GPIO", "4"),
    _pslv10_(this, "@brief DDR control register."),
    CFG_BUS1_PSLV_DDR(this, "CFG_BUS1_PSLV_DDR", "5"),
    _xslv11_(this, "Configuration index of the Plug-n-Play module."),
    CFG_BUS1_PSLV_PNP(this, "CFG_BUS1_PSLV_PNP", "6"),
    _pslv12_(this, "Total number of the APB slaves devices on Bus[1]."),
    CFG_BUS1_PSLV_TOTAL(this, "CFG_BUS1_PSLV_TOTAL", "7"),
    _vec0_(this),
    bus1_apb_in_vector_def_(this, ""),
    bus1_apb_out_vector_def_(this, ""),
    bus1_mapinfo_vector_def_(this, ""),
    _map0_(this),
    _map1_(this, "Bus 1 device tree"),
    CFG_BUS1_MAP(this),
    _n_(this)
{
    glob_bus1_cfg_ = this;
}

