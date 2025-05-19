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

#include "pcie_io_tx_engine.h"

pcie_io_tx_engine::pcie_io_tx_engine(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "pcie_io_tx_engine", name, comment),
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
    i_req_addr(this, "i_req_addr", "13", NO_COMMENT),
    i_req_bytes(this, "i_req_bytes", "10", NO_COMMENT),
    _t3_(this, ""),
    i_dma_resp_valid(this, "i_dma_resp_valid", "1", NO_COMMENT),
    i_dma_resp_last(this, "i_dma_resp_last", "1", NO_COMMENT),
    i_dma_resp_fault(this, "i_dma_resp_fault", "1", "Error on memory access"),
    i_dma_resp_addr(this, "i_dma_resp_addr", "13", NO_COMMENT),
    i_dma_resp_data(this, "i_dma_resp_data", "64", NO_COMMENT),
    o_dma_resp_ready(this, "o_dma_resp_ready", "1", "Ready to accept response"),
    i_completer_id(this, "i_completer_id", "16", NO_COMMENT),
    // params
    _fmt0_(this, ""),
    PIO_CPLD_FMT_TYPE(this, "PIO_CPLD_FMT_TYPE", "7", "0x4A", NO_COMMENT),
    PIO_CPL_FMT_TYPE(this, "PIO_CPL_FMT_TYPE", "7", "0x0A", NO_COMMENT),
    PIO_TX_RST_STATE(this, "PIO_TX_RST_STATE", "2", "0x0", NO_COMMENT),
    PIO_TX_WAIT_DMA_RESP(this, "PIO_TX_WAIT_DMA_RESP", "4", "0x1", NO_COMMENT),
    PIO_TX_CPLD_QW1(this, "PIO_TX_CPLD_QW1", "4", "0x2", NO_COMMENT),
    PIO_TX_RD_FIRST(this, "PIO_TX_RD_FIRST", "4", "0x4", NO_COMMENT),
    PIO_TX_RD_BURST(this, "PIO_TX_RD_BURST", "4", "0x8", NO_COMMENT),
    // signals
    // registers
    s_axis_tx_tdata(this, "s_axis_tx_tdata", "C_DATA_WIDTH", "'0", NO_COMMENT),
    s_axis_tx_tkeep(this, "s_axis_tx_tkeep", "KEEP_WIDTH", "'0", NO_COMMENT),
    s_axis_tx_tlast(this, "s_axis_tx_tlast", "1", RSTVAL_ZERO, NO_COMMENT),
    s_axis_tx_tvalid(this, "s_axis_tx_tvalid", "1", RSTVAL_ZERO, NO_COMMENT),
    dma_resp_ready(this, "dma_resp_ready", "1", RSTVAL_ZERO, NO_COMMENT),
    compl_done(this, "compl_done", "1", RSTVAL_ZERO, NO_COMMENT),
    req_compl_wd_q(this, "req_compl_wd_q", "1", "1", NO_COMMENT),
    req_addr(this, "req_addr", "13", "'0", NO_COMMENT),
    req_rid(this, "req_rid", "16", NO_COMMENT),
    req_tag(this, "req_tag", "8", NO_COMMENT),
    req_be(this, "req_be", "4", "'0", NO_COMMENT),
    rd_data(this, "rd_data", "64", "'0", NO_COMMENT),
    rd_addr(this, "rd_addr", "13", "'0", NO_COMMENT),
    rd_last(this, "rd_last", "1", "'0", NO_COMMENT),
    state(this, "state", "4", "PIO_TX_RST_STATE", NO_COMMENT),
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

void pcie_io_tx_engine::proc_comb() {
    TEXT("The completer field 'lower address' DWORD[2][6:0]:");
    TEXT("For completions other than for memory reads, this value is set to 0.");
    TEXT("For memory reads it is the lower byte address of the first byte in");
    TEXT("the returned data (or partial data). This is set for the first");
    TEXT("(or only) completion and will be 0 in the lower 7 bits from then on,");
    TEXT("as the completions, if split, must be naturally aligned to a read");
    TEXT("completion boundary (RCB), which is usually 128 bytes");
    TEXT("(though 64 bytes in root complex).");
    IF (EZ(req_compl_wd_q));
        TEXT("Request without payload");
        SETZERO(comb.vb_lower_addr);
    ELSE();
        SETVAL(comb.vb_lower_addr, BITS(req_addr, 6, 0));
    ENDIF();

TEXT();
    IF (NZ(i_s_axis_tx_tready));
        SETZERO(s_axis_tx_tvalid);
        SETZERO(s_axis_tx_tlast);
    ENDIF();
    IF (NZ(i_dma_resp_valid));
        SETZERO(dma_resp_ready);
    ENDIF();
    SETZERO(compl_done);

TEXT();
    SWITCH(state);
    CASE (PIO_TX_RST_STATE);
        SETZERO(s_axis_tx_tdata);
        SETZERO(s_axis_tx_tkeep);
        IF (NZ(i_req_compl));
            SETVAL(req_addr, i_req_addr);
            SETVAL(req_rid, i_req_rid);
            SETVAL(req_tag, i_req_tag);
            SETVAL(req_be, i_req_be);
            SETVAL(req_compl_wd_q, i_req_compl_wd);
            SETBITS(comb.vb_s_axis_tx_tdata, 63, 48, i_completer_id);
            SETBITS(comb.vb_s_axis_tx_tdata, 47, 45, CONST("0", 3));
            SETBIT(comb.vb_s_axis_tx_tdata, 44, CONST("0", 1));
            SETBITS(comb.vb_s_axis_tx_tdata, 43, 32, i_req_bytes);
            SETBIT(comb.vb_s_axis_tx_tdata, 31, CONST("0", 1));
            IF (NZ(req_compl_wd_q));
                SETBITS(comb.vb_s_axis_tx_tdata, 30, 24, PIO_CPLD_FMT_TYPE);
            ELSE();
                SETBITS(comb.vb_s_axis_tx_tdata, 30, 24, PIO_CPL_FMT_TYPE);
            ENDIF();
            SETBIT(comb.vb_s_axis_tx_tdata, 23, CONST("0", 1));
            SETBITS(comb.vb_s_axis_tx_tdata, 22, 20, i_req_tc);
            SETBITS(comb.vb_s_axis_tx_tdata, 19, 16, CONST("0", 3));
            SETBIT(comb.vb_s_axis_tx_tdata, 15, i_req_td);
            SETBIT(comb.vb_s_axis_tx_tdata, 14, i_req_ep);
            SETBITS(comb.vb_s_axis_tx_tdata, 13, 12, i_req_attr);
            SETBITS(comb.vb_s_axis_tx_tdata, 11, 10, CONST("0", 2));
            SETBITS(comb.vb_s_axis_tx_tdata, 9, 0, i_req_len);
            SETVAL(s_axis_tx_tdata, comb.vb_s_axis_tx_tdata);
            SETVAL(s_axis_tx_tkeep, CONST("0xFF", 8));
            IF (NZ(i_req_compl_wd));
                TEXT("Send this TLP qword only on DMA response");
                SETONE(dma_resp_ready);
                SETVAL(state, PIO_TX_WAIT_DMA_RESP);
            ELSE();
                TEXT("Send now");
                SETONE(s_axis_tx_tvalid);
                SETVAL(state, PIO_TX_CPLD_QW1);
            ENDIF();
        ENDIF();
    ENDCASE();

    TEXT();
    CASE (PIO_TX_WAIT_DMA_RESP);
        IF (NZ(i_dma_resp_valid));
            SETONE(s_axis_tx_tvalid, "Transmit DW1DW2");
            SETVAL(rd_data, i_dma_resp_data);
            SETVAL(rd_addr, i_dma_resp_addr);
            SETVAL(rd_last, i_dma_resp_last);
            SETVAL(state, PIO_TX_CPLD_QW1);
        ENDIF();
    ENDCASE();

    TEXT();
    CASE (PIO_TX_CPLD_QW1);
        IF (NZ(i_s_axis_tx_tready));
            SETONE(s_axis_tx_tvalid);
            SETVAL(s_axis_tx_tlast, rd_last);
            TEXT("Swap DWORDS for AXI");
            IF (NZ(BIT(req_addr, 2)));
                SETBITS(comb.vb_s_axis_tx_tdata, 63, 32, BITS(rd_data, 63, 32));
            ELSE();
                SETBITS(comb.vb_s_axis_tx_tdata, 63, 32, BITS(rd_data, 31, 0));
            ENDIF();
            SETBITS(comb.vb_s_axis_tx_tdata, 31, 16, req_rid);
            SETBITS(comb.vb_s_axis_tx_tdata, 15, 8, req_tag);
            SETBIT(comb.vb_s_axis_tx_tdata, 7, CONST("0", 1));
            SETBITS(comb.vb_s_axis_tx_tdata, 6, 0, comb.vb_lower_addr);
            SETVAL(s_axis_tx_tdata, comb.vb_s_axis_tx_tdata);

            TEXT();
            TEXT("Mask data strob if data no need:");
            IF (AND2(NZ(req_compl_wd_q), NZ(rd_last)));
                SETVAL(s_axis_tx_tkeep, CONST("0xFF", 8));
            ELSE();
                SETVAL(s_axis_tx_tkeep, CONST("0x0F", 8));
            ENDIF();
            SETVAL(compl_done, rd_last);
            SETZERO(rd_last);
            IF (NZ(rd_last));
                SETVAL(state, PIO_TX_RST_STATE);
            ELSE();
                SETVAL(state, PIO_TX_RD_FIRST);
            ENDIF();
        ENDIF();
    ENDCASE();

    TEXT();
    CASE(PIO_TX_RD_BURST);
        IF (NZ(i_s_axis_tx_tready));
            SETONE(s_axis_tx_tvalid);
            SETVAL(s_axis_tx_tdata, rd_data);
            SETVAL(s_axis_tx_tkeep, CONST("0xFF", 8));
            SETVAL(state, PIO_TX_RD_BURST);
        ENDIF();
    ENDCASE();

    TEXT();
    CASE(PIO_TX_RD_BURST);
        SETVAL(dma_resp_ready, i_s_axis_tx_tready);
        IF (AND3(NZ(i_s_axis_tx_tready), NZ(i_dma_resp_valid), NZ(dma_resp_ready)));
            SETONE(s_axis_tx_tvalid);
            SETVAL(s_axis_tx_tlast, i_dma_resp_last);
            SETVAL(s_axis_tx_tdata, i_dma_resp_data);
            SETVAL(compl_done, i_dma_resp_last);
            SETVAL(dma_resp_ready, INV_L(i_dma_resp_last));
            IF (NZ(i_dma_resp_last));
                SETZERO(dma_resp_ready);
                SETVAL(state, PIO_TX_RST_STATE);
            ENDIF();
        ENDIF();
    ENDCASE();

    TEXT();
    CASEDEF();
        SETVAL(state, PIO_TX_RST_STATE);
    ENDCASE();
    ENDSWITCH();


TEXT_ASSIGN();
    ASSIGN(o_s_axis_tx_tdata, s_axis_tx_tdata);
    ASSIGN(o_s_axis_tx_tkeep, s_axis_tx_tkeep);
    ASSIGN(o_s_axis_tx_tlast, s_axis_tx_tlast);
    ASSIGN(o_s_axis_tx_tvalid, s_axis_tx_tvalid);
    ASSIGN(o_dma_resp_ready, dma_resp_ready);
    ASSIGN(o_compl_done, compl_done);

TEXT_ASSIGN();
    TEXT_ASSIGN("Unused discontinue");
    ASSIGN(o_tx_src_dsc, CONST("0", 1));
}

void pcie_io_tx_engine::proc_reqff() {
}
