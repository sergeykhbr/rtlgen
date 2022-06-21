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
    // Generic parameters
    hartid(this, "hartid", new UI32D("0")),
    fpu_ena(this, "fpu_ena", new BOOL("true")),
    coherence_ena(this, "coherence_ena", new BOOL("false")),
    tracer_ena(this, "tracer_ena", new BOOL("true")),
    // Ports
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
    _Interrupts0_(this, "Interrupt line from external interrupts controller (PLIC):"),
    i_msip(this, "i_msip", new Logic(), "machine software pending interrupt"),
    i_mtip(this, "i_mtip", new Logic(), "machine timer pending interrupt"),
    i_meip(this, "i_meip", new Logic(), "machine external pending interrupt"),
    i_seip(this, "i_seip", new Logic(), "supervisor external pending interrupt"),
    _Debug0(this, "Debug interface:"),
    i_haltreq(this, "i_haltreq", new Logic(), "DMI: halt request from debug unit"),
    i_resumereq(this, "i_resumereq", new Logic(), "DMI: resume request from debug unit"),
    i_dport_req_valid(this, "i_dport_req_valid", new Logic(), "Debug access from DSU is valid"),
    i_dport_type(this, "i_dport_type", new Logic("DPortReq_Total"), "Debug access type"),
    i_dport_addr(this, "i_dport_addr", new Logic("CFG_CPU_ADDR_BITS"), "dport address"),
    i_dport_wdata(this, "i_dport_wdata", new Logic("RISCV_ARCH"), "Write value"),
    i_dport_size(this, "i_dport_size", new Logic("3"), "reg/mem access size:0=1B;...,4=128B;"),
    o_dport_req_ready(this, "o_dport_req_ready", new Logic(), ""),
    i_dport_resp_ready(this, "i_dport_resp_ready", new Logic(), "ready to accepd response"),
    o_dport_resp_valid(this, "o_dport_resp_valid", new Logic(), "Response is valid"),
    o_dport_resp_error(this, "o_dport_resp_error", new Logic(), "Something wrong during command execution"),
    o_dport_rdata(this, "o_dport_rdata", new Logic("RISCV_ARCH"), "Response value"),
    i_progbuf(this, "i_progbuf", new Logic("MUL(32,CFG_PROGBUF_REG_TOTAL)"), "progam buffer"),
    o_halted(this, "o_halted", new Logic(), "CPU halted via debug interface"),
    // Singals:
    _ControlPath0_(this, "Control path:"),
    w_req_ctrl_ready(this, "w_req_ctrl_ready", new Logic()),
    w_req_ctrl_valid(this, "w_req_ctrl_valid", new Logic()),
    wb_req_ctrl_addr(this, "wb_req_ctrl_addr", new Logic("CFG_CPU_ADDR_BITS")),
    w_resp_ctrl_valid(this, "w_resp_ctrl_valid", new Logic()),
    wb_resp_ctrl_addr(this, "wb_resp_ctrl_addr", new Logic("CFG_CPU_ADDR_BITS")),
    wb_resp_ctrl_data(this, "wb_resp_ctrl_data", new Logic("64")),
    w_resp_ctrl_load_fault(this, "w_resp_ctrl_load_fault", new Logic()),
    w_resp_ctrl_executable(this, "w_resp_ctrl_executable", new Logic()),
    w_resp_ctrl_ready(this, "w_resp_ctrl_ready", new Logic()),
    _DataPath0_(this, "Data path:"),
    w_req_data_ready(this, "w_req_data_ready", new Logic()),
    w_req_data_valid(this, "w_req_data_valid", new Logic()),
    wb_req_data_type(this, "wb_req_data_type", new Logic("MemopType_Total")),
    wb_req_data_addr(this, "wb_req_data_addr", new Logic("CFG_CPU_ADDR_BITS")),
    wb_req_data_wdata(this, "wb_req_data_wdata", new Logic("64")),
    wb_req_data_wstrb(this, "wb_req_data_wstrb", new Logic("8")),
    wb_req_data_size(this, "wb_req_data_size", new Logic("2")),
    w_resp_data_valid(this, "w_resp_data_valid", new Logic()),
    wb_resp_data_addr(this, "wb_resp_data_addr", new Logic("CFG_CPU_ADDR_BITS")),
    wb_resp_data_data(this, "wb_resp_data_data", new Logic("64")),
    w_resp_data_load_fault(this, "w_resp_data_load_fault", new Logic()),
    w_resp_data_store_fault(this, "w_resp_data_store_fault", new Logic()),
    w_resp_data_er_mpu_load(this, "w_resp_data_er_mpu_load", new Logic()),
    w_resp_data_er_mpu_store(this, "w_resp_data_er_mpu_store", new Logic()),
    wb_resp_data_fault_addr(this, "wb_resp_data_fault_addr", new Logic("CFG_CPU_ADDR_BITS")),
    w_resp_data_ready(this, "w_resp_data_ready", new Logic()),
    w_mpu_region_we(this, "w_mpu_region_we", new Logic()),
    wb_mpu_region_idx(this, "wb_mpu_region_idx", new Logic("CFG_MPU_TBL_WIDTH")),
    wb_mpu_region_addr(this, "wb_mpu_region_addr", new Logic("CFG_CPU_ADDR_BITS")),
    wb_mpu_region_mask(this, "wb_mpu_region_mask", new Logic("CFG_CPU_ADDR_BITS")),
    wb_mpu_region_flags(this, "wb_mpu_region_flags", new Logic("CFG_MPU_FL_TOTAL")),
    wb_flush_address(this, "wb_flush_address", new Logic("CFG_CPU_ADDR_BITS")),
    w_flush_valid(this, "w_flush_valid", new Logic()),
    wb_data_flush_address(this, "wb_data_flush_address", new Logic("CFG_CPU_ADDR_BITS")),
    w_data_flush_valid(this, "w_data_flush_valid", new Logic()),
    w_data_flush_end(this, "w_data_flush_end", new Logic())
{
    // Create and connet Sub-modules:
    ModuleObject *p = static_cast<ModuleObject *>(SCV_get_module("Processor"));
    if (p) {
        proc0 = p->createInstance(this, "proc0");
    } else {
        SHOW_ERROR();
        proc0 = 0;
    }
    p = static_cast<ModuleObject *>(SCV_get_module("CacheTop"));
    if (p) {
        cache0 = p->createInstance(this, "cache0");
    } else {
        SHOW_ERROR();
        cache0 = 0;
    }
}

river_top::river_top(GenObject *parent) :
    FileObject(parent, "river_top"),
    top_(this)
{
}
