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
    _t3_(this, ""),
    o_rd_addr(this, "o_rd_addr", "11", NO_COMMENT),
    o_rd_be(this, "o_rd_be", "4", NO_COMMENT),
    i_rd_data(this, "i_rd_data", "32", NO_COMMENT),
    i_completer_id(this, "i_completer_id", "16", NO_COMMENT),
    // params
    _fmt0_(this, ""),
    PIO_CPLD_FMT_TYPE(this, "PIO_CPLD_FMT_TYPE", "7", "0x4A", NO_COMMENT),
    PIO_CPL_FMT_TYPE(this, "PIO_CPL_FMT_TYPE", "7", "0x0A", NO_COMMENT),
    PIO_TX_RST_STATE(this, "PIO_TX_RST_STATE", "2", "0x0", NO_COMMENT),
    PIO_TX_CPLD_QW1_FIRST(this, "PIO_TX_CPLD_QW1_FIRST", "2", "0x1", NO_COMMENT),
    PIO_TX_CPLD_QW1_TEMP(this, "PIO_TX_CPLD_QW1_TEMP", "2", "0x2", NO_COMMENT),
    PIO_TX_CPLD_QW1(this, "PIO_TX_CPLD_QW1", "2", "0x3", NO_COMMENT),
    // signals
    // registers
    s_axis_tx_tdata(this, "s_axis_tx_tdata", "C_DATA_WIDTH", "'0", NO_COMMENT),
    s_axis_tx_tkeep(this, "s_axis_tx_tkeep", "KEEP_WIDTH", "'0", NO_COMMENT),
    s_axis_tx_tlast(this, "s_axis_tx_tlast", "1", RSTVAL_ZERO, NO_COMMENT),
    s_axis_tx_tvalid(this, "s_axis_tx_tvalid", "1", RSTVAL_ZERO, NO_COMMENT),
    compl_done(this, "compl_done", "1", RSTVAL_ZERO, NO_COMMENT),
    rd_be(this, "rd_be", "4", "'0", NO_COMMENT),
    req_compl_q(this, "req_compl_q", "1", RSTVAL_ZERO, NO_COMMENT),
    req_compl_wd_q(this, "req_compl_wd_q", "1", "1", NO_COMMENT),
    compl_busy_i(this, "compl_busy_i", "1", RSTVAL_ZERO, NO_COMMENT),
    state(this, "state", "2", "PIO_TX_RST_STATE", NO_COMMENT),
    w_compl_wd(this, "w_compl_wd", "1", RSTVAL_ZERO, NO_COMMENT),
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
    SETVAL(rd_be, i_req_be);
    TEXT("Calculate byte count based on byte enable");
    SETVAL(comb.vb_add_be20, ADD2(CC2(CONST("0", 1), BIT(rd_be, 3)), CC2(CONST("0", 1), BIT(rd_be, 2))));
    SETVAL(comb.vb_add_be21, ADD2(CC2(CONST("0", 1), BIT(rd_be, 1)), CC2(CONST("0", 1), BIT(rd_be, 0))));
    SETVAL(comb.vb_byte_count, ADD2(CC2(CONST("0", 2), comb.vb_add_be20), CC2(CONST("0", 2), comb.vb_add_be21)));

TEXT();
    SETVAL(req_compl_q, i_req_compl);
    SETVAL(req_compl_wd_q, i_req_compl_wd);

TEXT();
    IF (EZ(w_compl_wd));
        TEXT("Request without payload");
        SETZERO(comb.vb_lower_addr);
    ELSIF (NZ(BIT(rd_be, 0)));
        SETZERO(comb.vb_lower_addr);
    ELSIF (NZ(BIT(rd_be, 1)));
        SETVAL(comb.vb_lower_addr, CC2(BITS(i_req_addr, 6, 2), CONST("1", 2)));
    ELSIF (NZ(BIT(rd_be, 2)));
        SETVAL(comb.vb_lower_addr, CC2(BITS(i_req_addr, 6, 2), CONST("2", 2)));
    ELSIF (NZ(BIT(rd_be, 3)));
        SETVAL(comb.vb_lower_addr, CC2(BITS(i_req_addr, 6, 2), CONST("3", 2)));
    ENDIF();

TEXT();
    SETZERO(compl_done);
    IF (NZ(req_compl_q));
        SETONE(compl_busy_i);
    ENDIF();
    SWITCH(state);
    CASE (PIO_TX_RST_STATE);
        IF (NZ(compl_busy_i));
            SETZERO(s_axis_tx_tdata);
            SETVAL(s_axis_tx_tkeep, CONST("0xFF", 8));
            SETZERO(s_axis_tx_tlast);
            SETZERO(s_axis_tx_tvalid);
            IF (NZ(i_s_axis_tx_tready));
                SETVAL(state, PIO_TX_CPLD_QW1_FIRST);
            ELSE();
                SETVAL(state, PIO_TX_RST_STATE);
            ENDIF();
        ELSE();
            SETZERO(s_axis_tx_tlast);
            SETZERO(s_axis_tx_tvalid);
            SETZERO(s_axis_tx_tdata);
            SETVAL(s_axis_tx_tkeep, CONST("0xFF", 8));
            SETZERO(compl_done);
            SETVAL(state, PIO_TX_RST_STATE);
        ENDIF();
    ENDCASE();

    TEXT();
    CASE (PIO_TX_CPLD_QW1_FIRST);
        IF (NZ(i_s_axis_tx_tready));
            SETZERO(s_axis_tx_tlast);
            SETBITS(comb.vb_s_axis_tx_tdata, 63, 48, i_completer_id);
            SETBITS(comb.vb_s_axis_tx_tdata, 47, 45, CONST("0", 3));
            SETBIT(comb.vb_s_axis_tx_tdata, 44, CONST("0", 1));
            SETBITS(comb.vb_s_axis_tx_tdata, 43, 32, comb.vb_byte_count);
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
            SETVAL(state, PIO_TX_CPLD_QW1_TEMP);
        ELSE();
            SETVAL(state, PIO_TX_RST_STATE);
        ENDIF();
    ENDCASE();

    TEXT();
    CASE (PIO_TX_CPLD_QW1_TEMP);
        SETONE(s_axis_tx_tvalid);
        SETVAL(state, PIO_TX_CPLD_QW1);
    ENDCASE();

    TEXT();
    CASE (PIO_TX_CPLD_QW1);
        IF (NZ(i_s_axis_tx_tready));
            SETONE(s_axis_tx_tlast);
            SETONE(s_axis_tx_tvalid);
            TEXT("Swap DWORDS for AXI");
            SETBITS(comb.vb_s_axis_tx_tdata, 63, 32, i_rd_data);
            SETBITS(comb.vb_s_axis_tx_tdata, 31, 16, i_req_rid);
            SETBITS(comb.vb_s_axis_tx_tdata, 15, 8, i_req_tag);
            SETBIT(comb.vb_s_axis_tx_tdata, 7, CONST("0", 1));
            SETBITS(comb.vb_s_axis_tx_tdata, 6, 0, comb.vb_lower_addr);
            SETVAL(s_axis_tx_tdata, comb.vb_s_axis_tx_tdata);

            TEXT();
            TEXT("Mask data strob if data no need:");
            IF (NZ(req_compl_wd_q));
                SETVAL(s_axis_tx_tkeep, CONST("0xFF", 8));
            ELSE();
                SETVAL(s_axis_tx_tkeep, CONST("0x0F", 8));
            ENDIF();
            SETONE(compl_done);
            SETZERO(compl_busy_i);
            SETVAL(state, PIO_TX_RST_STATE);
        ELSE();
            SETVAL(state, PIO_TX_CPLD_QW1);
        ENDIF();
    ENDCASE();
    
    TEXT();
    CASEDEF();
        SETVAL(state, PIO_TX_RST_STATE);
    ENDCASE();
    ENDSWITCH();


TEXT_ASSIGN();
    ASSIGN(w_compl_wd, req_compl_wd_q);
    ASSIGN(o_s_axis_tx_tdata, s_axis_tx_tdata);
    ASSIGN(o_s_axis_tx_tkeep, s_axis_tx_tkeep);
    ASSIGN(o_s_axis_tx_tlast, s_axis_tx_tlast);
    ASSIGN(o_s_axis_tx_tvalid, s_axis_tx_tvalid);
    ASSIGN(o_compl_done, compl_done);
    ASSIGN(o_rd_be, rd_be);

TEXT_ASSIGN();
    TEXT_ASSIGN("Unused discontinue");
    ASSIGN(o_tx_src_dsc, CONST("0", 1));

TEXT_ASSIGN();
    TEXT_ASSIGN("Present address and byte enable to memory module");
    ASSIGN(o_rd_addr, BITS(i_req_addr, 12, 2));
}

void pcie_io_tx_engine::proc_reqff() {
}
