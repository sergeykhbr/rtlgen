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

#include "signals.h"
#include "utils.h"
#include <map>

namespace sysvc {

static GenObject* getModuleInput(GenObject *p, const char *portname) {
    while (!p->isModule()) {
        p = p->getParent();
    }
    for (auto &i : p->getEntries()) {
        if (i->isInput() && i->getName() == portname) {
            return i;
        }
    }
    return 0;
}

Signal::Signal(GenObject *parent,
                     GenObject *clk,
                     EClockEdge edge,
                     GenObject *nrst,
                     EResetActive active,
                     const char *name,
                     const char *width,
                     const char *rstval,
                     const char *comment)
    : Logic(parent, clk, edge, nrst, active,
            name, width, rstval, comment) {
    // Assign default clock ports: i_clk and i_nrst
    if (clk == 0 && edge != CLK_ALWAYS) {
        objClock_ = getModuleInput(parent, "i_clk");
        if (!objClock_) {
            SHOW_ERROR("Register '%s.%s' does not have clock",
                    parent->getName().c_str(), 
                    getName().c_str());
        }
    }

    if (nrst == 0 && active != ACTIVE_NONE) {
        objReset_ = getModuleInput(parent, "i_nrst");
        if (!objReset_) {
            SHOW_ERROR("Register '%s.%s' does not have reset",
                    parent->getName().c_str(),
                    getName().c_str());
        }
    }
}

Signal1::Signal1(GenObject *parent,
                       GenObject *clk,
                       EClockEdge edge,
                       GenObject *nrst,
                       EResetActive active,
                       const char *name,
                       const char *width,
                       const char *rstval,
                       const char *comment)
    : Logic1(parent, clk, edge, nrst, active,
            name, width, rstval, comment) {
    // Assign default clock ports: i_clk and i_nrst
    if (clk == 0 && edge != CLK_ALWAYS) {
        objClock_ = getModuleInput(parent, "i_clk");
        if (!objClock_) {
            SHOW_ERROR("Register '%s.%s' does not have clock",
                    parent->getName().c_str(), 
                    getName().c_str());
        }
    }

    if (nrst == 0 && active != ACTIVE_NONE) {
        objReset_ = getModuleInput(parent, "i_nrst");
        if (!objReset_) {
            SHOW_ERROR("Register '%s.%s' does not have reset",
                    parent->getName().c_str(),
                    getName().c_str());
        }
    }
}


static std::string copyreg(GenObject *r,
                           const char *i,
                           const char *dst_prefix,
                           const char *optype,
                           const char *src_prefix) {
    std::string ret;
    std::string dst = dst_prefix;
    std::string src = src_prefix;
    std::string idx = "";
    if (dst.size()) {
        dst += ".";
    }
    if (src.size()) {
        src += ".";
    }
    if (SCV_is_sysc() || SCV_is_sv()) {
        if (r->getDepth()) {
            ret += addspaces() + "for (int " + i + " = 0; " + i + " < ";
            ret += r->getStrDepth() + "; " + i + "++) ";
            if (SCV_is_sysc()) {
                ret += "{\n";
            } else {
                ret += "begin\n";
            }
            idx = "[" + std::string(i) + "]";
            pushspaces();
        }
        ret += addspaces() + dst + r->getName() + idx + " " + optype + " ";

        if (src.size()) {
            ret += src + r->getName() + idx + ";\n";
        } else {
            ret += r->getStrValue() + ";\n";
        }

        if (r->getDepth()) {
            popspaces();
            if (SCV_is_sysc()) {
                ret += addspaces() + "}\n";
            } else {
                ret += addspaces() + "end\n";
            }
        }
    }
    return ret;
}

}
