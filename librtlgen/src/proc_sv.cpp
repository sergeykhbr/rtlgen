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

static const char *SV_STR_CLKEDGE[3] = {"*", "posedge", "negedge"};
static const char *SV_STR_RSTEDGE[3] = {"", "negedge", "posedge"};
static const char *SV_STR_ACTIVE[3] = {"", "1'b0", "1'b1"};

std::string ProcObject::generate_sv() {
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
        bodytext += generate_all_localvar();

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
        ret += addspaces();
        if (clkport) {
            ret += "\nalways_ff @(";
            ret += SV_STR_CLKEDGE[getClockEdge()];
            ret += std::string(" ") + clkport->getName();

            if (rstport && getResetActive() != ACTIVE_NONE) {
                ret += std::string(", ") + SV_STR_RSTEDGE[getResetActive()] + " ";
                ret += rstport->getName();
            }

            ret += ") ";
        } else {
            ret += "\nalways_comb\n" + addspaces();
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
std::string RegisterCopyProcess::generate_sv() {
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
    Operation::push_obj(NO_PARENT);

    GenObject *async_reset = getParent()->getChildByName("async_reset");
    if (rstport && getResetActive() != ACTIVE_NONE && async_reset) {
        // Generate async_reset/!async_reset blocks:
        StringConst *pNameEn = new StringConst((generate_name + "_en").c_str());
        StringConst *pName = new StringConst(generate_name.c_str());
        StringConst *pNameDis = new StringConst((generate_name + "_dis").c_str());
        GenObject &block = 
        GENERATE("");
        IFGEN(*async_reset, pNameEn);
            TEXT();
            ALWAYS_FF(EDGE(*clkport, getClockEdge()), EDGE(*rstport, getResetActive()));
                if (getResetActive() == ACTIVE_LOW) {
                    IF (EZ(*rstport));
                } else {
                    IF (NZ(*rstport));
                }
                    SETVAL_NB(*rstruct_->r_instance(), *rstruct_->rst_instance());
                ELSE();
                    SETVAL_NB(*rstruct_->r_instance(), *rstruct_->rin_instance());
                ENDIF();
            ENDALWAYS();
            TEXT();
        ELSEGEN(pName);
            TEXT();
            ALWAYS_FF(EDGE(*clkport, getClockEdge()));
                SETVAL_NB(*rstruct_->r_instance(), *rstruct_->rin_instance());
            ENDALWAYS();
            TEXT();
        ENDIFGEN(pNameDis);
        ENDGENERATE("");
        ret += block.generate() + "\n";
    } else if (rstport && getResetActive() != ACTIVE_NONE) {
        GenObject &block = 
        ALWAYS_FF(EDGE(*clkport, getClockEdge()), EDGE(*rstport, getResetActive()));
            if (getResetActive() == ACTIVE_LOW) {
                IF (EZ(*rstport));
            } else {
                IF (NZ(*rstport));
            }
                SETVAL_NB(*rstruct_->r_instance(), *rstruct_->rst_instance());
            ELSE();
                SETVAL_NB(*rstruct_->r_instance(), *rstruct_->rin_instance());
            ENDIF();
        ENDALWAYS();
        ret += block.generate() + "\n";
    } else {
        GenObject &block = 
        ALWAYS_FF(EDGE(*clkport, getClockEdge()));
            SETVAL_NB(*rstruct_->r_instance(), *rstruct_->rin_instance());
        ENDALWAYS();
        ret += block.generate() + "\n";
    }

    Operation::pop_obj();
    return ret;
}

}  // namespace sysvc
