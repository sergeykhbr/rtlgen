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

vip_uart_receiver::vip_uart_receiver(GenObject *parent, const char *name) :
    ModuleObject(parent, "vip_uart_receiver", name),
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
    startbit(this, "2", "startbit", "0"),
    data(this, "2", "data", "1"),
    stopbit(this, "2", "stopbit", "2"),
    // signals
    // registers
    state(this, "state", "2", "startbit"),
    rdy(this, "rdy", "1"),
    rdata(this, "rdata", "8"),
    sample(this, "sample", "32"),
    bitpos(this, "bitpos", "4"),
    scratch(this, "scratch", "8"),
    //
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void vip_uart_receiver::proc_comb() {
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
        IF (OR2(NZ(i_rx), NZ(sample)));
            SETVAL(sample, INC(sample));
        ENDIF();

        TEXT();
        IF(EQ(sample, scaler_max));
            SETVAL(state, data);
            SETZERO(bitpos);
            SETZERO(sample);
            SETZERO(scratch);
        ENDIF();
        ENDCASE();

    CASE(data);
        IF (EQ(sample, scaler_max));
            SETZERO(sample);
        ELSE();
            SETVAL(sample, INC(sample));
        ENDIF();

        TEXT();
        IF (EQ(sample, scaler_mid));
            SETVAL(scratch, CC2(i_rx, BITS(scratch, 7, 1)));
            SETVAL(bitpos, INC(bitpos));
        ENDIF();
        IF (AND2(EQ(bitpos, CONST("8", 8)), EQ(sample, scaler_mid)));
            SETVAL(state, stopbit);
        ENDIF();
        ENDCASE();

    CASE(stopbit);
		TEXT("");
		TEXT("Our baud clock may not be running at exactly the");
		TEXT("same rate as the transmitter.  If we thing that");
		TEXT("we're at least half way into the stop bit, allow");
		TEXT("transition into handling the next start bit.");
		TEXT("");
        IF(OR2(EQ(sample, scaler_max), AND2(GE(sample, scaler_mid), NZ(i_rx))));
            SETVAL(state, startbit);
            SETVAL(rdata, scratch);
            SETONE(rdy);
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
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_rdy, rdy);
    SETVAL(o_data, rdata);
}
