// 
//  Copyright 2025 Sergey Khabarov, sergeykhbr@gmail.com
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

#include "axi_slv_tb.h"

axi_slv_tb::axi_slv_tb(GenObject *parent, const char *name) :
    ModuleObject(parent, "axi_slv_tb", name, NO_COMMENT),
    // parameters
    // Ports
    w_nrst(this, "w_nrst", "1", "1", "Power-on system reset active LOW"),
    w_clk(this, "w_clk", "1", RSTVAL_ZERO, NO_COMMENT),
    clk_cnt(this, &w_clk, CLK_POSEDGE, &w_nrst, ACTIVE_LOW, "wb_clk_cnt", "32", "'0", NO_COMMENT),
    test_cnt(this, &w_clk, CLK_POSEDGE, &w_nrst, ACTIVE_LOW, "test_cnt", "32", "'0", NO_COMMENT),
    wb_mst_o_cfg(this, "wb_mst_o_cfg", "Master config descriptor"),
    wb_xslvi(this, "wb_xslvi", NO_COMMENT),
    wb_xslvo(this, "wb_xslvo", NO_COMMENT),
    wb_slv_o_cfg(this, "wb_slv_o_cfg", "Slave config descriptor"),
    wb_slv_mapinfo(this, "wb_slv_mapinfo", NO_COMMENT),
    w_slv_o_req_valid(this, "w_slv_o_req_valid", "1"),
    wb_slv_o_req_addr(this, "wb_slv_o_req_addr", "CFG_SYSBUS_ADDR_BITS"),
    wb_slv_o_req_size(this, "wb_slv_o_req_size", "8"),
    w_slv_o_req_write(this, "w_slv_o_req_write", "1"),
    wb_slv_o_req_wdata(this, "wb_slv_o_req_wdata", "CFG_SYSBUS_DATA_BITS"),
    wb_slv_o_req_wstrb(this, "wb_slv_o_req_wstrb", "CFG_SYSBUS_DATA_BYTES"),
    w_slv_o_req_last(this, "w_slv_o_req_last", "1"),
    w_slv_i_req_ready(this, "w_slv_i_req_ready", "1"),
    w_slv_i_resp_valid(this, "w_slv_i_resp_valid", "1"),
    wb_slv_i_resp_rdata(this, "wb_slv_i_resp_rdata", "CFG_SYSBUS_DATA_BITS"),
    w_slv_i_resp_err(this, "w_slv_i_resp_err", "1"),
    v_busy(this, "v_busy", "1", RSTVAL_ZERO, NO_COMMENT),
    rd_valid(this, "rd_valid", "3", RSTVAL_ZERO, NO_COMMENT),
    req_ready(this, "req_ready", "1", RSTVAL_ZERO, NO_COMMENT),
    rd_addr(this, "rd_addr", "4", "'0", NO_COMMENT),
    rd_data(this, "rd_data", "64", "'0", NO_COMMENT),
    mem(this, "mem", "64", "16", NO_COMMENT),
    // submodules:
    clk(this, "clk", NO_COMMENT),
    slv0(this, "slv0", NO_COMMENT),
    // processes:
    comb(this),
    test_clk(this, &w_clk)
{
    Operation::start(this);

    clk.period.setObjValue(new FloatConst(25.0));  // 40 MHz
    NEW(clk, clk.getName().c_str());
        CONNECT(clk, 0, clk.o_clk, w_clk);
    ENDNEW();

TEXT();
    NEW(slv0, slv0.getName().c_str());
        CONNECT(slv0, 0, slv0.i_clk, w_clk);
        CONNECT(slv0, 0, slv0.i_nrst, w_nrst);
        CONNECT(slv0, 0, slv0.i_mapinfo, wb_slv_mapinfo);
        CONNECT(slv0, 0, slv0.o_cfg, wb_slv_o_cfg);
        CONNECT(slv0, 0, slv0.i_xslvi, wb_xslvi);
        CONNECT(slv0, 0, slv0.o_xslvo, wb_xslvo);
        CONNECT(slv0, 0, slv0.o_req_valid, w_slv_o_req_valid);
        CONNECT(slv0, 0, slv0.o_req_addr, wb_slv_o_req_addr);
        CONNECT(slv0, 0, slv0.o_req_size, wb_slv_o_req_size);
        CONNECT(slv0, 0, slv0.o_req_write, w_slv_o_req_write);
        CONNECT(slv0, 0, slv0.o_req_wdata, wb_slv_o_req_wdata);
        CONNECT(slv0, 0, slv0.o_req_wstrb, wb_slv_o_req_wstrb);
        CONNECT(slv0, 0, slv0.o_req_last, w_slv_o_req_last);
        CONNECT(slv0, 0, slv0.i_req_ready, w_slv_i_req_ready);
        CONNECT(slv0, 0, slv0.i_resp_valid, w_slv_i_resp_valid);
        CONNECT(slv0, 0, slv0.i_resp_rdata, wb_slv_i_resp_rdata);
        CONNECT(slv0, 0, slv0.i_resp_err, w_slv_i_resp_err);
    ENDNEW();

    INITIAL();
        SETZERO(w_nrst);
        SETVAL_DELAY(w_nrst, CONST("1", 1), *new FloatConst(1000.0));
    ENDINITIAL();

    Operation::start(&comb);
    proc_comb();

    Operation::start(&test_clk);
    proc_test_clk();
}

void axi_slv_tb::proc_comb() {
    SETVAL(comb.vb_mapinfo.addr_start, CONST("0x80000000", 48));
    SETVAL(comb.vb_mapinfo.addr_end, CONST("0x80001000", 48));
    SETVAL(wb_slv_mapinfo, comb.vb_mapinfo);

    TEXT();
    SETVAL(clk_cnt, INC(clk_cnt));

    TEXT();
    IF (EQ(test_cnt, CONST("0", 32)));
        SETVAL(comb.vb_xslvi.ar_valid, CONST("1", 1));
        SETVAL(comb.vb_xslvi.ar_bits.addr, CONST("0x10", 48));
        SETVAL(comb.vb_xslvi.ar_bits.len, CONST("0", 8));

        SETVAL(comb.vb_xslvi.aw_valid, CONST("1", 1));
        SETVAL(comb.vb_xslvi.aw_bits.addr, CONST("0x10", 48));
        SETVAL(comb.vb_xslvi.aw_bits.len, CONST("0", 8));

        SETVAL(comb.vb_xslvi.w_valid, CONST("1", 1));
        SETVAL(comb.vb_xslvi.w_data, CONST("1", 64));
        SETVAL(comb.vb_xslvi.w_last, CONST("1", 1));
    ELSE ();
    ENDIF();

    TEXT();
    SETVAL(comb.vb_xslvi.ar_bits.size, CONST("3", 3));
    SETVAL(comb.vb_xslvi.ar_bits.burst, CONST("1", 2));
    SETVAL(comb.vb_xslvi.ar_bits.lock, CONST("0", 1));
    SETVAL(comb.vb_xslvi.ar_bits.cache, CONST("0", 1));
    SETVAL(comb.vb_xslvi.ar_bits.prot, CONST("0", 3));
    SETVAL(comb.vb_xslvi.ar_bits.qos, CONST("0", 4));
    SETVAL(comb.vb_xslvi.ar_bits.region, CONST("0", 4));
    SETVAL(comb.vb_xslvi.aw_bits.size, CONST("3", 3));
    SETVAL(comb.vb_xslvi.aw_bits.burst, CONST("1", 2));
    SETVAL(comb.vb_xslvi.aw_bits.lock, CONST("0", 1));
    SETVAL(comb.vb_xslvi.aw_bits.cache, CONST("0", 1));
    SETVAL(comb.vb_xslvi.aw_bits.prot, CONST("0", 3));
    SETVAL(comb.vb_xslvi.aw_bits.qos, CONST("0", 4));
    SETVAL(comb.vb_xslvi.aw_bits.region, CONST("0", 4));
    SETVAL(comb.vb_xslvi.aw_id, ALLZEROS());
    SETVAL(comb.vb_xslvi.aw_user, ALLZEROS());
    SETVAL(comb.vb_xslvi.w_strb, ALLONES());
    SETVAL(comb.vb_xslvi.w_user, ALLZEROS());
    SETVAL(comb.vb_xslvi.b_ready, CONST("1", 1));
    SETVAL(comb.vb_xslvi.ar_id, ALLZEROS());
    SETVAL(comb.vb_xslvi.ar_user, ALLZEROS());
    SETVAL(comb.vb_xslvi.r_ready, CONST("1", 1));

    TEXT();
    SETVAL(wb_xslvi, comb.vb_xslvi);
}


void axi_slv_tb::proc_test_clk() {
    IF (EZ(w_nrst));
        SETZERO(test_cnt);
        SETZERO(rd_valid);
        SETZERO(req_ready);
        SETZERO(rd_addr);
        SETZERO(rd_data);
        SETZERO(v_busy);
    ELSE();
        SETZERO(v_busy);
        IF (AND2(NZ(w_slv_o_req_write), NZ(w_slv_o_req_valid)));
            SETARRITEM(mem, TO_INT(BITS(wb_slv_o_req_addr, 5, 2)), mem, wb_slv_o_req_wdata);
        ENDIF();
        SETVAL(rd_addr, BITS(wb_slv_o_req_addr, 5, 2));
        IF (NZ(AND2_L(w_slv_o_req_valid, INV_L(v_busy))));
            SETVAL(rd_data, wb_slv_o_req_addr);//ARRITEM(mem, TO_INT(BITS(wb_slv_o_req_addr, 5, 2)), mem));
        ENDIF();
        SETVAL(rd_valid, CC2(BITS(rd_valid, 1, 0),  AND2_L(w_slv_o_req_valid, INV_L(v_busy))));
    ENDIF();
    SETVAL(wb_slv_i_resp_rdata, rd_data);
    SETVAL(w_slv_i_resp_valid, BIT(rd_valid, 0));
    SETVAL(w_slv_i_req_ready, CONST("1", 1));
    SETZERO(w_slv_i_resp_err);
}

