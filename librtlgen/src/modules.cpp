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

namespace sysvc {

ModuleObject::ModuleObject(GenObject *parent, const char *name) :
    GenObject(parent, ID_MODULE, name) {
}

std::string ModuleObject::getType(EGenerateType v) {
    std::string out = "";
    if (v == SYSC_ALL) {
        out += "SC_MODULE";
    } else if (v == SYSVERILOG_ALL) {
        out += "module";
    } else {
    }
    return out;
}

std::string ModuleObject::generate(EGenerateType v) {
    std::string out = "";
    if (v == SYSC_DECLRATION) {
        out += generate_sysc_h();
    }
    return out;
}

bool ModuleObject::isRegisters() {
    for (auto &e: entries_) {
        if (e->getId() == ID_MODULE) {
            if (static_cast<ModuleObject *>(e)->isRegisters()) {
                return true;
            }
        } else if (e->getId() == ID_REG) {
            return true;
        }
    }
    return false;
}

std::string ModuleObject::generate_sysc_h() {
    std::string out = "";

    out +=
        "SC_MODULE(" + getName() + ") {\n";

    // Input/Output signal declaration
    std::string ln;
    std::string text = "";
    for (auto &p: entries_) {
        if (p->getId() != ID_INPUT && p->getId() != ID_OUTPUT) {
            if (p->getId() == ID_COMMENT) {
                text = p->generate(SYSC_ALL);
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
    if (isRegisters()) {
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
    if (isRegisters()) {
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
        //out += static_cast<ModuleInstance *>(p)->getModuleName(SYSC_ALL);
        out += " *" + p->getName() + "\n";
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

}
