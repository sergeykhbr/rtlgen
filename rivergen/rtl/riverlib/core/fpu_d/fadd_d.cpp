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

#include "fadd_d.h"

DoubleAdd::DoubleAdd(GenObject *parent, const char *name) :
    ModuleObject(parent, "DoubleAdd", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_ena(this, "i_ena", "1"),
    i_add(this, "i_add", "1"),
    i_sub(this, "i_sub", "1"),
    i_eq(this, "i_eq", "1"),
    i_lt(this, "i_lt", "1"),
    i_le(this, "i_le", "1"),
    i_max(this, "i_max", "1"),
    i_min(this, "i_min", "1"),
    i_a(this, "i_a", "64", "Operand 1"),
    i_b(this, "i_b", "64", "Operand 2"),
    o_res(this, "o_res", "64", "Result"),
    o_illegal_op(this, "o_illegal_op", "1" ,"nanA | nanB"),
    o_overflow(this, "o_overflow", "1"),
    o_valid(this, "o_valid", "1", "Result is valid"),
    o_busy(this, "o_busy", "1", "Multiclock instruction under processing"),
    // registers
    busy(this, "busy", "1"),
    ena(this, "ena", "8"),
    a(this, "a", "64"),
    b(this, "b", "64"),
    result(this, "result", "64"),
    illegal_op(this, "illegal_op", "1"),
    overflow(this, "overflow", "1"),
    add(this, "add", "1"),
    sub(this, "sub", "1"),
    eq(this, "eq", "1"),
    lt(this, "lt", "1"),
    le(this, "le", "1"),
    max(this, "max", "1"),
    min(this, "min", "1"),
    flMore(this, "flMore", "1"),
    flEqual(this, "flEqual", "1"),
    flLess(this, "flLess", "1"),
    preShift(this, "preShift", "12"),
    signOpMore(this, "signOpMore", "1"),
    expMore(this, "expMore", "11"),
    mantMore(this, "mantMore", "53"),
    mantLess(this, "mantLess", "53"),
    mantLessScale(this, "mantLessScale", "105"),
    mantSum(this, "mantSum", "106"),
    lshift(this, "lshift", "7"),
    mantAlign(this, "mantAlign", "105"),
    expPostScale(this, "expPostScale", "12"),
    expPostScaleInv(this, "expPostScaleInv", "12"),
    mantPostScale(this, "mantPostScale", "105"),
    // process
    comb(this)
{
    Operation::start(&comb);
    proc_comb();
}

void DoubleAdd::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;
    GenObject *i;

    SETVAL(comb.v_ena, AND2(i_ena, INV(busy)));
    SETVAL(ena, CC2(BITS(ena, 1, 0), comb.v_ena));


TEXT();
    SYNC_RESET(*this);

}

