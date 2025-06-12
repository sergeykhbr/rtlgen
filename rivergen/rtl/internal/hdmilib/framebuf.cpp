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

#include "framebuf.h"

framebuf::framebuf(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "framebuf", name, comment),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_hsync(this, "i_hsync", "1", "Horizontal sync"),
    i_vsync(this, "i_vsync", "1", "Vertical sync"),
    i_de(this, "i_de", "1", "data enable"),
    i_width_m1(this, "i_width_m1", "12", "x-width: 4K = 3840 - 1"),
    i_height_m1(this, "i_height_m1", "12", "y-height: 4K = 2160 - 1"),
    o_hsync(this, "o_hsync", "1", "delayed horizontal sync"),
    o_vsync(this, "o_vsync", "1", "delayed vertical sync"),
    o_de(this, "o_de", "1", "delayed data enable"),
    o_rgb565(this, "o_rgb565", "16", "RGB 16-bits pixels"),
    _dma0_(this, "DMA engine compatible interface (always read). Get pixels array:"),
    i_req_2d_ready(this, "i_req_2d_ready", "1", "2D pixels ready to accept request"),
    o_req_2d_valid(this, "o_req_2d_valid", "1", "2D pixels request is valid"),
    o_req_2d_bytes(this, "o_req_2d_bytes", "12", "0=4096 Bytes; 4=DWORD; 8=QWORD; ..."),
    o_req_2d_addr(this, "o_req_2d_addr", "24", "16 MB allocated for framebuffer"),
    i_resp_2d_valid(this, "i_resp_2d_valid", "1", "2D pixels buffer response is valid"),
    i_resp_2d_last(this, "i_resp_2d_last", "1", "Last data in burst read"),
    i_resp_2d_addr(this, "i_resp_2d_addr", "24", "16 MB allocated for framebuffer"),
    i_resp_2d_data(this, "i_resp_2d_data", "64", "Read data"),
    o_resp_2d_ready(this, "o_resp_2d_ready", "1", "Ready to accept 2D pixels response"),
    // params
    _state0_(this, "state machine states:"),
    STATE_Request(this, "STATE_Request", "2", "0x1", NO_COMMENT),
    STATE_Writing(this, "STATE_Writing", "2", "0x2", NO_COMMENT),
    STATE_Idle(this, "STATE_Idle", "2", "0x0", NO_COMMENT),
    // signals
    wb_ring0_addr(this, "wb_ring0_addr", "6", "'0", NO_COMMENT),
    w_ring0_wena(this, "w_ring0_wena", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_ring0_rdata(this, "wb_ring0_rdata", "64", "'0", NO_COMMENT),
    wb_ring1_addr(this, "wb_ring1_addr", "6", "'0", NO_COMMENT),
    w_ring1_wena(this, "w_ring1_wena", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_ring1_rdata(this, "wb_ring1_rdata", "64", "'0", NO_COMMENT),
    wb_ring2_addr(this, "wb_ring2_addr", "6", "'0", NO_COMMENT),
    w_ring2_wena(this, "w_ring2_wena", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_ring2_rdata(this, "wb_ring2_rdata", "64", "'0", NO_COMMENT),
    wb_ring3_addr(this, "wb_ring3_addr", "6", "'0", NO_COMMENT),
    w_ring3_wena(this, "w_ring3_wena", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_ring3_rdata(this, "wb_ring3_rdata", "64", "'0", NO_COMMENT),
    // registers
    wr_row(this, "wr_row", "12", "'0", NO_COMMENT),
    wr_col(this, "wr_col", "12", "'0", NO_COMMENT),
    wr_addr(this, "wr_addr", "8", "'0", NO_COMMENT),
    rd_row(this, "rd_row", "12", "'0", NO_COMMENT),
    rd_col(this, "rd_col", "12", "'0", NO_COMMENT),
    rd_addr(this, "rd_addr", "8", "'0", NO_COMMENT),
    mux_ena(this, "mux_ena", "4", "0x1", NO_COMMENT),
    ring_sel(this, "ring_sel", "4", "'0", NO_COMMENT),
    pix_sel(this, "pix_sel", "4", "'0", NO_COMMENT),
    difcnt(this, "difcnt", "9", "'0", NO_COMMENT),
    state(this, "state", "2", "STATE_Idle", NO_COMMENT),
    rowcnt(this, "rowcnt", "12", "0", NO_COMMENT),
    req_addr(this, "req_addr", "24", "'0", "16 MB allocated space split on 64 B: 32x64=2048 B"),
    req_valid(this, "req_valid", "1", RSTVAL_ZERO, NO_COMMENT),
    resp_ready(this, "resp_ready", "1", RSTVAL_ZERO, NO_COMMENT),
    h_sync(this, "h_sync", "4", RSTVAL_ZERO, NO_COMMENT),
    v_sync(this, "v_sync", "4", RSTVAL_ZERO, NO_COMMENT),
    de(this, "de", "4", RSTVAL_ZERO, NO_COMMENT),
    rgb(this, "rgb", "16", "'0", NO_COMMENT),
    // modules
    ring0(this, "ring0", NO_COMMENT),
    ring1(this, "ring1", NO_COMMENT),
    ring2(this, "ring2", NO_COMMENT),
    ring3(this, "ring3", NO_COMMENT),
    // processes
    comb(this)
{
    Operation::start(this);

    ring0.abits.setObjValue(new DecConst(6));
    ring0.dbits.setObjValue(new DecConst(64));
    NEW(ring0, ring0.getName().c_str());
        CONNECT(ring0, 0, ring0.i_clk, i_clk);
        CONNECT(ring0, 0, ring0.i_addr, wb_ring0_addr);
        CONNECT(ring0, 0, ring0.i_wena, w_ring0_wena);
        CONNECT(ring0, 0, ring0.i_wdata, i_resp_2d_data);
        CONNECT(ring0, 0, ring0.o_rdata, wb_ring0_rdata);
    ENDNEW();

    TEXT();
    ring1.abits.setObjValue(new DecConst(6));
    ring1.dbits.setObjValue(new DecConst(64));
    NEW(ring1, ring1.getName().c_str());
        CONNECT(ring1, 0, ring1.i_clk, i_clk);
        CONNECT(ring1, 0, ring1.i_addr, wb_ring1_addr);
        CONNECT(ring1, 0, ring1.i_wena, w_ring1_wena);
        CONNECT(ring1, 0, ring1.i_wdata, i_resp_2d_data);
        CONNECT(ring1, 0, ring1.o_rdata, wb_ring1_rdata);
    ENDNEW();

    TEXT();
    ring2.abits.setObjValue(new DecConst(6));
    ring2.dbits.setObjValue(new DecConst(64));
    NEW(ring2, ring2.getName().c_str());
        CONNECT(ring2, 0, ring2.i_clk, i_clk);
        CONNECT(ring2, 0, ring2.i_addr, wb_ring2_addr);
        CONNECT(ring2, 0, ring2.i_wena, w_ring2_wena);
        CONNECT(ring2, 0, ring2.i_wdata, i_resp_2d_data);
        CONNECT(ring2, 0, ring2.o_rdata, wb_ring2_rdata);
    ENDNEW();

    TEXT();
    ring3.abits.setObjValue(new DecConst(6));
    ring3.dbits.setObjValue(new DecConst(64));
    NEW(ring3, ring3.getName().c_str());
        CONNECT(ring3, 0, ring3.i_clk, i_clk);
        CONNECT(ring3, 0, ring3.i_addr, wb_ring3_addr);
        CONNECT(ring3, 0, ring3.i_wena, w_ring3_wena);
        CONNECT(ring3, 0, ring3.i_wdata, i_resp_2d_data);
        CONNECT(ring3, 0, ring3.o_rdata, wb_ring3_rdata);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void framebuf::proc_comb() {
    TEXT("delayed signals:");
    SETVAL(de, CC2(BITS(de, 2, 0), i_de));
    SETVAL(h_sync, CC2(BITS(h_sync, 2, 0), i_hsync));
    SETVAL(v_sync, CC2(BITS(v_sync, 2, 0), i_vsync));

    TEXT();
    SWITCH(state);
    CASE(STATE_Idle);
        IF (OR2(NZ(BIT(difcnt, 8)), LE(difcnt, CONST("96", 9))));
            SETONE(req_valid);
            SETVAL(req_addr, CC2(wr_row, wr_col));
            SETVAL(state, STATE_Request);
        ENDIF();
    ENDCASE();
    CASE(STATE_Request);
        SETONE(req_valid);
        SETZERO(resp_ready);
        IF (AND2(NZ(req_valid), NZ(i_req_2d_ready)));
            SETZERO(req_valid);
            SETONE(resp_ready);
            SETVAL(state, STATE_Writing);
        ENDIF();
    ENDCASE();
    CASE(STATE_Writing);
        IF (NZ(i_resp_2d_valid));
            SETVAL(wr_col, ADD2(wr_col, CONST("4", 12)), "64-bits contains 4x16-bits pixels");
            SETVAL(wr_addr, INC(wr_addr));
            IF (NZ(i_resp_2d_last));
                IF (GE(wr_col, i_width_m1));
                    SETZERO(wr_col);
                    SETVAL(wr_row, INC(wr_row));
                    SETVAL(wr_addr, CC2(INC(BITS(wr_addr, 7, 6)), CONST("0", 6)));
                    IF (GE(wr_row, i_height_m1));
                        SETZERO(wr_row);
                    ENDIF();
                ENDIF();

                TEXT();
                SETZERO(resp_ready);
                SETVAL(state, STATE_Idle);
            ENDIF();
        ENDIF();
    ENDCASE();
    CASEDEF();
    ENDCASE();
    ENDSWITCH();


    TEXT();
    IF (NZ(i_de));
        SETVAL(mux_ena, CC2(BITS(mux_ena, 2, 0), BIT(mux_ena, 3)));
        IF (NZ(BIT(mux_ena, 0)));
            SETVAL(rd_addr, INC(rd_addr));
            SETVAL(rd_col, ADD2(rd_col, CONST("4", 12)), "64-bits contains 4x16-bits pixels");
        ENDIF();
    ELSIF(AND2(NZ(BIT(de, 0)), EZ(i_de)));
        TEXT("Back front of the de (end of row)");
        SETVAL(rd_addr, CC2(INC(BITS(rd_addr, 7, 6)), CONST("0", 6)));
        SETVAL(mux_ena, CONST("0x1", 4));
        SETZERO(rd_col);
        SETVAL(rd_row, INC(rd_row));
        IF (GE(rd_row, i_height_m1));
            SETZERO(rd_row);
        ENDIF();
    ENDIF();

    IF (AND2(NZ(i_resp_2d_valid), EZ(BIT(mux_ena, 0))));
        SETVAL(difcnt, INC(difcnt));
    ELSIF(AND3(EZ(i_resp_2d_valid), NZ(BIT(mux_ena, 0)), NZ(i_de)));
        SETVAL(difcnt, DEC(difcnt));
    ENDIF();


    TEXT();
    IF (EQ(BITS(wr_addr, 7, 6), CONST("0",2)));
        SETVAL(w_ring0_wena, i_resp_2d_valid);
        SETVAL(wb_ring0_addr, BITS(wr_addr, 6, 0));
        SETZERO(w_ring1_wena);
        SETVAL(wb_ring1_addr, BITS(rd_addr, 6, 0));
        SETZERO(w_ring2_wena);
        SETVAL(wb_ring2_addr, BITS(rd_addr, 6, 0));
        SETZERO(w_ring3_wena);
        SETVAL(wb_ring3_addr, BITS(rd_addr, 6, 0));
    ELSIF (EQ(BITS(wr_addr, 7, 6), CONST("1",2)));
        SETZERO(w_ring0_wena);
        SETVAL(wb_ring0_addr, BITS(rd_addr, 6, 0));
        SETVAL(w_ring1_wena, i_resp_2d_valid);
        SETVAL(wb_ring1_addr, BITS(wr_addr, 6, 0));
        SETZERO(w_ring2_wena);
        SETVAL(wb_ring2_addr, BITS(rd_addr, 6, 0));
        SETZERO(w_ring3_wena);
        SETVAL(wb_ring3_addr, BITS(rd_addr, 6, 0));
    ELSIF (EQ(BITS(wr_addr, 7, 6), CONST("2",2)));
        SETZERO(w_ring0_wena);
        SETVAL(wb_ring0_addr, BITS(rd_addr, 6, 0));
        SETZERO(w_ring1_wena);
        SETVAL(wb_ring1_addr, BITS(rd_addr, 6, 0));
        SETVAL(w_ring2_wena, i_resp_2d_valid);
        SETVAL(wb_ring2_addr, BITS(wr_addr, 6, 0));
        SETZERO(w_ring3_wena);
        SETVAL(wb_ring3_addr, BITS(rd_addr, 6, 0));
    ELSE();
        SETZERO(w_ring0_wena);
        SETVAL(wb_ring0_addr, BITS(rd_addr, 6, 0));
        SETZERO(w_ring1_wena);
        SETVAL(wb_ring1_addr, BITS(rd_addr, 6, 0));
        SETZERO(w_ring2_wena);
        SETVAL(wb_ring2_addr, BITS(rd_addr, 6, 0));
        SETVAL(w_ring3_wena, i_resp_2d_valid);
        SETVAL(wb_ring3_addr, BITS(wr_addr, 6, 0));
    ENDIF();
    IF(EQ(BITS(rd_addr, 7, 6), CONST("0", 2)));
        SETVAL(ring_sel, CONST("0x1", 4));
    ELSIF(EQ(BITS(rd_addr, 7, 6), CONST("1", 2)));
        SETVAL(ring_sel, CONST("0x2", 4));
    ELSIF(EQ(BITS(rd_addr, 7, 6), CONST("2", 2)));
        SETVAL(ring_sel, CONST("0x4", 4));
    ELSE();
        SETVAL(ring_sel, CONST("0x8", 4));
    ENDIF();
    SETVAL(pix_sel, mux_ena);

    TEXT();
    IF (NZ(BIT(ring_sel, 0)));
        SETVAL(comb.vb_ring_rdata, wb_ring0_rdata);
    ELSIF (NZ(BIT(ring_sel, 1)));
        SETVAL(comb.vb_ring_rdata, wb_ring1_rdata);
    ELSIF (NZ(BIT(ring_sel, 2)));
        SETVAL(comb.vb_ring_rdata, wb_ring2_rdata);
    ELSE();
        SETVAL(comb.vb_ring_rdata, wb_ring3_rdata);
    ENDIF();
    IF (EZ(de));
        SETZERO(comb.vb_pix);
    ELSIF (NZ(BIT(pix_sel, 0)));
        SETVAL(comb.vb_pix, BITS(comb.vb_ring_rdata, 15, 0));
    ELSIF (NZ(BIT(pix_sel, 1)));
        SETVAL(comb.vb_pix, BITS(comb.vb_ring_rdata, 31, 16));
    ELSIF (NZ(BIT(pix_sel, 2)));
        SETVAL(comb.vb_pix, BITS(comb.vb_ring_rdata, 47, 32));
    ELSE();
        SETVAL(comb.vb_pix, BITS(comb.vb_ring_rdata, 63, 48));
    ENDIF();
    SETVAL(rgb, comb.vb_pix);

    TEXT();
    SYNC_RESET();

    TEXT();
    SETVAL(o_hsync, BIT(h_sync, 1));
    SETVAL(o_vsync, BIT(v_sync, 1));
    SETVAL(o_de, BIT(de, 1));
    SETVAL(o_rgb565, rgb);

    TEXT();
    SETVAL(o_req_2d_valid, req_valid);
    SETVAL(o_req_2d_bytes, CONST("64", 12), "Xilinx MIG is limited to burst beat length 8");
    SETVAL(o_req_2d_addr, req_addr);
    SETVAL(o_resp_2d_ready, resp_ready);
}

