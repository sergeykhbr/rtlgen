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

#include "pio_ep_mem_access.h"

pio_ep_mem_access::pio_ep_mem_access(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "PIO_EP_MEM_ACCESS", name, comment),
    i_nrst(this, "i_nrst", "1", NO_COMMENT),
    i_clk(this, "i_clk", "1", "System bus clock"),
    _t0_(this, "Read Port"),
    i_rd_addr(this, "i_rd_addr", "11", NO_COMMENT),
    i_rd_be(this, "i_rd_be", "4", NO_COMMENT),
    o_rd_data(this, "o_rd_data", "32", NO_COMMENT),
    _t1_(this, "Write Port"),
    i_wr_addr(this, "i_wr_addr", "11", NO_COMMENT),
    i_wr_be(this, "i_wr_be", "8", NO_COMMENT),
    i_wr_data(this, "i_wr_data", "32", NO_COMMENT),
    i_wr_en(this, "i_wr_en", "1", NO_COMMENT),
    o_wr_busy(this, "o_wr_busy", "1", NO_COMMENT),
    // params
    // signals
    // registers
    //
    comb(this),
    reqff(this, "reqff", &i_clk, CLK_POSEDGE, 0, ACTIVE_NONE, NO_COMMENT)
{
    Operation::start(this);


    Operation::start(&comb);
    proc_comb();

    Operation::start(&reqff);
    proc_reqff();
}

void pio_ep_mem_access::proc_comb() {
}

void pio_ep_mem_access::proc_reqff() {
}
