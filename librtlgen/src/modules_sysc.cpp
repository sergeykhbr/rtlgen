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
            ln += e->getType() + " " + e->getName() + " = " + e->getStrValue();
            if (e != tmpllist.back()) {
                ln += ",";
            } else {
                ln += ">";
            }
            if (e->getComment().size()) {
                while (ln.size() < 60) {
                    ln += " ";
                }
                ln += "// " + e->getComment();
            }
            out += ln + "\n";
            ln = "";
        }
    }

    tcnt = 0;
    out += "SC_MODULE(" + getType() + ") {\n";
    out += " public:\n";

    // Input/Output signal declaration
    for (auto &p: entries_) {
        if (p->getId() != ID_INPUT && p->getId() != ID_OUTPUT) {
            if (p->getId() == ID_COMMENT) {
                text = "    " + p->generate();
            } else {
                text = "";
            }
            continue;
        }
        ln = "";
        ln += "    " + p->getType();
        ln += " " + p->getName() + ";";
        if (p->getComment().size()) {
            while (ln.size() < 60) {
                ln += " ";
            }
            ln += "// " + p->getComment();
        }
        if (text.size()) {
            out += text;
        }
        out += ln + "\n";
        text = "";
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
        out += "    void " + p->getName() + "();\n";
        hasProcess = true;
    }
    if (isRegProcess()) {
        out += "    void registers();\n";
        hasProcess = true;
    }
    if (hasProcess) {
        out += "\n";
        out += "    SC_HAS_PROCESS(" + getType() + ");\n";
    }


    out += "\n";
    // Constructor declaration:
    std::string space1 = "    " + getType() + "(";
    out += space1 + "sc_module_name name";
    if (isAsyncReset()) {
        ln = "";
        while (ln.size() < space1.size()) {
            ln += " ";
        }
        ln += "bool async_reset";           // Mandatory generic parameter
        out += ",\n" + ln;
    }
    for (auto &p: entries_) {
        if (p->getId() != ID_DEF_PARAM) {
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
        out += "    virtual ~" + getType() + "();\n";
    }
    out += "\n";

    // VCD generator
    if (isVcd()) {
        out += "    void generateVCD(sc_trace_file *i_vcd, sc_trace_file *o_vcd);\n";
    }

    out += "\n";
    out += " private:\n";

    // Generic parameter local storage:
    tcnt = 0;
    if (isAsyncReset()) {
        out += "    " + (new Logic())->getType() + " async_reset_;\n";
        tcnt++;
    }
    for (auto &p: entries_) {
        if (p->getId() != ID_DEF_PARAM) {
            continue;
        }
        out += "    " + p->getType() + " " + p->getName() + "_;\n";
        tcnt++;
    }
    if (tcnt) {
        out += "\n";
        tcnt = 0;
    }

    // Local paramaters visible inside of module
    for (auto &p: entries_) {
        if (p->getId() != ID_PARAM) {
            continue;
        }
        if (!static_cast<GenValue *>(p)->isLocal()) {
            continue;
        }
        if (p->getType() == "std::string") {
            continue;
        } else {
            out += "    static const " + p->getType() + " " + p->getName();
            out += " = " + p->getStrValue() + ";\n";
        }
        tcnt++;
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
        ln = "    " + p->getType();
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
    Operation::set_space(1);
    for (auto &p: entries_) {
        if (p->getId() != ID_STRUCT_DEF) {
            continue;
        }
        out += p->generate();
        tcnt++;
    }
    if (tcnt) {
        out += "\n";
        tcnt = 0;
    }
    // Register structure definition
    bool twodim = false;        // if 2-dimensional register array, then do not use reset function
    if (isRegProcess()) {
        out += "    struct " + getType() + "_registers {\n";
        for (auto &p: entries_) {
            if (!p->isReg()) {
                continue;
            }
            ln = "        " + p->getType() + " " + p->getName();
            if (p->getDepth()) {
                twodim = true;
                ln += "[" + p->getStrDepth() + "]";
            }
            ln += ";";
            if (p->getComment().size()) {
                while (ln.size() < 60) {
                    ln += " ";
                }
                ln += "// " + p->getComment();
            }
            out += ln + "\n";
        }
        out += "    } v, r;\n";
        out += "\n";
        // Reset function only if no two-dimensial signals
        if (!twodim) {
            out += "    void " + getType() + "_r_reset(" + getType() + "_registers &iv) {\n";
            for (auto &p: entries_) {
                if (!p->isReg()) {
                    continue;
                }
                out += "        iv." + p->getName() + " = ";
                out += p->getStrValue();
                out += ";\n";
            }
            out += "    }\n";
            out += "\n";
        }
    }


    // Signals list
    text = "";
    for (auto &p: getEntries()) {
        if (p->isReg() || (p->getId() != ID_SIGNAL
                        && p->getId() != ID_VALUE
                        && p->getId() != ID_STRUCT_INST
                        && p->getId() != ID_ARRAY_DEF)) {
            if (p->getId() == ID_COMMENT) {
                text += "    " + p->generate();
            } else {
                text = "";
            }
            continue;
        }
        if (p->getId() == ID_ARRAY_DEF) {
            ArrayObject *a = static_cast<ArrayObject *>(p);
            if (a->getItem()->getId() == ID_MODULE_INST) {
                text = "";
                continue;
            }
        }
        if (text.size()) {
            out += text;
            text = "";
        }
        ln = "    " + p->getType() + " " + p->getName();
        if (p->getDepth()) {
            ln += "[" + p->getStrDepth() + "]";
        }
        ln += ";";
        if (p->getComment().size()) {
            while (ln.size() < 60) {
                ln += " ";
            }
            ln += "// " + p->getComment();
        }
        out += ln + "\n";
        tcnt++;
    }
    for (auto &p: getEntries()) {
        if (p->getId() != ID_FILEVALUE) {
            continue;
        }
        out += "    FILE *" + p->getName() + ";\n";
        tcnt++;
    }
    if (tcnt) {
        out += "\n";
        tcnt = 0;
    }

    // Sub-module list
    for (auto &p: entries_) {
        if (p->getId() == ID_MODULE_INST) {
            out += "    " + p->getType();
            out += generate_sysc_template_param(p);
            out += " *" + p->getName() + ";\n";
            tcnt ++;
        } else if (p->getId() == ID_ARRAY_DEF) {
            ArrayObject *a = static_cast<ArrayObject *>(p);
            if (a->getItem()->getId() == ID_MODULE_INST) {
                out += "    " + a->getType();
                out += generate_sysc_template_param(a->getItem());
                out += " *" + p->getName();
                out += "[" + a->getStrDepth() + "];\n";
            }
            tcnt ++;
        }
    }
    if (tcnt) {
        out += "\n";
        tcnt = 0;
    }


    out += 
        "};\n"
        "\n";

    // Templates only. Generated in h-file
    if (tmpllist.size()) {
        out += generate_sysc_cpp();
    }
    return out;
}

std::string ModuleObject::generate_sysc_proc_registers() {
    std::string out = "";
    std::string xrst = "";

    out += generate_sysc_template_f_name();
    out += "::registers() {\n";
    Operation::set_space(Operation::get_space() + 1);
    if (isAsyncReset()) {
        out += Operation::reset("r", 0, this, xrst);
        out += " else {\n";
        Operation::set_space(Operation::get_space() + 1);
    }
    out += Operation::copyreg("r", "v", this);
    if (isAsyncReset()) {
        Operation::set_space(Operation::get_space() - 1);
        out += Operation::addspaces();
        out += "}\n";
    }
    for (auto &e: getEntries()) {
        if (e->getId() != ID_PROCESS || e->getName() != "registers") {
            continue;
        }
        out += "\n";
        for (auto &r: e->getEntries()) {
            out += r->generate();
        }
    }
    Operation::set_space(Operation::get_space() - 1);
    out += Operation::addspaces();
    out += "}\n";
    out += "\n";
    return out;
}

std::string ModuleObject::generate_sysc_sensitivity(std::string prefix,
                                                    std::string name, 
                                                    GenObject *obj) {
    std::string ret = "";
    bool prefix_applied = true;
    if (obj->getId() == ID_STRUCT_DEF
        || obj->getId() == ID_MODULE_INST
        || obj->getId() == ID_PROCESS) {
        return ret;
    }
    if (obj->getId() == ID_ARRAY_DEF) {
        // Check when array stores module instantiation:
        if (static_cast<ArrayObject *>(obj)->getItem()->getId() == ID_MODULE_INST) {
            return ret;
        }
    }

    for (int i = 0; i < prefix.size(); i++) {
        if (i >= name.size()
            || prefix.c_str()[i] != name.c_str()[i]) {
            prefix_applied = false;
            break;
        }
    }

    if ((obj->getId() == ID_SIGNAL && obj->getParent()->getId() != ID_ARRAY_DEF)    // signal is a part of array not a child structure
        || obj->getId() == ID_ARRAY_DEF
        || obj->getId() == ID_STRUCT_INST) {
        if (name.size()) {
            name += ".";
        }
        name += obj->getName();
        if (obj->isReg() && !prefix_applied) {
            name = prefix + name;
        }
    }

    if (obj->getId() == ID_INPUT && obj->getName() != "i_clk") {
        ret += Operation::addspaces();
        ret += "sensitive << " + obj->getName() + ";\n";
    } else if  (obj->getId() == ID_SIGNAL) {
        ret += Operation::addspaces();
        ret += "sensitive << " + name + ";\n";
    } else if (obj->getId() == ID_ARRAY_DEF
        && static_cast<ArrayObject *>(obj)->getItem()->getId() != ID_VALUE) {
        // ignore value (not signals) declared in module scope
        name += "[i]";
        ret += Operation::addspaces();
        ret += "for (int i = 0; i < " + obj->getStrDepth() + "; i++) {\n";
        GenObject *item = static_cast<ArrayObject *>(obj)->getItem();
        Operation::set_space(Operation::get_space() + 1);
        if (item->getEntries().size() == 0) {
            ret += generate_sysc_sensitivity(prefix, name, item);
        } else {
            for (auto &s: item->getEntries()) {
                ret += generate_sysc_sensitivity(prefix, name, s);
            }
        }
        Operation::set_space(Operation::get_space() - 1);
        ret += Operation::addspaces();
        ret += "}\n";
    } else if (obj->getId() == ID_STRUCT_INST) {
        for (auto &s: obj->getEntries()) {
            ret += generate_sysc_sensitivity(prefix, name, s);
        }
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
    if (obj->getId() == ID_STRUCT_DEF
        || obj->getId() == ID_MODULE_INST
        || obj->getId() == ID_PROCESS) {
        return ret;
    }

    // TODO: check top level module instead of hardcoded name
    if (getName() != "RiverTop"
        && (obj->getName() == "i_clk" || obj->getName() == "i_nrst")) {
        return ret;
    }


    if (name1.size() < 2
        || name1.c_str()[0] != 'r' || name1.c_str()[1] != '.') {
        prefix_applied = false;
    }

    if (obj->getParent()->getId() == ID_ARRAY_DEF) {
        name1 += "[";
        name1 += obj->getName();
        name1 += "]";
    } else if ((obj->getId() == ID_SIGNAL && obj->getParent()->getId() != ID_ARRAY_DEF)    // signal is a part of array not a child structure
        || obj->getId() == ID_ARRAY_DEF
        || obj->getId() == ID_STRUCT_INST) {
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
    }

    if (!obj->isVcd()) {
        // skip it
    } else if (obj->getId() == ID_INPUT || obj->getId() == ID_OUTPUT) {
        ret += Operation::addspaces();
        ret += "sc_trace(o_vcd, " + obj->getName() + ", " + obj->getName() + ".name());\n";
    } else if (obj->getId() == ID_ARRAY_DEF && obj->isReg()) {
        static_cast<ArrayObject *>(obj)->setSelector(new I32D("0", "i"));
        name2 += "%d";
        ret += Operation::addspaces();
        ret += "for (int i = 0; i < " + obj->getStrDepth() + "; i++) {\n";
        std::list<GenObject *>::iterator it = obj->getEntries().begin();
        Operation::set_space(Operation::get_space() + 1);

        ret += Operation::addspaces();
        ret += "char tstr[1024];\n";
        ret += generate_sysc_vcd_entries(name1, name2, *it);

        Operation::set_space(Operation::get_space() - 1);
        ret += Operation::addspaces();
        ret += "}\n";
    } else if (obj->getId() == ID_SIGNAL && obj->isReg()) {
        ret += Operation::addspaces();
        if (obj->getParent()->getId() == ID_ARRAY_DEF
            || obj->getParent()->getParent()->getId() == ID_ARRAY_DEF) {
            ret += "RISCV_sprintf(tstr, sizeof(tstr), \"%s" + name2 + "\", pn.c_str(), i);\n";
            ret += Operation::addspaces() + "sc_trace(o_vcd, " + name1 + ", tstr);\n";
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
            ret += e->getStrValue();
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
        ret += Operation::addspaces();
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

    ret += Operation::addspaces();
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
        if (p->getId() != ID_PARAM) {
            continue;
        }
        if (!static_cast<GenValue *>(p)->isLocal()) {
            continue;
        }
        if (p->getType() != "std::string") {
            continue;
        }
        ret += "static " + p->getType() + " " + p->getName();
        ret += " = " + p->getStrValue() + ";\n";

        tcnt++;
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
        ret += Operation::addspaces();
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

    ret += Operation::addspaces();
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
    if (isAsyncReset()) {
        ln = "";
        while (ln.size() < space1.size()) {
            ln += " ";
        }
        ln += "bool async_reset";           // Mandatory generic parameter
        ret += ",\n" + ln;
    }
    for (auto &p: entries_) {
        if (p->getId() != ID_DEF_PARAM) {
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
    for (auto &p: entries_) {
        if (p->getId() != ID_INPUT && p->getId() != ID_OUTPUT) {
            continue;
        }
        ret += ",\n    " + p->getName() + "(\"" + p->getName() + "\")";
    }
    if (tcnt == 0) {
        // not IO ports
        ret += " ";
    }
    ret += "{\n";
    ret += "\n";
    // local copy of the generic parameters:
    if (isAsyncReset()) {
        ret += "    async_reset_ = async_reset;\n";
    }
    for (auto &p: entries_) {
        if (p->getId() != ID_DEF_PARAM) {
            continue;
        }
        ret += "    " + p->getName() + "_ = " + p->getName() + ";\n";
    }

    // Sub-module instantiation
    Operation::set_space(1);
    ret += generate_sysc_submodule_nullify();
    for (auto &p: entries_) {
        if (p->getId() != ID_OPERATION) {
            continue;
        }
        ret += p->generate();
        ret += "\n";
    }

    // Process sensitivity list:
    std::list<GenObject *> objlist;
    std::string prefix1 = "r.";
    for (auto &p: entries_) {
        if (p->getId() != ID_PROCESS) {
            continue;
        }
        if (p->getName() == "registers") {
            continue;
        }
        ret += "\n";
        ret += "    SC_METHOD(" + p->getName() + ");\n";

        ln = std::string("");
        Operation::set_space(1);
        ret += generate_sysc_sensitivity(prefix1, ln, this);
    }
    if (isRegProcess()) {
        ret += "\n";
        ret += "    SC_METHOD(registers);\n";
        if (isAsyncReset()) {
            ret += "    sensitive << i_nrst;\n";
        }
        ret += "    sensitive << i_clk.pos();\n";
    }
    ret += "}\n";
    ret += "\n";
    return ret;
}

std::string ModuleObject::generate_sysc_submodule_nullify() {
    std::string ret = "";
    int icnt = 0;

    for (auto &p: entries_) {
        if (p->getId() == ID_MODULE_INST) {
            ret += Operation::addspaces() + "" + p->getName() + " = 0;\n";
            icnt++;
        } else if (p->getId() == ID_ARRAY_DEF) {
            ArrayObject *a = static_cast<ArrayObject *>(p);
            if (a->getItem()->getId() == ID_MODULE_INST) {
                icnt++;
                ret += Operation::addspaces();
                ret += "for (int i = 0; i < " + a->getStrDepth() + "; i++) {\n";
                Operation::set_space(Operation::get_space() + 1);
                ret += Operation::addspaces() + p->getName() + "[i] = 0;\n";
                Operation::set_space(Operation::get_space() - 1);
                ret += Operation::addspaces() + "}\n";
            }
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
    Operation::set_space(Operation::get_space() + 1);
    for (auto &p: entries_) {
        if (p->getId() == ID_MODULE_INST) {
            ret += Operation::addspaces() + "if (" + p->getName() + ") {\n";
            Operation::set_space(Operation::get_space() + 1);
            ret += Operation::addspaces() + "delete " + p->getName() + ";\n";
            Operation::set_space(Operation::get_space() - 1);
            ret += Operation::addspaces() + "}\n";
        } else if (p->getId() == ID_ARRAY_DEF) {
            ArrayObject *a = static_cast<ArrayObject *>(p);
            if (a->getItem()->getId() == ID_MODULE_INST) {
                ret += Operation::addspaces();
                ret += "for (int i = 0; i < " + a->getStrDepth() + "; i++) {\n";
                Operation::set_space(Operation::get_space() + 1);
                ret += Operation::addspaces() + "if (" + p->getName() + "[i]) {\n";
                Operation::set_space(Operation::get_space() + 1);
                ret += Operation::addspaces() + "delete " + p->getName() + "[i];\n";
                Operation::set_space(Operation::get_space() - 1);
                ret += Operation::addspaces() + "}\n";
                Operation::set_space(Operation::get_space() - 1);
                ret += Operation::addspaces() + "}\n";
            }
        }
    }
    Operation::set_space(Operation::get_space() - 1);
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
    Operation::set_space(Operation::get_space() + 1);
    if (isRegProcess()) {
        ret += Operation::addspaces() + "std::string pn(name());\n";
    }
    ret += Operation::addspaces() + "if (o_vcd) {\n";
    Operation::set_space(Operation::get_space() + 1);
    ret += generate_sysc_vcd_entries(ln, ln2, this);
    Operation::set_space(Operation::get_space() - 1);
    ret += Operation::addspaces() + "}\n";
    ret += "\n";

    // Sub modules:
    for (auto &p: entries_) {
        if (p->getId() == ID_MODULE_INST && p->isVcd()) {
            ret += Operation::addspaces() + "if (" + p->getName() + ") {\n";
            Operation::set_space(Operation::get_space() + 1);
            ret += Operation::addspaces() + p->getName() + "->generateVCD(i_vcd, o_vcd);\n";
            Operation::set_space(Operation::get_space() - 1);
            ret += Operation::addspaces() + "}\n";
        } else if (p->getId() == ID_ARRAY_DEF) {
            ArrayObject *a = static_cast<ArrayObject *>(p);
            if (a->getItem()->getId() == ID_MODULE_INST && a->isVcd()) {
                ret += Operation::addspaces();
                ret += "for (int i = 0; i < " + a->getStrDepth() + "; i++) {\n";
                Operation::set_space(Operation::get_space() + 1);
                ret += Operation::addspaces() + "if (" + p->getName() + ") {\n";
                Operation::set_space(Operation::get_space() + 1);
                ret += Operation::addspaces();
                ret += p->getName() + "[i]->generateVCD(i_vcd, o_vcd);\n";
                Operation::set_space(Operation::get_space() - 1);
                ret += Operation::addspaces() + "}\n";
                Operation::set_space(Operation::get_space() - 1);
                ret += Operation::addspaces() + "}\n";
            }
        }
    }
    Operation::set_space(Operation::get_space() - 1);
    ret += Operation::addspaces() + "}\n";
    ret += "\n";
    return ret;
}

std::string ModuleObject::generate_sysc_cpp() {
    std::string out = "";
    std::string ln;
    std::string text = "";

    // static strings
    Operation::set_space(0);
    out += generate_sysc_param_strings();

    // Constructor
    Operation::set_space(0);
    out += generate_sysc_constructor();

    // Destructor:
    if (isSubModules()) {
        Operation::set_space(0);
        out += generate_sysc_destructor();
    }

    // generateVCD function
    if (isVcd()) {
        Operation::set_space(0);
        out += generate_sysc_vcd();
    }

    // Functions
    for (auto &p: entries_) {
        if (p->getId() != ID_FUNCTION) {
            continue;
        }
        Operation::set_space(0);
        out += generate_sysc_func(p);
    }

    // Process
    for (auto &p: entries_) {
        if (p->getId() != ID_PROCESS) {
            continue;
        }
        if (p->getName() == "registers") {
            continue;
        }
        Operation::set_space(0);
        out += generate_sysc_proc(p);
    }

    if (isRegProcess()) {
        Operation::set_space(0);
        out += generate_sysc_proc_registers();
    }

    return out;
}

std::string ModuleObject::generate_sysc_func(GenObject *func) {
    std::string ret = "";
    std::list<GenObject *> argslist;
    int tcnt = 0;
    
    static_cast<FunctionObject *>(func)->getArgsList(argslist);

    ret += generate_sysc_template_f_name(func->getType().c_str());
    ret += "::" + func->getName() + "(";
    if (argslist.size() == 1) {
        for (auto &e: argslist) {
            ret += e->getType() + " " + e->getName();
        }
    } else if (argslist.size() > 1) {
        Operation::set_space(Operation::get_space() + 2);
        ret += "\n" + Operation::addspaces();
        for (auto &e: argslist) {
            ret += e->getType() + " " + e->getName();
            if (e != argslist.back()) {
                ret += ",\n" + Operation::addspaces();
            }
        }
        Operation::set_space(Operation::get_space() - 2);
    }
    ret += ") {\n";
    
    // process variables declaration
    if (func->getType() == "std::string") {
        ret += "    char tstr[256];\n";
    }
    tcnt = 0;
    bool skiparg;
    for (auto &e: func->getEntries()) {
        skiparg = false;
        for (auto &arg: argslist) {
            if (e->getName() == arg->getName()) {
                skiparg = true;
                break;
            }
        }
        if (skiparg) {
            continue;
        }

        if (e->getId() == ID_VALUE) {
            ret += "    " + e->getType() + " " + e->getName();
            tcnt++;
        } else if (e->getId() == ID_ARRAY_DEF) {
            ret += "    " + e->getType() + " " + e->getName();
            ret += "[";
            ret += e->getStrDepth();
            ret += "]";
        } else if (e->getId() == ID_STRUCT_INST) {
            ret += "    " + e->getType() + " " + e->getName();
        } else {
            continue;
        }
        tcnt++;
        ret += ";\n";
    }
    if (tcnt) {
        ret += "\n";
        tcnt = 0;
    }

    // Generate operations:
    Operation::set_space(1);
    for (auto &e: func->getEntries()) {
        if (e->getId() != ID_OPERATION) {
            continue;
        }
        ret += e->generate();
    }

    // return value
    if (static_cast<FunctionObject *>(func)->getpReturn()) {
        ret += "    return " + static_cast<FunctionObject *>(func)->getpReturn()->getName() + ";\n";
    }

    ret += "}\n";
    ret += "\n";
    return ret;
}

std::string ModuleObject::generate_sysc_proc(GenObject *proc) {
    std::string ret = "";
    std::string ln;
    int tcnt = 0;

    ret += generate_sysc_template_f_name();
    ret += "::" + proc->getName() + "() {\n";
    
    // process variables declaration
    tcnt = 0;
    for (auto &e: proc->getEntries()) {
        ln = "";
        if (e->getId() == ID_VALUE) {
            ln += "    " + e->getType() + " " + e->getName();
            tcnt++;
        } else if (e->getId() == ID_ARRAY_DEF) {
            ln += "    " + e->getType() + " " + e->getName();
            ln += "[";
            ln += e->getStrDepth();
            ln += "]";
        } else if (e->getId() == ID_STRUCT_INST) {
            ln += "    " + e->getType() + " " + e->getName();
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
    for (auto &e: proc->getEntries()) {
        if (e->getId() == ID_VALUE) {
            ret += "    " + e->getName() + " = 0;";
            tcnt++;
        } else if (e->getId() == ID_ARRAY_DEF) {
            ret += "    for (int i = 0; i < " + e->getStrDepth() + "; i++) {\n";
            ret += "        " + e->getName() + "[i] = 0;\n";
            ret += "    }";
        } else {
            continue;
        }
        ret += "\n";
    }

    if (tcnt) {
        ret += "\n";
    }
    if (isRegProcess()) {
        Operation::set_space(1);
        ret += Operation::copyreg("v", "r", this);
        ret += "\n";
    }

    // Generate operations:
    Operation::set_space(1);
    for (auto &e: proc->getEntries()) {
        if (e->getId() != ID_OPERATION) {
            continue;
        }
        ret += e->generate();
    }

    ret += "}\n";
    ret += "\n";
    return ret;
}

}
