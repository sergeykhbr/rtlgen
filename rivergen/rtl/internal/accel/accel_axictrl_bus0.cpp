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

#include "accel_axictrl_bus0.h"

accel_axictrl_bus0::accel_axictrl_bus0(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "accel_axictrl_bus0", name, comment),
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
    wb_def_xslvi(this, "wb_def_xslvi", NO_COMMENT),
    wb_def_xslvo(this, "wb_def_xslvo", NO_COMMENT),
    wb_def_mapinfo(this, "wb_def_mapinfo", NO_COMMENT),
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
    w_select(this, "w_select", "MUL(CFG_BUS0_XMST_TOTAL, CFG_BUS0_XSLV_LOG2_TOTAL)", "'0", NO_COMMENT),
    w_active(this, "w_active", "CFG_BUS0_XMST_TOTAL", "'0", NO_COMMENT),
    r_def_valid(this, "r_def_valid", "1", RSTVAL_ZERO, NO_COMMENT),
    // modules
    xdef0(this, "xdef0", NO_COMMENT),
    // process
    comb(this)
{
    Operation::start(this);
    xdef0.vid.setObjValue(SCV_get_cfg_type(this, "VENDOR_OPTIMITECH"));
    xdef0.did.setObjValue(SCV_get_cfg_type(this, "OPTIMITECH_AXI_INTERCONNECT"));
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

void accel_axictrl_bus0::proc_comb() {
    GenObject *i;
    GenObject *ii;
    GenObject *mst_total = SCV_get_cfg_type(this, "CFG_BUS0_XMST_TOTAL");
    GenObject *mst_log2 = SCV_get_cfg_type(this, "CFG_BUS0_XMST_LOG2_TOTAL");
    GenObject *slv_total = SCV_get_cfg_type(this, "CFG_BUS0_XSLV_TOTAL");
    GenObject *slv_log2 = SCV_get_cfg_type(this, "CFG_BUS0_XSLV_LOG2_TOTAL");
    types_accel_bus0::CONST_CFG_BUS0_MAP *map = 
        dynamic_cast<types_accel_bus0::CONST_CFG_BUS0_MAP *>(SCV_get_cfg_type(this, "CFG_BUS0_MAP"));

    TEXT();
    i = &FOR ("i", CONST("0"), *mst_total, "++");
        SETARRITEM(comb.vmsto, *i, comb.vmsto, ARRITEM(i_xmsto, *i, i_xmsto), "Cannot read vector item from port in systemc");
        SETARRITEM(comb.vmsti, *i, comb.vmsti, *SCV_get_cfg_type(this, "axi4_master_in_none"));
    ENDFOR();

    TEXT();
    i = &FOR ("i", CONST("0"), *slv_total, "++");
        SETARRITEM(comb.vslvo, *i, comb.vslvo, ARRITEM(i_xslvo, *i, i_xslvo), "Cannot read vector item from port in systemc");
        SETARRITEM(comb.vslvi, *i, comb.vslvi, *SCV_get_cfg_type(this, "axi4_slave_in_none"));
    ENDFOR();
    TEXT("Local unmapped slots:");
    SETARRITEM(comb.vslvo, DEC(*slv_total), comb.vslvo, wb_def_xslvo);

    TEXT();
    i = &FOR ("i", CONST("0"), *mst_total, "++");
        ii = &FOR ("ii", CONST("0"), DEC(*slv_total), "++");
            TEXT("Connect AR channel");
            IF (ANDx(2, &LE(ARRITEM(*map, *ii, BITS(map->addr_start, DEC(*SCV_get_cfg_type(this, "CFG_SYSBUS_ADDR_BITS")), CONST("12"))),
                            ARRITEM(comb.vmsto, *i, BITS(comb.vmsto.ar_bits.addr, DEC(*SCV_get_cfg_type(this, "CFG_SYSBUS_ADDR_BITS")), CONST("12")))), 
                        &LS(ARRITEM(comb.vmsto, *i, BITS(comb.vmsto.ar_bits.addr, DEC(*SCV_get_cfg_type(this, "CFG_SYSBUS_ADDR_BITS")), CONST("12"))),
                            ARRITEM(*map, *ii, BITS(map->addr_end, DEC(*SCV_get_cfg_type(this, "CFG_SYSBUS_ADDR_BITS")), CONST("12"))))));
                SETBIT(comb.vb_ar_hit, *i, ARRITEM(comb.vmsto, *i, comb.vmsto.ar_valid));
                SETBIT(comb.vb_ar_select, ADD2(MUL2(*i, *slv_total), *ii),
                        AND2(ARRITEM(comb.vmsto, *i, comb.vmsto.ar_valid),
                             BIT(comb.vb_ar_available, ADD2(MUL2(*i, *slv_total), *ii))));
                TEXT("Update availability bit for the next master and this slave:");
                SETBIT(comb.vb_ar_available, ADD2(MUL2(INC(*i), *slv_total), *ii),
                        AND2(INV_L(ARRITEM(comb.vmsto, *i, comb.vmsto.ar_valid)),
                                   BIT(comb.vb_ar_available, ADD2(MUL2(*i, *slv_total), *ii))));
            ENDIF();

            TEXT();
            TEXT("Connect AW channel");
            IF (ANDx(2, &LE(ARRITEM(*map, *ii, BITS(map->addr_start, DEC(*SCV_get_cfg_type(this, "CFG_SYSBUS_ADDR_BITS")), CONST("12"))),
                            ARRITEM(comb.vmsto, *i, BITS(comb.vmsto.aw_bits.addr, DEC(*SCV_get_cfg_type(this, "CFG_SYSBUS_ADDR_BITS")), CONST("12")))), 
                        &LS(ARRITEM(comb.vmsto, *i, BITS(comb.vmsto.aw_bits.addr, DEC(*SCV_get_cfg_type(this, "CFG_SYSBUS_ADDR_BITS")), CONST("12"))),
                            ARRITEM(*map, *ii, BITS(map->addr_end, DEC(*SCV_get_cfg_type(this, "CFG_SYSBUS_ADDR_BITS")), CONST("12"))))));
                SETBIT(comb.vb_aw_hit, *i, ARRITEM(comb.vmsto, *i, comb.vmsto.aw_valid));
                SETBIT(comb.vb_aw_select, ADD2(MUL2(*i, *slv_total), *ii),
                        AND2(ARRITEM(comb.vmsto, *i, comb.vmsto.aw_valid),
                             BIT(comb.vb_aw_available, ADD2(MUL2(*i, *slv_total), *ii))));
                TEXT("Update availability bit for the next master and this slave:");
                SETBIT(comb.vb_aw_available, ADD2(MUL2(INC(*i), *slv_total), *ii),
                        AND2(INV_L(ARRITEM(comb.vmsto, *i, comb.vmsto.aw_valid)),
                                   BIT(comb.vb_aw_available, ADD2(MUL2(*i, *slv_total), *ii))));
            ENDIF();
        ENDFOR();
    ENDFOR();

    TEXT();
    TEXT("access to unmapped slave:");
    i = &FOR ("i", CONST("0"), *mst_total, "++");
        IF (AND2(NZ(ARRITEM(comb.vmsto, *i, comb.vmsto.ar_valid)), EZ(BIT(comb.vb_ar_hit, *i))));
            SETBIT(comb.vb_ar_select, ADD2(MUL2(*i, *slv_total), DEC(*slv_total)),
                    AND2(ARRITEM(comb.vmsto, *i, comb.vmsto.ar_valid),
                            BIT(comb.vb_ar_available, ADD2(MUL2(*i, *slv_total), DEC(*slv_total)))));
        ENDIF();
        SETBIT(comb.vb_ar_available, ADD2(MUL2(INC(*i), *slv_total), DEC(*slv_total)),
                ANDx(2, &INV_L(BIT(comb.vb_ar_select, ADD2(MUL2(*i, *slv_total), DEC(*slv_total)))),
                        &BIT(comb.vb_ar_available, ADD2(MUL2(*i, *slv_total), DEC(*slv_total)))));

        TEXT();
        IF (AND2(NZ(ARRITEM(comb.vmsto, *i, comb.vmsto.aw_valid)), EZ(BIT(comb.vb_aw_hit, *i))));
            SETBIT(comb.vb_aw_select, ADD2(MUL2(*i, *slv_total), DEC(*slv_total)),
                    AND2(ARRITEM(comb.vmsto, *i, comb.vmsto.aw_valid),
                            BIT(comb.vb_aw_available, ADD2(MUL2(*i, *slv_total), DEC(*slv_total)))));
        ENDIF();
        SETBIT(comb.vb_aw_available, ADD2(MUL2(INC(*i), *slv_total), DEC(*slv_total)),
                ANDx(2, &INV_L(BIT(comb.vb_aw_select, ADD2(MUL2(*i, *slv_total), DEC(*slv_total)))),
                        &BIT(comb.vb_aw_available, ADD2(MUL2(*i, *slv_total), DEC(*slv_total)))));
    ENDFOR();

    TEXT();
    SETVAL(comb.vb_w_select, w_select);
    SETVAL(comb.vb_w_active, w_active);
    i = &FOR ("i", CONST("0"), *mst_total, "++");
        ii = &FOR ("ii", CONST("0"), *slv_total, "++");
            IF (NZ(BIT(comb.vb_ar_select, ADD2(MUL2(*i, *slv_total), *ii))));
                SETARRITEM(comb.vmsti, *i, comb.vmsti.ar_ready, ARRITEM(comb.vslvo, *ii, comb.vslvo.ar_ready));
                SETARRITEM(comb.vslvi, *ii, comb.vslvi.ar_valid, ARRITEM(comb.vmsto, *i, comb.vmsto.ar_valid));
                SETARRITEM(comb.vslvi, *ii, comb.vslvi.ar_bits, ARRITEM(comb.vmsto, *i, comb.vmsto.ar_bits));
                SETARRITEM(comb.vslvi, *ii, comb.vslvi.ar_user, ARRITEM(comb.vmsto, *i, comb.vmsto.ar_user));
                SETARRITEM(comb.vslvi, *ii, comb.vslvi.ar_id, 
                    CC2(ARRITEM(comb.vmsto, *i, comb.vmsto.ar_id), TO_LOGIC(*i, *mst_log2)));
            ENDIF();
            IF (NZ(BIT(comb.vb_aw_select, ADD2(MUL2(*i, *slv_total), *ii))));
                SETARRITEM(comb.vmsti, *i, comb.vmsti.aw_ready, ARRITEM(comb.vslvo, *ii, comb.vslvo.aw_ready));
                SETARRITEM(comb.vslvi, *ii, comb.vslvi.aw_valid, ARRITEM(comb.vmsto, *i, comb.vmsto.aw_valid));
                SETARRITEM(comb.vslvi, *ii, comb.vslvi.aw_bits, ARRITEM(comb.vmsto, *i, comb.vmsto.aw_bits));
                SETARRITEM(comb.vslvi, *ii, comb.vslvi.aw_user, ARRITEM(comb.vmsto, *i, comb.vmsto.aw_user));
                SETARRITEM(comb.vslvi, *ii, comb.vslvi.aw_id, 
                    CC2(ARRITEM(comb.vmsto, *i, comb.vmsto.aw_id), TO_LOGIC(*i, *mst_log2)));
                IF (NZ(ARRITEM(comb.vslvo, *ii, comb.vslvo.aw_ready)));
                    TEXT("Switch W-channel index to future w-transaction without id");
                    SETBITSW(comb.vb_w_select, MUL2(*i, *slv_log2), *slv_log2, TO_LOGIC(*ii, *slv_log2));
                    SETBIT(comb.vb_w_active, *i, CONST("1", 1));
                ENDIF();
            ENDIF();
        ENDFOR();
    ENDFOR();
    SETVAL(w_select, comb.vb_w_select);

    TEXT();
    TEXT("W-channel");
    i = &FOR ("i", CONST("0"), *mst_total, "++");
        IF (AND2(NZ(ARRITEM(comb.vmsto, *i, comb.vmsto.w_valid)), NZ(BIT(w_active, *i))));
            SETARRITEM(comb.vmsti, *i, comb.vmsti.w_ready,
                ARRITEM(comb.vslvo, TO_INT(BITSW(w_select, MUL2(*i, *slv_log2), *slv_log2)), comb.vslvo.w_ready));
            SETARRITEM(comb.vslvi, TO_INT(BITSW(w_select, MUL2(*i, *slv_log2), *slv_log2)), comb.vslvi.w_valid,
                ARRITEM(comb.vmsto, *i, comb.vmsto.w_valid));
            SETARRITEM(comb.vslvi, TO_INT(BITSW(w_select, MUL2(*i, *slv_log2), *slv_log2)), comb.vslvi.w_data,
                ARRITEM(comb.vmsto, *i, comb.vmsto.w_data));
            SETARRITEM(comb.vslvi, TO_INT(BITSW(w_select, MUL2(*i, *slv_log2), *slv_log2)), comb.vslvi.w_strb,
                ARRITEM(comb.vmsto, *i, comb.vmsto.w_strb));
            SETARRITEM(comb.vslvi, TO_INT(BITSW(w_select, MUL2(*i, *slv_log2), *slv_log2)), comb.vslvi.w_last,
                ARRITEM(comb.vmsto, *i, comb.vmsto.w_last));
            SETARRITEM(comb.vslvi, TO_INT(BITSW(w_select, MUL2(*i, *slv_log2), *slv_log2)), comb.vslvi.w_user,
                ARRITEM(comb.vmsto, *i, comb.vmsto.w_user));
            IF (ANDx(2, &NZ(ARRITEM(comb.vmsto, *i, comb.vmsto.w_last)),
                        &NZ(ARRITEM(comb.vslvo, TO_INT(BITSW(w_select, MUL2(*i, *slv_log2), *slv_log2)), comb.vslvo.w_ready))));
                SETBIT(comb.vb_w_active, *i, CONST("0", 1));
            ENDIF();
        ENDIF();
    ENDFOR();
    SETVAL(w_active, comb.vb_w_active);

    TEXT();
    TEXT("B-channel");
    ii = &FOR ("ii", CONST("0"), *slv_total, "++");
        IF (NZ(ARRITEM(comb.vslvo, *ii, comb.vslvo.b_valid)));
            SETARRITEM(comb.vslvi, *ii, comb.vslvi.b_ready,
                ARRITEM(comb.vmsto, TO_INT(ARRITEM(comb.vslvo, *ii, BITS(comb.vslvo.b_id, DEC(*mst_log2), CONST("0")))), comb.vmsto.b_ready));
            SETARRITEM(comb.vmsti, TO_INT(ARRITEM(comb.vslvo, *ii, BITS(comb.vslvo.b_id, DEC(*mst_log2), CONST("0")))), comb.vmsti.b_valid,
                ARRITEM(comb.vslvo, *ii, comb.vslvo.b_valid));
            SETARRITEM(comb.vmsti, TO_INT(ARRITEM(comb.vslvo, *ii, BITS(comb.vslvo.b_id, DEC(*mst_log2), CONST("0")))), comb.vmsti.b_resp,
                ARRITEM(comb.vslvo, *ii, comb.vslvo.b_resp));
            SETARRITEM(comb.vmsti, TO_INT(ARRITEM(comb.vslvo, *ii, BITS(comb.vslvo.b_id, DEC(*mst_log2), CONST("0")))), comb.vmsti.b_user,
                ARRITEM(comb.vslvo, *ii, comb.vslvo.b_user));
            SETARRITEM(comb.vmsti, TO_INT(ARRITEM(comb.vslvo, *ii, BITS(comb.vslvo.b_id, DEC(*mst_log2), CONST("0")))), comb.vmsti.b_id,
                ARRITEM(comb.vslvo, *ii, BITS(comb.vslvo.b_id, DEC(*SCV_get_cfg_type(this, "CFG_SYSBUS_ID_BITS")), *mst_log2)));
        ENDIF();
    ENDFOR();

    TEXT();
    TEXT("R-channel");
    ii = &FOR ("ii", CONST("0"), *slv_total, "++");
        IF (NZ(ARRITEM(comb.vslvo, *ii, comb.vslvo.r_valid)));
            SETARRITEM(comb.vslvi, *ii, comb.vslvi.r_ready,
                ARRITEM(comb.vmsto, TO_INT(ARRITEM(comb.vslvo, *ii, BITS(comb.vslvo.r_id, DEC(*mst_log2), CONST("0")))), comb.vmsto.r_ready));
            SETARRITEM(comb.vmsti, TO_INT(ARRITEM(comb.vslvo, *ii, BITS(comb.vslvo.r_id, DEC(*mst_log2), CONST("0")))), comb.vmsti.r_valid,
                ARRITEM(comb.vslvo, *ii, comb.vslvo.r_valid));
            SETARRITEM(comb.vmsti, TO_INT(ARRITEM(comb.vslvo, *ii, BITS(comb.vslvo.r_id, DEC(*mst_log2), CONST("0")))), comb.vmsti.r_data,
                ARRITEM(comb.vslvo, *ii, comb.vslvo.r_data));
            SETARRITEM(comb.vmsti, TO_INT(ARRITEM(comb.vslvo, *ii, BITS(comb.vslvo.r_id, DEC(*mst_log2), CONST("0")))), comb.vmsti.r_last,
                ARRITEM(comb.vslvo, *ii, comb.vslvo.r_last));
            SETARRITEM(comb.vmsti, TO_INT(ARRITEM(comb.vslvo, *ii, BITS(comb.vslvo.r_id, DEC(*mst_log2), CONST("0")))), comb.vmsti.r_resp,
                ARRITEM(comb.vslvo, *ii, comb.vslvo.r_resp));
            SETARRITEM(comb.vmsti, TO_INT(ARRITEM(comb.vslvo, *ii, BITS(comb.vslvo.r_id, DEC(*mst_log2), CONST("0")))), comb.vmsti.r_user,
                ARRITEM(comb.vslvo, *ii, comb.vslvo.r_user));
            SETARRITEM(comb.vmsti, TO_INT(ARRITEM(comb.vslvo, *ii, BITS(comb.vslvo.r_id, DEC(*mst_log2), CONST("0")))), comb.vmsti.r_id,
                ARRITEM(comb.vslvo, *ii, BITS(comb.vslvo.r_id, DEC(*SCV_get_cfg_type(this, "CFG_SYSBUS_ID_BITS")), *mst_log2)));
        ENDIF();
    ENDFOR();
    
    TEXT();
    SETONE(w_def_req_ready);
    SETVAL(r_def_valid, w_def_req_valid);
    SETVAL(w_def_resp_valid, r_def_valid);
    SETVAL(wb_def_resp_rdata, ALLONES());
    SETONE(w_def_resp_err);

    TEXT();
    SYNC_RESET();

    TEXT();
    i = &FOR ("i", CONST("0"), *mst_total, "++");
        SETARRITEM(o_xmsti, *i, o_xmsti, ARRITEM(comb.vmsti, *i, comb.vmsti));
    ENDFOR();
    i = &FOR ("i", CONST("0"), *slv_total, "++");
        SETARRITEM(o_xslvi, *i, o_xslvi, ARRITEM(comb.vslvi, *i, comb.vslvi));
        SETARRITEM(o_mapinfo, *i, o_mapinfo, ARRITEM(*map, *i, *map));
    ENDFOR();
    SETVAL(wb_def_xslvi, ARRITEM(comb.vslvi, DEC(*slv_total), comb.vslvi));
    SETVAL(wb_def_mapinfo, ARRITEM(*map, DEC(*slv_total), *map));
}
