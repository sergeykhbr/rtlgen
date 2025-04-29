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

/*std::string ModuleObject::generate_sv_pkg_localparam() {
    std::string ret = "";
    std::string ln = "";
    std::string comment = "";
    int tcnt = 0;
    // Local paramaters visible inside of module
    for (auto &p: entries_) {
        if (p->isComment()) {
            comment += p->generate();
            continue;
        } else if (p->isParam() && p->isString() && !p->isParamGeneric()) {
            ret += comment;
            ret += p->generate();
        } else if (p->isParam() && !p->isParamGeneric() && !p->isGenericDep()) {
            ret += comment;
            if (p->isString()) {
                // Vivado doesn't support string parameters, skip type
                ln = "localparam " + p->getName();
            } else {
                ln = "localparam " + p->getType() + " " + p->getName();
            }
            ln += " = " + p->getStrValue() + ";";
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
}*/

/*std::string ModuleObject::generate_sv_pkg_reg_struct() {
    std::map<std::string,std::list<GenObject *>> regmap;
    std::map<std::string,bool> is2dm;
    std::string ret = "";
    std::string ln = "";
    std::string r;
    std::string v;

    getSortedRegsMap(regmap,is2dm);
    for (std::map<std::string,std::list<GenObject *>>::iterator it = regmap.begin();
        it != regmap.end(); it++) {
        r = it->first;                          // map sorted by v_prefix
        v = (*it->second.begin())->v_prefix();  // all obj in a list has the same v_prefix as the first one

        ret += addspaces() + "typedef struct {\n";
        pushspaces();
        for (auto &p: it->second) {
            ln = addspaces() + p->getType() + " " + p->getName();
            if (p->getObjDepth()) {
                ln += "[0: " + p->getStrDepth() + " - 1]";
            }
            ln += ";";
            p->addComment(ln);
            ret += ln + "\n";
        }
        popspaces();
        ret += addspaces() + "} " + getType();
        ret += "_" + r + "egisters;\n";
        ret += "\n";

        // Generate reset function only for simple regs with defined reset:
        if (!is2dm[it->first] && (*it->second.begin())->getResetActive() != ACTIVE_NONE) {
            ret += addspaces();
            ret += "const " + getType() + "_" + r + "egisters " + getType() + "_" + r + "_reset = '{\n";
            pushspaces();
            for (auto &p: it->second) {
                ln = addspaces() + p->getStrValue();
                if (p != it->second.back()) {
                    ln += ",";
                }
                while (ln.size() < 40) {
                    ln += " ";
                }
                ln += "// " + p->getName();
                ret += ln + "\n";
            }
            popspaces();
            ret += addspaces() + "};\n";
            ret += "\n";
        }
    }
    return ret;
}*/

/*std::string ModuleObject::generate_sv_pkg_struct() {
    std::string ret = "";
    std::string ln = "";
    std::string tstr;
    int tcnt = 0;
#if 1
    if (getType() == "axictrl_bus0") {
        bool v = true;
    }
#endif

    // struct definitions
    for (auto &p: getEntries()) {
        if (p->isStruct() && 
            (p->isTypedef() || p->isConst())) {
            ret += p->generate();
            tcnt++;
        }
    }
    if (tcnt) {
        ret += "\n";
        tcnt = 0;
    }
    // Register structure definition
    //ret += generate_sv_pkg_reg_struct();
    return ret;
}*/


std::string ModuleObject::generate_sv_pkg() {
    std::string ret = "";
    std::string comment = "";
    bool prev_was_param = false;        // to minimize backward difference
    //ret += generate_sv_pkg_localparam();
    //ret += generate_sv_pkg_struct();
    for (auto &p: getEntries()) {
        if (p->isComment()) {
            comment = p->generate();
            continue;
        }
        if ((p->isParam() && p->isString() && !p->isParamGeneric())
            || (p->isParam() && !p->isParamGeneric() && !p->isGenericDep())
            || (p->isStruct() && (p->isTypedef() || p->isConst()))) {

            if (p->isStruct() && prev_was_param) {
                ret += "\n";
            }
            ret += comment;
            ret += p->generate();

            prev_was_param = p->isParam();
        }
        comment = "";
    }

    if (ret.size()) {
        ret += "\n";
    }
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

std::string ModuleObject::generate_sv_mod_param_strings() {
    std::string ret = "";
    int tcnt = 0;
    for (auto &p: getEntries()) {
        if (!p->isParam() || p->isParamGeneric()) {
            continue;
        }
        if (!p->isGenericDep()) {
            continue;
        }
        if (p->isString()) {
            // Vivado doesn't support string parameters
            ret += "localparam " + p->getName();
        } else {
            ret += "localparam " + p->getType() + " " + p->getName();
        }
        if (p->getObjDepth()) {
            ret += "[0: " + p->getStrDepth() +"-1]";
        }
        ret += " = " + p->generate() + ";\n";
        tcnt++;
    }
    if (tcnt) {
        ret += "\n";
    }
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
    } else {
        // insert pkg data for template modules: ram, queue, ..
        //ret += generate_sv_pkg_localparam();
        ret += generate_sv_pkg();
    }

    // static strings
    ret += generate_sv_mod_param_strings();

    // struct definitions:
    //if (tmplparam.size()) {
    //    ret += generate_sv_pkg_struct();
    //}


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
    tcnt = ret.size();
    for (auto &p: getEntries()) {
        if (p->isOperation()) {
            ret += p->generate();
        }
    }
    if (tcnt != ret.size()) {
        ret += "\n";
    }

    // Process
    for (auto &p: getEntries()) {
        if (!p->isProcess()) {
            continue;
        }
        ret += p->generate();
    }
#if 1
    if (addspaces().size() != 0) {
        const char *tname = getName().c_str();
        bool lt = true;
    }
#endif
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
