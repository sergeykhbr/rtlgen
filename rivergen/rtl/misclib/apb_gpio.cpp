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

#include "apb_gpio.h"

apb_gpio::apb_gpio(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "apb_gpio", name, comment),
    width(this, "width", "12"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_mapinfo(this, "i_mapinfo", "interconnect slot information"),
    o_cfg(this, "o_cfg", "Device descriptor"),
    i_apbi(this, "i_apbi", "APB Slave to Bridge interface"),
    o_apbo(this, "o_apbo", "APB Bridge to Slave interface"),
    i_gpio(this, "i_gpio", "width"),
    o_gpio_dir(this, "o_gpio_dir", "width", "1 as input; 0 as output"),
    o_gpio(this, "o_gpio", "width"),
    o_irq(this, "o_irq", "width"),
    // params
    // signals
    w_req_valid(this, "w_req_valid", "1"),
    wb_req_addr(this, "wb_req_addr", "32"),
    w_req_write(this, "w_req_write", "1"),
    wb_req_wdata(this, "wb_req_wdata", "32"),
    // registers
    input_val(this, "input_val", "width"),
    input_en(this, "input_en", "width", "'1"),
    output_en(this, "output_en", "width"),
    output_val(this, "output_val", "width"),
    ie(this, "ie", "width"),
    ip(this, "ip", "width"),
    resp_valid(this, "resp_valid", "1"),
    resp_rdata(this, "resp_rdata", "32"),
    resp_err(this, "resp_err", "1"),
    //
    comb(this),
    pslv0(this, "pslv0", NO_COMMENT)
{
    Operation::start(this);

    pslv0.vid.setObjValue(&glob_pnp_cfg_->VENDOR_OPTIMITECH);
    pslv0.did.setObjValue(&glob_pnp_cfg_->OPTIMITECH_GPIO);
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

void apb_gpio::proc_comb() {
    SETVAL(input_val, AND2_L(i_gpio, input_en));

TEXT();
    TEXT("Registers access:");
    SWITCH (BITS(wb_req_addr, 11, 2));
    CASE (CONST("0", 10), "0x00: RO input_val");
        SETBITS(comb.vb_rdata, DEC(width), CONST("0"), input_val);
        ENDCASE();
    CASE (CONST("1", 10), "0x04: input_en");
        SETBITS(comb.vb_rdata, DEC(width), CONST("0"), input_en);
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            SETVAL(input_en, BITS(wb_req_wdata, DEC(width), CONST("0")));
        ENDIF();
        ENDCASE();
    CASE (CONST("2", 10), "0x08: output_en");
        SETBITS(comb.vb_rdata, DEC(width), CONST("0"), output_en);
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            SETVAL(output_en, BITS(wb_req_wdata, DEC(width), CONST("0")));
        ENDIF();
        ENDCASE();
    CASE (CONST("3", 10), "0x0C: output_val");
        SETBITS(comb.vb_rdata, DEC(width), CONST("0"), output_val);
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            SETVAL(output_val, BITS(wb_req_wdata, DEC(width), CONST("0")));
        ENDIF();
        ENDCASE();
    CASE (CONST("4", 10), "0x10: ie");
        SETBITS(comb.vb_rdata, DEC(width), CONST("0"), ie);
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            SETVAL(ie, BITS(wb_req_wdata, DEC(width), CONST("0")));
        ENDIF();
        ENDCASE();
    CASE (CONST("5", 10), "0x14: ip");
        SETBITS(comb.vb_rdata, DEC(width), CONST("0"), ip);
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            SETVAL(ip, BITS(wb_req_wdata, DEC(width), CONST("0")));
        ENDIF();
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
    SETVAL(o_gpio_dir, input_en);
    SETVAL(o_gpio, output_val);
    SETVAL(o_irq, AND2_L(ie, ip));
}
