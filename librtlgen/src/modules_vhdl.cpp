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
        if (p->getId() == ID_COMMENT) {
            comment += p->generate();
            continue;
        } else if (p->isParam() && !p->isParamGeneric() && p->isLocal() && !p->isGenericDep()) {
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

std::string ModuleObject::generate_vhdl_pkg_reg_struct(bool negedge) {
    std::string ret = "";
    std::string ln = "";
    std::string tstr;
    int tcnt = 0;
    bool twodim = false;

    ret += addspaces();
    ret += "type " + getType();
    if (negedge == false) {
        ret += "_registers";
    } else {
        ret += "_nregisters";
    }
    ret += " is record\n";
    pushspaces();
    for (auto &p: entries_) {
        if ((!p->isReg() && negedge == false)
            || (!p->isNReg() && negedge == true)) {
            continue;
        }
        ln = addspaces() + p->getName() + " : " + p->getType();
        if (p->getDepth()) {
            twodim = true;
            ln += "(0 to " + p->getStrDepth() + " - 1)";
        }
        ln += ";";
        if (p->getComment().size()) {
            while (ln.size() < 60) {
                ln += " ";
            }
            ln += p->addComment();
        }
        ret += ln + "\n";
    }
    popspaces();
    ret += addspaces() + "end record;\n";
    ret += "\n";

    // Reset function only if no two-dimensial signals
    tcnt = 0;
    for (auto &p: entries_) {
        if ((p->isReg() && negedge == false)
            || (p->isNReg() && negedge == true)) {
            tcnt++;
        }
    }
    if (!twodim) {
        ret += addspaces();
        if (negedge == false) {
            ret += "constant " + getType() + "_r_reset : " + getType() + "_registers := (\n";
        } else {
            ret += "constant " + getType() + "_nr_reset : " + getType() + "_nregisters := (\n";
        }
        pushspaces();
        for (auto &p: entries_) {
            if ((!p->isReg() && negedge == false)
                || (!p->isNReg() && negedge == true)) {
                continue;
            }
            ln = addspaces() + p->getStrValue();
            if (--tcnt) {
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
    bool twodim = false;        // if 2-dimensional register array, then do not use reset function
    if (isRegs() && isCombProcess()) {
        ret += generate_vhdl_pkg_reg_struct(false);
    }
    if (isNRegs() && isCombProcess()) {
        ret += generate_vhdl_pkg_reg_struct(true);
    }
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

    if (genparam.size() == 0 && !getAsyncReset()) {
        return ret;
    }
    ret += addspaces() + "generic (\n";
    pushspaces();

    if (getAsyncReset()) {
        ret += addspaces() + "async_reset : boolean := '0'";           // Mandatory generic parameter
        if (genparam.size()) {
            ret += ";";
        }
        ret += "\n";
        icnt++;
    }

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
        if (!p->isLocal() || !p->isGenericDep()) {
            continue;
        }
        ret += "constant " + p->getType() + " " + p->getName();
        if (p->getDepth() > 1) {
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
        if (p->getId() == ID_COMMENT) {
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
        if (p->isReg() || p->isNReg()
            || (!p->isSignal()
                && p->getId() != ID_VALUE
                && !p->isStruct()
                && p->getId() != ID_CLOCK
                && p->getId() != ID_ARRAY_DEF)) {
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
    for (auto &p: getEntries()) {
        if (p->getId() != ID_FILEVALUE) {
            continue;
        }
        ret += "file " + p->getName() + ";\n";
        tcnt++;
    }

    if (isRegs() && isCombProcess()) {
        ret += addspaces();
        ret += "signal r, rin : " + getType() + "_registers;\n";
        tcnt++;
    }
    if (isNRegs() && isCombProcess()) {
        ret += addspaces();
        ret += "signal nr, nrin : " + getType() + "_nregisters;\n";
        tcnt++;
    }
    if (tcnt) {
        ret += "\n";
        tcnt = 0;
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
            if (p->getId() == ID_COMMENT) {
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
    for (auto &p: entries_) {
        if (p->getId() != ID_FUNCTION) {
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
        if (p->getId() != ID_CLOCK) {
            continue;
        }
        ret += generate_sv_mod_clock(p);
    }

    // Process
    for (auto &p: entries_) {
        if (p->getId() != ID_PROCESS) {
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
