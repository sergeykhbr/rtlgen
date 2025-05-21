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

#include "pcie_io_ep.h"

pcie_io_ep::pcie_io_ep(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "pcie_io_ep", name, comment),
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
    i_m_axis_rx_tuser(this, "i_m_axis_rx_tuser", "9", NO_COMMENT),
    i_cfg_completer_id(this, "i_cfg_completer_id", "16", "Bus, Device, Function"),
    _t3_(this, "Memory access signals:"),
    i_req_mem_ready(this, "i_req_mem_ready", "1", "Ready to accept next memory request"),
    o_req_mem_valid(this, "o_req_mem_valid", "1", "Request data is valid to accept"),
    o_req_mem_write(this, "o_req_mem_write", "1", "0=read; 1=write operation"),
    o_req_mem_bytes(this, "o_req_mem_bytes", "10", "0=1024 B; 4=DWORD; 8=QWORD; ..."),
    o_req_mem_addr(this, "o_req_mem_addr", "CFG_PCIE_DMAADDR_WIDTH", "Address to read/write"),
    o_req_mem_strob(this, "o_req_mem_strob", "8", "Byte enabling write strob"),
    o_req_mem_data(this, "o_req_mem_data", "64", "Data to write"),
    o_req_mem_last(this, "o_req_mem_last", "1", "Last data payload in a sequence"),
    i_resp_mem_valid(this, "i_resp_mem_valid", "1", "Read/Write data is valid. All write transaction with valid response."),
    i_resp_mem_last(this, "i_resp_mem_last", "1", "Last response in sequence"),
    i_resp_mem_fault(this, "i_resp_mem_fault", "1", "Error on memory access"),
    i_resp_mem_addr(this, "i_resp_mem_addr", "CFG_PCIE_DMAADDR_WIDTH", "Read address value"),
    i_resp_mem_data(this, "i_resp_mem_data", "64", "Read data value"),
    o_resp_mem_ready(this, "o_resp_mem_ready", "1", "Ready to accept response"),
    _f0_(this),
    SwapEndianess32(this),
    // params
    // signals
    w_tx_ena(this, "w_tx_ena", "1", NO_COMMENT),
    w_tx_completion(this, "w_tx_completion", "1", NO_COMMENT),
    w_tx_with_data(this, "w_tx_with_data", "1", NO_COMMENT),
    w_compl_done_int(this, "w_compl_done_int", "1", NO_COMMENT),
    wb_req_tc(this, "wb_req_tc", "3", NO_COMMENT),
    w_req_td(this, "w_req_td", "1", NO_COMMENT),
    w_req_ep(this, "w_req_ep", "1", NO_COMMENT),
    wb_req_attr(this, "wb_req_attr", "2", NO_COMMENT),
    wb_req_len(this, "wb_req_len", "10", NO_COMMENT),
    wb_req_rid(this, "wb_req_rid", "16", NO_COMMENT),
    wb_req_tag(this, "wb_req_tag", "8", NO_COMMENT),
    wb_req_be(this, "wb_req_be", "8", NO_COMMENT),
    wb_req_addr(this, "wb_req_addr", "CFG_PCIE_DMAADDR_WIDTH", NO_COMMENT),
    wb_req_bytes(this, "wb_req_bytes", "10", NO_COMMENT),
    wb_req_mem_data(this, "wb_req_mem_data", "C_DATA_WIDTH", NO_COMMENT),
    wb_resp_mem_data(this, "wb_resp_mem_data", "C_DATA_WIDTH", NO_COMMENT),
    // registers
    //
    EP_RX_inst(this, "EP_RX_inst", NO_COMMENT),
    EP_TX_inst(this, "EP_TX_inst", NO_COMMENT),
    comb(this)
{
    Operation::start(this);

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
        CONNECT(EP_RX_inst, 0, EP_RX_inst.o_tx_ena, w_tx_ena);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.o_tx_completion, w_tx_completion);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.o_tx_with_data, w_tx_with_data);
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
        CONNECT(EP_RX_inst, 0, EP_RX_inst.i_req_mem_ready, i_req_mem_ready);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.o_req_mem_valid, o_req_mem_valid);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.o_req_mem_write, o_req_mem_write);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.o_req_mem_bytes, wb_req_bytes);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.o_req_mem_addr, o_req_mem_addr);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.o_req_mem_strob, o_req_mem_strob);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.o_req_mem_data, wb_req_mem_data);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.o_req_mem_last, o_req_mem_last);
        CONNECT(EP_RX_inst, 0, EP_RX_inst.i_resp_mem_valid, i_resp_mem_valid);
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
        CONNECT(EP_TX_inst, 0, EP_TX_inst.i_tx_ena, w_tx_ena);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.i_tx_completion, w_tx_completion);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.i_tx_with_data, w_tx_with_data);
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
        CONNECT(EP_TX_inst, 0, EP_TX_inst.i_req_bytes, wb_req_bytes);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.i_dma_resp_valid, i_resp_mem_valid);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.i_dma_resp_last, i_resp_mem_last);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.i_dma_resp_fault, i_resp_mem_fault);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.i_dma_resp_addr, i_resp_mem_addr);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.i_dma_resp_data, wb_resp_mem_data);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.o_dma_resp_ready, o_resp_mem_ready);
        CONNECT(EP_TX_inst, 0, EP_TX_inst.i_completer_id, i_cfg_completer_id);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

pcie_io_ep::SwapEndianess32_func::SwapEndianess32_func(GenObject *parent)
    : FunctionObject(parent, "SwapEndianess32"),
    ret(this, "ret", "64"),
    dword(this, "dword", "64") {
    SETBITS(ret, 31, 0, CCx(4, &BITS(dword, 7, 0),
                               &BITS(dword, 15, 8),
                               &BITS(dword, 23, 16),
                               &BITS(dword, 31, 24)));
    SETBITS(ret, 63, 32, CCx(4, &BITS(dword, 39, 32),
                               &BITS(dword, 47, 40),
                               &BITS(dword, 55, 48),
                               &BITS(dword, 63, 56)));
}

void pcie_io_ep::proc_comb() {
TEXT();
    TEXT("Correct PCIe endieness:");
    CALLF(&o_req_mem_data, SwapEndianess32, 1, &wb_req_mem_data);
    CALLF(&wb_resp_mem_data, SwapEndianess32, 1, &i_resp_mem_data);

    ASSIGN(o_req_mem_bytes, wb_req_bytes);
}
