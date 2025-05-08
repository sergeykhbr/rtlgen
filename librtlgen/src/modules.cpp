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

#include "modules.h"
#include "defparams.h"
#include "ports.h"
#include "signals.h"
#include "utils.h"
#include "files.h"
#include "operations.h"
#include "proc.h"
#include <string.h>
#include <algorithm>

namespace sysvc {

ModuleObject::ModuleObject(GenObject *parent,
                           const char *type,
                           const char *name,
                           const char *comment)
    : GenObject(parent, comment) {
    name_ = std::string(name);
    type_ = std::string(type);
    if (name_ == "") {
        SHOW_ERROR("%s", "Unnamed module");
    }
    loopidx_[0] = 'i';
    loopidx_[1] = '\0';

    SCV_add_module(this);
}

void ModuleObject::registerModuleReg(GenObject *r) {
    int same_clkrst_cnt = 1;        // to generate unique structure name
    int add_reg_idx = 0;
    for (auto &p : sorted_regs_) {
        if (r->getClockEdge() == p->getClockEdge()
            && r->getResetActive() == p->getResetActive()) {
            add_reg_idx = 1;
        }
        if (p->getClockPort()->getName() != r->getClockPort()->getName()) {
            continue;
        }
        if (p->getClockEdge() != r->getClockEdge()) {
            continue;
        }
        if (p->getResetActive() != r->getResetActive()) {
            continue;
        }
        if (p->getResetActive() != ACTIVE_NONE) {
            if (p->getResetPort()->getName()
                    != r->getResetPort()->getName()) {
                continue;
            }
        }

        // Check that this name already registered (for structs):
        for (auto &check : p->getEntries()) {
            if (check->getName() == r->getName()) {
                if (!r->isStruct()) {
                    SHOW_ERROR("Reg already registered %s",
                                r->getName().c_str());
                }
                continue;
            }
        }

        r->setParent(p);
        p->add_entry(r);
        getEntries().remove(r);
        return;
    }
    same_clkrst_cnt += add_reg_idx;
    
    std::string r_suffix = RegTypedefStruct::reg_suffix(r, same_clkrst_cnt);
    std::string procname = "r" + r_suffix + "egisters";
    std::string rstruct_type = getName() + "_" + procname;
    std::string rstruct_rst = getName() + "_r" + r_suffix + "_reset";

    // New register typedef structure:
    RegTypedefStruct *pnew = new RegTypedefStruct(this,
                                          r->getClockPort(),
                                          r->getClockEdge(),
                                          r->getResetPort(),
                                          r->getResetActive(),
                                          r_suffix.c_str(),
                                          rstruct_type.c_str(),     // type
                                          rstruct_rst.c_str());     // rstval
    sorted_regs_.push_back(pnew);

    r->setParent(pnew);
    pnew->add_entry(r);
    getEntries().remove(r);
}

void ModuleObject::postInit() {
    GenObject * clkport;
    GenObject * rstport;
    GenObject *pAsyncReset = 0;

    SCV_set_local_module(this);

    pAsyncReset = getChildByName("async_reset");
    if (isAsyncResetParam() && pAsyncReset == 0) {
        pAsyncReset = 
            new DefParamLogic(NO_PARENT, "async_reset", "1", "0", NO_COMMENT);
        // Push front to provide less differences:
        pAsyncReset->setParent(this);
        getEntries().push_front(pAsyncReset);
    }

    // Sorting register and create v, rin, r structures
    std::list<GenObject *> tentries = getEntries();
    for (auto &p: tentries) {
        clkport = p->getClockPort();
        rstport = p->getResetPort();
        if (!clkport || p->getClockEdge() == CLK_ALWAYS) {
            continue;
        }
        if (p->isProcess()) {
            continue;
        }
        registerModuleReg(p);
    }

    // Bind generated structures and comb processes:
    std::list<GenObject *> proclist;
    getCombProcess(proclist);
    for (auto &p : proclist) {
        dynamic_cast<ProcObject *>(p)->setSortedRegs(&sorted_regs_);
    }

    GenObject::postInit();

    // Connect async_reset to sub-modules:
    GenObject *pParam;
    for (auto &m : getEntries()) {
        if (!m->isModule()) {
            continue;
        }
        pParam = m->getChildByName("async_reset");
        if (pParam && pAsyncReset) {
            pParam->setObjValue(pAsyncReset);
        } else {
            // USe default CFG_ASYNC_RESET value
        }
    }
}

/**
    SystemC doesn't contain rin signal
            r_reset implemented as a function
    SV and VHDL contain rin
            r_reset is a structure
 */
void ModuleObject::configureGenerator(ECfgGenType cfg) {
    for (auto &r : sorted_regs_) {
        // Initial state
        getEntries().remove(r->rin_instance());
        getEntries().remove(r->v_instance());
        for (auto &p : getEntries()) {
            if (p->isProcess() && p->getClockEdge() == CLK_ALWAYS) {
                p->getEntries().remove(r->v_instance());
                break;
            }
        }

        if (cfg == CFG_GEN_SYSC) {
            // set v as a module variable
            r->v_instance()->setParent(this);
            // insert v just before r variable
            auto lt = getEntries().begin();
            for (auto &p : getEntries()) {
                if (p == r->r_instance()) {
                    getEntries().insert(lt, r->v_instance());
                    break;
                }
                lt++;
            }
        } else if (cfg == CFG_GEN_SV || cfg == CFG_GEN_VHDL) {
            // insert rin after r for better looking code:
            auto lt = getEntries().begin();
            for (auto &p : getEntries()) {
                lt++;
                if (p == r->r_instance()) {
                    getEntries().insert(lt, r->rin_instance());
                    break;
                }
            }
            // add v to comb process variable
            for (auto &p : getEntries()) {
                if (p->isProcess() && p->getClockEdge() == CLK_ALWAYS) {
                    p->getEntries().push_front(r->v_instance());
                    r->v_instance()->setParent(p);
                    break;
                }
            }
        }
    }
    GenObject::configureGenerator(cfg);
}

std::string ModuleObject::generate() {
    std::string ret = "";
    if (SCV_is_sysc_h()) {
        return generate_sysc_h();
    } else if (SCV_is_sysc()) {
        return generate_sysc_cpp();
    }
    return ret;
}

bool ModuleObject::isAsyncResetParam() {
    for (auto &p: getEntries()) {
        if (p->getClockEdge() != CLK_ALWAYS
            && p->getResetActive() != ACTIVE_NONE) {
            return true;
        }
    }
    return GenObject::isAsyncResetParam();
}


void ModuleObject::getCombProcess(std::list<GenObject *> &proclist) {
    for (auto &p: getEntries()) {
        if (p->isProcess() && p->getClockEdge() == CLK_ALWAYS) {
            proclist.push_back(p);
        }
    }
}

bool ModuleObject::isSubModules() {
    for (auto &p: entries_) {
        if (p->isModule()) {
            return true;
        }
    }
    return false;
}

void ModuleObject::getTmplParamList(std::list<GenObject *> &genlist) {
    for (auto &e : entries_) {
        if (e->isParamTemplate()) {
            genlist.push_back(e);
        }
    }
}

void ModuleObject::getParamList(std::list<GenObject *> &genlist) {
    for (auto &e : entries_) {
        if (e->isParamGeneric() && !e->isParamTemplate()) {
            genlist.push_back(e);
        }
    }
}

void ModuleObject::getIoList(std::list<GenObject *> &iolist) {
    for (auto &e : entries_) {
        if (e->isInput() || e->isOutput()) {
            iolist.push_back(e);
        }
    }
}

std::string ModuleObject::generate_all_struct() {
    std::string ret = "";
    std::string comment = "";
    for (auto &p: getEntries()) {
        if (p->isComment()) {
            comment += p->generate();
            continue;
        }
        if (p->isConst() && p->is2Dim()) {
            // We can generate 2-dim reset structures but Vivado has an
            // issue to use it. So skip it here.
            comment = "";
            continue;
        }
        if (p->isStruct() && (p->isTypedef() || p->isConst())) {
            ret += comment;
            ret += p->generate();
        }
        if (SCV_is_sysc_h()) {
            // SystemC Reset function body in a same way as const structure
            if (p->isFunction() && p->isResetConst()) {
                SCV_set_generator(SYSC_ALL);
                ret += addspaces() + "void " + p->generate() + "\n";
                SCV_set_generator(SYSC_H);
            }
        }
        comment = "";
    }
    return ret;
}

std::string ModuleObject::generate_all_mod_variables() {
    std::string ret = "";
    std::string ln;
    std::string text;
    text = "";
    for (auto &p: getEntries()) {
        if (p->isComment()) {
            text += p->generate();
            continue;
        }
        // Signals and local variable (like int, string)
        if (!p->isValue()) {
            text = "";
            continue;
        }
        if (p->isInput()
            || p->isOutput()
            || p->isTypedef()
            || p->isConst()
            || p->isParam()) {
            text = "";
            continue;
        }
        if (p->getName() == "") {
            // todo: struct_def should be skipped (mark it as a typedef true). 
            SHOW_ERROR("Unnamed entry of type %s", p->getType().c_str());
            text = "";
            continue;
        }
        ret += text;
        text = "";
        ln = addspaces();
        if (SCV_is_sysc() && p->isSignal() && !p->isIgnoreSignal()) {
            ln += "sc_signal<";
        }
        ln += p->getType();
        if (SCV_is_sysc() && p->isSignal() && !p->isIgnoreSignal()) {
            ln += ">";
        }
        ln += " " + p->getName();
        if (p->getDepth() && !p->isVector()) {
            if (SCV_is_sysc()) {
                ln += "[";
            } else if (SCV_is_sv()) {
                ln += "[0: ";
            } else if (SCV_is_vhdl()) {
                ln += "(0 upto ";
            }
            ln += p->getStrDepth();
            if (SCV_is_sysc()) {
                ln += "]";
            } else if (SCV_is_sv()) {
                ln += " - 1]";
            } else if (SCV_is_vhdl()) {
                ln += " - 1)";
            }
        }
        ln += ";";
        p->addComment(ln);
        ret += ln + "\n";
    }
    if (ret.size()) {
        ret += "\n";
    }
    return ret;
}


}
