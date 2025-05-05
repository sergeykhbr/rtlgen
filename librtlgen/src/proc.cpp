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
#include "api_rtlgen.h"
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
    reglist_ = 0;

    Operation::start(this);
}

void ProcObject::postInit() {
    GenObject vinitobj(NO_PARENT, "vinitobj");
    Operation::push_obj(&vinitobj);
    int tcnt = 0;

    // nullify proc local variable:
    for (auto &p: getEntries()) {
        if (!p->isValue()) {
            continue;
        }
        if (p->getObjValue()) {
            SETVAL(*p, *p->getObjValue());
            tcnt++;
        }
    }
    if (tcnt) {
        TEXT();
    }
    Operation::pop_obj();

    if (vinitobj.getEntries().size()) {
        // It is better to insert after local vars declaration
        getEntries().insert(getEntries().begin(),
                            vinitobj.getEntries().begin(),
                            vinitobj.getEntries().end());
    }
    GenObject::postInit();
}

void CombinationalProcess::postInit() {
    ProcObject::postInit();

    // rin <= v; should be added for SV and VHDL, but not for SystemC
    v2rin_ = new Operation(this, NO_COMMENT);
    Operation::push_obj(v2rin_);
    if (reglist_->size()) {
        TEXT();
    }
    for (auto &p: *reglist_) {
        SETVAL(*p->rin_instance(), *p->v_instance());
    }
    Operation::pop_obj();
}

void CombinationalProcess::configureGenerator(ECfgGenType cfg) {
    ProcObject::configureGenerator(cfg);
    // inital state
    getEntries().remove(v2rin_);

    // rin <= v;
    if (cfg == CFG_GEN_SV || cfg == CFG_GEN_VHDL) {
        getEntries().push_back(v2rin_);
    }
}

void ProcObject::setSortedRegs(std::list<RegTypedefStruct *> *reglist) {
    reglist_ = reglist;
    GenObject *vobj;

    // Add local 'v' variable into process for each register strcture
    for (auto &item : *reglist) {
        vobj = item->v_instance();
        vobj->setParent(this);
        getEntries().push_front(vobj);
    }
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

// process variables declaration
std::string ProcObject::generate_all_localvar() {
    std::string ret = "";
    std::string ln = "";

    for (auto &e: getEntries()) {
        if (!e->isValue()) {
            continue;
        }

        ln = addspaces() + e->getType() + " " + e->getName();
        if (e->getObjDepth()) {
            if (SCV_is_sysc()) {
                ln += "[" + e->getStrDepth() + "]";
            } else if (SCV_is_sv()) {
                ln += "[0: " + e->getStrDepth() + "-1]";
            } else if (SCV_is_vhdl()) {
                ln += "(0 upto " + e->getStrDepth() + " - 1)";
            }
        }
        ln += ";";
        e->addComment(ln);
        ret += ln + "\n";
    }
    if (ret.size()) {
        ret += "\n";
    }
    return ret;
}

std::string ProcObject::generate() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        return generate_sysc();
    } else if (SCV_is_sv()) {
        return generate_sv();
    }
    return ret;
}

}  // namespace sysvc
