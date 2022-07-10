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

namespace sysvc {

std::string ModuleObject::generate_sysc_h() {
    std::string out = "";
    std::string ln;
    std::string text = "";
    std::list<GenObject *> tmpllist;
    int tcnt = 0;

    getTmplParamList(tmpllist);
    if (tmpllist.size()) {
        out += "template<";
        for (auto &e: tmpllist) {
            if (tcnt++) {
                out += ", ";
            }
            out += e->getType() + " " + e->getName() + " = " + e->getStrValue();
        }
        out += ">\n";
    }

    tcnt = 0;
    out += "SC_MODULE(" + getType() + ") {\n";

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
    // Constructor delcartion:
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
    out += "    virtual ~" + getType() + "();\n";
    out += "\n";

    // Mandatory VCD generator
    out += "    void generateVCD(sc_trace_file *i_vcd, sc_trace_file *o_vcd);\n";
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
        out += "    static const " + p->getType() + " " + p->getName();
        out += " = " + p->getStrValue() + ";\n";
        tcnt++;
    }
    if (tcnt) {
        out += "\n";
        tcnt = 0;
    }

    // struct definitions
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
    if (tcnt) {
        out += "\n";
        tcnt = 0;
    }

    // Sub-module list
    for (auto &p: entries_) {
        if (p->getId() == ID_MODULE_INST) {
            out += "    " + p->getType() + " *" + p->getName() + ";\n";
            tcnt ++;
        } else if (p->getId() == ID_ARRAY_DEF) {
            ArrayObject *a = static_cast<ArrayObject *>(p);
            if (a->getItem()->getId() == ID_MODULE_INST) {
                out += "    " + a->getType() + " *" + p->getName();
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
    if (isRegProcess()) {
        Operation::set_space(1);
        out += generate_sysc_proc_registers();
    }
    return out;
}

std::string ModuleObject::generate_sysc_proc_registers() {
    std::string out = "";
    std::string xrst = "";
    std::list<GenObject *> tmpllist;
    int tcnt = 0;

    getTmplParamList(tmpllist);
    if (tmpllist.size()) {
        out += Operation::addspaces();
        out += "template<";
        for (auto &e: tmpllist) {
            if (tcnt++) {
                out += ", ";
            }
            out += e->getType() + " " + e->getName();
        }
        out += ">\n";
    }
    tcnt = 0;

    out += Operation::addspaces();
    out += "void " + getType();
    if (tmpllist.size()) {
        out += "<";
        for (auto &e: tmpllist) {
            if (tcnt++) {
                out += ", ";
            }
            out += e->getName();
        }
        out += ">";
    }
    out += "::registers() {\n";
    Operation::set_space(Operation::get_space() + 1);
    out += Operation::reset("r", 0, this, xrst);
    out += " else {\n";
    Operation::set_space(Operation::get_space() + 1);
    out += Operation::copyreg("r", "v", this);
    Operation::set_space(Operation::get_space() - 1);
    out += Operation::addspaces();
    out += "}\n";
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
    } else if (obj->getId() == ID_ARRAY_DEF) {
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

std::string ModuleObject::generate_sysc_vcd(std::string name1, std::string name2, GenObject *obj) {
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

    if (obj->getId() == ID_INPUT || obj->getId() == ID_OUTPUT) {
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
        ret += generate_sysc_vcd(name1, name2, *it);

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
            ret += generate_sysc_vcd(name1, name2, s);
        }
    }

    return ret;
}

std::string ModuleObject::generate_sysc_cpp() {
    std::string out = "";
    std::string ln;
    std::string text = "";

    // Constructor delcartion:
    std::string space1 = getType() + "::" + getType() + "(";
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
    out += ")\n";
    out += "    : sc_module(name)";
    // Input/Output signal declaration
    int tcnt = 0;
    for (auto &p: entries_) {
        if (p->getId() != ID_INPUT && p->getId() != ID_OUTPUT) {
            continue;
        }
        out += ",\n    " + p->getName() + "(\"" + p->getName() + "\")";
    }
    if (tcnt == 0) {
        // not IO ports
        out += " ";
    }
    out += "{\n";
    out += "\n";
    // local copy of the generic parameters:
    if (isAsyncReset()) {
        out += "    async_reset_ = async_reset;\n";
    }
    for (auto &p: entries_) {
        if (p->getId() != ID_DEF_PARAM) {
            continue;
        }
        out += "    " + p->getName() + "_ = " + p->getName() + ";\n";
    }

    // Sub-module instantiation
    Operation::set_space(1);
    for (auto &p: entries_) {
        if (p->getId() != ID_OPERATION) {
            continue;
        }
        out += "\n";
        out += p->generate();
    }

    // Process sensitivity list:
    std::list<GenObject *> objlist;
    std::string prefix1 = "r.";
    for (auto &p: entries_) {
        if (p->getId() != ID_PROCESS) {
            continue;
        }
        out += "\n";
        out += "    SC_METHOD(" + p->getName() + ");\n";

        ln = std::string("");
        Operation::set_space(1);
        out += generate_sysc_sensitivity(prefix1, ln, this);
    }
    if (isRegProcess()) {
        out += "\n";
        out += "    SC_METHOD(registers);\n";
        out += "    sensitive << i_nrst;\n";
        out += "    sensitive << i_clk.pos();\n";
    }
    out += "}\n";


    // Destructor delcartion:
    out += "\n" + getType() + "::~" + getType() + "() {\n";
    for (auto &p: entries_) {
        if (p->getId() == ID_MODULE_INST) {
            out += "    delete " + p->getName() + ";\n";
        } else if (p->getId() == ID_ARRAY_DEF) {
            ArrayObject *a = static_cast<ArrayObject *>(p);
            if (a->getItem()->getId() == ID_MODULE_INST) {
                out += "    for (int i = 0; i < " + a->getStrDepth() + "; i++) {\n";
                out += "        delete " + p->getName() + "[i];\n";
                out += "    }\n";
            }
        }
    }
    out += "}\n";
    out += "\n";

    // generateVCD function
    out += "void " + getType() + "::generateVCD(sc_trace_file *i_vcd, sc_trace_file *o_vcd) {\n";
    if (isRegProcess()) {
        out += "    std::string pn(name());\n";
    }
    out += "    if (o_vcd) {\n";

    ln = "";
    std::string ln2 = "";
    Operation::set_space(2);
    out += generate_sysc_vcd(ln, ln2, this);

    out += "    }\n";
    out += "\n";
    // Sub modules:
    for (auto &p: entries_) {
        if (p->getId() == ID_MODULE_INST) {
            out += "    " + p->getName() + "->generateVCD(i_vcd, o_vcd);\n";
        } else if (p->getId() == ID_ARRAY_DEF) {
            ArrayObject *a = static_cast<ArrayObject *>(p);
            if (a->getItem()->getId() == ID_MODULE_INST) {
                out += "    for (int i = 0; i < " + a->getStrDepth() + "; i++) {\n";
                out += "        " + p->getName() + "[i]->generateVCD(i_vcd, o_vcd);\n";
                out += "    }\n";
            }
        }
    }
    out += "}\n";
    out += "\n";


    // Process
    for (auto &p: entries_) {
        if (p->getId() != ID_PROCESS) {
            continue;
        }
        out += generate_sysc_cpp_proc(p);
    }

    if (isRegProcess()) {
        Operation::set_space(0);
        out += generate_sysc_proc_registers();
    }

    return out;
}

std::string ModuleObject::generate_sysc_cpp_proc(GenObject *proc) {
    std::string ret = "";
    ret += "void " + getType() + "::" + proc->getName() + "() {\n";
    
    // process variables declaration
    int tcnt = 0;
    for (auto &e: proc->getEntries()) {
        if (e->getId() == ID_VALUE) {
            ret += "    " + e->getType() + " " + e->getName();
            tcnt++;
        } else if (e->getId() == ID_ARRAY_DEF) {
            ret += "    " + e->getType() + " " + e->getName();
            ret += "[";
            ret += e->getStrDepth();
            ret += "]";
        } else {
            continue;
        }
        ret += ";\n";
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
