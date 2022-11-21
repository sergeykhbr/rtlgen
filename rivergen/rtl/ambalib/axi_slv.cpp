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

axi_slv::axi_slv(GenObject *parent, const char *name) :
    ModuleObject(parent, "axi_slv", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_xslvi(this, "i_xslvi", "AXI Slave input interface"),
    o_xslvo(this, "o_xslvo", "AXI Slave output interface"),
    o_req_valid(this, "o_req_valid", "1"),
    o_req_addr(this, "o_req_addr", "CFG_SYSBUS_ADDR_BITS"),
    o_req_write(this, "o_req_write", "1"),
    o_req_wdata(this, "o_req_wdata", "CFG_SYSBUS_DATA_BITS"),
    o_req_burst(this, "o_req_burst", "1"),
    o_req_last(this, "o_req_last", "1"),
    i_resp_valid(this, "i_resp_valid", "1"),
    i_resp_rdata(this, "i_resp_rdata", "CFG_SYSBUS_DATA_BITS"),
    i_resp_err(this, "i_resp_err", "1"),
    // params
    State_Idle(this, "3", "State_Idle", "0"),
    State_w(this, "3", "State_w", "1"),
    State_burst_w(this, "3", "State_burst_w", "2"),
    State_last_w(this, "3", "State_last_w", "3"),
    State_burst_r(this, "3", "State_burst_r", "4"),
    State_last_r(this, "3", "State_last_r", "5"),
    State_b(this, "3", "State_b", "6"),
    // signals
    // registers
    state(this, "state", "3", "State_Idle"),
    req_valid(this, "req_valid", "1"),
    req_addr(this, "req_addr", "CFG_SYSBUS_ADDR_BITS"),
    req_write(this, "req_write", "1"),
    req_wdata(this, "req_wdata", "CFG_SYSBUS_DATA_BITS"),
    req_wstrb(this, "req_wstrb", "CFG_SYSBUS_DATA_BYTES"),
    req_xsize(this, "req_xsize", "8"),
    req_len(this, "req_len", "8"),
    req_user(this, "req_user", "CFG_SYSBUS_USER_BITS"),
    req_burst(this, "req_burst", "1"),
    req_last(this, "req_last", "1"),
    resp_valid(this, "resp_valid", "1"),
    resp_rdata(this, "resp_rdata", "CFG_SYSBUS_DATA_BITS"),
    resp_err(this, "resp_err", "1"),
    // process
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void axi_slv::proc_comb() {
    types_amba *glb = glob_types_amba_;
    SETZERO(req_valid);
    SETZERO(req_last);
    SETVAL(comb.vb_req_addr_next, ADD2(BITS(req_addr, 11, 0), req_xsize));

TEXT();
    SWITCH(state);
    CASE(State_Idle);
        SETZERO(req_burst);
        SETZERO(resp_valid);
        SETZERO(resp_err);
        IF (NZ(i_xslvi.aw_valid));
            SETONE(req_write);
            SETVAL(req_addr, i_xslvi.aw_bits.addr);
            CALLF(&req_xsize, glb->XSizeToBytes, 1, &i_xslvi.aw_bits.size);
            SETVAL(req_len, i_xslvi.aw_bits.len);
            SETVAL(req_user, i_xslvi.aw_user);
            SETVAL(req_wdata, i_xslvi.w_data, "AXI Lite compatible");
            SETVAL(req_wstrb, i_xslvi.w_strb);
            IF (NZ(i_xslvi.w_valid));
                TEXT("AXI Lite does not support burst transaction");
                SETVAL(state, State_last_w);
                SETONE(req_valid);
                SETONE(req_last);
            ELSE();
                SETVAL(state, State_w);
            ENDIF();
        ELSIF (NZ(i_xslvi.ar_valid));
            SETONE(req_valid);
            SETZERO(req_write);
            SETVAL(req_addr, i_xslvi.ar_bits.addr);
            CALLF(&req_xsize, glob_types_amba_->XSizeToBytes, 1, &i_xslvi.ar_bits.size);
            SETVAL(req_len, i_xslvi.ar_bits.len);
            SETVAL(req_user, i_xslvi.ar_user);
            IF(NZ(i_xslvi.ar_bits.len));
                SETVAL(state, State_burst_r);
                SETONE(req_burst);
            ELSE();
                SETVAL(state, State_last_r);
                SETONE(req_last);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(State_w);
        SETVAL(req_wdata, i_xslvi.w_data);
        SETVAL(req_wstrb, i_xslvi.w_strb);
        IF(NZ(i_xslvi.w_valid));
            SETONE(req_valid);
            IF(NZ(req_len));
                SETVAL(state, State_burst_w);
                SETONE(req_burst);
            ELSE();
                SETVAL(state, State_last_w);
                SETONE(req_last);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(State_burst_w);
        IF (NZ(i_xslvi.w_valid));
            SETONE(req_valid);
            SETVAL(req_addr, CC2(BITS(req_addr, DEC(glb->CFG_SYSBUS_ADDR_BITS), CONST("12")),
                                 comb.vb_req_addr_next));
            SETVAL(req_wdata, i_xslvi.w_data);
            SETVAL(req_wstrb, i_xslvi.w_strb);
            IF (NZ(req_len));
                SETVAL(req_len, DEC(req_len));
            ENDIF();
            IF (EQ(req_len, CONST("1", 8)));
                SETVAL(state, State_last_w);
                SETONE(req_last);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (State_last_w);
        SETVAL(resp_valid, i_resp_valid);
        IF (NZ(i_resp_valid));
            SETVAL(resp_err, i_resp_err);
            SETVAL(state, State_b);
        ENDIF();
        ENDCASE();
    CASE (State_b);
        IF (NZ(i_xslvi.b_ready));
            SETVAL(state, State_Idle);
        ENDIF();
        ENDCASE();
    CASE (State_burst_r);
        SETONE(req_valid);
        IF (NZ(i_xslvi.r_ready));
            SETVAL(req_addr, CC2(BITS(req_addr, DEC(glb->CFG_SYSBUS_ADDR_BITS), CONST("12")),
                                 comb.vb_req_addr_next));
            IF (NZ(req_len));
                SETVAL(req_len, DEC(req_len));
            ENDIF();
            IF (EQ(req_len, CONST("1", 8)));
                SETVAL(state, State_last_w);
                SETONE(req_last);
            ENDIF();
        ENDIF();
        IF (NZ(i_resp_valid));
            SETVAL(resp_rdata, i_resp_rdata);
            SETVAL(resp_err, i_resp_err);
            SETVAL(state, State_b);
        ENDIF();
        ENDCASE();
    CASE (State_last_r);
        IF (NZ(i_xslvi.r_ready));
            SETVAL(state, State_Idle);
            SETZERO(resp_valid);
        ENDIF();
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_req_valid, req_valid);
    SETVAL(o_req_addr, req_addr);
    SETVAL(o_req_write, req_write);
    SETVAL(o_req_wdata, req_wdata);

TEXT();
    SETVAL(comb.vapbo.pready, resp_valid);
    SETVAL(comb.vapbo.prdata, resp_rdata);
    SETVAL(comb.vapbo.pslverr, resp_err);
    //SETVAL(o_apbo, comb.vapbo);
}
