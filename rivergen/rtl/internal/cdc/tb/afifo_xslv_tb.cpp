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

#include "afifo_xslv_tb.h"

afifo_xslv_tb::afifo_xslv_tb(GenObject *parent, const char *name) :
    ModuleObject(parent, "afifo_xslv_tb", name, NO_COMMENT),
    // parameters
    // Ports
    i_nrst(this, "i_nrst", "1", "1", "Power-on system reset active LOW"),
    w_clk1(this, "w_clk1", "1", RSTVAL_ZERO, NO_COMMENT),
    w_clk2(this, "w_clk2", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_clk1_cnt(this, "wb_clk1_cnt", "32", "'0", NO_COMMENT),
    wb_clk2_cnt(this, "wb_clk2_cnt", "32", "'0", NO_COMMENT),
    wb_clk1_xslvi(this, "wb_clk1_xslvi", "Clock 1 input"),
    wb_clk1_xslvo(this, "wb_clk1_xslvo", "Clock 1 output"),
    wb_clk2_xmsto(this, "wb_clk2_xmsto", "Clock 2 output"),
    wb_clk2_xmsti(this, "wb_clk2_xmsti", "Clock 2 input"),
    wb_slv_i_mapinfo(this, "wb_slv_i_mapinfo", "Base address information from the interconnect port"),
    wb_slv_o_cfg(this, "wb_slv_o_cfg", "Slave config descriptor"),
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
    rd_valid(this, "rd_valid", "3", RSTVAL_ZERO, NO_COMMENT),
    req_ready(this, "req_ready", "1", RSTVAL_ZERO, NO_COMMENT),
    rd_addr(this, "rd_addr", "4", "'0", NO_COMMENT),
    mem(this, "mem", "64", "16", NO_COMMENT),
    // submodules:
    clk1(this, "clk1", NO_COMMENT),
    clk2(this, "clk2", NO_COMMENT),
    slv0(this, "slv0", NO_COMMENT),
    tt(this, "tt", NO_COMMENT),
    // processes:
    comb(this),
    test_clk1(this, &w_clk1),
    test_clk2(this, &w_clk2)
{
    Operation::start(this);

    // Create and connet Sub-modules:
    clk1.period.setObjValue(new FloatConst(5.0));  // 200 MHz
    NEW(clk1, clk1.getName().c_str());
        CONNECT(clk1, 0, clk1.o_clk, w_clk1);
    ENDNEW();

TEXT();
    clk2.period.setObjValue(new FloatConst(25.0));  // 40 MHz
    NEW(clk2, clk2.getName().c_str());
        CONNECT(clk2, 0, clk2.o_clk, w_clk2);
    ENDNEW();

TEXT();
    NEW(tt, tt.getName().c_str());
        CONNECT(tt, 0, tt.i_xslv_nrst, i_nrst);
        CONNECT(tt, 0, tt.i_xslv_clk, w_clk1);
        CONNECT(tt, 0, tt.i_xslvi, wb_clk1_xslvi);
        CONNECT(tt, 0, tt.o_xslvo, wb_clk1_xslvo);
        CONNECT(tt, 0, tt.i_xmst_nrst, i_nrst);
        CONNECT(tt, 0, tt.i_xmst_clk, w_clk2);
        CONNECT(tt, 0, tt.o_xmsto, wb_clk2_xmsto);
        CONNECT(tt, 0, tt.i_xmsti, wb_clk2_xmsti);
    ENDNEW();

TEXT();
    NEW(slv0, slv0.getName().c_str());
        CONNECT(slv0, 0, slv0.i_clk, w_clk2);
        CONNECT(slv0, 0, slv0.i_nrst, i_nrst);
        CONNECT(slv0, 0, slv0.i_mapinfo, wb_slv_i_mapinfo);
        CONNECT(slv0, 0, slv0.o_cfg, wb_slv_o_cfg);
        CONNECT(slv0, 0, slv0.i_xslvi, wb_clk2_xmsto);
        CONNECT(slv0, 0, slv0.o_xslvo, wb_clk2_xmsti);
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

    Operation::start(&test_clk1);
    proc_test_clk1();

    Operation::start(&test_clk2);
    proc_test_clk2();
}

void afifo_xslv_tb::proc_comb() {
    SETVAL(comb.vb_mapinfo.addr_start, CONST("0x08000000", 48));
    SETVAL(comb.vb_mapinfo.addr_end, CONST("0x08001000", 48));
    SETVAL(wb_slv_i_mapinfo, comb.vb_mapinfo);
}


void afifo_xslv_tb::proc_test_clk1() {
    SETVAL(wb_clk1_cnt, INC(wb_clk1_cnt));
    IF (LS(wb_clk1_cnt, CONST("10")));
        SETZERO(i_nrst);
    ELSE();
        SETONE(i_nrst);
    ENDIF();

    TEXT();
    SETVAL(test_clk1.vb_xslvi, *SCV_get_cfg_type(this, "axi4_slave_in_none"));
    SETONE(test_clk1.vb_xslvi.r_ready);
    SETONE(test_clk1.vb_xslvi.b_ready);
    IF(EQ(wb_clk1_cnt, CONST("20")));
        SETONE(test_clk1.vb_xslvi.aw_valid);
        SETVAL(test_clk1.vb_xslvi.aw_bits.addr, CONST("0x08000008", 48));
        SETVAL(test_clk1.vb_xslvi.aw_bits.size, CONST("0x3", 3));
        SETVAL(test_clk1.vb_xslvi.aw_bits.len, CONST("0x3", 3));
    ELSIF(EQ(wb_clk1_cnt, CONST("21")));
        SETONE(test_clk1.vb_xslvi.w_valid);
        SETVAL(test_clk1.vb_xslvi.w_data, CONST("0x1122334455667788", 64));
        SETVAL(test_clk1.vb_xslvi.w_strb, CONST("0xFF", 8));
    ELSIF(EQ(wb_clk1_cnt, CONST("22")));
        SETONE(test_clk1.vb_xslvi.w_valid);
        SETVAL(test_clk1.vb_xslvi.w_data, CONST("0x2222334455667788", 64));
        SETVAL(test_clk1.vb_xslvi.w_strb, CONST("0xFF", 8));
    ELSIF(EQ(wb_clk1_cnt, CONST("23")));
        SETONE(test_clk1.vb_xslvi.w_valid);
        SETVAL(test_clk1.vb_xslvi.w_data, CONST("0x3322334455667788", 64));
        SETVAL(test_clk1.vb_xslvi.w_strb, CONST("0xFF", 8));
    ELSIF(EQ(wb_clk1_cnt, CONST("24")));
        SETONE(test_clk1.vb_xslvi.w_valid);
        SETVAL(test_clk1.vb_xslvi.w_data, CONST("0x4422334455667788", 64));
        SETVAL(test_clk1.vb_xslvi.w_strb, CONST("0xFF", 8));
        SETONE(test_clk1.vb_xslvi.w_last);
    ELSIF(EQ(wb_clk1_cnt, CONST("1000")));
        SETONE(test_clk1.vb_xslvi.ar_valid);
        SETVAL(test_clk1.vb_xslvi.ar_bits.addr, CONST("0x08000008", 48));
        SETVAL(test_clk1.vb_xslvi.ar_bits.size, CONST("0x3", 3));
        SETVAL(test_clk1.vb_xslvi.ar_bits.len, CONST("0x3", 3));
    ELSIF(EQ(wb_clk1_cnt, CONST("2000")));
        SETONE(test_clk1.vb_xslvi.aw_valid);
        SETVAL(test_clk1.vb_xslvi.aw_bits.addr, CONST("0x08000028", 48));
        SETVAL(test_clk1.vb_xslvi.aw_bits.size, CONST("0x3", 3));
        SETVAL(test_clk1.vb_xslvi.aw_bits.len, CONST("0x3", 3));

        SETONE(test_clk1.vb_xslvi.ar_valid);
        SETVAL(test_clk1.vb_xslvi.ar_bits.addr, CONST("0x08000028", 48));
        SETVAL(test_clk1.vb_xslvi.ar_bits.size, CONST("0x3", 3));
        SETVAL(test_clk1.vb_xslvi.ar_bits.len, CONST("0x3", 3));
    ELSIF(EQ(wb_clk1_cnt, CONST("2001")));
        SETONE(test_clk1.vb_xslvi.w_valid);
        SETVAL(test_clk1.vb_xslvi.w_data, CONST("0xFF22334455667788", 64));
        SETVAL(test_clk1.vb_xslvi.w_strb, CONST("0xFF", 8));

        SETONE(test_clk1.vb_xslvi.ar_valid);
        SETVAL(test_clk1.vb_xslvi.ar_bits.addr, CONST("0x08000038", 48));
        SETVAL(test_clk1.vb_xslvi.ar_bits.size, CONST("0x3", 3));
        SETVAL(test_clk1.vb_xslvi.ar_bits.len, CONST("0x0", 8));
    ELSIF(EQ(wb_clk1_cnt, CONST("2002")));
        SETONE(test_clk1.vb_xslvi.w_valid);
        SETVAL(test_clk1.vb_xslvi.w_data, CONST("0xFA22334455667788", 64));
        SETVAL(test_clk1.vb_xslvi.w_strb, CONST("0xFF", 8));
    ELSIF(EQ(wb_clk1_cnt, CONST("2003")));
        SETONE(test_clk1.vb_xslvi.w_valid);
        SETVAL(test_clk1.vb_xslvi.w_data, CONST("0xFB22334455667788", 64));
        SETVAL(test_clk1.vb_xslvi.w_strb, CONST("0xFF", 8));
    ELSIF(EQ(wb_clk1_cnt, CONST("2004")));
        SETONE(test_clk1.vb_xslvi.w_valid);
        SETVAL(test_clk1.vb_xslvi.w_data, CONST("0xFC22334455667788", 64));
        SETVAL(test_clk1.vb_xslvi.w_strb, CONST("0xFF", 8));
        SETONE(test_clk1.vb_xslvi.w_last);
    ELSIF(EQ(wb_clk1_cnt, CONST("2005")));
        SETONE(test_clk1.vb_xslvi.aw_valid);
        SETVAL(test_clk1.vb_xslvi.aw_bits.addr, CONST("0x08000040", 48));
        SETVAL(test_clk1.vb_xslvi.aw_bits.size, CONST("0x3", 3));
        SETVAL(test_clk1.vb_xslvi.aw_bits.len, CONST("0x3", 3));
    ELSIF(EQ(wb_clk1_cnt, CONST("2156")));
        SETONE(test_clk1.vb_xslvi.w_valid);
        SETVAL(test_clk1.vb_xslvi.w_data, CONST("0x1111", 64));
        SETVAL(test_clk1.vb_xslvi.w_strb, CONST("0xFF", 8));
    ELSIF(EQ(wb_clk1_cnt, CONST("2267")));
        SETONE(test_clk1.vb_xslvi.w_valid);
        SETVAL(test_clk1.vb_xslvi.w_data, CONST("0x2222", 64));
        SETVAL(test_clk1.vb_xslvi.w_strb, CONST("0xFF", 8));
    ELSIF(EQ(wb_clk1_cnt, CONST("2378")));
        SETONE(test_clk1.vb_xslvi.w_valid);
        SETVAL(test_clk1.vb_xslvi.w_data, CONST("0x3333", 64));
        SETVAL(test_clk1.vb_xslvi.w_strb, CONST("0xFF", 8));
    ELSIF(EQ(wb_clk1_cnt, CONST("2489")));
        SETONE(test_clk1.vb_xslvi.w_valid);
        SETVAL(test_clk1.vb_xslvi.w_data, CONST("0x4444", 64));
        SETVAL(test_clk1.vb_xslvi.w_strb, CONST("0xFF", 8));
        SETONE(test_clk1.vb_xslvi.w_last);
    ENDIF();

    TEXT();
    SETVAL(wb_clk1_xslvi, test_clk1.vb_xslvi);
}

void afifo_xslv_tb::proc_test_clk2() {
    IF (EZ(i_nrst));
        SETZERO(rd_valid);
        SETZERO(req_ready);
        SETZERO(rd_addr);
    ELSE();
        SETVAL(wb_clk2_cnt, INC(wb_clk2_cnt));
        IF (AND2(NZ(w_slv_o_req_write), NZ(w_slv_o_req_valid)));
            SETARRITEM(mem, TO_INT(BITS(wb_slv_o_req_addr, 5, 2)), mem, wb_slv_o_req_wdata);
        ENDIF();
        SETVAL(rd_addr, BITS(wb_slv_o_req_addr, 5, 2));
        SETVAL(req_ready, AND_REDUCE(BITS(wb_clk2_cnt, 2, 0)));
        //SETVAL(rd_valid, CC2(BITS(rd_valid, 1, 0), AND2_L(w_slv_o_req_valid, req_ready)));
        SETVAL(rd_valid, CC2(BITS(rd_valid, 1, 0),  w_slv_o_req_valid));
    ENDIF();
    SETVAL(wb_slv_i_resp_rdata, ARRITEM(mem, TO_INT(BITS(rd_addr, 3, 0)), mem));
    SETVAL(w_slv_i_resp_valid, BIT(rd_valid, 0));
    SETVAL(w_slv_i_req_ready, req_ready);
    SETZERO(w_slv_i_resp_err);
}

