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

#include "int_div.h"

IntDiv::IntDiv(GenObject *parent, const char *name) :
    ModuleObject(parent, "IntDiv", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_ena(this, "i_ena", "1", "Enable bit"),
    i_unsigned(this, "i_unsigned", "1", "Unsigned operands"),
    i_rv32(this, "i_rv32", "1", "32-bits operands enabled"),
    i_residual(this, "i_residual", "1", "Compute: 0 =division; 1=residual"),
    i_a1(this, "i_a1", "RISCV_ARCH", "Operand 1"),
    i_a2(this, "i_a2", "RISCV_ARCH", "Operand 2"),
    o_res(this, "o_res", "RISCV_ARCH", "Result"),
    o_valid(this, "o_valid", "1", "Result is valid"),
    // registers
    rv32(this, "rv32", "1"),
    resid(this, "resid", "1"),
    invert(this, "invert", "1"),
    div_on_zero(this, "div_on_zero", "1"),
    overflow(this, "overflow", "1"),
    busy(this, "busy", "1"),
    ena(this, "ena", "10"),
    divident_i(this, "divident_i", "64"),
    divisor_i(this, "divisor_i", "120"),
    bits_i(this, "bits_i", "64"),
    result(this, "result", "RISCV_ARCH"),
    reference_div(this, "reference_div", "RISCV_ARCH"),
    a1_dbg(this, "a1_dbg", "64", "0", "Store this value for output in a case of error"),
    a2_dbg(this, "a2_dbg", "64"),
    // process
    comb(this)
{
    Operation::start(&comb);
    proc_comb();
}

void IntDiv::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;


TEXT();
    SYNC_RESET(*this);

TEXT();
}

