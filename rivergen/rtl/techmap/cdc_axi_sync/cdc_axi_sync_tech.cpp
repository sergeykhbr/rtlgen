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

#include "cdc_axi_sync_tech.h"

cdc_axi_sync_tech::cdc_axi_sync_tech(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "cdc_axi_sync_tech", name, comment),
    i_xslv_clk(this, "i_xslv_clk", "1", "system clock"),
    i_xslv_nrst(this, "i_xslv_nrst", "1", "system reset"),
    i_xslvi(this, "i_xslvi", "system clock"),
    o_xslvo(this, "o_xslvo", "system clock"),
    i_xmst_clk(this, "i_xmst_clk", "1", "ddr clock"),
    i_xmst_nrst(this, "i_xmst_nrst", "1", "ddr reset"),
    o_xmsto(this, "o_xmsto", "ddr clock"),
    i_xmsti(this, "i_xmsti", "ddr clock"),
    // process
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void cdc_axi_sync_tech::proc_comb() {
    SETVAL(o_xmsto, i_xslvi);
    SETVAL(o_xslvo, i_xmsti);
}

