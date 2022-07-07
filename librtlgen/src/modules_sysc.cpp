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
#include "minstance.h"
#include "files.h"
#include "structs.h"
#include "regs.h"
#include "operations.h"

namespace sysvc {

std::string ModuleObject::generate_sysc_h() {
    std::string out = "";
    std::string ln;
    std::string text = "";

    out +=
        "SC_MODULE(" + getName() + ") {\n";

    // Input/Output signal declaration
    for (auto &p: entries_) {
        if (p->getId() != ID_INPUT && p->getId() != ID_OUTPUT) {
            if (p->getId() == ID_COMMENT) {
                text = "    " + p->generate(SYSC_ALL);
            } else {
                text = "";
            }
            continue;
        }
        ln = "";
        ln += "    " + p->getType(SYSC_ALL);
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
        out += "    SC_HAS_PROCESS(" + getName() + ");\n";
    }


    out += "\n";
    // Constructor delcartion:
    std::string space1 = "    " + getName() + "(";
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
        ln += p->getType(SYSC_ALL);
        ln += " " + p->getName();
        out += ln;
    }
    out += ");\n";
    // Destructor declaration
    out += "    virtual ~" + getName() + "();\n";
    out += "\n";

    // Mandatory VCD generator
    out += "    void generateVCD(sc_trace_file *i_vcd, sc_trace_file *o_vcd);\n";
    out += "\n";
    out += " private:\n";
    // Generic parameter local storage:
    if (isAsyncReset()) {
        out += "    " + (new Logic())->getType(SYSC_ALL) + " async_reset_;\n";
    }
    for (auto &p: entries_) {
        if (p->getId() != ID_DEF_PARAM) {
            continue;
        }
        out += "    " + p->getType(SYSC_ALL) + " " + p->getName() + "_;\n";
    }
    out += "\n";

    // struct definitions
    for (auto &p: entries_) {
        if (p->getId() != ID_STRUCT_DEF) {
            continue;
        }
        out += p->generate(SYSC_ALL);
    }
    out += "\n";
    // Register structure definition
    bool twodim = false;        // if 2-dimensional register array, then do not use reset function
    if (isRegProcess()) {
        out += "    struct " + getName() + "_registers {\n";
        for (auto &p: entries_) {
            if (!p->isReg()) {
                continue;
            }
            ln = "        " + p->getType(SYSC_ALL) + " " + p->getName();
            if (p->getDepth()) {
                twodim = true;
                ln += "[" + p->getDepth(SYSC_ALL) + "]";
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
            out += "    void " + getName() + "_r_reset(" + getName() + "_registers &iv) {\n";
            for (auto &p: entries_) {
                if (!p->isReg()) {
                    continue;
                }
                out += "        iv." + p->getName() + " = ";
                out += p->getValue(SYSC_ALL);
                out += ";\n";
            }
            out += "    }\n";
            out += "\n";
        }
    }


    // Signals list
    text = "";
    for (auto &p: entries_) {
        if (p->isReg() || (p->getId() != ID_SIGNAL && p->getId() != ID_STRUCT_INST)) {
            if (p->getId() == ID_COMMENT) {
                text += "    " + p->generate(SYSC_ALL);
            } else {
                text = "";
            }
            continue;
        }
        if (text.size()) {
            out += text;
            text = "";
        }
        out += "    " + p->getType(SYSC_ALL) + " " + p->getName();
        if (p->getDepth()) {
            out += "[" + p->getDepth(SYSC_ALL) + "]";
        }
        out += ";\n";
    }
    out += "\n";

    // Sub-module list
    for (auto &p: entries_) {
        if (p->getId() != ID_MINSTANCE) {
            continue;
        }
        out += "    " + p->getType(SYSC_ALL);
        out += " *" + p->getName() + ";\n";
    }
    out += "\n";

    out += 
        "};\n"
        "\n";
    return out;
}

std::string ModuleObject::generate_sysc_cpp() {
    std::string out = "";
    std::string ln;
    std::string text = "";

    // Constructor delcartion:
    std::string space1 = getName() + "::" + getName() + "(";
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
        ln += p->getType(SYSC_ALL);
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
    // Generic parameter local storage:
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
    for (auto &p: entries_) {
        if (p->getId() != ID_MINSTANCE) {
            continue;
        }
        out += "\n";
        out += p->generate(SYSC_ALL);
    }

    // Process sensitivity list:
    for (auto &p: entries_) {
        if (p->getId() != ID_PROCESS) {
            continue;
        }
        out += "\n";
        out += "    SC_METHOD(" + p->getName() + ");\n";
        for (auto &s: entries_) {
            if (s->getId() == ID_INPUT && s->getName() != "i_clk") {
                out += "    sensitive << " + s->getName() + ";\n";
            } else if (s->isReg()) {
                std::list<GenObject *> objlist;
                s->getSignals(objlist);
                for (auto &r : objlist) {
                    out += "    sensitive << r." + r->getFullName(SYSC_ALL, s) + ";\n";
                }
            }
        }
    }
    if (isRegProcess()) {
        out += "\n";
        out += "    SC_METHOD(registers);\n";
        out += "    sensitive << i_nrst;\n";
        out += "    sensitive << i_clk.pos();\n";
    }
    out += "}\n";


    // Destructor delcartion:
    out += "\n" + getName() + "::~" + getName() + "() {\n";
    for (auto &p: entries_) {
        if (p->getId() != ID_MINSTANCE) {
            continue;
        }
        out += "    delete " + p->getName() + ";\n";
    }
    out += "}\n";
    out += "\n";

    // generateVCD function
    out += "void " + getName() + "::generateVCD(sc_trace_file *i_vcd, sc_trace_file *o_vcd) {\n";
    out += "    if (o_vcd) {\n";
    for (auto &p: entries_) {
        if (p->getId() != ID_INPUT && p->getId() != ID_OUTPUT) {
            continue;
        }
        // TODO: check top level module instead of hardcoded name
        if (getName() != "RiverTop") {
            if (p->getName() == "i_clk" || p->getName() == "i_nrst" ) {
                // i_clk and i_nrst generate only on top level module
                continue;
            }
        }
        out += "        sc_trace(o_vcd, " + p->getName() + ", " + p->getName() + ".name());\n";
    }
    // Add all registers to trace file:
    bool twodim = is2DimReg();;
    std::list<GenObject *> objlist;
    if (isRegProcess()) {
        out += "\n";
        out += "        std::string pn(name());\n";
        for (auto &p: entries_) {
            if (!p->isReg()) {
                continue;
            }
            if (p->getId() == ID_ARRAY_DEF) {
                p->getSignals(objlist);
                for (auto &s: objlist) {
                    std::string fname = s->getFullName(SYSC_ALL, p);
                    out += "        sc_trace(o_vcd, r." + fname + ", pn + \".r_" + fname + "\");\n";
                }
            } else {
                out += "        sc_trace(o_vcd, r." + p->getName() + ", pn + \".r_" + p->getName() + "\");\n";
            }
        }
    }

    out += "    }\n";
    out += "\n";
    // Sub modules:
    for (auto &p: entries_) {
        if (p->getId() != ID_MINSTANCE) {
            continue;
        }
        out += "    " + p->getName() + "->generateVCD(i_vcd, o_vcd);\n";
    }
    out += "}\n";
    out += "\n";


    // Process
    int proccnt = 0;
    for (auto &p: entries_) {
        if (p->getId() != ID_PROCESS) {
            continue;
        }
        out += generate_sysc_cpp_proc(proccnt, p);
        proccnt++;
    }

    if (isRegProcess()) {
        out += "void " + getName() + "::registers() {\n";
        out += "    if (async_reset_ && i_nrst.read() == 0) {\n";
        if (!twodim) {
            // reset using function
            out += "        " + getName() + "_r_reset(r);\n";
        } else {
            // reset each register separatly
            for (auto &p: entries_) {
                if (!p->isReg()) {
                    continue;
                }
                if (p->getId() == ID_ARRAY_DEF) {
                    out += "        for (int i = 0; i < " + p->getDepth(SYSC_ALL) + "; i++) {\n";
                    std::list<GenObject *>::iterator it = p->getEntries().begin();  // element[0]
                    ln = "            r." + p->getName() + "[i]";
                    for (auto &s: (*it)->getEntries()) {
                        out += ln + "." + s->getName() + " = " + s->getValue(SYSC_ALL) + ";\n";
                    }
                    if ((*it)->getEntries().size() == 0) {
                        out += ln + " = " + p->getValue(SYSC_ALL);
                    }
                    out += "        }\n";
                } else {
                    out += "        r." + p->getName() + " = " + p->getValue(SYSC_ALL) + ";\n";
                }
            }
        }
        out += "    } else {\n";
        out += "        r = v;\n";
        out += "    }\n";
        out += "}\n";
        out += "\n";
    }

    return out;
}

std::string ModuleObject::generate_sysc_cpp_proc(int proccnt, GenObject *proc) {
    std::string ret = "";
    ret += "void " + getName() + "::" + proc->getName() + "() {\n";
    
    // process variables declaration
    int tcnt = 0;
    for (auto &e: proc->getEntries()) {
        if (e->getId() != ID_VALUE) {
            continue;
        }
        ret += "    " + e->getType(SYSC_ALL) + " " + e->getName() + ";\n";
        tcnt++;
    }

    if (tcnt) {
        ret += "\n";
    }
    if (proccnt == 0 && isRegProcess()) {
        ret += "    v = r;\n";
    }
    ret += "\n";

    // Generate operations:
    Operation::set_space(1);
    for (auto &e: proc->getEntries()) {
        if (e->getId() != ID_OPERATION) {
            continue;
        }
        ret += e->generate(SYSC_ALL);
    }

    ret += "}\n";
    ret += "\n";
    return ret;
}

}
