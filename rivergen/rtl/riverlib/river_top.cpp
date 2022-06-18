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

#include "river_top.h"

RiverTop::RiverTop(GenObject *parent) :
    ModuleObject(parent, "RiverTop"),
    io_start_(this),
    i_clk(this, "i_clk", new I32D(1), "CPU clock"),
    i_nrst(this, "i_nrst", new I32D(1), "Reset: active LOW"),
    _1_(this, "Memory interface:"),
    i_req_mem_ready(this, "i_req_mem_ready", new I32D(1), "System Bus is ready to accept memory operation request"),
    o_req_mem_path(this, "o_req_mem_path", new I32D(1), "0=ctrl; 1=data path"),
    o_req_mem_valid(this, "o_req_mem_valid", new I32D(1), "AXI memory request is valid"),
    o_req_mem_type(this, "o_req_mem_type", new I32D("REQ_MEM_TYPE_BITS"), "AXI memory request type"),
    o_req_mem_size(this, "o_req_mem_size", new I32D(3), "request size: 0=1 B;...; 7=128 B"),
    o_req_mem_addr(this, "o_req_mem_addr", new I32D("REQ_MEM_TYPE_BITS"), "AXI memory request address"),
    o_req_mem_strob(this, "o_req_mem_strob", new I32D("REQ_MEM_TYPE_BITS"), "Writing strob. 1 bit per Byte (uncached only)"),
    o_req_mem_data(this, "o_req_mem_data", new I32D("REQ_MEM_TYPE_BITS"), "Writing data"),
    i_resp_mem_valid(this, "i_resp_mem_valid", new I32D(1), "AXI response is valid"),
    i_resp_mem_path(this, "i_resp_mem_path", new I32D(1), "0=ctrl; 1=data path"),
    i_resp_mem_data(this, "i_resp_mem_data", new I32D("REQ_MEM_TYPE_BITS"), "Read data"),
    i_resp_mem_load_fault(this, "i_resp_mem_load_fault", new I32D(1), "data load error"),
    i_resp_mem_store_fault(this, "i_resp_mem_store_fault", new I32D(1), "data store error"),
    io_end_(this)
{
}

river_top::river_top(GenObject *parent) :
    FileObject(parent, "river_top"),
    top_(this)
{
}
