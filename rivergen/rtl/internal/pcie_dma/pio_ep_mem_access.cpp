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
    wb_addr(this, "wb_addr", "11", NO_COMMENT),
    // registers
    r_wr_busy(this, "r_wr_busy", "1", RSTVAL_ZERO, NO_COMMENT),
    //
    comb(this),
    reqff(this, "reqff", &i_clk, CLK_POSEDGE, 0, ACTIVE_NONE, NO_COMMENT),
    ram0(this, "ram0")
{
    Operation::start(this);

    ram0.abits.setObjValue(new DecConst(11));
    ram0.log2_dbytes.setObjValue(new DecConst(2));
    NEW(ram0, ram0.getName().c_str());
        CONNECT(ram0, 0, ram0.i_clk, i_clk);
        CONNECT(ram0, 0, ram0.i_addr, wb_addr);
        CONNECT(ram0, 0, ram0.i_wena, i_wr_en);
        CONNECT(ram0, 0, ram0.i_wstrb, i_wr_be);
        CONNECT(ram0, 0, ram0.i_wdata, i_wr_data);
        CONNECT(ram0, 0, ram0.o_rdata, o_rd_data);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();

    Operation::start(&reqff);
    proc_reqff();
}

void pio_ep_mem_access::proc_comb() {
    IF (NZ(i_wr_en));
        SETVAL(wb_addr, i_wr_addr);
    ELSE();
        SETVAL(wb_addr, i_rd_addr);
    ENDIF();
    ASSIGN(o_wr_busy, r_wr_busy);
}

void pio_ep_mem_access::proc_reqff() {
    IF (EZ(i_nrst));
        SETVAL_NB(r_wr_busy, CONST("0"));
    ELSE();
        SETVAL_NB(r_wr_busy, i_wr_en);
    ENDIF();
}
