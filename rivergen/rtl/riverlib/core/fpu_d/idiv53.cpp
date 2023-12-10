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

#include "idiv53.h"

idiv53::idiv53(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "idiv53", name, comment),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_ena(this, "i_ena", "1", "divider enable pulse (1 clock)"),
    i_divident(this, "i_divident", "53", "integer value"),
    i_divisor(this, "i_divisor", "53", "integer value"),
    o_result(this, "o_result", "105", "resulting bits"),
    o_lshift(this, "o_lshift", "7", "first non-zero bit index"),
    o_rdy(this, "o_rdy", "1", "delayed 'enable' signal"),
    o_overflow(this, "o_overflow", "1", "overflow flag"),
    o_zero_resid(this, "o_zero_resid", "1", "reasidual is zero flag"),
    // signals
    w_mux_ena_i(this, "w_mux_ena_i", "1"),
    wb_muxind_i(this, "wb_muxind_i", "56"),
    wb_divident_i(this, "wb_divident_i", "61"),
    wb_divisor_i(this, "wb_divisor_i", "53"),
    wb_dif_o(this, "wb_dif_o", "53"),
    wb_bits_o(this, "wb_bits_o", "8"),
    wb_muxind_o(this, "wb_muxind_o", "7"),
    w_muxind_rdy_o(this, "w_muxind_rdy_o", "1"),
    // registers
    delay(this, "delay", "15", "'0", NO_COMMENT),
    lshift(this, "lshift", "7", "'0", NO_COMMENT),
    lshift_rdy(this, "lshift_rdy", "1"),
    divisor(this, "divisor", "53", "'0", NO_COMMENT),
    divident(this, "divident", "61", "'0", NO_COMMENT),
    bits(this, "bits", "105", "'0", NO_COMMENT),
    overflow(this, "overflow", "1"),
    zero_resid(this, "zero_resid", "1"),
    // process
    comb(this),
    divstage0(this, "divstage0")
{
    Operation::start(this);

    NEW(divstage0, divstage0.getName().c_str());
        CONNECT(divstage0, 0, divstage0.i_mux_ena, w_mux_ena_i);
        CONNECT(divstage0, 0, divstage0.i_muxind, wb_muxind_i);
        CONNECT(divstage0, 0, divstage0.i_divident, wb_divident_i);
        CONNECT(divstage0, 0, divstage0.i_divisor, wb_divisor_i);
        CONNECT(divstage0, 0, divstage0.o_dif, wb_dif_o);
        CONNECT(divstage0, 0, divstage0.o_bits, wb_bits_o);
        CONNECT(divstage0, 0, divstage0.o_muxind, wb_muxind_o);
        CONNECT(divstage0, 0, divstage0.o_muxind_rdy, w_muxind_rdy_o);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void idiv53::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;

    SETVAL(comb.vb_bits, bits);

TEXT();
    SETVAL(comb.v_ena, i_ena);
    SETVAL(delay, CC2(BITS(delay, 13, 0), comb.v_ena));
    IF (NZ(i_ena));
        SETVAL(divident, CC2(CONST("0", 8), i_divident));
        SETVAL(divisor, i_divisor);
        SETZERO(lshift_rdy);
        SETZERO(overflow);
        SETZERO(zero_resid);
    ELSIF (NZ(BIT(delay, 0)));
        SETVAL(comb.v_mux_ena_i, INV(lshift_rdy));
        SETVAL(divident, CC2(wb_dif_o, CONST("0", 8)));
        SETBIT(comb.vb_bits, 104, INV(BIT(wb_dif_o, 52)));
    ELSIF (NZ(BIT(delay, 1)));
        SETVAL(comb.v_mux_ena_i, INV(lshift_rdy));
        SETVAL(divident, CC2(wb_dif_o, CONST("0", 8)));
        SETBITS(comb.vb_muxind, 55, 49, CONST("1"));
        SETBITS(comb.vb_muxind, 48, 42, CONST("2"));
        SETBITS(comb.vb_muxind, 41, 35, CONST("3"));
        SETBITS(comb.vb_muxind, 34, 28, CONST("4"));
        SETBITS(comb.vb_muxind, 27, 21, CONST("5"));
        SETBITS(comb.vb_muxind, 20, 14, CONST("6"));
        SETBITS(comb.vb_muxind, 13, 7, CONST("7"));
        SETBITS(comb.vb_muxind, 6, 0, CONST("8"));
        SETBITS(comb.vb_bits, 103, 96, wb_bits_o);
    ELSIF (NZ(BIT(delay, 2)));
        SETVAL(comb.v_mux_ena_i, INV(lshift_rdy));
        SETVAL(divident, CC2(wb_dif_o, CONST("0", 8)));
        SETBITS(comb.vb_muxind, 55, 49, CONST("9"));
        SETBITS(comb.vb_muxind, 48, 42, CONST("10"));
        SETBITS(comb.vb_muxind, 41, 35, CONST("11"));
        SETBITS(comb.vb_muxind, 34, 28, CONST("12"));
        SETBITS(comb.vb_muxind, 27, 21, CONST("13"));
        SETBITS(comb.vb_muxind, 20, 14, CONST("14"));
        SETBITS(comb.vb_muxind, 13, 7, CONST("15"));
        SETBITS(comb.vb_muxind, 6, 0, CONST("16"));
        SETBITS(comb.vb_bits, 95, 88, wb_bits_o);
    ELSIF (NZ(BIT(delay, 3)));
        SETVAL(comb.v_mux_ena_i, INV(lshift_rdy));
        SETVAL(divident, CC2(wb_dif_o, CONST("0", 8)));
        SETBITS(comb.vb_muxind, 55, 49, CONST("17"));
        SETBITS(comb.vb_muxind, 48, 42, CONST("18"));
        SETBITS(comb.vb_muxind, 41, 35, CONST("19"));
        SETBITS(comb.vb_muxind, 34, 28, CONST("20"));
        SETBITS(comb.vb_muxind, 27, 21, CONST("21"));
        SETBITS(comb.vb_muxind, 20, 14, CONST("22"));
        SETBITS(comb.vb_muxind, 13, 7, CONST("23"));
        SETBITS(comb.vb_muxind, 6, 0, CONST("24"));
        SETBITS(comb.vb_bits, 87, 80, wb_bits_o);
    ELSIF (NZ(BIT(delay, 4)));
        SETVAL(comb.v_mux_ena_i, INV(lshift_rdy));
        SETVAL(divident, CC2(wb_dif_o, CONST("0", 8)));
        SETBITS(comb.vb_muxind, 55, 49, CONST("25"));
        SETBITS(comb.vb_muxind, 48, 42, CONST("26"));
        SETBITS(comb.vb_muxind, 41, 35, CONST("27"));
        SETBITS(comb.vb_muxind, 34, 28, CONST("28"));
        SETBITS(comb.vb_muxind, 27, 21, CONST("29"));
        SETBITS(comb.vb_muxind, 20, 14, CONST("30"));
        SETBITS(comb.vb_muxind, 13, 7, CONST("31"));
        SETBITS(comb.vb_muxind, 6, 0, CONST("32"));
        SETBITS(comb.vb_bits, 79, 72, wb_bits_o);
    ELSIF (NZ(BIT(delay, 5)));
        SETVAL(comb.v_mux_ena_i, INV(lshift_rdy));
        SETVAL(divident, CC2(wb_dif_o, CONST("0", 8)));
        SETBITS(comb.vb_muxind, 55, 49, CONST("33"));
        SETBITS(comb.vb_muxind, 48, 42, CONST("34"));
        SETBITS(comb.vb_muxind, 41, 35, CONST("35"));
        SETBITS(comb.vb_muxind, 34, 28, CONST("36"));
        SETBITS(comb.vb_muxind, 27, 21, CONST("37"));
        SETBITS(comb.vb_muxind, 20, 14, CONST("38"));
        SETBITS(comb.vb_muxind, 13, 7, CONST("39"));
        SETBITS(comb.vb_muxind, 6, 0, CONST("40"));
        SETBITS(comb.vb_bits, 71, 64, wb_bits_o);
    ELSIF (NZ(BIT(delay, 6)));
        SETVAL(comb.v_mux_ena_i, INV(lshift_rdy));
        SETVAL(divident, CC2(wb_dif_o, CONST("0", 8)));
        SETBITS(comb.vb_muxind, 55, 49, CONST("41"));
        SETBITS(comb.vb_muxind, 48, 42, CONST("42"));
        SETBITS(comb.vb_muxind, 41, 35, CONST("43"));
        SETBITS(comb.vb_muxind, 34, 28, CONST("44"));
        SETBITS(comb.vb_muxind, 27, 21, CONST("45"));
        SETBITS(comb.vb_muxind, 20, 14, CONST("46"));
        SETBITS(comb.vb_muxind, 13, 7, CONST("47"));
        SETBITS(comb.vb_muxind, 6, 0, CONST("48"));
        SETBITS(comb.vb_bits, 63, 56, wb_bits_o);
    ELSIF (NZ(BIT(delay, 7)));
        SETVAL(comb.v_mux_ena_i, INV(lshift_rdy));
        SETVAL(divident, CC2(wb_dif_o, CONST("0", 8)));
        SETBITS(comb.vb_muxind, 55, 49, CONST("49"));
        SETBITS(comb.vb_muxind, 48, 42, CONST("50"));
        SETBITS(comb.vb_muxind, 41, 35, CONST("51"));
        SETBITS(comb.vb_muxind, 34, 28, CONST("52"));
        SETBITS(comb.vb_muxind, 27, 21, CONST("53"));
        SETBITS(comb.vb_muxind, 20, 14, CONST("54"));
        SETBITS(comb.vb_muxind, 13, 7, CONST("55"));
        SETBITS(comb.vb_muxind, 6, 0, CONST("56"));
        SETBITS(comb.vb_bits, 55, 48, wb_bits_o);
    ELSIF (NZ(BIT(delay, 8)));
        SETVAL(comb.v_mux_ena_i, INV(lshift_rdy));
        SETVAL(divident, CC2(wb_dif_o, CONST("0", 8)));
        SETBITS(comb.vb_muxind, 55, 49, CONST("57"));
        SETBITS(comb.vb_muxind, 48, 42, CONST("58"));
        SETBITS(comb.vb_muxind, 41, 35, CONST("59"));
        SETBITS(comb.vb_muxind, 34, 28, CONST("60"));
        SETBITS(comb.vb_muxind, 27, 21, CONST("61"));
        SETBITS(comb.vb_muxind, 20, 14, CONST("62"));
        SETBITS(comb.vb_muxind, 13, 7, CONST("63"));
        SETBITS(comb.vb_muxind, 6, 0, CONST("64"));
        SETBITS(comb.vb_bits, 47, 40, wb_bits_o);
    ELSIF (NZ(BIT(delay, 9)));
        SETVAL(comb.v_mux_ena_i, INV(lshift_rdy));
        SETVAL(divident, CC2(wb_dif_o, CONST("0", 8)));
        SETBITS(comb.vb_muxind, 55, 49, CONST("65"));
        SETBITS(comb.vb_muxind, 48, 42, CONST("66"));
        SETBITS(comb.vb_muxind, 41, 35, CONST("67"));
        SETBITS(comb.vb_muxind, 34, 28, CONST("68"));
        SETBITS(comb.vb_muxind, 27, 21, CONST("69"));
        SETBITS(comb.vb_muxind, 20, 14, CONST("70"));
        SETBITS(comb.vb_muxind, 13, 7, CONST("71"));
        SETBITS(comb.vb_muxind, 6, 0, CONST("72"));
        SETBITS(comb.vb_bits, 39, 32, wb_bits_o);
    ELSIF (NZ(BIT(delay, 10)));
        SETVAL(comb.v_mux_ena_i, INV(lshift_rdy));
        SETVAL(divident, CC2(wb_dif_o, CONST("0", 8)));
        SETBITS(comb.vb_muxind, 55, 49, CONST("73"));
        SETBITS(comb.vb_muxind, 48, 42, CONST("74"));
        SETBITS(comb.vb_muxind, 41, 35, CONST("75"));
        SETBITS(comb.vb_muxind, 34, 28, CONST("76"));
        SETBITS(comb.vb_muxind, 27, 21, CONST("77"));
        SETBITS(comb.vb_muxind, 20, 14, CONST("78"));
        SETBITS(comb.vb_muxind, 13, 7, CONST("79"));
        SETBITS(comb.vb_muxind, 6, 0, CONST("80"));
        SETBITS(comb.vb_bits, 31, 24, wb_bits_o);
    ELSIF (NZ(BIT(delay, 11)));
        SETVAL(comb.v_mux_ena_i, INV(lshift_rdy));
        SETVAL(divident, CC2(wb_dif_o, CONST("0", 8)));
        SETBITS(comb.vb_muxind, 55, 49, CONST("81"));
        SETBITS(comb.vb_muxind, 48, 42, CONST("82"));
        SETBITS(comb.vb_muxind, 41, 35, CONST("83"));
        SETBITS(comb.vb_muxind, 34, 28, CONST("84"));
        SETBITS(comb.vb_muxind, 27, 21, CONST("85"));
        SETBITS(comb.vb_muxind, 20, 14, CONST("86"));
        SETBITS(comb.vb_muxind, 13, 7, CONST("87"));
        SETBITS(comb.vb_muxind, 6, 0, CONST("88"));
        SETBITS(comb.vb_bits, 23, 16, wb_bits_o);
    ELSIF (NZ(BIT(delay, 12)));
        SETVAL(comb.v_mux_ena_i, INV(lshift_rdy));
        SETVAL(divident, CC2(wb_dif_o, CONST("0", 8)));
        SETBITS(comb.vb_muxind, 55, 49, CONST("89"));
        SETBITS(comb.vb_muxind, 48, 42, CONST("90"));
        SETBITS(comb.vb_muxind, 41, 35, CONST("91"));
        SETBITS(comb.vb_muxind, 34, 28, CONST("92"));
        SETBITS(comb.vb_muxind, 27, 21, CONST("93"));
        SETBITS(comb.vb_muxind, 20, 14, CONST("94"));
        SETBITS(comb.vb_muxind, 13, 7, CONST("95"));
        SETBITS(comb.vb_muxind, 6, 0, CONST("96"));
        SETBITS(comb.vb_bits, 15, 8, wb_bits_o);
    ELSIF (NZ(BIT(delay, 13)));
        SETVAL(comb.v_mux_ena_i, INV(lshift_rdy));
        SETVAL(divident, CC2(wb_dif_o, CONST("0", 8)));
        SETBITS(comb.vb_muxind, 55, 49, CONST("97"));
        SETBITS(comb.vb_muxind, 48, 42, CONST("98"));
        SETBITS(comb.vb_muxind, 41, 35, CONST("99"));
        SETBITS(comb.vb_muxind, 34, 28, CONST("100"));
        SETBITS(comb.vb_muxind, 27, 21, CONST("101"));
        SETBITS(comb.vb_muxind, 20, 14, CONST("102"));
        SETBITS(comb.vb_muxind, 13, 7, CONST("103"));
        SETBITS(comb.vb_muxind, 6, 0, CONST("104"));
        SETBITS(comb.vb_bits, 7, 0, wb_bits_o);

TEXT();
        IF (EZ(wb_dif_o));
            SETONE(zero_resid);
        ENDIF();
        IF (EQ(lshift, CONST("0x7F", 7)));
            SETONE(overflow);
        ENDIF();
    ENDIF();

TEXT();
    IF (EZ(lshift_rdy));
        IF (NZ(w_muxind_rdy_o));
            SETONE(lshift_rdy);
            SETVAL(lshift, wb_muxind_o);
        ELSIF (NZ(BIT(delay, 13)));
            SETONE(lshift_rdy);
            SETVAL(lshift, CONST("104", 7));
        ENDIF();
    ENDIF();

TEXT();
    SETVAL(w_mux_ena_i, comb.v_mux_ena_i);
    SETVAL(wb_divident_i, divident);
    SETVAL(wb_divisor_i, divisor);
    SETVAL(wb_muxind_i, comb.vb_muxind);
    SETVAL(bits, comb.vb_bits);


TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_result, bits);
    SETVAL(o_lshift, lshift);
    SETVAL(o_overflow, overflow);
    SETVAL(o_zero_resid, zero_resid);
    SETVAL(o_rdy, BIT(delay, 14));
}

