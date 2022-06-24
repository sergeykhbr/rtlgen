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
        ln += "    " + static_cast<PortObject *>(p)->getType(SYSC_ALL);
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
        ln += static_cast<DefParam *>(p)->getType(SYSC_ALL);
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

    // Sub-module list
    for (auto &p: entries_) {
        if (p->getId() != ID_MINSTANCE) {
            continue;
        }
        out += "    " + p->getType(SYSC_ALL);
        out += " *" + p->getName() + ";\n";
    }
    out += "\n";

    // Signals list
    text = "";
    for (auto &p: entries_) {
        if (p->getId() != ID_SIGNAL) {
            if (p->getId() == ID_COMMENT) {
                text = p->generate(SYSC_ALL);
            } else {
                text = "";
            }
            continue;
        }
        if (text.size()) {
            out += "    " + text;
            text = "";
        }
        out += "    " + static_cast<Signal *>(p)->getType(SYSC_ALL);
        out += " " + p->getName() + ";\n";
    }
    
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
        ln += static_cast<DefParam *>(p)->getType(SYSC_ALL);
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
    for (auto &p: entries_) {
        if (p->getId() != ID_MINSTANCE) {
            continue;
        }
        out += "\n";
        out += p->generate(SYSC_ALL);
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
    return out;
}

}
