// 
//  Copyright 2025 Sergey Khabarov, sergeykhbr@gmail.com
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

#include "cdc_dp_mem.h"

cdc_dp_mem::cdc_dp_mem(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "cdc_dp_mem", name, comment),
    abits(this, "abits", "6", NO_COMMENT),
    dbits(this, "dbits", "65", NO_COMMENT),
    i_wclk(this, "i_wclk", "1", "Write clock"),
    i_wena(this, "i_wena", "1"),
    i_waddr(this, "i_addr", "abits", "write address"),
    i_wdata(this, "i_wdata", "dbits"),
    i_rclk(this, "i_rclk", "1", "Read clock"),
    i_raddr(this, "i_raddr", "abits", "read address"),
    o_rdata(this, "o_rdata", "dbits"),
    DEPTH(this, "DEPTH", "POW2(1,abits)"),
    mem(this, "mem", "dbits", "DEPTH", NO_COMMENT),
    // process
    wproc(this, "wproc", &i_wclk, CLK_POSEDGE, 0, ACTIVE_NONE, NO_COMMENT),
    rproc(this, "rproc", 0, CLK_ALWAYS, 0, ACTIVE_NONE, NO_COMMENT)
{
    Operation::start(this);

    Operation::start(&wproc);
    proc_wproc();

    Operation::start(&rproc);
    proc_rproc();
}

void cdc_dp_mem::proc_wproc() {
    IF (NZ(i_wena));
        SETARRITEM(mem, TO_INT(i_waddr), mem, i_wdata);
    ENDIF();
}

void cdc_dp_mem::proc_rproc() {
    ASSIGN(o_rdata, ARRITEM(mem, TO_INT(i_raddr), mem));
}

