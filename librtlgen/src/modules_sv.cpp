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
#include <cstring>

namespace sysvc {

/**
    module without package: no_gendep = false && only_gendep = false
    module package:         no_gendep = true && only_gendep = false
    module with package:    no_gendep = false && only_gendep = true
   
 */
std::string ModuleObject::generate_sv_localparam(bool no_gendep,
                                                 bool only_gendep) {
    std::string ret = "";
    std::string comment = "";
    for (auto &p: getEntries()) {
        if (p->isComment()) {
            comment += p->generate();
            continue;
        }
        if (!p->isParam() || p->isParamGeneric()) {
            comment = "";
            continue;
        }
        if (no_gendep && p->isGenericDep()
            || only_gendep && !p->isGenericDep()) {
            comment = "";
            continue;
        }
        ret += comment;
        ret += p->generate();
        comment = "";
    }

    if (ret.size()) {
        ret += "\n";
    }
    return ret;
}

std::string ModuleObject::generate_sv_struct() {
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
        comment = "";
    }
    return ret;
}


std::string ModuleObject::generate_sv_pkg() {
    std::string ret = "";
    std::string comment = "";
    ret += generate_sv_localparam(true,     // no_gendep
                                  false);   // only_gendep
    ret += generate_sv_struct();
    return ret;
}

std::string ModuleObject::generate_sv_mod_genparam() {
    std::string ret = "";
    std::string ln;
    int icnt = 0;
    std::list<GenObject *> genparam;
    getTmplParamList(genparam);
    getParamList(genparam);

    pushspaces();
    for (auto &p : genparam) {
        ln = addspaces() + "parameter ";
        if (!p->isString()) {
            // vivado doesn't support string as parameter:
            ln += p->getType() + " ";
        }
        ln += p->getName() + " = " + p->getStrValue();
        if (p != genparam.back()) {
            ln += ",";
        }
        p->addComment(ln);
        ret += ln + "\n";
    }
    popspaces();

    return ret;
}

std::string ModuleObject::generate_sv_mod_func(GenObject *func) {
    std::string ret = "function ";
    ret += static_cast<FunctionObject *>(func)->generate();
    return ret;
}

std::string ModuleObject::generate_sv_mod_signals() {
    std::string ret = "";
    std::string ln;
    std::string text;
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
            || p->isParam()
            || p->isConst()) {
            text = "";
            continue;
        }
        if (p->getName() == "") {
            // todo: struct_def should be skipped (mark it as a typedef true). 
            SHOW_ERROR("Unnamed entry of type %s", p->getType().c_str());
            text = "";
            continue;
        }
        if (!p->isSignal()
                && !p->isValue()
                && !p->isStruct()
                && !p->isClock()) {
            text = "";
            continue;
        }
        if (text.size()) {
            ret += text;
            text = "";
        }
        ln = addspaces() + p->getType() + " " + p->getName();
        if (p->getDepth() && !p->isVector()) {
            ln += "[0: " + p->getStrDepth() + " - 1]";
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

std::string ModuleObject::generate_sv_mod_clock(GenObject *p) {
    std::string ret = "";

    ret += addspaces() + "always begin\n";
    pushspaces();

    ret += addspaces() + "#(0.5 * ";
    ret += "1000000000 * ";     // timescale default 1ns/10ps
    ret += p->getStrValue() + ") ";
    ret += p->getName() + " = ~" + p->getName() + ";\n";

    popspaces();
    ret += addspaces() + "end\n";
    ret += "\n";
    return ret;
}


std::string ModuleObject::generate_sv_mod(bool no_pkg) {
    size_t tcnt = 0;
    std::string ret = "";
    std::string text;
    std::string ln;
    std::list<GenObject *> tmplparam;
    getTmplParamList(tmplparam);

    ret += "module " + getType();

    // Generic parameters
    ln = generate_sv_mod_genparam();
    if (ln.size()) {
        ret += " #(\n";
        ret += ln;
        ret += ")\n";
    }

    // In/Out ports
    ret += "(\n";
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
        ln = addspaces();
        if (p->isInput() && p->isOutput()) {
            ln += "inout ";
        } else if (p->isInput()) {
            ln += "input ";
        } else {
            ln += "output ";
        }
        SCV_set_generator(SV_PKG);  // to generate with package name
        ln += p->getType();
        SCV_set_generator(SV_ALL);

        ln += " " + p->getName();
        if (--port_cnt) {
            ln += ",";
        }
        p->addComment(ln);
        ret += ln + "\n";
    }
    popspaces();
    ret += ");\n";
    ret += "\n";

    // import statement:
    std::list<std::string> pkglst;
    FileObject *pf = static_cast<FileObject *>(getParent());
    pf->getDepList(pkglst);
    for (auto &e: pkglst) {
        ret += "import " + e + "::*;\n";
    }
    if (no_pkg == false) {
        ret += "import " + pf->getName() + "_pkg::*;\n";
        ret += "\n";
        ret += generate_sv_localparam(false,    // no_gendep
                                      true);    // only_gendep
    } else {
        ret += generate_sv_localparam(false,    // no_gendep
                                      false);   // only_gendep
        ret += generate_sv_struct();
    }

    // Signal list:
    ret += generate_sv_mod_signals();

    // Functions
    for (auto &p: getEntries()) {
        if (!p->isFunction()) {
            continue;
        }
        ret += generate_sv_mod_func(p);
    }

    // Sub-module instantiation
//    tcnt = ret.size();
    for (auto &p: getEntries()) {
        if (p->isOperation()) {
            ret += p->generate();
        }
    }
//    if (tcnt != ret.size()) {
//        ret += "\n";
//    }

    // Process
    for (auto &p: getEntries()) {
        if (!p->isProcess()) {
            continue;
        }
        ret += p->generate();
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
    std::list<GenObject *> combproc;
    getCombProcess(combproc);
    for (auto &p: combproc) {
        ret += "\n";
        ret += generate_sv_mod_proc(p);
    }

    ret += generate_sv_mod_proc_registers();
*/

    ret += "endmodule: " + getType() + "\n";


    return ret;
}

}
