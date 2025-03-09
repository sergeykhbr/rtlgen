// 
//  Copyright 2023 Sergey Khabarov, sergeykhbr@gmail.com
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

#include "SysPLL_tech.h"

SysPLL_tech::SysPLL_tech(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "SysPLL_tech", name, comment),
    i_reset(this, "i_reset", "1", "Active HIGH"),
    i_clk_tcxo(this, "i_clk_tcxo", "1"),
    o_clk_sys(this, "o_clk_sys", "1", "Bus clock 40 MHz"),
    o_clk_ddr(this, "o_clk_ddr", "1", "DDR clock 200 MHz"),
    o_clk_pcie(this, "o_clk_pcie", "1", "PCIE PHY 100 MHz"),
    o_locked(this, "o_locked", "1"),
    // process
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void SysPLL_tech::proc_comb() {
    SETVAL(o_clk_sys, i_clk_tcxo);
    SETVAL(o_clk_ddr, i_clk_tcxo);
    SETVAL(o_clk_pcie, i_clk_tcxo);
    SETONE(o_locked);
}

