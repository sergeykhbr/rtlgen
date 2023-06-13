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

#include "axictrl_bus0.h"

axictrl_bus0::axictrl_bus0(GenObject *parent, const char *name) :
    ModuleObject(parent, "axictrl_bus0", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    o_cfg(this, "o_cfg", "Slave config descriptor"),
    i_xmsto(this, "i_xmsto", "AXI4 masters output vector"),
    o_xmsti(this, "o_xmsti", "AXI4 masters input vector"),
    i_xslvo(this, "i_xslvo", "AXI4 slaves output vectors"),
    o_xslvi(this, "o_xslvi", "AXI4 slaves input vectors"),
    o_mapinfo(this, "o_mapinfo", "AXI devices memory mapping information"),
    // params
    // signals
    wb_def_mapinfo(this, "wb_def_mapinfo"),
    wb_def_xslvi(this, "wb_def_xslvi"),
    wb_def_xslvo(this, "wb_def_xslvo"),
    w_def_req_valid(this, "w_def_req_valid", "1"),
    wb_def_req_addr(this, "wb_def_req_addr", "CFG_SYSBUS_ADDR_BITS"),
    wb_def_req_size(this, "wb_def_req_size", "8"),
    w_def_req_write(this, "w_def_req_write", "1"),
    wb_def_req_wdata(this, "wb_def_req_wdata", "CFG_SYSBUS_DATA_BITS"),
    wb_def_req_wstrb(this, "wb_def_req_wstrb", "CFG_SYSBUS_DATA_BYTES"),
    w_def_req_last(this, "w_def_req_last", "1"),
    w_def_req_ready(this, "w_def_req_ready", "1"),
    w_def_resp_valid(this, "w_def_resp_valid", "1"),
    wb_def_resp_rdata(this, "wb_def_resp_rdata", "CFG_SYSBUS_DATA_BITS"),
    w_def_resp_err(this, "w_def_resp_err", "1"),
    // registers
    r_midx(this, "r_midx", "CFG_BUS0_XMST_LOG2_TOTAL", "CFG_BUS0_XMST_TOTAL"),
    r_sidx(this, "r_sidx", "CFG_BUS0_XSLV_LOG2_TOTAL", "CFG_BUS0_XSLV_TOTAL"),
    w_midx(this, "w_midx", "CFG_BUS0_XMST_LOG2_TOTAL", "CFG_BUS0_XMST_TOTAL"),
    w_sidx(this, "w_sidx", "CFG_BUS0_XSLV_LOG2_TOTAL", "CFG_BUS0_XSLV_TOTAL"),
    b_midx(this, "b_midx", "CFG_BUS0_XMST_LOG2_TOTAL", "CFG_BUS0_XMST_TOTAL"),
    b_sidx(this, "b_sidx", "CFG_BUS0_XSLV_LOG2_TOTAL", "CFG_BUS0_XSLV_TOTAL"),
    // modules
    xdef0(this, "xdef0"),
    // process
    comb(this)
{
    Operation::start(this);
    xdef0.vid.setObjValue(&glob_pnp_cfg_->VENDOR_OPTIMITECH);
    xdef0.did.setObjValue(&glob_pnp_cfg_->OPTIMITECH_AXI_INTERCONNECT);
    NEW(xdef0, xdef0.getName().c_str());
        CONNECT(xdef0, 0, xdef0.i_clk, i_clk);
        CONNECT(xdef0, 0, xdef0.i_nrst, i_nrst);
        CONNECT(xdef0, 0, xdef0.i_mapinfo, wb_def_mapinfo);
        CONNECT(xdef0, 0, xdef0.o_cfg, o_cfg);
        CONNECT(xdef0, 0, xdef0.i_xslvi, wb_def_xslvi);
        CONNECT(xdef0, 0, xdef0.o_xslvo, wb_def_xslvo);
        CONNECT(xdef0, 0, xdef0.o_req_valid, w_def_req_valid);
        CONNECT(xdef0, 0, xdef0.o_req_addr, wb_def_req_addr);
        CONNECT(xdef0, 0, xdef0.o_req_size, wb_def_req_size);
        CONNECT(xdef0, 0, xdef0.o_req_write, w_def_req_write);
        CONNECT(xdef0, 0, xdef0.o_req_wdata, wb_def_req_wdata);
        CONNECT(xdef0, 0, xdef0.o_req_wstrb, wb_def_req_wstrb);
        CONNECT(xdef0, 0, xdef0.o_req_last, w_def_req_last);
        CONNECT(xdef0, 0, xdef0.i_req_ready, w_def_req_ready);
        CONNECT(xdef0, 0, xdef0.i_resp_valid, w_def_resp_valid);
        CONNECT(xdef0, 0, xdef0.i_resp_rdata, wb_def_resp_rdata);
        CONNECT(xdef0, 0, xdef0.i_resp_err, w_def_resp_err);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void axictrl_bus0::proc_comb() {
    types_amba* cfg = glob_types_amba_;
    types_bus0 *bus0 = glob_bus0_cfg_;
    GenObject *i;
    
    SETZERO(comb.vb_def_mapinfo.addr_start);
    SETZERO(comb.vb_def_mapinfo.addr_end);
    i = &FOR ("i", CONST("0"), bus0->CFG_BUS0_XMST_TOTAL, "++");
        SETARRITEM(comb.vmsto, *i, comb.vmsto, ARRITEM(i_xmsto, *i, i_xmsto), "Cannot read vector item from port in systemc");
        SETARRITEM(comb.vmsti, *i, comb.vmsti, cfg->axi4_master_in_none);
    ENDFOR();
    TEXT("Unmapped default slots:");
    SETARRITEM(comb.vmsto, bus0->CFG_BUS0_XMST_TOTAL, comb.vmsto, cfg->axi4_master_out_none);
    SETARRITEM(comb.vmsti, bus0->CFG_BUS0_XMST_TOTAL, comb.vmsti, cfg->axi4_master_in_none);

TEXT();
    i = &FOR ("i", CONST("0"), bus0->CFG_BUS0_XSLV_TOTAL, "++");
        SETARRITEM(comb.vslvo, *i, comb.vslvo, ARRITEM(i_xslvo, *i, i_xslvo), "Cannot read vector item from port in systemc");
        SETARRITEM(comb.vslvi, *i, comb.vslvi, cfg->axi4_slave_in_none);
    ENDFOR();
    TEXT("Unmapped default slots:");
    SETARRITEM(comb.vslvo, bus0->CFG_BUS0_XSLV_TOTAL, comb.vslvo, wb_def_xslvo);
    SETARRITEM(comb.vslvi, bus0->CFG_BUS0_XSLV_TOTAL, comb.vslvi, cfg->axi4_slave_in_none);

TEXT();
    SETONE(w_def_req_ready);
    SETONE(w_def_resp_valid);
    SETVAL(wb_def_resp_rdata, ALLONES());
    SETONE(w_def_resp_err);
    SETVAL(comb.i_ar_midx, bus0->CFG_BUS0_XMST_TOTAL);
    SETVAL(comb.i_aw_midx, bus0->CFG_BUS0_XMST_TOTAL);
    SETVAL(comb.i_ar_sidx, bus0->CFG_BUS0_XSLV_TOTAL);
    SETVAL(comb.i_aw_sidx, bus0->CFG_BUS0_XSLV_TOTAL);
    SETVAL(comb.i_r_midx, TO_INT(r_midx));
    SETVAL(comb.i_r_sidx, TO_INT(r_sidx));
    SETVAL(comb.i_w_midx, TO_INT(w_midx));
    SETVAL(comb.i_w_sidx, TO_INT(w_sidx));
    SETVAL(comb.i_b_midx, TO_INT(b_midx));
    SETVAL(comb.i_b_sidx, TO_INT(b_sidx));

TEXT();
    TEXT("Select Master bus:");
    i = &FOR ("i", CONST("0"), bus0->CFG_BUS0_XMST_TOTAL, "++");
        IF (NZ(ARRITEM(comb.vmsto, *i, comb.vmsto->ar_valid)));
            SETVAL(comb.i_ar_midx, *i);
        ENDIF();
        IF (NZ(ARRITEM(comb.vmsto, *i, comb.vmsto->aw_valid)));
            SETVAL(comb.i_aw_midx, *i);
        ENDIF();
    ENDFOR();

TEXT();
    TEXT("Select Slave interface:");
    i = &FOR ("i", CONST("0"), bus0->CFG_BUS0_XSLV_TOTAL, "++");
        IF (ANDx(2, &LE(ARRITEM(bus0->CFG_BUS0_MAP, *i, BITS(bus0->CFG_BUS0_MAP.addr_start, DEC(cfg->CFG_SYSBUS_ADDR_BITS), CONST("12"))),
                        ARRITEM(comb.vmsto, comb.i_ar_midx, BITS(comb.vmsto->ar_bits.addr, DEC(cfg->CFG_SYSBUS_ADDR_BITS), CONST("12")))), 
                    &LS(ARRITEM(comb.vmsto, comb.i_ar_midx, BITS(comb.vmsto->ar_bits.addr, DEC(cfg->CFG_SYSBUS_ADDR_BITS), CONST("12"))),
                        ARRITEM(bus0->CFG_BUS0_MAP, *i, BITS(bus0->CFG_BUS0_MAP.addr_end, DEC(cfg->CFG_SYSBUS_ADDR_BITS), CONST("12"))))));
            SETVAL(comb.i_ar_sidx, *i);
        ENDIF();
        IF (ANDx(2, &LE(ARRITEM(bus0->CFG_BUS0_MAP, *i, BITS(bus0->CFG_BUS0_MAP.addr_start, DEC(cfg->CFG_SYSBUS_ADDR_BITS), CONST("12"))),
                        ARRITEM(comb.vmsto, comb.i_aw_midx, BITS(comb.vmsto->aw_bits.addr, DEC(cfg->CFG_SYSBUS_ADDR_BITS), CONST("12")))), 
                    &LS(ARRITEM(comb.vmsto, comb.i_aw_midx, BITS(comb.vmsto->aw_bits.addr, DEC(cfg->CFG_SYSBUS_ADDR_BITS), CONST("12"))),
                        ARRITEM(bus0->CFG_BUS0_MAP, *i, BITS(bus0->CFG_BUS0_MAP.addr_end, DEC(cfg->CFG_SYSBUS_ADDR_BITS), CONST("12"))))));
            SETVAL(comb.i_aw_sidx, *i);
        ENDIF();
    ENDFOR();

TEXT();
    TEXT("Read Channel:");
    SETVAL(comb.v_ar_fire, AND2_L(ARRITEM(comb.vmsto, comb.i_ar_midx, comb.vmsto->ar_valid),
                                  ARRITEM(comb.vslvo, comb.i_ar_sidx, comb.vslvo->ar_ready)));
    SETVAL(comb.v_r_fire, AND3_L(ARRITEM(comb.vmsto, comb.i_r_midx, comb.vmsto->r_ready),
                                 ARRITEM(comb.vslvo, comb.i_r_sidx, comb.vslvo->r_valid),
                                 ARRITEM(comb.vslvo, comb.i_r_sidx, comb.vslvo->r_last)));
    TEXT("Write channel:");
    SETVAL(comb.v_aw_fire, AND2_L(ARRITEM(comb.vmsto, comb.i_aw_midx, comb.vmsto->aw_valid),
                                  ARRITEM(comb.vslvo, comb.i_aw_sidx, comb.vslvo->aw_ready)));
    SETVAL(comb.v_w_fire, AND3_L(ARRITEM(comb.vmsto, comb.i_w_midx, comb.vmsto->w_valid),
                                 ARRITEM(comb.vmsto, comb.i_w_midx, comb.vmsto->w_last),
                                 ARRITEM(comb.vslvo, comb.i_w_sidx, comb.vslvo->w_ready)));
    TEXT("Write confirm channel");
    SETVAL(comb.v_b_fire, AND2_L(ARRITEM(comb.vmsto, comb.i_b_midx, comb.vmsto->b_ready),
                                 ARRITEM(comb.vslvo, comb.i_b_sidx, comb.vslvo->b_valid)));

TEXT();
    IF (AND2(NE(r_sidx, bus0->CFG_BUS0_XSLV_TOTAL), EZ(comb.v_r_fire)));
        SETONE(comb.v_r_busy);
    ENDIF();

TEXT();
    IF (ORx(2, &AND2(NE(w_sidx, bus0->CFG_BUS0_XSLV_TOTAL), EZ(comb.v_w_fire)),
               &AND2(NE(b_sidx, bus0->CFG_BUS0_XSLV_TOTAL), EZ(comb.v_b_fire))));
        SETONE(comb.v_w_busy);
    ENDIF();

TEXT();
    IF (AND2(NE(b_sidx, bus0->CFG_BUS0_XSLV_TOTAL), EZ(comb.v_b_fire)));
        SETONE(comb.v_b_busy);
    ENDIF();

TEXT();
    IF (AND2(NZ(comb.v_ar_fire), EZ(comb.v_r_busy)));
        SETVAL(r_sidx, comb.i_ar_sidx);
        SETVAL(r_midx, comb.i_ar_midx);
    ELSIF (NZ(comb.v_r_fire));
        SETVAL(r_sidx, bus0->CFG_BUS0_XSLV_TOTAL);
        SETVAL(r_midx, bus0->CFG_BUS0_XMST_TOTAL);
    ENDIF();

TEXT();
    IF (AND2(NZ(comb.v_aw_fire), EZ(comb.v_w_busy)));
        SETVAL(w_sidx, comb.i_aw_sidx);
        SETVAL(w_midx, comb.i_aw_midx);
    ELSIF (AND2(NZ(comb.v_w_fire), EZ(comb.v_b_busy)));
        SETVAL(w_sidx, bus0->CFG_BUS0_XSLV_TOTAL);
        SETVAL(w_midx, bus0->CFG_BUS0_XMST_TOTAL);
    ENDIF();

TEXT();
    IF (AND2(NZ(comb.v_w_fire), EZ(comb.v_b_busy)));
        SETVAL(b_sidx, w_sidx);
        SETVAL(b_midx, w_midx);
    ELSIF (NZ(comb.v_b_fire));
        SETVAL(b_sidx, bus0->CFG_BUS0_XSLV_TOTAL);
        SETVAL(b_midx, bus0->CFG_BUS0_XMST_TOTAL);
    ENDIF();

TEXT();
    SETARRITEM(comb.vmsti, comb.i_ar_midx, comb.vmsti->ar_ready, AND2_L(ARRITEM(comb.vslvo, comb.i_ar_sidx, comb.vslvo->ar_ready),
                                                                         INV(comb.v_r_busy)));
    SETARRITEM(comb.vslvi, comb.i_ar_sidx, comb.vslvi->ar_valid, AND2_L(ARRITEM(comb.vmsto, comb.i_ar_midx, comb.vmsto->ar_valid),
                                                                         INV(comb.v_r_busy)));
    SETARRITEM(comb.vslvi, comb.i_ar_sidx, comb.vslvi->ar_bits, ARRITEM(comb.vmsto, comb.i_ar_midx, comb.vmsto->ar_bits));
    SETARRITEM(comb.vslvi, comb.i_ar_sidx, comb.vslvi->ar_id, ARRITEM(comb.vmsto, comb.i_ar_midx, comb.vmsto->ar_id));
    SETARRITEM(comb.vslvi, comb.i_ar_sidx, comb.vslvi->ar_user, ARRITEM(comb.vmsto, comb.i_ar_midx, comb.vmsto->ar_user));

TEXT();
    SETARRITEM(comb.vmsti, comb.i_r_midx, comb.vmsti->r_valid, ARRITEM(comb.vslvo, comb.i_r_sidx, comb.vslvo->r_valid));
    SETARRITEM(comb.vmsti, comb.i_r_midx, comb.vmsti->r_resp, ARRITEM(comb.vslvo, comb.i_r_sidx, comb.vslvo->r_resp));
    SETARRITEM(comb.vmsti, comb.i_r_midx, comb.vmsti->r_data, ARRITEM(comb.vslvo, comb.i_r_sidx, comb.vslvo->r_data));
    SETARRITEM(comb.vmsti, comb.i_r_midx, comb.vmsti->r_last, ARRITEM(comb.vslvo, comb.i_r_sidx, comb.vslvo->r_last));
    SETARRITEM(comb.vmsti, comb.i_r_midx, comb.vmsti->r_id, ARRITEM(comb.vslvo, comb.i_r_sidx, comb.vslvo->r_id));
    SETARRITEM(comb.vmsti, comb.i_r_midx, comb.vmsti->r_user, ARRITEM(comb.vslvo, comb.i_r_sidx, comb.vslvo->r_user));
    SETARRITEM(comb.vslvi, comb.i_r_sidx, comb.vslvi->r_ready, ARRITEM(comb.vmsto, comb.i_r_midx, comb.vmsto->r_ready));

TEXT();
    SETARRITEM(comb.vmsti, comb.i_aw_midx, comb.vmsti->aw_ready, AND2_L(ARRITEM(comb.vslvo, comb.i_aw_sidx, comb.vslvo->aw_ready),
                                                                         INV(comb.v_w_busy)));
    SETARRITEM(comb.vslvi, comb.i_aw_sidx, comb.vslvi->aw_valid, AND2_L(ARRITEM(comb.vmsto, comb.i_aw_midx, comb.vmsto->aw_valid),
                                                                         INV(comb.v_w_busy)));
    SETARRITEM(comb.vslvi, comb.i_aw_sidx, comb.vslvi->aw_bits, ARRITEM(comb.vmsto, comb.i_aw_midx, comb.vmsto->aw_bits));
    SETARRITEM(comb.vslvi, comb.i_aw_sidx, comb.vslvi->aw_id, ARRITEM(comb.vmsto, comb.i_aw_midx, comb.vmsto->aw_id));
    SETARRITEM(comb.vslvi, comb.i_aw_sidx, comb.vslvi->aw_user, ARRITEM(comb.vmsto, comb.i_aw_midx, comb.vmsto->aw_user));

TEXT();
    SETARRITEM(comb.vmsti, comb.i_w_midx, comb.vmsti->w_ready, AND2_L(ARRITEM(comb.vslvo, comb.i_w_sidx, comb.vslvo->w_ready),
                                                                      INV(comb.v_b_busy)));
    SETARRITEM(comb.vslvi, comb.i_w_sidx, comb.vslvi->w_valid, AND2_L(ARRITEM(comb.vmsto, comb.i_w_midx, comb.vmsto->w_valid),
                                                                      INV(comb.v_b_busy)));
    SETARRITEM(comb.vslvi, comb.i_w_sidx, comb.vslvi->w_data, ARRITEM(comb.vmsto, comb.i_w_midx, comb.vmsto->w_data));
    SETARRITEM(comb.vslvi, comb.i_w_sidx, comb.vslvi->w_last, ARRITEM(comb.vmsto, comb.i_w_midx, comb.vmsto->w_last));
    SETARRITEM(comb.vslvi, comb.i_w_sidx, comb.vslvi->w_strb, ARRITEM(comb.vmsto, comb.i_w_midx, comb.vmsto->w_strb));
    SETARRITEM(comb.vslvi, comb.i_w_sidx, comb.vslvi->w_user, ARRITEM(comb.vmsto, comb.i_w_midx, comb.vmsto->w_user));

TEXT();
    SETARRITEM(comb.vmsti, comb.i_b_midx, comb.vmsti->b_valid, ARRITEM(comb.vslvo, comb.i_b_sidx, comb.vslvo->b_valid));
    SETARRITEM(comb.vmsti, comb.i_b_midx, comb.vmsti->b_resp, ARRITEM(comb.vslvo, comb.i_b_sidx, comb.vslvo->b_resp));
    SETARRITEM(comb.vmsti, comb.i_b_midx, comb.vmsti->b_id, ARRITEM(comb.vslvo, comb.i_b_sidx, comb.vslvo->b_id));
    SETARRITEM(comb.vmsti, comb.i_b_midx, comb.vmsti->b_user, ARRITEM(comb.vslvo, comb.i_b_sidx, comb.vslvo->b_user));
    SETARRITEM(comb.vslvi, comb.i_b_sidx, comb.vslvi->b_ready, ARRITEM(comb.vmsto, comb.i_b_midx, comb.vmsto->b_ready));

TEXT();
    SYNC_RESET(*this);

TEXT();
     i = &FOR ("i", CONST("0"), bus0->CFG_BUS0_XMST_TOTAL, "++");
        SETARRITEM(o_xmsti, *i, o_xmsti, ARRITEM(comb.vmsti, *i, comb.vmsti));
    ENDFOR();
     i = &FOR ("i", CONST("0"), bus0->CFG_BUS0_XSLV_TOTAL, "++");
        SETARRITEM(o_xslvi, *i, o_xslvi, ARRITEM(comb.vslvi, *i, comb.vslvi));
        SETARRITEM(o_mapinfo, *i, o_mapinfo, ARRITEM(bus0->CFG_BUS0_MAP, *i, bus0->CFG_BUS0_MAP));
    ENDFOR();
    SETVAL(wb_def_xslvi, ARRITEM(comb.vslvi, bus0->CFG_BUS0_XSLV_TOTAL, comb.vslvi));
    SETVAL(wb_def_mapinfo, comb.vb_def_mapinfo);
}
