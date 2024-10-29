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

std::string ModuleObject::generate_sv_pkg_localparam() {
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
}

std::string ModuleObject::generate_sv_pkg_reg_struct() {
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

        // Generate reset constant if possible:
        if (!is2dm[it->first]) {
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
}

std::string ModuleObject::generate_sv_pkg_struct() {
    std::string ret = "";
    std::string ln = "";
    std::string tstr;
    int tcnt = 0;

    // struct definitions
    for (auto &p: getEntries()) {
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
    ret += generate_sv_pkg_reg_struct();
    return ret;
}


std::string ModuleObject::generate_sv_pkg() {
    std::string ret = "";
    ret += generate_sv_pkg_localparam();
    ret += generate_sv_pkg_struct();
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
    if (isAsyncResetParam() && getAsyncResetParam() == 0) {
        ret += addspaces() + "parameter bit async_reset = 1'b0";           // Mandatory generic parameter
        if (genparam.size()) {
            ret += ",";
        }
        ret += "\n";
        icnt++;
    }

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
            || p->isParam()
            || p->isReg()
            || p->isNReg()) {
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
        tcnt++;
    }

    // r, rin:
    std::list<GenObject *> proclist;
    getCombProcess(proclist);
    if (proclist.size()) {
        std::map<std::string,std::list<GenObject *>>regmap;
        std::map<std::string,bool> is2dm;
        std::string r;

        getSortedRegsMap(regmap, is2dm);
        for (std::map<std::string,std::list<GenObject *>>::iterator it = regmap.begin();
            it != regmap.end(); it++) {
            r = it->first;                          // map sorted by v_prefix
            ret += getType() + "_" + r + "egisters " + r + ", " + r + "in;\n";
            tcnt++;
        }

    }
    if (tcnt) {
        ret += "\n";
        tcnt = 0;
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

std::string ModuleObject::generate_sv_mod_proc(GenObject *proc) {
    std::string ret = "";
    std::string ln;
    int tcnt = 0;

    if (proc->isGenerate() == false) {
        ret += "always_comb\n";
        ret += "begin: " + proc->getName() + "_proc\n";
    }
    pushspaces();
    
    // process variables declaration
    tcnt = 0;
    if (isRegs()) {
        ret += addspaces() + getType() + "_registers v;\n";
    }
    if (isNRegs()) {
        ret += addspaces() + getType() + "_nregisters nv;\n";
    }

    for (auto &e: proc->getEntries()) {
        if (!e->isValue() && !e->isStruct()) {    // no structure inside of process, typedef can be removed
            continue;
        }

        ln = addspaces() + e->getType() + " " + e->getName();
        if (e->getObjDepth()) {
            ln += "[0: " + e->getStrDepth() + "-1]";
        }
        ln += ";";
        e->addComment(ln);
        ret += ln + "\n";
        tcnt++;
    }
    if (tcnt) {
        ret += "\n";
        tcnt = 0;
    }

    // nullify all local variables to avoid latches:
    size_t ret_sz = ret.size();
    for (auto &e: proc->getEntries()) {
        ret += generate_all_proc_nullify(e, "", "i");
    }
    if (ret.size() != ret_sz) {
        ret += "\n";
    }

    if (isRegs()) {
        ret += Operation::copyreg("v", "r", this);
        tcnt++;
    }
    if (isNRegs()) {
        ret += Operation::copyreg("nv", "nr", this);
        tcnt++;
    }
    if (tcnt) {
        ret += "\n";
        tcnt = 0;
    }

    // Generate operations:
    for (auto &e: proc->getEntries()) {
        if (e->isOperation()) {
            ret += e->generate();
        }
    }

    if (isRegs()) {
        ret += "\n";
        ret += Operation::copyreg("rin", "v", this);
    }
    if (isNRegs()) {
        ret += Operation::copyreg("nrin", "nv", this);
    }
    popspaces();
    if (proc->isGenerate() == false) {
        ret += "end: " + proc->getName() + "_proc\n";
    }
    ret += "\n";
    return ret;
}

std::string ModuleObject::generate_sv_mod_proc_always(bool async_on_off) {
    std::map<std::string,std::list<GenObject *>> regmap;
    std::map<std::string,bool> is2dm;
    std::list<GenObject *> combproc;
    std::string ret = "";
    std::string r;
    std::string procname;
    EResetActive active;
    std::string blkname = "rg_proc";  // to minimize differences. Could be removed later
    GenObject *preg;
    GenObject *resobj;
    char i_idx[2] = {0};

    const char *SV_STR_CLKEDGE[3] = {"*", "posedge", "negedge"};
    const char *SV_STR_RSTEDGE[3] = {"", "negedge", "posgedge"};
    const char *SV_STR_ACTIVE[3] = {"", "1'b0", "1'b1"};

    getCombProcess(combproc);
    getSortedRegsMap(regmap, is2dm);

    /**
        always_ff @(posedge clk, negedge nrst) begin: rg_proc
            if (nrst == 1'b0) begin
                r <= ModuleType_r_reset;
            end else begin
                r <= rin;
            end
        end: rg_proc
    */
    for (std::map<std::string,std::list<GenObject *>>::iterator it = regmap.begin();
        it != regmap.end(); it++) {
        preg = (*it->second.begin());
        resobj = preg->getResetPort();

        r = it->first;                          // map sorted by v_prefix
        blkname = r + "g_proc";                 // to minimize differences. Could be removed later
        active = preg->getResetActive();
        procname = r + "egisters";

        ret += addspaces() + "always_ff @(";
        ret += SV_STR_CLKEDGE[preg->getClockEdge()];
        ret += std::string(" ") + preg->getClockPort()->getName();

        if (async_on_off && active != ACTIVE_NONE) {
            ret += std::string(", ") + SV_STR_RSTEDGE[active] + " ";
            ret += resobj->getName();
        }

        ret += ") begin: " + blkname + "\n";
        pushspaces();

        if (async_on_off && active != ACTIVE_NONE) {
            ret += addspaces() + "if (" + resobj->getName() + " == ";
            ret += std::string(SV_STR_ACTIVE[active]) + ") begin\n";
            pushspaces();
                
            // r <= reset
            if (!is2dm[it->first]) {
                ret += addspaces() + r + " <= " + getType() + "_" + r + "_reset;\n";
            } else {
                for (auto &p : it->second) {
                    i_idx[0] = 'i';
                    ret += p->getCopyValue(i_idx, r.c_str(), "<=", "");
                }
            }

            popspaces();
            ret += addspaces() + "end else begin\n";
            pushspaces();
        }

        // r <= rin
        if (!is2dm[it->first]) {
            ret += addspaces() + r + " <= " + r + "in;\n";
        } else {
            for (auto &p : it->second) {
                i_idx[0] = 'i';
                ret += p->getCopyValue(i_idx, r.c_str(), "<=", (r + "in").c_str());
            }
        }

        if (async_on_off && active != ACTIVE_NONE) {
            popspaces();
            ret += addspaces() + "end\n";
        }

        // additional operation on posedge clock events
        for(auto &p : getEntries()) {
            if(!p->isProcess() || p->getName() != procname) {
                continue;
            }
            ret += "\n";
            for(auto &s: p->getEntries()) {
                ret += s->generate();
            }
        }

        popspaces();
        ret += addspaces() + "end: " + blkname + "\n";
    }
    return ret; 
}

std::string ModuleObject::generate_sv_mod_proc_registers() {
    std::string ret = "";

    // Check if registers with reset exists in this module
    bool reset_exists = false;
    for (auto &p : getEntries()) {
        if (p->getResetActive() != ACTIVE_NONE) {
            reset_exists = true;
            break;
        }
    }

    // Async reset only if registers exists
    if (isAsyncResetParam() && reset_exists) {
        ret += "\n";            // remove me: just to minimize diff.
        ret += "generate\n";
        pushspaces();

        ret += addspaces() + "if (async_reset) begin: async_rst_gen\n";
        pushspaces();

        ret += "\n";
        ret += generate_sv_mod_proc_always(true);
        ret += "\n";

        popspaces();
        ret += addspaces() + "end: async_rst_gen\n";
        ret += addspaces() + "else begin: no_rst_gen\n";
        pushspaces();
        ret += "\n";
    }

    ret += generate_sv_mod_proc_always(false);

    if (isAsyncResetParam() && reset_exists) {
        ret += "\n";
        popspaces();
        ret += addspaces() + "end: no_rst_gen\n";
        popspaces();
        ret += "endgenerate\n";
        ret += "\n";
    }
    return ret;
}


std::string ModuleObject::generate_sv_mod() {
    int tcnt = 0;
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
    if (tmplparam.size() == 0) {
        ret += "import " + pf->getName() + "_pkg::*;\n";
        ret += "\n";
    } else {
        // insert pkg data for template modules: ram, queue, ..
        ret += generate_sv_pkg_localparam();
    }

    // static strings
    ret += generate_sv_mod_param_strings();

    // struct definitions:
    if (tmplparam.size()) {
        ret += generate_sv_pkg_struct();
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
    for (auto &p: getEntries()) {
        if (p->isOperation()) {
            ret += p->generate();
        }
    }

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

    ret += "endmodule: " + getType() + "\n";


    return ret;
}

}
