// 
//  Copyright 2025 Sergey Khabarov, sergeykhbr@gmail.com
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

#include "proc.h"
#include "operations.h"
#include "api_rtlgen.h"
#include <cstring>

namespace sysvc {

std::string ProcObject::generate_sysc() {
    // Do no add return type, it depends of module's templates parameters
    std::string ret;

    // process variables declaration
    ret = generate_all_localvar();

    // Generate operations:
    for (auto &e: getEntries()) {
        if (e->isOperation() || e->isComment()) {
            ret += e->generate();
        }
    }
    return ret;
}

std::string RegisterCopyProcess::generate_sysc() {
    std::string ret = "";
    GenObject *m = getParent();
    GenObject *clkport = getClockPort();
    GenObject *rstport = getResetPort();
    std::string generate_name = "async_" + rstruct_->r_instance()->getName();

    if (m == 0) {
        SHOW_ERROR("Process %s parent is zero", getName());
        return ret;
    }
    if (clkport == 0) {
        SHOW_ERROR("RegisterCopyProcess %s without clock", getName());
        return ret;
    }

    GenObject *block;
    Operation::push_obj(NO_PARENT);
    GenObject *async_reset = getParent()->getChildByName("async_reset");
    if (rstport && getResetActive() != ACTIVE_NONE) {
        if (async_reset) {
            // if (async_reset && i_nrst.read() == 0/1)
            if (getResetActive() == ACTIVE_LOW) {
                block = &IF (AND2(NZ(*async_reset), EZ(*rstport)));
            } else {
                block = &IF (AND2(NZ(*async_reset), NZ(*rstport)));
            }
        } else {
            // if (i_nrst.read() == 0/1)
            if (getResetActive() == ACTIVE_LOW) {
                block = &IF (EZ(*rstport));
            } else {
                block = &IF (NZ(*rstport));
            }
        }
            CALLF(0, *rstruct_->rst_func_instance(), 1, rstruct_->r_instance());
        ELSE();
            SETVAL(*rstruct_->r_instance(), *rstruct_->v_instance());
        ENDIF();
    } else {
        block = &SETVAL(*rstruct_->r_instance(), *rstruct_->v_instance());
    }
    ret += block->generate();
    Operation::pop_obj();
    return ret;
}


}  // namespace sysvc
