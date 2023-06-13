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

#include "axi2apb_bus1.h"

axi2apb_bus1::axi2apb_bus1(GenObject *parent, const char *name) :
    ModuleObject(parent, "axi2apb_bus1", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_mapinfo(this, "i_mapinfo", "Base address information from the interconnect port"),
    o_cfg(this, "o_cfg", "Slave config descriptor"),
    i_xslvi(this, "i_xslvi", "AXI4 Interconnect Bridge interface"),
    o_xslvo(this, "o_xslvo", "AXI4 Bridge to Interconnect interface"),
    i_apbo(this, "i_apbo", "APB slaves output vector"),
    o_apbi(this, "o_apbi", "APB slaves input vector"),
    o_mapinfo(this, "o_mapinfo", "APB devices memory mapping information"),
    // params
    State_Idle(this, "2", "State_Idle", "0"),
    State_setup(this, "2", "State_setup", "1"),
    State_access(this, "2", "State_access", "2"),
    State_out(this, "2", "State_out", "3"),
    // signals
    w_req_valid(this, "w_req_valid", "1"),
    wb_req_addr(this, "wb_req_addr", "CFG_SYSBUS_ADDR_BITS"),
    wb_req_size(this, "wb_req_size", "8"),
    w_req_write(this, "w_req_write", "1"),
    wb_req_wdata(this, "wb_req_wdata", "CFG_SYSBUS_DATA_BITS"),
    wb_req_wstrb(this, "wb_req_wstrb", "CFG_SYSBUS_DATA_BYTES"),
    w_req_last(this, "w_req_last", "1"),
    w_req_ready(this, "w_req_ready", "1"),
    // registers
    state(this, "state", "3", "State_Idle"),
    selidx(this, "selidx", "CFG_BUS1_PSLV_LOG2_TOTAL", "0"),
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
    size(this, "size", "8"),
    // modules
    axi0(this, "axi0"),
    // process
    comb(this)
{
    Operation::start(this);
    axi0.vid.setObjValue(&glob_pnp_cfg_->VENDOR_OPTIMITECH);
    axi0.did.setObjValue(&glob_pnp_cfg_->OPTIMITECH_AXI2APB_BRIDGE);
    NEW(axi0, axi0.getName().c_str());
        CONNECT(axi0, 0, axi0.i_clk, i_clk);
        CONNECT(axi0, 0, axi0.i_nrst, i_nrst);
        CONNECT(axi0, 0, axi0.i_mapinfo, i_mapinfo);
        CONNECT(axi0, 0, axi0.o_cfg, o_cfg);
        CONNECT(axi0, 0, axi0.i_xslvi, i_xslvi);
        CONNECT(axi0, 0, axi0.o_xslvo, o_xslvo);
        CONNECT(axi0, 0, axi0.o_req_valid, w_req_valid);
        CONNECT(axi0, 0, axi0.o_req_addr, wb_req_addr);
        CONNECT(axi0, 0, axi0.o_req_size, wb_req_size);
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

void axi2apb_bus1::proc_comb() {
    types_amba* cfg = glob_types_amba_;
    types_bus1 *bus1 = glob_bus1_cfg_;
    GenObject *i;
    
    i = &FOR ("i", CONST("0"), bus1->CFG_BUS1_PSLV_TOTAL, "++");
        SETARRITEM(comb.vapbo, *i, comb.vapbo, ARRITEM(i_apbo, *i, i_apbo), "Cannot read vector item from port in systemc");
    ENDFOR();
    TEXT("Unmapped default slave:");
    SETARRITEM(comb.vapbo, bus1->CFG_BUS1_PSLV_TOTAL, comb.vapbo->pready, CONST("1", 1));
    SETARRITEM(comb.vapbo, bus1->CFG_BUS1_PSLV_TOTAL, comb.vapbo->pslverr, CONST("1", 1));
    SETARRITEM(comb.vapbo, bus1->CFG_BUS1_PSLV_TOTAL, comb.vapbo->prdata, ALLONES());
    SETZERO(w_req_ready);
    SETZERO(pvalid);
    SETVAL(comb.iselidx, TO_INT(selidx));

TEXT();
    SWITCH(state);
    CASE (State_Idle);
        SETONE(w_req_ready);
        SETZERO(pslverr);
        SETZERO(penable);
        SETZERO(pselx);
        SETVAL(selidx, bus1->CFG_BUS1_PSLV_TOTAL);
        i = &FOR ("i", CONST("0"), bus1->CFG_BUS1_PSLV_TOTAL, "++");
            IF (ANDx(2, &GE(wb_req_addr, ARRITEM(bus1->CFG_BUS1_MAP, *i, bus1->CFG_BUS1_MAP.addr_start)),
                        &LS(wb_req_addr, ARRITEM(bus1->CFG_BUS1_MAP, *i, bus1->CFG_BUS1_MAP.addr_end))));
                SETVAL(selidx, *i);
            ENDIF();
        ENDFOR();
        IF (NZ(w_req_valid));
            SETVAL(pwrite, w_req_write);
            SETONE(pselx);
            SETVAL(paddr, CC2(BITS(wb_req_addr, 31, 2), CONST("0", 2)));
            SETZERO(pprot);
            IF (NZ(BIT(wb_req_addr, 2)));
                SETVAL(pwdata, CC2(CONST("0", 32), BITS(wb_req_wdata, 63, 32)));
                SETVAL(pstrb, CC2(CONST("0", 4), BITS(wb_req_wstrb, 7, 4)));
            ELSE();
                SETVAL(pwdata, wb_req_wdata);
                SETVAL(pstrb, wb_req_wstrb);
            ENDIF();
            SETVAL(state, State_setup);
            SETVAL(size, wb_req_size);
            IF (EZ(w_req_last));
                SETVAL(state, State_out, "Burst is not supported");
                SETZERO(pselx);
                SETONE(pslverr);
                SETVAL(prdata, ALLONES());
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (State_setup);
        SETONE(penable);
        SETVAL(state, State_access);
        ENDCASE();
    CASE (State_access);
        SETVAL(pslverr, ARRITEM(comb.vapbo, comb.iselidx, comb.vapbo->pslverr));
        IF (NZ(ARRITEM(comb.vapbo, comb.iselidx, comb.vapbo->pready)));
            SETZERO(penable);
            IF (EZ(BIT(paddr, 2)));
                SETVAL(prdata, CC2(BITS(prdata, 63, 32), ARRITEM(comb.vapbo, comb.iselidx, comb.vapbo->prdata)));
            ELSE();
                SETVAL(prdata, CC2(ARRITEM(comb.vapbo, comb.iselidx, comb.vapbo->prdata),
                                   BITS(prdata, 31, 0)));
            ENDIF();
            IF (GT(size, CONST("4", 8)));
                SETVAL(size, SUB2(size, CONST("4")));
                SETVAL(paddr, ADD2(paddr, CONST("4")));
                SETVAL(pwdata, CC2(CONST("0", 32), BITS(wb_req_wdata, 63, 32)));
                SETVAL(pstrb, CC2(CONST("0", 4), BITS(wb_req_wstrb, 7, 4)));
                SETVAL(state, State_setup);
            ELSE();
                SETONE(pvalid);
                SETVAL(state, State_out);
                SETZERO(pselx);
                SETZERO(pwrite);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(State_out);
        SETZERO(pvalid);
        SETZERO(pslverr);
        SETVAL(state, State_Idle);
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    SETARRITEM(comb.vapbi, comb.iselidx, comb.vapbi->paddr, paddr);
    SETARRITEM(comb.vapbi, comb.iselidx, comb.vapbi->pwrite, pwrite);
    SETARRITEM(comb.vapbi, comb.iselidx, comb.vapbi->pwdata, BITS(pwdata, 31, 0));
    SETARRITEM(comb.vapbi, comb.iselidx, comb.vapbi->pstrb, BITS(pstrb, 3, 0));
    SETARRITEM(comb.vapbi, comb.iselidx, comb.vapbi->pselx, pselx);
    SETARRITEM(comb.vapbi, comb.iselidx, comb.vapbi->penable, penable);
    SETARRITEM(comb.vapbi, comb.iselidx, comb.vapbi->pprot, pprot);


TEXT();
    SYNC_RESET(*this);

TEXT();
     i = &FOR ("i", CONST("0"), bus1->CFG_BUS1_PSLV_TOTAL, "++");
        SETARRITEM(o_apbi, *i, o_apbi, ARRITEM(comb.vapbi, *i, comb.vapbi));
        SETARRITEM(o_mapinfo, *i, o_mapinfo, ARRITEM(bus1->CFG_BUS1_MAP, *i, bus1->CFG_BUS1_MAP));
    ENDFOR();
}
