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

#include "pio_rx_engine.h"

pio_rx_engine::pio_rx_engine(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "PIO_RX_ENGINE", name, comment),
    C_DATA_WIDTH(this, "C_DATA_WIDTH", "64", NO_COMMENT),
    KEEP_WIDTH(this, "KEEP_WIDTH", "DIV(C_DATA_WIDTH,8)", NO_COMMENT),
    i_nrst(this, "i_nrst", "1", NO_COMMENT),
    i_clk(this, "i_clk", "1", "System bus clock"),
    _t0_(this, ""),
    i_m_axis_rx_tdata(this, "i_m_axis_rx_tdata", "C_DATA_WIDTH", NO_COMMENT),
    i_m_axis_rx_tkeep(this, "i_m_axis_rx_tkeep", "KEEP_WIDTH", NO_COMMENT),
    i_m_axis_rx_tlast(this, "i_m_axis_rx_tlast", "1", NO_COMMENT),
    i_m_axis_rx_tvalid(this, "i_m_axis_rx_tvalid", "1", NO_COMMENT),
    o_m_axis_rx_tready(this, "o_m_axis_rx_tready", "1", NO_COMMENT),
    i_m_axis_rx_tuser(this, "i_m_axis_rx_tuser", "22", NO_COMMENT),
    _t1_(this, ""),
    o_req_compl(this, "o_req_compl", "1", ""),
    o_req_compl_wd(this, "o_req_compl_wd", "1", "req completion without data (IO WR32 request)"),
    i_compl_done(this, "i_compl_done", "1", ""),
    _t2_(this, ""),
    o_req_tc(this, "o_req_tc", "3", "Memory Read TC"),
    o_req_td(this, "o_req_td", "1", "Memory Read TD"),
    o_req_ep(this, "o_req_ep", "1", "Memory Read EP"),
    o_req_attr(this, "o_req_attr", "2", "Memory Read Attribute"),
    o_req_len(this, "o_req_len", "10", "Memory Read Length (1DW)"),
    o_req_rid(this, "o_req_rid", "16", "Memory Read Requestor ID"),
    o_req_tag(this, "o_req_tag", "8", "Memory Read Tag"),
    o_req_be(this, "o_req_be", "8", "Memory Read Byte Enables"),
    o_req_addr(this, "o_req_addr", "13", "Memory Read Address"),
    _t3_(this, ""),
    o_wr_addr(this, "o_wr_addr", "10", "Memory Write Address"),
    o_wr_be(this, "o_wr_be", "8", "Memory Write Byte Enable"),
    o_wr_data(this, "o_wr_data", "32", "Memory Write Data"),
    o_wr_en(this, "o_wr_en", "1", "Memory Write Enable"),
    i_wr_busy(this, "i_wr_busy", "1", "Memory Write Busy"),
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

void pio_rx_engine::proc_comb() {
}

void pio_rx_engine::proc_reqff() {
}
