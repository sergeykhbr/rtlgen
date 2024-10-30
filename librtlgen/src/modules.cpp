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
#include <string.h>
#include <algorithm>

namespace sysvc {

ModuleObject::ModuleObject(GenObject *parent,
                           const char *type,
                           const char *name,
                           const char *comment)
    : GenObject(parent, comment),
    async_reset_(0, "async_reset", "1", "0", NO_COMMENT) {
    name_ = std::string(name);
    type_ = std::string(type);
    if (name_ == "") {
        SHOW_ERROR("%s", "Unnamed module");
    }

    SCV_add_module(this);
}

GenObject *ModuleObject::getAsyncResetParam() {
    for (auto &p: getEntries()) {
        if (p->isParam()) {
            if (p->getName() == "async_reset") {
                return p;
            }
        }
    }
    return 0;
}

GenObject *ModuleObject::getResetPort() {
    for (auto &p: getEntries()) {
        if (p->isInput()) {
            if (p->getName() == "i_nrst") {
                return p;
            }
        }
    }
    return 0;
}

GenObject *ModuleObject::getClockPort() {
    for (auto &p: getEntries()) {
        if (p->isInput()) {
            if (p->getName() == "i_clk") {
                return p;
            }
        }
    }
    return 0;
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

void ModuleObject::getCombProcess(std::list<GenObject *> &proclist) {
    std::map<std::string,std::list<GenObject *>> regmap;
    std::map<std::string,bool> is2dm;
    std::list<std::string> regproclist;

    // list of registers process depending of clock and reset
    getSortedRegsMap(regmap,is2dm);
    for (std::map<std::string,std::list<GenObject *>>::iterator it = regmap.begin();
        it != regmap.end(); ++it) {
        regproclist.push_back(it->first + "egisters");
    }

    for (auto &p: getEntries()) {
        if (!p->isProcess()) {
            continue;
        }
        // Exclude process with name equals to <r>egisters process,
        // because content of such process should be added to trigger action
        if (std::find(regproclist.begin(),regproclist.end(),p->getName())
            != regproclist.end()) {
            continue;
        }
        proclist.push_back(p);
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

std::string ModuleObject::generate_all_proc_nullify(GenObject *obj,
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
}

/**
    v = r;      (inverse = false)
    or
    rin <= v    (inverse = true), SystemVerilog, VHDL
 */
std::string ModuleObject::generate_all_proc_r_to_v(bool inverse) {
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
std::string ModuleObject::generate_all_proc_v_reset(std::string &xrst) {
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

        ret += addspaces() + "if ";
        if (xrst.size()) {
            ret += "(";
        }
        if (isAsyncResetParam()) {
            if (SCV_is_sysc()) {
                ret += "(!async_reset_ && ";
            } else if (SCV_is_sv()) {
                ret += "(~async_reset && ";
            } else if (SCV_is_vhdl()) {
                ret += "(not async_reset and ";
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



}
