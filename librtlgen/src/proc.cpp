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
    GenObject tempobj(NO_PARENT, "tempobj");
    Operation::push_obj(&tempobj);
    int tcnt = 0;

    // nullify proc local variable:
    for (auto &p: getEntries()) {
        if (!p->isValue()) {
            continue;
        }
        if (p->getObjValue()) {
            if (p->getObjDepth()) {
                GenObject &i = FOR("i", CONST("0"), *p->getObjDepth(), "++");
                    SETARRITEM(*p, i, *p, *p->getObjValue());
                ENDFOR();
            } else {
                SETVAL(*p, *p->getObjValue());
            }
            tcnt++;
        }
    }
    if (tcnt) {
        TEXT();
    }
    Operation::pop_obj();

    if (tempobj.getEntries().size()) {
        // It is better to insert after local vars declaration
        getEntries().insert(getEntries().begin(),
                            tempobj.getEntries().begin(),
                            tempobj.getEntries().end());
    }
    GenObject::postInit();
}

void CombinationalProcess::postInit() {
    ProcObject::postInit();

    // rin <= v;
    Operation::push_obj(this);
    if (reglist_->size()) {
        TEXT();
    }
    for (auto &p: *reglist_) {
        SETVAL(*p->rin_instance(), *p->v_instance());
    }
    Operation::pop_obj();
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

std::string ProcObject::generate() {
    std::string ret = "";
    if (SCV_is_sysc_h()) {
        return generate_sysc_h();
    } else if (SCV_is_sysc()) {
        return generate_sysc_cpp();
    } else if (SCV_is_sv()) {
        return generate_sv(false);
    }
    return ret;
}

}  // namespace sysvc
