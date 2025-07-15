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

#include "pcie_dma_tb.h"

pcie_dma_tb::pcie_dma_tb(GenObject *parent, const char *name) :
    ModuleObject(parent, "pcie_dma_tb", name, NO_COMMENT),
    // parameters
    // Ports
    i_nrst(this, "i_nrst", "1", "1", "Power-on system reset active LOW"),
    i_clk(this, "i_clk", "1", RSTVAL_ZERO, NO_COMMENT),
    w_pci_clk(this, "w_pci_clk", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_clk_cnt(this, "wb_clk_cnt", "32", "'0", NO_COMMENT),
    wb_i_pcie_completer_id(this, "wb_i_pcie_completer_id", "16", "Bus, Device, Function"),
    wb_o_dma_state(this, "wb_o_dma_state", "4", "State machine debug output, connected to APB controller"),
    wb_i_pcie_dmai(this, "wb_i_pcie_dmai", "PCIE-EP to system DMA requests"),
    wb_o_pcie_dmao(this, "wb_o_pcie_dmao", "System DMA responds to PCIE EP"),
    wb_o_xmst_cfg(this, "wb_o_xmst_cfg", "PCIE DMA master interface descriptor"),
    wb_i_xmsti(this, "wb_i_xmsti", NO_COMMENT),
    wb_o_xmsto(this, "wb_o_xmsto", NO_COMMENT),
    w_o_dbg_valid(this, "w_o_dbg_valid", "1"),
    wb_o_dbg_payload(this, "wb_o_dbg_payload", "64"),
    wb_slv_i_mapinfo(this, "wb_slv_i_mapinfo", "Base address information from the interconnect port"),
    wb_slv_o_cfg(this, "wb_slv_o_cfg", "Slave config descriptor"),
    wb_slv_i_xslvi(this, "wb_slv_i_xslvi", "AXI Slave input interface"),
    wb_slv_o_xslvo(this, "wb_slv_o_xslvo", "AXI Slave output interface"),
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
    rd_valid(this, "rd_valid", "1", RSTVAL_ZERO, NO_COMMENT),
    rd_addr(this, "rd_addr", "4", "'0", NO_COMMENT),
    mem(this, "mem", "64", "16", NO_COMMENT),
    // submodules:
    clk0(this, "clk0", NO_COMMENT),
    clk1(this, "clk1", NO_COMMENT),
    slv0(this, "slv0", NO_COMMENT),
    tt(this, "tt", NO_COMMENT),
    // processes:
    comb(this),
    test(this, &w_pci_clk),
    bus(this, &i_clk)
{
    Operation::start(this);

    // Create and connet Sub-modules:
    clk0.period.setObjValue(new FloatConst(25.0));  // 40 MHz
    NEW(clk0, clk0.getName().c_str());
        CONNECT(clk0, 0, clk0.o_clk, i_clk);
    ENDNEW();

TEXT();
    clk1.period.setObjValue(new FloatConst(16.0));  // 62.5 MHz
    NEW(clk1, clk1.getName().c_str());
        CONNECT(clk1, 0, clk1.o_clk, w_pci_clk);
    ENDNEW();

TEXT();
    NEW(tt, tt.getName().c_str());
        CONNECT(tt, 0, tt.i_nrst, i_nrst);
        CONNECT(tt, 0, tt.i_clk, i_clk);
        CONNECT(tt, 0, tt.i_pcie_phy_clk, w_pci_clk);
        CONNECT(tt, 0, tt.i_pcie_completer_id, wb_i_pcie_completer_id);
        CONNECT(tt, 0, tt.o_dma_state, wb_o_dma_state);
        CONNECT(tt, 0, tt.i_pcie_dmai, wb_i_pcie_dmai);
        CONNECT(tt, 0, tt.o_pcie_dmao, wb_o_pcie_dmao);
        CONNECT(tt, 0, tt.o_xmst_cfg, wb_o_xmst_cfg);
        CONNECT(tt, 0, tt.i_xmsti, wb_i_xmsti);
        CONNECT(tt, 0, tt.o_xmsto, wb_o_xmsto);
        CONNECT(tt, 0, tt.o_dbg_valid, w_o_dbg_valid);
        CONNECT(tt, 0, tt.o_dbg_payload, wb_o_dbg_payload);
    ENDNEW();

TEXT();
    NEW(slv0, slv0.getName().c_str());
        CONNECT(slv0, 0, slv0.i_clk, i_clk);
        CONNECT(slv0, 0, slv0.i_nrst, i_nrst);
        CONNECT(slv0, 0, slv0.i_mapinfo, wb_slv_i_mapinfo);
        CONNECT(slv0, 0, slv0.o_cfg, wb_slv_o_cfg);
        CONNECT(slv0, 0, slv0.i_xslvi, wb_slv_i_xslvi);
        CONNECT(slv0, 0, slv0.o_xslvo, wb_slv_o_xslvo);
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
    comb_proc();

    Operation::start(&test);
    test_proc();

    Operation::start(&bus);
    bus_proc();
}

void pcie_dma_tb::comb_proc() {
    SETVAL(comb.vb_mapinfo.addr_start, CONST("0x08000000", 48));
    SETVAL(comb.vb_mapinfo.addr_end, CONST("0x08001000", 48));
    SETVAL(wb_slv_i_mapinfo, comb.vb_mapinfo);

    TEXT();
    SETVAL(comb.vb_xslvi.aw_valid, wb_o_xmsto.aw_valid);
    SETVAL(comb.vb_xslvi.aw_bits, wb_o_xmsto.aw_bits);
    SETVAL(comb.vb_xslvi.aw_id, wb_o_xmsto.aw_id);
    SETVAL(comb.vb_xslvi.aw_user, wb_o_xmsto.aw_user);
    SETVAL(comb.vb_xslvi.w_valid, wb_o_xmsto.w_valid);
    SETVAL(comb.vb_xslvi.w_data, wb_o_xmsto.w_data);
    SETVAL(comb.vb_xslvi.w_last, wb_o_xmsto.w_last);
    SETVAL(comb.vb_xslvi.w_strb, wb_o_xmsto.w_strb);
    SETVAL(comb.vb_xslvi.w_user, wb_o_xmsto.w_user);
    SETVAL(comb.vb_xslvi.b_ready, wb_o_xmsto.b_ready);
    SETVAL(comb.vb_xslvi.ar_valid, wb_o_xmsto.ar_valid);
    SETVAL(comb.vb_xslvi.ar_bits, wb_o_xmsto.ar_bits);
    SETVAL(comb.vb_xslvi.ar_id, wb_o_xmsto.ar_id);
    SETVAL(comb.vb_xslvi.ar_user, wb_o_xmsto.ar_user);
    SETVAL(comb.vb_xslvi.r_ready, wb_o_xmsto.r_ready);
    SETVAL(wb_slv_i_xslvi, comb.vb_xslvi);

    TEXT();
    SETVAL(comb.vb_xmsti.aw_ready, wb_slv_o_xslvo.aw_ready);
    SETVAL(comb.vb_xmsti.w_ready, wb_slv_o_xslvo.w_ready);
    SETVAL(comb.vb_xmsti.b_valid, wb_slv_o_xslvo.b_valid);
    SETVAL(comb.vb_xmsti.b_resp, wb_slv_o_xslvo.b_resp);
    SETVAL(comb.vb_xmsti.b_id, wb_slv_o_xslvo.b_id);
    SETVAL(comb.vb_xmsti.b_user, wb_slv_o_xslvo.b_user);
    SETVAL(comb.vb_xmsti.ar_ready, wb_slv_o_xslvo.ar_ready);
    SETVAL(comb.vb_xmsti.r_valid, wb_slv_o_xslvo.r_valid);
    SETVAL(comb.vb_xmsti.r_resp, wb_slv_o_xslvo.r_resp);
    SETVAL(comb.vb_xmsti.r_data, wb_slv_o_xslvo.r_data);
    SETVAL(comb.vb_xmsti.r_last, wb_slv_o_xslvo.r_last);
    SETVAL(comb.vb_xmsti.r_id, wb_slv_o_xslvo.r_id);
    SETVAL(comb.vb_xmsti.r_user, wb_slv_o_xslvo.r_user);
    SETVAL(wb_i_xmsti, comb.vb_xmsti);
}


void pcie_dma_tb::test_proc() {
    SETVAL(wb_clk_cnt, INC(wb_clk_cnt));
    IF (LS(wb_clk_cnt, CONST("10")));
        SETZERO(i_nrst);
    ELSE();
        SETONE(i_nrst);
    ENDIF();

    TEXT();
    SETVAL(test.vb_dmai, *SCV_get_cfg_type(this, "pcie_dma64_in_none"));
    SETVAL(test.vb_dmai.bar_hit, CONST("0x01", 7), "BAR0");
    SWITCH(wb_clk_cnt);
    TEXT("Wr32 to address [0x0104] <= 0x44332211");
    CASE(CONST("20"));
        SETONE(test.vb_dmai.valid);
        SETVAL(test.vb_dmai.strob, CONST("0xff", 8));
        SETBITS(test.vb_dmai.data, 31, 24, CONST("0x40", 7), "Wr32");
        SETBITS(test.vb_dmai.data, 9, 0, CONST("0x01", 10), "Length");
        SETBITS(test.vb_dmai.data, 39, 32, CONST("0x0f", 8), "be");
    ENDCASE();
    CASE(CONST("21"));
        SETONE(test.vb_dmai.valid);
        SETONE(test.vb_dmai.last);
        SETVAL(test.vb_dmai.strob, CONST("0xff", 8));
        SETBITS(test.vb_dmai.data, 31, 0, CONST("0x0104", 32), "Addr");
        SETBITS(test.vb_dmai.data, 63, 32, CONST("0x11223344", 32), "Data[31:0]");
    ENDCASE();

    TEXT("Rd32 from address [0x0104] => ");
    CASE(CONST("40"));
        SETONE(test.vb_dmai.valid);
        SETVAL(test.vb_dmai.strob, CONST("0xff", 8));
        SETBITS(test.vb_dmai.data, 31, 24, CONST("0x00", 7), "Rd32");
        SETBITS(test.vb_dmai.data, 9, 0, CONST("0x01", 10), "Length");
        SETBITS(test.vb_dmai.data, 39, 32, CONST("0x01", 8), "be");
    ENDCASE();
    CASE(CONST("41"));
        SETONE(test.vb_dmai.valid);
        SETONE(test.vb_dmai.last);
        SETVAL(test.vb_dmai.strob, CONST("0x0f", 8));
        SETBITS(test.vb_dmai.data, 31, 0, CONST("0x0104", 32), "Addr");
    ENDCASE();
    TEXT("Rd32 from address [0x0105] => ");
    CASE(CONST("42"));
        SETONE(test.vb_dmai.valid);
        SETVAL(test.vb_dmai.strob, CONST("0xff", 8));
        SETBITS(test.vb_dmai.data, 31, 24, CONST("0x00", 7), "Rd32");
        SETBITS(test.vb_dmai.data, 9, 0, CONST("0x01", 10), "Length");
        SETBITS(test.vb_dmai.data, 39, 32, CONST("0x02", 8), "be");
    ENDCASE();
    CASE(CONST("43"));
        SETONE(test.vb_dmai.valid);
        SETONE(test.vb_dmai.last);
        SETVAL(test.vb_dmai.strob, CONST("0x0f", 8));
        SETBITS(test.vb_dmai.data, 31, 0, CONST("0x0104", 32), "Addr");
    ENDCASE();
    TEXT("Rd32 from address [0x0106] => ");
    CASE(CONST("44"));
        SETONE(test.vb_dmai.valid);
        SETVAL(test.vb_dmai.strob, CONST("0xff", 8));
        SETBITS(test.vb_dmai.data, 31, 24, CONST("0x00", 7), "Rd32");
        SETBITS(test.vb_dmai.data, 9, 0, CONST("0x01", 10), "Length");
        SETBITS(test.vb_dmai.data, 39, 32, CONST("0x04", 8), "be");
    ENDCASE();
    CASE(CONST("45"));
        SETONE(test.vb_dmai.valid);
        SETONE(test.vb_dmai.last);
        SETVAL(test.vb_dmai.strob, CONST("0x0f", 8));
        SETBITS(test.vb_dmai.data, 31, 0, CONST("0x0104", 32), "Addr");
    ENDCASE();
    TEXT("Rd32 from address [0x0107] => ");
    CASE(CONST("46"));
        SETONE(test.vb_dmai.valid);
        SETVAL(test.vb_dmai.strob, CONST("0xff", 8));
        SETBITS(test.vb_dmai.data, 31, 24, CONST("0x00", 7), "Rd32");
        SETBITS(test.vb_dmai.data, 9, 0, CONST("0x01", 10), "Length");
        SETBITS(test.vb_dmai.data, 39, 32, CONST("0x08", 8), "be");
    ENDCASE();
    CASE(CONST("47"));
        SETONE(test.vb_dmai.valid);
        SETONE(test.vb_dmai.last);
        SETVAL(test.vb_dmai.strob, CONST("0x0f", 8));
        SETBITS(test.vb_dmai.data, 31, 0, CONST("0x0104", 32), "Addr");
    ENDCASE();

    TEXT();
    TEXT("Write 1x32-bits into 32-bits BAR");
    CASE(CONST("100"));
        SETONE(test.vb_dmai.valid);
        SETVAL(test.vb_dmai.strob, CONST("0xff", 8));
        SETBITS(test.vb_dmai.data, 31, 24, CONST("0x40", 7), "Wr32");
        SETBITS(test.vb_dmai.data, 9, 0, CONST("0x02", 10), "Length");
        SETBITS(test.vb_dmai.data, 39, 32, CONST("0xff", 8), "be");
    ENDCASE();
    CASE(CONST("101"));
        SETONE(test.vb_dmai.valid);
        SETVAL(test.vb_dmai.strob, CONST("0xff", 8));
        SETBITS(test.vb_dmai.data, 31, 0, CONST("0x0108", 32), "Addr");
        SETBITS(test.vb_dmai.data, 63, 32, CONST("0x11223344", 32), "Data[31:0]");
    ENDCASE();
    CASE(CONST("102"));
        SETONE(test.vb_dmai.valid);
        SETONE(test.vb_dmai.last);
        SETVAL(test.vb_dmai.strob, CONST("0x0f", 8));
        SETBITS(test.vb_dmai.data, 31, 0, CONST("0x55667788", 32), "Data[63:32]");
    ENDCASE();

    TEXT();
    TEXT("Write 2x32-bits into 32-bits BAR");
    CASE(CONST("103"));
        SETONE(test.vb_dmai.valid);
        SETVAL(test.vb_dmai.strob, CONST("0xff", 8));
        SETBITS(test.vb_dmai.data, 31, 24, CONST("0x40", 7), "Wr32");
        SETBITS(test.vb_dmai.data, 9, 0, CONST("0x02", 10), "Length");
        SETBITS(test.vb_dmai.data, 39, 32, CONST("0xff", 8), "be");
    ENDCASE();
    CASE(CONST("104"));
        SETONE(test.vb_dmai.valid);
        SETVAL(test.vb_dmai.strob, CONST("0xff", 8));
        SETBITS(test.vb_dmai.data, 31, 0, CONST("0x0108", 32), "Addr");
        SETBITS(test.vb_dmai.data, 63, 32, CONST("0x44332211", 32), "Data[31:0]");
    ENDCASE();
    CASE(CONST("105"));
        SETONE(test.vb_dmai.valid);
        SETVAL(test.vb_dmai.strob, CONST("0xff", 8));
        SETVAL(test.vb_dmai.data, CONST("0xaabbccdd88776655", 64));
    ENDCASE();
    CASE(CONST("106"));
        SETONE(test.vb_dmai.valid);
        SETONE(test.vb_dmai.last);
        SETVAL(test.vb_dmai.strob, CONST("0x0f", 8));
        SETBITS(test.vb_dmai.data, 31, 0, CONST("0xeeeeffff", 32), "Data");
    ENDCASE();

    TEXT();
    TEXT("Read 1x32-bits from 32-bits BAR");
    CASE(CONST("130"));
        SETONE(test.vb_dmai.valid);
        SETVAL(test.vb_dmai.strob, CONST("0xff", 8));
        SETBITS(test.vb_dmai.data, 31, 24, CONST("0x00", 7), "Rd32");
        SETBITS(test.vb_dmai.data, 9, 0, CONST("0x01", 10), "Length");
        SETBITS(test.vb_dmai.data, 39, 32, CONST("0x0f", 8), "be");
    ENDCASE();
    CASE(CONST("131"));
        SETONE(test.vb_dmai.valid);
        SETONE(test.vb_dmai.last);
        SETVAL(test.vb_dmai.strob, CONST("0x0f", 8));
        SETBITS(test.vb_dmai.data, 31, 0, CONST("0x0104", 32), "Addr");
    ENDCASE();
    TEXT("Read 1x64-bits from 32-bits BAR");
    CASE(CONST("132"));
        SETONE(test.vb_dmai.valid);
        SETVAL(test.vb_dmai.strob, CONST("0xff", 8));
        SETBITS(test.vb_dmai.data, 31, 24, CONST("0x00", 7), "Rd32");
        SETBITS(test.vb_dmai.data, 9, 0, CONST("0x02", 10), "Length");
        SETBITS(test.vb_dmai.data, 39, 32, CONST("0xff", 8), "be");
    ENDCASE();
    CASE(CONST("133"));
        SETONE(test.vb_dmai.valid);
        SETONE(test.vb_dmai.last);
        SETVAL(test.vb_dmai.strob, CONST("0x0f", 8));
        SETBITS(test.vb_dmai.data, 31, 0, CONST("0x0108", 32), "Addr");
    ENDCASE();
    TEXT("Read 2x64-bites from 32-bits BAR");
    CASE(CONST("134"));
        SETONE(test.vb_dmai.valid);
        SETVAL(test.vb_dmai.strob, CONST("0xff", 8));
        SETBITS(test.vb_dmai.data, 31, 24, CONST("0x00", 7), "Rd32");
        SETBITS(test.vb_dmai.data, 9, 0, CONST("0x04", 10), "Length");
        SETBITS(test.vb_dmai.data, 39, 32, CONST("0xff", 8), "be");
    ENDCASE();
    CASE(CONST("135"));
        SETONE(test.vb_dmai.valid);
        SETONE(test.vb_dmai.last);
        SETVAL(test.vb_dmai.strob, CONST("0x0f", 8));
        SETBITS(test.vb_dmai.data, 31, 0, CONST("0x0108", 32), "Addr");
    ENDCASE();


    TEXT();
    TEXT("Write 1x32-bits into 64-bits BAR");
    CASE(CONST("303"));
        SETONE(test.vb_dmai.valid);
        SETVAL(test.vb_dmai.strob, CONST("0xff", 8));
        SETBITS(test.vb_dmai.data, 31, 24, CONST("0x60", 7), "Wr64");
        SETBITS(test.vb_dmai.data, 9, 0, CONST("0x01", 10), "Length");
        SETBITS(test.vb_dmai.data, 39, 32, CONST("0x0f", 8), "be");
    ENDCASE();
    CASE(CONST("304"));
        SETONE(test.vb_dmai.valid);
        SETVAL(test.vb_dmai.strob, CONST("0xff", 8));
        SETBITS(test.vb_dmai.data, 63, 32, CONST("0x0124", 32), "Addr[31:0]");
        SETBITS(test.vb_dmai.data, 31, 0, CONST("0x0", 32), "Addr[63:32]");
    ENDCASE();
    CASE(CONST("305"));
        SETONE(test.vb_dmai.valid);
        SETONE(test.vb_dmai.last);
        SETVAL(test.vb_dmai.strob, CONST("0x0f", 8));
        SETBITS(test.vb_dmai.data, 31, 0, CONST("0x12345678", 32), "Data[31:0]");
    ENDCASE();
    TEXT();
    TEXT("Read 1x32-bits from 64-bits BAR");
    CASE(CONST("306"));
        SETONE(test.vb_dmai.valid);
        SETVAL(test.vb_dmai.strob, CONST("0xff", 8));
        SETBITS(test.vb_dmai.data, 31, 24, CONST("0x20", 7), "Rd64");
        SETBITS(test.vb_dmai.data, 9, 0, CONST("0x01", 10), "Length");
        SETBITS(test.vb_dmai.data, 39, 32, CONST("0x0f", 8), "be");
    ENDCASE();
    CASE(CONST("307"));
        SETONE(test.vb_dmai.valid);
        SETONE(test.vb_dmai.last);
        SETVAL(test.vb_dmai.strob, CONST("0xff", 8));
        SETBITS(test.vb_dmai.data, 63, 32, CONST("0x0124", 32), "Addr[31:0]");
        SETBITS(test.vb_dmai.data, 31, 0, CONST("0x0", 32), "Addr[63:32]");

    TEXT("Read 1x64-bits from 64-bits BAR");
    CASE(CONST("360"));
        SETONE(test.vb_dmai.valid);
        SETVAL(test.vb_dmai.strob, CONST("0xff", 8));
        SETBITS(test.vb_dmai.data, 31, 24, CONST("0x20", 7), "Rd64");
        SETBITS(test.vb_dmai.data, 9, 0, CONST("0x01", 10), "Length");
        SETBITS(test.vb_dmai.data, 39, 32, CONST("0x0f", 8), "be");
    ENDCASE();
    CASE(CONST("361"));
        SETONE(test.vb_dmai.valid);
        SETONE(test.vb_dmai.last);
        SETVAL(test.vb_dmai.strob, CONST("0xff", 8));
        SETBITS(test.vb_dmai.data, 63, 32, CONST("0x0120", 32), "Addr[31:0]");
        SETBITS(test.vb_dmai.data, 31, 0, CONST("0x0", 32), "Addr[63:32]");
    ENDCASE();


    TEXT("Test FIFO full flag");
    CASE(CONST("500"));
        SETONE(test.vb_dmai.valid);
        SETVAL(test.vb_dmai.strob, CONST("0xff", 8));
        SETVAL(test.vb_dmai.data, CONST("0xCCCCCCCCDDDDDDDD", 64));
    ENDCASE();

    TEXT();
    CASEDEF();
    ENDCASE();
    ENDSWITCH();

    TEXT();
    SETONE(test.vb_dmai.ready);
    SETVAL(wb_i_pcie_dmai, test.vb_dmai);
}

void pcie_dma_tb::bus_proc() {
    IF (EZ(i_nrst));
        SETZERO(rd_valid);
        SETZERO(rd_addr);
    ELSE();
        IF (AND2(NZ(w_slv_o_req_write), NZ(w_slv_o_req_valid)));
            SETARRITEM(mem, TO_INT(BITS(wb_slv_o_req_addr, 5, 2)), mem, wb_slv_o_req_wdata);
        ENDIF();
        SETVAL(rd_addr, BITS(wb_slv_o_req_addr, 5, 2));
        SETVAL(rd_valid, w_slv_o_req_valid);
    ENDIF();
    SETVAL(wb_slv_i_resp_rdata, ARRITEM(mem, TO_INT(BITS(rd_addr, 3, 0)), mem));
    SETVAL(w_slv_i_resp_valid, rd_valid);
    SETONE(w_slv_i_req_ready);
    SETZERO(w_slv_i_resp_err);
}

