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

#include "shift.h"

Shifter::Shifter(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "Shifter", name, comment),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_mode(this, "i_mode", "4", "operation type: [0]0=rv64;1=rv32;[1]=sll;[2]=srl;[3]=sra"),
    i_a1(this, "i_a1", "RISCV_ARCH", "Operand 1"),
    i_a2(this, "i_a2", "6", "Operand 2"),
    o_res(this, "o_res", "RISCV_ARCH", "Result"),
    // registers
    res(this, "res", "RISCV_ARCH", "'0", NO_COMMENT),
    // process
    comb(this)
{
    Operation::start(&comb);
    proc_comb();
}

void Shifter::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;
    char tstr[64];

    SETVAL(comb.v64, i_a1);
    SETVAL(comb.v32, BITS(i_a1, 31, 0));

TEXT();
    IF (NZ(BIT(i_a1, 63)));
        SETVAL(comb.msk64, ALLONES());
    ELSE();
        SETZERO(comb.msk64);
    ENDIF();
    IF (NZ(BIT(i_a1, 31)));
        SETVAL(comb.msk32, ALLONES());
    ELSE();
        SETZERO(comb.msk32);
    ENDIF();

TEXT();
    SWITCH (i_a2);
    CASE (CONST("0", 6));
        SETVAL(comb.wb_sll, comb.v64);
        SETVAL(comb.wb_srl, comb.v64);
        SETVAL(comb.wb_sra, comb.v64);
        ENDCASE();
    for (int i = 0; i < 63; i++) {
    RISCV_sprintf(tstr, sizeof(tstr), "%d", i+1);
    CASE (CONST(tstr, 6));
        SETVAL(comb.wb_sll, CC2(comb.v64, CONST("0", i+1)));
        SETVAL(comb.wb_srl, BITS(comb.v64, 63, (i+1)));
        SETVAL(comb.wb_sra, CC2(BITS(comb.msk64, 63, 63-i), BITS(comb.v64, 63, i+1)));
        ENDCASE();
    }
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    SWITCH (BITS(i_a2, 4, 0));
    CASE (CONST("0", 5));
        SETVAL(comb.wb_sllw, comb.v32);
        SETVAL(comb.wb_srlw, comb.v32);
        SETVAL(comb.wb_sraw, CC2(BITS(comb.msk32, 63, 32), comb.v32));
        ENDCASE();
    for (int i = 0; i < 31; i++) {
    RISCV_sprintf(tstr, sizeof(tstr), "%d", i+1);
    CASE (CONST(tstr, 5));
        SETVAL(comb.wb_sllw, CC2(comb.v32, CONST("0", i+1)));
        SETVAL(comb.wb_srlw, BITS(comb.v32, 31, (i+1)));
        SETVAL(comb.wb_sraw, CC2(BITS(comb.msk32, 63, 31-i), BITS(comb.v32, 31, i+1)));
        ENDCASE();
    }
    CASEDEF();
        ENDCASE();
    ENDSWITCH();


TEXT();
    IF (NZ(BIT(comb.wb_sllw, 31)));
        SETBITS(comb.wb_sllw, 63, 32, ALLONES());
    ELSE();
        SETBITS(comb.wb_sllw, 63, 32, ALLZEROS());
    ENDIF();

TEXT();
    IF (NZ(BIT(comb.wb_srlw, 31)));
        TEXT("when shift right == 0 and a1[31] = 1");
        SETBITS(comb.wb_srlw, 63, 32, ALLONES());
    ENDIF();

TEXT();
    IF (NZ(BIT(i_mode, 0)));
        IF (NZ(BIT(i_mode, 1)));
            SETVAL(res, comb.wb_sllw);
        ELSIF (NZ(BIT(i_mode, 2)));
            SETVAL(res, comb.wb_srlw);
        ELSE();
            SETVAL(res, comb.wb_sraw);
        ENDIF();
    ELSE();
        IF (NZ(BIT(i_mode, 1)));
            SETVAL(res, comb.wb_sll);
        ELSIF (NZ(BIT(i_mode, 2)));
            SETVAL(res, comb.wb_srl);
        ELSE();
            SETVAL(res, comb.wb_sra);
        ENDIF();
    ENDIF();

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_res, res);
}

