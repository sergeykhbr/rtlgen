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

#include "sdctrl_err.h"

sdctrl_err::sdctrl_err(GenObject *parent, const char *name) :
    ModuleObject(parent, "sdctrl_err", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_err_valid(this, "i_err_valid", "1"),
    i_err_code(this, "i_err_code", "4"),
    i_err_clear(this, "i_err_clear", "1"),
    o_err_code(this, "o_err_code", "4"),
    o_err_pending(this, "o_err_pending", "1"),
    // parameters
    // signals
    // registers
    code(this, "code", "4", "CMDERR_NONE"),
    //
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void sdctrl_err::proc_comb() {
    IF (NZ(i_err_clear));
        SETVAL(code, sdctrl_cfg_->CMDERR_NONE);
    ELSIF (ANDx(2, &NZ(i_err_valid),
                   &EQ(code, sdctrl_cfg_->CMDERR_NONE)));
        SETVAL(code, i_err_code);
    ENDIF();

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_err_code, code);
    SETVAL(o_err_pending, OR_REDUCE(code));
}
