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

#include "pcie_io_rx_engine.h"

pcie_io_rx_engine::pcie_io_rx_engine(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "pcie_io_rx_engine", name, comment),
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
    i_m_axis_rx_tuser(this, "i_m_axis_rx_tuser", "9", NO_COMMENT),
    _t1_(this, ""),
    o_tx_ena(this, "o_tx_ena", "1", "Trigger Tx engine to wait response from xDMA"),
    o_tx_completion(this, "o_tx_completion", "1", "Transmit completion TLP"),
    o_tx_with_data(this, "o_tx_with_data", "1", "Transmit TLP with payload"),
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
    o_req_addr(this, "o_req_addr", "CFG_PCIE_DMAADDR_WIDTH", "Memory Read Address"),
    _t33_(this, ""),
    i_req_mem_ready(this, "i_req_mem_ready", "1", "Ready to accept next memory request"),
    o_req_mem_valid(this, "o_req_mem_valid", "1", "Request data is valid to accept"),
    o_req_mem_write(this, "o_req_mem_write", "1", "0=read; 1=write operation"),
    o_req_mem_bytes(this, "o_req_mem_bytes", "12", "0=4095 B; 4=DWORD; 8=QWORD; ..."),
    o_req_mem_addr(this, "o_req_mem_addr", "CFG_PCIE_DMAADDR_WIDTH", "Address to read/write"),
    o_req_mem_strob(this, "o_req_mem_strob", "8", "Byte enabling write strob"),
    o_req_mem_data(this, "o_req_mem_data", "64", "Data to write"),
    o_req_mem_last(this, "o_req_mem_last", "1", "Last data payload in a sequence"),
    i_resp_mem_valid(this, "i_resp_mem_valid", "1", "Read/Write data is valid. All write transaction with valid response."),
    // params
    _t4_(this),
    PIO_RX_MEM_RD32_FMT_TYPE(this, "PIO_RX_MEM_RD32_FMT_TYPE", "7", "0x00", NO_COMMENT),
    PIO_RX_MEM_WR32_FMT_TYPE(this, "PIO_RX_MEM_WR32_FMT_TYPE", "7", "0x40", NO_COMMENT),
    PIO_RX_MEM_RD64_FMT_TYPE(this, "PIO_RX_MEM_RD64_FMT_TYPE", "7", "0x20", NO_COMMENT),
    PIO_RX_MEM_WR64_FMT_TYPE(this, "PIO_RX_MEM_WR64_FMT_TYPE", "7", "0x60", NO_COMMENT),
    PIO_RX_IO_RD32_FMT_TYPE(this, "PIO_RX_IO_RD32_FMT_TYPE", "7", "0x02", NO_COMMENT),
    PIO_RX_IO_WR32_FMT_TYPE(this, "PIO_RX_IO_WR32_FMT_TYPE", "7", "0x42", NO_COMMENT),
    _t5_(this),
    PIO_RX_RST_STATE(this, "PIO_RX_RST_STATE", "7", "0x00", NO_COMMENT),
    PIO_RX_MEM_RD32_DW1DW2(this, "PIO_RX_MEM_RD32_DW1DW2", "7", "0x01", NO_COMMENT),
    PIO_RX_MEM_WR32_DW1DW2(this, "PIO_RX_MEM_WR32_DW1DW2", "7", "0x02", NO_COMMENT),
    PIO_RX_MEM_RD64_DW1DW2(this, "PIO_RX_MEM_RD64_DW1DW2", "7", "0x04", NO_COMMENT),
    PIO_RX_MEM_WR64_DW1DW2(this, "PIO_RX_MEM_WR64_DW1DW2", "7", "0x08", NO_COMMENT),
    PIO_RX_MEM_WR_BURST(this, "PIO_RX_MEM_WR_BURST", "7", "0x10", NO_COMMENT),
    PIO_RX_IO_WR_DW1DW2(this, "PIO_RX_IO_WR_DW1DW2", "7", "0x20", "Burst disabled"),
    PIO_RX_WAIT_TX_COMPLETION(this, "PIO_RX_WAIT_TX_COMPLETION", "7", "0x40", NO_COMMENT),
    _tlp_resp0_(this, "TLP Response Types:"),
    TLP_NON_POSTED(this, "TLP_NON_POSTED", "3", "0x1", "No response at all"),
    TLP_COMPLETION(this, "TLP_COMPLETION", "3", "0x3", "Response without payload"),
    TLP_POSTED(this, "TLP_POSTED", "3", "0x5", "Response with data payload"),
    // signals
    // registers
    m_axis_rx_tready(this, "m_axis_rx_tready", "1", RSTVAL_ZERO, NO_COMMENT),
    req_valid(this, "req_valid", "1", RSTVAL_ZERO, NO_COMMENT),
    req_last(this, "req_last", "1", RSTVAL_ZERO, NO_COMMENT),
    req_tc(this, "req_tc", "3", RSTVAL_ZERO, NO_COMMENT),
    req_td(this, "req_td", "1", RSTVAL_ZERO, NO_COMMENT),
    req_ep(this, "req_ep", "1", RSTVAL_ZERO, NO_COMMENT),
    req_attr(this, "req_attr", "2", RSTVAL_ZERO, NO_COMMENT),
    req_len(this, "req_len", "10", RSTVAL_ZERO, NO_COMMENT),
    req_rid(this, "req_rid", "16", RSTVAL_ZERO, NO_COMMENT),
    req_tag(this, "req_tag", "8", RSTVAL_ZERO, NO_COMMENT),
    req_be(this, "req_be", "8", RSTVAL_ZERO, NO_COMMENT),
    req_bytes(this, "req_bytes", "12", "'0", NO_COMMENT),
    req_addr(this, "req_addr", "CFG_PCIE_DMAADDR_WIDTH", RSTVAL_ZERO, NO_COMMENT),
    wr_en(this, "wr_en", "1", RSTVAL_ZERO, NO_COMMENT),
    wr_data(this, "wr_data", "64", "'0", NO_COMMENT),
    wr_strob(this, "wr_strob", "8", "'0", NO_COMMENT),
    wr_data_dw1(this, "wr_data_dw1", "32", "'0", "Odd word in a burst sequence"),
    wr_dw1_valid(this, "wr_dw1_valid", "1", "'0", "Not last DW1 was received in burst sequence"),
    state(this, "state", "7", "PIO_RX_RST_STATE", NO_COMMENT),
    tlp_type(this, "tlp_type", "8", RSTVAL_ZERO, NO_COMMENT),
    tlp_resp(this, "tlp_resp", "3", "'0", NO_COMMENT),
    //
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void pcie_io_rx_engine::proc_comb() {
TEXT();
    IF (NZ(BIT(i_m_axis_rx_tuser, 2)), "Select BAR0 region");
        SETVAL(comb.vb_bar_offset, CONST("0x08000000", "CFG_PCIE_DMAADDR_WIDTH"), "BAR0, 32-bits, 2MB, SRAM");
    ELSIF (NZ(BIT(i_m_axis_rx_tuser, 3)), "Select BAR1 region");
        SETVAL(comb.vb_bar_offset, CONST("0x10000000", "CFG_PCIE_DMAADDR_WIDTH"), "BAR1, 32-bits, 2MB");
    ELSE();
        SETVAL(comb.vb_bar_offset, CONST("0x80000000", "CFG_PCIE_DMAADDR_WIDTH"), "BAR2/BAR3 64-bits, 16MB to DDR");
    ENDIF();

TEXT();
    IF (NZ(BIT(req_be, 0)));
        SETZERO(comb.vb_req_addr_1_0);
    ELSIF (NZ(BIT(req_be, 1)));
        SETVAL(comb.vb_req_addr_1_0, CONST("1", 2));
    ELSIF (NZ(BIT(req_be, 2)));
        SETVAL(comb.vb_req_addr_1_0, CONST("2", 2));
    ELSIF (NZ(BIT(req_be, 3)));
        SETVAL(comb.vb_req_addr_1_0, CONST("3", 2));
    ENDIF();
    TEXT("Max implemented BAR is 4GB so take 32-bits of address");
    SETVAL(comb.vb_addr_ldw, CCx(3, &BITS(comb.vb_bar_offset, DEC(CONST("CFG_PCIE_DMAADDR_WIDTH")), CONST("16")),
                                    &BITS(i_m_axis_rx_tdata, 15, 2),
                                    &comb.vb_req_addr_1_0));
    SETVAL(comb.vb_addr_mdw, CCx(3, &BITS(comb.vb_bar_offset, DEC(CONST("CFG_PCIE_DMAADDR_WIDTH")), CONST("16")),
                                    &BITS(i_m_axis_rx_tdata, 47, 34),
                                    &comb.vb_req_addr_1_0));

TEXT();
    TEXT("Calculate byte count based on byte enable");
    SETVAL(comb.vb_add_be20, ADD2(CC2(CONST("0", 1), BIT(req_be, 3)), CC2(CONST("0", 1), BIT(req_be, 2))));
    SETVAL(comb.vb_add_be21, ADD2(CC2(CONST("0", 1), BIT(req_be, 1)), CC2(CONST("0", 1), BIT(req_be, 0))));
    IF (EQ(req_len, CONST("1", 10)));
        SETVAL(comb.vb_req_bytes, ADD2(CC2(CONST("0", 10), comb.vb_add_be20), CC2(CONST("0", 10), comb.vb_add_be21)));
    ELSE ();
        TEXT("PCI request size 0 is equal to 1024 bytes:");
        SETVAL(comb.vb_req_bytes_msb, CC2(CONST("0", 1), INV_L(OR_REDUCE(req_len))));
        SETVAL(comb.vb_req_bytes, CC3(comb.vb_req_bytes_msb, req_len, CONST("0", 2)));
    ENDIF();

TEXT();
    IF (NZ(i_req_mem_ready));
        SETZERO(req_valid);
        SETZERO(req_last);
    ENDIF();
    IF (NZ(i_m_axis_rx_tvalid));
        SETZERO(m_axis_rx_tready);
    ENDIF();

TEXT();
    SWITCH(state);
    CASE(PIO_RX_RST_STATE);
        SETONE(m_axis_rx_tready);
        SETZERO(tlp_resp);
        SETZERO(wr_en, "IO Write");
        SETZERO(wr_strob);
        SETZERO(wr_data);
        SETZERO(req_len);

        TEXT();
        IF (AND2(NZ(m_axis_rx_tready), NZ(i_m_axis_rx_tvalid)));
            SETVAL(tlp_type, BITS(i_m_axis_rx_tdata, 31, 24));
            SETVAL(req_tc, BITS(i_m_axis_rx_tdata, 22, 20));
            SETVAL(req_td, BIT(i_m_axis_rx_tdata, 15));
            SETVAL(req_ep, BIT(i_m_axis_rx_tdata, 14));
            SETVAL(req_attr, BITS(i_m_axis_rx_tdata, 13, 12));
            SETVAL(req_len, BITS(i_m_axis_rx_tdata, 9, 0));
            SETVAL(req_rid, BITS(i_m_axis_rx_tdata, 63, 48));
            SETVAL(req_tag, BITS(i_m_axis_rx_tdata, 47, 40));
            SETVAL(req_be, BITS(i_m_axis_rx_tdata, 39, 32));

            TEXT();
            SWITCH(BITS(i_m_axis_rx_tdata, 30, 24));
            CASE(PIO_RX_MEM_RD32_FMT_TYPE);
                SETVAL(state, PIO_RX_MEM_RD32_DW1DW2);
            ENDCASE();

            TEXT();
            CASE(PIO_RX_MEM_WR32_FMT_TYPE);
                SETVAL(state, PIO_RX_MEM_WR32_DW1DW2);
                SETONE(wr_en);
            ENDCASE();

            TEXT();
            CASE(PIO_RX_MEM_RD64_FMT_TYPE);
                SETVAL(state, PIO_RX_MEM_RD64_DW1DW2);
            ENDCASE();

            TEXT();
            CASE(PIO_RX_MEM_WR64_FMT_TYPE);
                SETVAL(state, PIO_RX_MEM_WR64_DW1DW2);
                SETONE(wr_en);
            ENDCASE();

            TEXT();
            CASE(PIO_RX_IO_RD32_FMT_TYPE);
                SETVAL(state, PIO_RX_MEM_RD32_DW1DW2);
            ENDCASE();

            TEXT();
            CASE(PIO_RX_IO_WR32_FMT_TYPE);
                IF (EQ(BITS(i_m_axis_rx_tdata, 9, 0), CONST("1", 10)));
                    SETVAL(state, PIO_RX_IO_WR_DW1DW2);
                    SETONE(wr_en);
                ENDIF();
            ENDCASE();

            TEXT();
            CASEDEF();
                SETVAL(state, PIO_RX_RST_STATE);
            ENDCASE();
            ENDSWITCH();
        ELSE();
            SETVAL(state, PIO_RX_RST_STATE);
        ENDIF();
    ENDCASE();

    TEXT();
    CASE(PIO_RX_MEM_RD32_DW1DW2);
        IF (NZ(i_m_axis_rx_tvalid));
            SETONE(req_valid);
            SETVAL(req_last, i_m_axis_rx_tlast);
            SETVAL(req_addr, comb.vb_addr_ldw);
            SETVAL(req_bytes, comb.vb_req_bytes);
            SETVAL(tlp_resp, TLP_POSTED);
            SETVAL(state, PIO_RX_WAIT_TX_COMPLETION);
        ENDIF();
    ENDCASE();

    TEXT();
    CASE(PIO_RX_MEM_RD64_DW1DW2);
        IF (NZ(i_m_axis_rx_tvalid));
            SETONE(req_valid);
            SETVAL(req_addr, comb.vb_addr_mdw);
            SETVAL(req_bytes, comb.vb_req_bytes);
            SETVAL(tlp_resp, TLP_POSTED);
            SETVAL(state, PIO_RX_WAIT_TX_COMPLETION);
        ENDIF();
    ENDCASE();

    TEXT();
    CASE(PIO_RX_MEM_WR32_DW1DW2);
        IF (NZ(i_m_axis_rx_tvalid));
            SETVAL(req_valid, i_m_axis_rx_tlast);
            SETVAL(req_last, i_m_axis_rx_tlast);
            SETVAL(req_addr, comb.vb_addr_ldw);
            SETVAL(req_bytes, comb.vb_req_bytes);
            SETVAL(wr_data, CC2(BITS(i_m_axis_rx_tdata, 63, 32),
                                BITS(i_m_axis_rx_tdata, 63, 32)));
            SETVAL(wr_data_dw1, BITS(i_m_axis_rx_tdata, 63, 32));
            IF (NZ(BIT(i_m_axis_rx_tdata, 2)));
                SETVAL(wr_strob, CC2(BITS(req_be, 3, 0), BITS(req_be, 7, 4)));
            ELSE();
                SETVAL(wr_strob, req_be);
            ENDIF();

            TEXT();
            TEXT("Burst support:");
            SETVAL(m_axis_rx_tready, AND2_L(i_req_mem_ready, INV_L(i_m_axis_rx_tlast)));
            IF (NZ(i_m_axis_rx_tlast));
                TEXT("Send response after full TLP received:");
                SETVAL(tlp_resp, TLP_NON_POSTED);
                SETVAL(state, PIO_RX_WAIT_TX_COMPLETION);
            ELSE();
                SETONE(wr_dw1_valid);
                SETVAL(wr_strob, CONST("0xff", 8));
                SETVAL(state, PIO_RX_MEM_WR_BURST);
            ENDIF();
        ENDIF();
    ENDCASE();

    TEXT();
    CASE(PIO_RX_MEM_WR64_DW1DW2);
        IF (NZ(i_m_axis_rx_tvalid));
            SETVAL(req_addr, comb.vb_addr_mdw);
            SETVAL(req_bytes, comb.vb_req_bytes);
            IF (NZ(BIT(i_m_axis_rx_tdata, 34)));
                SETVAL(wr_strob, CC2(BITS(req_be, 3, 0), BITS(req_be, 7, 4)));
            ELSE();
                SETVAL(wr_strob, req_be);
            ENDIF();
            SETVAL(state, PIO_RX_MEM_WR_BURST);
        ENDIF();
    ENDCASE();

    TEXT();
    CASE(PIO_RX_MEM_WR_BURST);
        IF (NZ(i_m_axis_rx_tvalid));
            SETVAL(m_axis_rx_tready, AND2_L(i_req_mem_ready, INV_L(i_m_axis_rx_tlast)));
            SETONE(req_valid);
            SETVAL(req_last, i_m_axis_rx_tlast);
            IF (AND2(NZ(BITS(i_m_axis_rx_tkeep, 7, 4)), NZ(BITS(i_m_axis_rx_tkeep, 3, 0))));
                IF (NZ(wr_dw1_valid));
                    SETVAL(wr_data, CC2(BITS(i_m_axis_rx_tdata, 31, 0),
                                        wr_data_dw1));
                    SETVAL(wr_data_dw1, BITS(i_m_axis_rx_tdata, 63, 32));
                ELSE();
                    SETVAL(wr_data, i_m_axis_rx_tdata);
                ENDIF();
            ELSE();
                SETZERO(wr_dw1_valid);
                IF (NZ(wr_dw1_valid));
                    SETVAL(wr_data, CC2(BITS(i_m_axis_rx_tdata, 31, 0),
                                        BITS(wr_data_dw1, 31, 0)));
                ELSE();
                    SETVAL(wr_data, CC2(BITS(i_m_axis_rx_tdata, 31, 0),
                                        BITS(i_m_axis_rx_tdata, 31, 0)));
                ENDIF();
            ENDIF();

            TEXT();      
            IF (NZ(i_m_axis_rx_tlast));
                TEXT("Send response after full TLP received:");
                SETZERO(wr_dw1_valid, "Software has to use 8-bytes aligned burst transactions");
                SETVAL(tlp_resp, TLP_NON_POSTED);
                SETVAL(state, PIO_RX_WAIT_TX_COMPLETION);
            ENDIF();
        ENDIF();
    ENDCASE();

    TEXT();
    CASE(PIO_RX_IO_WR_DW1DW2);
        IF (NZ(i_m_axis_rx_tvalid));
            SETONE(req_valid);
            SETONE(wr_en);
            SETVAL(req_addr, comb.vb_addr_ldw);
            SETVAL(req_bytes, comb.vb_req_bytes);
            SETVAL(wr_data, CC2(BITS(i_m_axis_rx_tdata, 63, 32),
                                BITS(i_m_axis_rx_tdata, 63, 32)));
            IF (NZ(BIT(i_m_axis_rx_tdata, 2)));
                SETVAL(wr_strob, CC2(BITS(req_be, 3, 0), BITS(req_be, 7, 4)));
            ELSE();
                SETVAL(wr_strob, req_be);
            ENDIF();
            SETVAL(tlp_resp, TLP_COMPLETION);
            SETVAL(state, PIO_RX_WAIT_TX_COMPLETION);
        ENDIF();
    ENDCASE();

    TEXT();
    CASE(PIO_RX_WAIT_TX_COMPLETION);
        IF(NZ(i_compl_done));
            SETONE(m_axis_rx_tready);
            SETZERO(wr_en, "IO Write");
            SETZERO(wr_strob);
            SETZERO(wr_data);
            SETZERO(req_len);
            SETZERO(tlp_resp);
            SETVAL(state, PIO_RX_RST_STATE);
        ENDIF();
    ENDCASE();

    TEXT();
    CASEDEF();
        SETVAL(state, PIO_RX_RST_STATE);
    ENDCASE();
    ENDSWITCH();

TEXT_ASSIGN();
    ASSIGN(o_m_axis_rx_tready, m_axis_rx_tready);
    ASSIGN(o_tx_ena, BIT(tlp_resp, 0));
    ASSIGN(o_tx_completion, BIT(tlp_resp, 1));
    ASSIGN(o_tx_with_data, BIT(tlp_resp, 2));
    ASSIGN(o_req_tc, req_tc);
    ASSIGN(o_req_td, req_td);
    ASSIGN(o_req_ep, req_ep);
    ASSIGN(o_req_attr, req_attr);
    ASSIGN(o_req_len, req_len);
    ASSIGN(o_req_rid, req_rid);
    ASSIGN(o_req_tag, req_tag);
    ASSIGN(o_req_be, req_be);
    ASSIGN(o_req_addr, req_addr);

TEXT_ASSIGN();
    ASSIGN(o_req_mem_valid, req_valid);
    ASSIGN(o_req_mem_write, wr_en);
    ASSIGN(o_req_mem_bytes, req_bytes);
    ASSIGN(o_req_mem_addr, req_addr);
    ASSIGN(o_req_mem_strob, wr_strob);
    ASSIGN(o_req_mem_data, wr_data);
    ASSIGN(o_req_mem_last, req_last);
}

