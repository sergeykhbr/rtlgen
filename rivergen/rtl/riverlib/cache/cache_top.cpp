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

CacheTop::CacheTop(GenObject *parent, const char *name) :
    ModuleObject(parent, "CacheTop", name),
    coherence_ena(this, "coherence_ena", "false"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    _ControlPath0_(this, "Control path:"),
    i_req_ctrl_valid(this, "i_req_ctrl_valid", "1", "Control request from CPU Core is valid"),
    i_req_ctrl_addr(this, "i_req_ctrl_addr", "CFG_CPU_ADDR_BITS", "Control request address"),
    o_req_ctrl_ready(this, "o_req_ctrl_ready", "1", "Control request from CPU Core is accepted"),
    o_resp_ctrl_valid(this, "o_resp_ctrl_valid", "1", "ICache response is valid and can be accepted"),
    o_resp_ctrl_addr(this, "o_resp_ctrl_addr", "CFG_CPU_ADDR_BITS", "ICache response address"),
    o_resp_ctrl_data(this, "o_resp_ctrl_data", "64", "ICache read data"),
    o_resp_ctrl_load_fault(this, "o_resp_ctrl_load_fault", "1", "Bus response ERRSLV or ERRDEC on read"),
    o_resp_ctrl_executable(this, "o_resp_ctrl_executable", "1", "MPU flag: executable"),
    i_resp_ctrl_ready(this, "i_resp_ctrl_ready", "1", "CPU Core is ready to accept ICache response"),
    _DataPath0_(this, "Data path:"),
    i_req_data_valid(this, "i_req_data_valid", "1", "Data path request from CPU Core is valid"),
    i_req_data_type(this, "i_req_data_type", "MemopType_Total", "Data write memopy operation flag"),
    i_req_data_addr(this, "i_req_data_addr", "CFG_CPU_ADDR_BITS", "Memory operation address"),
    i_req_data_wdata(this, "i_req_data_wdata", "64", "Memory operation write value"),
    i_req_data_wstrb(this, "i_req_data_wstrb", "8", "8-bytes aligned strob"),
    i_req_data_size(this, "i_req_data_size", "2"),
    o_req_data_ready(this, "o_req_data_ready", "1", "Memory operation request accepted by DCache"),
    o_resp_data_valid(this, "o_resp_data_valid", "1", "DCache response is ready"),
    o_resp_data_addr(this, "o_resp_data_addr", "CFG_CPU_ADDR_BITS", "DCache response address"),
    o_resp_data_data(this, "o_resp_data_data", "64", "DCache response read data"),
    o_resp_data_fault_addr(this, "o_resp_data_fault_addr", "CFG_CPU_ADDR_BITS", "AXI B-channel error"),
    o_resp_data_load_fault(this, "o_resp_data_load_fault", "1", "Bus response ERRSLV or ERRDEC on read"),
    o_resp_data_store_fault(this, "o_resp_data_store_fault", "1", "Bus response ERRSLV or ERRDEC on write"),
    o_resp_data_er_mpu_load(this, "o_resp_data_er_mpu_load"),
    o_resp_data_er_mpu_store(this, "o_resp_data_er_mpu_store"),
    i_resp_data_ready(this, "i_resp_data_ready", "1", "CPU Core is ready to accept DCache repsonse"),
    _MemInterface0_(this, "Memory interface:"),
    i_req_mem_ready(this, "i_req_mem_ready", "1", "System Bus is ready to accept memory operation request"),
    o_req_mem_path(this, "o_req_mem_path", "1", "0=ctrl; 1=data path"),
    o_req_mem_valid(this, "o_req_mem_valid", "1", "AXI memory request is valid"),
    o_req_mem_type(this, "o_req_mem_type", "REQ_MEM_TYPE_BITS", "AXI memory request type"),
    o_req_mem_size(this, "o_req_mem_size", "3", "request size: 0=1 B;...; 7=128 B"),
    o_req_mem_addr(this, "o_req_mem_addr", "CFG_CPU_ADDR_BITS", "AXI memory request address"),
    o_req_mem_strob(this, "o_req_mem_strob", "L1CACHE_BYTES_PER_LINE", "Writing strob. 1 bit per Byte (uncached only)"),
    o_req_mem_data(this, "o_req_mem_data", "L1CACHE_LINE_BITS", "Writing data"),
    i_resp_mem_valid(this, "i_resp_mem_valid", "1", "AXI response is valid"),
    i_resp_mem_path(this, "i_resp_mem_path", "1", "0=ctrl; 1=data path"),
    i_resp_mem_data(this, "i_resp_mem_data", "L1CACHE_LINE_BITS", "Read data"),
    i_resp_mem_load_fault(this, "i_resp_mem_load_fault", "1", "data load error"),
    i_resp_mem_store_fault(this, "i_resp_mem_store_fault", "1", "data store error"),
    _MpuInterface0_(this, "MPU interface:"),
    i_mpu_region_we(this, "i_mpu_region_we"),
    i_mpu_region_idx(this, "i_mpu_region_idx", "CFG_MPU_TBL_WIDTH", ""),
    i_mpu_region_addr(this, "i_mpu_region_addr", "CFG_CPU_ADDR_BITS", ""),
    i_mpu_region_mask(this, "i_mpu_region_mask", "CFG_CPU_ADDR_BITS", ""),
    i_mpu_region_flags(this, "i_mpu_region_flags", "CFG_MPU_FL_TOTAL", "{ena, cachable, r, w, x}"),
    _DSnoopInterface0_(this, "$D Snoop interface:"),
    i_req_snoop_valid(this, "i_req_snoop_valid"),
    i_req_snoop_type(this, "i_req_snoop_type", "SNOOP_REQ_TYPE_BITS"),
    o_req_snoop_ready(this, "o_req_snoop_ready"),
    i_req_snoop_addr(this, "i_req_snoop_addr", "CFG_CPU_ADDR_BITS"),
    i_resp_snoop_ready(this, "i_resp_snoop_ready"),
    o_resp_snoop_valid(this, "o_resp_snoop_valid"),
    o_resp_snoop_data(this, "o_resp_snoop_data", "L1CACHE_LINE_BITS"),
    o_resp_snoop_flags(this, "o_resp_snoop_flags", "DTAG_FL_TOTAL"),
    _DebugSignals0_(this, "Debug signals:"),
    i_flush_address(this, "i_flush_address", "CFG_CPU_ADDR_BITS", "clear ICache address from debug interface"),
    i_flush_valid(this, "i_flush_valid", "1", "address to clear icache is valid"),
    i_data_flush_address(this, "i_data_flush_address", "CFG_CPU_ADDR_BITS", ""),
    i_data_flush_valid(this, "i_data_flush_valid"),
    o_data_flush_end(this, "o_data_flush_end"),

    test(this, "test", "32", "0", "test register")
{
}
