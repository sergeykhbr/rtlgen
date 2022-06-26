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

#include "proc.h"

Processor::Processor(GenObject *parent) :
    ModuleObject(parent, "Processor"),
    hartid(this, "hartid", new UI32D("0")),
    fpu_ena(this, "fpu_ena", new BOOL("true")),
    tracer_ena(this, "tracer_ena", new BOOL("true")),
    i_clk(this, "i_clk", new Logic(), "CPU clock"),
    i_nrst(this, "i_nrst", new Logic(), "Reset: active LOW"),
    _ControlPath0_(this, "Control path:"),
    i_req_ctrl_ready(this, "i_req_ctrl_ready", new Logic(), "ICache is ready to accept request"),
    o_req_ctrl_valid(this, "o_req_ctrl_valid", new Logic(), "Request to ICache is valid"),
    o_req_ctrl_addr(this, "o_req_ctrl_addr", new Logic("CFG_CPU_ADDR_BITS"), "Requesting address to ICache"),
    i_resp_ctrl_valid(this, "i_resp_ctrl_valid", new Logic(), "ICache response is valid"),
    i_resp_ctrl_addr(this, "i_resp_ctrl_addr", new Logic("CFG_CPU_ADDR_BITS"), "Response address must be equal to the latest request address"),
    i_resp_ctrl_data(this, "i_resp_ctrl_data", new Logic("64"), "Read value"),
    i_resp_ctrl_load_fault(this, "i_resp_ctrl_load_fault", new Logic()),
    i_resp_ctrl_executable(this, "i_resp_ctrl_executable", new Logic(), "MPU flag"),
    o_resp_ctrl_ready(this, "o_resp_ctrl_ready", new Logic(), "Core is ready to accept response from ICache"),
    _DataPath0_(this, "Data path:"),
    i_req_data_ready(this, "i_req_data_ready", new Logic(), "DCache is ready to accept request"),
    o_req_data_valid(this, "o_req_data_valid", new Logic(), "Request to DCache is valid"),
    o_req_data_type(this, "o_req_data_type", new Logic("MemopType_Total"), "Read/Write transaction plus additional flags"),
    o_req_data_addr(this, "o_req_data_addr", new Logic("CFG_CPU_ADDR_BITS"), "Requesting address to DCache"),
    o_req_data_wdata(this, "o_req_data_wdata", new Logic("64"), "Writing value"),
    o_req_data_wstrb(this, "o_req_data_wstrb", new Logic("8"), "8-bytes aligned strobs"),
    o_req_data_size(this, "o_req_data_size", new Logic("2"), "memory operation 1,2,4 or 8 bytes"),
    i_resp_data_valid(this, "i_resp_data_valid", new Logic(), "DCache response is valid"),
    i_resp_data_addr(this, "i_resp_data_addr", new Logic("CFG_CPU_ADDR_BITS"), "DCache response address must be equal to the latest request address"),
    i_resp_data_data(this, "i_resp_data_data", new Logic("64"), "Read value"),
    i_resp_data_fault_addr(this, "i_resp_data_fault_addr", new Logic("CFG_CPU_ADDR_BITS"), "write-error address (B-channel)"),
    i_resp_data_load_fault(this, "i_resp_data_load_fault", new Logic(), "Bus response with SLVERR or DECERR on read"),
    i_resp_data_store_fault(this, "i_resp_data_store_fault", new Logic(), "Bus response with SLVERR or DECERR on write"),
    i_resp_data_er_mpu_load(this, "i_resp_data_er_mpu_load", new Logic(), ""),
    i_resp_data_er_mpu_store(this, "i_resp_data_er_mpu_store", new Logic(), ""),
    o_resp_data_ready(this, "o_resp_data_ready", new Logic(), "Core is ready to accept response from DCache"),
    _Interrupts0_(this, "Interrupt line from external interrupts controller (PLIC):"),
    i_msip(this, "i_msip", new Logic(), "machine software pending interrupt"),
    i_mtip(this, "i_mtip", new Logic(), "machine timer pending interrupt"),
    i_meip(this, "i_meip", new Logic(), "machine external pending interrupt"),
    i_seip(this, "i_seip", new Logic(), "supervisor external pending interrupt"),
    _MpuInterface0_(this, "MPU interface"),
    o_mpu_region_we(this, "o_mpu_region_we", new Logic()),
    o_mpu_region_idx(this, "o_mpu_region_idx", new Logic("CFG_MPU_TBL_WIDTH")),
    o_mpu_region_addr(this, "o_mpu_region_addr", new Logic("CFG_CPU_ADDR_BITS")),
    o_mpu_region_mask(this, "o_mpu_region_mask", new Logic("CFG_CPU_ADDR_BITS")),
    o_mpu_region_flags(this, "o_mpu_region_flags", new Logic("CFG_MPU_FL_TOTAL"), "{ena, cachable, r, w, x}"),
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
    _CacheDbg0_(this, "Cache debug signals:"),
    o_flush_address(this, "o_flush_address", new Logic("CFG_CPU_ADDR_BITS"), "Address of instruction to remove from ICache"),
    o_flush_valid(this, "o_flush_valid", new Logic(), "Remove address from ICache is valid"),
    o_data_flush_address(this, "o_data_flush_address", new Logic("CFG_CPU_ADDR_BITS"), "Address of instruction to remove from D$"),
    o_data_flush_valid(this, "o_data_flush_valid", new Logic(), "Remove address from D$ is valid"),
    i_data_flush_end(this, "i_data_flush_end", new Logic()),
    // Structures declaration
    /*structDefFetchType(this),
    structDefInstructionDecodeType(this),
    structDefExecuteType(this),
    structDefMemoryType(this),
    structDefWriteBackType(this),
    structDefPipelineType(this),*/
    // Signal/struct instances
    comb(this),
    w(this, "w", "5-stages CPU pipeline")
{
}

proc::proc(GenObject *parent) :
    FileObject(parent, "proc"),
    proc_(this)
{
}
