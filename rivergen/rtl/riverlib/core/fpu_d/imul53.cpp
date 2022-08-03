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

#include "imul53.h"

imul53::imul53(GenObject *parent, const char *name) :
    ModuleObject(parent, "idiv53", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_ena(this, "i_ena", "1", "enable pulse (1 clock)"),
    i_a(this, "i_a", "53", "integer value"),
    i_b(this, "i_b", "53", "integer value"),
    o_result(this, "o_result", "106", "resulting bits"),
    o_shift(this, "o_shift", "7", "first non-zero bit index"),
    o_rdy(this, "o_rdy", "1", "delayed 'enable' signal"),
    o_overflow(this, "o_overflow", "1", "overflow flag"),
    // registers
    delay(this, "delay", "16"),
    shift(this, "shift", "7"),
    accum_ena(this, "accum_ena", "1"),
    b(this, "b", "56"),
    sum(this, "sum", "106"),
    overflow(this, "overflow", "1"),
    // process
    comb(this)
{
    Operation::start(&comb);
    proc_comb();
}

void imul53::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;

TEXT();
    SETARRITEM(comb.vb_mux, 0, ALLZEROS());
    SETARRITEM(comb.vb_mux, 1, i_a);                            // 1*a
    SETARRITEM(comb.vb_mux, 2, CC2(i_a, CONST("0", 1)));        // 2*a
    SETARRITEM(comb.vb_mux, 3, ADD2(ARRITEM(comb.vb_mux, 2), ARRITEM(comb.vb_mux, 1)));          // 2*a + 1*a
    SETARRITEM(comb.vb_mux, 4, i_a.read().to_uint64() << 2;    // 4*a
    SETARRITEM(comb.vb_mux, 5, vb_mux[4] + vb_mux[1];          // 4*a + 1*a
    SETARRITEM(comb.vb_mux, 6, vb_mux[4] + vb_mux[2];          // 4*a + 2*a
    SETARRITEM(comb.vb_mux, 8, i_a.read().to_uint64() << 3;    // 8*a
    SETARRITEM(comb.vb_mux, 7, vb_mux[8] - vb_mux[1];          // 8*a - 1*a
    SETARRITEM(comb.vb_mux, 9, vb_mux[8] + vb_mux[1];          // 8*a + 1*a
    SETARRITEM(comb.vb_mux, 10, vb_mux[8] + vb_mux[2];         // 8*a + 2*a
    SETARRITEM(comb.vb_mux, 11, vb_mux[10] + vb_mux[1];        // (8*a + 2*a) + 1*a
    SETARRITEM(comb.vb_mux, 12, vb_mux[8] + vb_mux[4];         // 8*a + 4*a
    SETARRITEM(comb.vb_mux, 16, (i_a.read().to_uint64() << 4); // unused
    SETARRITEM(comb.vb_mux, 13, vb_mux[16] - vb_mux[3];        // 16*a - (2*a + 1*a)
    SETARRITEM(comb.vb_mux, 14, vb_mux[16] - vb_mux[2];        // 16*a - 2*a
    SETARRITEM(comb.vb_mux, 15, vb_mux[16] - vb_mux[1];        // 16*a - 1*a

TEXT();
    SETVAL(comb.v_ena, i_ena);
    SETVAL(delay, CC2(BITS(delay, 14, 0), comb.v_ena));


TEXT();
    SYNC_RESET(*this);

TEXT();
}

