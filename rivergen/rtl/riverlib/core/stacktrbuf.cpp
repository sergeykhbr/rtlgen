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

#include "stacktrbuf.h"

StackTraceBuffer::StackTraceBuffer(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "StackTraceBuffer", name, comment),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_raddr(this, "i_raddr", "CFG_LOG2_STACK_TRACE_ADDR"),
    o_rdata(this, "o_rdata", "MUL(2,RISCV_ARCH)"),
    i_we(this, "i_we", "1"),
    i_waddr(this, "i_waddr", "CFG_LOG2_STACK_TRACE_ADDR"),
    i_wdata(this, "i_wdata", "MUL(2,RISCV_ARCH)"),
    // registers
    raddr(this, "raddr", "5"),
    stackbuf(this, "stackbuf", "MUL(2,RISCV_ARCH)", "STACK_TRACE_BUF_SIZE", NO_COMMENT),
    // process
    comb(this)
{
}

void StackTraceBuffer::proc_comb() {
    SETVAL(raddr, i_raddr);

    IF (NZ(i_we));
        SETARRITEM(stackbuf, TO_INT(i_waddr), stackbuf, i_wdata);
    ENDIF();

TEXT();
    SETVAL(o_rdata, ARRITEM(stackbuf, TO_INT(raddr), stackbuf));
}
