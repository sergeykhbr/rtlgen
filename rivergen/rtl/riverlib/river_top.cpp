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

RiverTop::RiverTop(GenObject *parent, const char *name) :
    ModuleObject(parent, "RiverTop", name),
    // Generic parameters
    hartid(this, "hartid", "0"),
    fpu_ena(this, "fpu_ena", "true"),
    coherence_ena(this, "coherence_ena", "false"),
    tracer_ena(this, "tracer_ena", "true"),
    // Ports
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_mtimer(this, "i_mtimer", "64", "Read-only shadow value of memory-mapped mtimer register (see CLINT)."),
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
    _DSnoopInterface0_(this, "$D Snoop interface:"),
    i_req_snoop_valid(this, "i_req_snoop_valid"),
    i_req_snoop_type(this, "i_req_snoop_type", "SNOOP_REQ_TYPE_BITS"),
    o_req_snoop_ready(this, "o_req_snoop_ready"),
    i_req_snoop_addr(this, "i_req_snoop_addr", "CFG_CPU_ADDR_BITS"),
    i_resp_snoop_ready(this, "i_resp_snoop_ready"),
    o_resp_snoop_valid(this, "o_resp_snoop_valid"),
    o_resp_snoop_data(this, "o_resp_snoop_data", "L1CACHE_LINE_BITS"),
    o_resp_snoop_flags(this, "o_resp_snoop_flags", "DTAG_FL_TOTAL"),
    o_flush_l2(this, "o_flush_l2", "1", "Flush L2 after D$ has been finished"),
    _Interrupts0_(this, "Interrupt lines:"),
    i_irq_pending(this, "i_irq_pending", "IRQ_TOTAL", "Per Hart pending interrupts pins"),
    _Debug0(this, "Debug interface:"),
    i_haltreq(this, "i_haltreq", "1", "DMI: halt request from debug unit"),
    i_resumereq(this, "i_resumereq", "1", "DMI: resume request from debug unit"),
    i_dport_req_valid(this, "i_dport_req_valid", "1", "Debug access from DSU is valid"),
    i_dport_type(this, "i_dport_type", "DPortReq_Total", "Debug access type"),
    i_dport_addr(this, "i_dport_addr", "CFG_CPU_ADDR_BITS", "dport address"),
    i_dport_wdata(this, "i_dport_wdata", "RISCV_ARCH", "Write value"),
    i_dport_size(this, "i_dport_size", "3", "reg/mem access size:0=1B;...,4=128B;"),
    o_dport_req_ready(this, "o_dport_req_ready"),
    i_dport_resp_ready(this, "i_dport_resp_ready", "1", "ready to accepd response"),
    o_dport_resp_valid(this, "o_dport_resp_valid", "1", "Response is valid"),
    o_dport_resp_error(this, "o_dport_resp_error", "1", "Something wrong during command execution"),
    o_dport_rdata(this, "o_dport_rdata", "RISCV_ARCH", "Response value"),
    i_progbuf(this, "i_progbuf", "MUL(32,CFG_PROGBUF_REG_TOTAL)", "progam buffer"),
    o_halted(this, "o_halted", "1", "CPU halted via debug interface"),
    // Singals:
    _ControlPath0_(this, "Control path:"),
    w_req_ctrl_ready(this, "w_req_ctrl_ready", "1"),
    w_req_ctrl_valid(this, "w_req_ctrl_valid", "1"),
    wb_req_ctrl_addr(this, "wb_req_ctrl_addr", "CFG_CPU_ADDR_BITS"),
    w_resp_ctrl_valid(this, "w_resp_ctrl_valid", "1"),
    wb_resp_ctrl_addr(this, "wb_resp_ctrl_addr", "CFG_CPU_ADDR_BITS"),
    wb_resp_ctrl_data(this, "wb_resp_ctrl_data", "64"),
    w_resp_ctrl_load_fault(this, "w_resp_ctrl_load_fault", "1"),
    w_resp_ctrl_executable(this, "w_resp_ctrl_executable", "1"),
    w_resp_ctrl_ready(this, "w_resp_ctrl_ready", "1"),
    _DataPath0_(this, "Data path:"),
    w_req_data_ready(this, "w_req_data_ready", "1"),
    w_req_data_valid(this, "w_req_data_valid", "1"),
    wb_req_data_type(this, "wb_req_data_type", "MemopType_Total"),
    wb_req_data_addr(this, "wb_req_data_addr", "CFG_CPU_ADDR_BITS"),
    wb_req_data_wdata(this, "wb_req_data_wdata", "64"),
    wb_req_data_wstrb(this, "wb_req_data_wstrb", "8"),
    wb_req_data_size(this, "wb_req_data_size", "2"),
    w_resp_data_valid(this, "w_resp_data_valid", "1"),
    wb_resp_data_addr(this, "wb_resp_data_addr", "CFG_CPU_ADDR_BITS"),
    wb_resp_data_data(this, "wb_resp_data_data", "64"),
    w_resp_data_load_fault(this, "w_resp_data_load_fault", "1"),
    w_resp_data_store_fault(this, "w_resp_data_store_fault", "1"),
    w_resp_data_er_mpu_load(this, "w_resp_data_er_mpu_load", "1"),
    w_resp_data_er_mpu_store(this, "w_resp_data_er_mpu_store", "1"),
    wb_resp_data_fault_addr(this, "wb_resp_data_fault_addr", "CFG_CPU_ADDR_BITS"),
    w_resp_data_ready(this, "w_resp_data_ready", "1"),
    w_mpu_region_we(this, "w_mpu_region_we", "1"),
    wb_mpu_region_idx(this, "wb_mpu_region_idx", "CFG_MPU_TBL_WIDTH"),
    wb_mpu_region_addr(this, "wb_mpu_region_addr", "CFG_CPU_ADDR_BITS"),
    wb_mpu_region_mask(this, "wb_mpu_region_mask", "CFG_CPU_ADDR_BITS"),
    wb_mpu_region_flags(this, "wb_mpu_region_flags", "CFG_MPU_FL_TOTAL"),
    wb_flush_address(this, "wb_flush_address", "CFG_CPU_ADDR_BITS"),
    w_flush_valid(this, "w_flush_valid", "1"),
    wb_data_flush_address(this, "wb_data_flush_address", "CFG_CPU_ADDR_BITS"),
    w_data_flush_valid(this, "w_data_flush_valid", "1"),
    w_data_flush_end(this, "w_data_flush_end", "1"),
    proc0(this, "proc0"),
    cache0(this, "cache0"),
    comb(this)
{
    Operation::start(this);

    // Create and connet Sub-modules:
    NEW(proc0, proc0.getName().c_str());
        CONNECT(proc0, 0, proc0.i_clk, i_clk);
        CONNECT(proc0, 0, proc0.i_nrst, i_nrst);
        CONNECT(proc0, 0, proc0.i_mtimer, i_mtimer);
        CONNECT(proc0, 0, proc0.i_req_ctrl_ready, w_req_ctrl_ready);
        CONNECT(proc0, 0, proc0.o_req_ctrl_valid, w_req_ctrl_valid);
        CONNECT(proc0, 0, proc0.o_req_ctrl_addr, wb_req_ctrl_addr);
        CONNECT(proc0, 0, proc0.i_resp_ctrl_valid, w_resp_ctrl_valid);
        CONNECT(proc0, 0, proc0.i_resp_ctrl_addr, wb_resp_ctrl_addr);
        CONNECT(proc0, 0, proc0.i_resp_ctrl_data, wb_resp_ctrl_data);
        CONNECT(proc0, 0, proc0.i_resp_ctrl_load_fault, w_resp_ctrl_load_fault);
        CONNECT(proc0, 0, proc0.i_resp_ctrl_executable, w_resp_ctrl_executable);
        CONNECT(proc0, 0, proc0.o_resp_ctrl_ready, w_resp_ctrl_ready);
        CONNECT(proc0, 0, proc0.i_req_data_ready, w_req_data_ready);
        CONNECT(proc0, 0, proc0.o_req_data_valid, w_req_data_valid);
        CONNECT(proc0, 0, proc0.o_req_data_type, wb_req_data_type);
        CONNECT(proc0, 0, proc0.o_req_data_addr, wb_req_data_addr);
        CONNECT(proc0, 0, proc0.o_req_data_wdata, wb_req_data_wdata);
        CONNECT(proc0, 0, proc0.o_req_data_wstrb, wb_req_data_wstrb);
        CONNECT(proc0, 0, proc0.o_req_data_size, wb_req_data_size);
        CONNECT(proc0, 0, proc0.i_resp_data_valid, w_resp_data_valid);
        CONNECT(proc0, 0, proc0.i_resp_data_addr, wb_resp_data_addr);
        CONNECT(proc0, 0, proc0.i_resp_data_data, wb_resp_data_data);
        CONNECT(proc0, 0, proc0.i_resp_data_fault_addr, wb_resp_data_fault_addr);
        CONNECT(proc0, 0, proc0.i_resp_data_load_fault, w_resp_data_load_fault);
        CONNECT(proc0, 0, proc0.i_resp_data_store_fault, w_resp_data_store_fault);
        CONNECT(proc0, 0, proc0.i_resp_data_er_mpu_load, w_resp_data_er_mpu_load);
        CONNECT(proc0, 0, proc0.i_resp_data_er_mpu_store, w_resp_data_er_mpu_store);
        CONNECT(proc0, 0, proc0.o_resp_data_ready, w_resp_data_ready);
        CONNECT(proc0, 0, proc0.i_irq_pending, i_irq_pending);
        CONNECT(proc0, 0, proc0.o_mpu_region_we, w_mpu_region_we);
        CONNECT(proc0, 0, proc0.o_mpu_region_idx, wb_mpu_region_idx);
        CONNECT(proc0, 0, proc0.o_mpu_region_addr, wb_mpu_region_addr);
        CONNECT(proc0, 0, proc0.o_mpu_region_mask, wb_mpu_region_mask);
        CONNECT(proc0, 0, proc0.o_mpu_region_flags, wb_mpu_region_flags);
        CONNECT(proc0, 0, proc0.i_haltreq, i_haltreq);
        CONNECT(proc0, 0, proc0.i_resumereq, i_resumereq);
        CONNECT(proc0, 0, proc0.i_dport_req_valid, i_dport_req_valid);
        CONNECT(proc0, 0, proc0.i_dport_type, i_dport_type);
        CONNECT(proc0, 0, proc0.i_dport_addr, i_dport_addr);
        CONNECT(proc0, 0, proc0.i_dport_wdata, i_dport_wdata);
        CONNECT(proc0, 0, proc0.i_dport_size, i_dport_size);
        CONNECT(proc0, 0, proc0.o_dport_req_ready, o_dport_req_ready);
        CONNECT(proc0, 0, proc0.i_dport_resp_ready, i_dport_resp_ready);
        CONNECT(proc0, 0, proc0.o_dport_resp_valid, o_dport_resp_valid);
        CONNECT(proc0, 0, proc0.o_dport_resp_error, o_dport_resp_error);
        CONNECT(proc0, 0, proc0.o_dport_rdata, o_dport_rdata);
        CONNECT(proc0, 0, proc0.i_progbuf, i_progbuf);
        CONNECT(proc0, 0, proc0.o_halted, o_halted);
        CONNECT(proc0, 0, proc0.o_flush_address, wb_flush_address);
        CONNECT(proc0, 0, proc0.o_flush_valid, w_flush_valid);
        CONNECT(proc0, 0, proc0.o_data_flush_address, wb_data_flush_address);
        CONNECT(proc0, 0, proc0.o_data_flush_valid, w_data_flush_valid);
        CONNECT(proc0, 0, proc0.i_data_flush_end, w_data_flush_end);
    ENDNEW();

    NEW(cache0, cache0.getName().c_str());
        //CONNECT(cache0, 0, cache0.coherence_ena, coherence_ena);
        CONNECT(cache0, 0, cache0.i_clk, i_clk);
        CONNECT(cache0, 0, cache0.i_nrst, i_nrst);
        CONNECT(cache0, 0, cache0.i_req_ctrl_valid, w_req_ctrl_valid);
        CONNECT(cache0, 0, cache0.i_req_ctrl_addr, wb_req_ctrl_addr);
        CONNECT(cache0, 0, cache0.o_req_ctrl_ready, w_req_ctrl_ready);
        CONNECT(cache0, 0, cache0.o_resp_ctrl_valid, w_resp_ctrl_valid);
        CONNECT(cache0, 0, cache0.o_resp_ctrl_addr, wb_resp_ctrl_addr);
        CONNECT(cache0, 0, cache0.o_resp_ctrl_data, wb_resp_ctrl_data);
        CONNECT(cache0, 0, cache0.o_resp_ctrl_load_fault, w_resp_ctrl_load_fault);
        CONNECT(cache0, 0, cache0.o_resp_ctrl_executable, w_resp_ctrl_executable);
        CONNECT(cache0, 0, cache0.i_resp_ctrl_ready, w_resp_ctrl_ready);
        CONNECT(cache0, 0, cache0.i_req_data_valid, w_req_data_valid);
        CONNECT(cache0, 0, cache0.i_req_data_type, wb_req_data_type);
        CONNECT(cache0, 0, cache0.i_req_data_addr, wb_req_data_addr);
        CONNECT(cache0, 0, cache0.i_req_data_wdata, wb_req_data_wdata);
        CONNECT(cache0, 0, cache0.i_req_data_wstrb, wb_req_data_wstrb);
        CONNECT(cache0, 0, cache0.i_req_data_size, wb_req_data_size);
        CONNECT(cache0, 0, cache0.o_req_data_ready, w_req_data_ready);
        CONNECT(cache0, 0, cache0.o_resp_data_valid, w_resp_data_valid);
        CONNECT(cache0, 0, cache0.o_resp_data_addr, wb_resp_data_addr);
        CONNECT(cache0, 0, cache0.o_resp_data_data, wb_resp_data_data);
        CONNECT(cache0, 0, cache0.o_resp_data_fault_addr, wb_resp_data_fault_addr);
        CONNECT(cache0, 0, cache0.o_resp_data_load_fault, w_resp_data_load_fault);
        CONNECT(cache0, 0, cache0.o_resp_data_store_fault, w_resp_data_store_fault);
        CONNECT(cache0, 0, cache0.o_resp_data_er_mpu_load, w_resp_data_er_mpu_load);
        CONNECT(cache0, 0, cache0.o_resp_data_er_mpu_store, w_resp_data_er_mpu_store);
        CONNECT(cache0, 0, cache0.i_resp_data_ready, w_resp_data_ready);
        CONNECT(cache0, 0, cache0.i_req_mem_ready, i_req_mem_ready);
        CONNECT(cache0, 0, cache0.o_req_mem_path, o_req_mem_path);
        CONNECT(cache0, 0, cache0.o_req_mem_valid, o_req_mem_valid);
        CONNECT(cache0, 0, cache0.o_req_mem_type, o_req_mem_type);
        CONNECT(cache0, 0, cache0.o_req_mem_size, o_req_mem_size);
        CONNECT(cache0, 0, cache0.o_req_mem_addr, o_req_mem_addr);
        CONNECT(cache0, 0, cache0.o_req_mem_strob, o_req_mem_strob);
        CONNECT(cache0, 0, cache0.o_req_mem_data, o_req_mem_data);
        CONNECT(cache0, 0, cache0.i_resp_mem_valid, i_resp_mem_valid);
        CONNECT(cache0, 0, cache0.i_resp_mem_path, i_resp_mem_path);
        CONNECT(cache0, 0, cache0.i_resp_mem_data, i_resp_mem_data);
        CONNECT(cache0, 0, cache0.i_resp_mem_load_fault, i_resp_mem_load_fault);
        CONNECT(cache0, 0, cache0.i_resp_mem_store_fault, i_resp_mem_store_fault);
        CONNECT(cache0, 0, cache0.i_mpu_region_we, w_mpu_region_we);
        CONNECT(cache0, 0, cache0.i_mpu_region_idx, wb_mpu_region_idx);
        CONNECT(cache0, 0, cache0.i_mpu_region_addr, wb_mpu_region_addr);
        CONNECT(cache0, 0, cache0.i_mpu_region_mask, wb_mpu_region_mask);
        CONNECT(cache0, 0, cache0.i_mpu_region_flags, wb_mpu_region_flags);
        CONNECT(cache0, 0, cache0.i_req_snoop_valid, i_req_snoop_valid);
        CONNECT(cache0, 0, cache0.i_req_snoop_type, i_req_snoop_type);
        CONNECT(cache0, 0, cache0.o_req_snoop_ready, o_req_snoop_ready);
        CONNECT(cache0, 0, cache0.i_req_snoop_addr, i_req_snoop_addr);
        CONNECT(cache0, 0, cache0.i_resp_snoop_ready, i_resp_snoop_ready);
        CONNECT(cache0, 0, cache0.o_resp_snoop_valid, o_resp_snoop_valid);
        CONNECT(cache0, 0, cache0.o_resp_snoop_data, o_resp_snoop_data);
        CONNECT(cache0, 0, cache0.o_resp_snoop_flags, o_resp_snoop_flags);
        CONNECT(cache0, 0, cache0.i_flush_address, wb_flush_address);
        CONNECT(cache0, 0, cache0.i_flush_valid, w_flush_valid);
        CONNECT(cache0, 0, cache0.i_data_flush_address, wb_data_flush_address);
        CONNECT(cache0, 0, cache0.i_data_flush_valid, w_data_flush_valid);
        CONNECT(cache0, 0, cache0.o_data_flush_end, w_data_flush_end);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void RiverTop::proc_comb() {
    SETVAL(o_flush_l2, w_data_flush_end);
}
