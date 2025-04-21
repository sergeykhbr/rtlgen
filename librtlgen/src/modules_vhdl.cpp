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
#include "structs.h"
#include "regs.h"
#include "operations.h"
#include "array.h"
#include "funcs.h"
#include <list>

namespace sysvc {

std::string ModuleObject::generate_vhdl_pkg_localparam() {
    std::string ret = "";
    std::string ln = "";
    std::string comment = "";
    int tcnt = 0;
    // Local paramaters visible inside of module
    for (auto &p: entries_) {
        if (p->isComment()) {
            comment += p->generate();
            continue;
        } else if (p->isParam() && !p->isParamGeneric() && !p->isGenericDep()) {
            ret += comment;
            ln = addspaces() + "constant " + p->getName() + " : " + p->getType();
            ln += " := " + p->generate() + ";";
            p->addComment(ln);
            ret += ln + "\n";
            tcnt++;
        }
        comment = "";
    }
    if (tcnt) {
        ret += "\n";
    }
    return ret;
}

std::string ModuleObject::generate_vhdl_pkg_reg_struct() {
    std::map<std::string, std::list<GenObject *>> regmap;
    std::map<std::string, bool> is2dm;
    std::string ret = "";
    std::string ln = "";
    std::string r;
    std::string v;

    getSortedRegsMap(regmap, is2dm);
    for (std::map<std::string, std::list<GenObject *>>::iterator it = regmap.begin();
        it != regmap.end(); it++) {
        r = it->first;                          // map sorted by v_prefix
        v = (*it->second.begin())->v_prefix();  // all obj in a list has the same v_prefix as the first one

        ret += addspaces() + "type " + getType() + "_" + r + "egisters is record\n";
        pushspaces();
        for (auto &p: it->second) {
            ln = addspaces();

            ln = addspaces() + p->getName() + " : " + p->getType();
            if (p->getObjDepth()) {
                ln += "(0 to " + p->getStrDepth() + " - 1)";
            }
            ln += ";";
            p->addComment(ln);
            ret += ln + "\n";
        }
        popspaces();
        ret += addspaces() + "end record;\n";
        ret += "\n";

        // Generate reset function only for simple regs with defined reset:
        if (!is2dm[it->first] && (*it->second.begin())->getResetActive() != ACTIVE_NONE) {
            ret += addspaces();
            ret += "constant " + getType() + "_" + r + "_reset : " + getType() + "_" + r +"egisters := (\n";
            pushspaces();
            for (auto &p: it->second) {
                ln = addspaces() + p->getStrValue();
                if (p != it->second.back()) {
                    ln += ",";
                }
                while (ln.size() < 40) {
                    ln += " ";
                }
                ln += "-- " + p->getName();
                ret += ln + "\n";
            }
            popspaces();
            ret += addspaces() + ");\n";
            ret += "\n";
        }
    }
    return ret;
}


std::string ModuleObject::generate_vhdl_pkg_struct() {
    std::string ret = "";
    std::string ln = "";
    std::string tstr;
    int tcnt = 0;

    // struct definitions
    for (auto &p: entries_) {
        if (p->isStruct() && p->isTypedef()) {
            ret += p->generate();
            tcnt++;
        }
    }
    if (tcnt) {
        ret += "\n";
        tcnt = 0;
    }
    // Register structure definition
    ret += generate_vhdl_pkg_reg_struct();
    return ret;
}


std::string ModuleObject::generate_vhdl_pkg() {
    std::string ret = "";
    ret += generate_vhdl_pkg_localparam();
    ret += generate_vhdl_pkg_struct();
    return ret;
}


std::string ModuleObject::generate_vhdl_mod_genparam() {
    std::string ret = "";
    std::string ln;
    int icnt = 0;
    std::list<GenObject *> genparam;
    getTmplParamList(genparam);
    getParamList(genparam);

    if (genparam.size() == 0 && !isAsyncResetParam()) {
        return ret;
    }
    ret += addspaces() + "generic (\n";
    pushspaces();

    /*if (isAsyncResetParam() && getAsyncResetParam() == 0) {
        ret += addspaces() + "async_reset : boolean := '0'";           // Mandatory generic parameter
        if (genparam.size()) {
            ret += ";";
        }
        ret += "\n";
        icnt++;
    }*/

    for (auto &p : genparam) {
        ln = addspaces() + p->getName() + " : ";
        ln += p->getType() + " := " + p->generate();

        if (p != genparam.back()) {
            ln += ";";
        }
        p->addComment(ln);
        ret += ln + "\n";
    }

    popspaces();
    ret += addspaces() + ");\n";
    return ret;
}

std::string ModuleObject::generate_vhdl_mod_param_strings() {
    std::string ret = "";
    int tcnt = 0;
    for (auto &p: getEntries()) {
        if (!p->isParam() || p->isParamGeneric()) {
            continue;
        }
        if (!p->isGenericDep()) {
            continue;
        }
        ret += "constant " + p->getType() + " " + p->getName();
        if (p->getObjDepth()) {
            ret += "(0 up " + p->getStrDepth() +"-1)";
        }
        ret += " := " + p->generate() + ";\n";

        tcnt++;
    }
    if (tcnt) {
        ret += "\n";
    }
    return ret;
}

std::string ModuleObject::generate_vhdl_mod_func(GenObject *func) {
    std::string ret = "function ";
    ret += static_cast<FunctionObject *>(func)->generate();
    return ret;
}

std::string ModuleObject::generate_vhdl_mod_signals() {
    std::string ret = "";
    std::string ln;
    std::string text;
    int tcnt = 0;
    tcnt = 0;
    text = "";
    for (auto &p: getEntries()) {
        if (p->isComment()) {
            text += p->generate();
            continue;
        }
        if (p->isInput()
            || p->isOutput()
            || p->isOperation()
            || p->isTypedef()
            || p->isParam()) {
            text = "";
            continue;
        }
        if (p->getName() == "") {
            SHOW_ERROR("Unnamed type %s", p->getType().c_str());
            // typedef should be skipped
            continue;
        }
        if (p->getClockEdge() == CLK_POSEDGE || p->getClockEdge() == CLK_NEGEDGE
            || (!p->isSignal()
                && !p->isValue()
                && !p->isStruct()
                && !p->isClock())) {
            text = "";
            continue;
        }
        if (text.size()) {
            ret += text;
            text = "";
        }
        ln = addspaces() + "signal " + p->getName() + " : " + p->getType();
        if (p->getDepth() && !p->isVector()) {
            ln += "(0 up " + p->getStrDepth() + " - 1)";
        }
        ln += ";";
        if (p->getComment().size()) {
            while (ln.size() < 60) {
                ln += " ";
            }
            ln += p->addComment();
        }
        ret += ln + "\n";
        tcnt++;
    }

    std::map<std::string, std::list<GenObject *>> regmap;
    std::map<std::string, bool> is2dm;
    std::string r;
    getSortedRegsMap(regmap, is2dm);
    for (std::map<std::string, std::list<GenObject *>>::iterator it = regmap.begin();
        it != regmap.end(); it++) {
        r = it->first;                          // map sorted by v_prefix
        ret += addspaces();
        ret += "signal " + r + ", " + r + "in : " + getType() + "_" + r + "egisters;\n";
    }
    if (regmap.size()) {
        ret += "\n";
    }
    return ret;
}

std::string ModuleObject::generate_vhdl_mod() {
    int tcnt = 0;
    std::string ret = "";
    std::string text;
    std::string ln;
    std::list<GenObject *> tmplparam;
    std::map<std::string, std::list<GenObject *>> vhdlibs;
    getTmplParamList(tmplparam);

    // import statement:
    std::map<std::string, std::list<std::string>> pkglst;
    FileObject *pf = static_cast<FileObject *>(getParent());
    pf->getDepLibList(pkglst);

    if (tmplparam.size() == 0) {
        // Add own package
        std::string ownpkg = pf->getName() + "_pkg";
        pkglst[pf->getLibName()].push_back(ownpkg);
    }

    ret += "library ieee;\n";
    ret += "use ieee.std_logic_1164.all;\n";
    ret += "use ieee.numeric_std.all;\n";

    for (std::map<std::string, std::list<std::string>>::const_iterator 
        it = pkglst.begin(); it != pkglst.end(); ++it) {
        ret += "\nlibrary " + it->first + ";\n";
        for (auto &e: it->second) {
            ret += "use " + it->first + "." + e + ".all;\n";
        }
    }
    if (tmplparam.size() != 0) {
        // insert pkg data for template modules: ram, queue, ..
        ret += generate_vhdl_pkg_localparam();
    }

    ret += "\n";
    ret += "entity " + getType() + " is ";

    // Generic parameters
    ret += generate_vhdl_mod_genparam();

    // In/Out ports
    ret += addspaces() + "port (\n";
    pushspaces();
    int port_cnt = 0;
    for (auto &p: entries_) {
        if (p->isInput() || p->isOutput()) {
            port_cnt++;
        }
    }
    text = "";
    std::string strtype;
    for (auto &p: entries_) {
        if (!p->isInput() && !p->isOutput()) {
            if (p->isComment()) {
                text += p->generate();
            } else {
                text = "";
            }
            continue;
        }
        ret += text;
        text = "";
        ln = addspaces() + p->getName() + " : ";
        if (p->isInput() && p->isOutput()) {
            ln += "inout ";
        } else if (p->isInput()) {
            ln += "in ";
        } else {
            ln += "out ";
        }
        SCV_set_generator(VHDL_PKG);  // to generate with package name
        strtype = p->getType();
//        if (SCV_is_cfg_parameter(strtype) && SCV_get_cfg_file(strtype).size()) {
//            // whole types (like vectors or typedef)
//            ln += SCV_get_cfg_file(strtype) + "_pkg::" + strtype;
//        } else {
            // Width or depth definitions
            ln += strtype;
//        }
        SCV_set_generator(VHDL_ALL);

        if (--port_cnt) {
            ln += ";";
        }
        if (p->getComment().size()) {
            while (ln.size() < 60) {
                ln += " ";
            }
            ln += "-- " + p->getComment();
        }
        ret += ln + "\n";
    }
    ret += addspaces() + ");\n";
    popspaces();
    ret += "end;\n";
    ret += "\n";

    ret += "architecture arch_" + getType() + " of " + getType() + " is\n";
    ret += "\n";
    pushspaces();

    // static strings
    ret += generate_vhdl_mod_param_strings();

    // struct definitions:
    if (tmplparam.size()) {
        ret += generate_vhdl_pkg_struct();
    }


    // Signal list:
    ret += generate_vhdl_mod_signals();

    // Functions
    for (auto &p: getEntries()) {
        if (!p->isFunction()) {
            continue;
        }
        ret += generate_vhdl_mod_func(p);
    }

    popspaces();
    ret += "begin\n";
    ret += "\n";
    pushspaces();


    // Sub-module instantiation
    for (auto &p: getEntries()) {
        if (p->isOperation()) {
            ret += p->generate();
        }
    }
/*
    // Clock process
    for (auto &p: entries_) {
        if (!p->isClock()) {
            continue;
        }
        ret += generate_sv_mod_clock(p);
    }

    // Process
    for (auto &p: entries_) {
        if (!p->isProcess()) {
            continue;
        }
        if (p->getName() == "registers") {
            continue;
        }
        ret += generate_sv_mod_proc(p);
    }

    if (isRegProcess()) {
        ret += generate_sv_mod_proc_registers();
    }
*/

    popspaces();
    ret += "end;\n";

    return ret;
}

}
