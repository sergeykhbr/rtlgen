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
    wb_rd_addr(this, "wb_rd_addr", "10", NO_COMMENT),
    wb_rd_be(this, "wb_rd_be", "4", NO_COMMENT),
    wb_rd_data(this, "wb_rd_data", "32", NO_COMMENT),
    wb_wr_addr(this, "wb_wr_addr", "11", NO_COMMENT),
    wb_wr_be(this, "wb_wr_be", "8", NO_COMMENT),
    wb_wr_data(this, "wb_wr_data", "32", NO_COMMENT),
    w_wr_en(this, "w_wr_en", "1", NO_COMMENT),
    w_wr_busy(this, "w_wr_busy", "1", NO_COMMENT),
    w_req_compl_int(this, "w_req_compl_int", "1", NO_COMMENT),
    w_req_compl_wd(this, "w_req_compl_wd", "1", NO_COMMENT),
    w_compl_done_int(this, "w_compl_done_int", "1", NO_COMMENT),
    wb_req_tc(this, "wb_req_tc", "3", NO_COMMENT),
    w_req_td(this, "w_req_td", "1", NO_COMMENT),
    w_req_ep(this, "w_req_ep", "1", NO_COMMENT),
    wb_req_attr(this, "wb_req_attr", "2", NO_COMMENT),
    wb_req_len(this, "wb_req_len", "10", NO_COMMENT),
    wb_req_rid(this, "wb_req_rid", "16", NO_COMMENT),
    wb_req_tag(this, "wb_req_tag", "8", NO_COMMENT),
    wb_req_be(this, "wb_req_be", "8", NO_COMMENT),
    wb_req_addr(this, "wb_req_addr", "13", NO_COMMENT),
    // registers
    //
    EP_MEM_inst(this, "EP_MEM_inst", NO_COMMENT),
    EP_RX_inst(this, "EP_RX_inst", NO_COMMENT),
    EP_TX_inst(this, "EP_TX_inst", NO_COMMENT),
    comb(this)
{
    Operation::start(this);

    NEW(EP_MEM_inst, EP_MEM_inst.getName().c_str());
      CONNECT(EP_MEM_inst, 0, EP_MEM_inst.i_nrst, i_nrst);
      CONNECT(EP_MEM_inst, 0, EP_MEM_inst.i_clk, i_clk);
      CONNECT(EP_MEM_inst, 0, EP_MEM_inst.i_rd_addr, wb_rd_addr);
      CONNECT(EP_MEM_inst, 0, EP_MEM_inst.i_rd_be, wb_rd_be);
      CONNECT(EP_MEM_inst, 0, EP_MEM_inst.o_rd_data, wb_rd_data);
      CONNECT(EP_MEM_inst, 0, EP_MEM_inst.i_wr_addr, wb_wr_addr);
      CONNECT(EP_MEM_inst, 0, EP_MEM_inst.i_wr_be, wb_wr_be);
      CONNECT(EP_MEM_inst, 0, EP_MEM_inst.i_wr_data, wb_wr_data);
      CONNECT(EP_MEM_inst, 0, EP_MEM_inst.i_wr_en, w_wr_en);
      CONNECT(EP_MEM_inst, 0, EP_MEM_inst.o_wr_busy, w_wr_busy);
    ENDNEW();

TEXT();
    EP_RX_inst.C_DATA_WIDTH.setObjValue(&C_DATA_WIDTH);
    EP_RX_inst.KEEP_WIDTH.setObjValue(&KEEP_WIDTH);
    NEW(EP_RX_inst, EP_RX_inst.getName().c_str());
        CONNECT(EP_RX_inst, 0, EP_RX_inst.i_nrst, i_nrst);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.i_clk, i_clk);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.i_m_axis_rx_tdata, i_m_axis_rx_tdata);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.i_m_axis_rx_tkeep, i_m_axis_rx_tkeep);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.i_m_axis_rx_tlast, i_m_axis_rx_tlast);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.i_m_axis_rx_tvalid, i_m_axis_rx_tvalid);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.o_m_axis_rx_tready, o_m_axis_rx_tready);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.i_m_axis_rx_tuser, i_m_axis_rx_tuser);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.o_req_compl, w_req_compl_int);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.o_req_compl_wd, w_req_compl_wd);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.i_compl_done, w_compl_done_int);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.o_req_tc, wb_req_tc);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.o_req_td, w_req_td);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.o_req_ep, w_req_ep);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.o_req_attr, wb_req_attr);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.o_req_len, wb_req_len);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.o_req_rid, wb_req_rid);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.o_req_tag, wb_req_tag);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.o_req_be, wb_req_be);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.o_req_addr, wb_req_addr);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.o_wr_addr, wb_wr_addr);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.o_wr_be, wb_wr_be);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.o_wr_data, wb_wr_data);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.o_wr_en, w_wr_en);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.i_wr_busy, w_wr_busy);
    ENDNEW();

TEXT();
    EP_TX_inst.C_DATA_WIDTH.setObjValue(&C_DATA_WIDTH);
    EP_TX_inst.KEEP_WIDTH.setObjValue(&KEEP_WIDTH);
    NEW(EP_TX_inst, EP_TX_inst.getName().c_str());
        CONNECT(EP_TX_inst, 0, EP_TX_inst.i_nrst, i_nrst);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.i_clk, i_clk);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.i_s_axis_tx_tready, i_s_axis_tx_tready);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.o_s_axis_tx_tdata, o_s_axis_tx_tdata);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.o_s_axis_tx_tkeep, o_s_axis_tx_tkeep);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.o_s_axis_tx_tlast, o_s_axis_tx_tlast);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.o_s_axis_tx_tvalid, o_s_axis_tx_tvalid);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.o_tx_src_dsc, o_tx_src_dsc);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.i_req_compl, w_req_compl_int);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.i_req_compl_wd, w_req_compl_wd);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.o_compl_done, w_compl_done_int);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.i_req_tc, wb_req_tc);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.i_req_td, w_req_td);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.i_req_ep, w_req_ep);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.i_req_attr, wb_req_attr);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.i_req_len, wb_req_len);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.i_req_rid, wb_req_rid);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.i_req_tag, wb_req_tag);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.i_req_be, wb_req_be);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.i_req_addr, wb_req_addr);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.o_rd_addr, wb_rd_addr);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.o_rd_be, wb_rd_be);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.i_rd_data, wb_rd_data);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.i_completer_id, i_cfg_completer_id);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void pio_ep::proc_comb() {
    ASSIGN(o_req_compl, w_req_compl_int);
    ASSIGN(o_compl_done, w_compl_done_int);
}
