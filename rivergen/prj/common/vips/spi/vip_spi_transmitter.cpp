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

#include "vip_spi_transmitter.h"

vip_spi_transmitter::vip_spi_transmitter(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "vip_spi_transmitter", name, comment),
    scaler(this, "scaler", "8"),
    scaler_max(this, "scaler_max", "SUB(MUL(2,scaler),1)"),
    scaler_mid(this, "scaler_mid", "scaler"),
    i_nrst(this, "i_nrst", "1"),
    i_clk(this, "i_clk", "1"),
    i_csn(this, "i_csn", "1"),
    i_sclk(this, "i_sclk", "1"),
    i_mosi(this, "i_mosi", "1"),
    o_miso(this, "o_miso", "1"),
    o_req_valid(this, "o_req_valid", "1"),
    o_req_write(this, "o_req_write", "1"),
    o_req_addr(this, "o_req_addr", "32"),
    o_req_wdata(this, "o_req_wdata", "32"),
    i_req_ready(this, "i_req_ready", "1"),
    i_resp_valid(this, "i_resp_valid", "1"),
    i_resp_rdata(this, "i_resp_rdata", "32"),
    o_resp_ready(this, "o_resp_ready", "1"),
    // params
    state_cmd(this, "state_cmd", "2", "0", NO_COMMENT),
    state_addr(this, "state_addr", "2", "1", NO_COMMENT),
    state_data(this, "state_data", "2", "2", NO_COMMENT),
    // signals
    // registers
    state(this, "state", "2", "state_cmd", NO_COMMENT),
    sclk(this, "sclk", "1"),
    rxshift(this, "rxshift", "32", "'1", NO_COMMENT),
    txshift(this, "txshift", "32", "'1", NO_COMMENT),
    bitcnt(this, "bitcnt", "4", "'0", NO_COMMENT),
    bytecnt(this, "bytecnt", "3", "'0", NO_COMMENT),
    byterdy(this, "byterdy", "1"),
    req_valid(this, "req_valid", "1"),
    req_write(this, "req_write", "1"),
    req_addr(this, "req_addr", "32", "'0", NO_COMMENT),
    req_wdata(this, "req_wdata", "32", "'0", NO_COMMENT),
    //
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void vip_spi_transmitter::proc_comb() {
    SETZERO(byterdy);
    IF (AND2(NZ(req_valid), NZ(i_req_ready)));
        SETZERO(req_valid);
    ENDIF();

TEXT();
    SETVAL(sclk, i_sclk);
    SETVAL(comb.v_pos, AND2(INV(sclk), i_sclk));
    SETVAL(comb.v_neg, AND2(sclk, INV(i_sclk)));

TEXT();
    IF (AND2(EZ(i_csn), NZ(comb.v_pos)));
        SETVAL(rxshift, CC2(BITS(rxshift, 31, 0), i_mosi));
        SETVAL(bitcnt, INC(bitcnt));
        IF (EQ(bitcnt, CONST("0x7", 4)));
            SETONE(byterdy);
        ENDIF();
    ELSIF(NZ(i_csn));
        SETZERO(bitcnt);
    ENDIF();

TEXT();
    
    IF(AND2(EZ(i_csn), NZ(comb.v_neg)));
        SETONE(comb.v_resp_ready);
        IF (NZ(i_resp_valid));
            TEXT("There's one negedge before CSn goes high:");
            SETVAL(txshift, i_resp_rdata);
        ELSE();
            SETVAL(txshift, CC2(BITS(txshift, 31, 0), CONST("1", 1)));
        ENDIF();
    ENDIF();

TEXT();
    IF (NZ(byterdy));
        SWITCH (state);
        CASE(state_cmd);
            SETZERO(bytecnt);
            IF (EQ(BITS(rxshift, 7, 0), CONST("0x41", 8)));
                SETVAL(state, state_addr);
                SETZERO(req_write, "Read request");
            ELSIF (EQ(BITS(rxshift, 7, 0), CONST("0x42", 8)));
                SETVAL(state, state_addr);
                SETONE(req_write, "Write request");
            ENDIF();
            ENDCASE();

        CASE(state_addr);
            SETVAL(bytecnt, INC(bytecnt));
            IF (EQ(bytecnt, CONST("3", 3)));
                SETZERO(bytecnt);
                SETVAL(state, state_data);
                SETVAL(req_addr, rxshift);
                SETVAL(req_valid, INV(req_write));
            ENDIF();
            ENDCASE();

        CASE(state_data);
            SETVAL(bytecnt, INC(bytecnt));
            IF (EQ(bytecnt, CONST("3", 3)));
                SETZERO(bytecnt);
                SETVAL(state, state_cmd);
                SETVAL(req_wdata, rxshift);
                SETVAL(req_valid, req_write);
            ENDIF();
            ENDCASE();

        CASEDEF();
            SETVAL(state, state_cmd);
            ENDCASE();
        ENDSWITCH();
    ENDIF();

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_req_valid, req_valid);
    SETVAL(o_req_write, req_write);
    SETVAL(o_req_addr, req_addr);
    SETVAL(o_req_wdata, req_wdata);
    SETVAL(o_resp_ready, comb.v_resp_ready);
    SETVAL(o_miso, BIT(txshift, 31));
}
