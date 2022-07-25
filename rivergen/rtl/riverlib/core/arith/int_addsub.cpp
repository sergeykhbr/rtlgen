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

#include "int_addsub.h"

IntAddSub::IntAddSub(GenObject *parent, const char *name) :
    ModuleObject(parent, "IntAddSub", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_mode(this, "i_mode", "7", "[0]0=rv64;1=rv32;[1]0=sign;1=unsign[2]Add[3]Sub[4]less[5]min[6]max"),
    i_a1(this, "i_a1", "RISCV_ARCH", "Operand 1"),
    i_a2(this, "i_a2", "RISCV_ARCH", "Operand 2"),
    o_res(this, "o_res", "RISCV_ARCH", "Result"),
    // registers
    res(this, "res", "RISCV_ARCH"),
    // process
    comb(this)
{
    Operation::start(&comb);
    proc_comb();
}

void IntAddSub::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;


TEXT();
    SYNC_RESET(*this);

TEXT();
}

