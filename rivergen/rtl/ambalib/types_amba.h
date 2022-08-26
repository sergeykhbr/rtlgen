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

#pragma once

#include <api.h>

using namespace sysvc;

class types_amba : public FileObject {
 public:
    types_amba(GenObject *parent);

    class axi4_metadata_type : public StructObject {
    public:
        axi4_metadata_type(GenObject* parent, const char* name = "", const char* comment = "")
            : StructObject(parent, "axi4_metadata_type", name, -1, comment),
            addr("CFG_SYSBUS_ADDR_WIDTH", "addr", "0", this),
            _len0_(this, "@brief   Burst length."),
            _len1_(this, "@details This signal indicates the exact number of transfers in"),
            _len2_(this, "         a burst. This changes between AXI3 and AXI4. nastiXLenBits=8 so"),
            _len3_(this, "         this is an AXI4 implementation."),
            _len4_(this, "             Burst_Length = len[7:0] + 1"),
            len("8", "len", "0", this),
            _size0_(this, "@brief   Burst size."),
            _size1_(this, "@details This signal indicates the size of each transfer"),
            _size2_(this, "         in the burst: 0=1 byte; ..., 6=64 bytes; 7=128 bytes;"),
            size("3", "size", "0", this),
            _burst0_(this, "@brief   Read response."),
            _burst1_(this, "@details This signal indicates the status of the read transfer."),
            _burst2_(this, "The responses are:"),
            _burst3_(this, "     0b00 FIXED - In a fixed burst, the address is the same for every transfer"),
            _burst4_(this, "                 in the burst. Typically is used for FIFO."),
            _burst5_(this, "     0b01 INCR - Incrementing. In an incrementing burst, the address for each"),
            _burst6_(this, "                 transfer in the burst is an increment of the address for the"),
            _burst7_(this, "                 previous transfer. The increment value depends on the size of"),
            _burst8_(this, "                 the transfer."),
            _burst9_(this, "     0b10 WRAP - A wrapping burst is similar to an incrementing burst, except"),
            _burst10_(this, "                 that the address wraps around to a lower address if an upper address"),
            _burst11_(this, "                 limit is reached."),
            _burst12_(this, "     0b11 resrved."),
            burst("2", "burst", "AXI_BURST_INCR", this),
            lock("1", "lock", "0", this),
            cache("4", "cache", "0", this),
            _prot0_(this, "@brief   Protection type."),
            _prot1_(this, "@details This signal indicates the privilege and security level"),
            _prot2_(this, "         of the transaction, and whether the transaction is a data access"),
            _prot3_(this, "         or an instruction access:"),
            _prot4_(this, " [0] :   0 = Unpriviledge access"),
            _prot5_(this, "         1 = Priviledge access"),
            _prot6_(this, " [1] :   0 = Secure access"),
            _prot7_(this, "         1 = Non-secure access"),
            _prot8_(this, " [2] :   0 = Data access"),
            _prot9_(this, "         1 = Instruction access"),
            prot("3", "prot", "0", this),
            _qos0_(this, "@brief   Quality of Service, QoS."),
            _qos1_(this, "@details QoS identifier sent for each read transaction."),
            _qos2_(this, "         Implemented only in AXI4:"),
            _qos3_(this, "             0b0000 - default value. Indicates that the interface is"),
            _qos4_(this, "                      not participating in any QoS scheme."),
            qos("4", "qos", "0", this),
            _region0_(this, "@brief Region identifier."),
            _region1_(this, "@details Permits a single physical interface on a slave to be used for"),
            _region2_(this, "         multiple logical interfaces. Implemented only in AXI4. This is"),
            _region3_(this, "         similar to the banks implementation in Leon3 without address"),
            _region4_(this, "         decoding."),
            region("4", "region", "0", this) {
    }

    public:
        Logic addr;
        TextLine _len0_;
        TextLine _len1_;
        TextLine _len2_;
        TextLine _len3_;
        TextLine _len4_;
        Logic len;
        TextLine _size0_;
        TextLine _size1_;
        TextLine _size2_;
        Logic size;
        TextLine _burst0_;
        TextLine _burst1_;
        TextLine _burst2_;
        TextLine _burst3_;
        TextLine _burst4_;
        TextLine _burst5_;
        TextLine _burst6_;
        TextLine _burst7_;
        TextLine _burst8_;
        TextLine _burst9_;
        TextLine _burst10_;
        TextLine _burst11_;
        TextLine _burst12_;
        Logic burst;
        Logic lock;
        Logic cache;
        TextLine _prot0_;
        TextLine _prot1_;
        TextLine _prot2_;
        TextLine _prot3_;
        TextLine _prot4_;
        TextLine _prot5_;
        TextLine _prot6_;
        TextLine _prot7_;
        TextLine _prot8_;
        TextLine _prot9_;
        Logic prot;
        TextLine _qos0_;
        TextLine _qos1_;
        TextLine _qos2_;
        TextLine _qos3_;
        TextLine _qos4_;
        Logic qos;
        TextLine _region0_;
        TextLine _region1_;
        TextLine _region2_;
        TextLine _region3_;
        TextLine _region4_;
        Logic region;
    };

 public:
    ParamI32D CFG_SYSBUS_ADDR_WIDTH;
    TextLine _1_;
    TextLine _burst0_;
    TextLine _burst1_;
    TextLine _burst2_;
    ParamLogic AXI_BURST_FIXED;
    TextLine _burst3_;
    TextLine _burst4_;
    TextLine _burst5_;
    TextLine _burst6_;
    ParamLogic AXI_BURST_INCR;
    TextLine _burst7_;
    TextLine _burst8_;
    TextLine _burst9_;
    TextLine _burst10_;
    ParamLogic AXI_BURST_WRAP;
    TextLine _burst11_;
    TextLine _meta0_;
    axi4_metadata_type axi4_metadata_type_def;
    TextLine _meta1_;
    axi4_metadata_type META_NONE;
    TextLine _n_;
};


