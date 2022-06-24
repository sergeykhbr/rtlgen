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

types_amba::types_amba(GenObject *parent) :
    FileObject(parent, "types_amba"),
    CFG_BUS_ADDR_WIDTH(this, "CFG_BUS_ADDR_WIDTH", new I32D("64"), ""),
    _Memtype0_(this),
    REQ_MEM_TYPE_WRITE(this, "REQ_MEM_TYPE_WRITE", new I32D("0")),
    REQ_MEM_TYPE_CACHED(this, "REQ_MEM_TYPE_CACHED", new I32D("1")),
    REQ_MEM_TYPE_UNIQUE(this, "REQ_MEM_TYPE_UNIQUE", new I32D("2")),
    REQ_MEM_TYPE_BITS(this, "REQ_MEM_TYPE_BITS", new I32D("3")),
    _Snoop0_(this),
    ReadNoSnoop(this),
    _Snoop1_(this),
    ReadShared(this),
    _Snoop2_(this),
    ReadMakeUnique(this),
    _Snoop3_(this),
    WriteNoSnoop(this),
    _Snoop4_(this),
    WriteLineUnique(this),
    _Snoop5_(this),
    WriteBack(this),
    _1_(this)
{
}
