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

#include "proc.h"
#include "operations.h"

namespace sysvc {

ProcObject::ProcObject(GenObject *parent,
                       const char *name,
                       GenObject *clk,
                       EClockEdge edge,
                       GenObject *rst,
                       EResetActive active,
                       const char *comment)
    : GenObject(parent, comment) ,
    clk_(clk),
    edge_(edge),
    rst_(rst),
    active_(active) {
    name_ = std::string(name);

    if (clk == 0 && strstr(name, "egisters")) {
        // Proc could be defined inside of module only (parent != 0).
        if (strcmp(name, "registers")) {
            bool st = true;
        }
        for (auto &p : parent->getEntries()) {
            if (p->isInput() && p->getName() == "i_clk") {
                clk_ = p;
                edge_ = CLK_POSEDGE;
            } else if (p->isInput() && p->getName() == "i_nrst") {
                rst_ = p;
                active_ = ACTIVE_LOW;
            }
        }
    }

    Operation::start(this);
}

std::string ProcObject::getPostAssign() {
    std::string ret = "";
    if (!SCV_is_sv()) {
        return ret;
    }

    if (listPostAssign_.size()) {
        ret += "\n";
    }

    for (auto &p: listPostAssign_) {
        ret += p->generate();
    }
    return ret;
}

}
