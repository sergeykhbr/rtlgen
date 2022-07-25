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

#include "int_mul.h"

IntMul::IntMul(GenObject *parent, const char *name) :
    ModuleObject(parent, "IntMul", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_ena(this, "i_ena", "1", "Enable bit"),
    i_unsigned(this, "i_unsigned", "1", "Unsigned operands"),
    i_hsu(this, "i_hsu", "1", "MULHSU instruction: signed * unsigned"),
    i_high(this, "i_high", "1", "High multiplied bits [127:64]"),
    i_rv32(this, "i_rv32", "1", "32-bits operands enabled"),
    i_a1(this, "i_a1", "RISCV_ARCH", "Operand 1"),
    i_a2(this, "i_a2", "RISCV_ARCH", "Operand 2"),
    o_res(this, "o_res", "RISCV_ARCH", "Result"),
    o_valid(this, "o_valid", "1", "Result is valid"),
    // registers
    busy(this, "busy", "1"),
    ena(this, "ena", "4"),
    a1(this, "a1", "RISCV_ARCH"),
    a2(this, "a2", "RISCV_ARCH"),
    unsign(this, "unsign", "1"),
    high(this, "high", "1"),
    rv32(this, "rv32", "1"),
    zero(this, "zero", "1"),
    inv(this, "inv", "1"),
    result(this, "result", "128"),
    a1_dbg(this, "a1_dbg", "RISCV_ARCH"),
    a2_dbg(this, "a2_dbg", "RISCV_ARCH"),
    reference_mul(this, "reference_mul", "RISCV_ARCH", "0", "Used for run-time comparision"),

    // process
    comb(this)
{
    Operation::start(&comb);
    proc_comb();
}

void IntMul::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;


TEXT();
    SYNC_RESET(*this);

TEXT();
}

