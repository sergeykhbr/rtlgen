// 
//  Copyright 2023 Sergey Khabarov, sergeykhbr@gmail.com
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

#include "clint.h"

clint::clint(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "clint", name, comment),
    cpu_total(this, "cpu_total", "4", NO_COMMENT),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_mapinfo(this, "i_mapinfo", "interconnect slot information"),
    o_cfg(this, "o_cfg", "Device descriptor"),
    i_xslvi(this, "i_xslvi", "AXI Slave to Bridge interface"),
    o_xslvo(this, "o_xslvo", "AXI Bridge to Slave interface"),
    o_mtimer(this, "o_mtimer", "64", "Shadow read-only access from Harts"),
    o_msip(this, "o_msip", "cpu_total", "Machine mode Softare Pending Interrupt"),
    o_mtip(this, "o_mtip", "cpu_total", "Machine mode Timer Pending Interrupt"),
    // params
    // struct declaration
    clint_cpu_type_def_(this, "clint_cpu_type", NO_COMMENT),
    // signals
    w_req_valid(this, "w_req_valid", "1"),
    wb_req_addr(this, "wb_req_addr", "CFG_SYSBUS_ADDR_BITS"),
    wb_req_size(this, "wb_req_size", "8"),
    w_req_write(this, "w_req_write", "1"),
    wb_req_wdata(this, "wb_req_wdata", "CFG_SYSBUS_DATA_BITS"),
    wb_req_wstrb(this, "wb_req_wstrb", "CFG_SYSBUS_DATA_BYTES"),
    w_req_last(this, "w_req_last", "1"),
    w_req_ready(this, "w_req_ready", "1"),
    w_resp_valid(this, "w_resp_valid", "1"),
    wb_resp_rdata(this, "wb_resp_rdata", "CFG_SYSBUS_DATA_BITS"),
    wb_resp_err(this, "wb_resp_err", "1"),
    // registers
    mtime(this, "mtime", "64", "'0", NO_COMMENT),
    hart(this, &i_clk, &i_nrst, "hart", NO_COMMENT),
    rdata(this, "rdata", "64", "'0", NO_COMMENT),
    //
    comb(this),
    xslv0(this, "xslv0")
{
    Operation::start(this);

    xslv0.vid.setObjValue(&glob_pnp_cfg_->VENDOR_OPTIMITECH);
    xslv0.did.setObjValue(&glob_pnp_cfg_->OPTIMITECH_CLINT);
    NEW(xslv0, xslv0.getName().c_str());
        CONNECT(xslv0, 0, xslv0.i_clk, i_clk);
        CONNECT(xslv0, 0, xslv0.i_nrst, i_nrst);
        CONNECT(xslv0, 0, xslv0.i_mapinfo, i_mapinfo);
        CONNECT(xslv0, 0, xslv0.o_cfg, o_cfg);
        CONNECT(xslv0, 0, xslv0.i_xslvi, i_xslvi);
        CONNECT(xslv0, 0, xslv0.o_xslvo, o_xslvo);
        CONNECT(xslv0, 0, xslv0.o_req_valid, w_req_valid);
        CONNECT(xslv0, 0, xslv0.o_req_addr, wb_req_addr);
        CONNECT(xslv0, 0, xslv0.o_req_size, wb_req_size);
        CONNECT(xslv0, 0, xslv0.o_req_write, w_req_write);
        CONNECT(xslv0, 0, xslv0.o_req_wdata, wb_req_wdata);
        CONNECT(xslv0, 0, xslv0.o_req_wstrb, wb_req_wstrb);
        CONNECT(xslv0, 0, xslv0.o_req_last, w_req_last);
        CONNECT(xslv0, 0, xslv0.i_req_ready, w_req_ready);
        CONNECT(xslv0, 0, xslv0.i_resp_valid, w_resp_valid);
        CONNECT(xslv0, 0, xslv0.i_resp_rdata, wb_resp_rdata);
        CONNECT(xslv0, 0, xslv0.i_resp_err, wb_resp_err);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void clint::proc_comb() {
    GenObject *i;
    SETVAL(mtime, INC(mtime));
    SETVAL(comb.regidx, TO_INT(BITS(wb_req_addr, 13, 3)));

TEXT();
    i = &FOR ("i", CONST("0"), cpu_total, "++");
        SETARRITEM(hart, *i, hart.mtip, CONST("0", 1));
        IF (GE(mtime, ARRITEM(hart, *i, hart.mtimecmp)));
            SETARRITEM(hart, *i, hart.mtip, CONST("1", 1));
        ENDIF();
    ENDFOR();

TEXT();
    SWITCH (BITS(wb_req_addr, 15, 14));
    CASE (CONST("0", 2));
        SETBIT(comb.vrdata, 0, ARRITEM(hart, comb.regidx, hart.msip));
        SETBIT(comb.vrdata, 32, ARRITEM(hart, INC(comb.regidx), hart.msip));
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            IF (NZ(OR_REDUCE(BITS(wb_req_wstrb, 3, 0))));
                SETARRITEM(hart, comb.regidx, hart.msip, BIT(wb_req_wdata, 0));
            ENDIF();
            IF (NZ(OR_REDUCE(BITS(wb_req_wstrb, 7, 4))));
                SETARRITEM(hart, INC(comb.regidx), hart.msip, BIT(wb_req_wdata, 32));
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (CONST("1", 2));
        SETVAL(comb.vrdata, ARRITEM(hart, comb.regidx, hart.mtimecmp));
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            SETARRITEM(hart, comb.regidx, hart.mtimecmp, wb_req_wdata);
        ENDIF();
        ENDCASE();
    CASE (CONST("2", 2));
        IF (EQ(BITS(wb_req_addr, 13, 3), CONST("0x7ff", 11)));
            SETVAL(comb.vrdata, mtime, "[RO]");
        ENDIF();
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();
    SETVAL(rdata, comb.vrdata);

TEXT();
    SYNC_RESET(*this);

TEXT();
    i = &FOR ("i", CONST("0"), cpu_total, "++");
        SETBIT(comb.vb_msip, *i, ARRITEM(hart, *i, hart.msip));
        SETBIT(comb.vb_mtip, *i, ARRITEM(hart, *i, hart.mtip));
    ENDFOR();

TEXT();
    SETONE(w_req_ready);
    SETONE(w_resp_valid);
    SETVAL(wb_resp_rdata, rdata);
    SETZERO(wb_resp_err);
    SETVAL(o_msip, comb.vb_msip);
    SETVAL(o_mtip, comb.vb_mtip);
    SETVAL(o_mtimer, mtime);
}
