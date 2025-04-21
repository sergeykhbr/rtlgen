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

#include "vip_uart_receiver.h"

vip_uart_receiver::vip_uart_receiver(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "vip_uart_receiver", name, comment),
    scaler(this, "scaler", "8"),
    scaler_max(this, "scaler_max", "SUB(MUL(2,scaler),1)"),
    scaler_mid(this, "scaler_mid", "scaler"),
    i_nrst(this, "i_nrst", "1"),
    i_clk(this, "i_clk", "1"),
    i_rx(this, "i_rx", "1"),
    o_rdy(this, "o_rdy", "1"),
    i_rdy_clr(this, "i_rdy_clr", "1"),
    o_data(this, "o_data", "8"),
    // params
    startbit(this, "startbit", "2", "0", NO_COMMENT),
    data(this, "data", "2", "1", NO_COMMENT),
    stopbit(this, "stopbit", "2", "2", NO_COMMENT),
    dummy(this, "dummy", "2", "3", NO_COMMENT),
    // signals
    // registers
    rx(this, "rx", "1"),
    state(this, "state", "2", "startbit"),
    rdy(this, "rdy", "1"),
    rdata(this, "rdata", "8"),
    sample(this, "sample", "32"),
    bitpos(this, "bitpos", "4"),
    scratch(this, "scratch", "8"),
    rx_err(this, "rx_err", "1"),
    //
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void vip_uart_receiver::proc_comb() {
    SETVAL(rx, i_rx);
    SETVAL(comb.v_rx_pos, AND2(INV(rx), i_rx));
    SETVAL(comb.v_rx_neg, AND2(rx, INV(i_rx)));
    IF (NZ(i_rdy_clr));
        SETZERO(rdy);
    ENDIF();

TEXT();
    SWITCH (state);
    CASE(startbit);
        TEXT("");
        TEXT("Start counting from the first low sample, once we've");
        TEXT("sampled a full bit, start collecting data bits.");
        TEXT("");
        IF (OR2(EZ(i_rx), NZ(sample)));
            SETVAL(sample, INC(sample));
        ENDIF();

        TEXT();
        IF(OR2(EQ(sample, scaler_max), NZ(comb.v_rx_pos)));
            SETVAL(state, data);
            SETZERO(bitpos);
            SETZERO(sample);
            SETZERO(scratch);
            SETZERO(rx_err);
        ENDIF();
        ENDCASE();

    CASE(data);
        IF (ORx(2, &EQ(sample, scaler_max),
                   &AND2(GT(sample, scaler_mid), OR2(NZ(comb.v_rx_neg), NZ(comb.v_rx_pos)))));
            SETZERO(sample);
            IF (EQ(bitpos, CONST("8", 8)));
                SETVAL(state, stopbit);
            ENDIF();
        ELSE();
            SETVAL(sample, INC(sample));
        ENDIF();

        TEXT();
        IF (EQ(sample, scaler_mid));
            SETVAL(scratch, CC2(i_rx, BITS(scratch, 7, 1)));
            SETVAL(bitpos, INC(bitpos));
        ENDIF();
        ENDCASE();

    CASE(stopbit);
        IF(EQ(sample, scaler_mid));
            SETVAL(rdata, scratch);
            SETONE(rdy);
            IF (EZ(i_rx));
                SETONE(rx_err);
            ELSE();
            ENDIF();
        ENDIF();
        IF (EQ(sample, scaler_max));
            SETVAL(state, startbit, "dummy bit disabled");
            SETZERO(sample);
        ELSE();
            SETVAL(sample, INC(sample));
        ENDIF();
        ENDCASE();

    CASE(dummy);
        TEXT("Idle state in UART generates additional byte and it works");
        TEXT("even if rx=0 on real device:");
        IF(GE(sample, scaler_mid));
            SETVAL(state, startbit);
            SETZERO(sample);
        ELSE();
            SETVAL(sample, INC(sample));
        ENDIF();
        ENDCASE();

    CASEDEF();
        SETVAL(state, startbit);
        ENDCASE();
    ENDSWITCH();

TEXT();
    SYNC_RESET();

TEXT();
    SETVAL(o_rdy, rdy);
    SETVAL(o_data, rdata);
}
