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

#include "pcie_io.h"

pcie_io::pcie_io(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "pcie_io", name, comment),
    C_DATA_WIDTH(this, "C_DATA_WIDTH", "64", NO_COMMENT),
    KEEP_WIDTH(this, "KEEP_WIDTH", "DIV(C_DATA_WIDTH,8)", NO_COMMENT),
    i_clk(this, "i_clk", "1", "System bus clock"),
    i_user_clk(this, "i_user_clk", "1", "PCIE PHY clock"),
    i_user_reset(this, "i_user_reset", "1", NO_COMMENT),
    i_user_lnk_up(this, "i_user_lnk_up", "1", NO_COMMENT),
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
    i_cfg_to_turnoff(this, "i_cfg_to_turnoff", "1", NO_COMMENT),
    o_cfg_turnoff_ok(this, "o_cfg_turnoff_ok", "1", NO_COMMENT),
    i_cfg_completer_id(this, "i_cfg_completer_id", "16", "Bus, Device, Function"),
    // params
    _fifo0_(this, ""),
    REQ_FIFO_WIDTH(this, "REQ_FIFO_WIDTH", &CALCWIDTHx(4, &i_m_axis_rx_tuser,
                                                          &i_m_axis_rx_tlast,
                                                          &i_m_axis_rx_tkeep,
                                                          &i_m_axis_rx_tdata)),
    RESP_FIFO_WIDTH(this, "RESP_FIFO_WIDTH", &CALCWIDTHx(4, &o_tx_src_dsc,
                                                            &o_s_axis_tx_tlast,
                                                            &o_s_axis_tx_tkeep,
                                                            &o_s_axis_tx_tdata)),
    // signals
    wb_reqfifo_payload_i(this, "wb_reqfifo_payload_i", "REQ_FIFO_WIDTH"),
    wb_reqfifo_payload_o(this, "wb_reqfifo_payload_o", "REQ_FIFO_WIDTH"),
    w_m_axis_rx_tready(this, "w_m_axis_rx_tready", "1"),
    w_m_axis_rx_tvalid(this, "w_m_axis_rx_tvalid", "1"),
    wb_m_axis_rx_tdata(this, "wb_m_axis_rx_tdata", "C_DATA_WIDTH", NO_COMMENT),
    wb_m_axis_rx_tkeep(this, "wb_m_axis_rx_tkeep", "KEEP_WIDTH", NO_COMMENT),
    w_m_axis_rx_tlast(this, "w_m_axis_rx_tlast", "1", NO_COMMENT),
    wb_m_axis_rx_tuser(this, "wb_m_axis_rx_tuser", "22", NO_COMMENT),
    wb_respfifo_payload_i(this, "wb_respfifo_payload_i", "RESP_FIFO_WIDTH"),
    wb_respfifo_payload_o(this, "wb_respfifo_payload_o", "RESP_FIFO_WIDTH"),
    w_s_axis_tx_tready(this, "w_s_axis_tx_tready", "1"),
    w_s_axis_tx_tvalid(this, "w_s_axis_tx_tvalid", "1"),
    w_tx_src_dsc(this, "w_tx_src_dsc", "1", NO_COMMENT),
    w_s_axis_tx_tlast(this, "w_s_axis_tx_tlast", "1", NO_COMMENT),
    wb_s_axis_tx_tkeep(this, "wb_s_axis_tx_tkeep", "KEEP_WIDTH", NO_COMMENT),
    wb_s_axis_tx_tdata(this, "wb_s_axis_tx_tdata", "C_DATA_WIDTH", NO_COMMENT),
    _t4_(this, ""),
    w_req_compl(this, "w_req_compl", "1", NO_COMMENT),
    w_compl_done(this, "w_compl_done", "1", NO_COMMENT),
    r_trn_pending(this, "r_trn_pending", "1", RSTVAL_ZERO, "Transaction pending"),
    r_cfg_turnoff_ok(this, "r_cfg_turnoff_ok", "1", RSTVAL_ZERO, "Turn-off handshake"),
    // registers
    //
    comb(this),
    reqff(this, "reqff", &i_user_clk, CLK_POSEDGE, 0, ACTIVE_NONE, NO_COMMENT),
    reqfifo(this, "reqfifo"),
    respfifo(this, "respfifo"),
    PIO_EP_inst(this, "PIO_EP_inst", NO_COMMENT)
{
    Operation::start(this);

    TEXT("PCIE EP (200 MHz) -> DMA (40 MHz)");
    reqfifo.abits.setObjValue(SCV_get_cfg_type(this, "CFG_PCIE_DMAFIFO_DEPTH"));
    reqfifo.dbits.setObjValue(&REQ_FIFO_WIDTH);
    NEW(reqfifo, reqfifo.getName().c_str());
        CONNECT(reqfifo, 0, reqfifo.i_nrst, i_user_lnk_up);
        CONNECT(reqfifo, 0, reqfifo.i_wclk, i_user_clk);
        CONNECT(reqfifo, 0, reqfifo.i_wr, i_m_axis_rx_tvalid);
        CONNECT(reqfifo, 0, reqfifo.i_wdata, wb_reqfifo_payload_i);
        CONNECT(reqfifo, 0, reqfifo.o_wready, o_m_axis_rx_tready);
        CONNECT(reqfifo, 0, reqfifo.i_rclk, i_clk);
        CONNECT(reqfifo, 0, reqfifo.i_rd, w_m_axis_rx_tready);
        CONNECT(reqfifo, 0, reqfifo.o_rdata, wb_reqfifo_payload_o);
        CONNECT(reqfifo, 0, reqfifo.o_rvalid, w_m_axis_rx_tvalid);
    ENDNEW();

TEXT();
    TEXT("DMA (40 MHz) -> PCIE EP (200 MHz)");
    respfifo.abits.setObjValue(SCV_get_cfg_type(this, "CFG_PCIE_DMAFIFO_DEPTH"));
    respfifo.dbits.setObjValue(&RESP_FIFO_WIDTH);
    NEW(respfifo, respfifo.getName().c_str());
        CONNECT(respfifo, 0, respfifo.i_nrst, i_user_lnk_up);
        CONNECT(respfifo, 0, respfifo.i_wclk, i_clk);
        CONNECT(respfifo, 0, respfifo.i_wr, w_s_axis_tx_tvalid);
        CONNECT(respfifo, 0, respfifo.i_wdata, wb_respfifo_payload_i);
        CONNECT(respfifo, 0, respfifo.o_wready, w_s_axis_tx_tready);
        CONNECT(respfifo, 0, respfifo.i_rclk, i_user_clk);
        CONNECT(respfifo, 0, respfifo.i_rd, i_s_axis_tx_tready);
        CONNECT(respfifo, 0, respfifo.o_rdata, wb_respfifo_payload_o);
        CONNECT(respfifo, 0, respfifo.o_rvalid, o_s_axis_tx_tvalid);
    ENDNEW();

TEXT();
    PIO_EP_inst.C_DATA_WIDTH.setObjValue(&C_DATA_WIDTH);
    PIO_EP_inst.KEEP_WIDTH.setObjValue(&KEEP_WIDTH);
    NEW(PIO_EP_inst, PIO_EP_inst.getName().c_str());
        CONNECT(PIO_EP_inst, 0, PIO_EP_inst.i_nrst, i_user_lnk_up);
        CONNECT(PIO_EP_inst, 0, PIO_EP_inst.i_clk, i_clk);
        CONNECT(PIO_EP_inst, 0, PIO_EP_inst.i_s_axis_tx_tready, w_s_axis_tx_tready);
        CONNECT(PIO_EP_inst, 0, PIO_EP_inst.o_s_axis_tx_tdata, wb_s_axis_tx_tdata);
        CONNECT(PIO_EP_inst, 0, PIO_EP_inst.o_s_axis_tx_tkeep, wb_s_axis_tx_tkeep);
        CONNECT(PIO_EP_inst, 0, PIO_EP_inst.o_s_axis_tx_tlast, w_s_axis_tx_tlast);
        CONNECT(PIO_EP_inst, 0, PIO_EP_inst.o_s_axis_tx_tvalid, w_s_axis_tx_tvalid);
        CONNECT(PIO_EP_inst, 0, PIO_EP_inst.o_tx_src_dsc, w_tx_src_dsc);
        CONNECT(PIO_EP_inst, 0, PIO_EP_inst.i_m_axis_rx_tdata, wb_m_axis_rx_tdata);
        CONNECT(PIO_EP_inst, 0, PIO_EP_inst.i_m_axis_rx_tkeep, wb_m_axis_rx_tkeep);
        CONNECT(PIO_EP_inst, 0, PIO_EP_inst.i_m_axis_rx_tlast, w_m_axis_rx_tlast);
        CONNECT(PIO_EP_inst, 0, PIO_EP_inst.i_m_axis_rx_tvalid, w_m_axis_rx_tvalid);
        CONNECT(PIO_EP_inst, 0, PIO_EP_inst.o_m_axis_rx_tready, w_m_axis_rx_tready);
        CONNECT(PIO_EP_inst, 0, PIO_EP_inst.i_m_axis_rx_tuser, wb_m_axis_rx_tuser);
        CONNECT(PIO_EP_inst, 0, PIO_EP_inst.o_req_compl, w_req_compl);
        CONNECT(PIO_EP_inst, 0, PIO_EP_inst.o_compl_done, w_compl_done);
        CONNECT(PIO_EP_inst, 0, PIO_EP_inst.i_cfg_completer_id, i_cfg_completer_id);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();

    Operation::start(&reqff);
    proc_reqff();
}

void pcie_io::proc_comb() {
    TEXT_ASSIGN("PCIE PHY clock to system clock AFIFO:");
    ASSIGN(wb_reqfifo_payload_i, CCx(4, &i_m_axis_rx_tuser,
                                        &i_m_axis_rx_tlast,
                                        &i_m_axis_rx_tkeep,
                                        &i_m_axis_rx_tdata));

TEXT_ASSIGN();
    SPLx_ASSIGN(wb_reqfifo_payload_o, 4, &wb_m_axis_rx_tuser,
                                         &w_m_axis_rx_tlast,
                                         &wb_m_axis_rx_tkeep,
                                         &wb_m_axis_rx_tdata);

TEXT_ASSIGN();
    TEXT_ASSIGN("System Clock to PCIE PHY clock AFIFO:");
    ASSIGN(wb_respfifo_payload_i, CCx(4, &w_tx_src_dsc,
                                         &w_s_axis_tx_tlast,
                                         &wb_s_axis_tx_tkeep,
                                         &wb_s_axis_tx_tdata));

TEXT_ASSIGN();
    SPLx_ASSIGN(wb_respfifo_payload_o, 4, &o_tx_src_dsc,
                                          &o_s_axis_tx_tlast,
                                          &o_s_axis_tx_tkeep,
                                          &o_s_axis_tx_tdata);

TEXT_ASSIGN();
    ASSIGN(o_cfg_turnoff_ok, r_cfg_turnoff_ok);
}

void pcie_io::proc_reqff() {
    IF (EZ(i_user_lnk_up));
        SETZERO(r_trn_pending);
        SETZERO(r_cfg_turnoff_ok);
    ELSE();
        IF (AND2(EZ(r_trn_pending), NZ(w_req_compl)));
            SETONE(r_trn_pending);
        ELSIF(NZ(w_compl_done));
            SETZERO(r_trn_pending);
        ENDIF();

TEXT();
        IF (AND2(EZ(r_trn_pending), NZ(i_cfg_to_turnoff)));
            SETONE(r_cfg_turnoff_ok);
        ELSE();
            SETZERO(r_cfg_turnoff_ok);
        ENDIF();
    ENDIF();
}
