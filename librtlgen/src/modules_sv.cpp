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
#include "operations.h"
#include <list>

namespace sysvc {


std::string ModuleObject::generate_sv_pkg() {
    std::string ret = "";
    return ret;
}

std::string ModuleObject::generate_sv_genparam() {
    std::string ret = "";
    int icnt = 0;
    std::list<GenObject *> tmpllist;
    std::list<GenObject *> genparam;
    getTmplParamList(tmpllist);
    getParamList(genparam);

    if (isAsyncReset()) {
        ret += "    parameter bit async_reset = 1'b0";           // Mandatory generic parameter
        icnt++;
    }

    for (auto &p : tmpllist) {
        if (icnt++) {
            ret += ",\n";
        }
        ret += "    parameter " + p->getType();
        ret += " " + p->getName() + " = " + p->getStrValue();
    }

    for (auto &p : genparam) {
        if (icnt++) {
            ret += ",\n";
        }
        ret += "    parameter " + p->getType();
        ret += " " + p->getName() + " = " + p->getStrValue();
    }

    return ret;
}

std::string ModuleObject::generate_sv_proc(GenObject *proc) {
    std::string ret = "";
    std::string ln;
    int tcnt = 0;

    ret += "always_comb\n";
    ret += "begin: " + proc->getName() + "_proc\n";
    
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
            ret += "    for (int i = 0; i < " + e->getStrDepth() + "; i++) begin\n";
            ret += "        " + e->getName() + "[i] = 0;\n";
            ret += "    end";
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

    ret += "end: " + proc->getName() + "_proc\n";
    ret += "\n";
    return ret;
}


std::string ModuleObject::generate_sv_proc_registers() {
    std::string out = "";
    std::string xrst = "";

    out += "generate\n";
    Operation::set_space(Operation::get_space() + 1);
    if (isAsyncReset()) {
        out += Operation::addspaces() + "if (async_reset) begin: async_rst_gen\n";
        Operation::set_space(Operation::get_space() + 1);
        out += Operation::addspaces() + "always_ff @(posedge i_clk or negedge i_nrst) begin: rg_proc\n";
        out += Operation::reset("r", 0, this, xrst);
        Operation::set_space(Operation::get_space() - 1);
        out += " end else begin\n";
        Operation::set_space(Operation::get_space() + 1);
    }
    out += Operation::copyreg("r", "v", this);
    if (isAsyncReset()) {
        Operation::set_space(Operation::get_space() - 1);
        out += Operation::addspaces();
        out += "end\n";
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
    out += "end\n";
    out += "endgenerate\n";
    out += "\n";
    return out;
}


std::string ModuleObject::generate_sv_mod() {
    std::string ret = "";
    std::string text;
    std::string ln;
    std::list<GenObject *> genparam;
    getParamList(genparam);

    ret += "module " + getType();

    // Generic parameters
    ln = generate_sv_genparam();
    if (ln.size()) {
        ret += " #(\n";
        ret += ln + "\n";
        ret += ")\n";
    }

    // In/Out ports
    ret += "(\n";
    int port_cnt = 0;
    for (auto &p: entries_) {
        if (p->getId() == ID_INPUT || p->getId() == ID_OUTPUT) {
            port_cnt++;
        }
    }
    text = "";
    for (auto &p: entries_) {
        if (p->getId() != ID_INPUT && p->getId() != ID_OUTPUT) {
            if (p->getId() == ID_COMMENT) {
                text += "    " + p->generate();
            } else {
                text = "";
            }
            continue;
        }
        if (text.size()) {
            ret += text;
            text = "";
        }
        ln = "";
        ln += "    " + p->getType();
        ln += " " + p->getName();
        if (--port_cnt) {
            ln += ",";
        }
        if (p->getComment().size()) {
            while (ln.size() < 60) {
                ln += " ";
            }
            ln += "// " + p->getComment();
        }
        ret += ln + "\n";
    }
    ret += ");\n";
    ret += "\n";

    // import statement:
    ret += "import " + getFile() + "_pkg::*;\n";
    ret += "\n";


    // Signal list:
    text = "";
    for (auto &p: entries_) {
        if (p->getId() != ID_SIGNAL) {
            if (p->getId() == ID_COMMENT) {
                text = p->generate();
            } else {
                text = "";
            }
            continue;
        }
        if (text.size()) {
            ret += text;
            text = "";
        }
        ret += static_cast<Signal *>(p)->getType();
        ret += " " + p->getName() + ";\n";
    }

    // Signal assignments:
    for (auto &p: entries_) {
        if (p->getId() != ID_OPERATION) {
            continue;
        }
        ret += "assign ";
        ret += p->generate();
        ret += "\n";
    }


    // Sub module instances:
    for (auto &p: entries_) {
        if (p->getId() != ID_MODULE_INST) {
            continue;
        }
        ret += "\n";
        ret += p->generate();
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
        ret += generate_sv_proc(p);
    }

    if (isRegProcess()) {
        Operation::set_space(0);
        ret += generate_sv_proc_registers();
    }


    ret += "endmodule: " + getType() + "\n";


    return ret;
}

}
