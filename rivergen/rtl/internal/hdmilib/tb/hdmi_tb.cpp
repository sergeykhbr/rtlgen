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

#include "hdmi_tb.h"

hdmi_tb::hdmi_tb(GenObject *parent, const char *name) :
    ModuleObject(parent, "hdmi_tb", name, NO_COMMENT),
    // parameters
    // Ports
    i_nrst(this, "i_nrst", "1", "1", "Power-on system reset active LOW"),
    w_clk(this, "w_clk", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_clk_cnt(this, "wb_clk_cnt", "32", "'0", NO_COMMENT),
    w_hdmi_hsync(this, "w_hdmi_hsync", "1", RSTVAL_ZERO, NO_COMMENT),
    w_hdmi_vsync(this, "w_hdmi_vsync", "1", RSTVAL_ZERO, NO_COMMENT),
    w_hdmi_de(this, "w_hdmi_de", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_hdmi_data(this, "wb_hdmi_data", "18", RSTVAL_ZERO, NO_COMMENT),
    w_hdmi_spdif(this, "w_hdmi_spdif", "1", RSTVAL_ZERO, NO_COMMENT),
    w_hdmi_spdif_out(this, "w_hdmi_spdif_out", "1", RSTVAL_ZERO, NO_COMMENT),
    w_hdmi_irq(this, "w_hdmi_irq", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_xmsto(this, "wb_xmsto", "Clock 2 output"),
    wb_xmsti(this, "wb_xmsti", "Clock 2 input"),
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
    tt(this, "tt", NO_COMMENT),
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
    NEW(tt, tt.getName().c_str());
        CONNECT(tt, 0, tt.i_nrst, i_nrst);
        CONNECT(tt, 0, tt.i_clk, w_clk);
        CONNECT(tt, 0, tt.i_hdmi_clk, w_clk);
        CONNECT(tt, 0, tt.o_hsync, w_hdmi_hsync);
        CONNECT(tt, 0, tt.o_vsync, w_hdmi_vsync);
        CONNECT(tt, 0, tt.o_de, w_hdmi_de);
        CONNECT(tt, 0, tt.o_data, wb_hdmi_data);
        CONNECT(tt, 0, tt.o_spdif, w_hdmi_spdif);
        CONNECT(tt, 0, tt.i_spdif_out, w_hdmi_spdif_out);
        CONNECT(tt, 0, tt.i_irq, w_hdmi_irq);
        CONNECT(tt, 0, tt.o_xmst_cfg, wb_mst_o_cfg);
        CONNECT(tt, 0, tt.i_xmsti, wb_xmsti);
        CONNECT(tt, 0, tt.o_xmsto, wb_xmsto);
    ENDNEW();

TEXT();
    NEW(slv0, slv0.getName().c_str());
        CONNECT(slv0, 0, slv0.i_clk, w_clk);
        CONNECT(slv0, 0, slv0.i_nrst, i_nrst);
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

    Operation::start(&comb);
    proc_comb();

    Operation::start(&test_clk);
    proc_test_clk();
}

void hdmi_tb::proc_comb() {
    SETVAL(comb.vb_mapinfo.addr_start, CONST("0x80000000", 48));
    SETVAL(comb.vb_mapinfo.addr_end, CONST("0x80001000", 48));
    SETVAL(wb_slv_mapinfo, comb.vb_mapinfo);

    SETVAL(comb.vb_xslvi.ar_valid, wb_xmsto.ar_valid);
    SETVAL(comb.vb_xslvi.ar_bits.addr, wb_xmsto.ar_bits.addr);
    SETVAL(comb.vb_xslvi.ar_bits.len, wb_xmsto.ar_bits.len);
    SETVAL(comb.vb_xslvi.ar_bits.size, wb_xmsto.ar_bits.size);
    SETVAL(comb.vb_xslvi.ar_bits.burst, wb_xmsto.ar_bits.burst);
    SETVAL(comb.vb_xslvi.ar_bits.lock, wb_xmsto.ar_bits.lock);
    SETVAL(comb.vb_xslvi.ar_bits.cache, wb_xmsto.ar_bits.cache);
    SETVAL(comb.vb_xslvi.ar_bits.prot, wb_xmsto.ar_bits.prot);
    SETVAL(comb.vb_xslvi.ar_bits.qos, wb_xmsto.ar_bits.qos);
    SETVAL(comb.vb_xslvi.ar_bits.region, wb_xmsto.ar_bits.region);
    SETVAL(comb.vb_xslvi.aw_valid, wb_xmsto.aw_valid);
    SETVAL(comb.vb_xslvi.aw_bits.addr, wb_xmsto.aw_bits.addr);
    SETVAL(comb.vb_xslvi.aw_bits.len, wb_xmsto.aw_bits.len);
    SETVAL(comb.vb_xslvi.aw_bits.size, wb_xmsto.aw_bits.size);
    SETVAL(comb.vb_xslvi.aw_bits.burst, wb_xmsto.aw_bits.burst);
    SETVAL(comb.vb_xslvi.aw_bits.lock, wb_xmsto.aw_bits.lock);
    SETVAL(comb.vb_xslvi.aw_bits.cache, wb_xmsto.aw_bits.cache);
    SETVAL(comb.vb_xslvi.aw_bits.prot, wb_xmsto.aw_bits.prot);
    SETVAL(comb.vb_xslvi.aw_bits.qos, wb_xmsto.aw_bits.qos);
    SETVAL(comb.vb_xslvi.aw_bits.region, wb_xmsto.aw_bits.region);
    SETVAL(comb.vb_xslvi.aw_id, wb_xmsto.aw_id);
    SETVAL(comb.vb_xslvi.aw_user, wb_xmsto.aw_user);
    SETVAL(comb.vb_xslvi.w_valid, wb_xmsto.w_valid);
    SETVAL(comb.vb_xslvi.w_data, wb_xmsto.w_data);
    SETVAL(comb.vb_xslvi.w_last, wb_xmsto.w_last);
    SETVAL(comb.vb_xslvi.w_strb, wb_xmsto.w_strb);
    SETVAL(comb.vb_xslvi.w_user, wb_xmsto.w_user);
    SETVAL(comb.vb_xslvi.b_ready, wb_xmsto.b_ready);
    SETVAL(comb.vb_xslvi.ar_id, wb_xmsto.ar_id);
    SETVAL(comb.vb_xslvi.ar_user, wb_xmsto.ar_user);
    SETVAL(comb.vb_xslvi.r_ready, wb_xmsto.r_ready);

    TEXT();
    SETVAL(comb.vb_xmsti.aw_ready, wb_xslvo.aw_ready);
    SETVAL(comb.vb_xmsti.w_ready, wb_xslvo.w_ready);
    SETVAL(comb.vb_xmsti.b_valid, wb_xslvo.b_valid);
    SETVAL(comb.vb_xmsti.b_resp, wb_xslvo.b_resp);
    SETVAL(comb.vb_xmsti.b_id, wb_xslvo.b_id);
    SETVAL(comb.vb_xmsti.b_user, wb_xslvo.b_user);
    SETVAL(comb.vb_xmsti.ar_ready, wb_xslvo.ar_ready);
    SETVAL(comb.vb_xmsti.r_valid, wb_xslvo.r_valid);
    SETVAL(comb.vb_xmsti.r_resp, wb_xslvo.r_resp);
    SETVAL(comb.vb_xmsti.r_data, wb_xslvo.r_data);
    SETVAL(comb.vb_xmsti.r_last, wb_xslvo.r_last);
    SETVAL(comb.vb_xmsti.r_id, wb_xslvo.r_id);
    SETVAL(comb.vb_xmsti.r_user, wb_xslvo.r_user);

    TEXT();
    SETVAL(wb_xslvi, comb.vb_xslvi);
    SETVAL(wb_xmsti, comb.vb_xmsti);
}


void hdmi_tb::proc_test_clk() {
    SETVAL(wb_clk_cnt, INC(wb_clk_cnt));
    IF (LS(wb_clk_cnt, CONST("10")));
        SETZERO(i_nrst);
    ELSE();
        SETONE(i_nrst);
    ENDIF();

    IF (EZ(i_nrst));
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

