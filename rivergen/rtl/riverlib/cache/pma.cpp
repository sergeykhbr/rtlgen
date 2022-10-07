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

PMA::PMA(GenObject *parent, const char *name) :
    ModuleObject(parent, "PMA", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_iaddr(this, "i_iaddr", "CFG_CPU_ADDR_BITS"),
    i_daddr(this, "i_daddr", "CFG_CPU_ADDR_BITS"),
    o_icached(this, "o_icached", "1", "Hardcoded cached memory range for I$"),
    o_dcached(this, "o_dcached", "1", "Hardcoded cached memory range for D$"),
    // params
    CLINT_BAR(this, "CFG_CPU_ADDR_BITS", "CLINT_BAR",   "0x0000000002000000"),
    CLINT_MASK(this, "CFG_CPU_ADDR_BITS", "CLINT_MASK", "0x0000000000007FFF", "32 KB"),
    PLIC_BAR(this, "CFG_CPU_ADDR_BITS", "PLIC_BAR",   "0x000000000C000000"),
    PLIC_MASK(this, "CFG_CPU_ADDR_BITS", "PLIC_MASK", "0x000000000000FFFF", "64 KB"),
    IO1_BAR(this, "CFG_CPU_ADDR_BITS", "IO1_BAR",   "0x0000000010000000"),
    IO1_MASK(this, "CFG_CPU_ADDR_BITS", "IO1_MASK", "0x000000000003FFFF", "256 KB"),
    // registers
    icached(this, "icached", "1"),
    dcached(this, "dcached", "1"),
    // process
    comb(this)
{
    Operation::start(&comb);
    proc_comb();
}

void PMA::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;

    SETONE(comb.v_icached);
    IF (EQ(OR2_L(i_iaddr, CLINT_MASK), CLINT_BAR));
        SETZERO(comb.v_icached);
    ELSIF (EQ(OR2_L(i_iaddr, PLIC_MASK), PLIC_BAR));
        SETZERO(comb.v_icached);
    ELSIF (EQ(OR2_L(i_iaddr, IO1_MASK), IO1_BAR));
        SETZERO(comb.v_icached);
    ENDIF();

TEXT();
    SETONE(dcached);
    IF (EQ(OR2_L(i_iaddr, CLINT_MASK), CLINT_BAR));
        SETZERO(comb.v_dcached);
    ELSIF (EQ(OR2_L(i_iaddr, PLIC_MASK), PLIC_BAR));
        SETZERO(comb.v_dcached);
    ELSIF (EQ(OR2_L(i_iaddr, IO1_MASK), IO1_BAR));
        SETZERO(comb.v_dcached);
    ENDIF();

    TEXT();
    SETVAL(icached, comb.v_icached);
    SETVAL(dcached, comb.v_dcached);

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_icached, icached);
    SETVAL(o_dcached, dcached);
}
