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

#include "cache_top.h"

CacheTop::CacheTop(GenObject *parent) :
    ModuleObject(parent, "CacheTop"),
    i_clk(this, "i_clk", new Logic(), "CPU clock"),
    i_nrst(this, "i_nrst", new Logic(), "Reset: active LOW"),
    _MemInterface0_(this, "Memory interface:"),
    i_req_mem_ready(this, "i_req_mem_ready", new Logic(), "System Bus is ready to accept memory operation request"),
    o_req_mem_path(this, "o_req_mem_path", new Logic(), "0=ctrl; 1=data path"),
    o_req_mem_valid(this, "o_req_mem_valid", new Logic(), "AXI memory request is valid"),
    o_req_mem_type(this, "o_req_mem_type", new Logic("REQ_MEM_TYPE_BITS"), "AXI memory request type"),
    o_req_mem_size(this, "o_req_mem_size", new Logic("3"), "request size: 0=1 B;...; 7=128 B"),
    o_req_mem_addr(this, "o_req_mem_addr", new Logic("CFG_CPU_ADDR_BITS"), "AXI memory request address"),
    o_req_mem_strob(this, "o_req_mem_strob", new Logic("L1CACHE_BYTES_PER_LINE"), "Writing strob. 1 bit per Byte (uncached only)"),
    o_req_mem_data(this, "o_req_mem_data", new Logic("L1CACHE_LINE_BITS"), "Writing data"),
    i_resp_mem_valid(this, "i_resp_mem_valid", new Logic(), "AXI response is valid"),
    i_resp_mem_path(this, "i_resp_mem_path", new Logic(), "0=ctrl; 1=data path"),
    i_resp_mem_data(this, "i_resp_mem_data", new Logic("L1CACHE_LINE_BITS"), "Read data"),
    i_resp_mem_load_fault(this, "i_resp_mem_load_fault", new Logic(), "data load error"),
    i_resp_mem_store_fault(this, "i_resp_mem_store_fault", new Logic(), "data store error"),
    _DSnoopInterface0_(this, "$D Snoop interface:"),
    i_req_snoop_valid(this, "i_req_snoop_valid", new Logic()),
    i_req_snoop_type(this, "i_req_snoop_type", new Logic("SNOOP_REQ_TYPE_BITS")),
    o_req_snoop_ready(this, "o_req_snoop_ready", new Logic(), ""),
    i_req_snoop_addr(this, "i_req_snoop_addr", new Logic("CFG_CPU_ADDR_BITS")),
    i_resp_snoop_ready(this, "i_resp_snoop_ready", new Logic()),
    o_resp_snoop_valid(this, "o_resp_snoop_valid", new Logic(), ""),
    o_resp_snoop_data(this, "o_resp_snoop_data", new Logic("L1CACHE_LINE_BITS")),
    o_resp_snoop_flags(this, "o_resp_snoop_flags", new Logic("DTAG_FL_TOTAL")),

    test(this, "test", new Logic("32"), "test register")
{
}

cache_top::cache_top(GenObject *parent) :
    FileObject(parent, "cache_top"),
    cache_(this)
{
}
