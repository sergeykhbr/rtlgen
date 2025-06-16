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

#include "afifo_xmst.h"

afifo_xmst::afifo_xmst(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "afifo_xmst", name, comment),
    // Generic parameters
    abits_depth(this, "abits_depth", "2", "Depth of the address channels AR/AW/B"),
    dbits_depth(this, "dbits_depth", "10", "Depth of the data channels R/W"),
    // Ports
    i_xmst_nrst(this, "i_xmst_nrst", "1", "Input requests reset A"),
    i_xmst_clk(this, "i_xmst_clk", "1", "Input requests clock A"),
    i_xmsto(this, "i_xmsto", "Input slave interface"),
    o_xmsti(this, "o_xmsti", "Response on input slave reuqest"),
    i_xslv_nrst(this, "i_xslv_nrst", "1", "Output request reset B"),
    i_xslv_clk(this, "i_xslv_clk", "1", "Output request clock B"),
    o_xslvi(this, "o_xslvi", "Output request to connected slave"),
    i_xslvo(this, "i_xslvo", "Response from the connected slave"),
    // params
    AR_REQ_WIDTH(this, "AR_REQ_WIDTH", &CALCWIDTHx(11, &i_xmsto.ar_bits.addr,
                                                       &i_xmsto.ar_bits.len,
                                                       &i_xmsto.ar_bits.size,
                                                       &i_xmsto.ar_bits.burst,
                                                       &i_xmsto.ar_bits.lock,
                                                       &i_xmsto.ar_bits.cache,
                                                       &i_xmsto.ar_bits.prot,
                                                       &i_xmsto.ar_bits.qos,
                                                       &i_xmsto.ar_bits.region,
                                                       &i_xmsto.ar_id,
                                                       &i_xmsto.ar_user)),
    AW_REQ_WIDTH(this, "AW_REQ_WIDTH", &CALCWIDTHx(11, &i_xmsto.aw_bits.addr,
                                                       &i_xmsto.aw_bits.len,
                                                       &i_xmsto.aw_bits.size,
                                                       &i_xmsto.aw_bits.burst,
                                                       &i_xmsto.aw_bits.lock,
                                                       &i_xmsto.aw_bits.cache,
                                                       &i_xmsto.aw_bits.prot,
                                                       &i_xmsto.aw_bits.qos,
                                                       &i_xmsto.aw_bits.region,
                                                       &i_xmsto.aw_id,
                                                       &i_xmsto.aw_user)),
    W_REQ_WIDTH(this, "W_REQ_WIDTH", &CALCWIDTHx(4, &i_xmsto.w_data,
                                                    &i_xmsto.w_last,
                                                    &i_xmsto.w_strb,
                                                    &i_xmsto.w_user)),
    R_RESP_WIDTH(this, "R_RESP_WIDTH", &CALCWIDTHx(5, &i_xslvo.r_resp,
                                                      &i_xslvo.r_data,
                                                      &i_xslvo.r_last,
                                                      &i_xslvo.r_id,
                                                      &i_xslvo.r_user)),
    B_RESP_WIDTH(this, "B_RESP_WIDTH", &CALCWIDTHx(3, &i_xslvo.b_resp,
                                                      &i_xslvo.b_id,
                                                      &i_xslvo.b_user)),
    // Singals:
    // registers
    w_req_ar_valid_i(this, "w_req_ar_valid_i", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_req_ar_payload_i(this, "wb_req_ar_payload_i", "AR_REQ_WIDTH", RSTVAL_ZERO, NO_COMMENT),
    w_req_ar_wready_o(this, "w_req_ar_wready_o", "1", RSTVAL_ZERO, NO_COMMENT),
    w_req_ar_rd_i(this, "w_req_ar_rd_i", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_req_ar_payload_o(this, "wb_req_ar_payload_o", "AR_REQ_WIDTH", RSTVAL_ZERO, NO_COMMENT),
    w_req_ar_rvalid_o(this, "w_req_ar_rvalid_o", "1", RSTVAL_ZERO, NO_COMMENT),
    w_req_aw_valid_i(this, "w_req_aw_valid_i", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_req_aw_payload_i(this, "wb_req_aw_payload_i", "AW_REQ_WIDTH", RSTVAL_ZERO, NO_COMMENT),
    w_req_aw_wready_o(this, "w_req_aw_wready_o", "1", RSTVAL_ZERO, NO_COMMENT),
    w_req_aw_rd_i(this, "w_req_aw_rd_i", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_req_aw_payload_o(this, "wb_req_aw_payload_o", "AW_REQ_WIDTH", RSTVAL_ZERO, NO_COMMENT),
    w_req_aw_rvalid_o(this, "w_req_aw_rvalid_o", "1", RSTVAL_ZERO, NO_COMMENT),
    w_req_w_valid_i(this, "w_req_w_valid_i", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_req_w_payload_i(this, "wb_req_w_payload_i", "W_REQ_WIDTH", RSTVAL_ZERO, NO_COMMENT),
    w_req_w_wready_o(this, "w_req_w_wready_o", "1", RSTVAL_ZERO, NO_COMMENT),
    w_req_w_rd_i(this, "w_req_w_rd_i", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_req_w_payload_o(this, "wb_req_w_payload_o", "W_REQ_WIDTH", RSTVAL_ZERO, NO_COMMENT),
    w_req_w_rvalid_o(this, "w_req_w_rvalid_o", "1", RSTVAL_ZERO, NO_COMMENT),
    w_resp_r_valid_i(this, "w_resp_r_valid_i", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_resp_r_payload_i(this, "wb_resp_r_payload_i", "R_RESP_WIDTH", RSTVAL_ZERO, NO_COMMENT),
    w_resp_r_wready_o(this, "w_resp_r_wready_o", "1", RSTVAL_ZERO, NO_COMMENT),
    w_resp_r_rd_i(this, "w_resp_r_rd_i", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_resp_r_payload_o(this, "wb_resp_r_payload_o", "R_RESP_WIDTH", RSTVAL_ZERO, NO_COMMENT),
    w_resp_r_rvalid_o(this, "w_resp_r_rvalid_o", "1", RSTVAL_ZERO, NO_COMMENT),
    w_resp_b_valid_i(this, "w_resp_b_valid_i", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_resp_b_payload_i(this, "wb_resp_b_payload_i", "B_RESP_WIDTH", RSTVAL_ZERO, NO_COMMENT),
    w_resp_b_wready_o(this, "w_resp_b_wready_o", "1", RSTVAL_ZERO, NO_COMMENT),
    w_resp_b_rd_i(this, "w_resp_b_rd_i", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_resp_b_payload_o(this, "wb_resp_b_payload_o", "B_RESP_WIDTH", RSTVAL_ZERO, NO_COMMENT),
    w_resp_b_rvalid_o(this, "w_resp_b_rvalid_o", "1", RSTVAL_ZERO, NO_COMMENT),
    // functions
    // submodules:
    req_ar(this, "req_ar"),
    req_aw(this, "req_aw"),
    req_w(this, "req_w"),
    resp_r(this, "resp_r"),
    resp_b(this, "resp_b"),
    comb(this)
{
    Operation::start(this);

    req_ar.abits.setObjValue(&abits_depth);
    req_ar.dbits.setObjValue(&AR_REQ_WIDTH);
    NEW(req_ar, req_ar.getName().c_str());
        CONNECT(req_ar, 0, req_ar.i_nrst, i_xslv_nrst);
        CONNECT(req_ar, 0, req_ar.i_wclk, i_xslv_clk);
        CONNECT(req_ar, 0, req_ar.i_wr, w_req_ar_valid_i);
        CONNECT(req_ar, 0, req_ar.i_wdata, wb_req_ar_payload_i);
        CONNECT(req_ar, 0, req_ar.o_wready, w_req_ar_wready_o);
        CONNECT(req_ar, 0, req_ar.i_rclk, i_xmst_clk);
        CONNECT(req_ar, 0, req_ar.i_rd, w_req_ar_rd_i);
        CONNECT(req_ar, 0, req_ar.o_rdata, wb_req_ar_payload_o);
        CONNECT(req_ar, 0, req_ar.o_rvalid, w_req_ar_rvalid_o);
    ENDNEW();

    TEXT();
    req_aw.abits.setObjValue(&abits_depth);
    req_aw.dbits.setObjValue(&AW_REQ_WIDTH);
    NEW(req_aw, req_aw.getName().c_str());
        CONNECT(req_aw, 0, req_aw.i_nrst, i_xslv_nrst);
        CONNECT(req_aw, 0, req_aw.i_wclk, i_xslv_clk);
        CONNECT(req_aw, 0, req_aw.i_wr, w_req_aw_valid_i);
        CONNECT(req_aw, 0, req_aw.i_wdata, wb_req_aw_payload_i);
        CONNECT(req_aw, 0, req_aw.o_wready, w_req_aw_wready_o);
        CONNECT(req_aw, 0, req_aw.i_rclk, i_xmst_clk);
        CONNECT(req_aw, 0, req_aw.i_rd, w_req_aw_rd_i);
        CONNECT(req_aw, 0, req_aw.o_rdata, wb_req_aw_payload_o);
        CONNECT(req_aw, 0, req_aw.o_rvalid, w_req_aw_rvalid_o);
    ENDNEW();

    TEXT();
    req_w.abits.setObjValue(&dbits_depth);
    req_w.dbits.setObjValue(&W_REQ_WIDTH);
    NEW(req_w, req_w.getName().c_str());
        CONNECT(req_w, 0, req_w.i_nrst, i_xslv_nrst);
        CONNECT(req_w, 0, req_w.i_wclk, i_xslv_clk);
        CONNECT(req_w, 0, req_w.i_wr, w_req_w_valid_i);
        CONNECT(req_w, 0, req_w.i_wdata, wb_req_w_payload_i);
        CONNECT(req_w, 0, req_w.o_wready, w_req_w_wready_o);
        CONNECT(req_w, 0, req_w.i_rclk, i_xmst_clk);
        CONNECT(req_w, 0, req_w.i_rd, w_req_w_rd_i);
        CONNECT(req_w, 0, req_w.o_rdata, wb_req_w_payload_o);
        CONNECT(req_w, 0, req_w.o_rvalid, w_req_w_rvalid_o);
    ENDNEW();

    TEXT();
    resp_r.abits.setObjValue(&dbits_depth);
    resp_r.dbits.setObjValue(&R_RESP_WIDTH);
    NEW(resp_r, resp_r.getName().c_str());
        CONNECT(resp_r, 0, resp_r.i_nrst, i_xmst_nrst);
        CONNECT(resp_r, 0, resp_r.i_wclk, i_xmst_clk);
        CONNECT(resp_r, 0, resp_r.i_wr, w_resp_r_valid_i);
        CONNECT(resp_r, 0, resp_r.i_wdata, wb_resp_r_payload_i);
        CONNECT(resp_r, 0, resp_r.o_wready, w_resp_r_wready_o);
        CONNECT(resp_r, 0, resp_r.i_rclk, i_xslv_clk);
        CONNECT(resp_r, 0, resp_r.i_rd, w_resp_r_rd_i);
        CONNECT(resp_r, 0, resp_r.o_rdata, wb_resp_r_payload_o);
        CONNECT(resp_r, 0, resp_r.o_rvalid, w_resp_r_rvalid_o);
    ENDNEW();

    TEXT();
    resp_b.abits.setObjValue(&abits_depth);
    resp_b.dbits.setObjValue(&B_RESP_WIDTH);
    NEW(resp_b, resp_b.getName().c_str());
        CONNECT(resp_b, 0, resp_b.i_nrst, i_xmst_nrst);
        CONNECT(resp_b, 0, resp_b.i_wclk, i_xmst_clk);
        CONNECT(resp_b, 0, resp_b.i_wr, w_resp_b_valid_i);
        CONNECT(resp_b, 0, resp_b.i_wdata, wb_resp_b_payload_i);
        CONNECT(resp_b, 0, resp_b.o_wready, w_resp_b_wready_o);
        CONNECT(resp_b, 0, resp_b.i_rclk, i_xslv_clk);
        CONNECT(resp_b, 0, resp_b.i_rd, w_resp_b_rd_i);
        CONNECT(resp_b, 0, resp_b.o_rdata, wb_resp_b_payload_o);
        CONNECT(resp_b, 0, resp_b.o_rvalid, w_resp_b_rvalid_o);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void afifo_xmst::proc_comb() {
    TEXT("ar channel write side:");
    SETVAL(w_req_ar_valid_i, i_xmsto.ar_valid);
    SETVAL(comb.vb_xmsti.ar_ready, w_req_ar_wready_o);
    SETVAL(wb_req_ar_payload_i, CCx(11, &i_xmsto.ar_bits.addr,
                                        &i_xmsto.ar_bits.len,
                                        &i_xmsto.ar_bits.size,
                                        &i_xmsto.ar_bits.burst,
                                        &i_xmsto.ar_bits.lock,
                                        &i_xmsto.ar_bits.cache,
                                        &i_xmsto.ar_bits.prot,
                                        &i_xmsto.ar_bits.qos,
                                        &i_xmsto.ar_bits.region,
                                        &i_xmsto.ar_id,
                                        &i_xmsto.ar_user));
    TEXT("ar channel read side:");
    SETVAL(comb.vb_xslvi.ar_valid, w_req_ar_rvalid_o);
    SETVAL(w_req_ar_rd_i, i_xslvo.ar_ready);
    SPLx(wb_req_ar_payload_o, 11, &comb.vb_xslvi.ar_bits.addr,
                                  &comb.vb_xslvi.ar_bits.len,
                                  &comb.vb_xslvi.ar_bits.size,
                                  &comb.vb_xslvi.ar_bits.burst,
                                  &comb.vb_xslvi.ar_bits.lock,
                                  &comb.vb_xslvi.ar_bits.cache,
                                  &comb.vb_xslvi.ar_bits.prot,
                                  &comb.vb_xslvi.ar_bits.qos,
                                  &comb.vb_xslvi.ar_bits.region,
                                  &comb.vb_xslvi.ar_id,
                                  &comb.vb_xslvi.ar_user);


    TEXT();
    TEXT("aw channel write side:");
    SETVAL(w_req_aw_valid_i, i_xmsto.aw_valid);
    SETVAL(comb.vb_xmsti.aw_ready, w_req_aw_wready_o);
    SETVAL(wb_req_aw_payload_i, CCx(11, &i_xmsto.aw_bits.addr,
                                        &i_xmsto.aw_bits.len,
                                        &i_xmsto.aw_bits.size,
                                        &i_xmsto.aw_bits.burst,
                                        &i_xmsto.aw_bits.lock,
                                        &i_xmsto.aw_bits.cache,
                                        &i_xmsto.aw_bits.prot,
                                        &i_xmsto.aw_bits.qos,
                                        &i_xmsto.aw_bits.region,
                                        &i_xmsto.aw_id,
                                        &i_xmsto.aw_user));
    TEXT("aw channel read side:");
    SETVAL(comb.vb_xslvi.aw_valid, w_req_aw_rvalid_o);
    SETVAL(w_req_aw_rd_i, i_xslvo.aw_ready);
    SPLx(wb_req_aw_payload_o, 11, &comb.vb_xslvi.aw_bits.addr,
                                  &comb.vb_xslvi.aw_bits.len,
                                  &comb.vb_xslvi.aw_bits.size,
                                  &comb.vb_xslvi.aw_bits.burst,
                                  &comb.vb_xslvi.aw_bits.lock,
                                  &comb.vb_xslvi.aw_bits.cache,
                                  &comb.vb_xslvi.aw_bits.prot,
                                  &comb.vb_xslvi.aw_bits.qos,
                                  &comb.vb_xslvi.aw_bits.region,
                                  &comb.vb_xslvi.aw_id,
                                  &comb.vb_xslvi.aw_user);

    TEXT();
    TEXT("w channel write side:");
    SETVAL(w_req_w_valid_i, i_xmsto.w_valid);
    SETVAL(comb.vb_xmsti.w_ready, w_req_w_wready_o);
    SETVAL(wb_req_w_payload_i, CCx(4, &i_xmsto.w_data,
                                      &i_xmsto.w_last,
                                      &i_xmsto.w_strb,
                                      &i_xmsto.w_user));
    TEXT("w channel read side:");
    SETVAL(comb.vb_xslvi.w_valid, w_req_w_rvalid_o);
    SETVAL(w_req_w_rd_i, i_xslvo.w_ready);
    SPLx(wb_req_w_payload_o, 4, &comb.vb_xslvi.w_data,
                                 &comb.vb_xslvi.w_last,
                                 &comb.vb_xslvi.w_strb,
                                 &comb.vb_xslvi.w_user);

    TEXT();
    TEXT("r channel write side:");
    SETVAL(w_resp_r_valid_i, i_xslvo.r_valid);
    SETVAL(comb.vb_xslvi.r_ready, w_resp_r_wready_o);
    SETVAL(wb_resp_r_payload_i, CCx(5, &i_xslvo.r_resp,
                                       &i_xslvo.r_data,
                                       &i_xslvo.r_last,
                                       &i_xslvo.r_id,
                                       &i_xslvo.r_user));
    TEXT("r channel read side:");
    SETVAL(comb.vb_xmsti.r_valid, w_resp_r_rvalid_o);
    SETVAL(w_resp_r_rd_i, i_xmsto.r_ready);
    SPLx(wb_resp_r_payload_o, 5, &comb.vb_xmsti.r_resp,
                                 &comb.vb_xmsti.r_data,
                                 &comb.vb_xmsti.r_last,
                                 &comb.vb_xmsti.r_id,
                                 &comb.vb_xmsti.r_user);

    TEXT();
    TEXT("b channel write side:");
    SETVAL(w_resp_b_valid_i, i_xslvo.b_valid);
    SETVAL(comb.vb_xslvi.b_ready, w_resp_b_wready_o);
    SETVAL(wb_resp_b_payload_i, CCx(3, &i_xslvo.b_resp,
                                       &i_xslvo.b_id,
                                       &i_xslvo.b_user));
    TEXT("b channel read side:");
    SETVAL(comb.vb_xmsti.b_valid, w_resp_b_rvalid_o);
    SETVAL(w_resp_b_rd_i, i_xmsto.b_ready);
    SPLx(wb_resp_b_payload_o, 3, &comb.vb_xmsti.b_resp,
                                 &comb.vb_xmsti.b_id,
                                 &comb.vb_xmsti.b_user);

    TEXT();
    SETVAL(o_xmsti, comb.vb_xmsti);
    SETVAL(o_xslvi, comb.vb_xslvi);
}
