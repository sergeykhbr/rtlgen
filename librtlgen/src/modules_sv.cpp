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
    int tcnt = 0;
    GenObject *prev = 0;
    // Local paramaters visible inside of module
    for (auto &p: entries_) {
        if (p->getId() != ID_PARAM) {
            prev = 0;
            if (p->getId() == ID_COMMENT) {
                prev = p;
            }
            continue;
        }
        if (!static_cast<GenValue *>(p)->isLocal()) {
            prev = 0;
            continue;
        }
        if (p->getType() == "std::string") {
            // exclude strings (tracer case)
            prev = 0;
            continue;
        } else {
            if (prev) {
                ret += prev->generate();
            }
            ln = "localparam " + p->getType() + " " + p->getName();
            ln += " = " + p->getStrValue() + ";";
        }
        if (p->getComment().size()) {
            while (ln.size() < 60) {
                ln += " ";
            }
            ln += "// " + p->getComment();
        }
        ret += ln + "\n";
        tcnt++;
        prev = 0;
    }
    if (tcnt) {
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
    for (auto &p: entries_) {
        if (p->getId() != ID_STRUCT_DEF) {
            continue;
        }
        ret += p->generate();
        tcnt++;
    }
    if (tcnt) {
        ret += "\n";
        tcnt = 0;
    }
    // Register structure definition
    bool twodim = false;        // if 2-dimensional register array, then do not use reset function
    if (isRegProcess() && isCombProcess()) {
        ret += "typedef struct {\n";
        for (auto &p: entries_) {
            if (!p->isReg()) {
                continue;
            }
            ln = "    " + p->getType() + " " + p->getName();
            if (p->getDepth()) {
                twodim = true;
                ln += "[0: " + p->getStrDepth() + " - 1]";
            }
            ln += ";";
            if (p->getComment().size()) {
                while (ln.size() < 60) {
                    ln += " ";
                }
                ln += "// " + p->getComment();
            }
            ret += ln + "\n";
        }
        ret += "} " + getType() + "_registers;\n";
        ret += "\n";

        // Reset function only if no two-dimensial signals
        tcnt = 0;
        for (auto &p: entries_) {
            if (p->isReg()) {
                tcnt++;
            }
        }
        if (!twodim) {
            ret += "const " + getType() + "_registers " + getType() + "_r_reset = '{\n";
            for (auto &p: entries_) {
                if (!p->isReg()) {
                    continue;
                }
                ln = "    ";
                tstr = p->getStrValue();    // to provide compatibility with gcc
                if (p->isNumber(tstr) && p->getValue() == 0) {
                    if (p->getWidth() == 1) {
                        ln += "1'b0";
                    } else {
                        ln += "'0";
                    }
                } else {
                    ln += p->getStrValue();
                }
                if (--tcnt) {
                    ln += ",";
                }
                while (ln.size() < 40) {
                    ln += " ";
                }
                ln += "// " + p->getName();
                ret += ln + "\n";
            }
            ret += "};\n";
            ret += "\n";
        }
    }
    return ret;
}


std::string ModuleObject::generate_sv_pkg() {
    std::string ret = "";
    Operation::set_space(0);
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

    if (isAsyncReset()) {
        ret += "    parameter bit async_reset = 1'b0";           // Mandatory generic parameter
        if (genparam.size()) {
            ret += ",";
        }
        ret += "\n";
        icnt++;
    }

    for (auto &p : genparam) {
        ln = "    parameter " + p->getType();
        ln += " " + p->getName() + " = " + p->getStrValue();

        if (p != genparam.back()) {
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

    return ret;
}

std::string ModuleObject::generate_sv_mod_param_strings() {
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
        ret += "localparam string " + p->getName();
        ret += " = " + p->getStrValue() + ";\n";

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
    std::string ret = "";
    std::list<GenObject *> argslist;
    FunctionObject *pf = static_cast<FunctionObject *>(func);
    int tcnt = 0;
    
    pf->getArgsList(argslist);

    ret += "function " + func->getType() + " " + func->getName() + "(";
    if (argslist.size() == 1) {
        for (auto &e: argslist) {
            ret += "input " + e->getType() + " " + e->getName();
        }
    } else if (argslist.size() > 1) {
        Operation::set_space(Operation::get_space() + 2);
        ret += "\n" + Operation::addspaces();
        for (auto &e: argslist) {
            ret += "input " + e->getType() + " " + e->getName();
            if (e != argslist.back()) {
                ret += ",\n" + Operation::addspaces();
            }
        }
        Operation::set_space(Operation::get_space() - 2);
    }
    ret += ");\n";
    
    // process variables declaration
    if (pf->getpReturn()) {
        ret += pf->getpReturn()->getType() + " "
             + pf->getpReturn()->getName() + ";\n";
    }
    ret += "begin\n";
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
        if (e->getName() == pf->getpReturn()->getName()) {
            skiparg = true;
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

    ret += "end\n";
    ret += "endfunction: " + pf->getName() + "\n";
    ret += "\n";
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
        if (p->isReg() || (p->getId() != ID_SIGNAL
                        && p->getId() != ID_VALUE
                        && p->getId() != ID_STRUCT_INST
                        && p->getId() != ID_ARRAY_DEF)) {
            if (p->getId() == ID_COMMENT) {
                text += p->generate();
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
            ret += text;
            text = "";
        }
        ln = p->getType() + " " + p->getName();
        if (p->getDepth()) {
            ln += "[0: " + p->getStrDepth() + " - 1]";
        }
        ln += ";";
        if (p->getComment().size()) {
            while (ln.size() < 60) {
                ln += " ";
            }
            ln += "// " + p->getComment();
        }
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

    if (isRegProcess() && isCombProcess()) {
        ret += getType() + "_registers r, rin;\n";
        tcnt++;
    }
    if (tcnt) {
        ret += "\n";
        tcnt = 0;
    }
    return ret;
}

std::string ModuleObject::generate_sv_mod_proc(GenObject *proc) {
    std::string ret = "";
    std::string ln;
    int tcnt = 0;

    ret += "always_comb\n";
    ret += "begin: " + proc->getName() + "_proc\n";
    
    // process variables declaration
    tcnt = 0;
    if (isRegProcess()) {
        ret += "    " + getType() + "_registers v;\n";
    }

    for (auto &e: proc->getEntries()) {
        ln = "";
        if (e->getId() == ID_VALUE
            || e->getId() == ID_STRUCT_INST
            || e->getId() == ID_VECTOR) {
            ln += "    " + e->getType() + " " + e->getName();
            tcnt++;
        } else if (e->getId() == ID_ARRAY_DEF) {
            ln += "    " + e->getType() + " " + e->getName();
            ln += "[0: ";
            ln += e->getStrDepth();
            ln += "-1]";
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
    GenObject *arritem;
    for (auto &e: proc->getEntries()) {
        if (e->getId() == ID_VALUE) {
            ret += "    " + e->getName() + " = 0;";
            tcnt++;
        } else if (e->getId() == ID_ARRAY_DEF) {
            ret += "    for (int i = 0; i < " + e->getStrDepth() + "; i++) begin\n";
            ret += "        " + e->getName() + "[i] = ";
            arritem = static_cast<ArrayObject *>(e)->getItem();
            if (arritem->getId() == ID_STRUCT_INST && arritem->getStrValue().size() == 0) {
                SHOW_ERROR("todo: %s", "crawl through sub-structure element");
            }
            ret += arritem->getStrValue();
            ret += ";\n";
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

    if (isRegProcess()) {
        ret += "\n";
        ret += Operation::copyreg("rin", "v", this);
    }
    ret += "end: " + proc->getName() + "_proc\n";
    ret += "\n";
    return ret;
}


std::string ModuleObject::generate_sv_mod_proc_registers() {
    std::string out = "";
    std::string xrst = "";

    out += "generate\n";
    Operation::set_space(Operation::get_space() + 1);
    if (isAsyncReset()) {
        out += Operation::addspaces() + "if (async_reset) begin: async_rst_gen\n";
        Operation::set_space(Operation::get_space() + 1);
        out += "\n";
        out += Operation::addspaces() + "always_ff @(posedge i_clk or negedge i_nrst) begin: rg_proc\n";
        Operation::set_space(Operation::get_space() + 1);
        out += Operation::reset("r", 0, this, xrst);
        out += " else begin\n";
        Operation::set_space(Operation::get_space() + 1);
        if (isCombProcess()) {
            out += Operation::copyreg("r", "rin", this);
        }
        Operation::set_space(Operation::get_space() - 1);
        out += Operation::addspaces();
        out += "end\n";

        // additional operation on posedge clock events
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
        out += Operation::addspaces() + "end: rg_proc\n";

        Operation::set_space(Operation::get_space() - 1);
        out += "\n";
        out += Operation::addspaces() + "end: async_rst_gen\n";
        out += Operation::addspaces() + "else begin: no_rst_gen\n";
    } else {
        out += Operation::addspaces() + "begin: no_rst_gen\n";
    }
    Operation::set_space(Operation::get_space() + 1);
    out += "\n";
    out += Operation::addspaces() + "always_ff @(posedge i_clk) begin: rg_proc\n";
    Operation::set_space(Operation::get_space() + 1);
    if (isCombProcess()) {
        out += Operation::copyreg("r", "rin", this);
    }
    // additional operation on posedge clock events
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
    out += Operation::addspaces() + "end: rg_proc\n";
    out += "\n";
    Operation::set_space(Operation::get_space() - 1);
    out += Operation::addspaces() + "end: no_rst_gen\n";
    Operation::set_space(Operation::get_space() - 1);
    out += "endgenerate\n";
    out += "\n";
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
    std::list<std::string> pkglst;
    FileObject *pf = static_cast<FileObject *>(getParent());
    pf->getDepList(pkglst, tmplparam.size());
    for (auto &e: pkglst) {
        ret += "import " + e + "::*;\n";
    }
    if (tmplparam.size() == 0) {
        ret += "import " + pf->getName() + "_pkg::*;\n";
        ret += "\n";
    } else {
        // insert pkg data for template modules: ram, queue, ..
        ret += generate_sv_pkg();
    }

    // static strings
    Operation::set_space(0);
    ret += generate_sv_mod_param_strings();

    // Signal list:
    ret += generate_sv_mod_signals();

    // Functions
    for (auto &p: entries_) {
        if (p->getId() != ID_FUNCTION) {
            continue;
        }
        Operation::set_space(0);
        ret += generate_sv_mod_func(p);
    }

    // Sub-module instantiation
    Operation::set_space(0);
    for (auto &p: entries_) {
        if (p->getId() != ID_OPERATION) {
            continue;
        }
        ln = p->generate();
        ret += ln;
        ret += "\n";
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
        ret += generate_sv_mod_proc(p);
    }

    if (isRegProcess()) {
        Operation::set_space(0);
        ret += generate_sv_mod_proc_registers();
    }


    ret += "endmodule: " + getType() + "\n";


    return ret;
}

}
