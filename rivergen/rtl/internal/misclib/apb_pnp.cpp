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

#include "apb_pnp.h"

apb_pnp::apb_pnp(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "apb_pnp", name, comment),
    cfg_slots(this, "cfg_slots", "1", NO_COMMENT),
    hw_id(this, "hwid", "32", "0x20221123", NO_COMMENT),
    cpu_max(this, "cpu_max", "1"),
    l2cache_ena(this, "l2cache_ena", "1"),
    plic_irq_max(this, "plic_irq_max", "127"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_mapinfo(this, "i_mapinfo", "interconnect slot information"),
    i_cfg(this, "i_cfg", "Device descriptors vector"),
    o_cfg(this, "o_cfg", "PNP Device descriptor"),
    i_apbi(this, "i_apbi", "APB  Slave to Bridge interface"),
    o_apbo(this, "o_apbo", "APB Bridge to Slave interface"),
    o_irq(this, "o_irq", "1", ""),
    // params
    // signals
    w_req_valid(this, "w_req_valid", "1"),
    wb_req_addr(this, "wb_req_addr", "32"),
    w_req_write(this, "w_req_write", "1"),
    wb_req_wdata(this, "wb_req_wdata", "32"),
    // registers
    fw_id(this, "fw_id", "32", "'0", NO_COMMENT),
    idt_l(this, "idt_l", "32", "'0", NO_COMMENT),
    idt_m(this, "idt_m", "32", "'0", NO_COMMENT),
    malloc_addr_l(this, "malloc_addr_l", "32", "'0", NO_COMMENT),
    malloc_addr_m(this, "malloc_addr_m", "32", "'0", NO_COMMENT),
    malloc_size_l(this, "malloc_size_l", "32", "'0", NO_COMMENT),
    malloc_size_m(this, "malloc_size_m", "32", "'0", NO_COMMENT),
    fwdbg1(this, "fwdbg1", "32", "'0", NO_COMMENT),
    fwdbg2(this, "fwdbg2", "32", "'0", NO_COMMENT),
    fwdbg3(this, "fwdbg3", "32", "'0", NO_COMMENT),
    fwdbg4(this, "fwdbg4", "32", "'0", NO_COMMENT),
    fwdbg5(this, "fwdbg5", "32", "'0", NO_COMMENT),
    fwdbg6(this, "fwdbg6", "32", "'0", NO_COMMENT),
    irq(this, "irq", "1"),
    resp_valid(this, "resp_valid", "1"),
    resp_rdata(this, "resp_rdata", "32", "'0", NO_COMMENT),
    resp_err(this, "resp_err", "1"),
    //
    comb(this),
    pslv0(this, "pslv0", NO_COMMENT)
{
    Operation::start(this);

    pslv0.vid.setObjValue(&glob_pnp_cfg_->VENDOR_OPTIMITECH);
    pslv0.did.setObjValue(&glob_pnp_cfg_->OPTIMITECH_PNP);
    NEW(pslv0, pslv0.getName().c_str());
        CONNECT(pslv0, 0, pslv0.i_clk, i_clk);
        CONNECT(pslv0, 0, pslv0.i_nrst, i_nrst);
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

void apb_pnp::proc_comb() {
    GenObject *i;
    SETZERO(irq);

TEXT();
    i = &FOR ("i", CONST("0"), cfg_slots, "++");
        SETARRITEM(comb.cfgmap, MUL2(CONST("8"), *i), comb.cfgmap,
                    CC3(CONST("0", 22),
                       ARRITEM(i_cfg, *i, i_cfg.descrtype),
                       ARRITEM(i_cfg, *i, i_cfg.descrsize)));
        SETARRITEM(comb.cfgmap, ADD2(MUL2(CONST("8"), *i), CONST("1")), comb.cfgmap,
                    CC2(ARRITEM(i_cfg, *i, i_cfg.vid),
                        ARRITEM(i_cfg, *i, i_cfg.did)));
        SETARRITEM(comb.cfgmap, ADD2(MUL2(CONST("8"), *i), CONST("4")), comb.cfgmap,
                    BITS(ARRITEM(i_cfg, *i, i_cfg.addr_start), 31, 0));
        SETARRITEM(comb.cfgmap, ADD2(MUL2(CONST("8"), *i), CONST("5")), comb.cfgmap,
                    BITS(ARRITEM(i_cfg, *i, i_cfg.addr_start), 63, 32));
        SETARRITEM(comb.cfgmap, ADD2(MUL2(CONST("8"), *i), CONST("6")), comb.cfgmap,
                    BITS(ARRITEM(i_cfg, *i, i_cfg.addr_end), 31, 0));
        SETARRITEM(comb.cfgmap, ADD2(MUL2(CONST("8"), *i), CONST("7")), comb.cfgmap,
                    BITS(ARRITEM(i_cfg, *i, i_cfg.addr_end), 63, 32));
    ENDFOR();

TEXT();
    IF (EQ(BITS(wb_req_addr, 11, 2), CONST("0", 9)));
        SETVAL(comb.vrdata, hw_id);
        IF(NZ(AND2_L(w_req_valid, w_req_write)));
            SETONE(irq);
        ENDIF();
    ELSIF (EQ(BITS(wb_req_addr, 11, 2), CONST("1", 9)));
        SETVAL(comb.vrdata, fw_id);
        IF(NZ(AND2_L(w_req_valid, w_req_write)));
            SETVAL(fw_id, wb_req_wdata);
        ENDIF();
    ELSIF (EQ(BITS(wb_req_addr, 11, 2), CONST("2", 9)));
        SETBITS(comb.vrdata, 31, 28, BITS(cpu_max, 3, 0));
        SETBIT(comb.vrdata, 24, l2cache_ena),
        SETBITS(comb.vrdata, 15, 8, BITS(cfg_slots, 7, 0)),
        SETBITS(comb.vrdata, 7, 0, BITS(plic_irq_max, 7, 0));
    ELSIF (EQ(BITS(wb_req_addr, 11, 2), CONST("3", 9)));
        SETZERO(comb.vrdata);
    ELSIF (EQ(BITS(wb_req_addr, 11, 2), CONST("4", 9)));
        SETVAL(comb.vrdata, idt_l);
        IF(NZ(AND2_L(w_req_valid, w_req_write)));
            SETVAL(idt_l, wb_req_wdata);
        ENDIF();
    ELSIF (EQ(BITS(wb_req_addr, 11, 2), CONST("5", 9)));
        SETVAL(comb.vrdata, idt_m);
        IF(NZ(AND2_L(w_req_valid, w_req_write)));
            SETVAL(idt_m, wb_req_wdata);
        ENDIF();
    ELSIF (EQ(BITS(wb_req_addr, 11, 2), CONST("6", 9)));
        SETVAL(comb.vrdata, malloc_addr_l);
        IF(NZ(AND2_L(w_req_valid, w_req_write)));
            SETVAL(malloc_addr_l, wb_req_wdata);
        ENDIF();
    ELSIF (EQ(BITS(wb_req_addr, 11, 2), CONST("7", 9)));
        SETVAL(comb.vrdata, malloc_addr_m);
        IF(NZ(AND2_L(w_req_valid, w_req_write)));
            SETVAL(malloc_addr_m, wb_req_wdata);
        ENDIF();
    ELSIF (EQ(BITS(wb_req_addr, 11, 2), CONST("8", 9)));
        SETVAL(comb.vrdata, malloc_size_l);
        IF(NZ(AND2_L(w_req_valid, w_req_write)));
            SETVAL(malloc_size_l, wb_req_wdata);
        ENDIF();
    ELSIF (EQ(BITS(wb_req_addr, 11, 2), CONST("9", 9)));
        SETVAL(comb.vrdata, malloc_size_m);
        IF(NZ(AND2_L(w_req_valid, w_req_write)));
            SETVAL(malloc_size_m, wb_req_wdata);
        ENDIF();
    ELSIF (EQ(BITS(wb_req_addr, 11, 2), CONST("10", 9)));
        SETVAL(comb.vrdata, fwdbg1);
        IF(NZ(AND2_L(w_req_valid, w_req_write)));
            SETVAL(fwdbg1, wb_req_wdata);
        ENDIF();
    ELSIF (EQ(BITS(wb_req_addr, 11, 2), CONST("11", 9)));
        SETVAL(comb.vrdata, fwdbg2);
        IF(NZ(AND2_L(w_req_valid, w_req_write)));
            SETVAL(fwdbg2, wb_req_wdata);
        ENDIF();
    ELSIF (EQ(BITS(wb_req_addr, 11, 2), CONST("12", 9)));
        SETVAL(comb.vrdata, fwdbg3);
        IF(NZ(AND2_L(w_req_valid, w_req_write)));
            SETVAL(fwdbg3, wb_req_wdata);
        ENDIF();
    ELSIF (EQ(BITS(wb_req_addr, 11, 2), CONST("13", 9)));
        SETVAL(comb.vrdata, fwdbg4);
        IF(NZ(AND2_L(w_req_valid, w_req_write)));
            SETVAL(fwdbg4, wb_req_wdata);
        ENDIF();
    ELSIF (EQ(BITS(wb_req_addr, 11, 2), CONST("14", 9)));
        SETVAL(comb.vrdata, fwdbg5);
        IF(NZ(AND2_L(w_req_valid, w_req_write)));
            SETVAL(fwdbg5, wb_req_wdata);
        ENDIF();
    ELSIF (EQ(BITS(wb_req_addr, 11, 2), CONST("15", 9)));
        SETVAL(comb.vrdata, fwdbg6);
        IF(NZ(AND2_L(w_req_valid, w_req_write)));
            SETVAL(fwdbg6, wb_req_wdata);
        ENDIF();
    ELSIF (ANDx(2, &GE(BITS(wb_req_addr, 11, 2), CONST("16", 9)),
                   &LS(BITS(wb_req_addr, 11, 2), ADD2(CONST("16"), MUL2(CONST("8"), cfg_slots)))));
        SETVAL(comb.vrdata, ARRITEM
            (comb.cfgmap, SUB2(TO_INT(BITS(wb_req_addr, 11, 2)), CONST("16")), comb.cfgmap));
    ENDIF();

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(resp_valid, w_req_valid);
    SETVAL(resp_rdata, comb.vrdata);
    SETZERO(resp_err);
    SETVAL(o_irq, irq);
}
