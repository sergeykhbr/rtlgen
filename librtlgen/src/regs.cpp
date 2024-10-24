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

#include "regs.h"
#include "utils.h"
#include "modules.h"

namespace sysvc {

RegCommon::RegCommon(GenObject *parent, Logic *clk, ERegClockEdge edge,
    Logic *rstn, ERegResetActive active)
    : regclk_(clk), edge_(edge), regrstn_(rstn), active_(active) {
    GenObject *p = parent;
    while (!p->isModule()) {
        p = p->getParent();
    }
    dynamic_cast<ModuleObject *>(p)->registerRegister(parent);

    if (regclk_ == 0) {
        for (auto &i : p->getEntries()) {
            if (i->isInput() && i->getName() == "i_clk") {
                regclk_ = i;
                break;
            }
        }
        if (!regclk_) {
            printf("Register '%s.%s' does not have clock\n",
                    p->getName().c_str(), 
                    parent->getName().c_str());
        }
    }

    if (regrstn_ == 0 && active != REG_RESET_NONE) {
        for (auto &i : p->getEntries()) {
            if (i->isInput() && i->getName() == "i_nrst") {
                regrstn_ = i;
                break;
            }
        }
        if (!regrstn_) {
            printf("Register '%s.%s' does not have reset\n",
                    p->getName().c_str(),
                    parent->getName().c_str());
        }
    }
}

}
