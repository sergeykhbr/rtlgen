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

#include "sdctrl_wdog.h"

sdctrl_wdog::sdctrl_wdog(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "sdctrl_wdog", name, comment),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_ena(this, "i_ena", "1"),
    i_period(this, "i_period", "16"),
    o_trigger(this, "o_trigger", "1"),
    // parameters
    // signals
    // registers
    cnt(this, "cnt", "16"),
    trigger(this, "trigger", "1"),
    //
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void sdctrl_wdog::proc_comb() {
    SETZERO(trigger);
    IF (EZ(i_ena));
        SETVAL(cnt, i_period);
    ELSIF(NZ(cnt));
        SETVAL(cnt, DEC(cnt));
    ELSE();
        SETONE(trigger);
    ENDIF();

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_trigger, trigger);
}
