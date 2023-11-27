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
        if (p->getId() == ID_COMMENT) {
            comment += p->generate();
            continue;
        } else if (p->isParam() && !p->isParamGeneric() && p->isLocal() && !p->isGenericDep()) {
            ret += comment;
            if (p->isString()) {
                // Vivado doesn't support string parameters, skip type
                ln = "localparam " + p->getName();
            } else {
                ln = "localparam " + p->getType() + " " + p->getName();
            }
            ln += " = " + p->generate() + ";";
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

std::string ModuleObject::generate_sv_pkg_reg_struct(bool negedge) {
    std::string ret = "";
    std::string ln = "";
    int tcnt = 0;
    bool generate_struct_rst = true;    // do not generate structure if there's another structure without _none value
                                        // or array

    ret += addspaces() + "typedef struct {\n";
    pushspaces();
    for (auto &p: getEntries()) {
        if ((!p->isReg() && negedge == false)
            || (!p->isNReg() && negedge == true)) {
            continue;
        }
        ln = addspaces() + p->getType() + " " + p->getName();
        if (p->getDepth() > 1) {
            generate_struct_rst = false;
            ln += "[0: " + p->getStrDepth() + " - 1]";
        }
        ln += ";";
        p->addComment(ln);
        ret += ln + "\n";
        tcnt++;
    }
    popspaces();
    ret += addspaces() + "} " + getType();
    if (negedge == false) {
        ret += "_registers;\n";
    } else {
        ret += "_nregisters;\n";
    }
    ret += "\n";


    // Generate reset constant if possible:
    if (generate_struct_rst) {
        ret += addspaces();
        if (negedge == false) {
            ret += "const " + getType() + "_registers " + getType() + "_r_reset = '{\n";
        } else {
            ret += "const " + getType() + "_nregisters " + getType() + "_nr_reset = '{\n";
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
            ln += "// " + p->getName();
            ret += ln + "\n";
        }
        popspaces();
        ret += addspaces() + "};\n";
        ret += "\n";
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
    bool twodim = false;        // if 2-dimensional register array, then do not use reset function
    if (isRegs() && isCombProcess()) {
        ret += generate_sv_pkg_reg_struct(false);
    }
    if (isNRegs() && isCombProcess()) {
        ret += generate_sv_pkg_reg_struct(true);
    }
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

    if (getAsyncReset() && getEntryByName("async_reset") == 0) {
        ret += "    parameter bit async_reset = 1'b0";           // Mandatory generic parameter
        if (genparam.size()) {
            ret += ",";
        }
        ret += "\n";
        icnt++;
    }

    for (auto &p : genparam) {
        ln = "    parameter ";
        if (!p->isString()) {
            // vivado doesn't support string as parameter:
            ln += p->getType() + " ";
        }
        ln += p->getName() + " = " + p->generate();
        if (p != genparam.back()) {
            ln += ",";
        }
        p->addComment(ln);
        ret += ln + "\n";
    }

    return ret;
}

std::string ModuleObject::generate_sv_mod_param_strings() {
    std::string ret = "";
    int tcnt = 0;
    for (auto &p: getEntries()) {
        if (!p->isParam() || p->isParamGeneric()) {
            continue;
        }
        if (!p->isLocal() || !p->isGenericDep()) {
            continue;
        }
        if (p->isString()) {
            // Vivado doesn't support string parameters
            ret += "localparam " + p->getName();
        } else {
            ret += "localparam " + p->getType() + " " + p->getName();
        }
        ret += " = " + p->generate() + ";\n";

        tcnt++;
    }
    for (auto &p: getEntries()) {
        if (p->getId() != ID_ARRAY_STRING) {
            continue;
        }
        ret += "localparam string " + p->getName() + "[0: " + p->getStrDepth() +"-1]";
        ret += " = '{\n";
        for (auto &e: p->getEntries()) {
            ret += "    \"" + e->getName() + "\"";
            if (e != p->getEntries().back()) {
                ret += ",";
            }
            ret += "\n";
        }
        ret += "};\n";
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
            // todo: struct_def should be skipped (mark it as a typedef true). 
            SHOW_ERROR("Unnamed entry of type %s", p->getType().c_str());
            text = "";
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
        ln = addspaces() + p->getType() + " " + p->getName();
        if (p->getDepth() && !p->isVector()) {
            ln += "[0: " + p->getStrDepth() + " - 1]";
        }
        ln += ";";
        p->addComment(ln);
        ret += ln + "\n";
        tcnt++;
    }
    for (auto &p: getEntries()) {
        if (p->getId() != ID_FILEVALUE) {
            continue;
        }
        ret += "int " + p->getName() + ";\n";
        tcnt++;
    }

    if (isRegs() && isCombProcess()) {
        ret += getType() + "_registers r, rin;\n";
        tcnt++;
    }
    if (isNRegs() && isCombProcess()) {
        ret += getType() + "_nregisters nr, nrin;\n";
        tcnt++;
    }
    if (tcnt) {
        ret += "\n";
        tcnt = 0;
    }
    return ret;
}

std::string ModuleObject::generate_sv_mod_proc_nullify(GenObject *obj,
                                                       std::string prefix,
                                                       std::string i) {
    std::string ret = "";
    if (obj->getId() != ID_VALUE
        && !(obj->isStruct() && !obj->isTypedef())) {
        return ret;
    }

    if (prefix.size()) {
        prefix += ".";
    }
    prefix += obj->getName();

    if (obj->getDepth() > 1) {
        prefix += "[" + i + "]";
        ret += addspaces();
        ret += "for (int " + i + " = 0; " + i + " < " + obj->getStrDepth() + "; " + i + "++) begin\n";
        pushspaces();
    }
    if (obj->isStruct() && obj->getStrValue().size() == 0) {
        // Initialization of struct each field separetly:
        const char tidx[2] = {i.c_str()[0] + 1, 0};
        i = std::string(tidx);
        for (auto &e: obj->getEntries()) {
            ret += generate_sv_mod_proc_nullify(e, prefix, i);
        }
    } else {
        ret += addspaces() + prefix + " = " + obj->getStrValue() + ";\n";
    }

    if (obj->getDepth() > 1) {
        popspaces();
        ret += addspaces() + "end\n";
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
        ln = "";
        if (e->getId() == ID_VALUE
            || e->isStruct()) {    // no structure inside of process, typedef can be removed
            ln += addspaces() + e->getType() + " " + e->getName();
            if (e->getDepth() > 1) {
                ln += "[0: " + e->getStrDepth() + "-1]";
            }
            tcnt++;
        } else if (e->getId() == ID_ARRAY_DEF) {
            // DELME:
            ln += addspaces() + e->getType() + " " + e->getName();
            ln += "[0: ";
            ln += e->getStrDepth();
            ln += "-1]";
        } else {
            continue;
        }
        tcnt++;
        ln += ";";
        e->addComment(ln);
        ret += ln + "\n";
    }
    if (tcnt) {
        ret += "\n";
        tcnt = 0;
    }

    // nullify all local variables to avoid latches:
    size_t ret_sz = ret.size();
    for (auto &e: proc->getEntries()) {
        ret += generate_sv_mod_proc_nullify(e, "", "i");
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

std::string ModuleObject::generate_sv_mod_always_ff_rst(bool clkpos) {
    std::string out = "";
    std::string xrst = "";
    std::string src = "rin";
    std::string dst = "r";
    std::string blkname = ": rg_proc";  // to minimiz differences. Could be removed later
    bool isregs;
    out += addspaces() + "always_ff @(";
    if (clkpos) {
        out += "posedge ";
        isregs = isRegs();
    } else {
        out += "negedge ";
        src = "nrin";
        dst = "nr";
        blkname = "";
        isregs = isNRegs();
    }
    out += getClockPort()->getName();
    if (getResetPort()) {
        out += ", ";
        if (!getResetActive()) {
            out += "negedge ";
        } else {
            out += "posedge ";
        }
        out += getResetPort()->getName();
    }
    out += ") begin" + blkname + "\n";
    pushspaces();
    if (isCombProcess() &&  isregs) {
        if (getResetPort()) {
            out += Operation::reset(dst.c_str(), 0, this, xrst);
            out += " else begin\n";
            pushspaces();
            out += Operation::copyreg(dst.c_str(), src.c_str(), this);
            popspaces();
            out += addspaces() + "end\n";
        } else {
            out += Operation::copyreg(dst.c_str(), src.c_str(), this);
        }
    }
    // additional operation on posedge clock events
    out += generate_sv_mod_always_ops();
    popspaces();
    out += addspaces() + "end" + blkname + "\n";
    return out; 
}

// additional operations on posedge clock events
std::string ModuleObject::generate_sv_mod_always_ops() {
    std::string out = "";
    for (auto &e: getEntries()) {
        if (e->getId() != ID_PROCESS
            || e->getName() != "registers") {
            continue;
        }
        out += "\n";
        for (auto &r: e->getEntries()) {
            out += r->generate();
        }
    }
    return out;
}

std::string ModuleObject::generate_sv_mod_proc_registers() {
    std::string out = "";
    std::string xrst = "";

    if (getAsyncReset() && getResetPort()) {
        // Need to generate both cases: always with and without reset
        out += "generate\n";
        pushspaces();
        out += addspaces() + "if (async_reset) begin: async_rst_gen\n";
        pushspaces();
        out += "\n";
    }
    if (getResetPort()) {
        out += generate_sv_mod_always_ff_rst(true);
        out += "\n";
        if (isNRegProcess()) {
            out += generate_sv_mod_always_ff_rst(false);
            out += "\n";
        }
    }
    if (getAsyncReset() && getResetPort()) {
        popspaces();
        out += "\n";
        out += addspaces() + "end: async_rst_gen\n";
        out += addspaces() + "else begin: no_rst_gen\n";
        pushspaces();
        out += "\n";
    }

    if (getAsyncReset() || getResetPort() == 0) {
        out += addspaces() + "always_ff @(posedge ";
        out += getClockPort()->getName();
        out += ") begin: rg_proc\n";
        pushspaces();
        if (isRegs() && isCombProcess()) {
            out += Operation::copyreg("r", "rin", this);
        }
        out += generate_sv_mod_always_ops();   // additional operation on posedge clock events
        popspaces();
        out += addspaces() + "end: rg_proc\n";
        out += "\n";
    }

    if (getAsyncReset() && getResetPort()) {
        popspaces();
        out += addspaces() + "end: no_rst_gen\n";
        popspaces();
        out += "endgenerate\n";
        out += "\n";
    }
    return out;
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
            if (p->getId() == ID_COMMENT) {
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
        strtype = p->getType();
        if (SCV_is_cfg_parameter(strtype) && SCV_get_cfg_file(strtype).size()) {
            // whole types (like vectors or typedef)
            ln += SCV_get_cfg_file(strtype) + "_pkg::" + strtype;
        } else {
            // Width or depth definitions
            ln += strtype;
        }
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
    for (auto &p: entries_) {
        if (p->getId() != ID_FUNCTION) {
            continue;
        }
        ret += generate_sv_mod_func(p);
    }

    // Sub-module instantiation
    for (auto &p: entries_) {
        if (p->isOperation()) {
            ret += p->generate();
            ret += "\n";        // No need in this.
        }
    }

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


    ret += "endmodule: " + getType() + "\n";


    return ret;
}

}
