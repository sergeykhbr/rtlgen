// 
//  Copyright 2022 Sergey Khabarov, sergeykhbr@gmail.com
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

#include "apb_ddr.h"

apb_ddr::apb_ddr(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "apb_ddr", name, comment),
    i_clk(this, "i_clk", "1", "APB clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_mapinfo(this, "i_mapinfo", "interconnect slot information"),
    o_cfg(this, "o_cfg", "Device descriptor"),
    i_apbi(this, "i_apbi", "APB input interface"),
    o_apbo(this, "o_apbo", "APB output interface"),
    i_pll_locked(this, "i_pll_locked", "1", "PLL locked"),
    i_init_calib_done(this, "i_init_calib_done", "1", "DDR initialization done"),
    i_device_temp(this, "i_device_temp", "12", "Temperature monitor value"),
    i_sr_active(this, "i_sr_active", "1"),
    i_ref_ack(this, "i_ref_ack", "1"),
    i_zq_ack(this, "i_zq_ack", "1"),
    // params
    // signals
    w_req_valid(this, "w_req_valid", "1"),
    wb_req_addr(this, "wb_req_addr", "32"),
    w_req_write(this, "w_req_write", "1"),
    wb_req_wdata(this, "wb_req_wdata", "32"),
    // registers
    pll_locked(this, "pll_locked", "1", "0"),
    init_calib_done(this, "init_calib_done", "1", "0"),
    device_temp(this, "device_temp", "12", "'0", NO_COMMENT),
    sr_active(this, "sr_active", "1", "0"),
    ref_ack(this, "ref_ack", "1", "0"),
    zq_ack(this, "zq_ack", "1", "0"),
    resp_valid(this, "resp_valid", "1", "0"),
    resp_rdata(this, "resp_rdata", "32", "'0", NO_COMMENT),
    resp_err(this, "resp_err", "1", "0"),
    //
    comb(this),
    pslv0(this, "pslv0", NO_COMMENT)
{
    Operation::start(this);

    pslv0.vid.setObjValue(&glob_pnp_cfg_->VENDOR_OPTIMITECH);
    pslv0.did.setObjValue(&glob_pnp_cfg_->OPTIMITECH_DDRCTRL);
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

void apb_ddr::proc_comb() {
    SETVAL(pll_locked, i_pll_locked);
    SETVAL(init_calib_done, i_init_calib_done);
    SETVAL(device_temp, i_device_temp);
    SETVAL(sr_active, i_sr_active);
    SETVAL(ref_ack, i_ref_ack);
    SETVAL(zq_ack, i_zq_ack);

TEXT();
    SETZERO(resp_err);
    TEXT("Registers access:");
    SWITCH (BITS(wb_req_addr, 11, 2));
    CASE (CONST("0", 10), "0x00: clock status");
        SETBIT(comb.vb_rdata, 0, pll_locked);
        SETBIT(comb.vb_rdata, 1, init_calib_done);
        ENDCASE();
    CASE (CONST("1", 10), "0x04: temperature");
        SETBITS(comb.vb_rdata, 11, 0, device_temp); 
        ENDCASE();
    CASE (CONST("2", 10), "0x08: app bits");
        SETBIT(comb.vb_rdata, 0, sr_active, "[0] ");
        SETBIT(comb.vb_rdata, 1, ref_ack, "[1] ");
        SETBIT(comb.vb_rdata, 2, zq_ack, "[2] ");
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    SETVAL(resp_valid, w_req_valid);
    SETVAL(resp_rdata, comb.vb_rdata);

TEXT();
    SYNC_RESET();
}
