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

#include "pio_ep.h"

pio_ep::pio_ep(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "PIO_EP", name, comment),
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
    i_m_axis_rx_tdata(this, "i_m_axis_rx_tdata", "C_DATA_WIDTH", NO_COMMENT),
    i_m_axis_rx_tkeep(this, "i_m_axis_rx_tkeep", "KEEP_WIDTH", NO_COMMENT),
    i_m_axis_rx_tlast(this, "i_m_axis_rx_tlast", "1", NO_COMMENT),
    i_m_axis_rx_tvalid(this, "i_m_axis_rx_tvalid", "1", NO_COMMENT),
    o_m_axis_rx_tready(this, "o_m_axis_rx_tready", "1", NO_COMMENT),
    i_m_axis_rx_tuser(this, "i_m_axis_rx_tuser", "22", NO_COMMENT),
    _t2_(this, ""),
    o_req_compl(this, "o_req_compl", "1", NO_COMMENT),
    o_compl_done(this, "o_compl_done", "1", NO_COMMENT),
    i_cfg_completer_id(this, "i_cfg_completer_id", "16", "Bus, Device, Function"),
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

void pio_ep::proc_comb() {
}

void pio_ep::proc_reqff() {
}
