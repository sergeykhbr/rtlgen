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
    State_Idle(this, "State_Idle", "3", "0", NO_COMMENT),
    State_w(this, "State_w", "3", "1", NO_COMMENT), 
    State_burst_w(this, "State_burst_w", "3", "2", NO_COMMENT),
    State_addr_r(this, "State_addr_r", "3", "3", NO_COMMENT),
    State_data_r(this, "State_data_r", "3", "4", NO_COMMENT),
    State_out_r(this, "State_out_r", "3", "5", NO_COMMENT),
    State_b(this, "State_b", "3", "6", NO_COMMENT),
    // signals
    // registers
    state(this, "state", "3", "State_Idle"),
    req_valid(this, "req_valid", "1"),
    req_addr(this, "req_addr", "CFG_SYSBUS_ADDR_BITS", "'0", NO_COMMENT),
    req_write(this, "req_write", "1"),
    req_wdata(this, "req_wdata", "CFG_SYSBUS_DATA_BITS", "'0", NO_COMMENT),
    req_wstrb(this, "req_wstrb", "CFG_SYSBUS_DATA_BYTES", "'0", NO_COMMENT),
    req_xsize(this, "req_xsize", "8", "'0", NO_COMMENT),
    req_len(this, "req_len", "8", "'0", NO_COMMENT),
    req_user(this, "req_user", "CFG_SYSBUS_USER_BITS", "'0", NO_COMMENT),
    req_id(this, "req_id", "CFG_SYSBUS_ID_BITS", "'0", NO_COMMENT),
    req_burst(this, "req_burst", "2", "'0", NO_COMMENT),
    req_last_a(this, "req_last_a", "1"),
    req_last_r(this, "req_last_r", "1"),
    req_done(this, "req_done", "1"),
    resp_valid(this, "resp_valid", "1"),
    resp_last(this, "resp_last", "1"),
    resp_rdata(this, "resp_rdata", "CFG_SYSBUS_DATA_BITS", "'0", NO_COMMENT),
    resp_err(this, "resp_err", "1"),
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
    SETVAL(comb.vb_req_addr_next, ADD2(BITS(req_addr, 11, 0), req_xsize));
    IF (EQ(req_burst, *SCV_get_cfg_type(this, "AXI_BURST_FIXED")));
        SETVAL(comb.vb_req_addr_next, BITS(req_addr, 11, 0));
    ELSIF (EQ(req_burst, *SCV_get_cfg_type(this, "AXI_BURST_WRAP")));
        TEXT("Wrap suppported only 2, 4, 8 or 16 Bytes. See ARMDeveloper spec.");
        IF (EQ(req_xsize, CONST("2")));
            SETBITS(comb.vb_req_addr_next, 11, 1, BITS(req_addr, 11, 1));
        ELSIF(EQ(req_xsize, CONST("4")));
            SETBITS(comb.vb_req_addr_next, 11, 2, BITS(req_addr, 11, 2));
        ELSIF(EQ(req_xsize, CONST("8")));
            SETBITS(comb.vb_req_addr_next, 11, 3, BITS(req_addr, 11, 3));
        ELSIF(EQ(req_xsize, CONST("16")));
            SETBITS(comb.vb_req_addr_next, 11, 4, BITS(req_addr, 11, 4));
        ELSIF(EQ(req_xsize, CONST("32")));
            TEXT("Optional (not in ARM spec)");
            SETBITS(comb.vb_req_addr_next, 11, 5, BITS(req_addr, 11, 5));
        ENDIF();
    ENDIF();

TEXT();
    SETVAL(comb.vb_req_len_next, req_len);
    IF (NZ(req_len));
        SETVAL(comb.vb_req_len_next, DEC(req_len));
    ENDIF();

TEXT();
    SWITCH(state);
    CASE(State_Idle);
        SETZERO(req_done);
        SETZERO(req_valid);
        SETZERO(req_write);
        SETZERO(resp_valid);
        SETZERO(resp_last);
        SETZERO(resp_err);
        SETONE(comb.vxslvo.aw_ready);
        SETONE(comb.vxslvo.w_ready, "No burst AXILite ready");
        SETVAL(comb.vxslvo.ar_ready, INV(i_xslvi.aw_valid));
        IF (NZ(i_xslvi.aw_valid));
            SETVAL(req_addr, SUB2(i_xslvi.aw_bits.addr, i_mapinfo.addr_start));
            CALLF(&req_xsize, *SCV_get_cfg_type(this, "XSizeToBytes"), 1, &i_xslvi.aw_bits.size);
            SETVAL(req_len, i_xslvi.aw_bits.len);
            SETVAL(req_burst, i_xslvi.aw_bits.burst);
            SETVAL(req_id, i_xslvi.aw_id);
            SETVAL(req_user, i_xslvi.aw_user);
            SETVAL(req_wdata, i_xslvi.w_data, "AXI Lite compatible");
            SETVAL(req_wstrb, i_xslvi.w_strb);
            IF (NZ(i_xslvi.w_valid));
                TEXT("AXI Lite does not support burst transaction");
                SETVAL(state, State_burst_w);
                SETONE(req_valid);
                SETONE(req_write);
                SETVAL(req_last_a, INV(OR_REDUCE(i_xslvi.aw_bits.len)));
            ELSE();
                SETVAL(state, State_w);
            ENDIF();
        ELSIF (NZ(i_xslvi.ar_valid));
            SETONE(req_valid);
            SETVAL(req_last_a, INV(OR_REDUCE(i_xslvi.ar_bits.len)));
            SETVAL(req_addr, SUB2(i_xslvi.ar_bits.addr, i_mapinfo.addr_start));
            CALLF(&req_xsize, glob_types_amba_->XSizeToBytes, 1, &i_xslvi.ar_bits.size);
            SETVAL(req_len, i_xslvi.ar_bits.len);
            SETVAL(req_burst, i_xslvi.ar_bits.burst);
            SETVAL(req_id, i_xslvi.ar_id);
            SETVAL(req_user, i_xslvi.ar_user);
            SETVAL(state, State_addr_r);
        ENDIF();
        ENDCASE();
    CASE(State_w);
        SETONE(comb.vxslvo.w_ready);
        SETVAL(req_wdata, i_xslvi.w_data);
        SETVAL(req_wstrb, i_xslvi.w_strb);
        IF(NZ(i_xslvi.w_valid));
            SETONE(req_valid);
            SETONE(req_write);
            SETVAL(req_last_a, INV(OR_REDUCE(comb.vb_req_len_next)));
            SETVAL(state, State_burst_w);
        ENDIF();
        ENDCASE();
    CASE(State_burst_w);
        SETVAL(comb.vxslvo.w_ready, i_req_ready);
        IF (NZ(i_xslvi.w_valid));
            SETONE(req_valid);
            SETVAL(req_wdata, i_xslvi.w_data);
            SETVAL(req_wstrb, i_xslvi.w_strb);
        ELSIF(NZ(i_req_ready));
            SETZERO(req_valid);
        ENDIF();
        IF (AND2(NZ(req_valid), NZ(i_req_ready)));
            SETONE(req_done);
            SETVAL(req_addr, CC2(BITS(req_addr, DEC(*SCV_get_cfg_type(this, "CFG_SYSBUS_ADDR_BITS")), CONST("12")),
                                 comb.vb_req_addr_next));
            SETVAL(req_last_a, INV(OR_REDUCE(comb.vb_req_len_next)));
            IF (NZ(req_len));
                SETVAL(req_len, DEC(req_len));
            ELSE();
                SETZERO(req_write);
                SETZERO(req_last_a);
                SETVAL(state, State_b);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (State_b);
        SETVAL(comb.vxslvo.b_valid, i_resp_valid);
        IF (AND2(NZ(i_xslvi.b_ready), NZ(i_resp_valid)));
            SETZERO(req_done);
            SETVAL(state, State_Idle);
        ENDIF();
        ENDCASE();
    CASE (State_addr_r);
        TEXT("Setup address:");
        IF (NZ(i_req_ready));
            SETVAL(state, State_data_r);
            SETVAL(req_addr, CC2(BITS(req_addr, DEC(*SCV_get_cfg_type(this, "CFG_SYSBUS_ADDR_BITS")), CONST("12")),
                                    comb.vb_req_addr_next));
            SETVAL(req_len, comb.vb_req_len_next);
            SETVAL(req_last_a, INV(OR_REDUCE(comb.vb_req_len_next)));
            SETVAL(req_last_r, req_last_a);
            SETVAL(req_valid, OR_REDUCE(req_len));
            SETONE(req_done);
        ENDIF();
        ENDCASE();
    CASE (State_data_r);
        SETVAL(req_valid, AND2_L(INV(req_last_r), i_xslvi.r_ready));
        IF (AND2(NZ(i_resp_valid), NZ(req_done)));
            SETZERO(req_done);
            SETONE(resp_valid);
            SETVAL(resp_last, req_last_r);
            SETVAL(resp_rdata, i_resp_rdata);
            SETVAL(resp_err, i_resp_err);
            IF (NZ(req_last_r));
                SETVAL(state, State_out_r);
            ENDIF();
        ELSIF(NZ(i_xslvi.r_ready));
            SETZERO(resp_valid);
        ENDIF();
        IF (AND2(NZ(req_valid), NZ(i_req_ready)));
            SETVAL(req_addr, CC2(BITS(req_addr, DEC(*SCV_get_cfg_type(this, "CFG_SYSBUS_ADDR_BITS")), CONST("12")),
                                    comb.vb_req_addr_next));
            SETVAL(req_len, comb.vb_req_len_next);
            SETVAL(req_last_a, INV(OR_REDUCE(comb.vb_req_len_next)));
            SETVAL(req_last_r, req_last_a);
            SETVAL(req_valid, AND2_L(INV(req_last_a), i_xslvi.r_ready));
            SETONE(req_done);
        ENDIF();
        ENDCASE();
    CASE (State_out_r);
        IF (NZ(i_xslvi.r_ready));
            SETZERO(req_last_a);
            SETZERO(req_last_r);
            SETZERO(resp_last);
            SETZERO(resp_valid);
            SETVAL(state, State_Idle);
        ENDIF();
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    SYNC_RESET();

TEXT();
    SETVAL(o_req_valid, req_valid);
    SETVAL(o_req_last, req_last_a);
    SETVAL(o_req_addr, req_addr);
    SETVAL(o_req_size, req_xsize);
    SETVAL(o_req_write, req_write);
    SETVAL(o_req_wdata, req_wdata);
    SETVAL(o_req_wstrb, req_wstrb);

TEXT();
    SETVAL(comb.vxslvo.b_id, req_id);
    SETVAL(comb.vxslvo.b_user, req_user);
    SETVAL(comb.vxslvo.b_resp, CC2(i_resp_err, CONST("0", 1)));
    SETVAL(comb.vxslvo.r_valid, resp_valid);
    SETVAL(comb.vxslvo.r_id, req_id);
    SETVAL(comb.vxslvo.r_user, req_user);
    SETVAL(comb.vxslvo.r_resp, CC2(resp_err, CONST("0", 1)));
    SETVAL(comb.vxslvo.r_data, resp_rdata);
    SETVAL(comb.vxslvo.r_last, resp_last);
    SETVAL(o_xslvo, comb.vxslvo);
    SETVAL(o_cfg, comb.vcfg);
}
