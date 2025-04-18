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
#include <cstring>

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

std::string ProcObject::generate() {
    std::string ret = "";
    if (SCV_is_sysc_h()) {
        return generate_sysc_h();
    } else if (SCV_is_sysc()) {
        return generate_sysc_cpp();
    } else if (SCV_is_sv()) {
        GenObject *rstport = getResetPort();
        if (rstport && rstport->getResetActive() != ACTIVE_NONE) {
            ret += generate_sv(true);
            ret += generate_sv(false);
        } else {
            ret += generate_sv(false);
        }
    }
    return ret;
}

std::string ProcObject::generate_sysc_h() {
    std::string ret = "";
    return ret;
}

std::string ProcObject::generate_sysc_cpp() {
    std::string ret = "";
    return ret;
}

static const char *SV_STR_CLKEDGE[3] = {"*", "posedge", "negedge"};
static const char *SV_STR_RSTEDGE[3] = {"", "negedge", "posedge"};
static const char *SV_STR_ACTIVE[3] = {"", "1'b0", "1'b1"};

std::string ProcObject::generate_sv(bool async_on_off) {
    std::string ret = "";
    std::string bodytext;
    GenObject *m = getParent();
    if (m == 0) {
        SHOW_ERROR("Process %s parent is zero", getName());
        return ret;
    }

    if (isAssign()) {
        // All entries inside of process should be executed out-of process:
        for (auto &e: getEntries()) {
            addPostAssign(e);
        }
    } else {
        pushspaces();
        for (auto &e: getEntries()) {
            if (e->isOperation()) {
                if (e->isAssign()) {
                    addPostAssign(e);
                } else {
                    bodytext += e->generate();
                }
            }
        }
        popspaces();
    }

    if (bodytext.size()) {
        // Proc is not empty
        GenObject *clkport = getClockPort();
        GenObject *rstport = getResetPort();
        // proc prolog:
        if (clkport) {
            ret += addspaces() + "always_ff @(";
            ret += SV_STR_CLKEDGE[clkport->getClockEdge()];
            ret += std::string(" ") + clkport->getClockPort()->getName();

            if (async_on_off && rstport->getResetActive() != ACTIVE_NONE) {
                ret += std::string(", ") + SV_STR_RSTEDGE[rstport->getResetActive()] + " ";
                ret += rstport->getName();
            }

            ret += ") ";
        } else {
            ret += "always_comb\n" + addspaces();
        }
        ret += "begin: " + getName() + "_proc\n";

        ret += bodytext;

        // proc epilog
        ret += addspaces() + "end: " + getName() + "_proc\n";
    }
    // Post assinment stage:
    ret += getPostAssign();

    if (ret.size()) {
        ret += "\n";
    }
    return ret;
}

/**
    When async_on_off = true:
        always_ff @(posedge clk, negedge nrst) begin: name_proc
            if (nrst == 1'b0) begin
                r <= ModuleType_r_reset;
            end else begin
                r <= rin;
            end
        end: name_proc

    When async_on_off = false:
        always_ff @(posedge clk) begin: name_proc
            r <= rin;
        end: name_proc
*/
std::string RegisterCopyProcess::generate_sv(bool async_on_off) {
    std::string ret = "";
    GenObject *m = getParent();
    GenObject *clkport = getClockPort();
    GenObject *rstport = getResetPort();
    std::string r;
    char i_idx[2] = {0};

    if (m == 0) {
        SHOW_ERROR("Process %s parent is zero", getName());
        return ret;
    }
    if (clkport == 0) {
        SHOW_ERROR("RegisterCopyProcess %s without clock", getName());
        return ret;
    }
    
    // Collect parent module registers with the same clock
    std::list<GenObject *> reglist;
    bool is2dm = false;
    for (auto &p : m->getEntries()) {
        GenObject *regclk = p->getClockPort();
        if (!regclk || regclk->getClockEdge() == CLK_ALWAYS) {
            continue;
        }
        if (p->isProcess()) {
            continue;
        }
        if (clkport->getName() != regclk->getName()) {
            continue;
        }

        r = p->r_prefix();
        reglist.push_back(p);
        is2dm |= p->is2Dim();
    }

    if (reglist.size() == 0) {
        SHOW_ERROR("RegisterCopyProcess %s registers not found", getName());
        return ret;
    }

    // process prolog:
    ret += addspaces() + "always_ff @(";
    ret += SV_STR_CLKEDGE[clkport->getClockEdge()];
    ret += std::string(" ") + clkport->getClockPort()->getName();

    if (async_on_off && rstport->getResetActive() != ACTIVE_NONE) {
        ret += std::string(", ") + SV_STR_RSTEDGE[rstport->getResetActive()] + " ";
        ret += rstport->getName();
    }
    ret += ") begin: " + getName() + "_proc\n";
    pushspaces();

    // With ASYNC reset
    if (async_on_off && getResetActive() != ACTIVE_NONE) {
        ret += addspaces() + "if (" + rstport->getName() + " == ";
        ret += std::string(SV_STR_ACTIVE[rstport->getResetActive()]) + ") begin\n";
        pushspaces();
                
        // r <= reset
        if (!is2dm) {
            ret += addspaces() + r + " <= " + getType() + "_" + r + "_reset;\n";
        } else {
            for (auto &p : reglist) {
                i_idx[0] = 'i';
                ret += p->getCopyValue(i_idx, r.c_str(), "<=", "");
            }
        }

        popspaces();
        ret += addspaces() + "end else begin\n";
        pushspaces();
    }

    // No reset just r <= rin only if comb process exists in the module:
    if (!is2dm) {
        ret += addspaces() + r + " <= " + r + "in;\n";
    } else {
        for (auto &p : reglist) {
            i_idx[0] = 'i';
            ret += p->getCopyValue(i_idx, r.c_str(), "<=", (r + "in").c_str());
        }
    }

    if (async_on_off && rstport->getResetActive() != ACTIVE_NONE) {
        popspaces();
        ret += addspaces() + "end\n";
    }


    // process epilog
    popspaces();
    ret += addspaces() + "end: " + getName() + "_proc\n";
    ret += "\n";

    return ret;
}

}
