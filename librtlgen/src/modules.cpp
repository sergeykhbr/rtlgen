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

    SCV_add_module(this);
}

std::string reg_suffix(GenObject *p, int unique_idx) {
    std::string ret = "";
    if (unique_idx > 1) {
        char tstr[16];
        RISCV_sprintf(tstr, sizeof(tstr), "%d", unique_idx);
        ret += std::string(tstr);
    }
    if (p->getClockEdge() == CLK_NEGEDGE) {
        ret += "n";
    }
    if (p->getResetActive() == ACTIVE_NONE) {
        ret += "x";
    }
    return ret;
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
    
    std::string r_suffix = reg_suffix(r, same_clkrst_cnt);
    std::string procname = "r" + r_suffix + "egisters";
    std::string rstruct_type = getName() + "_" + procname;
    std::string rstruct_rst = "";
    if (r->getResetActive() != ACTIVE_NONE) {
        rstruct_rst = getName() + "_r" + r_suffix + "_reset";
    }

    // New register typedef structure:
    RegTypedefStruct *pnew = new RegTypedefStruct(this,
                                          r->getClockPort(),
                                          r->getClockEdge(),
                                          r->getResetPort(),
                                          r->getResetActive(),
                                          rstruct_type.c_str(),   // type
                                          rstruct_rst.c_str());         // rstval
    r->setParent(pnew);
    pnew->add_entry(r);
    getEntries().remove(r);
    sorted_regs_.push_back(pnew);

    RegResetStruct *r_rst = 0;
    RegSignalInstance *rin_inst = 0;
    RegSignalInstance *v_inst = 0;
    RegSignalInstance *r_inst = 0;

    // "*_r_reset" value:
    if (r->getResetActive() != ACTIVE_NONE) {
        r_rst = new RegResetStruct(this,
                                   pnew,
                                   rstruct_rst.c_str());
    }


    std::list<GenObject *> proclist;
    getCombProcess(proclist);

    // "r" value (have input 'v' and output 'r' ports):
    std::string r_name = "r" + r_suffix;
    r_inst = new RegSignalInstance(this,
                               pnew,
                               r_name.c_str(),
                               rstruct_rst.c_str());

    if (proclist.size()) {
        // "v" value:
        std::string v_name = "v" + r_suffix;
        v_inst = new RegSignalInstance(NO_PARENT,
                                   pnew,
                                   v_name.c_str(),
                                   r_name.c_str());

        // "rin" value:
        std::string rin_name = "r" + r_suffix + "in";
        rin_inst = new RegSignalInstance(this,
                                         pnew,
                                         rin_name.c_str(),
                                         rstruct_rst.c_str());
    }
    
    // Register flip-flop process
    new RegisterCopyProcess(this,
                            procname.c_str(),
                            pnew);

    pnew->setRegInstances(r_rst, v_inst, rin_inst, r_inst);
}

void ModuleObject::postInit() {
    GenObject * clkport;
    GenObject * rstport;
    GenObject *pAsyncReset = 0;

    SCV_set_local_module(this);

    if (isAsyncResetParam()) {
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
        pParam = m->getChildByName("async_reset");
        if (pParam) {
            pParam->setObjValue(pAsyncReset);
        }
    }
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

void ModuleObject::getSortedRegsMap(
    std::map<std::string, std::list<GenObject *>> &regmap,
    std::map<std::string, bool>  &is2dm)
{
    for (auto &p : getEntries()) {
        GenObject *clkport = p->getClockPort();
        if (!clkport || p->getClockEdge() == CLK_ALWAYS) {
            continue;
        }
        if (p->isProcess()) {
            continue;
        }
        if (p->r_prefix().size() == 0) {
            SHOW_ERROR("%s::%s r-preifx not defined",
                        getName().c_str(), p->getName().c_str());
        }
        regmap[p->r_prefix()].push_back(p);
        if (is2dm.find(p->r_prefix()) == is2dm.end()) {
            is2dm[p->r_prefix()] = false;
        }
        is2dm[p->r_prefix()] |= p->is2Dim();
    }
}

/*std::string ModuleObject::generate_all_proc_nullify(GenObject *obj,
                                                    std::string prefix,
                                                    std::string i) {
    std::string ret = "";
    if (((obj->isValue() && !obj->isConst())
        || (obj->isStruct() && !obj->isTypedef())) == 0) {
        return ret;
    }

    if (prefix.size()) {
        prefix += ".";
    }
    prefix += obj->getName();

    if (obj->getObjDepth()) {
        prefix += "[" + i + "]";
        ret += addspaces();
        ret += "for (int " + i + " = 0; " + i + " < " + obj->getStrDepth() + "; " + i + "++)";
        if (SCV_is_sysc()) {
            ret += " {";
        } else if (SCV_is_sv()) {
            ret += " begin";
        } else if (SCV_is_vhdl()) {
            ret += " begin";
        }
        ret += "\n";
        pushspaces();

        const char tidx[2] = {static_cast<char>(static_cast<int>(i.c_str()[0]) + 1), 0};
        i = std::string(tidx);
    }

    if (obj->isStruct()
        && ((SCV_is_sysc() && obj->getStrValue().c_str()[0] == '{')
            || (SCV_is_sv() && obj->getStrValue().c_str()[0] == '\''))) {
        for (auto &p : obj->getEntries()) {
            ret += generate_all_proc_nullify(p, prefix, i);
        }
    } else {
        ret += addspaces() + prefix + " = " + obj->getStrValue() + ";\n";
    }

    if (obj->getObjDepth()) {
        popspaces();
        ret += addspaces();
        if (SCV_is_sysc()) {
            ret += "}";
        } else if (SCV_is_sv()) {
            ret += "end";
        } else if (SCV_is_vhdl()) {
            ret += "end";
        }
        ret += "\n";
    }
    return ret;
}*/

/**
    v = r;      (inverse = false)
    or
    rin <= v    (inverse = true), SystemVerilog, VHDL
 */
/*std::string ModuleObject::generate_all_proc_r_to_v(bool inverse) {
    std::string ret;
    std::map<std::string, std::list<GenObject *>> regmap;
    std::map<std::string, bool> is2dm;
    std::string src;
    std::string dst;
    std::string eq = "=";
    if (SCV_is_vhdl()) {
        if (inverse) {
            eq = "<=";
        } else {
            eq = ":=";
        }
    }

    getSortedRegsMap(regmap, is2dm);
    
    // compatibility spacing:
    if (inverse && regmap.size()) {
        ret += "\n";
    }

    // v = r
    for (std::map<std::string, std::list<GenObject *>>::iterator it = regmap.begin();
        it != regmap.end(); it++) {
        if (inverse) {
            src = (*it->second.begin())->v_prefix();    // v:
            dst = it->first + "in";                     // rin:
        } else {
            src = it->first;                            // r: map sorted by v_prefix
            dst = (*it->second.begin())->v_prefix();    // v: all obj in a list has the same v_prefix as the first one
        }
        if (!is2dm[it->first]) {
            ret += addspaces() + dst + " " + eq + " " + src + ";\n";
        } else {
            char i_idx[2] = {0};
            for (auto &p : it->second) {
                i_idx[0] = 'i';
                ret += p->getCopyValue(i_idx, dst.c_str(),
                                       eq.c_str(),
                                       src.c_str());
            }
        }
    }
    return ret;
}
*/
/** Generate in process (synchronous) reset. Multiple resets are supported:

      if (!async_reset && i_rst0.read() == LOW) {
          module_type_r0(v0);
      }
      if (!async_reset && i_rst1.read() == LOW) {
          module_type_r1(v1);
      }
      if (!async_reset && i_rst0.read() == HIGH) {
          module_type_r0(v0);
      }
 */
/*std::string ModuleObject::generate_all_proc_v_reset(std::string &xrst) {
    std::map<std::string, std::list<GenObject *>>regmap;
    std::map<std::string, bool> is2dm;
    GenObject *preg;
    std::string ret;
    std::string v;
    std::string r;
    DecLogicConst low(new DecConst(1), 0);
    DecLogicConst high(new DecConst(1), 1);
    DecLogicConst *SV_STR_ACTIVE[3] = {0, &low, &high};

    getSortedRegsMap(regmap, is2dm);

    for (std::map<std::string, std::list<GenObject *>>::iterator it = regmap.begin();
        it != regmap.end(); ++it) {
        preg = (*it->second.begin());       // all registers in a group have the same reset and clock signals
        if (preg->getResetActive() == ACTIVE_NONE) {
            continue;
        }
        ret += addspaces() + "if (";
        if (xrst.size()) {
            ret += "(";
        }
        if (isAsyncResetParam()) {
            if (SCV_is_sysc()) {
                ret += "!async_reset_ && ";
            } else if (SCV_is_sv()) {
                ret += "~async_reset && ";
            } else if (SCV_is_vhdl()) {
                ret += "not async_reset and ";
            }
        }
        ret += preg->getResetPort()->getName();
        if (SCV_is_sysc()) {
            ret += ".read() == ";
        } else if (SCV_is_sv()) {
            ret += " == ";
        } else if (SCV_is_vhdl()) {
            ret += " = ";
        }
        ret += SV_STR_ACTIVE[preg->getResetActive()]->getStrValue() + ")";
        if (xrst.size()) {
            if (SCV_is_sysc() || SCV_is_sv()) {
                ret += " || ";
            } else if (SCV_is_vhdl()) {
                ret += " or ";
            }
            ret += xrst + ")";
        }
        if (SCV_is_sysc()) {
            ret += " {\n";
        } else if (SCV_is_sv()) {
            ret += " begin\n";
        } else if (SCV_is_vhdl()) {
            ret += " begin\n";
        }
        pushspaces();

        r = preg->r_prefix();
        v = preg->v_prefix();
        if (!is2dm[it->first]) {
            ret += addspaces();
            if (SCV_is_sysc()) {
                ret += getType() + "_" + r + "_reset(" + v + ");\n";
            } else if (SCV_is_sv()) {
                ret += v + " = " + getType() + "_" + r + "_reset;\n";
            } else if (SCV_is_vhdl()) {
                ret += v + " := " + getType() + "_" + r + "_reset;\n";
            }
        } else {
            char i_idx[2] = {0};
            for (auto &r : it->second) {
                i_idx[0] = 'i';
                ret += r->getCopyValue(i_idx, v.c_str(), "=", RSTVAL_NONE);
            }
        }

        popspaces();
        ret += addspaces();
        if (SCV_is_sysc()) {
            ret += "}\n";
        } else if (SCV_is_sv()) {
            ret += "end\n";
        } else if (SCV_is_vhdl()) {
            ret += "end\n";
        }
    }
    return ret;
}
*/


}
