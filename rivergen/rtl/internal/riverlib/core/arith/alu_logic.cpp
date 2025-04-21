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

#include "alu_logic.h"

AluLogic::AluLogic(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "AluLogic", name, comment),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_mode(this, "i_mode", "3", "operation type: [0]AND;[1]=OR;[2]XOR"),
    i_a1(this, "i_a1", "RISCV_ARCH", "Operand 1"),
    i_a2(this, "i_a2", "RISCV_ARCH", "Operand 2"),
    o_res(this, "o_res", "RISCV_ARCH", "Result"),
    // registers
    res(this, "res", "RISCV_ARCH", "'0", NO_COMMENT),
    // process
    comb(this)
{
    Operation::start(&comb);
    proc_comb();
}

void AluLogic::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;

    IF (NZ(BIT(i_mode, 1)));
        SETVAL(res, OR2_L(i_a1, i_a2));
    ELSIF (NZ(BIT(i_mode, 2)));
        SETVAL(res, XOR2(i_a1, i_a2));
    ELSE();
        SETVAL(res, AND2_L(i_a1, i_a2));
    ENDIF();

TEXT();
    SYNC_RESET();

TEXT();
    SETVAL(o_res, res);
}

