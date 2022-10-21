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

Processor::Processor(GenObject *parent, const char *name) :
    ModuleObject(parent, "Processor", name),
    hartid(this, "hartid", "0"),
    fpu_ena(this, "fpu_ena", "true"),
    tracer_ena(this, "tracer_ena", "true"),
    trace_file(this, "trace_file", "trace_river_sysc"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_mtimer(this, "i_mtimer", "64", "Read-only shadow value of memory-mapped mtimer register (see CLINT)."),
    _ControlPath0_(this, "Control path:"),
    i_req_ctrl_ready(this, "i_req_ctrl_ready", "1", "ICache is ready to accept request"),
    o_req_ctrl_valid(this, "o_req_ctrl_valid", "1", "Request to ICache is valid"),
    o_req_ctrl_addr(this, "o_req_ctrl_addr", "CFG_CPU_ADDR_BITS", "Requesting address to ICache"),
    i_resp_ctrl_valid(this, "i_resp_ctrl_valid", "1", "ICache response is valid"),
    i_resp_ctrl_addr(this, "i_resp_ctrl_addr", "CFG_CPU_ADDR_BITS", "Response address must be equal to the latest request address"),
    i_resp_ctrl_data(this, "i_resp_ctrl_data", "64", "Read value"),
    i_resp_ctrl_load_fault(this, "i_resp_ctrl_load_fault"),
    o_resp_ctrl_ready(this, "o_resp_ctrl_ready", "1", "Core is ready to accept response from ICache"),
    _DataPath0_(this, "Data path:"),
    i_req_data_ready(this, "i_req_data_ready", "1", "DCache is ready to accept request"),
    o_req_data_valid(this, "o_req_data_valid", "1", "Request to DCache is valid"),
    o_req_data_type(this, "o_req_data_type", "MemopType_Total", "Read/Write transaction plus additional flags"),
    o_req_data_addr(this, "o_req_data_addr", "CFG_CPU_ADDR_BITS", "Requesting address to DCache"),
    o_req_data_wdata(this, "o_req_data_wdata", "64", "Writing value"),
    o_req_data_wstrb(this, "o_req_data_wstrb", "8", "8-bytes aligned strobs"),
    o_req_data_size(this, "o_req_data_size", "2", "memory operation 1,2,4 or 8 bytes"),
    i_resp_data_valid(this, "i_resp_data_valid", "1", "DCache response is valid"),
    i_resp_data_addr(this, "i_resp_data_addr", "CFG_CPU_ADDR_BITS", "DCache response address must be equal to the latest request address"),
    i_resp_data_data(this, "i_resp_data_data", "64", "Read value"),
    i_resp_data_fault_addr(this, "i_resp_data_fault_addr", "CFG_CPU_ADDR_BITS", "write-error address (B-channel)"),
    i_resp_data_load_fault(this, "i_resp_data_load_fault", "1", "Bus response with SLVERR or DECERR on read"),
    i_resp_data_store_fault(this, "i_resp_data_store_fault", "1", "Bus response with SLVERR or DECERR on write"),
    o_resp_data_ready(this, "o_resp_data_ready", "1", "Core is ready to accept response from DCache"),
    _Interrupts0_(this, "Interrupt line from external interrupts controller (PLIC):"),
    i_irq_pending(this, "i_irq_pending", "IRQ_TOTAL", "Per Hart pending interrupts pins"),
    _PmpInterface0_(this, "PMP interface"),
    o_pmp_ena(this, "o_pmp_ena", "1", "PMP is active in S or U modes or if L/MPRV bit is set in M-mode"),
    o_pmp_we(this, "o_pmp_we", "1", "write enable into PMP"),
    o_pmp_region(this, "o_pmp_region", "CFG_PMP_TBL_WIDTH", "selected PMP region"),
    o_pmp_start_addr(this, "o_pmp_start_addr", "CFG_CPU_ADDR_BITS", "PMP region start address"),
    o_pmp_end_addr(this, "o_pmp_end_addr", "CFG_CPU_ADDR_BITS", "PMP region end address (inclusive)"),
    o_pmp_flags(this, "o_pmp_flags", "CFG_PMP_FL_TOTAL", "{ena, lock, r, w, x}"),
    _Debug0(this, "Debug interface:"),
    i_haltreq(this, "i_haltreq", "1", "DMI: halt request from debug unit"),
    i_resumereq(this, "i_resumereq", "1", "DMI: resume request from debug unit"),
    i_dport_req_valid(this, "i_dport_req_valid", "1", "Debug access from DSU is valid"),
    i_dport_type(this, "i_dport_type", "DPortReq_Total", "Debug access type"),
    i_dport_addr(this, "i_dport_addr", "CFG_CPU_ADDR_BITS", "dport address"),
    i_dport_wdata(this, "i_dport_wdata", "RISCV_ARCH", "Write value"),
    i_dport_size(this, "i_dport_size", "3", "reg/mem access size:0=1B;...,4=128B;"),
    o_dport_req_ready(this, "o_dport_req_ready", "1"),
    i_dport_resp_ready(this, "i_dport_resp_ready", "1", "ready to accepd response"),
    o_dport_resp_valid(this, "o_dport_resp_valid", "1", "Response is valid"),
    o_dport_resp_error(this, "o_dport_resp_error", "1", "Something wrong during command execution"),
    o_dport_rdata(this, "o_dport_rdata", "RISCV_ARCH", "Response value"),
    i_progbuf(this, "i_progbuf", "MUL(32,CFG_PROGBUF_REG_TOTAL)", "progam buffer"),
    o_halted(this, "o_halted", "1", "CPU halted via debug interface"),
    _CacheDbg0_(this, "Cache debug signals:"),
    o_flushi_valid(this, "o_flushi_valid", "1", "Remove address from ICache is valid"),
    o_flushi_addr(this, "o_flushi_addr", "CFG_CPU_ADDR_BITS", "Address of instruction to remove from ICache"),
    o_flushd_valid(this, "o_flushd_valid", "1", "Remove address from D$ is valid"),
    o_flushd_addr(this, "o_flushd_addr", "CFG_CPU_ADDR_BITS", "Address of instruction to remove from D$"),
    i_flushd_end(this, "i_flushd_end", "1"),
    // struct declration
    FetchTypeDef_(this),
    MmuTypeDef_(this),
    InstructionDecodeTypeDef_(this),
    ExecuteTypeDef_(this),
    MemoryTypeDef_(this),
    WriteBackTypeDef_(this),
    IntRegsTypeDef_(this),
    CsrTypeDef_(this),
    DebugTypeDef_(this),
    BranchPredictorTypeDef_(this),
    PipelineTypeDef_(this),
    // Internal signals
    w(this, "w", "5-stages CPU pipeline"),
    immu(this, "immu"),
    dmmu(this, "dmmu"),
    ireg(this, "ireg"),
    csr(this, "csr"),
    dbg(this, "dbg"),
    bp(this, "bp"),
    _CsrBridge0_(this),
    _CsrBridge1_(this, "csr bridge to executor unit"),
    iccsr_m0_req_ready(this, "iccsr_m0_req_ready", "1"),
    iccsr_m0_resp_valid(this, "iccsr_m0_resp_valid", "1"),
    iccsr_m0_resp_data(this, "iccsr_m0_resp_data", "RISCV_ARCH"),
    iccsr_m0_resp_exception(this, "iccsr_m0_resp_exception", "1"),
    _CsrBridge2_(this, "csr bridge to debug unit"),
    iccsr_m1_req_ready(this, "iccsr_m1_req_ready", "1"),
    iccsr_m1_resp_valid(this, "iccsr_m1_resp_valid", "1"),
    iccsr_m1_resp_data(this, "iccsr_m1_resp_data", "RISCV_ARCH"),
    iccsr_m1_resp_exception(this, "iccsr_m1_resp_exception", "1"),
    _CsrBridge3_(this, "csr bridge to CSR module"),
    iccsr_s0_req_valid(this, "iccsr_s0_req_valid", "1"),
    iccsr_s0_req_type(this, "iccsr_s0_req_type", "CsrReq_TotalBits"),
    iccsr_s0_req_addr(this, "iccsr_s0_req_addr", "12"),
    iccsr_s0_req_data(this, "iccsr_s0_req_data", "RISCV_ARCH"),
    iccsr_s0_resp_ready(this, "iccsr_s0_resp_ready", "1"),
    iccsr_s0_resp_exception(this, "iccsr_s0_resp_exception", "1"),
    _CsrBridge4_(this),
    w_mem_resp_error(this, "w_mem_resp_error", "1"),
    w_writeback_ready(this, "w_writeback_ready", "1"),
    w_reg_wena(this, "w_reg_wena", "1"),
    wb_reg_waddr(this, "wb_reg_waddr", "6"),
    wb_reg_wdata(this, "wb_reg_wdata", "RISCV_ARCH"),
    wb_reg_wtag(this, "wb_reg_wtag", "CFG_REG_TAG_WIDTH"),
    w_reg_inorder(this, "w_reg_inorder", "1"),
    w_reg_ignored(this, "w_reg_ignored", "1"),
    w_f_flush_ready(this, "w_f_flush_ready", "1"),
    unused_immu_mem_req_type(this, "unused_immu_mem_req_type", "MemopType_Total"),
    unused_immu_mem_req_wdata(this, "unused_immu_mem_req_wdata", "64"),
    unused_immu_mem_req_wstrb(this, "unused_immu_mem_req_wstrb", "8"),
    unused_immu_mem_req_size(this, "unused_immu_mem_req_size", "2"),
    w_immu_core_req_fetch(this, "w_immu_core_req_fetch", "1", "0", "assign to 1: fetch instruction"),
    w_dmmu_core_req_fetch(this, "w_dmmu_core_req_fetch", "1", "0", "assign to 0: data"),
    unused_immu_core_req_type(this, "unused_immu_core_req_type", "MemopType_Total"),
    unused_immu_core_req_wdata(this, "unused_immu_core_req_wdata", "64"),
    unused_immu_core_req_wstrb(this, "unused_immu_core_req_wstrb", "8"),
    unused_immu_core_req_size(this, "unused_immu_core_req_size", "2"),
    unused_immu_mem_resp_store_fault(this, "unused_immu_mem_resp_store_fault", "1"),
    // process
    comb(this),
    // sub-modules
    fetch0(this, "fetch0"),
    dec0(this, "dec0"),
    exec0(this, "exec0"),
    mem0(this, "mem0"),
    immu0(this, "immu0"),
    dmmu0(this, "dmmu0"),
    predic0(this, "predic0"),
    iregs0(this, "iregs0"),
    iccsr0(this, "iccsr0"),
    csr0(this, "csr0"),
    trace0(this, "trace0"),
    dbg0(this, "dbg0")
{
    Operation::start(this);

    // Create and connet Sub-modules:
    NEW(immu0, immu0.getName().c_str());
        CONNECT(immu0, 0, immu0.i_clk, i_clk);
        CONNECT(immu0, 0, immu0.i_nrst, i_nrst);
        CONNECT(immu0, 0, immu0.o_core_req_ready, immu.req_ready);
        CONNECT(immu0, 0, immu0.i_core_req_valid, w.f.imem_req_valid);
        CONNECT(immu0, 0, immu0.i_core_req_addr, w.f.imem_req_addr);
        CONNECT(immu0, 0, immu0.i_core_req_fetch, w_immu_core_req_fetch);
        CONNECT(immu0, 0, immu0.i_core_req_type, unused_immu_core_req_type);
        CONNECT(immu0, 0, immu0.i_core_req_wdata, unused_immu_core_req_wdata);
        CONNECT(immu0, 0, immu0.i_core_req_wstrb, unused_immu_core_req_wstrb);
        CONNECT(immu0, 0, immu0.i_core_req_size, unused_immu_core_req_size);
        CONNECT(immu0, 0, immu0.o_core_resp_valid, immu.valid);
        CONNECT(immu0, 0, immu0.o_core_resp_addr, immu.addr);
        CONNECT(immu0, 0, immu0.o_core_resp_data, immu.data);
        CONNECT(immu0, 0, immu0.o_core_resp_load_fault, immu.load_fault);
        CONNECT(immu0, 0, immu0.o_core_resp_store_fault, immu.store_fault);
        CONNECT(immu0, 0, immu0.o_core_resp_page_x_fault, immu.page_fault_x);
        CONNECT(immu0, 0, immu0.o_core_resp_page_r_fault, immu.page_fault_r);
        CONNECT(immu0, 0, immu0.o_core_resp_page_w_fault, immu.page_fault_w);
        CONNECT(immu0, 0, immu0.i_core_resp_ready, w.f.imem_resp_ready);
        CONNECT(immu0, 0, immu0.i_mem_req_ready, i_req_ctrl_ready);
        CONNECT(immu0, 0, immu0.o_mem_req_valid, o_req_ctrl_valid);
        CONNECT(immu0, 0, immu0.o_mem_req_addr, o_req_ctrl_addr);
        CONNECT(immu0, 0, immu0.o_mem_req_type, unused_immu_mem_req_type);
        CONNECT(immu0, 0, immu0.o_mem_req_wdata, unused_immu_mem_req_wdata);
        CONNECT(immu0, 0, immu0.o_mem_req_wstrb, unused_immu_mem_req_wstrb);
        CONNECT(immu0, 0, immu0.o_mem_req_size, unused_immu_mem_req_size);
        CONNECT(immu0, 0, immu0.i_mem_resp_valid, i_resp_ctrl_valid);
        CONNECT(immu0, 0, immu0.i_mem_resp_addr, i_resp_ctrl_addr);
        CONNECT(immu0, 0, immu0.i_mem_resp_data, i_resp_ctrl_data);
        CONNECT(immu0, 0, immu0.i_mem_resp_load_fault, i_resp_ctrl_load_fault);
        CONNECT(immu0, 0, immu0.i_mem_resp_store_fault, unused_immu_mem_resp_store_fault);
        CONNECT(immu0, 0, immu0.o_mem_resp_ready, o_resp_ctrl_ready);
        CONNECT(immu0, 0, immu0.i_mmu_ena, csr.immu_ena);
        CONNECT(immu0, 0, immu0.i_mmu_sv39, csr.mmu_sv39);
        CONNECT(immu0, 0, immu0.i_mmu_sv48, csr.mmu_sv48);
        CONNECT(immu0, 0, immu0.i_mmu_ppn, csr.mmu_ppn);
        CONNECT(immu0, 0, immu0.i_fence, csr.flushmmu_valid);
        CONNECT(immu0, 0, immu0.i_fence_addr, csr.flush_addr);
    ENDNEW();

    NEW(fetch0, fetch0.getName().c_str());
        CONNECT(fetch0, 0, fetch0.i_clk, i_clk);
        CONNECT(fetch0, 0, fetch0.i_nrst, i_nrst);
        CONNECT(fetch0, 0, fetch0.i_bp_valid, bp.f_valid);
        CONNECT(fetch0, 0, fetch0.i_bp_pc, bp.f_pc);
        CONNECT(fetch0, 0, fetch0.i_mem_req_ready, immu.req_ready);
        CONNECT(fetch0, 0, fetch0.o_mem_addr_valid, w.f.imem_req_valid);
        CONNECT(fetch0, 0, fetch0.o_mem_addr, w.f.imem_req_addr);
        CONNECT(fetch0, 0, fetch0.i_mem_data_valid, immu.valid);
        CONNECT(fetch0, 0, fetch0.i_mem_data_addr, immu.addr);
        CONNECT(fetch0, 0, fetch0.i_mem_data, immu.data);
        CONNECT(fetch0, 0, fetch0.i_mem_load_fault, immu.load_fault);
        CONNECT(fetch0, 0, fetch0.i_mem_page_fault_x, immu.page_fault_x);
        CONNECT(fetch0, 0, fetch0.o_mem_resp_ready, w.f.imem_resp_ready);
        CONNECT(fetch0, 0, fetch0.i_flush_pipeline, csr.flushi_valid);
        CONNECT(fetch0, 0, fetch0.i_progbuf_ena, dbg.progbuf_ena);
        CONNECT(fetch0, 0, fetch0.i_progbuf_pc, dbg.progbuf_pc);
        CONNECT(fetch0, 0, fetch0.i_progbuf_instr, dbg.progbuf_instr);
        CONNECT(fetch0, 0, fetch0.o_instr_load_fault, w.f.instr_load_fault);
        CONNECT(fetch0, 0, fetch0.o_instr_page_fault_x, w.f.instr_page_fault_x);
        CONNECT(fetch0, 0, fetch0.o_requested_pc, w.f.requested_pc);
        CONNECT(fetch0, 0, fetch0.o_fetching_pc, w.f.fetching_pc);
        CONNECT(fetch0, 0, fetch0.o_pc, w.f.pc);
        CONNECT(fetch0, 0, fetch0.o_instr, w.f.instr);
    ENDNEW();

    NEW(dec0, dec0.getName().c_str());
        CONNECT(dec0, 0, dec0.i_clk, i_clk);
        CONNECT(dec0, 0, dec0.i_nrst, i_nrst);
        CONNECT(dec0, 0, dec0.i_f_pc, w.f.pc);
        CONNECT(dec0, 0, dec0.i_f_instr, w.f.instr);
        CONNECT(dec0, 0, dec0.i_instr_load_fault, w.f.instr_load_fault);
        CONNECT(dec0, 0, dec0.i_instr_page_fault_x, w.f.instr_page_fault_x);
        CONNECT(dec0, 0, dec0.i_e_npc, w.e.npc);
        CONNECT(dec0, 0, dec0.o_radr1, w.d.radr1);
        CONNECT(dec0, 0, dec0.o_radr2, w.d.radr2);
        CONNECT(dec0, 0, dec0.o_waddr, w.d.waddr);
        CONNECT(dec0, 0, dec0.o_csr_addr, w.d.csr_addr),
        CONNECT(dec0, 0, dec0.o_imm, w.d.imm);
        CONNECT(dec0, 0, dec0.i_flush_pipeline, csr.flushi_valid);
        CONNECT(dec0, 0, dec0.i_progbuf_ena, dbg.progbuf_ena);
        CONNECT(dec0, 0, dec0.o_pc, w.d.pc);
        CONNECT(dec0, 0, dec0.o_instr, w.d.instr);
        CONNECT(dec0, 0, dec0.o_memop_store, w.d.memop_store);
        CONNECT(dec0, 0, dec0.o_memop_load, w.d.memop_load);
        CONNECT(dec0, 0, dec0.o_memop_sign_ext, w.d.memop_sign_ext);
        CONNECT(dec0, 0, dec0.o_memop_size, w.d.memop_size);
        CONNECT(dec0, 0, dec0.o_unsigned_op, w.d.unsigned_op);
        CONNECT(dec0, 0, dec0.o_rv32, w.d.rv32);
        CONNECT(dec0, 0, dec0.o_compressed, w.d.compressed);
        CONNECT(dec0, 0, dec0.o_amo, w.d.amo);
        CONNECT(dec0, 0, dec0.o_f64, w.d.f64);
        CONNECT(dec0, 0, dec0.o_isa_type, w.d.isa_type);
        CONNECT(dec0, 0, dec0.o_instr_vec, w.d.instr_vec);
        CONNECT(dec0, 0, dec0.o_exception, w.d.exception);
        CONNECT(dec0, 0, dec0.o_instr_load_fault, w.d.instr_load_fault);
        CONNECT(dec0, 0, dec0.o_instr_page_fault_x, w.d.page_fault_x);
        CONNECT(dec0, 0, dec0.o_progbuf_ena, w.d.progbuf_ena);
    ENDNEW();

    NEW(exec0, exec0.getName().c_str());
        CONNECT(exec0, 0, exec0.i_clk, i_clk);
        CONNECT(exec0, 0, exec0.i_nrst, i_nrst);
        CONNECT(exec0, 0, exec0.i_d_pc, w.d.pc);
        CONNECT(exec0, 0, exec0.i_d_instr, w.d.instr);
        CONNECT(exec0, 0, exec0.i_d_progbuf_ena, w.d.progbuf_ena);
        CONNECT(exec0, 0, exec0.i_d_radr1, w.d.radr1);
        CONNECT(exec0, 0, exec0.i_d_radr2, w.d.radr2);
        CONNECT(exec0, 0, exec0.i_d_waddr, w.d.waddr);
        CONNECT(exec0, 0, exec0.i_d_csr_addr, w.d.csr_addr);
        CONNECT(exec0, 0, exec0.i_d_imm, w.d.imm);
        CONNECT(exec0, 0, exec0.i_wb_waddr, w.w.waddr);
        CONNECT(exec0, 0, exec0.i_memop_store, w.d.memop_store);
        CONNECT(exec0, 0, exec0.i_memop_load, w.d.memop_load);
        CONNECT(exec0, 0, exec0.i_memop_sign_ext, w.d.memop_sign_ext);
        CONNECT(exec0, 0, exec0.i_memop_size, w.d.memop_size);
        CONNECT(exec0, 0, exec0.i_unsigned_op, w.d.unsigned_op);
        CONNECT(exec0, 0, exec0.i_rv32, w.d.rv32);
        CONNECT(exec0, 0, exec0.i_compressed, w.d.compressed);
        CONNECT(exec0, 0, exec0.i_amo, w.d.amo);
        CONNECT(exec0, 0, exec0.i_f64, w.d.f64);
        CONNECT(exec0, 0, exec0.i_isa_type, w.d.isa_type);
        CONNECT(exec0, 0, exec0.i_ivec, w.d.instr_vec);
        CONNECT(exec0, 0, exec0.i_stack_overflow, csr.stack_overflow);
        CONNECT(exec0, 0, exec0.i_stack_underflow, csr.stack_underflow);
        CONNECT(exec0, 0, exec0.i_unsup_exception, w.d.exception);
        CONNECT(exec0, 0, exec0.i_instr_load_fault, w.d.instr_load_fault);
        CONNECT(exec0, 0, exec0.i_mem_valid, dmmu.valid);
        CONNECT(exec0, 0, exec0.i_mem_rdata, dmmu.data);
        CONNECT(exec0, 0, exec0.i_mem_ex_debug, w.m.debug_valid);
        CONNECT(exec0, 0, exec0.i_mem_ex_load_fault, dmmu.load_fault);
        CONNECT(exec0, 0, exec0.i_mem_ex_store_fault, dmmu.store_fault);
        CONNECT(exec0, 0, exec0.i_page_fault_x, w.d.page_fault_x);
        CONNECT(exec0, 0, exec0.i_page_fault_r, dmmu.page_fault_r);
        CONNECT(exec0, 0, exec0.i_page_fault_w, dmmu.page_fault_w);
        CONNECT(exec0, 0, exec0.i_mem_ex_addr, dmmu.addr);
        CONNECT(exec0, 0, exec0.i_irq_pending, csr.irq_pending);
        CONNECT(exec0, 0, exec0.i_wakeup, csr.wakeup);
        CONNECT(exec0, 0, exec0.i_haltreq, i_haltreq);
        CONNECT(exec0, 0, exec0.i_resumereq, i_resumereq);
        CONNECT(exec0, 0, exec0.i_step, csr.step);
        CONNECT(exec0, 0, exec0.i_dbg_progbuf_ena, dbg.progbuf_ena);
        CONNECT(exec0, 0, exec0.i_rdata1, ireg.rdata1);
        CONNECT(exec0, 0, exec0.i_rtag1, ireg.rtag1);
        CONNECT(exec0, 0, exec0.i_rdata2, ireg.rdata2);
        CONNECT(exec0, 0, exec0.i_rtag2, ireg.rtag2);
        CONNECT(exec0, 0, exec0.o_radr1, w.e.radr1);
        CONNECT(exec0, 0, exec0.o_radr2, w.e.radr2);
        CONNECT(exec0, 0, exec0.o_reg_wena, w.e.reg_wena);
        CONNECT(exec0, 0, exec0.o_reg_waddr, w.e.reg_waddr);
        CONNECT(exec0, 0, exec0.o_reg_wtag, w.e.reg_wtag);
        CONNECT(exec0, 0, exec0.o_reg_wdata, w.e.reg_wdata);
        CONNECT(exec0, 0, exec0.o_csr_req_valid, w.e.csr_req_valid);
        CONNECT(exec0, 0, exec0.i_csr_req_ready, iccsr_m0_req_ready);
        CONNECT(exec0, 0, exec0.o_csr_req_type, w.e.csr_req_type);
        CONNECT(exec0, 0, exec0.o_csr_req_addr, w.e.csr_req_addr);
        CONNECT(exec0, 0, exec0.o_csr_req_data, w.e.csr_req_data);
        CONNECT(exec0, 0, exec0.i_csr_resp_valid, iccsr_m0_resp_valid);
        CONNECT(exec0, 0, exec0.o_csr_resp_ready, w.e.csr_resp_ready);
        CONNECT(exec0, 0, exec0.i_csr_resp_data, iccsr_m0_resp_data);
        CONNECT(exec0, 0, exec0.i_csr_resp_exception, iccsr_m0_resp_exception);
        CONNECT(exec0, 0, exec0.o_memop_valid, w.e.memop_valid);
        CONNECT(exec0, 0, exec0.o_memop_debug, w.e.memop_debug);
        CONNECT(exec0, 0, exec0.o_memop_sign_ext, w.e.memop_sign_ext);
        CONNECT(exec0, 0, exec0.o_memop_type, w.e.memop_type);
        CONNECT(exec0, 0, exec0.o_memop_size, w.e.memop_size);
        CONNECT(exec0, 0, exec0.o_memop_memaddr, w.e.memop_addr);
        CONNECT(exec0, 0, exec0.o_memop_wdata, w.e.memop_wdata);
        CONNECT(exec0, 0, exec0.i_memop_ready, w.m.memop_ready);
        CONNECT(exec0, 0, exec0.i_memop_idle, w.m.idle);
        CONNECT(exec0, 0, exec0.i_dbg_mem_req_valid, dbg.mem_req_valid);
        CONNECT(exec0, 0, exec0.i_dbg_mem_req_write, dbg.mem_req_write);
        CONNECT(exec0, 0, exec0.i_dbg_mem_req_size, dbg.mem_req_size);
        CONNECT(exec0, 0, exec0.i_dbg_mem_req_addr, dbg.mem_req_addr);
        CONNECT(exec0, 0, exec0.i_dbg_mem_req_wdata, dbg.mem_req_wdata);
        CONNECT(exec0, 0, exec0.o_dbg_mem_req_ready, w.e.dbg_mem_req_ready);
        CONNECT(exec0, 0, exec0.o_dbg_mem_req_error, w.e.dbg_mem_req_error);
        CONNECT(exec0, 0, exec0.o_valid, w.e.valid);
        CONNECT(exec0, 0, exec0.o_pc, w.e.pc);
        CONNECT(exec0, 0, exec0.o_npc, w.e.npc);
        CONNECT(exec0, 0, exec0.o_instr, w.e.instr);
        CONNECT(exec0, 0, exec0.o_call, w.e.call);
        CONNECT(exec0, 0, exec0.o_ret, w.e.ret);
        CONNECT(exec0, 0, exec0.o_jmp, w.e.jmp);
        CONNECT(exec0, 0, exec0.o_halted, w.e.halted);
    ENDNEW();

    NEW(mem0, mem0.getName().c_str());
        CONNECT(mem0, 0, mem0.i_clk, i_clk);
        CONNECT(mem0, 0, mem0.i_nrst, i_nrst);
        CONNECT(mem0, 0, mem0.i_e_pc, w.e.pc);
        CONNECT(mem0, 0, mem0.i_e_instr, w.e.instr);
        CONNECT(mem0, 0, mem0.i_flushd_valid, csr.flushd_valid);
        CONNECT(mem0, 0, mem0.i_flushd_addr, csr.flush_addr);
        CONNECT(mem0, 0, mem0.o_flushd, w.m.flushd);
        CONNECT(mem0, 0, mem0.i_mmu_ena, csr.dmmu_ena);
        CONNECT(mem0, 0, mem0.i_mmu_sv39, csr.mmu_sv39);
        CONNECT(mem0, 0, mem0.i_mmu_sv48, csr.mmu_sv48);
        CONNECT(mem0, 0, mem0.o_mmu_ena, w.m.dmmu_ena);
        CONNECT(mem0, 0, mem0.o_mmu_sv39, w.m.dmmu_sv39);
        CONNECT(mem0, 0, mem0.o_mmu_sv48, w.m.dmmu_sv48);
        CONNECT(mem0, 0, mem0.i_reg_waddr, w.e.reg_waddr);
        CONNECT(mem0, 0, mem0.i_reg_wtag, w.e.reg_wtag);
        CONNECT(mem0, 0, mem0.i_memop_valid, w.e.memop_valid);
        CONNECT(mem0, 0, mem0.i_memop_debug, w.e.memop_debug);
        CONNECT(mem0, 0, mem0.i_memop_wdata, w.e.memop_wdata);
        CONNECT(mem0, 0, mem0.i_memop_sign_ext, w.e.memop_sign_ext);
        CONNECT(mem0, 0, mem0.i_memop_type, w.e.memop_type);
        CONNECT(mem0, 0, mem0.i_memop_size, w.e.memop_size);
        CONNECT(mem0, 0, mem0.i_memop_addr, w.e.memop_addr);
        CONNECT(mem0, 0, mem0.o_memop_ready, w.m.memop_ready);
        CONNECT(mem0, 0, mem0.o_wb_wena, w.w.wena);
        CONNECT(mem0, 0, mem0.o_wb_waddr, w.w.waddr);
        CONNECT(mem0, 0, mem0.o_wb_wdata, w.w.wdata);
        CONNECT(mem0, 0, mem0.o_wb_wtag, w.w.wtag);
        CONNECT(mem0, 0, mem0.i_wb_ready, w_writeback_ready);
        CONNECT(mem0, 0, mem0.i_mem_req_ready, dmmu.req_ready);
        CONNECT(mem0, 0, mem0.o_mem_valid, w.m.req_data_valid);
        CONNECT(mem0, 0, mem0.o_mem_type, w.m.req_data_type);
        CONNECT(mem0, 0, mem0.o_mem_addr, w.m.req_data_addr);
        CONNECT(mem0, 0, mem0.o_mem_wdata, w.m.req_data_wdata);
        CONNECT(mem0, 0, mem0.o_mem_wstrb, w.m.req_data_wstrb);
        CONNECT(mem0, 0, mem0.o_mem_size, w.m.req_data_size);
        CONNECT(mem0, 0, mem0.i_mem_data_valid, dmmu.valid);
        CONNECT(mem0, 0, mem0.i_mem_data_addr, dmmu.addr);
        CONNECT(mem0, 0, mem0.i_mem_data, dmmu.data);
        CONNECT(mem0, 0, mem0.o_mem_resp_ready, w.m.resp_data_ready);
        CONNECT(mem0, 0, mem0.o_pc, w.m.pc);
        CONNECT(mem0, 0, mem0.o_valid, w.m.valid);
        CONNECT(mem0, 0, mem0.o_idle, w.m.idle);
        CONNECT(mem0, 0, mem0.o_debug_valid, w.m.debug_valid);
    ENDNEW();


    NEW(dmmu0, dmmu0.getName().c_str());
        CONNECT(dmmu0, 0, dmmu0.i_clk, i_clk);
        CONNECT(dmmu0, 0, dmmu0.i_nrst, i_nrst);
        CONNECT(dmmu0, 0, dmmu0.o_core_req_ready, dmmu.req_ready);
        CONNECT(dmmu0, 0, dmmu0.i_core_req_valid, w.m.req_data_valid);
        CONNECT(dmmu0, 0, dmmu0.i_core_req_addr, w.m.req_data_addr);
        CONNECT(dmmu0, 0, dmmu0.i_core_req_fetch, w_dmmu_core_req_fetch);
        CONNECT(dmmu0, 0, dmmu0.i_core_req_type, w.m.req_data_type);
        CONNECT(dmmu0, 0, dmmu0.i_core_req_wdata, w.m.req_data_wdata);
        CONNECT(dmmu0, 0, dmmu0.i_core_req_wstrb, w.m.req_data_wstrb);
        CONNECT(dmmu0, 0, dmmu0.i_core_req_size, w.m.req_data_size);
        CONNECT(dmmu0, 0, dmmu0.o_core_resp_valid, dmmu.valid);
        CONNECT(dmmu0, 0, dmmu0.o_core_resp_addr, dmmu.addr);
        CONNECT(dmmu0, 0, dmmu0.o_core_resp_data, dmmu.data);
        CONNECT(dmmu0, 0, dmmu0.o_core_resp_load_fault, dmmu.load_fault);
        CONNECT(dmmu0, 0, dmmu0.o_core_resp_store_fault, dmmu.store_fault);
        CONNECT(dmmu0, 0, dmmu0.o_core_resp_page_x_fault, dmmu.page_fault_x);
        CONNECT(dmmu0, 0, dmmu0.o_core_resp_page_r_fault, dmmu.page_fault_r);
        CONNECT(dmmu0, 0, dmmu0.o_core_resp_page_w_fault, dmmu.page_fault_w);
        CONNECT(dmmu0, 0, dmmu0.i_core_resp_ready, w.m.resp_data_ready);
        CONNECT(dmmu0, 0, dmmu0.i_mem_req_ready, i_req_data_ready);
        CONNECT(dmmu0, 0, dmmu0.o_mem_req_valid, o_req_data_valid);
        CONNECT(dmmu0, 0, dmmu0.o_mem_req_addr, o_req_data_addr);
        CONNECT(dmmu0, 0, dmmu0.o_mem_req_type, o_req_data_type);
        CONNECT(dmmu0, 0, dmmu0.o_mem_req_wdata, o_req_data_wdata);
        CONNECT(dmmu0, 0, dmmu0.o_mem_req_wstrb, o_req_data_wstrb);
        CONNECT(dmmu0, 0, dmmu0.o_mem_req_size, o_req_data_size);
        CONNECT(dmmu0, 0, dmmu0.i_mem_resp_valid, i_resp_data_valid);
        CONNECT(dmmu0, 0, dmmu0.i_mem_resp_addr, i_resp_data_addr);
        CONNECT(dmmu0, 0, dmmu0.i_mem_resp_data, i_resp_data_data);
        CONNECT(dmmu0, 0, dmmu0.i_mem_resp_load_fault, i_resp_data_load_fault);
        CONNECT(dmmu0, 0, dmmu0.i_mem_resp_store_fault, i_resp_data_store_fault);
        CONNECT(dmmu0, 0, dmmu0.o_mem_resp_ready, o_resp_data_ready);
        CONNECT(dmmu0, 0, dmmu0.i_mmu_ena, w.m.dmmu_ena);
        CONNECT(dmmu0, 0, dmmu0.i_mmu_sv39, w.m.dmmu_sv39);
        CONNECT(dmmu0, 0, dmmu0.i_mmu_sv48, w.m.dmmu_sv48);
        CONNECT(dmmu0, 0, dmmu0.i_mmu_ppn, csr.mmu_ppn);
        CONNECT(dmmu0, 0, dmmu0.i_fence, csr.flushmmu_valid);
        CONNECT(dmmu0, 0, dmmu0.i_fence_addr, csr.flush_addr);
    ENDNEW();


    NEW(predic0, predic0.getName().c_str());
        CONNECT(predic0, 0, predic0.i_clk, i_clk);
        CONNECT(predic0, 0, predic0.i_nrst, i_nrst);
        CONNECT(predic0, 0, predic0.i_flush_pipeline, csr.flushi_valid);
        CONNECT(predic0, 0, predic0.i_resp_mem_valid, immu.valid);
        CONNECT(predic0, 0, predic0.i_resp_mem_addr, immu.addr);
        CONNECT(predic0, 0, predic0.i_resp_mem_data, immu.data);
        CONNECT(predic0, 0, predic0.i_e_jmp, w.e.jmp);
        CONNECT(predic0, 0, predic0.i_e_pc, w.e.pc);
        CONNECT(predic0, 0, predic0.i_e_npc, w.e.npc);
        CONNECT(predic0, 0, predic0.i_ra, ireg.ra);
        CONNECT(predic0, 0, predic0.o_f_valid, bp.f_valid);
        CONNECT(predic0, 0, predic0.o_f_pc, bp.f_pc);
        CONNECT(predic0, 0, predic0.i_f_requested_pc, w.f.requested_pc);
        CONNECT(predic0, 0, predic0.i_f_fetching_pc, w.f.fetching_pc);
        CONNECT(predic0, 0, predic0.i_f_fetched_pc, w.f.pc);
        CONNECT(predic0, 0, predic0.i_d_pc, w.d.pc);
    ENDNEW();

    NEW(iregs0, iregs0.getName().c_str());
        CONNECT(iregs0, 0, iregs0.i_clk, i_clk);
        CONNECT(iregs0, 0, iregs0.i_nrst, i_nrst);
        CONNECT(iregs0, 0, iregs0.i_radr1, w.e.radr1);
        CONNECT(iregs0, 0, iregs0.o_rdata1, ireg.rdata1);
        CONNECT(iregs0, 0, iregs0.o_rtag1, ireg.rtag1);
        CONNECT(iregs0, 0, iregs0.i_radr2, w.e.radr2);
        CONNECT(iregs0, 0, iregs0.o_rdata2, ireg.rdata2);
        CONNECT(iregs0, 0, iregs0.o_rtag2, ireg.rtag2);
        CONNECT(iregs0, 0, iregs0.i_waddr, wb_reg_waddr);
        CONNECT(iregs0, 0, iregs0.i_wena, w_reg_wena);
        CONNECT(iregs0, 0, iregs0.i_wtag, wb_reg_wtag);
        CONNECT(iregs0, 0, iregs0.i_wdata, wb_reg_wdata);
        CONNECT(iregs0, 0, iregs0.i_inorder, w_reg_inorder);
        CONNECT(iregs0, 0, iregs0.o_ignored, w_reg_ignored);
        CONNECT(iregs0, 0, iregs0.i_dport_addr, dbg.ireg_addr);
        CONNECT(iregs0, 0, iregs0.i_dport_ena, dbg.ireg_ena);
        CONNECT(iregs0, 0, iregs0.i_dport_write, dbg.ireg_write);
        CONNECT(iregs0, 0, iregs0.i_dport_wdata, dbg.ireg_wdata);
        CONNECT(iregs0, 0, iregs0.o_dport_rdata, ireg.dport_rdata);
        CONNECT(iregs0, 0, iregs0.o_ra, ireg.ra);
        CONNECT(iregs0, 0, iregs0.o_sp, ireg.sp);
        CONNECT(iregs0, 0, iregs0.o_gp, ireg.gp);
        CONNECT(iregs0, 0, iregs0.o_tp, ireg.tp);
        CONNECT(iregs0, 0, iregs0.o_t0, ireg.t0);
        CONNECT(iregs0, 0, iregs0.o_t1, ireg.t1);
        CONNECT(iregs0, 0, iregs0.o_t2, ireg.t2);
        CONNECT(iregs0, 0, iregs0.o_fp, ireg.fp);
        CONNECT(iregs0, 0, iregs0.o_s1, ireg.s1);
        CONNECT(iregs0, 0, iregs0.o_a0, ireg.a0);
        CONNECT(iregs0, 0, iregs0.o_a1, ireg.a1);
        CONNECT(iregs0, 0, iregs0.o_a2, ireg.a2);
        CONNECT(iregs0, 0, iregs0.o_a3, ireg.a3);
        CONNECT(iregs0, 0, iregs0.o_a4, ireg.a4);
        CONNECT(iregs0, 0, iregs0.o_a5, ireg.a5);
        CONNECT(iregs0, 0, iregs0.o_a6, ireg.a6);
        CONNECT(iregs0, 0, iregs0.o_a7, ireg.a7);
        CONNECT(iregs0, 0, iregs0.o_s2, ireg.s2);
        CONNECT(iregs0, 0, iregs0.o_s3, ireg.s3);
        CONNECT(iregs0, 0, iregs0.o_s4, ireg.s4);
        CONNECT(iregs0, 0, iregs0.o_s5, ireg.s5);
        CONNECT(iregs0, 0, iregs0.o_s6, ireg.s6);
        CONNECT(iregs0, 0, iregs0.o_s7, ireg.s7);
        CONNECT(iregs0, 0, iregs0.o_s8, ireg.s8);
        CONNECT(iregs0, 0, iregs0.o_s9, ireg.s9);
        CONNECT(iregs0, 0, iregs0.o_s10, ireg.s10);
        CONNECT(iregs0, 0, iregs0.o_s11, ireg.s11);
        CONNECT(iregs0, 0, iregs0.o_t3, ireg.t3);
        CONNECT(iregs0, 0, iregs0.o_t4, ireg.t4);
        CONNECT(iregs0, 0, iregs0.o_t5, ireg.t5);
        CONNECT(iregs0, 0, iregs0.o_t6, ireg.t6);
    ENDNEW();

    NEW(iccsr0, iccsr0.getName().c_str());
        CONNECT(iccsr0, 0, iccsr0.i_clk, i_clk);
        CONNECT(iccsr0, 0, iccsr0.i_nrst, i_nrst);
        CONNECT(iccsr0, 0, iccsr0.i_m0_req_valid, w.e.csr_req_valid);
        CONNECT(iccsr0, 0, iccsr0.o_m0_req_ready, iccsr_m0_req_ready);
        CONNECT(iccsr0, 0, iccsr0.i_m0_req_type, w.e.csr_req_type);
        CONNECT(iccsr0, 0, iccsr0.i_m0_req_addr, w.e.csr_req_addr);
        CONNECT(iccsr0, 0, iccsr0.i_m0_req_data, w.e.csr_req_data);
        CONNECT(iccsr0, 0, iccsr0.o_m0_resp_valid, iccsr_m0_resp_valid);
        CONNECT(iccsr0, 0, iccsr0.i_m0_resp_ready, w.e.csr_resp_ready);
        CONNECT(iccsr0, 0, iccsr0.o_m0_resp_data, iccsr_m0_resp_data);
        CONNECT(iccsr0, 0, iccsr0.o_m0_resp_exception, iccsr_m0_resp_exception);
        CONNECT(iccsr0, 0, iccsr0.i_m1_req_valid, dbg.csr_req_valid);
        CONNECT(iccsr0, 0, iccsr0.o_m1_req_ready, iccsr_m1_req_ready);
        CONNECT(iccsr0, 0, iccsr0.i_m1_req_type, dbg.csr_req_type);
        CONNECT(iccsr0, 0, iccsr0.i_m1_req_addr, dbg.csr_req_addr);
        CONNECT(iccsr0, 0, iccsr0.i_m1_req_data, dbg.csr_req_data);
        CONNECT(iccsr0, 0, iccsr0.o_m1_resp_valid, iccsr_m1_resp_valid);
        CONNECT(iccsr0, 0, iccsr0.i_m1_resp_ready, dbg.csr_resp_ready);
        CONNECT(iccsr0, 0, iccsr0.o_m1_resp_data, iccsr_m1_resp_data);
        CONNECT(iccsr0, 0, iccsr0.o_m1_resp_exception, iccsr_m1_resp_exception);
        CONNECT(iccsr0, 0, iccsr0.o_s0_req_valid, iccsr_s0_req_valid);
        CONNECT(iccsr0, 0, iccsr0.i_s0_req_ready, csr.req_ready);
        CONNECT(iccsr0, 0, iccsr0.o_s0_req_type, iccsr_s0_req_type);
        CONNECT(iccsr0, 0, iccsr0.o_s0_req_addr, iccsr_s0_req_addr);
        CONNECT(iccsr0, 0, iccsr0.o_s0_req_data, iccsr_s0_req_data);
        CONNECT(iccsr0, 0, iccsr0.i_s0_resp_valid, csr.resp_valid);
        CONNECT(iccsr0, 0, iccsr0.o_s0_resp_ready, iccsr_s0_resp_ready);
        CONNECT(iccsr0, 0, iccsr0.i_s0_resp_data, csr.resp_data);
        CONNECT(iccsr0, 0, iccsr0.i_s0_resp_exception, csr.resp_exception);
    ENDNEW();

    NEW(csr0, csr0.getName().c_str());
        CONNECT(csr0, 0, csr0.i_clk, i_clk);
        CONNECT(csr0, 0, csr0.i_nrst, i_nrst);
        CONNECT(csr0, 0, csr0.i_sp, ireg.sp);
        CONNECT(csr0, 0, csr0.i_req_valid, iccsr_s0_req_valid);
        CONNECT(csr0, 0, csr0.o_req_ready, csr.req_ready);
        CONNECT(csr0, 0, csr0.i_req_type, iccsr_s0_req_type);
        CONNECT(csr0, 0, csr0.i_req_addr, iccsr_s0_req_addr);
        CONNECT(csr0, 0, csr0.i_req_data, iccsr_s0_req_data);
        CONNECT(csr0, 0, csr0.o_resp_valid, csr.resp_valid);
        CONNECT(csr0, 0, csr0.i_resp_ready, iccsr_s0_resp_ready);
        CONNECT(csr0, 0, csr0.o_resp_data, csr.resp_data);
        CONNECT(csr0, 0, csr0.o_resp_exception, csr.resp_exception);
        CONNECT(csr0, 0, csr0.i_e_halted, w.e.halted);
        CONNECT(csr0, 0, csr0.i_e_pc, w.e.pc);
        CONNECT(csr0, 0, csr0.i_e_instr, w.e.instr);
        CONNECT(csr0, 0, csr0.i_irq_pending, i_irq_pending);
        CONNECT(csr0, 0, csr0.o_irq_pending, csr.irq_pending);
        CONNECT(csr0, 0, csr0.o_wakeup, csr.wakeup);
        CONNECT(csr0, 0, csr0.o_stack_overflow, csr.stack_overflow);
        CONNECT(csr0, 0, csr0.o_stack_underflow, csr.stack_underflow);
        CONNECT(csr0, 0, csr0.i_f_flush_ready, w_f_flush_ready);
        CONNECT(csr0, 0, csr0.i_e_valid, w.e.valid);
        CONNECT(csr0, 0, csr0.i_m_memop_ready, w.m.memop_ready);
        CONNECT(csr0, 0, csr0.i_m_idle, w.m.idle);
        CONNECT(csr0, 0, csr0.i_flushd_end, i_flushd_end);
        CONNECT(csr0, 0, csr0.i_mtimer, i_mtimer);
        CONNECT(csr0, 0, csr0.o_executed_cnt, csr.executed_cnt);
        CONNECT(csr0, 0, csr0.o_step, csr.step);
        CONNECT(csr0, 0, csr0.i_dbg_progbuf_ena, dbg.progbuf_ena);
        CONNECT(csr0, 0, csr0.o_progbuf_end, csr.progbuf_end);
        CONNECT(csr0, 0, csr0.o_progbuf_error, csr.progbuf_error);
        CONNECT(csr0, 0, csr0.o_flushd_valid, csr.flushd_valid);
        CONNECT(csr0, 0, csr0.o_flushi_valid, csr.flushi_valid);
        CONNECT(csr0, 0, csr0.o_flushmmu_valid, csr.flushmmu_valid);
        CONNECT(csr0, 0, csr0.o_flush_addr, csr.flush_addr);
        CONNECT(csr0, 0, csr0.o_pmp_ena, o_pmp_ena);
        CONNECT(csr0, 0, csr0.o_pmp_we, o_pmp_we);
        CONNECT(csr0, 0, csr0.o_pmp_region, o_pmp_region);
        CONNECT(csr0, 0, csr0.o_pmp_start_addr, o_pmp_start_addr);
        CONNECT(csr0, 0, csr0.o_pmp_end_addr, o_pmp_end_addr);
        CONNECT(csr0, 0, csr0.o_pmp_flags, o_pmp_flags);
        CONNECT(csr0, 0, csr0.o_immu_ena, csr.immu_ena);
        CONNECT(csr0, 0, csr0.o_dmmu_ena, csr.dmmu_ena);
        CONNECT(csr0, 0, csr0.o_mmu_ppn, csr.mmu_ppn);
        CONNECT(csr0, 0, csr0.o_mmu_sv39, csr.mmu_sv39);
        CONNECT(csr0, 0, csr0.o_mmu_sv48, csr.mmu_sv48);
    ENDNEW();

    NEW(dbg0, dbg0.getName().c_str());
        CONNECT(dbg0, 0, dbg0.i_clk, i_clk);
        CONNECT(dbg0, 0, dbg0.i_nrst, i_nrst);
        CONNECT(dbg0, 0, dbg0.i_dport_req_valid, i_dport_req_valid);
        CONNECT(dbg0, 0, dbg0.i_dport_type, i_dport_type);
        CONNECT(dbg0, 0, dbg0.i_dport_addr, i_dport_addr);
        CONNECT(dbg0, 0, dbg0.i_dport_wdata, i_dport_wdata);
        CONNECT(dbg0, 0, dbg0.i_dport_size, i_dport_size);
        CONNECT(dbg0, 0, dbg0.o_dport_req_ready, o_dport_req_ready);
        CONNECT(dbg0, 0, dbg0.i_dport_resp_ready, i_dport_resp_ready);
        CONNECT(dbg0, 0, dbg0.o_dport_resp_valid, o_dport_resp_valid);
        CONNECT(dbg0, 0, dbg0.o_dport_resp_error, o_dport_resp_error);
        CONNECT(dbg0, 0, dbg0.o_dport_rdata, o_dport_rdata);
        CONNECT(dbg0, 0, dbg0.o_csr_req_valid, dbg.csr_req_valid);
        CONNECT(dbg0, 0, dbg0.i_csr_req_ready, iccsr_m1_req_ready);
        CONNECT(dbg0, 0, dbg0.o_csr_req_type, dbg.csr_req_type);
        CONNECT(dbg0, 0, dbg0.o_csr_req_addr, dbg.csr_req_addr);
        CONNECT(dbg0, 0, dbg0.o_csr_req_data, dbg.csr_req_data);
        CONNECT(dbg0, 0, dbg0.i_csr_resp_valid, iccsr_m1_resp_valid);
        CONNECT(dbg0, 0, dbg0.o_csr_resp_ready, dbg.csr_resp_ready);
        CONNECT(dbg0, 0, dbg0.i_csr_resp_data, iccsr_m1_resp_data);
        CONNECT(dbg0, 0, dbg0.i_csr_resp_exception, iccsr_m1_resp_exception);
        CONNECT(dbg0, 0, dbg0.i_progbuf, i_progbuf);
        CONNECT(dbg0, 0, dbg0.o_progbuf_ena, dbg.progbuf_ena);
        CONNECT(dbg0, 0, dbg0.o_progbuf_pc, dbg.progbuf_pc);
        CONNECT(dbg0, 0, dbg0.o_progbuf_instr, dbg.progbuf_instr);
        CONNECT(dbg0, 0, dbg0.i_csr_progbuf_end, csr.progbuf_end);
        CONNECT(dbg0, 0, dbg0.i_csr_progbuf_error, csr.progbuf_error);
        CONNECT(dbg0, 0, dbg0.o_ireg_addr, dbg.ireg_addr);
        CONNECT(dbg0, 0, dbg0.o_ireg_wdata, dbg.ireg_wdata);
        CONNECT(dbg0, 0, dbg0.o_ireg_ena, dbg.ireg_ena);
        CONNECT(dbg0, 0, dbg0.o_ireg_write, dbg.ireg_write);
        CONNECT(dbg0, 0, dbg0.i_ireg_rdata, ireg.dport_rdata);
        CONNECT(dbg0, 0, dbg0.o_mem_req_valid, dbg.mem_req_valid);
        CONNECT(dbg0, 0, dbg0.i_mem_req_ready, w.e.dbg_mem_req_ready);
        CONNECT(dbg0, 0, dbg0.i_mem_req_error, w.e.dbg_mem_req_error);
        CONNECT(dbg0, 0, dbg0.o_mem_req_write, dbg.mem_req_write);
        CONNECT(dbg0, 0, dbg0.o_mem_req_addr, dbg.mem_req_addr);
        CONNECT(dbg0, 0, dbg0.o_mem_req_size, dbg.mem_req_size);
        CONNECT(dbg0, 0, dbg0.o_mem_req_wdata, dbg.mem_req_wdata);
        CONNECT(dbg0, 0, dbg0.i_mem_resp_valid, w.m.debug_valid);
        CONNECT(dbg0, 0, dbg0.i_mem_resp_error, w_mem_resp_error);
        CONNECT(dbg0, 0, dbg0.i_mem_resp_rdata, w.w.wdata);
        CONNECT(dbg0, 0, dbg0.i_e_pc, w.e.pc);
        CONNECT(dbg0, 0, dbg0.i_e_npc, w.e.npc);
        CONNECT(dbg0, 0, dbg0.i_e_call, w.e.call);
        CONNECT(dbg0, 0, dbg0.i_e_ret, w.e.ret);
        CONNECT(dbg0, 0, dbg0.i_e_memop_valid, w.e.memop_valid);
        CONNECT(dbg0, 0, dbg0.i_m_valid, w.m.valid);
    ENDNEW();

    GENERATE("trgen");
    IFGEN (tracer_ena, new STRING("tr_en"));
        NEW(trace0, trace0.getName().c_str());
            CONNECT(trace0, 0, trace0.i_clk, i_clk);
            CONNECT(trace0, 0, trace0.i_nrst, i_nrst);
            CONNECT(trace0, 0, trace0.i_dbg_executed_cnt, csr.executed_cnt);
            CONNECT(trace0, 0, trace0.i_e_valid, w.e.valid);
            CONNECT(trace0, 0, trace0.i_e_pc, w.e.pc);
            CONNECT(trace0, 0, trace0.i_e_instr, w.e.instr);
            CONNECT(trace0, 0, trace0.i_e_wena, w.e.reg_wena);
            CONNECT(trace0, 0, trace0.i_e_waddr, w.e.reg_waddr);
            CONNECT(trace0, 0, trace0.i_e_wdata, w.e.reg_wdata);
            CONNECT(trace0, 0, trace0.i_e_memop_valid, w.e.memop_valid);
            CONNECT(trace0, 0, trace0.i_e_memop_type, w.e.memop_type);
            CONNECT(trace0, 0, trace0.i_e_memop_size, w.e.memop_size);
            CONNECT(trace0, 0, trace0.i_e_memop_addr, w.e.memop_addr);
            CONNECT(trace0, 0, trace0.i_e_memop_wdata, w.e.memop_wdata);
            CONNECT(trace0, 0, trace0.i_e_flushd, csr.flushd_valid);
            CONNECT(trace0, 0, trace0.i_m_pc, w.m.pc);
            CONNECT(trace0, 0, trace0.i_m_valid, w.m.valid);
            CONNECT(trace0, 0, trace0.i_m_memop_ready, w.m.memop_ready);
            CONNECT(trace0, 0, trace0.i_m_wena, w.w.wena);
            CONNECT(trace0, 0, trace0.i_m_waddr, w.w.waddr);
            CONNECT(trace0, 0, trace0.i_m_wdata, w.w.wdata);
            CONNECT(trace0, 0, trace0.i_reg_ignored, w_reg_ignored);
        ENDNEW();
    ENDIFGEN(new STRING("tr_en"));
    ENDGENERATE("trgen");
}

void Processor::proc_comb() {
    SETVAL(w_mem_resp_error, OR2(i_resp_data_load_fault, i_resp_data_store_fault));
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

    SETONE(w_f_flush_ready);
    SETONE(w_immu_core_req_fetch);
    SETZERO(w_dmmu_core_req_fetch);
    SETZERO(unused_immu_core_req_type);
    SETZERO(unused_immu_core_req_wdata);
    SETZERO(unused_immu_core_req_wstrb);
    SETZERO(unused_immu_core_req_size);
    SETZERO(unused_immu_mem_resp_store_fault);

    SETVAL(o_flushi_valid, csr.flushi_valid);
    SETVAL(o_flushi_addr, csr.flush_addr);
    SETVAL(o_flushd_addr, ALLONES());
    SETVAL(o_flushd_valid, w.m.flushd);
    SETVAL(o_halted, w.e.halted);
}
