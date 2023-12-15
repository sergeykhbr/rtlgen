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

#include "pma.h"

PMA::PMA(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "PMA", name, comment),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_iaddr(this, "i_iaddr", "CFG_CPU_ADDR_BITS"),
    i_daddr(this, "i_daddr", "CFG_CPU_ADDR_BITS"),
    o_icached(this, "o_icached", "1", "Hardcoded cached memory range for I$"),
    o_dcached(this, "o_dcached", "1", "Hardcoded cached memory range for D$"),
    // params
    CLINT_BAR(this, "CLINT_BAR", "CFG_CPU_ADDR_BITS",   "0x0000000002000000", NO_COMMENT),
    CLINT_MASK(this, "CLINT_MASK", "CFG_CPU_ADDR_BITS", "0x000000000000FFFF", "64 KB"),
    PLIC_BAR(this, "PLIC_BAR", "CFG_CPU_ADDR_BITS",   "0x000000000C000000", NO_COMMENT),
    PLIC_MASK(this, "PLIC_MASK", "CFG_CPU_ADDR_BITS", "0x0000000003FFFFFF", "64 MB"),
    IO1_BAR(this, "IO1_BAR", "CFG_CPU_ADDR_BITS",   "0x0000000010000000", NO_COMMENT),
    IO1_MASK(this, "IO1_MASK", "CFG_CPU_ADDR_BITS", "0x00000000000FFFFF", "1 MB"),
    // registers
    // process
    comb(this)
{
    Operation::start(&comb);
    proc_comb();
}

void PMA::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;

    SETONE(comb.v_icached);
    IF (EQ(AND2_L(i_iaddr, INV_L(CLINT_MASK)), CLINT_BAR));
        SETZERO(comb.v_icached);
    ELSIF (EQ(AND2_L(i_iaddr, INV_L(PLIC_MASK)), PLIC_BAR));
        SETZERO(comb.v_icached);
    ELSIF (EQ(AND2_L(i_iaddr, INV_L(IO1_MASK)), IO1_BAR));
        SETZERO(comb.v_icached);
    ENDIF();

TEXT();
    SETONE(comb.v_dcached);
    IF (EQ(AND2_L(i_daddr, INV_L(CLINT_MASK)), CLINT_BAR));
        SETZERO(comb.v_dcached);
    ELSIF (EQ(AND2_L(i_daddr, INV_L(PLIC_MASK)), PLIC_BAR));
        SETZERO(comb.v_dcached);
    ELSIF (EQ(AND2_L(i_daddr, INV_L(IO1_MASK)), IO1_BAR));
        SETZERO(comb.v_dcached);
    ENDIF();

    TEXT();

TEXT();
    SETVAL(o_icached, comb.v_icached);
    SETVAL(o_dcached, comb.v_dcached);
}
