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

#include "river_amba.h"

RiverAmba::RiverAmba(GenObject *parent, const char *name, const char *depth) :
    ModuleObject(parent, "RiverAmba", name, depth),
    // Generic parameters
    hartid(this, "hartid", "0"),
    fpu_ena(this, "fpu_ena", "true"),
    coherence_ena(this, "coherence_ena", "false"),
    tracer_ena(this, "tracer_ena", "true"),
    // Ports
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_mtimer(this, "i_mtimer", "64", "Read-only shadow value of memory-mapped mtimer register (see CLINT)."),
    i_msti(this, "i_msti"),
    o_msto(this, "o_msto"),
    i_dport(this, "i_dport"),
    o_dport(this, "o_dport"),
    i_irq_pending(this, "i_irq_pending", "IRQ_TOTAL", "Per Hart pending interrupts pins"),
    o_flush_l2(this, "o_flush_l2", "1", "Flush L2 after D$ has been finished"),
    o_halted(this, "o_halted", "1", "CPU halted via debug interface"),
    o_available("1", "o_available", "1", this, "CPU was instantitated of stubbed"),
    i_progbuf(this, "i_progbuf", "MUL(32,CFG_PROGBUF_REG_TOTAL)", "progam buffer"),
    // params
    // Singals:
    req_mem_ready_i(this, "req_mem_ready_i", "1"),
    req_mem_path_o(this, "req_mem_path_o", "1"),
    req_mem_valid_o(this, "req_mem_valid_o", "1"),
    req_mem_type_o(this, "req_mem_type_o", "REQ_MEM_TYPE_BITS"),
    req_mem_size_o(this, "req_mem_size_o", "3"),
    req_mem_addr_o(this, "req_mem_addr_o", "CFG_CPU_ADDR_BITS"),
    req_mem_strob_o(this, "req_mem_strob_o", "L1CACHE_BYTES_PER_LINE"),
    req_mem_data_o(this, "req_mem_data_o", "L1CACHE_LINE_BITS"),
    resp_mem_data_i(this, "resp_mem_data_i", "L1CACHE_LINE_BITS"),
    resp_mem_path_i(this, "resp_mem_path_i", "1"),
    resp_mem_valid_i(this, "resp_mem_valid_i", "1"),
    resp_mem_load_fault_i(this, "resp_mem_load_fault_i", "1"),
    resp_mem_store_fault_i(this, "resp_mem_store_fault_i", "1"),
    _dsnoop0_(this, "D$ Snoop interface"),
    req_snoop_valid_i(this, "req_snoop_valid_i", "1"),
    req_snoop_type_i(this, "req_snoop_type_i", "SNOOP_REQ_TYPE_BITS"),
    req_snoop_ready_o(this, "req_snoop_ready_o", "1"),
    req_snoop_addr_i(this, "req_snoop_addr_i", "CFG_CPU_ADDR_BITS"),
    resp_snoop_ready_i(this, "resp_snoop_ready_i", "1"),
    resp_snoop_valid_o(this, "resp_snoop_valid_o", "1"),
    resp_snoop_data_o(this, "resp_snoop_data_o", "L1CACHE_LINE_BITS"),
    resp_snoop_flags_o(this, "resp_snoop_flags_o", "DTAG_FL_TOTAL"),
    w_dporti_haltreq(this, "w_dporti_haltreq", "1"),
    w_dporti_resumereq(this, "w_dporti_resumereq", "1"),
    w_dporti_resethaltreq(this, "w_dporti_resethaltreq", "1"),
    w_dporti_hartreset(this, "w_dporti_hartreset", "1"),
    w_dporti_req_valid(this, "w_dporti_req_valid", "1"),
    wb_dporti_dtype(this, "wb_dporti_dtype", "DPortReq_Total"),
    wb_dporti_addr(this, "wb_dporti_addr", "RISCV_ARCH"),
    wb_dporti_wdata(this, "wb_dporti_wdata", "RISCV_ARCH"),
    wb_dporti_size(this, "wb_dporti_size", "3"),
    w_dporti_resp_ready(this, "w_dporti_resp_ready", "1"),
    w_dporto_req_ready(this, "w_dporto_req_ready", "1"),
    w_dporto_resp_valid(this, "w_dporto_resp_valid", "1"),
    w_dporto_resp_error(this, "w_dporto_resp_error", "1"),
    wb_dporto_rdata(this, "wb_dporto_rdata", "RISCV_ARCH"),
    // registers
    // functions
    // submodules:
    river0(this, "river0"),
    l1dma0(this, "l1dma0", NO_COMMENT),
    comb(this)
{
    Operation::start(this);

    // Create and connet Sub-modules:
    river0.hartid.setObjValue(&hartid);
    river0.fpu_ena.setObjValue(&fpu_ena);
    river0.coherence_ena.setObjValue(&coherence_ena);
    river0.tracer_ena.setObjValue(&tracer_ena);
    NEW(river0, river0.getName().c_str());
        CONNECT(river0, 0, river0.i_clk, i_clk);
        CONNECT(river0, 0, river0.i_nrst, i_nrst);
        CONNECT(river0, 0, river0.i_mtimer, i_mtimer);
        CONNECT(river0, 0, river0.i_req_mem_ready, req_mem_ready_i);
        CONNECT(river0, 0, river0.o_req_mem_path, req_mem_path_o);
        CONNECT(river0, 0, river0.o_req_mem_valid, req_mem_valid_o);
        CONNECT(river0, 0, river0.o_req_mem_type, req_mem_type_o);
        CONNECT(river0, 0, river0.o_req_mem_size, req_mem_size_o);
        CONNECT(river0, 0, river0.o_req_mem_addr, req_mem_addr_o);
        CONNECT(river0, 0, river0.o_req_mem_strob, req_mem_strob_o);
        CONNECT(river0, 0, river0.o_req_mem_data, req_mem_data_o);
        CONNECT(river0, 0, river0.i_resp_mem_valid, resp_mem_valid_i);
        CONNECT(river0, 0, river0.i_resp_mem_path, resp_mem_path_i);
        CONNECT(river0, 0, river0.i_resp_mem_data, resp_mem_data_i);
        CONNECT(river0, 0, river0.i_resp_mem_load_fault, resp_mem_load_fault_i);
        CONNECT(river0, 0, river0.i_resp_mem_store_fault, resp_mem_store_fault_i);
        CONNECT(river0, 0, river0.i_req_snoop_valid, req_snoop_valid_i);
        CONNECT(river0, 0, river0.i_req_snoop_type, req_snoop_type_i);
        CONNECT(river0, 0, river0.o_req_snoop_ready, req_snoop_ready_o);
        CONNECT(river0, 0, river0.i_req_snoop_addr, req_snoop_addr_i);
        CONNECT(river0, 0, river0.i_resp_snoop_ready, resp_snoop_ready_i);
        CONNECT(river0, 0, river0.o_resp_snoop_valid, resp_snoop_valid_o);
        CONNECT(river0, 0, river0.o_resp_snoop_data, resp_snoop_data_o);
        CONNECT(river0, 0, river0.o_resp_snoop_flags, resp_snoop_flags_o);
        CONNECT(river0, 0, river0.o_flush_l2, o_flush_l2);
        CONNECT(river0, 0, river0.i_irq_pending, i_irq_pending);
        CONNECT(river0, 0, river0.i_haltreq, w_dporti_haltreq);
        CONNECT(river0, 0, river0.i_resumereq, w_dporti_resumereq);
        CONNECT(river0, 0, river0.i_dport_req_valid, w_dporti_req_valid);
        CONNECT(river0, 0, river0.i_dport_type, wb_dporti_dtype);
        CONNECT(river0, 0, river0.i_dport_addr, wb_dporti_addr);
        CONNECT(river0, 0, river0.i_dport_wdata, wb_dporti_wdata);
        CONNECT(river0, 0, river0.i_dport_size, wb_dporti_size);
        CONNECT(river0, 0, river0.o_dport_req_ready, w_dporto_req_ready);
        CONNECT(river0, 0, river0.i_dport_resp_ready, w_dporti_resp_ready);
        CONNECT(river0, 0, river0.o_dport_resp_valid, w_dporto_resp_valid);
        CONNECT(river0, 0, river0.o_dport_resp_error, w_dporto_resp_error);
        CONNECT(river0, 0, river0.o_dport_rdata, wb_dporto_rdata);
        CONNECT(river0, 0, river0.i_progbuf, i_progbuf);
        CONNECT(river0, 0, river0.o_halted, o_halted);
    ENDNEW();

    l1dma0.abits.setObjValue(SCV_get_cfg_type(this, "CFG_CPU_ADDR_BITS"));
    l1dma0.userbits.setObjValue(new DecConst(1));
    l1dma0.base_offset.setObjValue(new HexLogicConst(SCV_get_cfg_type(this, "CFG_CPU_ADDR_BITS"), 0x0));
    l1dma0.coherence_ena.setObjValue(&coherence_ena);
    NEW(l1dma0, l1dma0.getName().c_str());
        CONNECT(l1dma0, 0, l1dma0.i_nrst, i_nrst);
        CONNECT(l1dma0, 0, l1dma0.i_clk, i_clk);
        CONNECT(l1dma0, 0, l1dma0.o_req_mem_ready, req_mem_ready_i);
        CONNECT(l1dma0, 0, l1dma0.i_req_mem_path, req_mem_path_o);
        CONNECT(l1dma0, 0, l1dma0.i_req_mem_valid, req_mem_valid_o);
        CONNECT(l1dma0, 0, l1dma0.i_req_mem_type, req_mem_type_o);
        CONNECT(l1dma0, 0, l1dma0.i_req_mem_size, req_mem_size_o);
        CONNECT(l1dma0, 0, l1dma0.i_req_mem_addr, req_mem_addr_o);
        CONNECT(l1dma0, 0, l1dma0.i_req_mem_strob, req_mem_strob_o);
        CONNECT(l1dma0, 0, l1dma0.i_req_mem_data, req_mem_data_o);
        CONNECT(l1dma0, 0, l1dma0.o_resp_mem_path, resp_mem_path_i);
        CONNECT(l1dma0, 0, l1dma0.o_resp_mem_valid, resp_mem_valid_i);
        CONNECT(l1dma0, 0, l1dma0.o_resp_mem_load_fault, resp_mem_load_fault_i);
        CONNECT(l1dma0, 0, l1dma0.o_resp_mem_store_fault, resp_mem_store_fault_i);
        CONNECT(l1dma0, 0, l1dma0.o_resp_mem_data, resp_mem_data_i);
        CONNECT(l1dma0, 0, l1dma0.o_req_snoop_valid, req_snoop_valid_i);
        CONNECT(l1dma0, 0, l1dma0.o_req_snoop_type, req_snoop_type_i);
        CONNECT(l1dma0, 0, l1dma0.i_req_snoop_ready, req_snoop_ready_o);
        CONNECT(l1dma0, 0, l1dma0.o_req_snoop_addr, req_snoop_addr_i);
        CONNECT(l1dma0, 0, l1dma0.o_resp_snoop_ready, resp_snoop_ready_i);
        CONNECT(l1dma0, 0, l1dma0.i_resp_snoop_valid, resp_snoop_valid_o);
        CONNECT(l1dma0, 0, l1dma0.i_resp_snoop_data, resp_snoop_data_o);
        CONNECT(l1dma0, 0, l1dma0.i_resp_snoop_flags, resp_snoop_flags_o);
        CONNECT(l1dma0, 0, l1dma0.i_msti, i_msti);
        CONNECT(l1dma0, 0, l1dma0.o_msto, o_msto);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}


void RiverAmba::proc_comb() {
TEXT();
    SETVAL(w_dporti_haltreq, i_dport.haltreq, "systemc compatibility");
    SETVAL(w_dporti_resumereq, i_dport.resumereq, "systemc compatibility");
    SETVAL(w_dporti_resethaltreq, i_dport.resethaltreq, "systemc compatibility");
    SETVAL(w_dporti_hartreset, i_dport.hartreset, "systemc compatibility");
    SETVAL(w_dporti_req_valid, i_dport.req_valid, "systemc compatibility");
    SETVAL(wb_dporti_dtype, i_dport.dtype, "systemc compatibility");
    SETVAL(wb_dporti_addr, i_dport.addr, "systemc compatibility");
    SETVAL(wb_dporti_wdata, i_dport.wdata, "systemc compatibility");
    SETVAL(wb_dporti_size, i_dport.size, "systemc compatibility");
    SETVAL(w_dporti_resp_ready, i_dport.resp_ready, "systemc compatibility");

TEXT();
    SETVAL(comb.vdporto.req_ready, w_dporto_req_ready, "systemc compatibility");
    SETVAL(comb.vdporto.resp_valid, w_dporto_resp_valid, "systemc compatibility");
    SETVAL(comb.vdporto.resp_error, w_dporto_resp_error, "systemc compatibility");
    SETVAL(comb.vdporto.rdata, wb_dporto_rdata, "systemc compatibility");


TEXT();
    SETVAL(o_dport, comb.vdporto, "systemc compatibility");
    SETVAL(o_available, CONST("1", 1));
}
