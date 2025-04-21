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

#include "apb_slv.h"

apb_slv::apb_slv(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "apb_slv", name, comment),
    vid(this, "vid", "0", "Vendor ID"),
    did(this, "did", "0", "Device ID"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_mapinfo(this, "i_mapinfo", "Base address information from the interconnect port"),
    o_cfg(this, "o_cfg", "Slave config descriptor"),
    i_apbi(this, "i_apbi", "APB  Slave to Bridge interface"),
    o_apbo(this, "o_apbo", "APB Bridge to Slave interface"),
    o_req_valid(this, "o_req_valid", "1"),
    o_req_addr(this, "o_req_addr", "32"),
    o_req_write(this, "o_req_write", "1"),
    o_req_wdata(this, "o_req_wdata", "32"),
    i_resp_valid(this, "i_resp_valid", "1"),
    i_resp_rdata(this, "i_resp_rdata", "32"),
    i_resp_err(this, "i_resp_err", "1"),
    // params
    State_Idle(this, "State_Idle", "2", "0", NO_COMMENT),
    State_Request(this, "State_Request", "2", "1", NO_COMMENT),
    State_WaitResp(this, "State_WaitResp", "2", "2", NO_COMMENT),
    State_Resp(this, "State_Resp", "2", "3", NO_COMMENT),
    // signals
    // registers
    state(this, "state", "3", "State_Idle"),
    req_valid(this, "req_valid", "1"),
    req_addr(this, "req_addr", "32", "'0", NO_COMMENT),
    req_write(this, "req_write", "1"),
    req_wdata(this, "req_wdata", "32", "'0", NO_COMMENT),
    resp_valid(this, "resp_valid", "1"),
    resp_rdata(this, "resp_rdata", "32", "'0", NO_COMMENT),
    resp_err(this, "resp_err", "1"),
    // process
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void apb_slv::proc_comb() {
    types_pnp *pnp = glob_pnp_cfg_;
    SETVAL(comb.vcfg.descrsize, pnp->PNP_CFG_DEV_DESCR_BYTES);
    SETVAL(comb.vcfg.descrtype, pnp->PNP_CFG_TYPE_SLAVE);
    SETVAL(comb.vcfg.addr_start, i_mapinfo.addr_start);
    SETVAL(comb.vcfg.addr_end, i_mapinfo.addr_end);
    SETVAL(comb.vcfg.vid, vid);
    SETVAL(comb.vcfg.did, did);

TEXT();
    SETZERO(req_valid);

TEXT();
    SWITCH(state);
    CASE(State_Idle);
        SETZERO(resp_valid);
        SETZERO(resp_err);
        IF (NZ(i_apbi.pselx));
            SETVAL(state, State_Request);
            SETONE(req_valid);
            SETVAL(req_addr, i_apbi.paddr);
            SETVAL(req_write, i_apbi.pwrite);
            SETVAL(req_wdata, i_apbi.pwdata);
        ENDIF();
        ENDCASE();
    CASE(State_Request);
        TEXT("One clock wait state:");
        SETVAL(state, State_WaitResp);
        ENDCASE();
    CASE(State_WaitResp);
        SETVAL(resp_valid, i_resp_valid);
        IF (NZ(i_resp_valid));
            SETVAL(resp_rdata, i_resp_rdata);
            SETVAL(resp_err, i_resp_err);
            SETVAL(state, State_Resp);
        ENDIF();
        ENDCASE();
    CASE (State_Resp);
        IF (NZ(i_apbi.penable));
            SETVAL(state, State_Idle);
            SETZERO(resp_valid);
        ENDIF();
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    SYNC_RESET();

TEXT();
    SETVAL(o_req_valid, req_valid);
    SETVAL(o_req_addr, req_addr);
    SETVAL(o_req_write, req_write);
    SETVAL(o_req_wdata, req_wdata);

TEXT();
    SETVAL(comb.vapbo.pready, resp_valid);
    SETVAL(comb.vapbo.prdata, resp_rdata);
    SETVAL(comb.vapbo.pslverr, resp_err);
    SETVAL(o_apbo, comb.vapbo);
    SETVAL(o_cfg, comb.vcfg);
}
