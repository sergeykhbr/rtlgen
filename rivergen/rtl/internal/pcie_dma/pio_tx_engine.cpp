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

#include "pio_tx_engine.h"

pio_tx_engine::pio_tx_engine(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "PIO_TX_ENGINE", name, comment),
    C_DATA_WIDTH(this, "C_DATA_WIDTH", "64", NO_COMMENT),
    KEEP_WIDTH(this, "KEEP_WIDTH", "DIV(C_DATA_WIDTH,8)", NO_COMMENT),
    i_nrst(this, "i_nrst", "1", NO_COMMENT),
    i_clk(this, "i_clk", "1", "System bus clock"),
    _t0_(this, ""),
    i_s_axis_tx_tready(this, "i_s_axis_tx_tready", "1", NO_COMMENT),
    o_s_axis_tx_tdata(this, "o_s_axis_tx_tdata", "C_DATA_WIDTH", NO_COMMENT),
    o_s_axis_tx_tkeep(this, "o_s_axis_tx_tkeep", "KEEP_WIDTH", NO_COMMENT),
    o_s_axis_tx_tlast(this, "o_s_axis_tx_tlast", "1", NO_COMMENT),
    o_s_axis_tx_tvalid(this, "o_s_axis_tx_tvalid", "1", NO_COMMENT),
    o_tx_src_dsc(this, "o_tx_src_dsc", "1", NO_COMMENT),
    _t1_(this, ""),
    i_req_compl(this, "i_req_compl", "1", NO_COMMENT),
    i_req_compl_wd(this, "i_req_compl_wd", "1", NO_COMMENT),
    o_compl_done(this, "o_compl_done", "1", NO_COMMENT),
    _t2_(this, ""),
    i_req_tc(this, "i_req_tc", "3", NO_COMMENT),
    i_req_td(this, "i_req_td", "1", NO_COMMENT),
    i_req_ep(this, "i_req_ep", "1", NO_COMMENT),
    i_req_attr(this, "i_req_attr", "2", NO_COMMENT),
    i_req_len(this, "i_req_len", "10", NO_COMMENT),
    i_req_rid(this, "i_req_rid", "16", NO_COMMENT),
    i_req_tag(this, "i_req_tag", "8", NO_COMMENT),
    i_req_be(this, "i_req_be", "8", NO_COMMENT),
    i_req_addr(this, "i_req_addr", "12", NO_COMMENT),
    _t3_(this, ""),
    o_rd_addr(this, "o_rd_addr", "11", NO_COMMENT),
    o_rd_be(this, "o_rd_be", "4", NO_COMMENT),
    i_rd_data(this, "i_rd_data", "32", NO_COMMENT),
    i_completer_id(this, "i_completer_id", "16", NO_COMMENT),
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

void pio_tx_engine::proc_comb() {
}

void pio_tx_engine::proc_reqff() {
}
