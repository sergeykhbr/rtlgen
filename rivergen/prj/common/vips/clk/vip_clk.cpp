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

#include "vip_clk.h"

vip_clk::vip_clk(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "vip_clk", name, comment),
    period(this, "period", "1.0"),
    o_clk(this, "o_clk", "1"),
    //
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void vip_clk::proc_comb() {
    ALWAYS(0, NO_COMMENT);
        SETVAL_DELAY(o_clk, CONST("0", "1"), MUL2(*new FloatConst(0.5), period));
        SETVAL_DELAY(o_clk, CONST("1", "1"), MUL2(*new FloatConst(0.5), period));
    ENDALWAYS();
}
