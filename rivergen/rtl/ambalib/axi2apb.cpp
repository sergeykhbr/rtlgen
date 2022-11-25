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

#include "axi2apb.h"

axi2apb::axi2apb(GenObject *parent, const char *name) :
    ModuleObject(parent, "axi2apb", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_mapinfo(this, "i_mapinfo", "Base address information from the interconnect port"),
    o_cfg(this, "o_cfg", "Slave config descriptor"),
    i_xslvi(this, "i_xslvi", "AXI4 Interconnect Bridge interface"),
    o_xslvo(this, "o_xslvo", "AXI4 Bridge to Interconnect interface"),
    i_apbmi(this, "i_apbmi", "APB Slave to Bridge master-in/slave-out interface"),
    o_apbmo(this, "o_apbmo", "APB Bridge to master-out/slave-in interface"),
    // params
    State_Idle(this, "2", "State_Idle", "0"),
    State_setup(this, "2", "State_setup", "1"),
    State_access(this, "2", "State_access", "2"),
    State_err(this, "2", "State_err", "3"),
    // signals
    w_req_valid(this, "w_req_valid", "1"),
    wb_req_addr(this, "wb_req_addr", "CFG_SYSBUS_ADDR_BITS"),
    w_req_write(this, "w_req_write", "1"),
    wb_req_wdata(this, "wb_req_wdata", "CFG_SYSBUS_DATA_BITS"),
    wb_req_wstrb(this, "wb_req_wstrb", "CFG_SYSBUS_DATA_BYTES"),
    w_req_last(this, "w_req_last", "1"),
    w_req_ready(this, "w_req_ready", "1"),
    // registers
    state(this, "state", "3", "State_Idle"),
    pvalid(this, "pvalid", "1"),
    paddr(this, "paddr", "32"),
    pwdata(this, "pwdata", "CFG_SYSBUS_DATA_BITS"),
    prdata(this, "prdata", "CFG_SYSBUS_DATA_BITS"),
    pwrite(this, "pwrite", "1"),
    pstrb(this, "pstrb", "CFG_SYSBUS_DATA_BYTES"),
    pprot(this, "pprot", "3"),
    pselx(this, "pselx", "1"),
    penable(this, "penable", "1"),
    pslverr(this, "pslverr", "1"),
    xsize(this, "xsize", "8"),
    // modules
    axi0(this, "axi0"),
    // process
    comb(this)
{
    Operation::start(this);
    axi0.vid.setObjValue(&glob_types_amba_->VENDOR_OPTIMITECH);
    axi0.did.setObjValue(&glob_types_amba_->OPTIMITECH_AXI2APB_BRIDGE);
    NEW(axi0, axi0.getName().c_str());
        CONNECT(axi0, 0, axi0.i_clk, i_clk);
        CONNECT(axi0, 0, axi0.i_nrst, i_nrst);
        CONNECT(axi0, 0, axi0.i_mapinfo, i_mapinfo);
        CONNECT(axi0, 0, axi0.o_cfg, o_cfg);
        CONNECT(axi0, 0, axi0.i_xslvi, i_xslvi);
        CONNECT(axi0, 0, axi0.o_xslvo, o_xslvo);
        CONNECT(axi0, 0, axi0.o_req_valid, w_req_valid);
        CONNECT(axi0, 0, axi0.o_req_addr, wb_req_addr);
        CONNECT(axi0, 0, axi0.o_req_write, w_req_write);
        CONNECT(axi0, 0, axi0.o_req_wdata, wb_req_wdata);
        CONNECT(axi0, 0, axi0.o_req_wstrb, wb_req_wstrb);
        CONNECT(axi0, 0, axi0.o_req_last, w_req_last);
        CONNECT(axi0, 0, axi0.i_req_ready, w_req_ready);
        CONNECT(axi0, 0, axi0.i_resp_valid, pvalid);
        CONNECT(axi0, 0, axi0.i_resp_rdata, prdata);
        CONNECT(axi0, 0, axi0.i_resp_err, pslverr);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void axi2apb::proc_comb() {
    types_amba* cfg = glob_types_amba_;
    SETZERO(w_req_ready);
    SETZERO(pvalid);

TEXT();
    SWITCH(state);
    CASE (State_Idle);
        SETONE(w_req_ready);
        SETZERO(pslverr);
        SETZERO(penable);
        SETZERO(pselx);
        SETZERO(xsize);
        IF (NZ(w_req_valid));
            SETVAL(pwrite, w_req_write);
            SETONE(pselx);
            SETVAL(paddr, CC2(BITS(wb_req_addr, 31, 2), CONST("0", 2)));
            SETZERO(pprot);
            SETVAL(pwdata, wb_req_wdata);
            SETVAL(pstrb, wb_req_wstrb);
            SETVAL(state, State_setup);
            SETVAL(xsize, AND_REDUCE(wb_req_wstrb));
            IF (NZ(w_req_last));
                SETVAL(state, State_err, "Burst is not supported");
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (State_setup);
        SETONE(penable);
        SETVAL(state, State_access);
        ENDCASE();
    CASE (State_access);
        SETVAL(pslverr, i_apbmi.pslverr);
        IF (NZ(i_apbmi.pready));
            SETZERO(penable);
            SETZERO(pselx);
            IF (EZ(BIT(paddr, 2)));
                SETVAL(prdata, CC2(BITS(prdata, 63, 32), i_apbmi.prdata));
            ELSE();
                SETVAL(prdata, CC2(i_apbmi.prdata, BITS(prdata, 31, 0)));
            ENDIF();
            IF (NZ(xsize));
                SETVAL(xsize, DEC(xsize));
                SETVAL(paddr, ADD2(paddr, CONST("4")));
                SETVAL(state, State_setup);
            ELSE();
                SETONE(pvalid);
                SETVAL(state, State_Idle);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASEDEF();
        TEXT("Burst transactions are not supported:");
        SETONE(pvalid);
        SETVAL(prdata, ALLONES());
        SETONE(pslverr);
        SETVAL(state, State_Idle);
        ENDCASE();
    ENDSWITCH();

TEXT();
    SETVAL(comb.vapbmo.paddr, paddr);
    SETVAL(comb.vapbmo.pwrite, pwrite);
    IF (EZ(BIT(paddr, 2)));
        SETVAL(comb.vapbmo.pwdata, BITS(pwdata, 31, 0));
        SETVAL(comb.vapbmo.pstrb, BITS(pstrb, 3, 0));
    ELSE();
        SETVAL(comb.vapbmo.pwdata, BITS(pwdata, 63, 32));
        SETVAL(comb.vapbmo.pstrb, BITS(pstrb, 7, 4));
    ENDIF();
    SETVAL(comb.vapbmo.pselx, pselx);
    SETVAL(comb.vapbmo.penable, penable);
    SETVAL(comb.vapbmo.pprot, pprot);


TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_apbmo, comb.vapbmo);
}