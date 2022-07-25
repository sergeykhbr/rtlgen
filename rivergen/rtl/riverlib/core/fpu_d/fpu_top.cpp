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

#include "fpu_top.h"

FpuTop::FpuTop(GenObject *parent, const char *name) :
    ModuleObject(parent, "FpuTop", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_ena(this, "i_ena", "1"),
    i_ivec(this, "i_ivec", "Instr_FPU_Total"),
    i_a(this, "i_a", "64", "Operand 1"),
    i_b(this, "i_b", "64", "Operand 2"),
    o_res(this, "o_res", "64", "Result"),
    o_ex_invalidop(this, "o_ex_invalidop", "1", "Exception: invalid operation"),
    o_ex_divbyzero(this, "o_ex_divbyzero", "1", "Exception: divide by zero"),
    o_ex_overflow(this, "o_ex_overflow", "1", "Exception: overflow"),
    o_ex_underflow(this, "o_ex_underflow", "1", "Exception: underflow"),
    o_ex_inexact(this, "o_ex_inexact", "1", "Exception: inexact"),
    o_valid(this, "o_valid", "1", "Result is valid"),
    // registers
    res(this, "res", "RISCV_ARCH"),
    // process
    comb(this)
{
    Operation::start(&comb);
    proc_comb();
}

void FpuTop::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;


TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_res, res);
}

