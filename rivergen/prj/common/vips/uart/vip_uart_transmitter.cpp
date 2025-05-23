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

#include "vip_uart_transmitter.h"

vip_uart_transmitter::vip_uart_transmitter(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "vip_uart_transmitter", name, comment),
    scaler(this, "scaler", "8"),
    scaler_max(this, "scaler_max", "SUB(MUL(2,scaler),1)"),
    i_nrst(this, "i_nrst", "1"),
    i_clk(this, "i_clk", "1"),
    i_we(this, "i_we", "1"),
    i_wdata(this, "i_wdata", "8"),
    o_full(this, "o_full", "1"),
    o_tx(this, "o_tx", "1"),
    // params
    idle(this, "idle", "2", "0x0", NO_COMMENT),
    startbit(this, "startbit", "2", "0x1", NO_COMMENT),
    data(this, "data", "2", "0x2", NO_COMMENT),
    stopbit(this, "stopbit", "2", "0x3", NO_COMMENT),
    // signals
    // registers
    state(this, "state", "2", "idle", NO_COMMENT),
    sample(this, "sample", "32", "'0", NO_COMMENT),
    txdata_rdy(this, "txdata_rdy", "1", RSTVAL_ZERO, NO_COMMENT),
    txdata(this, "txdata", "8", "'0", NO_COMMENT),
    shiftreg(this, "shiftreg", "9", "'1", NO_COMMENT),
    bitpos(this, "bitpos", "4", "'0", NO_COMMENT),
    overflow(this, "overflow", "1", RSTVAL_ZERO, NO_COMMENT),
    //
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void vip_uart_transmitter::proc_comb() {
    IF(NZ(i_we));
        IF (NZ(txdata_rdy));
            SETONE(overflow);
        ELSE();
            SETONE(txdata_rdy);
            SETVAL(txdata, i_wdata);
        ENDIF();
    ENDIF();

TEXT();
    IF (EQ(sample, scaler_max));
        SETZERO(sample);
        SETONE(comb.v_next);
    ELSE();
        SETVAL(sample, INC(sample));
    ENDIF();

TEXT();
    IF (NZ(comb.v_next));
        SWITCH (state);
        CASE(idle);
            IF (NZ(txdata_rdy));
                SETZERO(txdata_rdy);
                SETZERO(overflow);
                SETVAL(shiftreg, CC2(txdata, CONST("0", 1)));
                SETVAL(state, startbit);
            ENDIF();
            ENDCASE();

        CASE(startbit);
            SETVAL(state, data);
            SETZERO(bitpos);
            SETVAL(shiftreg, CC2(CONST("1", 1), BITS(shiftreg, 8, 1)));
            ENDCASE();

        CASE(data);
            IF (EQ(bitpos, CONST("7", 4)));
                SETVAL(state, idle, "No dummy bit at the end");
                SETVAL(shiftreg, ALLONES());
            ELSE();
                SETVAL(shiftreg, CC2(CONST("1", 1), BITS(shiftreg, 8, 1)));
            ENDIF();
            SETVAL(bitpos, INC(bitpos));
            ENDCASE();

        CASE(stopbit);
            SETVAL(state, idle);
            ENDCASE();
        CASEDEF();
            ENDCASE();
        ENDSWITCH();
    ENDIF();

TEXT();
    SYNC_RESET();

TEXT();
    SETVAL(o_full, txdata_rdy);
    SETVAL(o_tx, BIT(shiftreg, 0));
}
