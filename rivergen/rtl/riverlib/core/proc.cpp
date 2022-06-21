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
    coherence_ena(this, "fpu_ena", new BOOL("false")),
    tracer_ena(this, "tracer_ena", new BOOL("true")),
    i_clk(this, "i_clk", new Logic(), "CPU clock"),
    i_nrst(this, "i_nrst", new Logic(), "Reset: active LOW"),
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
    o_halted(this, "o_halted", new Logic(), "CPU halted via debug interface")
{
}

proc::proc(GenObject *parent) :
    FileObject(parent, "proc"),
    proc_(this)
{
}
