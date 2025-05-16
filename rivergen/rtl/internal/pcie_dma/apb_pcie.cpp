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

#include "apb_pcie.h"

apb_pcie::apb_pcie(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "apb_pcie", name, comment),
    i_clk(this, "i_clk", "1", "APB clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_mapinfo(this, "i_mapinfo", "interconnect slot information"),
    o_cfg(this, "o_cfg", "Device descriptor"),
    i_apbi(this, "i_apbi", "APB input interface"),
    o_apbo(this, "o_apbo", "APB output interface"),
    i_pcie_completer_id(this, "i_pcie_completer_id", "16", "Bus, Device, Function"),
    i_dma_state(this, "i_dma_state", "4", "DMA engine brief state"),
    i_dbg_mem_valid(this, "i_dbg_mem_valid", "1"),
    i_dbg_mem_wren(this, "i_dbg_mem_wren", "1"),
    i_dbg_mem_wstrb(this, "i_dbg_mem_wstrb", "8"),
    i_dbg_mem_addr(this, "i_dbg_mem_addr", "13"),
    i_dbg_mem_data(this, "i_dbg_mem_data", "32"),
    // params
    // signals
    w_req_valid(this, "w_req_valid", "1"),
    wb_req_addr(this, "wb_req_addr", "32"),
    w_req_write(this, "w_req_write", "1"),
    wb_req_wdata(this, "wb_req_wdata", "32"),
    // registers
    resp_valid(this, "resp_valid", "1", "0"),
    resp_rdata(this, "resp_rdata", "32", "'0", NO_COMMENT),
    resp_err(this, "resp_err", "1", "0"),
    req_cnt(this, "req_cnt", "31", RSTVAL_ZERO, NO_COMMENT),
    req_data_arr(this, "req_data_arr", "64", "16", NO_COMMENT),
    //
    comb(this),
    reqff(this, "reqff", &i_clk, CLK_POSEDGE, 0, ACTIVE_NONE, NO_COMMENT),
    pslv0(this, "pslv0", NO_COMMENT)
{
    Operation::start(this);

    pslv0.vid.setObjValue(&glob_pnp_cfg_->VENDOR_OPTIMITECH);
    pslv0.did.setObjValue(&glob_pnp_cfg_->OPTIMITECH_PCIE_CTRL);
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

    Operation::start(&reqff);
    proc_reqff();
}

void apb_pcie::proc_comb() {
    SETZERO(resp_err);
    TEXT("Registers access:");
    IF (EQ(BITS(wb_req_addr, 11, 2), CONST("0", 10)));
        TEXT("0x00: link status");
        SETBITS(comb.vb_rdata, 3, 0, i_dma_state);
    ELSIF (EQ(BITS(wb_req_addr, 11, 2), CONST("1", 10)));
        TEXT("0x04: bus, device, function");
        SETBITS(comb.vb_rdata, 15, 0, i_pcie_completer_id);
    ELSIF (EQ(BITS(wb_req_addr, 11, 2), CONST("2", 10)));
        TEXT("0x08: request counter");
        SETVAL(comb.vb_rdata, req_cnt);
    ELSIF (EQ(BITS(wb_req_addr, 11, 7), CONST("1", 5)));
        TEXT("0x040..0x04F: debug buffer");
        IF (EZ(BIT(wb_req_addr, 2)));
            SETVAL(comb.vb_rdata,
                   BITS(ARRITEM(req_data_arr, TO_INT(BITS(wb_req_addr, 6, 3)), req_data_arr), 31, 0));
        ELSE();
            SETVAL(comb.vb_rdata,
                   BITS(ARRITEM(req_data_arr, TO_INT(BITS(wb_req_addr, 6, 3)), req_data_arr), 63, 32));
        ENDIF();
    ENDIF();

TEXT();
    SETVAL(resp_valid, w_req_valid);
    SETVAL(resp_rdata, comb.vb_rdata);

TEXT();
    SYNC_RESET();
}

void apb_pcie::proc_reqff() {
    IF (NZ(i_dbg_mem_valid));
        SETARRITEM_NB(req_data_arr, TO_INT(BITS(req_cnt, 3, 0)), req_data_arr,
                    CC4(i_dbg_mem_wren, CONST("0", 18), i_dbg_mem_addr, i_dbg_mem_data));
        SETVAL_NB(req_cnt, INC(req_cnt));
    ENDIF();
}
