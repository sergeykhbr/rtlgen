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

#include "plic.h"

plic::plic(GenObject *parent, const char *name) :
    ModuleObject(parent, "plic", name),
    ctxmax(this, "ctxmax", "8"),
    irqmax(this, "irqmax", "128"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_mapinfo(this, "i_mapinfo", "interconnect slot information"),
    o_cfg(this, "o_cfg", "Device descriptor"),
    i_xslvi(this, "i_xslvi", "AXI Slave to Bridge interface"),
    o_xslvo(this, "o_xslvo", "AXI Bridge to Slave interface"),
    i_irq_request(this, "i_irq_request", "irqmax", "[0] must be tight to GND"),
    o_ip(this, "o_ip", "ctxmax"),
    // params
    // struct declaration
    plic_context_type_def_(this, "plic_context_type"),
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
    src_priority(this, "src_priority", "MUL(4,1024)"),
    pending(this, "pending", "1024"),
    ip(this, "ip", "ctxmax"),
    ctx(this, "ctx"),
    rdata(this, "rdata", "64"),
    //
    comb(this),
    xslv0(this, "xslv0")
{
    Operation::start(this);

    xslv0.vid.setObjValue(&glob_pnp_cfg_->VENDOR_OPTIMITECH);
    xslv0.did.setObjValue(&glob_pnp_cfg_->OPTIMITECH_PLIC);
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

void plic::proc_comb() {
    GenObject *i;
    GenObject *n;

    TEXT("Warning SystemC limitation workaround:");
    TEXT("  Cannot directly write into bitfields of the signals v.* registers");
    TEXT("  So, use the following vb_* logic variables for that and then copy them.");
    SETVAL(comb.vb_src_priority, src_priority);
    SETVAL(comb.vb_pending, pending);
    i = &FOR ("i", CONST("0"), ctxmax, "++");
        SETARRITEM(comb.vb_ctx, *i, comb.vb_ctx.priority_th, ARRITEM(ctx, *i, ctx.priority_th));
        SETARRITEM(comb.vb_ctx, *i, comb.vb_ctx.ie, ARRITEM(ctx, *i, ctx.ie));
        SETARRITEM(comb.vb_ctx, *i, comb.vb_ctx.irq_idx, ARRITEM(ctx, *i, ctx.irq_idx));
        SETARRITEM(comb.vb_ctx, *i, comb.vb_ctx.irq_prio, ARRITEM(ctx, *i, ctx.irq_prio));
    ENDFOR();

TEXT();
    i = &FOR ("i", CONST("1"), irqmax, "++");
        IF (AND2(NZ(BIT(i_irq_request, *i)),
                 GT(TO_INT(BITSW(src_priority, MUL2(CONST("4"), *i), CONST("4"))), CONST("0", 4))));
            SETBITONE(comb.vb_pending, *i);
        ENDIF();
    ENDFOR();

TEXT();
    n = &FOR ("n", CONST("0"), ctxmax, "++");
        i = &FOR ("i", CONST("0"), irqmax, "++");
            IF (ANDx(3, &NZ(BIT(pending, *i)),
                        &NZ(BIT(ARRITEM(ctx, *n, ctx.ie), *i)),
                        &GT(TO_INT(BITSW(src_priority, MUL2(CONST("4"), *i), CONST("4"))),
                            ARRITEM(ctx, *n, ctx.priority_th))));
                SETARRITEMBITSW(comb.vb_ctx, *n, comb.vb_ctx.ip_prio, MUL2(CONST("4"), *i), CONST("4"),
                                BITSW(src_priority, MUL2(CONST("4"), *i), CONST("4")));
                SETARRITEMBIT(comb.vb_ctx, *n, comb.vb_ctx.prio_mask, TO_INT(BITSW(src_priority, MUL2(CONST("4"), *i), CONST("4"))),
                                CONST("1", 1));
            ENDIF();
        ENDFOR();
    ENDFOR();

TEXT();
    TEXT("Select max priority in each context");
    n = &FOR ("n", CONST("0"), ctxmax, "++");
        i = &FOR ("i", CONST("0"), CONST("16"), "++");
            IF (NZ(BIT(ARRITEM(ctx, *n, ctx.prio_mask), *i)));
                SETARRITEM(comb.vb_ctx, *n, comb.vb_ctx.sel_prio, *i);
            ENDIF();
        ENDFOR();
    ENDFOR();

TEXT();
    TEXT("Select max priority in each context");
    n = &FOR ("n", CONST("0"), ctxmax, "++");
        i = &FOR ("i", CONST("0"), irqmax, "++");
            IF (ANDx(2, &OR_REDUCE(ARRITEM(ctx, *n, ctx.sel_prio)),
                        &EQ(BITSW(ARRITEM(ctx, *n, ctx.ip_prio), MUL2(CONST("4"), *i), CONST("4")),
                            ARRITEM(ctx, *n, ctx.sel_prio))));
                TEXT("Most prio irq and prio level");
                SETARRITEM(comb.vb_irq_idx, *n, comb.vb_irq_idx, *i);
                SETARRITEM(comb.vb_irq_prio, *n, comb.vb_irq_prio, ARRITEM(ctx, *n, ctx.sel_prio));
            ENDIF();
        ENDFOR();
    ENDFOR();

TEXT();
    n = &FOR ("n", CONST("0"), ctxmax, "++");
        SETARRITEM(comb.vb_ctx, *n, comb.vb_ctx.irq_idx, ARRITEM(comb.vb_irq_idx, *n, comb.vb_irq_idx));
        SETARRITEM(comb.vb_ctx, *n, comb.vb_ctx.irq_prio, ARRITEM(comb.vb_irq_prio, *n, comb.vb_irq_prio));
        SETBIT(comb.vb_ip, *n, OR_REDUCE(ARRITEM(comb.vb_irq_idx, *n, comb.vb_irq_idx)));
    ENDFOR();

TEXT();
    TEXT("R/W registers access:");
    SETVAL(comb.rctx_idx, TO_INT(BITS(wb_req_addr, 20, 12)));
    IF (EQ(BITS(wb_req_addr, 21, 12), CONST("0", 10)), "src_prioirty");
        TEXT("0x000000..0x001000: Irq 0 unused");
        IF (NZ(OR_REDUCE(BITS(wb_req_addr, 11, 3))));
            SETBITS(comb.vrdata, 3, 0, TO_U64(BITSW(src_priority, MUL2(CONST("8"), BITS(wb_req_addr, 11, 3)), CONST("4"))));
            IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
                IF (NZ(OR_REDUCE(BITS(wb_req_wstrb, 3, 0))));
                    SETBITSW(comb.vb_src_priority,
                             MUL2(CONST("8"), BITS(wb_req_addr, 11, 3)),
                             CONST("4"),
                             BITS(wb_req_wdata, 3, 0));
                ENDIF();
            ENDIF();
        ENDIF();

        TEXT();
        SETBITS(comb.vrdata, 35, 32, TO_U64(BITSW(src_priority, 
                                            ADD2(MUL2(CONST("8"), BITS(wb_req_addr, 11, 3)), CONST("32")),
                                            CONST("4"))));
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            IF (NZ(OR_REDUCE(BITS(wb_req_wstrb, 7, 4))));
                SETBITSW(comb.vb_src_priority,
                         ADD2(MUL2(CONST("8"), BITS(wb_req_addr, 11, 3)), CONST("32")),
                         CONST("4"),
                         BITS(wb_req_wdata, 35, 32));
            ENDIF();
        ENDIF();
    ELSIF (EQ(BITS(wb_req_addr, 21, 12), CONST("1", 10)));
        TEXT("0x001000..0x001080");
        SETVAL(comb.vrdata, TO_U64(BITSW(pending, MUL2(CONST("64"), BITS(wb_req_addr, 6, 3)), CONST("64"))));
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            IF (NZ(OR_REDUCE(BITS(wb_req_wstrb, 3, 0))));
                SETBITSW(comb.vb_pending, MUL2(CONST("64"), BITS(wb_req_addr, 6, 3)), CONST("32"), BITS(wb_req_wdata, 31, 0));
            ENDIF();
            IF (NZ(OR_REDUCE(BITS(wb_req_wstrb, 7, 4))));
                SETBITSW(comb.vb_pending, ADD2(MUL2(CONST("64"), BITS(wb_req_addr, 6, 3)), CONST("32")), CONST("32"), BITS(wb_req_wdata, 63, 32));
            ENDIF();
        ENDIF();
    ELSIF (ANDx(2, &EQ(BITS(wb_req_addr, 21, 12), CONST("2", 10)),
                   &LS(BITS(wb_req_addr, 11, 7), ctxmax)));
        TEXT("First 32 context of 15867 support only");
        TEXT("0x002000,0x002080,...,0x200000");
        SETVAL(comb.vrdata, TO_U64(BITSW(ARRITEM(ctx, BITS(wb_req_addr, 11, 7), ctx.ie),
                                         MUL2(CONST("64"), BITS(wb_req_addr, 6, 3)),
                                         CONST("64"))));
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            IF (NZ(OR_REDUCE(BITS(wb_req_wstrb, 3, 0))));
                SETARRITEMBITSW(comb.vb_ctx, BITS(wb_req_addr, 11, 7), comb.vb_ctx.ie,
                               MUL2(CONST("64"), BITS(wb_req_addr, 6, 3)), CONST("32"), BITS(wb_req_wdata, 31, 0));
            ENDIF();
            IF (NZ(OR_REDUCE(BITS(wb_req_wstrb, 7, 4))));
                SETARRITEMBITSW(comb.vb_ctx, BITS(wb_req_addr, 11, 7), comb.vb_ctx.ie,
                               ADD2(MUL2(CONST("64"), BITS(wb_req_addr, 6, 3)), CONST("32")), CONST("32"), BITS(wb_req_wdata, 63, 32));
            ENDIF();
        ENDIF();
    ELSIF (AND2(GE(BITS(wb_req_addr, 21, 12), CONST("0x200", 10)), LS(BITS(wb_req_addr, 20, 12), ctxmax)));
        TEXT("0x200000,0x201000,...,0x4000000");
        IF (EQ(BITS(wb_req_addr, 11, 3), CONST("0", 9)));
            TEXT("masking (disabling) all interrupt with <= priority");
            SETBITS(comb.vrdata, 3, 0, ARRITEM(ctx, comb.rctx_idx, ctx.priority_th));
            SETBITS(comb.vrdata, 41, 32, ARRITEM(ctx, comb.rctx_idx, ctx.irq_idx));
            TEXT("claim/ complete. Reading clears pending bit");
            IF (NZ(BIT(ip, comb.rctx_idx)));
                SETBIT(comb.vb_pending, ARRITEM(ctx, comb.rctx_idx, ctx.irq_idx), CONST("0", 1));
            ENDIF();

            IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
                IF (NZ(OR_REDUCE(BITS(wb_req_wstrb, 3, 0))));
                    SETARRITEM(comb.vb_ctx, comb.rctx_idx, comb.vb_ctx.priority_th, BITS(wb_req_wdata, 3, 0));
                ENDIF();
                IF (NZ(OR_REDUCE(BITS(wb_req_wstrb, 7,4))));
                     TEXT("claim/ complete. Reading clears pedning bit");
                    SETARRITEM(comb.vb_ctx, comb.rctx_idx, comb.vb_ctx.irq_idx, ALLZEROS());
                ENDIF();
            ENDIF();
        ELSE();
              TEXT("reserved");
        ENDIF();
    ENDIF();
    SETVAL(rdata, comb.vrdata);

TEXT();
    SETVAL(src_priority, comb.vb_src_priority);
    SETVAL(pending, comb.vb_pending);
    SETVAL(ip, comb.vb_ip);
    n = &FOR ("n", CONST("0"), ctxmax, "++");
        SETARRITEM(ctx, *n, ctx.priority_th, ARRITEM(comb.vb_ctx, *n, comb.vb_ctx.priority_th));
        SETARRITEM(ctx, *n, ctx.ie, ARRITEM(comb.vb_ctx, *n, comb.vb_ctx.ie));
        SETARRITEM(ctx, *n, ctx.ip_prio, ARRITEM(comb.vb_ctx, *n, comb.vb_ctx.ip_prio));
        SETARRITEM(ctx, *n, ctx.prio_mask, ARRITEM(comb.vb_ctx, *n, comb.vb_ctx.prio_mask));
        SETARRITEM(ctx, *n, ctx.sel_prio, ARRITEM(comb.vb_ctx, *n, comb.vb_ctx.sel_prio));
        SETARRITEM(ctx, *n, ctx.irq_idx, ARRITEM(comb.vb_ctx, *n, comb.vb_ctx.irq_idx));
        SETARRITEM(ctx, *n, ctx.irq_prio, ARRITEM(comb.vb_ctx, *n, comb.vb_ctx.irq_prio));
    ENDFOR();

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETONE(w_req_ready);
    SETONE(w_resp_valid);
    SETVAL(wb_resp_rdata, rdata);
    SETZERO(wb_resp_err);
    SETVAL(o_ip, ip);
}
