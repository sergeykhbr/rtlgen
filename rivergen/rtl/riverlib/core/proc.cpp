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
    // Signal/struct instances
    w(this, "w", "5-stages CPU pipeline"),
    ireg(this, "ireg"),
    csr(this, "csr"),
    dbg(this, "dbg"),
    bp(this, "bp"),
    // Internal signal
    _CsrBridge0_(this),
    _CsrBridge1_(this, "csr bridge to executor unit"),
    iccsr_m0_req_ready(this, "iccsr_m0_req_ready", new Logic()),
    iccsr_m0_resp_valid(this, "iccsr_m0_resp_valid", new Logic()),
    iccsr_m0_resp_data(this, "iccsr_m0_resp_data", new Logic("RISCV_ARCH")),
    iccsr_m0_resp_exception(this, "iccsr_m0_resp_exception", new Logic()),
    _CsrBridge2_(this, "csr bridge to debug unit"),
    iccsr_m1_req_ready(this, "iccsr_m1_req_ready", new Logic()),
    iccsr_m1_resp_valid(this, "iccsr_m1_resp_valid", new Logic()),
    iccsr_m1_resp_data(this, "iccsr_m1_resp_data", new Logic("RISCV_ARCH")),
    iccsr_m1_resp_exception(this, "iccsr_m1_resp_exception", new Logic()),
    _CsrBridge3_(this, "csr bridge to CSR module"),
    iccsr_s0_req_valid(this, "iccsr_s0_req_valid", new Logic()),
    iccsr_s0_req_type(this, "iccsr_s0_req_type", new Logic("CsrReq_TotalBits")),
    iccsr_s0_req_addr(this, "iccsr_s0_req_addr", new Logic("12")),
    iccsr_s0_req_data(this, "iccsr_s0_req_data", new Logic("RISCV_ARCH")),
    iccsr_s0_resp_ready(this, "iccsr_s0_resp_ready", new Logic()),
    iccsr_s0_resp_exception(this, "iccsr_s0_resp_exception", new Logic()),
    _CsrBridge4_(this),
    w_flush_pipeline(this, "w_flush_pipeline", new Logic()),
    w_mem_resp_error(this, "w_mem_resp_error", new Logic()),
    w_writeback_ready(this, "w_writeback_ready", new Logic()),
    w_reg_wena(this, "w_reg_wena", new Logic()),
    wb_reg_waddr(this, "wb_reg_waddr", new Logic("6")),
    wb_reg_wdata(this, "wb_reg_wdata", new Logic("RISCV_ARCH")),
    wb_reg_wtag(this, "wb_reg_wtag", new Logic("CFG_REG_TAG_WIDTH")),
    w_reg_inorder(this, "w_reg_inorder", new Logic()),
    w_reg_ignored(this, "w_reg_ignored", new Logic()),
    // process
    comb(this)
{
    ModuleObject *p;

    // Create and connet Sub-modules:
    p = static_cast<ModuleObject *>(SCV_get_module("RegIntBank"));
    if (p) {
        iregs0 = p->createInstance(this, "iregs0");
        iregs0->connect_io("i_clk", &i_clk);
        iregs0->connect_io("i_nrst", &i_nrst);
        iregs0->connect_io("i_radr1", &w.e.radr1);
        iregs0->connect_io("o_rdata1", &ireg.rdata1);
        iregs0->connect_io("o_rtag1", &ireg.rtag1);
        iregs0->connect_io("i_radr2", &w.e.radr2);
        iregs0->connect_io("o_rdata2", &ireg.rdata2);
        iregs0->connect_io("o_rtag2", &ireg.rtag2);
        iregs0->connect_io("i_waddr", &wb_reg_waddr);
        iregs0->connect_io("i_wena", &w_reg_wena);
        iregs0->connect_io("i_wtag", &wb_reg_wtag);
        iregs0->connect_io("i_wdata", &wb_reg_wdata);
        iregs0->connect_io("i_inorder", &w_reg_inorder);
        iregs0->connect_io("o_ignored", &w_reg_ignored);
        iregs0->connect_io("i_dport_addr", &dbg.ireg_addr);
        iregs0->connect_io("i_dport_ena", &dbg.ireg_ena);
        iregs0->connect_io("i_dport_write", &dbg.ireg_write);
        iregs0->connect_io("i_dport_wdata", &dbg.ireg_wdata);
        iregs0->connect_io("o_dport_rdata", &ireg.dport_rdata);
        iregs0->connect_io("o_ra", &ireg.ra);
        iregs0->connect_io("o_sp", &ireg.sp);
    } else {
        SHOW_ERROR("%s", "RegIntBank not found");
        iregs0 = 0;
    }

    p = static_cast<ModuleObject *>(SCV_get_module("ic_csr_m2_s1"));
    if (p) {
        iccsr0 = p->createInstance(this, "iccsr0");
        iccsr0->connect_io("i_clk", &i_clk);
        iccsr0->connect_io("i_nrst", &i_nrst);
        iccsr0->connect_io("i_m0_req_valid", &w.e.csr_req_valid);
        iccsr0->connect_io("o_m0_req_ready", &iccsr_m0_req_ready);
        iccsr0->connect_io("i_m0_req_type", &w.e.csr_req_type);
        iccsr0->connect_io("i_m0_req_addr", &w.e.csr_req_addr);
        iccsr0->connect_io("i_m0_req_data", &w.e.csr_req_data);
        iccsr0->connect_io("o_m0_resp_valid", &iccsr_m0_resp_valid);
        iccsr0->connect_io("i_m0_resp_ready", &w.e.csr_resp_ready);
        iccsr0->connect_io("o_m0_resp_data", &iccsr_m0_resp_data);
        iccsr0->connect_io("o_m0_resp_exception", &iccsr_m0_resp_exception);
        iccsr0->connect_io("i_m1_req_valid", &dbg.csr_req_valid);
        iccsr0->connect_io("o_m1_req_ready", &iccsr_m1_req_ready);
        iccsr0->connect_io("i_m1_req_type", &dbg.csr_req_type);
        iccsr0->connect_io("i_m1_req_addr", &dbg.csr_req_addr);
        iccsr0->connect_io("i_m1_req_data", &dbg.csr_req_data);
        iccsr0->connect_io("o_m1_resp_valid", &iccsr_m1_resp_valid);
        iccsr0->connect_io("i_m1_resp_ready", &dbg.csr_resp_ready);
        iccsr0->connect_io("o_m1_resp_data", &iccsr_m1_resp_data);
        iccsr0->connect_io("o_m1_resp_exception", &iccsr_m1_resp_exception);
        iccsr0->connect_io("o_s0_req_valid", &iccsr_s0_req_valid);
        iccsr0->connect_io("i_s0_req_ready", &csr.req_ready);
        iccsr0->connect_io("o_s0_req_type", &iccsr_s0_req_type);
        iccsr0->connect_io("o_s0_req_addr", &iccsr_s0_req_addr);
        iccsr0->connect_io("o_s0_req_data", &iccsr_s0_req_data);
        iccsr0->connect_io("i_s0_resp_valid", &csr.resp_valid);
        iccsr0->connect_io("o_s0_resp_ready", &iccsr_s0_resp_ready);
        iccsr0->connect_io("i_s0_resp_data", &csr.resp_data);
        iccsr0->connect_io("i_s0_resp_exception", &csr.resp_exception);
    } else {
        SHOW_ERROR("%s", "ic_csr_m2_s1 not found");
        iccsr0 = 0;
    }
}

void Processor::proc_comb() {
    SETVAL(w_mem_resp_error, OR4(i_resp_data_load_fault, i_resp_data_store_fault, i_resp_data_er_mpu_store, i_resp_data_er_mpu_load));
    SETVAL(w_writeback_ready, INV(w.e.reg_wena));

    IF (NZ(w.e.reg_wena));
        SETVAL(w_reg_wena, w.e.reg_wena);
        SETVAL(wb_reg_waddr, w.e.reg_waddr);
        SETVAL(wb_reg_wdata, w.e.reg_wdata);
        SETVAL(wb_reg_wtag, w.e.reg_wtag);
        SETZERO(w_reg_inorder, "Executor can overwrite memory loading before it was loaded");
    ELSE();
        SETVAL(w_reg_wena, w.w.wena);
        SETVAL(wb_reg_waddr, w.w.waddr);
        SETVAL(wb_reg_wdata, w.w.wdata);
        SETVAL(wb_reg_wtag, w.w.wtag);
        SETONE(w_reg_inorder, "Cannot write loaded from memory value if it was overwritten");
    ENDIF();

    SETVAL(w_flush_pipeline, OR2(w.e.flushi, csr.flushi_ena));
    SETVAL(o_flush_valid, w_flush_pipeline);
    IF (NZ(w.e.flushi));
        TEXT("fencei or ebreak instructions");
        SETVAL(o_flush_address, w.e.flushi_addr);
    ELSE();
        TEXT("request through debug interface to clear cache");
        SETVAL(o_flush_address, csr.flushi_addr);
    ENDIF();

    SETALLONE(o_data_flush_address);
    SETVAL(o_data_flush_valid, w.m.flushd);
    SETVAL(o_req_ctrl_valid, w.f.imem_req_valid);
    SETVAL(o_req_ctrl_addr, w.f.imem_req_addr);
    SETVAL(o_halted, w.e.halted);
}


proc::proc(GenObject *parent) :
    FileObject(parent, "proc"),
    proc_(this)
{
}
