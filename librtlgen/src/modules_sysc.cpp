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

namespace sysvc {

std::string ModuleObject::generate_sysc_h_reg_struct(bool negedge) {
    std::string out = "";
    std::string ln = "";
    bool twodim = false;        // if 2-dimensional register array, then do not use reset function
    out += "    struct " + getType();
    if (!negedge) {
        out += "_registers";
    } else {
        out += "_nregisters";
    }
    out += " {\n";
    for (auto &p: getEntries()) {
        if (!negedge && !p->isReg()) {
            continue;
        }
        if (negedge && !p->isNReg()) {
            continue;
        }
        ln = "        ";
        if (p->isSignal()) {
            // some structure are not defined as a signal but probably should be
            ln += "sc_signal<";
        }
        ln += p->getType();
        if (p->isSignal()) {
            ln += ">";
        }
        ln += " " + p->getName();
        if (p->getDepth()) {
            twodim = true;
            ln += "[" + p->getStrDepth() + "]";
        }
        ln += ";";
        p->addComment(ln);
        out += ln + "\n";
    }
    if (!negedge) {
        out += "    } v, r;\n";
    } else {
        out += "    } nv, nr;\n";
    }
    out += "\n";
    // Reset function only if no two-dimensial signals
    if (!twodim) {
        out += "    void " + getType();
        if (!negedge) {
            out += "_r_reset(" + getType() + "_registers &iv) {\n";
        } else {
            out += "_nr_reset(" + getType() + "_nregisters &iv) {\n";
        }
        for (auto &p: entries_) {
            if (!negedge && !p->isReg()) {
                continue;
            }
            if (negedge && !p->isNReg()) {
                continue;
            }
            out += "        iv." + p->getName() + " = ";
            out += p->getStrValue();
            // TODO: add "_" for Generic value
            out += ";\n";
        }
        out += "    }\n";
        out += "\n";
    }
    return out;
}

std::string ModuleObject::generate_sysc_h_struct() {
    std::string out = "";
    int tcnt = 0;
    for (auto &p: entries_) {
        if (p->isStruct() && p->isTypedef()) {
            out += p->generate();
            tcnt++;
        }
    }
    if (tcnt) {
        out += "\n";
        tcnt = 0;
    }
    // Register structure definition
    if (isRegs() && isCombProcess()) {
        out += generate_sysc_h_reg_struct(false);
    }
    if (isNRegs() && isCombProcess()) {
        out += generate_sysc_h_reg_struct(true);
    }
    return out;
}

std::string ModuleObject::generate_sysc_h() {
    std::string out = "";
    std::string ln;
    std::string text = "";
    std::list<GenObject *> tmpllist;
    std::list<GenObject *> argslist;
    int tcnt = 0;

    getTmplParamList(tmpllist);
    if (tmpllist.size()) {
        ln = "template<";
        for (auto &e: tmpllist) {
            if (e != tmpllist.front()) {
                ln += "         ";
            }
            ln += e->getType() + " " + e->getName() + " = " + e->generate();
            if (e != tmpllist.back()) {
                ln += ",";
            } else {
                ln += ">";
            }
            e->addComment(ln);
            out += ln + "\n";
            ln = "";
        }
    }

    tcnt = 0;
    out += "SC_MODULE(" + getType() + ") {\n";
    out += " public:\n";
    pushspaces();

    // Input/Output signal declaration
    for (auto &p: entries_) {
        if (!p->isInput() && !p->isOutput()) {
            if (p->getId() == ID_COMMENT) {
                text += p->generate();
            } else {
                text = "";
            }
            continue;
        }
        out += text;
        text = "";
        ln = addspaces();
        if (p->isVector()) {
            ln += "sc_vector<";
        }
        if (p->isInput() && p->isOutput()) {
            ln += "sc_inout<";
        } else if (p->isInput()) {
            ln += "sc_in<";
        } else {
            ln += "sc_out<";
        }
        if (p->isVector()) {
            ln += p->getTypedef();
        } else {
            ln += p->getType();
        }
        ln += ">";
        if (p->isVector()) {
            ln += ">";
        }
        ln += " " + p->getName() + ";";
        p->addComment(ln);
        out += ln + "\n";
    }

    // Process declaration:
    out += "\n";
    bool hasProcess = false;
    for (auto &p: entries_) {
        if (p->getId() != ID_PROCESS) {
            continue;
        }
        if (p->getName() == "registers") {
            continue;
        }
        out += addspaces() + "void " + p->getName() + "();\n";
        hasProcess = true;
    }
    if (isRegProcess()) {
        out += addspaces() + "void registers();\n";
        hasProcess = true;
    }
    if (isNRegProcess()) {
        out += addspaces() + "void nregisters();\n";
        hasProcess = true;
    }
    if (hasProcess) {
        out += "\n";
        out += addspaces() + "SC_HAS_PROCESS(" + getType() + ");\n";
    }


    out += "\n";
    // Constructor declaration:
    std::string space1 = addspaces() + getType() + "(";
    out += space1 + "sc_module_name name";
    if (getAsyncReset() && getEntryByName("async_reset") == 0) {
        ln = "";
        while (ln.size() < space1.size()) {
            ln += " ";
        }
        ln += "bool async_reset";           // Mandatory generic parameter
        out += ",\n" + ln;
    }
    for (auto &p: getEntries()) {
        if (!p->isParamGeneric() || p->isParamTemplate()) {
            continue;
        }
        out += ",\n";
        ln = "";
        while (ln.size() < space1.size()) {
            ln += " ";
        }
        ln += p->getType();
        ln += " " + p->getName();
        out += ln;
    }
    out += ");\n";
    // Destructor declaration
    if (isSubModules()) {
        out += addspaces() + "virtual ~" + getType() + "();\n";
    }
    out += "\n";

    // VCD generator
    if (isVcd()) {
        out += addspaces()
            + "void generateVCD(sc_trace_file *i_vcd, sc_trace_file *o_vcd);\n";
    }

    out += "\n";
    popspaces();
    out += addspaces() + " private:\n";
    pushspaces();

    // Generic parameter local storage:
    tcnt = 0;
    if (getAsyncReset() && getEntryByName("async_reset") == 0) {
        out += addspaces() + (new Logic())->getType() + " async_reset_;\n";
        tcnt++;
    }
    for (auto &p: entries_) {
        if (p->isParamTemplate()) {
            // do nothing
        } else if (p->isParamGeneric()) {
            out += addspaces() + p->getType() + " " + p->getName() + "_;\n";
            tcnt++;
        } else if (p->isParam() && p->isGenericDep() && tmpllist.size() == 0) {
            // No underscore symbol
            out += addspaces() + p->getType() + " " + p->getName() + ";\n";
            tcnt++;
        }
    }
    if (tcnt) {
        out += "\n";
        tcnt = 0;
    }

    // Local paramaters visible inside of module
    std::string comment = "";
    for (auto &p: entries_) {
        if (p->getId() == ID_COMMENT) {
            comment += p->generate();
            continue;
        } else if (p->isParam() && !p->isParamGeneric()) {
            if (p->isString()) {
                // Do Nothing: strings defined in cpp-file
            } else if (p->isGenericDep() && tmpllist.size() == 0) {
                // Do nothing: This parameter depends of generic parameter (constrcutor argument)
            } else {
                out += comment;
                out += addspaces() + "static const " + p->getType() + " ";
                out += p->getName() + " = " + p->generate() + ";\n";
                tcnt++;
            }
        }
        comment = "";
    }
    if (tcnt) {
        out += "\n";
        tcnt = 0;
    }

    // Functions declaration:
    tcnt = 0;
    for (auto &p: entries_) {
        if (p->getId() != ID_FUNCTION) {
            continue;
        }
        tcnt++;
        ln = addspaces() + p->getType();
        ln += " " + p->getName();
        out += ln + "(";
        tcnt = 0;
        argslist.clear();
        static_cast<FunctionObject *>(p)->getArgsList(argslist);
        for (auto &io: argslist) {
            if (tcnt++) {
                out += ", ";
            }
            out += io->getType() + " " + io->getName();
        }
        out += ");\n";
    }
    if (tcnt) {
        out += "\n";
        tcnt = 0;
    }

    // struct definitions
    out += generate_sysc_h_struct();

    // Signals list
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
            SHOW_ERROR("Error: unnamed object of type %s", p->getType().c_str());
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
            out += text;
            text = "";
        }
        ln = addspaces();
        if (p->isSignal() && !p->isVector()) {
            ln += "sc_signal<";
        }
        ln += p->getType();
        if (p->isSignal() && !p->isVector()) {
            ln += ">";
        }
        ln += " " + p->getName();
        if (p->getDepth() && !p->isVector()) {
            ln += "[" + p->getStrDepth() + "]";
        }
        ln += ";";
        p->addComment(ln);
        out += ln + "\n";
        tcnt++;
    }
    for (auto &p: getEntries()) {
        if (p->getId() != ID_FILEVALUE) {
            continue;
        }
        out += addspaces() + "FILE *" + p->getName() + ";\n";
        tcnt++;
    }
    if (tcnt) {
        out += "\n";
        tcnt = 0;
    }

    // Sub-module list
    for (auto &p: getEntries()) {
        if (p->isModule()) {
            out += addspaces() + p->getType();
            out += generate_sysc_template_param(p);
            out += " *" + p->getName();
            if (p->getDepth() > 1) {
                out += "[" + p->getStrDepth() + "]";
            }
            out += ";\n";
            tcnt ++;
        }
    }
    if (tcnt) {
        out += "\n";
        tcnt = 0;
    }

    popspaces();
    out += 
        "};\n"
        "\n";

    // Templates only. Generated in h-file
    if (tmpllist.size()) {
        out += generate_sysc_cpp();
    }
    return out;
}

std::string ModuleObject::generate_sysc_proc_registers(bool clkpos) {
    std::string out = "";
    std::string xrst = "";
    std::string src = "v";
    std::string dst = "r";
    bool isregs;

    out += generate_sysc_template_f_name();
    if (clkpos) {
        out += "::registers() {\n";
        isregs = isRegs();
    } else {
        out += "::nregisters() {\n";
        src = "nv";
        dst = "nr";
        isregs = isNRegs();
    }
    pushspaces();
    if (isregs) {
        if (getResetPort()) {
            if (isCombProcess()) {
                out += Operation::reset(dst.c_str(), 0, this, xrst);
            }
            out += " else {\n";
            pushspaces();
        }
        if (isCombProcess()) {
            out += Operation::copyreg(dst.c_str(), src.c_str(), this);
        }
        if (getResetPort()) {
            popspaces();
            out += addspaces();
            out += "}\n";
        }
    }
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
    popspaces();
    out += addspaces();
    out += "}\n";
    out += "\n";
    return out;
}

std::string ModuleObject::generate_sysc_sensitivity(std::string prefix,
                                                    std::string name, 
                                                    GenObject *obj) {
    std::string ret = "";
    bool prefix_applied = true;

    if ((obj->isModule() && !obj->isTypedef())  // first call from file: module cannot include typedef of other modules, so we here only once excluding child module instances
        || (obj->isStruct() && obj->isTypedef())
        || obj->getId() == ID_PROCESS
        || (obj->isOutput() && !obj->isInput())) {
        return ret;
    }

    for (int i = 0; i < prefix.size(); i++) {
        if (i >= name.size()
            || prefix.c_str()[i] != name.c_str()[i]) {
            prefix_applied = false;
            break;
        }
    }

    if ((obj->isSignal() && obj->getParent()->getId() != ID_ARRAY_DEF)    // signal is a part of array not a child structure
        || obj->getId() == ID_ARRAY_DEF
        || (obj->isStruct() && !obj->isTypedef())) {
        if (name.size()) {
            name += ".";
        }
        name += obj->getName();
        if (obj->isReg() && !prefix_applied) {
            name = prefix + name;
        }
        if (obj->isNReg() && !prefix_applied) {
            // @warning: Structure is not supported
            name = "n" + prefix + name;
        }
    }
    if (obj->isInput() && obj->getName() != "i_clk") {
        ret += addspaces();
        if (obj->isVector()) {
            ret += "for (int i = 0; i < " + obj->getItem()->getStrDepth() + "; i++) {\n";
            pushspaces();
            ret += addspaces();
            ret += "sensitive << " + obj->getName() + "[i];\n";
            popspaces();
            ret += addspaces();
            ret += "}\n";
        } else {
            ret += "sensitive << " + obj->getName() + ";\n";
        }
    } else if  (obj->isSignal()) {
        ret += addspaces();
        if (obj->isVector() || obj->getId() == ID_ARRAY_DEF) {
            ret += "for (int i = 0; i < " + obj->getStrDepth() + "; i++) {\n";
            pushspaces();
            ret += addspaces();
            ret += "sensitive << " + name + "[i];\n";
            popspaces();
            ret += addspaces();
            ret += "}\n";
        } else {
            ret += "sensitive << " + name + ";\n";
        }
    } else if  (obj->getId() == ID_CLOCK) {
        ret += addspaces();
        ret += "sensitive << " + obj->getName() + ";\n";
    } else if (obj->getId() == ID_ARRAY_DEF
        && (obj->getItem()->getId() != ID_VALUE || obj->getItem()->isSignal())) {
        // ignore value (not signals) declared in module scope
        name += "[i]";
        ret += addspaces();
        ret += "for (int i = 0; i < " + obj->getStrDepth() + "; i++) {\n";
        GenObject *item = obj->getItem();
        pushspaces();
        if (item->getEntries().size() == 0) {
            ret += generate_sysc_sensitivity(prefix, name, item);
        } else {
            for (auto &s: item->getEntries()) {
                ret += generate_sysc_sensitivity(prefix, name, s);
            }
        }
        popspaces();
        ret += addspaces();
        ret += "}\n";
    } else {
        for (auto &s: obj->getEntries()) {
            ret += generate_sysc_sensitivity(prefix, name, s);
        }
    }
    return ret;
}

std::string ModuleObject::generate_sysc_vcd_entries(std::string name1, std::string name2, GenObject *obj) {
    std::string ret = "";
    bool prefix_applied = true;
    if ((obj->isStruct() && obj->isTypedef())
        || (obj->isModule() && !obj->isTypedef())
        || obj->getId() == ID_PROCESS
        || obj->isVector()) {
        return ret;
    }

    if (!isTop()
        && (obj->getName() == "i_clk" || obj->getName() == "i_nrst")) {
        return ret;
    }


    if (name1.size() < 2
        || name1.c_str()[0] != 'r' || name1.c_str()[1] != '.') {
        prefix_applied = false;
    }

    if (obj->getParent()->getId() == ID_ARRAY_DEF) {
        name1 += "[";
        if (obj->getSelector()) {
            name1 += obj->getSelector()->getName();
        } else {
            name1 += obj->getName();
        }
        name1 += "]";
    } else if ((obj->isSignal() && obj->getParent()->getId() != ID_ARRAY_DEF)    // signal is a part of array not a child structure
        || obj->getId() == ID_ARRAY_DEF
        || (obj->isStruct() && !obj->isTypedef())) {
        if (name1.size()) {
            name1 += ".";
            name2 += "_";
        }
        name1 += obj->getName();
        name2 += obj->getName();
        if (obj->isReg() && !prefix_applied) {
            name1 = "r." + name1;
            name2 = ".r_" + name2;
        }
        if (obj->isNReg() && !prefix_applied) {
            name1 = "nr." + name1;
            name2 = ".nr_" + name2;
        }
    }

    if (!obj->isVcd()) {
        // skip it
    } else if (obj->isInput() || obj->isOutput()) {
        ret += addspaces();
        ret += "sc_trace(o_vcd, " + obj->getName() + ", " + obj->getName() + ".name());\n";
    } else if (obj->getId() == ID_ARRAY_DEF && (obj->isReg() || obj->isNReg())) {
        (*obj->getEntries().begin())->setSelector(new ParamI32D(0, "i", "0"));
        name2 += "%d";
        ret += addspaces();
        ret += "for (int i = 0; i < " + obj->getStrDepth() + "; i++) {\n";
        std::list<GenObject *>::iterator it = obj->getEntries().begin();
        pushspaces();

        ret += addspaces();
        ret += "char tstr[1024];\n";
        ret += generate_sysc_vcd_entries(name1, name2, *it);

        popspaces();
        ret += addspaces();
        ret += "}\n";
    } else if (obj->isSignal() && (obj->isReg() || obj->isNReg())) {
        ret += addspaces();
        if (obj->getParent()->getId() == ID_ARRAY_DEF
            || obj->getParent()->getParent()->getId() == ID_ARRAY_DEF) {
            ret += "RISCV_sprintf(tstr, sizeof(tstr), \"%s" + name2 + "\", pn.c_str(), i);\n";
            ret += addspaces() + "sc_trace(o_vcd, " + name1 + ", tstr);\n";
        } else {
            ret += "sc_trace(o_vcd, " + name1 + ", pn + \"" + name2 + "\");\n";
        }
    } else {
        for (auto &s: obj->getEntries()) {
            ret += generate_sysc_vcd_entries(name1, name2, s);
        }
    }

    return ret;
}

std::string ModuleObject::generate_sysc_template_param(GenObject *p) {
    std::string ret = "";
    int tcnt = 0;
    std::list<GenObject *> tmpllist;

    static_cast<ModuleObject *>(p)->getTmplParamList(tmpllist);
    if (tmpllist.size()) {
        ret += "<";
        for (auto &e: tmpllist) {
            if (tcnt++) {
                ret += ", ";
            }
            ret += e->generate();
        }
        ret += ">";
    }
    return ret;
}

std::string ModuleObject::generate_sysc_template_f_name(const char *rettype) {
    std::string ret = "";
    int tcnt = 0;
    std::list<GenObject *> tmpllist;

    getTmplParamList(tmpllist);
    if (tmpllist.size()) {
        ret += addspaces();
        ret += "template<";
        for (auto &e: tmpllist) {
            if (tcnt++) {
                ret += ", ";
            }
            ret += e->getType() + " " + e->getName();
        }
        ret += ">\n";
    }
    tcnt = 0;

    ret += addspaces();
    ret += std::string(rettype);
    if (rettype[0]) {
        ret += " ";
    }
    ret += getType();
    if (tmpllist.size()) {
        ret += "<";
        for (auto &e: tmpllist) {
            if (tcnt++) {
                ret += ", ";
            }
            ret += e->getName();
        }
        ret += ">";
    }
    return ret;
}

std::string ModuleObject::generate_sysc_param_strings() {
    std::string ret = "";
    int tcnt = 0;
    for (auto &p: getEntries()) {
        if (p->isParam() && !p->isParamGeneric() && p->isLocal() && p->isString()) {
            // Only string parameter defined inside of this module
            ret += "static " + p->getType() + " " + p->getName();
            ret += " = " + p->generate() + ";\n";
            tcnt++;
        }
    }
    for (auto &p: getEntries()) {
        if (p->getId() != ID_ARRAY_STRING) {
            continue;
        }
        ret += "static " + p->getType() + " " + p->getName() + "[" + p->getStrDepth() +"]";
        ret += " = {\n";
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

std::string ModuleObject::generate_sysc_constructor() {
    std::string ret = "";
    std::string ln = "";
    std::list<GenObject *> tmpllist;
    int tcnt = 0;

    getTmplParamList(tmpllist);
    if (tmpllist.size()) {
        ret += addspaces();
        ret += "template<";
        for (auto &e: tmpllist) {
            if (tcnt++) {
                ret += ", ";
            }
            ret += e->getType() + " " + e->getName();
        }
        ret += ">\n";
    }
    tcnt = 0;

    ret += addspaces();
    std::string space1 = getType();
    if (tmpllist.size()) {
        space1 += "<";
        for (auto &e: tmpllist) {
            if (tcnt++) {
                space1 += ", ";
            }
            space1 += e->getName();
        }
        space1 += ">";
    }
    space1 += "::" + getType() + "(";
    ret += space1 + "sc_module_name name";
    if (getAsyncReset() && getEntryByName("async_reset") == 0) {
        ln = "";
        while (ln.size() < space1.size()) {
            ln += " ";
        }
        ln += "bool async_reset";           // Mandatory generic parameter
        ret += ",\n" + ln;
    }
    for (auto &p: getEntries()) {
        if (!p->isParamGeneric() || p->isParamTemplate()) {
            continue;
        }
        ret += ",\n";
        ln = "";
        while (ln.size() < space1.size()) {
            ln += " ";
        }
        ln += p->getType();
        ln += " " + p->getName();
        ret += ln;
    }
    ret += ")\n";
    ret += "    : sc_module(name)";
    // Input/Output signal declaration
    tcnt = 0;
    for (auto &p: getEntries()) {
        if (!p->isInput() && !p->isOutput()) {
            continue;
        }
        ret += ",\n    " + p->getName() + "(\"" + p->getName() + "\"";
        if (p->getItem()->isVector()) {
            ret += ", " + p->getItem()->getStrDepth();
        }
        ret += ")";
    }
    // Clock generators
    for (auto &p: getEntries()) {
        if (p->getId() != ID_CLOCK) {
            continue;
        }
        ret += ",\n    " + p->getName() + "(\"" + p->getName() + "\"";
        ret += ", " + p->getItem()->getStrValue();
        ret += ", SC_SEC";
        ret += ")";
    }
    // Signal Vectors (simple signals not initialized) also should be initialized
    for (auto &p: getEntries()) {
        if (p->isInput() || p->isOutput()) {
            continue;
        }
        if (!p->isVector() || p->getName() == "") {
            continue;
        }
        ret += ",\n    " + p->getName() + "(\"" + p->getName() + "\"";
        ret += ", " + p->getStrDepth();
        ret += ")";
    }
    ret += " {\n";
    ret += "\n";
    pushspaces();
    // local copy of the generic parameters:
    if (getAsyncReset() && getEntryByName("async_reset") == 0) {
        ret += addspaces() + "async_reset_ = async_reset;\n";
    }
    for (auto &p: entries_) {
        if (p->isParamTemplate()) {
            // do nothing
        } else if (p->isParamGeneric()) {
            ret += addspaces() + p->getName() + "_ = " + p->getName() + ";\n";
        } else if (p->isParam() && p->isGenericDep() && tmpllist.size() == 0) {
            ret += addspaces() + p->getName() + " = " + p->generate() + ";\n";
        }
    }

    // Sub-module instantiation
    ret += generate_sysc_submodule_nullify();
    for (auto &p: getEntries()) {
        if (p->isOperation()) {
            ret += p->generate();
            ret += "\n";
        }
    }

    // Process sensitivity list:
    std::list<GenObject *> objlist;
    std::string prefix1 = "r.";
    for (auto &p: getEntries()) {
        if (p->getId() != ID_PROCESS) {
            continue;
        }
        if (p->getName() == "registers") {
            continue;
        }
        ret += "\n";
        ret += addspaces() + "SC_METHOD(" + p->getName() + ");\n";

        ln = std::string("");
        ret += generate_sysc_sensitivity(prefix1, ln, this);
    }
    if (isRegProcess()) {
        ret += "\n";
        ret += addspaces() + "SC_METHOD(registers);\n";
        if (getResetPort()) {
            ret += addspaces() + "sensitive << " + getResetPort()->getName() + ";\n";
        }
        if (getClockPort()->isInput()) {
            ret += addspaces() + "sensitive << " + getClockPort()->getName() + ".pos();\n";
        } else {
            ret += addspaces() + "sensitive << " + getClockPort()->getName() + ".posedge_event();\n";
        }
    }

    if (isNRegProcess()) {
        ret += "\n";
        ret += addspaces() + "SC_METHOD(nregisters);\n";
        if (getResetPort()) {
            ret += addspaces() + "sensitive << " + getResetPort()->getName() + ";\n";
        }
        if (getClockPort()->isInput()) {
            ret += addspaces() + "sensitive << " + getClockPort()->getName() + ".neg();\n";
        } else {
            ret += addspaces() + "sensitive << " + getClockPort()->getName() + ".negedge_event();\n";
        }
    }
    popspaces();
    ret += addspaces() + "}\n";
    ret += "\n";
    return ret;
}

std::string ModuleObject::generate_sysc_submodule_nullify() {
    std::string ret = "";
    int icnt = 0;

    for (auto &p: getEntries()) {
        if (p->isModule()) {
            if (p->getDepth() > 1) {
                ret += addspaces();
                ret += "for (int i = 0; i < " + p->getStrDepth() + "; i++) {\n";
                pushspaces();
                ret += addspaces() + p->getName() + "[i] = 0;\n";
                popspaces();
                ret += addspaces() + "}\n";
            } else {
                ret += addspaces() + "" + p->getName() + " = 0;\n";
            }
            icnt++;
        }
    }
    if (icnt) {
        ret += "\n";
    }
    return ret;
}


std::string ModuleObject::generate_sysc_destructor() {
    std::string ret = "";

    ret += generate_sysc_template_f_name("");
    ret += "::~" + getType() + "() {\n";
    pushspaces();
    for (auto &p: getEntries()) {
        if (p->isModule()) {
            std::string tidx = "";
            if (p->getDepth() > 1) {
                ret += addspaces();
                ret += "for (int i = 0; i < " + p->getStrDepth() + "; i++) {\n";
                pushspaces();
                tidx = "[i]";
            }

            ret += addspaces() + "if (" + p->getName() + tidx + ") {\n";
            pushspaces();
            ret += addspaces() + "delete " + p->getName() + tidx + ";\n";
            popspaces();
            ret += addspaces() + "}\n";

            if (p->getDepth() > 1) {
                popspaces();
                ret += addspaces() + "}\n";
            }
        }
    }
    popspaces();
    ret += "}\n";
    ret += "\n";
    return ret;
}

std::string ModuleObject::generate_sysc_vcd() {
    std::string ret = "";
    std::string ln = "";
    std::string ln2 = "";

    ret += generate_sysc_template_f_name();
    ret += "::generateVCD(sc_trace_file *i_vcd, sc_trace_file *o_vcd) {\n";
    pushspaces();
    if (isRegs() && isCombProcess()) {
        ret += addspaces() + "std::string pn(name());\n";
    }
    ret += addspaces() + "if (o_vcd) {\n";
    pushspaces();
    ret += generate_sysc_vcd_entries(ln, ln2, this);
    popspaces();
    ret += addspaces() + "}\n";
    ret += "\n";

    // Sub modules:
    for (auto &p: entries_) {
        if (p->isModule() && p->isVcd()) {
            std::string tidx = "";
            if (p->getDepth() > 1) {
                ret += addspaces();
                ret += "for (int i = 0; i < " + p->getStrDepth() + "; i++) {\n";
                pushspaces();
                tidx = "[i]";
            }
            ret += addspaces() + "if (" + p->getName() + tidx + ") {\n";
            pushspaces();
            ret += addspaces() + p->getName() + tidx + "->generateVCD(i_vcd, o_vcd);\n";
            popspaces();
            ret += addspaces() + "}\n";

            if (p->getDepth() > 1) {
                popspaces();
                ret += addspaces() + "}\n";
            }
        }
    }
    popspaces();
    ret += addspaces() + "}\n";
    ret += "\n";
    return ret;
}

std::string ModuleObject::generate_sysc_cpp() {
    std::string out = "";
    std::string ln;
    std::string text = "";

    // static strings
    out += generate_sysc_param_strings();

    // Constructor
    out += generate_sysc_constructor();

    // Destructor:
    if (isSubModules()) {
        out += generate_sysc_destructor();
    }

    // generateVCD function
    if (isVcd()) {
        out += generate_sysc_vcd();
    }

    // Functions
    for (auto &p: entries_) {
        if (p->getId() != ID_FUNCTION) {
            continue;
        }
        out += generate_sysc_func(p);
        out += "\n";
    }

    // Process
    for (auto &p: entries_) {
        if (p->getId() != ID_PROCESS) {
            continue;
        }
        if (p->getName() == "registers") {
            continue;
        }
        out += generate_sysc_proc(p);
    }

    if (isRegProcess()) {
        out += generate_sysc_proc_registers(true);
    }
    if (isNRegProcess()) {
        out += generate_sysc_proc_registers(false);
    }
    return out;
}

std::string ModuleObject::generate_sysc_func(GenObject *func) {
    std::string ret = "";
    ret += generate_sysc_template_f_name(func->getType().c_str()) + "::";
    ret += static_cast<FunctionObject *>(func)->generate();
    return ret;
}

std::string ModuleObject::generate_sysc_proc_nullify(GenObject *obj,
                                                     std::string prefix,
                                                     std::string i) {
    std::string ret = "";
    if (obj->getId() == ID_VALUE
        || (obj->isStruct() && !obj->isTypedef() && obj->getStrValue().size() != 0)) {
        ret += addspaces() + prefix;
        if (obj->getName() != "0") {
            if (prefix.size() != 0) {
                ret += ".";
            }
            ret += obj->getName();
        }
        ret += " = " + obj->getStrValue() + ";\n";
    } else if (obj->isStruct() && !obj->isTypedef()) {
        std::string prefix2 = prefix;
        if (obj->getName() != "0") {
            if (prefix.size()) {
                prefix2 += ".";
            }
            prefix2 += obj->getName();
        }
        for (auto &e: obj->getEntries()) {
            ret += generate_sv_mod_proc_nullify(e, prefix2, i);
        }
    } else if (obj->getId() == ID_ARRAY_DEF) {
        GenObject *item = obj->getItem();
        ret += addspaces();
        ret += "for (int " + i + " = 0; " + i + " < " + obj->getStrDepth() + "; " + i + "++) {\n";
        pushspaces();

        std::string prefix2 = prefix + obj->getName() + "[" + i + "]";
        const char tidx[2] = {i.c_str()[0], 0};
        std::string i2 = std::string(tidx);
        ret += generate_sv_mod_proc_nullify(item, prefix2, i2);

        popspaces();
        ret += addspaces() + "}\n";
    }
    return ret;
}

std::string ModuleObject::generate_sysc_proc(GenObject *proc) {
    std::string ret = "";
    std::string ln;
    int tcnt = 0;

    ret += generate_sysc_template_f_name();
    ret += "::" + proc->getName() + "() {\n";
    pushspaces();

    // process variables declaration
    tcnt = 0;
    for (auto &e: proc->getEntries()) {
        ln = "";
        if (e->getId() == ID_VALUE
            || e->isStruct()) {  // no need to check typedef inside of proc
            ln += addspaces() + e->getType() + " " + e->getName();
        } else if (e->getId() == ID_ARRAY_DEF) {
            ln += addspaces() + e->getType() + " " + e->getName();
            ln += "[";
            ln += e->getStrDepth();
            ln += "]";
        } else {
            continue;
        }
        tcnt++;
        ln += ";";
        if (e->getComment().size()) {
            while (ln.size() < 60) {
                ln += " ";
            }
            ln += "// " + e->getComment();
        }
        ret += ln + "\n";
    }
    if (tcnt) {
        ret += "\n";
        tcnt = 0;
    }

    // nullify all local variables to avoid latches:
    size_t ret_sz = ret.size();
    for (auto &e: proc->getEntries()) {
        ret += generate_sysc_proc_nullify(e, "", "i");
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
        tcnt = 0;
        ret += "\n";
    }

    // Generate operations:
    for (auto &e: proc->getEntries()) {
        if (e->isOperation()) {
            ret += e->generate();
        }
    }

    popspaces();
    ret += "}\n";
    ret += "\n";
    return ret;
}

}
