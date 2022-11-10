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
    xaddr(this, "xaddr", "0"),
    xmask(this, "xmask", "-1"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    o_cfg(this, "o_cfg", "Slave config descriptor"),
    i_xslvi(this, "i_xslvi", "AXI4 Interconnect Bridge interface"),
    o_xslvo(this, "o_xslvo", "AXI4 Bridge to Interconnect interface"),
    i_apbmi(this, "i_apbmi", "APB Slave to Bridge master-in/slave-out interface"),
    o_apbmo(this, "o_apbmo", "APB Bridge to master-out/slave-in interface"),
    // params
    State_Idle(this, "3", "State_Idle", "0"),
    State_w(this, "3", "State_w", "1"),
    State_b(this, "3", "State_b", "2"),
    State_r(this, "3", "State_r", "3"),
    State_setup(this, "3", "State_setup", "4"),
    State_access(this, "3", "State_access", "5"),
    State_err(this, "3", "State_err", "6"),
    // signals
    // registers
    state(this, "state", "3", "State_Idle"),
    paddr(this, "paddr", "32"),
    pdata(this, "pdata", "CFG_SYSBUS_DATA_BITS"),
    pwrite(this, "pwrite", "1"),
    pstrb(this, "pstrb", "CFG_SYSBUS_DATA_BYTES"),
    pprot(this, "pprot", "3"),
    pselx(this, "pselx", "1"),
    penable(this, "penable", "1"),
    pslverr(this, "pslverr", "1"),
    xsize(this, "xsize", "8"),
    req_id(this, "req_id", "CFG_SYSBUS_ID_BITS"),
    req_user(this, "req_user", "CFG_SYSBUS_USER_BITS"),
    // process
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void axi2apb::proc_comb() {
    types_amba* cfg = glob_types_amba_;
    SETVAL(comb.vcfg.descrsize, cfg->PNP_CFG_DEV_DESCR_BYTES);
    SETVAL(comb.vcfg.descrtype, cfg->PNP_CFG_TYPE_SLAVE);
    SETVAL(comb.vcfg.xaddr, xaddr);
    SETVAL(comb.vcfg.xmask, xmask);
    SETVAL(comb.vcfg.vid, cfg->VENDOR_OPTIMITECH);
    SETVAL(comb.vcfg.did, cfg->OPTIMITECH_AXI2APB_BRIDGE);
    SETVAL(comb.vb_rdata, pdata);

TEXT();
    SWITCH(state);
    CASE (State_Idle);
        SETZERO(pslverr);
        SETZERO(penable);
        SETZERO(pselx);
        SETZERO(xsize);
        SETONE(comb.vslvo.aw_ready);
        SETONE(comb.vslvo.w_ready, "AXILite support");
        SETVAL(comb.vslvo.ar_ready, INV(i_xslvi.aw_valid));
        IF (NZ(i_xslvi.aw_valid));
            SETONE(pwrite);
            SETVAL(paddr, CC2(BITS(i_xslvi.aw_bits.addr, 31, 2), CONST("0", 2)));
            SETVAL(pprot, i_xslvi.aw_bits.prot);
            SETVAL(req_id, i_xslvi.aw_id);
            SETVAL(req_user, i_xslvi.aw_user);
            IF (GE(i_xslvi.aw_bits.size, CONST("3", 3)));
                SETONE(xsize);
                SETVAL(comb.vb_rdata, i_xslvi.w_data);
                SETVAL(pstrb, i_xslvi.w_strb);
            ELSIF(EZ(BIT(i_xslvi.aw_bits.addr, 2)));
                SETVAL(comb.vb_rdata, CC2(CONST("0", 32), BITS(i_xslvi.w_data, 31, 0)));
                SETVAL(pstrb, CC2(CONST("0", 4), BITS(i_xslvi.w_strb, 3, 0)));
            ELSE();
                SETVAL(comb.vb_rdata, CC2(CONST("0", 32), BITS(i_xslvi.w_data, 63, 32)));
                SETVAL(pstrb, CC2(CONST("0", 4), BITS(i_xslvi.w_strb, 7, 4)));
            ENDIF();
            IF (NZ(i_xslvi.aw_bits.len));
                SETVAL(state, State_err, "Burst is not supported");
            ELSIF (NZ(i_xslvi.w_valid));
                TEXT("AXILite support");
                SETVAL(state, State_setup);
                SETONE(pselx);
            ELSE();
                SETVAL(state, State_w);
            ENDIF();
        ELSIF(NZ(i_xslvi.ar_valid));
            SETZERO(pwrite);
            SETONE(pselx);
            SETVAL(paddr, CC2(BITS(i_xslvi.ar_bits.addr, 31, 2), CONST("0", 2)));
            SETVAL(pprot, i_xslvi.ar_bits.prot);
            SETVAL(req_id, i_xslvi.ar_id);
            SETVAL(req_user, i_xslvi.ar_user);
            IF (GE(i_xslvi.ar_bits.size, CONST("3", 3)));
                SETONE(xsize);
            ENDIF();
            IF (NZ(i_xslvi.ar_bits.len));
                SETVAL(state, State_err, "Burst is not supported");
            ELSE();
                SETVAL(state, State_setup);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (State_w);
        SETONE(comb.vslvo.w_ready);
        SETONE(pselx);
        IF (NZ(xsize));
            SETVAL(comb.vb_rdata, i_xslvi.w_data);
            SETVAL(pstrb, i_xslvi.w_strb);
        ELSIF(EZ(BIT(paddr, 2)));
            SETVAL(comb.vb_rdata, CC2(CONST("0", 32), BITS(i_xslvi.w_data, 31, 0)));
            SETVAL(pstrb, CC2(CONST("0", 4), BITS(i_xslvi.w_strb, 3, 0)));
        ELSE();
            SETVAL(comb.vb_rdata, CC2(CONST("0", 32), BITS(i_xslvi.w_data, 63, 32)));
            SETVAL(pstrb, CC2(CONST("0", 4), BITS(i_xslvi.w_strb, 7, 4)));
        ENDIF();
        IF (NZ(i_xslvi.w_valid));
            SETVAL(state, State_setup);
        ENDIF();
        ENDCASE();
    CASE (State_b);
        SETONE(comb.vslvo.b_valid);
        IF (NZ(i_xslvi.b_ready));
            SETVAL(state, State_Idle);
        ENDIF();
        ENDCASE();
    CASE (State_r);
        SETONE(comb.vslvo.r_valid);
        IF (NZ(i_xslvi.r_ready));
            SETVAL(state, State_Idle);
        ENDIF();
        ENDCASE();
    CASE (State_setup);
        SETONE(penable);
        SETVAL(state, State_access);
        ENDCASE();
    CASE (State_access);
        IF (EZ(pwrite));
            IF (NZ(xsize));
                SETBITS(comb.vb_rdata, 63, 32, i_apbmi.prdata);
            ELSE();
                SETBITS(comb.vb_rdata, 31, 0, i_apbmi.prdata);
            ENDIF();
        ENDIF();
        SETVAL(pslverr, i_apbmi.pslverr);
        IF (NZ(i_apbmi.pready));
            SETZERO(penable);
            SETZERO(pselx);
            IF (NZ(xsize));
                SETVAL(xsize, DEC(xsize));
                SETVAL(paddr, ADD2(paddr, CONST("4")));
                SETVAL(pstrb, CC2(CONST("0", 4), BITS(pstrb, 7, 4)));
                IF (NZ(pwrite));
                    SETVAL(comb.vb_rdata, CC2(CONST("0", 32), BITS(pdata, 63, 32)));
                ENDIF();
                SETVAL(state, State_setup);
            ELSIF (NZ(pwrite));
                SETVAL(state, State_b);
            ELSE();
                SETVAL(state, State_r);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASEDEF();
        TEXT("Burst transactions are not supported:");
        SETVAL(comb.vb_rdata, ALLONES());
        SETONE(pslverr);
        IF (NZ(pwrite));
            SETVAL(state, State_b);
        ELSE();
            SETVAL(state, State_r);
        ENDIF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    SETVAL(pdata, comb.vb_rdata);
    SETVAL(comb.vapbmo.paddr, paddr);
    SETVAL(comb.vapbmo.pwrite, pwrite);
    SETVAL(comb.vapbmo.pwdata, BITS(pdata, 31, 0));
    SETVAL(comb.vapbmo.pstrb, BITS(pstrb, 3, 0));
    SETVAL(comb.vapbmo.pselx, pselx);
    SETVAL(comb.vapbmo.penable, penable);
    SETVAL(comb.vapbmo.pprot, pprot);

TEXT();
    SETVAL(comb.vslvo.r_data, pdata);
    SETVAL(comb.vslvo.r_resp, CC2(pslverr, CONST("0", 1)));
    SETVAL(comb.vslvo.r_id, req_id);
    SETVAL(comb.vslvo.r_user, req_user);
    SETONE(comb.vslvo.r_last);

TEXT();
    SETVAL(comb.vslvo.b_resp, CC2(pslverr, CONST("0", 1)));
    SETVAL(comb.vslvo.b_id, req_id);
    SETVAL(comb.vslvo.b_user, req_user);

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_cfg, comb.vcfg);
    SETVAL(o_xslvo, comb.vslvo);
    SETVAL(o_apbmo, comb.vapbmo);
}