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
    coherence_ena(this, "coherence_ena", new BOOL("false")),
    i_clk(this, "i_clk", new Logic(), "CPU clock"),
    i_nrst(this, "i_nrst", new Logic(), "Reset: active LOW"),
    _ControlPath0_(this, "Control path:"),
    i_req_ctrl_valid(this, "i_req_ctrl_valid", new Logic(), "Control request from CPU Core is valid"),
    i_req_ctrl_addr(this, "i_req_ctrl_addr", new Logic("CFG_CPU_ADDR_BITS"), "Control request address"),
    o_req_ctrl_ready(this, "o_req_ctrl_ready", new Logic(), "Control request from CPU Core is accepted"),
    o_resp_ctrl_valid(this, "o_resp_ctrl_valid", new Logic(), "ICache response is valid and can be accepted"),
    o_resp_ctrl_addr(this, "o_resp_ctrl_addr", new Logic("CFG_CPU_ADDR_BITS"), "ICache response address"),
    o_resp_ctrl_data(this, "o_resp_ctrl_data", new Logic("64"), "ICache read data"),
    o_resp_ctrl_load_fault(this, "o_resp_ctrl_load_fault", new Logic(), "Bus response ERRSLV or ERRDEC on read"),
    o_resp_ctrl_executable(this, "o_resp_ctrl_executable", new Logic(), "MPU flag: executable"),
    i_resp_ctrl_ready(this, "i_resp_ctrl_ready", new Logic(), "CPU Core is ready to accept ICache response"),
    _DataPath0_(this, "Data path:"),
    i_req_data_valid(this, "i_req_data_valid", new Logic(), "Data path request from CPU Core is valid"),
    i_req_data_type(this, "i_req_data_type", new Logic("MemopType_Total"), "Data write memopy operation flag"),
    i_req_data_addr(this, "i_req_data_addr", new Logic("CFG_CPU_ADDR_BITS"), "Memory operation address"),
    i_req_data_wdata(this, "i_req_data_wdata", new Logic("64"), "Memory operation write value"),
    i_req_data_wstrb(this, "i_req_data_wstrb", new Logic("8"), "8-bytes aligned strob"),
    i_req_data_size(this, "i_req_data_size", new Logic("2")),
    o_req_data_ready(this, "o_req_data_ready", new Logic(), "Memory operation request accepted by DCache"),
    o_resp_data_valid(this, "o_resp_data_valid", new Logic(), "DCache response is ready"),
    o_resp_data_addr(this, "o_resp_data_addr", new Logic("CFG_CPU_ADDR_BITS"), "DCache response address"),
    o_resp_data_data(this, "o_resp_data_data", new Logic("64"), "DCache response read data"),
    o_resp_data_fault_addr(this, "o_resp_data_fault_addr", new Logic("CFG_CPU_ADDR_BITS"), "AXI B-channel error"),
    o_resp_data_load_fault(this, "o_resp_data_load_fault", new Logic(), "Bus response ERRSLV or ERRDEC on read"),
    o_resp_data_store_fault(this, "o_resp_data_store_fault", new Logic(), "Bus response ERRSLV or ERRDEC on write"),
    o_resp_data_er_mpu_load(this, "o_resp_data_er_mpu_load", new Logic(), ""),
    o_resp_data_er_mpu_store(this, "o_resp_data_er_mpu_store", new Logic(), ""),
    i_resp_data_ready(this, "i_resp_data_ready", new Logic(), "CPU Core is ready to accept DCache repsonse"),
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
    _MpuInterface0_(this, "MPU interface:"),
    i_mpu_region_we(this, "i_mpu_region_we", new Logic(), ""),
    i_mpu_region_idx(this, "i_mpu_region_idx", new Logic("CFG_MPU_TBL_WIDTH"), ""),
    i_mpu_region_addr(this, "i_mpu_region_addr", new Logic("CFG_CPU_ADDR_BITS"), ""),
    i_mpu_region_mask(this, "i_mpu_region_mask", new Logic("CFG_CPU_ADDR_BITS"), ""),
    i_mpu_region_flags(this, "i_mpu_region_flags", new Logic("CFG_MPU_FL_TOTAL"), "{ena, cachable, r, w, x}"),
    _DSnoopInterface0_(this, "$D Snoop interface:"),
    i_req_snoop_valid(this, "i_req_snoop_valid", new Logic()),
    i_req_snoop_type(this, "i_req_snoop_type", new Logic("SNOOP_REQ_TYPE_BITS")),
    o_req_snoop_ready(this, "o_req_snoop_ready", new Logic(), ""),
    i_req_snoop_addr(this, "i_req_snoop_addr", new Logic("CFG_CPU_ADDR_BITS")),
    i_resp_snoop_ready(this, "i_resp_snoop_ready", new Logic()),
    o_resp_snoop_valid(this, "o_resp_snoop_valid", new Logic(), ""),
    o_resp_snoop_data(this, "o_resp_snoop_data", new Logic("L1CACHE_LINE_BITS")),
    o_resp_snoop_flags(this, "o_resp_snoop_flags", new Logic("DTAG_FL_TOTAL")),
    _DebugSignals0_(this, "Debug signals:"),
    i_flush_address(this, "i_flush_address", new Logic("CFG_CPU_ADDR_BITS"), "clear ICache address from debug interface"),
    i_flush_valid(this, "i_flush_valid", new Logic(), "address to clear icache is valid"),
    i_data_flush_address(this, "i_data_flush_address", new Logic("CFG_CPU_ADDR_BITS"), ""),
    i_data_flush_valid(this, "i_data_flush_valid", new Logic(), ""),
    o_data_flush_end(this, "o_data_flush_end", new Logic(), ""),

    test(this, "test", new Logic("32"), "test register")
{
}

cache_top::cache_top(GenObject *parent) :
    FileObject(parent, "cache_top"),
    cache_(this)
{
}
