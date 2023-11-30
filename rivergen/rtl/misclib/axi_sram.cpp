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

#include "axi_sram.h"

axi_sram::axi_sram(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "axi_sram", name, comment),
    abits(this, "abits", "17"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_mapinfo(this, "i_mapinfo", "interconnect slot information"),
    o_cfg(this, "o_cfg", "Device descriptor"),
    i_xslvi(this, "i_xslvi", "AXI Slave to Bridge interface"),
    o_xslvo(this, "o_xslvo", "AXI Bridge to Slave interface"),
    // params
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
    wb_req_addr_abits(this, "wb_req_addr_abits", "abits"),
    //
    comb(this),
    xslv0(this, "xslv0"),
    tech0(this, "tech0")
{
    Operation::start(this);

    xslv0.vid.setObjValue(&glob_pnp_cfg_->VENDOR_OPTIMITECH);
    xslv0.did.setObjValue(&glob_pnp_cfg_->OPTIMITECH_SRAM);
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

TEXT();
    tech0.abits.setObjValue(&abits);
    tech0.log2_dbytes.setObjValue(&glob_types_amba_->CFG_LOG2_SYSBUS_DATA_BYTES);
    NEW(tech0, tech0.getName().c_str());
        CONNECT(tech0, 0, tech0.i_clk, i_clk);
        CONNECT(tech0, 0, tech0.i_addr, wb_req_addr_abits);
        CONNECT(tech0, 0, tech0.i_wena, w_req_write);
        CONNECT(tech0, 0, tech0.i_wstrb, wb_req_wstrb);
        CONNECT(tech0, 0, tech0.i_wdata, wb_req_wdata);
        CONNECT(tech0, 0, tech0.o_rdata, wb_resp_rdata);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void axi_sram::proc_comb() {
TEXT();
    SETVAL(wb_req_addr_abits, BITS(wb_req_addr, DEC(abits), CONST("0")));
    SETONE(w_req_ready);
    SETONE(w_resp_valid);
    SETZERO(wb_resp_err);
}
