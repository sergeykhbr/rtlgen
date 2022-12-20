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
    State_Idle(this, "4", "State_Idle", "0"),
    State_w(this, "4", "State_w", "1"), 
    State_burst_w(this, "4", "State_burst_w", "2"),
    State_last_w(this, "4", "State_last_w", "3"),
    State_addr_r(this, "4", "State_addr_r", "4"),
    State_addrdata_r(this, "4", "State_addrdata_r", "5"),
    State_data_r(this, "4", "State_data_r", "6"),
    State_out_r(this, "4", "State_out_r", "7"),
    State_b(this, "4", "State_b", "8"),
    // signals
    // registers
    state(this, "state", "4", "State_Idle"),
    req_valid(this, "req_valid", "1"),
    req_addr(this, "req_addr", "CFG_SYSBUS_ADDR_BITS"),
    req_write(this, "req_write", "1"),
    req_wdata(this, "req_wdata", "CFG_SYSBUS_DATA_BITS"),
    req_wstrb(this, "req_wstrb", "CFG_SYSBUS_DATA_BYTES"),
    req_xsize(this, "req_xsize", "8"),
    req_len(this, "req_len", "8"),
    req_user(this, "req_user", "CFG_SYSBUS_USER_BITS"),
    req_id(this, "req_id", "CFG_SYSBUS_ID_BITS"),
    req_burst(this, "req_burst", "2"),
    req_last(this, "req_last", "1"),
    resp_valid(this, "resp_valid", "1"),
    resp_last(this, "resp_last", "1"),
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
    SETVAL(comb.vcfg.descrsize, glb->PNP_CFG_DEV_DESCR_BYTES);
    SETVAL(comb.vcfg.descrtype, glb->PNP_CFG_TYPE_SLAVE);
    SETVAL(comb.vcfg.addr_start, i_mapinfo.addr_start);
    SETVAL(comb.vcfg.addr_end, i_mapinfo.addr_end);
    SETVAL(comb.vcfg.vid, vid);
    SETVAL(comb.vcfg.did, did);

TEXT();
    SETVAL(comb.vb_req_addr_next, ADD2(BITS(req_addr, 11, 0), req_xsize));
    IF (EQ(req_burst, glb->AXI_BURST_FIXED));
        SETVAL(comb.vb_req_addr_next, BITS(req_addr, 11, 0));
    ELSIF (EQ(req_burst, glb->AXI_BURST_WRAP));
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
    SETVAL(comb.v_req_last, INV(OR_REDUCE(req_len)));
    SETVAL(req_last, comb.v_req_last);

TEXT();
    SWITCH(state);
    CASE(State_Idle);
        SETZERO(req_valid);
        SETZERO(req_write);
        SETZERO(resp_valid);
        SETZERO(resp_last);
        SETZERO(resp_err);
        SETONE(comb.vxslvo.aw_ready);
        SETONE(comb.vxslvo.w_ready, "No burst AXILite ready");
        SETVAL(comb.vxslvo.ar_ready, INV(i_xslvi.aw_valid));
        IF (NZ(i_xslvi.aw_valid));
            SETVAL(req_addr, i_xslvi.aw_bits.addr);
            CALLF(&req_xsize, glb->XSizeToBytes, 1, &i_xslvi.aw_bits.size);
            SETVAL(req_len, i_xslvi.aw_bits.len);
            SETVAL(req_burst, i_xslvi.aw_bits.burst);
            SETVAL(req_id, i_xslvi.aw_id);
            SETVAL(req_user, i_xslvi.aw_user);
            SETVAL(req_wdata, i_xslvi.w_data, "AXI Lite compatible");
            SETVAL(req_wstrb, i_xslvi.w_strb);
            IF (NZ(i_xslvi.w_valid));
                TEXT("AXI Lite does not support burst transaction");
                SETVAL(state, State_last_w);
                SETONE(req_valid);
                SETONE(req_write);
            ELSE();
                SETVAL(state, State_w);
            ENDIF();
        ELSIF (NZ(i_xslvi.ar_valid));
            SETONE(req_valid);
            SETVAL(req_addr, i_xslvi.ar_bits.addr);
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
            IF(NZ(req_len));
                SETVAL(state, State_burst_w);
            ELSE();
                SETVAL(state, State_last_w);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(State_burst_w);
        SETVAL(req_valid, i_xslvi.w_valid);
        SETVAL(comb.vxslvo.w_ready, i_resp_valid);
        IF (AND2(NZ(i_xslvi.w_valid), NZ(i_resp_valid)));
            SETVAL(req_addr, CC2(BITS(req_addr, DEC(glb->CFG_SYSBUS_ADDR_BITS), CONST("12")),
                                 comb.vb_req_addr_next));
            SETVAL(req_wdata, i_xslvi.w_data);
            SETVAL(req_wstrb, i_xslvi.w_strb);
            IF (NZ(req_len));
                SETVAL(req_len, DEC(req_len));
            ENDIF();
            IF (EQ(req_len, CONST("1", 8)));
                SETVAL(state, State_last_w);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (State_last_w);
        TEXT("Wait cycle: w_ready is zero on the last write because it is laready accepted");
        IF (NZ(i_resp_valid));
            SETZERO(req_valid);
            SETZERO(req_write);
            SETVAL(resp_err, i_resp_err);
            SETVAL(state, State_b);
        ENDIF();
        ENDCASE();
    CASE (State_b);
        SETONE(comb.vxslvo.b_valid);
        IF (NZ(i_xslvi.b_ready));
            SETVAL(state, State_Idle);
        ENDIF();
        ENDCASE();
    CASE (State_addr_r);
        TEXT("Setup address:");
        IF (NZ(i_req_ready));
            IF (NZ(req_len));
                SETVAL(req_addr, CC2(BITS(req_addr, DEC(glb->CFG_SYSBUS_ADDR_BITS), CONST("12")),
                                        comb.vb_req_addr_next));
                SETVAL(req_len, DEC(req_len));
                SETVAL(state, State_addrdata_r);
            ELSE();
                SETZERO(req_valid);
                SETVAL(state, State_data_r);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (State_addrdata_r);
        SETVAL(resp_valid, i_resp_valid);
        SETVAL(resp_rdata, i_resp_rdata);
        SETVAL(resp_err, i_resp_err);
        IF (OR2(EZ(i_resp_valid), EZ(req_len)));
            SETZERO(req_valid);
            SETVAL(state, State_data_r);
        ELSIF (EZ(i_xslvi.r_ready));
            TEXT("Bus is not ready to accept read data");
            SETZERO(req_valid);
            SETVAL(state, State_out_r);
        ELSIF(EZ(i_req_ready));
            TEXT("Slave device is not ready to accept burst request");
            SETVAL(state, State_addr_r);
        ELSE();
            SETVAL(req_addr, CC2(BITS(req_addr, DEC(glb->CFG_SYSBUS_ADDR_BITS), CONST("12")),
                                    comb.vb_req_addr_next));
            SETVAL(req_len, DEC(req_len));
        ENDIF();
        ENDCASE();
    CASE (State_data_r);
        IF (NZ(i_resp_valid));
            SETONE(resp_valid);
            SETVAL(resp_rdata, i_resp_rdata);
            SETVAL(resp_err, i_resp_err);
            SETVAL(resp_last, INV(OR_REDUCE(req_len)));
            SETVAL(state, State_out_r);
        ENDIF();
        ENDCASE();
    CASE (State_out_r);
        IF (NZ(i_xslvi.r_ready));
            SETZERO(resp_valid);
            SETZERO(resp_last);
            IF (NZ(req_len));
                SETONE(req_valid);
                SETVAL(state, State_addr_r);
            ELSE();
                SETVAL(state, State_Idle);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_req_valid, req_valid);
    SETVAL(o_req_last, comb.v_req_last);
    SETVAL(o_req_addr, req_addr);
    SETVAL(o_req_size, req_xsize);
    SETVAL(o_req_write, req_write);
    SETVAL(o_req_wdata, req_wdata);
    SETVAL(o_req_wstrb, req_wstrb);

TEXT();
    SETVAL(comb.vxslvo.b_id, req_id);
    SETVAL(comb.vxslvo.b_user, req_user);
    SETVAL(comb.vxslvo.b_resp, CC2(resp_err, CONST("0", 1)));
    SETVAL(comb.vxslvo.r_valid, resp_valid);
    SETVAL(comb.vxslvo.r_id, req_id);
    SETVAL(comb.vxslvo.r_user, req_user);
    SETVAL(comb.vxslvo.r_resp, CC2(resp_err, CONST("0", 1)));
    SETVAL(comb.vxslvo.r_data, resp_rdata);
    SETVAL(comb.vxslvo.r_last, resp_last);
    SETVAL(o_xslvo, comb.vxslvo);
    SETVAL(o_cfg, comb.vcfg);
}
