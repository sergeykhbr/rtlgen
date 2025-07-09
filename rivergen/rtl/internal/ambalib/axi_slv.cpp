// 
//  Copyright 2022 Sergey Khabarov, sergeykhbr@gmail.com
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

#include "axi_slv.h"

axi_slv::axi_slv(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "axi_slv", name, comment),
    vid(this, "vid", "0", "Vendor ID"),
    did(this, "did", "0", "Device ID"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_mapinfo(this, "i_mapinfo", "Base address information from the interconnect port"),
    o_cfg(this, "o_cfg", "Slave config descriptor"),
    i_xslvi(this, "i_xslvi", "AXI Slave input interface"),
    o_xslvo(this, "o_xslvo", "AXI Slave output interface"),
    o_req_valid(this, "o_req_valid", "1"),
    o_req_addr(this, "o_req_addr", "CFG_SYSBUS_ADDR_BITS"),
    o_req_size(this, "o_req_size", "8"),
    o_req_write(this, "o_req_write", "1"),
    o_req_wdata(this, "o_req_wdata", "CFG_SYSBUS_DATA_BITS"),
    o_req_wstrb(this, "o_req_wstrb", "CFG_SYSBUS_DATA_BYTES"),
    o_req_last(this, "o_req_last", "1"),
    i_req_ready(this, "i_req_ready", "1"),
    i_resp_valid(this, "i_resp_valid", "1"),
    i_resp_rdata(this, "i_resp_rdata", "CFG_SYSBUS_DATA_BITS"),
    i_resp_err(this, "i_resp_err", "1"),
    // params
    State_r_idle(this, "State_r_idle", "5", "0", NO_COMMENT),
    State_r_addr(this, "State_r_addr", "5", "0x1", NO_COMMENT),
    State_r_data(this, "State_r_data", "5", "0x2", NO_COMMENT),
    State_r_last(this, "State_r_last", "5", "0x4", NO_COMMENT),
    State_r_buf(this, "State_r_buf", "5", "0x8", NO_COMMENT),
    State_r_wait_writing(this, "State_r_wait_writing", "5", "0x10", NO_COMMENT),
    State_w_idle(this, "State_w_idle", "7", "0", NO_COMMENT),
    State_w_wait_reading(this, "State_w_wait_reading", "7", "0x1", NO_COMMENT),
    State_w_wait_reading_light(this, "State_w_wait_reading_light", "7", "0x2", NO_COMMENT),
    State_w_req(this, "State_w_req", "7", "0x4", NO_COMMENT), 
    State_w_pipe(this, "State_w_pipe", "7", "0x8", NO_COMMENT), 
    State_w_buf(this, "State_w_buf", "7", "0x10", NO_COMMENT), 
    State_w_resp(this, "State_w_resp", "7", "0x20", NO_COMMENT), 
    State_b(this, "State_b", "7", "0x40", NO_COMMENT),
    // signals
    // registers
    rstate(this, "rstate", "5", "State_r_idle"),
    wstate(this, "wstate", "7", "State_w_idle"),
    ar_ready(this, "ar_ready", "1"),
    ar_addr(this, "ar_addr", "CFG_SYSBUS_ADDR_BITS", "'0", NO_COMMENT),
    ar_len(this, "ar_len", "9", "'0", NO_COMMENT),
    ar_bytes(this, "ar_bytes", "XSIZE_TOTAL", "'0", NO_COMMENT),
    ar_burst(this, "ar_burst", "2", "'0", NO_COMMENT),
    ar_id(this, "ar_id", "CFG_SYSBUS_ID_BITS", "'0", NO_COMMENT),
    ar_user(this, "ar_user", "CFG_SYSBUS_USER_BITS", "'0", NO_COMMENT),
    ar_last(this, "ar_last", "1", RSTVAL_ZERO, NO_COMMENT),
    aw_ready(this, "aw_ready", "1", RSTVAL_ZERO, NO_COMMENT),
    aw_addr(this, "aw_addr", "CFG_SYSBUS_ADDR_BITS", "'0", NO_COMMENT),
    aw_bytes(this, "aw_bytes", "XSIZE_TOTAL", "'0", NO_COMMENT),
    aw_burst(this, "aw_burst", "2", "'0", NO_COMMENT),
    aw_id(this, "aw_id", "CFG_SYSBUS_ID_BITS", "'0", NO_COMMENT),
    aw_user(this, "aw_user", "CFG_SYSBUS_USER_BITS", "'0", NO_COMMENT),
    w_last(this, "w_last", "1", RSTVAL_ZERO, NO_COMMENT),
    w_ready(this, "w_ready", "1", RSTVAL_ZERO, NO_COMMENT),
    r_valid(this, "r_valid", "1", RSTVAL_ZERO, NO_COMMENT),
    r_last(this, "r_last", "1", RSTVAL_ZERO, NO_COMMENT),
    r_data(this, "r_data", "CFG_SYSBUS_DATA_BITS", "'0", NO_COMMENT),
    r_err(this, "r_err", "1", RSTVAL_ZERO, NO_COMMENT),
    r_data_buf(this, "r_data_buf", "CFG_SYSBUS_DATA_BITS", "'0", NO_COMMENT),
    r_err_buf(this, "r_err_buf", "1", RSTVAL_ZERO, NO_COMMENT),
    r_last_buf(this, "r_last_buf", "1", RSTVAL_ZERO, NO_COMMENT),
    b_err(this, "b_err", "1", RSTVAL_ZERO, NO_COMMENT),
    b_valid(this, "b_valid", "1", RSTVAL_ZERO, NO_COMMENT),
    req_valid(this, "req_valid", "1", RSTVAL_ZERO, NO_COMMENT),
    req_addr(this, "req_addr", "CFG_SYSBUS_ADDR_BITS", "'0", NO_COMMENT),
    req_last(this, "req_last", "1", RSTVAL_ZERO, NO_COMMENT),
    req_write(this, "req_write", "1"),
    req_wdata(this, "req_wdata", "CFG_SYSBUS_DATA_BITS", "'0", NO_COMMENT),
    req_wstrb(this, "req_wstrb", "CFG_SYSBUS_DATA_BYTES", "'0", NO_COMMENT),
    req_bytes(this, "req_bytes", "8", "'0", NO_COMMENT),
    req_addr_buf(this, "req_addr_buf", "CFG_SYSBUS_ADDR_BITS", "'0", NO_COMMENT),
    req_last_buf(this, "req_last_buf", "1", RSTVAL_ZERO, NO_COMMENT),
    req_wdata_buf(this, "req_wdata_buf", "CFG_SYSBUS_DATA_BITS", "'0", NO_COMMENT),
    req_wstrb_buf(this, "req_wstrb_buf", "CFG_SYSBUS_DATA_BYTES", "'0", NO_COMMENT),
    // process
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void axi_slv::proc_comb() {
    SETVAL(comb.vcfg.descrsize, *SCV_get_cfg_type(this, "PNP_CFG_DEV_DESCR_BYTES"));
    SETVAL(comb.vcfg.descrtype, *SCV_get_cfg_type(this, "PNP_CFG_TYPE_SLAVE"));
    SETVAL(comb.vcfg.addr_start, i_mapinfo.addr_start);
    SETVAL(comb.vcfg.addr_end, i_mapinfo.addr_end);
    SETVAL(comb.vcfg.vid, vid);
    SETVAL(comb.vcfg.did, did);

    TEXT();
    SETVAL(comb.vb_ar_addr_next, ADD2(BITS(req_addr, 11, 0), CC2(CONST("0", 4), ar_bytes)));
    IF (EQ(ar_burst, *SCV_get_cfg_type(this, "AXI_BURST_FIXED")));
        SETVAL(comb.vb_ar_addr_next, BITS(req_addr, 11, 0));
    ELSIF (EQ(ar_burst, *SCV_get_cfg_type(this, "AXI_BURST_WRAP")));
        TEXT("Wrap suppported only 2, 4, 8 or 16 Bytes. See ARMDeveloper spec.");
        IF (EQ(ar_bytes, CONST("2")));
            SETBITS(comb.vb_ar_addr_next, 11, 1, BITS(req_addr, 11, 1));
        ELSIF(EQ(ar_bytes, CONST("4")));
            SETBITS(comb.vb_ar_addr_next, 11, 2, BITS(req_addr, 11, 2));
        ELSIF(EQ(ar_bytes, CONST("8")));
            SETBITS(comb.vb_ar_addr_next, 11, 3, BITS(req_addr, 11, 3));
        ELSIF(EQ(ar_bytes, CONST("16")));
            SETBITS(comb.vb_ar_addr_next, 11, 4, BITS(req_addr, 11, 4));
        ELSIF(EQ(ar_bytes, CONST("32")));
            TEXT("Optional (not in ARM spec)");
            SETBITS(comb.vb_ar_addr_next, 11, 5, BITS(req_addr, 11, 5));
        ENDIF();
    ENDIF();
    SETVAL(comb.vb_ar_len_next, DEC(ar_len));

    TEXT();
    SETVAL(comb.vb_aw_addr_next, ADD2(BITS(req_addr, 11, 0), CC2(CONST("0", 4), aw_bytes)));
    IF (EQ(aw_burst, *SCV_get_cfg_type(this, "AXI_BURST_FIXED")));
        SETVAL(comb.vb_aw_addr_next, BITS(req_addr, 11, 0));
    ELSIF (EQ(aw_burst, *SCV_get_cfg_type(this, "AXI_BURST_WRAP")));
        TEXT("Wrap suppported only 2, 4, 8 or 16 Bytes. See ARMDeveloper spec.");
        IF (EQ(aw_bytes, CONST("2")));
            SETBITS(comb.vb_aw_addr_next, 11, 1, BITS(req_addr, 11, 1));
        ELSIF(EQ(aw_bytes, CONST("4")));
            SETBITS(comb.vb_aw_addr_next, 11, 2, BITS(req_addr, 11, 2));
        ELSIF(EQ(aw_bytes, CONST("8")));
            SETBITS(comb.vb_aw_addr_next, 11, 3, BITS(req_addr, 11, 3));
        ELSIF(EQ(aw_bytes, CONST("16")));
            SETBITS(comb.vb_aw_addr_next, 11, 4, BITS(req_addr, 11, 4));
        ELSIF(EQ(aw_bytes, CONST("32")));
            TEXT("Optional (not in ARM spec)");
            SETBITS(comb.vb_aw_addr_next, 11, 5, BITS(req_addr, 11, 5));
        ENDIF();
    ENDIF();

    TEXT();
    IF (NZ(AND2_L(i_xslvi.ar_valid, ar_ready)));
        SETZERO(ar_ready);
    ENDIF();
    IF (NZ(AND2_L(i_xslvi.aw_valid, aw_ready)));
        SETZERO(aw_ready);
    ENDIF();
    IF (NZ(AND2_L(i_xslvi.w_valid, w_ready)));
        SETZERO(w_ready);
    ENDIF();
    IF (NZ(AND2_L(i_xslvi.r_ready, r_valid)));
        SETZERO(r_err);
        SETZERO(r_last);
        SETZERO(r_valid);
    ENDIF();
    IF (NZ(AND2_L(i_xslvi.b_ready, b_valid)));
        SETZERO(b_err);
        SETZERO(b_valid);
    ENDIF();
    IF (NZ(AND2_L(req_valid, i_req_ready)));
        SETZERO(req_valid);
    ENDIF();

    TEXT();
    TEXT("Reading channel (write first):");
    SWITCH(rstate);
    CASE(State_r_idle);
        SETVAL(ar_addr, SUB2(i_xslvi.ar_bits.addr, i_mapinfo.addr_start));
        SETVAL(ar_len, ADD2(CC2(CONST("0", 1), i_xslvi.ar_bits.len), CONST("1", 9)));
        SETVAL(ar_burst, i_xslvi.ar_bits.burst);
        CALLF(&ar_bytes, *SCV_get_cfg_type(this, "XSizeToBytes"), 1, &i_xslvi.ar_bits.size);
        SETVAL(ar_last, INV_L(OR_REDUCE(i_xslvi.ar_bits.len)));
        SETVAL(ar_id, i_xslvi.ar_id);
        SETVAL(ar_user, i_xslvi.ar_user);
        IF (AND2(NZ(ar_ready), NZ(i_xslvi.ar_valid)));
            IF (OR2(NZ(i_xslvi.aw_valid), NZ(wstate)));
                SETVAL(rstate, State_r_wait_writing);
            ELSE();
                SETVAL(rstate, State_r_addr);
                SETONE(req_valid);
                SETZERO(req_write);
                SETVAL(req_addr, SUB2(i_xslvi.ar_bits.addr, i_mapinfo.addr_start));
                SETVAL(req_last, INV_L(OR_REDUCE(i_xslvi.ar_bits.len)));
                CALLF(&req_bytes, *SCV_get_cfg_type(this, "XSizeToBytes"), 1, &i_xslvi.ar_bits.size);
            ENDIF();
        ELSE();
            SETONE(ar_ready);
        ENDIF();
    ENDCASE();
    CASE(State_r_addr);
        SETVAL(req_valid, i_xslvi.r_ready);
        IF(AND2(NZ(req_valid), NZ(i_req_ready)));
            IF (GT(ar_len, CONST("0x01", 9)));
                SETVAL(ar_len, DEC(ar_len));
                SETVAL(req_addr, CC2(BITS(req_addr,
                        DEC(*SCV_get_cfg_type(this, "CFG_SYSBUS_ADDR_BITS")), CONST("12")), comb.vb_ar_addr_next));
                SETVAL(req_last, INV_L(OR_REDUCE(BITS(comb.vb_ar_len_next, 8, 1))));
                SETVAL(rstate, State_r_data);
            ELSE();
                SETZERO(req_valid);
                SETZERO(ar_len);
                SETONE(ar_last);
                SETVAL(rstate, State_r_last);
            ENDIF();
        ENDIF();
    ENDCASE();
    CASE(State_r_data);
        SETVAL(req_valid, i_xslvi.r_ready);
        IF(AND2(NZ(req_valid), NZ(i_req_ready)));
            IF (GT(ar_len, CONST("0x01", 9)));
                SETVAL(ar_len, comb.vb_ar_len_next);
                SETVAL(req_addr, CC2(BITS(req_addr,
                        DEC(*SCV_get_cfg_type(this, "CFG_SYSBUS_ADDR_BITS")), CONST("12")), comb.vb_ar_addr_next));
                SETVAL(req_last, INV_L(OR_REDUCE(BITS(comb.vb_ar_len_next, 8, 1))));
            ELSE();
                SETZERO(ar_len);
                SETONE(req_last);
            ENDIF();
        ENDIF();
        IF (NZ(AND3_L(req_valid, req_last, i_req_ready)));
            SETVAL(rstate, State_r_last);
            SETZERO(req_valid);
        ENDIF();
        IF (NZ(i_resp_valid));
            IF (AND2(NZ(r_valid), EZ(i_xslvi.r_ready)));
                TEXT("We already requested the last value but previous was not accepted yet");
                SETVAL(r_data_buf, i_resp_rdata);
                SETVAL(r_err_buf, i_resp_err);
                SETVAL(r_last_buf, AND3_L(req_valid, req_last, i_req_ready));
                SETVAL(rstate, State_r_buf);
            ELSE();
                SETONE(r_valid);
                SETZERO(r_last);
                SETVAL(r_data, i_resp_rdata);
                SETVAL(r_err, i_resp_err);
            ENDIF();
        ENDIF();
    ENDCASE();
    CASE(State_r_last);
        IF (NZ(i_resp_valid));
            IF (AND2(NZ(r_valid), NZ(r_last)));
                TEXT("Ingore this response, because it means i_resp_valid is always=1");
            ELSIF (AND2(NZ(r_valid), EZ(i_xslvi.r_ready)));
                TEXT("We already requested the last value but previous (not last) was not accepted yet");
                SETVAL(r_data_buf, i_resp_rdata);
                SETVAL(r_err_buf, i_resp_err);
                SETONE(r_last_buf);
                SETVAL(rstate, State_r_buf);
            ELSE();
                SETONE(r_valid);
                SETONE(r_last);
                SETVAL(r_data, i_resp_rdata);
                SETVAL(r_err, i_resp_err);
            ENDIF();
        ENDIF();
        IF (AND3(NZ(r_valid), NZ(r_last), NZ(i_xslvi.r_ready)));
            SETONE(ar_ready);
            SETZERO(r_last);
            SETZERO(r_valid, "We need it in a case of i_resp_valid is always HIGH");
            SETVAL(rstate, State_r_idle);
        ENDIF();
    ENDCASE();
    CASE(State_r_buf);
        IF (NZ(i_xslvi.r_ready));
            SETONE(r_valid);
            SETVAL(r_last, r_last_buf);
            SETVAL(r_data, r_data_buf);
            SETVAL(r_err, r_err_buf);
            IF (NZ(r_last_buf));
                SETVAL(rstate, State_r_last);
            ELSE();
                SETVAL(rstate, State_r_data);
            ENDIF();
        ENDIF();
    ENDCASE();
    CASE(State_r_wait_writing);
        IF (ORx(2, &AND2(EZ(wstate), EZ(i_xslvi.aw_valid)),
                   &NZ(AND3_L(req_valid, req_last, i_req_ready))));
            TEXT("End of writing, start reading");
            SETONE(req_valid);
            SETZERO(req_write);
            SETVAL(req_addr, ar_addr);
            SETVAL(req_bytes, ar_bytes);
            SETVAL(req_last, ar_last);
            SETVAL(rstate, State_r_addr);
        ENDIF();
    ENDCASE();
    CASEDEF();
        SETVAL(rstate, State_r_idle);
    ENDCASE();
    ENDSWITCH();

    TEXT();
    TEXT("Writing channel:");
    SWITCH(wstate);
    CASE(State_w_idle);
        SETONE(w_ready);
        SETVAL(aw_addr, SUB2(i_xslvi.aw_bits.addr, i_mapinfo.addr_start));
        SETVAL(aw_burst, i_xslvi.aw_bits.burst);
        CALLF(&aw_bytes, *SCV_get_cfg_type(this, "XSizeToBytes"), 1, &i_xslvi.aw_bits.size);
        SETVAL(w_last, INV_L(OR_REDUCE(i_xslvi.aw_bits.len)));
        SETVAL(aw_id, i_xslvi.aw_id);
        SETVAL(aw_user, i_xslvi.aw_user);
        IF (AND2(NZ(aw_ready), NZ(i_xslvi.aw_valid)));
            SETVAL(req_wdata, i_xslvi.w_data);
            SETVAL(req_wstrb, i_xslvi.w_strb);
            IF (AND2(NZ(w_ready), NZ(i_xslvi.w_valid)));
                TEXT("AXI Light support:");
                SETVAL(wstate, State_w_pipe);
                SETVAL(w_last, i_xslvi.w_last);
                IF (NZ(rstate));
                    TEXT("Postpone writing");
                    SETZERO(w_ready);
                    SETVAL(wstate, State_w_wait_reading_light);
                ELSE();
                    TEXT("Start writing now");
                    SETVAL(req_addr, SUB2(i_xslvi.aw_bits.addr, i_mapinfo.addr_start));
                    CALLF(&req_bytes, *SCV_get_cfg_type(this, "XSizeToBytes"), 1, &i_xslvi.aw_bits.size);
                    SETVAL(req_last, i_xslvi.w_last);
                    SETONE(req_write);
                    SETONE(req_valid);
                    SETVAL(w_ready, i_req_ready);
                ENDIF();
            ELSIF(NZ(rstate));
                SETVAL(wstate, State_w_wait_reading);
                SETZERO(w_ready);
            ELSE();
                SETVAL(req_addr, SUB2(i_xslvi.aw_bits.addr, i_mapinfo.addr_start));
                CALLF(&req_bytes, *SCV_get_cfg_type(this, "XSizeToBytes"), 1, &i_xslvi.aw_bits.size);
                SETVAL(wstate, State_w_req);
                SETONE(w_ready);
                SETONE(req_write);
            ENDIF();
        ELSE();
            SETONE(aw_ready);
        ENDIF();
    ENDCASE();
    CASE(State_w_req);
        IF(NZ(i_xslvi.w_valid));
            SETVAL(w_ready, AND2_L(i_req_ready, INV_L(i_xslvi.w_last)));
            SETONE(req_valid);
            SETVAL(req_wdata, i_xslvi.w_data);
            SETVAL(req_wstrb, i_xslvi.w_strb);
            SETVAL(req_last, i_xslvi.w_last);
            SETVAL(wstate, State_w_pipe);
        ENDIF();
    ENDCASE();
    CASE(State_w_pipe);
        SETVAL(w_ready, AND2_L(OR2_L(i_req_ready, i_resp_valid), INV_L(req_last)));
        IF (AND2(NZ(w_ready), NZ(i_xslvi.w_valid)));
            IF (EZ(i_req_ready));
                SETVAL(wstate, State_w_buf);
                SETZERO(w_ready);
                SETVAL(req_addr_buf, CC2(BITS(req_addr,
                        DEC(*SCV_get_cfg_type(this, "CFG_SYSBUS_ADDR_BITS")), CONST("12")), comb.vb_aw_addr_next));
                SETVAL(req_wdata_buf, i_xslvi.w_data);
                SETVAL(req_wstrb_buf, i_xslvi.w_strb);
                SETVAL(req_last_buf, i_xslvi.w_last);
            ELSE();
                SETONE(req_valid);
                SETVAL(req_addr, CC2(BITS(req_addr,
                        DEC(*SCV_get_cfg_type(this, "CFG_SYSBUS_ADDR_BITS")), CONST("12")), comb.vb_aw_addr_next));
                SETVAL(req_wdata, i_xslvi.w_data);
                SETVAL(req_wstrb, i_xslvi.w_strb);
                SETVAL(req_last, i_xslvi.w_last);
            ENDIF();
        ENDIF();
        IF (AND3(NZ(req_valid), NZ(req_last), NZ(i_req_ready)));
            SETZERO(req_last);
            SETVAL(wstate, State_w_resp);
        ENDIF();
        IF (AND3(NZ(i_resp_valid), EZ(i_xslvi.w_valid), EZ(req_valid)));
            SETONE(w_ready);
            SETVAL(req_addr, CC2(BITS(req_addr,
                    DEC(*SCV_get_cfg_type(this, "CFG_SYSBUS_ADDR_BITS")), CONST("12")), comb.vb_aw_addr_next));
            SETVAL(wstate, State_w_req);
        ENDIF();
    ENDCASE();
    CASE(State_w_buf);
        IF (NZ(i_req_ready));
            SETONE(req_valid);
            SETVAL(req_last, req_last_buf);
            SETVAL(req_addr, req_addr_buf);
            SETVAL(req_wdata, req_wdata_buf);
            SETVAL(req_wstrb, req_wstrb_buf);
            SETVAL(wstate, State_w_pipe);
        ENDIF();
    ENDCASE();
    CASE(State_w_resp);
        IF (NZ(i_resp_valid));
            SETONE(b_valid);
            SETVAL(b_err, i_resp_err);
            SETZERO(w_last);
            SETVAL(wstate, State_b);
        ENDIF();
    ENDCASE();
    CASE(State_w_wait_reading);
        TEXT("ready to accept new data (no latched data)");
        IF (OR2(EZ(rstate), NZ(AND3_L(r_valid, r_last, i_xslvi.r_ready))));
            SETONE(w_ready);
            SETONE(req_write);
            SETVAL(req_addr, aw_addr);
            SETVAL(req_bytes, aw_bytes);
            SETVAL(wstate, State_w_req);
        ENDIF();
    ENDCASE();
    CASE(State_w_wait_reading_light);
        TEXT("Not ready to accept new data before writing the last one");
        IF (OR2(EZ(rstate), NZ(AND3_L(r_valid, r_last, i_xslvi.r_ready))));
            SETONE(req_valid);
            SETONE(req_write);
            SETVAL(req_addr, aw_addr);
            SETVAL(req_bytes, aw_bytes);
            SETVAL(req_last, w_last);
            SETVAL(wstate, State_w_pipe);
        ENDIF();
    ENDCASE();
    CASE(State_b);
        IF (AND2(NZ(b_valid), NZ(i_xslvi.b_ready)));
            SETZERO(b_valid);
            SETZERO(b_err);
            SETONE(aw_ready);
            SETONE(w_ready, "AXI light");
            SETVAL(wstate, State_w_idle);
        ENDIF();
    ENDCASE();
    CASEDEF();
        SETVAL(wstate, State_w_idle);
    ENDCASE();
    ENDSWITCH();


TEXT();
    SYNC_RESET();

TEXT();
    SETVAL(o_req_valid, req_valid);
    SETVAL(o_req_last, req_last);
    SETVAL(o_req_addr, req_addr);
    SETVAL(o_req_size, req_bytes);
    SETVAL(o_req_write, req_write);
    SETVAL(o_req_wdata, req_wdata);
    SETVAL(o_req_wstrb, req_wstrb);

TEXT();
    SETVAL(comb.vxslvo.ar_ready, ar_ready);
    SETVAL(comb.vxslvo.r_valid, r_valid);
    SETVAL(comb.vxslvo.r_id, ar_id);
    SETVAL(comb.vxslvo.r_user, ar_user);
    SETVAL(comb.vxslvo.r_resp, CC2(r_err, CONST("0", 1)));
    SETVAL(comb.vxslvo.r_data, r_data);
    SETVAL(comb.vxslvo.r_last, r_last);
    SETVAL(comb.vxslvo.aw_ready, aw_ready);
    SETVAL(comb.vxslvo.w_ready, w_ready);
    SETVAL(comb.vxslvo.b_valid, b_valid);
    SETVAL(comb.vxslvo.b_id, aw_id);
    SETVAL(comb.vxslvo.b_user, aw_user);
    SETVAL(comb.vxslvo.b_resp, CC2(b_err, CONST("0", 1)));
    SETVAL(o_xslvo, comb.vxslvo);
    SETVAL(o_cfg, comb.vcfg);
}
