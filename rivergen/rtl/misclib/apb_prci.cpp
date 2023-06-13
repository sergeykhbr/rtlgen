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

#include "apb_prci.h"

apb_prci::apb_prci(GenObject *parent, const char *name) :
    ModuleObject(parent, "apb_prci", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_pwrreset(this, "i_pwrreset", "1", "Power-on reset, external button. Active HIGH"),
    i_dmireset(this, "i_dmireset", "1", "Debug reset: system reset except DMI interface"),
    i_sys_locked(this, "i_sys_locked", "1"),
    i_ddr_locked(this, "i_ddr_locked", "1"),
    o_sys_rst(this, "o_sys_rst", "1", "System reset except DMI. Active HIGH"),
    o_sys_nrst(this, "o_sys_nrst", "1", "System reset except DMI. Active LOW"),
    o_dbg_nrst(this, "o_dbg_nrst", "1", "Reset DMI. Active LOW"),
    i_mapinfo(this, "i_mapinfo", "interconnect slot information"),
    o_cfg(this, "o_cfg", "Device descriptor"),
    i_apbi(this, "i_apbi", "APB  Slave to Bridge interface"),
    o_apbo(this, "o_apbo", "APB Bridge to Slave interface"),
    // params
    // signals
    w_req_valid(this, "w_req_valid", "1"),
    wb_req_addr(this, "wb_req_addr", "32"),
    w_req_write(this, "w_req_write", "1"),
    wb_req_wdata(this, "wb_req_wdata", "32"),
    // registers
    sys_rst(this, "sys_rst", "1"),
    sys_nrst(this, "sys_nrst", "1"),
    dbg_nrst(this, "dbg_nrst", "1"),
    sys_locked(this, "sys_locked", "1"),
    ddr_locked(this, "ddr_locked", "1"),
    resp_valid(this, "resp_valid", "1"),
    resp_rdata(this, "resp_rdata", "32"),
    resp_err(this, "resp_err", "1"),
    //
    comb(this),
    pslv0(this, "pslv0")
{
    Operation::start(this);

    pslv0.vid.setObjValue(&glob_pnp_cfg_->VENDOR_OPTIMITECH);
    pslv0.did.setObjValue(&glob_pnp_cfg_->OPTIMITECH_PRCI);
    NEW(pslv0, pslv0.getName().c_str());
        CONNECT(pslv0, 0, pslv0.i_clk, i_clk);
        CONNECT(pslv0, 0, pslv0.i_nrst, sys_nrst);
        CONNECT(pslv0, 0, pslv0.i_mapinfo, i_mapinfo);
        CONNECT(pslv0, 0, pslv0.o_cfg, o_cfg);
        CONNECT(pslv0, 0, pslv0.i_apbi, i_apbi);
        CONNECT(pslv0, 0, pslv0.o_apbo, o_apbo);
        CONNECT(pslv0, 0, pslv0.o_req_valid, w_req_valid);
        CONNECT(pslv0, 0, pslv0.o_req_addr, wb_req_addr);
        CONNECT(pslv0, 0, pslv0.o_req_write, w_req_write);
        CONNECT(pslv0, 0, pslv0.o_req_wdata, wb_req_wdata);
        CONNECT(pslv0, 0, pslv0.i_resp_valid, resp_valid);
        CONNECT(pslv0, 0, pslv0.i_resp_rdata, resp_rdata);
        CONNECT(pslv0, 0, pslv0.i_resp_err, resp_err);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void apb_prci::proc_comb() {
    SETVAL(sys_rst, OR3(i_pwrreset, INV(i_sys_locked), i_dmireset));
    SETVAL(sys_nrst, INV(OR3(i_pwrreset, INV(i_sys_locked), i_dmireset)));
    SETVAL(dbg_nrst, INV(OR2(i_pwrreset, INV(i_sys_locked))));

TEXT();
    TEXT("Registers access:");
    SWITCH (BITS(wb_req_addr, 11, 2));
    CASE (CONST("0", 10), "0x00: pll statuses");
        SETBIT(comb.vb_rdata, 0, i_sys_locked);
        SETBIT(comb.vb_rdata, 1, i_ddr_locked);
        ENDCASE();
    CASE (CONST("1", 10), "0x04: reset status");
        SETBIT(comb.vb_rdata, 0, sys_nrst);
        SETBIT(comb.vb_rdata, 1, dbg_nrst);
        IF (NZ(w_req_valid));
            IF (NZ(w_req_write));
                TEXT("todo:");
            ENDIF();
        ENDIF();
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    SETVAL(resp_valid, w_req_valid);
    SETVAL(resp_rdata, comb.vb_rdata);
    SETZERO(resp_err);

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_sys_rst, sys_rst);
    SETVAL(o_sys_nrst, sys_nrst);
    SETVAL(o_dbg_nrst, dbg_nrst);
}
