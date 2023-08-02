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

#include "sdctrl_regs.h"

sdctrl_regs::sdctrl_regs(GenObject *parent, const char *name) :
    ModuleObject(parent, "sdctrl_regs", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_pmapinfo(this, "i_pmapinfo", "APB interconnect slot information"),
    o_pcfg(this, "o_pcfg", "APB sd-controller configuration registers descriptor"),
    i_apbi(this, "i_apbi", "APB Slave to Bridge interface"),
    o_apbo(this, "o_apbo", "APB Bridge to Slave interface"),
    o_sck(this, "o_sck", "1", "SD-card clock usually upto 50 MHz"),
    o_sck_posedge(this, "o_sck_posedge", "1", "Strob just before positive edge"),
    o_sck_negedge(this, "o_sck_negedge", "1", "Strob just before negative edge"),
    // params
    // signals
    w_req_valid(this, "w_req_valid", "1"),
    wb_req_addr(this, "wb_req_addr", "32"),
    w_req_write(this, "w_req_write", "1"),
    wb_req_wdata(this, "wb_req_wdata", "32"),
    // registers
    sclk_ena(this, "sclk_ena", "1"),
    scaler(this, "scaler", "32"),
    scaler_cnt(this, "scaler_cnt", "32"),
    wdog(this, "wdog", "16"),
    wdog_cnt(this, "wdog_cnt", "16"),
    level(this, "level", "1"),
    resp_valid(this, "resp_valid", "1"),
    resp_rdata(this, "resp_rdata", "32"),
    resp_err(this, "resp_err", "1"),
    //
    comb(this),
    pslv0(this, "pslv0")
{
    Operation::start(this);

    pslv0.vid.setObjValue(&glob_pnp_cfg_->VENDOR_OPTIMITECH);
    pslv0.did.setObjValue(&glob_pnp_cfg_->OPTIMITECH_SDCTRL_REG);
    NEW(pslv0, pslv0.getName().c_str());
        CONNECT(pslv0, 0, pslv0.i_clk, i_clk);
        CONNECT(pslv0, 0, pslv0.i_nrst, i_nrst);
        CONNECT(pslv0, 0, pslv0.i_mapinfo, i_pmapinfo);
        CONNECT(pslv0, 0, pslv0.o_cfg, o_pcfg);
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

void sdctrl_regs::proc_comb() {
    TEXT("system bus clock scaler to baudrate:");
    IF (NZ(sclk_ena));
        IF (EQ(scaler_cnt, scaler));
            SETZERO(scaler_cnt);
            SETVAL(level, INV(level));
            SETVAL(comb.v_posedge, INV(level));
            SETVAL(comb.v_negedge, level);
        ELSE();
            SETVAL(scaler_cnt, INC(scaler_cnt));
        ENDIF();
    ENDIF();

    TEXT("Registers access:");
    SWITCH (BITS(wb_req_addr, 11, 2));
    CASE (CONST("0x0", 10), "0x00: sckdiv");
        SETVAL(comb.vb_rdata, scaler);
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            SETVAL(scaler, BITS(wb_req_wdata, 30, 0));
            SETZERO(scaler_cnt);
        ENDIF();
        ENDCASE();
    CASE (CONST("0x1", 10), "0x04: Global Control register");
        SETBIT(comb.vb_rdata, 0, sclk_ena);
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            SETVAL(sclk_ena, BIT(wb_req_wdata, 0));
        ENDIF();
        ENDCASE();
    CASE (CONST("0x2", 10), "0x08: reserved (watchdog)");
        SETBITS(comb.vb_rdata, 15, 0, wdog);
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            SETVAL(wdog, BITS(wb_req_wdata, 15, 0));
        ENDIF();
        ENDCASE();
    CASE (CONST("0x11", 10), "0x44: reserved 4 (txctrl)");
        ENDCASE();
    CASE (CONST("0x12", 10), "0x48: Tx FIFO Data");
        ENDCASE();
    CASE (CONST("0x13", 10), "0x4C: Rx FIFO Data");
        ENDCASE();
    CASE (CONST("0x14", 10), "0x50: Tx FIFO Watermark");
        ENDCASE();
    CASE (CONST("0x15", 10), "0x54: Rx FIFO Watermark");
        ENDCASE();
    CASE (CONST("0x16", 10), "0x58: CRC16 value (reserved FU740)");
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();
TEXT();
    SETVAL(resp_valid, w_req_valid);
    SETVAL(resp_rdata, comb.vb_rdata);
    SETZERO(resp_err);

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_sck, level);
    SETVAL(o_sck_posedge, comb.v_posedge);
    SETVAL(o_sck_negedge, comb.v_negedge);
}
