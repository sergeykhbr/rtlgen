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

#include "types_amba.h"

types_amba* glob_types_amba_ = 0;

types_amba::types_amba(GenObject *parent) :
    FileObject(parent, "types_amba"),
    CFG_SYSBUS_ADDR_WIDTH(this, "CFG_SYSBUS_ADDR_WIDTH", "64"),
    CFG_LOG2_SYSBUS_DATA_BYTES(this, "CFG_LOG2_SYSBUS_DATA_BYTES", "3"),
    CFG_SYSBUS_ID_BITS(this, "CFG_SYSBUS_ID_BITS", "5"),
    CFG_SYSBUS_USER_BITS(this, "CFG_SYSBUS_USER_BITS", "1"),
    _cfgbus0_(this),
    CFG_SYSBUS_DATA_BYTES(this, "CFG_SYSBUS_DATA_BYTES", "POW2(1,CFG_LOG2_SYSBUS_DATA_BYTES)"),
    CFG_SYSBUS_DATA_BITS(this, "CFG_SYSBUS_DATA_BITS", "MUL(8,CFG_LOG2_SYSBUS_DATA_BYTES)"),
    _1_(this),
    _burst0_(this, "@brief Fixed address burst operation."),
    _burst1_(this, "@details The address is the same for every transfer in the burst"),
    _burst2_(this, "         (FIFO type)"),
    AXI_BURST_FIXED(this, "2", "AXI_BURST_FIXED", "0"),
    _burst3_(this, "@brief Burst operation with address increment."),
    _burst4_(this, "@details The address for each transfer in the burst is an increment of"),
    _burst5_(this, "        the address for the previous transfer. The increment value depends"),
    _burst6_(this, "       on the size of the transfer."),
    AXI_BURST_INCR(this, "2", "AXI_BURST_INCR", "1"),
    _burst7_(this, "@brief Burst operation with address increment and wrapping."),
    _burst8_(this, "@details A wrapping burst is similar to an incrementing burst, except that"),
    _burst9_(this, "         the address wraps around to a lower address if an upper address"),
    _burst10_(this, "         limit is reached"),
    AXI_BURST_WRAP(this, "2", "AXI_BURST_WRAP", "2"),
    _burst11_(this, "@}"),
    _meta0_(this),
    axi4_metadata_type_def(this),
    _meta1_(this),
    META_NONE(this, "META_NONE"),
    _xmst0_(this),
    _xmst1_(this),
    axi4_master_out_type_def(this, ""),
    _xmst2_(this),
    _xmst3_(this, "@brief   Master device empty value."),
    _xmst4_(this, "@warning If the master is not connected to the vector begin vector value"),
    _xmst5_(this, "         MUST BE initialized by this value."),
    axi4_master_out_none(this, "axi4_master_out_none"),
    _xmst6_(this),
    _xmst7_(this, "@brief Master device input signals."),
    axi4_master_in_type_def(this, ""),
    _xmst8_(this),
    axi4_master_in_none(this, "axi4_master_in_none"),
    _n_(this)
{
    glob_types_amba_ = this;
}
