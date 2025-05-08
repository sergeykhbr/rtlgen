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
#include <algorithm>
#include <cstring>

namespace sysvc {

std::string ModuleObject::generate_sysc_h() {
    std::string out = "";
    std::string ln;
    std::string text = "";
    std::list<GenObject *> tmpllist;
    std::list<GenObject *> argslist;
    int tcnt = 0;

    getTmplParamList(tmpllist);
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
        e->addComment(ln);
        out += ln + "\n";
        ln = "";
    }

    tcnt = 0;
    out += "SC_MODULE(" + getType() + ") {\n";
    out += " public:\n";
    pushspaces();

    // Input/Output signal declaration
    for (auto &p: getEntries()) {
        if (!p->isInput() && !p->isOutput()) {
            if (p->isComment()) {
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
    for (auto &p: getEntries()) {
        if (!p->isProcess()) {
            continue;
        }
        out += addspaces() + "void " + p->getName() + "();\n";
    }

    // Constructor declaration:
    out += "\n";
    std::string space1 = addspaces() + getType() + "(";
    out += space1 + "sc_module_name name";
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
    for (auto &p: getEntries()) {
        if (p->isParamTemplate()) {
            // do nothing
        } else if (p->isParamGeneric()
               || (p->isGenericDep() && tmpllist.size() == 0)) {  // exclude gendep parameter as a static const
            // class variables definitions:
            out += addspaces() + p->getType() + " " + p->nameInModule(PORT_OUT, NO_SC_READ) + ";\n";
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
        if (p->isComment()) {
            comment += p->generate();
            continue;
        } else if (p->isParam() && !p->isParamGeneric()) {
            if (p->isString()) {
                // Do Nothing: strings defined in cpp-file
            } else if (p->isGenericDep() && tmpllist.size() == 0) {
                // Do nothing: This parameter depends of generic parameter (constrcutor argument)
            } else {
                out += comment;
                out += p->generate();
                tcnt++;
            }
        }
        comment = "";
    }
    if (tcnt) {
        out += "\n";
        tcnt = 0;
    }

    // struct definitions
    out += generate_all_struct();

    // Functions declaration:
    tcnt = 0;
    for (auto &p: getEntries()) {
        if (!p->isFunction() || p->isResetConst()) {
            // exclude r_reset function. Output it after structure definition
            continue;
        }
        tcnt++;
        SCV_set_generator(SYSC_H);
        out += p->generate();
        SCV_set_generator(SYSC_ALL);
    }
    if (tcnt) {
        out += "\n";
        tcnt = 0;
    }

    // Signals list
    out += generate_all_mod_variables();

    // Sub-module list
    for (auto &p: getEntries()) {
        if (p->isModule()) {
            out += addspaces() + p->getType();
            out += generate_sysc_template_param(p);
            out += " *" + p->getName();
            if (p->getObjDepth()) {
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

std::string ModuleObject::generate_sysc_sensitivity(GenObject *obj) {
    std::string ret = "";
    I32D *idx = 0;
    char tstr[256];
    if (obj->getObjDepth()) {
        idx = new I32D(NO_PARENT, loopidx_, "0", NO_COMMENT);
        obj->setSelector(idx);

        RISCV_sprintf(tstr, sizeof(tstr), "for (int %s = 0; %s < %s; %s++) {\n",
            loopidx_, loopidx_, obj->getStrDepth().c_str(), loopidx_);
        ret = addspaces() + tstr;
        pushspaces();
        loopidx_[0]++;
    }

    if (obj->isInterface() || obj->getEntries().size() == 0) {
        // simple signals or interface structures (inputs) with redefined operator <<:
        ret += addspaces() + "sensitive << " + obj->nameInModule(PORT_OUT, NO_SC_READ) + ";\n";
    } else {
        for (auto &e: obj->getEntries()) {
            ret += generate_sysc_sensitivity(e);
        }
    }

    if (obj->getObjDepth()) {
        loopidx_[0]--;
        obj->setSelector(0);
        delete idx;
        popspaces();
        ret += addspaces() + "}\n";
    }
    return ret;
}

std::string ModuleObject::generate_sysc_vcd_entries(GenObject *obj) {
    std::string ret = "";
    I32D *idx = 0;
    char tstr[256];
    if (obj->getObjDepth()) {
        idx = new I32D(NO_PARENT, loopidx_, "0", NO_COMMENT);
        obj->setSelector(idx);

        RISCV_sprintf(tstr, sizeof(tstr), "for (int %s = 0; %s < %s; %s++) {\n",
            loopidx_, loopidx_, obj->getStrDepth().c_str(), loopidx_);
        ret = addspaces() + tstr;
        pushspaces();
        loopidx_[0]++;
    }

    // Interface implements ofstream<< operator (all inputs/outputs):
    if (obj->isInput() || obj->isOutput()) {
        std::string tname = obj->nameInModule(PORT_OUT, NO_SC_READ);
        ret += addspaces() + "sc_trace(o_vcd, " + tname + ", " + tname + ".name());\n";
    } else if (obj->getEntries().size() == 0) {
        // r-structure entries:
        std::string tname = obj->nameInModule(PORT_OUT, NO_SC_READ);
        ret += addspaces() + "sc_trace(o_vcd, " + tname;
        ret += ", pn + \"." + tname + "\");\n";
    } else {
        // Register structure
        for (auto &e: obj->getEntries()) {
            if (!e->isVcd()) {
                continue;
            }
            ret += generate_sysc_vcd_entries(e);
        }
    }

    if (obj->getObjDepth()) {
        loopidx_[0]--;
        obj->setSelector(0);
        delete idx;
        popspaces();
        ret += addspaces() + "}\n";
    }
    return ret;
}

std::string ModuleObject::generate_sysc_template_param(GenObject *p) {
    std::string ret = "";
    int tcnt = 0;

    for (auto &e: p->getEntries()) {
        if (!e->isParamTemplate()) {
            continue;
        }

        // Additional check that all template\generic parameters are explictly connected
        /*if (e->getObjValue() == 0) {
            SCV_printf("warning: %s::%s::%s parameter is not assigned",
                        getType().c_str(),
                        p->getName().c_str(),
                        e->getName().c_str());
        }*/

        if (tcnt == 0) {
            ret += "<";
        }
        if (tcnt++) {
            ret += ", ";
        }
        ret += e->getStrValue();
    }
    if (tcnt) {
        ret += ">";
    }
    return ret;
}

std::string ModuleObject::generate_sysc_template_f_name(const char *rettype, bool with_class_name) {
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
    if (with_class_name) {
        ret += getType();
    }
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
        if (p->isString() && p->isParam() && !p->isParamGeneric()) {
            // Only string parameter defined inside of this module
            ret += p->generate();
            tcnt++;
        }
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
    // Sub-item constructors:
    tcnt = 0;
    for (auto &p: getEntries()) {
        if (p->isInput()
            || p->isOutput()
            || p->isVector()) {
            ret += ",\n    " + p->getName() + "(\"" + p->getName() + "\"";
            if (p->isVector()) {
                ret += ", " + p->getStrDepth();
            }
            ret += ")";
        }
    }
    ret += " {\n";
    ret += "\n";
    pushspaces();
    // local copy of the generic parameters:
    for (auto &p: getEntries()) {
        if (p->isParamTemplate()) {
            // do nothing
        } else if (p->isParamGeneric()) {
            ret += addspaces() + p->getName() + "_ = " + p->getName() + ";\n";
        } else if (p->isParam() && p->isGenericDep() && tmpllist.size() == 0) {
            ret += addspaces() + p->getName() + " = " + p->getStrValue() + ";\n";
        }
    }

    // Sub-module instantiation
    ret += generate_sysc_submodule_nullify();
    for (auto &p: getEntries()) {
        if (p->isOperation()) {
            ret += p->generate();
        }
    }

    // Process sensitivity list:
    for (auto &p: getEntries()) {
        if (!p->isProcess()) {
            continue;
        }

        ret += "\n";
        ret += addspaces() + p->getType() + "(" + p->getName() + ");\n";
        if (p->getClockPort()) {
            GenObject *clkport = p->getClockPort();
            GenObject *rstport = p->getResetPort();
            if (rstport) {
                ret += addspaces() + "sensitive << " + rstport->getName() + ";\n";
            }
            if (clkport->isInput()) {
                ret += addspaces() + "sensitive << " + clkport->getName() + ".pos();\n";
            } else {
                ret += addspaces() + "sensitive << " + clkport->getName() + ".posedge_event();\n";
            }
        } else {
            ln = std::string("i");
            for (auto &s: getEntries()) {
                if (!s->isSignal()) {
                    continue;
                }
                if (s->isTypedef() || s->getName() == "i_clk") {
                    continue;
                }
                ret += generate_sysc_sensitivity(s);
            }
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
            if (p->getObjDepth()) {
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
            if (p->getObjDepth()) {
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

            if (p->getObjDepth()) {
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

    ret += generate_sysc_template_f_name("void");
    ret += "::generateVCD(sc_trace_file *i_vcd, sc_trace_file *o_vcd) {\n";
    pushspaces();
    if (sorted_regs_.size()) {
        ret += addspaces() + "std::string pn(name());\n";
    }
    ret += addspaces() + "if (o_vcd) {\n";
    pushspaces();
    for (auto &p: getEntries()) {
        if (!p->isInput() && !p->isOutput()) {
            continue;
        }
        if ((p->getName() == "i_clk" || p->getName() == "i_nrst") && !isTop()) {
            continue;
        }
        if (!p->isVcd()) {
            // specially disabled signals
            continue;
        }
        if (p->isVector()) {
            // just to reduce trace size
            continue;
        }
        ret += generate_sysc_vcd_entries(p);
    }
    for (auto &p : sorted_regs_) {
        ret += generate_sysc_vcd_entries(p->r_instance());
    }
    popspaces();
    ret += addspaces() + "}\n";
    ret += "\n";

    // Sub modules:
    for (auto &p: getEntries()) {
        if (p->isModule() && p->isVcd()) {
            std::string tidx = "";
            if (p->getObjDepth()) {
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

            if (p->getObjDepth()) {
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
    std::list<GenObject *> proclist;
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
    for (auto &p: getEntries()) {
        if (!p->isFunction()) {
            continue;
        }
        if (p->isResetConst()) {
            // "r_reset" function always in header file
            continue;
        }
        out += generate_sysc_template_f_name(p->getType().c_str()) + "::";
        out += p->generate();
        out += "\n";
    }

    // Process
    for (auto &p: getEntries()) {
        if (!p->isProcess()) {
            continue;
        }
        out += generate_sysc_template_f_name("void") + "::";
        out += p->getName() + "() {\n";
        pushspaces();
        out += p->generate();
        popspaces();
        out += "}\n";
        out += "\n";
    }
    return out;
}

}
