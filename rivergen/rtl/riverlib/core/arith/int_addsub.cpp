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

IntAddSub::IntAddSub(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "IntAddSub", name, comment),
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

    TEXT("To support 32-bits instruction transform 32-bits operands to 64 bits");
    IF (NZ(BIT(i_mode, 0)));
        SETBITS(comb.vb_rdata1, 31, 0, BITS(i_a1, 31, 0));
        SETBITS(comb.vb_rdata2, 31, 0, BITS(i_a2, 31, 0));
        IF (NZ(BIT(comb.vb_rdata1, 31)));
            SETBITS(comb.vb_rdata1, 63, 32, ALLONES());
        ENDIF();
        IF (NZ(BIT(comb.vb_rdata2, 31)));
            SETBITS(comb.vb_rdata2, 63, 32, ALLONES());
        ENDIF();
    ELSE();
        SETVAL(comb.vb_rdata1, i_a1);
        SETVAL(comb.vb_rdata2, i_a2);
    ENDIF();

TEXT();
    SETVAL(comb.vb_add, ADD2(comb.vb_rdata1, comb.vb_rdata2));
    SETVAL(comb.vb_sub, SUB2(comb.vb_rdata1, comb.vb_rdata2));
    IF (NZ(BIT(i_mode, 2)));
        SETVAL(comb.vb_res, comb.vb_add);
    ELSIF (NZ(BIT(i_mode, 3)));
        SETVAL(comb.vb_res, comb.vb_sub);
    ELSIF (NZ(BIT(i_mode, 4)));
        IF (NZ(BIT(i_mode, 1)));
            TEXT("unsigned less");
            IF (LS(comb.vb_rdata1, comb.vb_rdata2));
                SETBITONE(comb.vb_res, 0);
            ENDIF();
        ELSE();
            TEXT("signed less");
            SETBIT(comb.vb_res, 0, BIT(comb.vb_sub, 63));
        ENDIF();
    ELSIF (NZ(BIT(i_mode, 5)));
        IF (NZ(BIT(i_mode, 1)));
            TEXT("unsigned min");
            IF (LS(comb.vb_rdata1, comb.vb_rdata2));
                SETVAL(comb.vb_res, comb.vb_rdata1);
            ELSE();
                SETVAL(comb.vb_res, comb.vb_rdata2);
            ENDIF();
        ELSE();
            TEXT("signed min");
            IF (NZ(BIT(comb.vb_sub, 63)));
                SETVAL(comb.vb_res, comb.vb_rdata1);
            ELSE();
                SETVAL(comb.vb_res, comb.vb_rdata2);
            ENDIF();
        ENDIF();
    ELSIF (NZ(BIT(i_mode, 6)));
        IF (NZ(BIT(i_mode, 1)));
            TEXT("unsigned max");
            IF (LS(comb.vb_rdata1, comb.vb_rdata2));
                SETVAL(comb.vb_res, comb.vb_rdata2);
            ELSE();
                SETVAL(comb.vb_res, comb.vb_rdata1);
            ENDIF();
        ELSE();
            TEXT("signed max");
            IF (NZ(BIT(comb.vb_sub, 63)));
                SETVAL(comb.vb_res, comb.vb_rdata2);
            ELSE();
                SETVAL(comb.vb_res, comb.vb_rdata1);
            ENDIF();
        ENDIF();
    ENDIF();
    IF (NZ(BIT(i_mode, 0)));
        IF (NZ(BIT(comb.vb_res, 31)));
            SETBITS(comb.vb_res, 63, 32, ALLONES());
        ELSE();
            SETBITS(comb.vb_res, 63, 32, ALLZEROS());
        ENDIF();
    ENDIF();

TEXT();
    SETVAL(res, comb.vb_res);

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_res, res);
}

