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

namespace sysvc {

std::string ModuleObject::generate_sysc_h_reg_struct() {
    std::map<std::string, std::list<GenObject *>> regmap;
    std::map<std::string, bool> is2dm;
    std::string out = "";
    std::string ln = "";
    std::string r;
    std::string v;
    
    getSortedRegsMap(regmap, is2dm);
    for (std::map<std::string, std::list<GenObject *>>::iterator it = regmap.begin();
        it != regmap.end(); it++) {
        r = it->first;                          // map sorted by v_prefix
        v = (*it->second.begin())->v_prefix();  // all obj in a list has the same v_prefix as the first one

        out += addspaces() + "struct " + getType() + "_" + r + "egisters {\n";
        pushspaces();
        for (auto &p: it->second) {
            ln = addspaces();

            if (p->isSignal() && !p->isIgnoreSignal()) {
                // some structure are not defined as a signal but probably should be
                ln += "sc_signal<";
            }
            ln += p->getType();
            if (p->isSignal() && !p->isIgnoreSignal()) {
                ln += ">";
            }
            ln += " " + p->getName();
            if (p->getObjDepth()) {
                ln += "[" + p->getStrDepth() + "]";
            }
            ln += ";";
            p->addComment(ln);
            out += ln + "\n";
        }
        popspaces();
        out += addspaces() + "} " + v + ", " + r + ";\n";

        out += "\n";
        if (!is2dm[it->first]) {
            out += addspaces() + "void " + getType();
            out += "_" + r + "_reset(" + getType() + "_" + r +"egisters &iv) {\n";
            pushspaces();
            for (auto &p: it->second) {
                out += addspaces() + "iv." + p->getName() + " = ";
                out += p->getStrValue();
                // TODO: add "_" for Generic value. Right now can not use generic param value as reset.
                out += ";\n";
            }
            popspaces();
            out += addspaces() + "}\n";
            out += "\n";
        }
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
    out += generate_sysc_h_reg_struct();
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
    std::map<std::string, std::list<GenObject *>> regmap;
    std::map<std::string, bool> is2dm;
    std::list<GenObject *> combproclist;
    std::list<std::string> regproclist;

    // Combinational process excluding register process:
    getCombProcess(combproclist);
    for (auto &p: combproclist) {
        out += addspaces() + "void " + p->getName() + "();\n";
    }

    // Registers processes:
    getSortedRegsMap(regmap, is2dm);
    for (std::map<std::string,std::list<GenObject *>>::iterator it = regmap.begin();
        it != regmap.end(); ++it) {
        regproclist.push_back(it->first + "egisters");
    }
    for (auto &reg_proc : regproclist) {
        out += addspaces() + "void " + reg_proc + "();\n";
    }

    if (combproclist.size() || regproclist.size()) {
        out += "\n";
        out += addspaces() + "SC_HAS_PROCESS(" + getType() + ");\n";
    }

    out += "\n";
    // Constructor declaration:
    std::string space1 = addspaces() + getType() + "(";
    out += space1 + "sc_module_name name";
    if (isAsyncResetParam() && getAsyncResetParam() == 0) {
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
    if (isAsyncResetParam() && getAsyncResetParam() == 0) {
        out += addspaces() + async_reset_.getType() + " async_reset_;\n";
        tcnt++;
    }
    for (auto &p: getEntries()) {
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
                out += addspaces() + "static const " + p->getType() + " ";
                out += p->getName() + " = " + p->getStrValue() + ";\n";
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
    for (auto &p: getEntries()) {
        if (!p->isFunction()) {
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

    // struct definitions
    out += generate_sysc_h_struct();

    // Signals list
    text = "";
    for (auto &p: getEntries()) {
        if (p->isComment()) {
            text += p->generate();
            continue;
        }
        if (p->isInput()
            || p->isOutput()
            || p->isOperation()
            || p->isTypedef()
            || p->isParam()
            || p->isReg()
            || p->isNReg()) {
            text = "";
            continue;
        }
        if (p->getName() == "") {
            SHOW_ERROR("Error: unnamed object of type %s", p->getType().c_str());
            text = "";
            continue;
        }
        if (!p->isSignal()
                && !p->isValue()
                && !p->isStruct()
                && !p->isClock()) {
                text = "";
            continue;
        }
        if (text.size()) {
            out += text;
            text = "";
        }
        ln = addspaces();
        if (p->isSignal() && !p->isIgnoreSignal()) {
            ln += "sc_signal<";
        }
        ln += p->getType();
        if (p->isSignal() && !p->isIgnoreSignal()) {
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

/* Generate in process (synchronous) reset. Multiple resets are supported:

      if (!async_reset && i_rst0.read() == LOW) {
          module_type_r0(v0);
      }
      if (!async_reset && i_rst1.read() == LOW) {
          module_type_r1(v1);
      }
      if (!async_reset && i_rst0.read() == HIGH) {
          module_type_r0(v0);
      }
 */
std::string ModuleObject::generate_sysc_proc_v_reset(std::string &xrst) {
    std::map<std::string, std::list<GenObject *>>regmap;
    std::map<std::string, bool> is2dm;
    GenObject *preg;
    std::string ret;
    std::string v;
    std::string r;
    const char *SV_STR_ACTIVE[3] = {"", "0", "1"};

    getSortedRegsMap(regmap, is2dm);

    for (std::map<std::string, std::list<GenObject *>>::iterator it = regmap.begin();
        it != regmap.end(); ++it) {
        preg = (*it->second.begin());       // all registers in a group have the same reset and clock signals
        if (preg->getResetActive() == ACTIVE_NONE) {
            continue;
        }

        ret += addspaces() + "if ";
        if (xrst.size()) {
            ret += "(";
        }
        if (isAsyncResetParam()) {
            ret += "(!async_reset_ && ";
        }
        ret += preg->getResetPort()->getName() + ".read() == ";
        ret += std::string(SV_STR_ACTIVE[preg->getResetActive()]) + ")";
        if (xrst.size()) {
            ret += " || " + xrst + ")";
        }
        ret += " {\n";
        pushspaces();

        r = preg->r_prefix();
        v = preg->v_prefix();
        if (!is2dm[it->first]) {
            ret += addspaces() + getType() + "_" + r + "_reset(" + v + ");\n";
        } else {
            char i_idx[2] = {0};
            for (auto &r : it->second) {
                i_idx[0] = 'i';
                ret += r->getCopyValue(i_idx, v.c_str(), "=", RSTVAL_NONE);
            }
        }

        popspaces();
        ret += addspaces() + "}\n";
    }
    return ret;
}

std::string ModuleObject::generate_sysc_proc_registers() {
    std::map<std::string, std::list<GenObject *>> regmap;
    std::map<std::string, bool> is2dm;
    std::string ret;
    std::string v;
    std::string r;
    std::string resetname;
    std::string procname;
    EResetActive active;
    GenObject *preg;
    GenObject *resobj;
    const char *SV_STR_ACTIVE[3] = {"", "0", "1"};

    // All registers with selected posedge and reset level:
    getSortedRegsMap(regmap, is2dm);

    for (std::map<std::string, std::list<GenObject *>>::iterator it = regmap.begin();
        it != regmap.end(); ++it) {
        preg = (*it->second.begin());
        resobj = preg->getResetPort();

        r = preg->r_prefix();
        v = preg->v_prefix();
        active = preg->getResetActive();
        procname = r + "egisters";

        ret += generate_sysc_template_f_name();
        ret += "::" + procname + "() {\n";
        pushspaces();

        if (active != ACTIVE_NONE) {
            resetname = resobj->getName();

            // Async Reset implementation
            ret += addspaces() + "if (";
            if (isAsyncResetParam()) {
                ret += "async_reset_ && ";
            }
            ret += resetname + ".read() == " + SV_STR_ACTIVE[active] + ") {\n";
            pushspaces();

            // r <= reset
            if (!is2dm[it->first]) {
                ret += addspaces() + getType() + "_" + r + "_reset(" + r + ");\n";
            } else {
                char i_idx[2] = {0};
                for (auto &p : it->second) {
                    i_idx[0] = 'i';
                    ret += p->getCopyValue(i_idx, r.c_str(), "=", "");
                }
            }

            popspaces();
            ret += addspaces() + "} else {\n";
            pushspaces();
        }
        // Copy r <= v:
        if (!is2dm[it->first]) {
            ret += addspaces() + r + " = " + v + ";\n";
        } else {
            char i_idx[2] = {0};
            for (auto &p : it->second) {
                i_idx[0] = 'i';
                ret += p->getCopyValue(i_idx, r.c_str(), "=", v.c_str());
            }
        }
        if (active != ACTIVE_NONE) {
            popspaces();
            ret += addspaces() + "}\n";
        }

        // Let's check process name that should be added to this register
        for (auto &p: getEntries()) {
            if (!p->isProcess() || p->getName() != procname) {
                continue;
            }

            ret += "\n";
            for (auto &s : p->getEntries()) {
                ret += s->getStrValue();
            }
        }

        popspaces();
        ret += addspaces() + "}\n";
        ret += "\n";
    }
    return ret;
}

std::string ModuleObject::generate_sysc_sensitivity(GenObject *obj,
                                                    std::string prefix,
                                                    std::string i,
                                                    std::string &loop) {
    std::string ret = "";
    if (!obj->isSignal()
        && !obj->isStruct()
        && !(obj->getClockEdge() == CLK_POSEDGE)
        && !(obj->getClockEdge() == CLK_NEGEDGE)) {
        return ret;
    }
    if (prefix == "") {
        // Check exceptions only on first level
        if (obj->isTypedef()
            || (obj->isOutput() && !obj->isInput())     // ignore out structures
            || obj->getName() == "i_clk") {
            return ret;
        }
    }

    if (prefix.size()) {
        prefix += ".";
    }
    prefix += obj->getName();

    if (obj->getObjDepth()) {
        prefix += "[" + i + "]";
        loop = addspaces();
        loop += "for (int " + i + " = 0; " + i + " < " + obj->getStrDepth() + "; " + i + "++) {\n";
        pushspaces();
    }

    if (obj->isStruct() && !obj->isInterface()) {
        const char tidx[2] = {static_cast<char>(static_cast<int>(i.c_str()[0]) + 1), 0};
        i = std::string(tidx);
        for (auto &e: obj->getEntries()) {
            ret += generate_sysc_sensitivity(e, prefix, i, loop);
        }
    } else {
        if (obj->r_prefix().size()) {
            prefix = obj->r_prefix() + "." + prefix;
        }
        if (!obj->isSignal()) {
            SCV_printf("warning: non signal '%s' used as a trigger, "
                "skip in the sensitivity list.", obj->getName().c_str());
            return ret;
        }
        ret += loop;
        ret += addspaces() + "sensitive << " + prefix + ";\n";
        loop = "";
    }

    if (obj->getObjDepth()) {
        // Insert end of 'for' loop if it was generated:
        if (loop == "") {
            popspaces();
            ret += addspaces() + "}\n";
        } else {
            popspaces();
            loop = "";
        }
    }
    return ret;
}

std::string ModuleObject::generate_sysc_vcd_entries(GenObject *obj,
                                                    std::string prefix,
                                                    std::string i,
                                                    std::string &loop) {    // do not print empty for loop cycle
    std::string ret = "";
    if (!obj->isInput()
        && !obj->isOutput()
        && !obj->isStruct()
        && !obj->isReg()
        && !obj->isNReg()) {
        return ret;
    }
    if (prefix == "") {
        // Check exceptions only on first level
        if (!obj->isVcd()
            || obj->isTypedef()
            || obj->isVector()  // just to reduce number of traced data
            || (obj->getName() == "i_clk" || obj->getName() == "i_nrst") && !isTop()) {
            return ret;
        }
    }


    std::string objname = obj->getName();
    if (obj->getObjDepth()) {
        if (i != "i") {
            // Currently double layer structures are not supported (tracer regs)
            return ret;
        }
        loop = addspaces();
        loop += "for (int i = 0; i < " + obj->getStrDepth() + "; i++) {\n";
        pushspaces();
        loop += addspaces() + "char tstr[1024];\n";
    }

    if (obj->isStruct() && !obj->isInterface()) {
        if (prefix.size()) {
            prefix += ".";
        }
        prefix += objname;
        // VCD for each entry of the struct separetely
        const char tidx[2] = {static_cast<char>(static_cast<int>(i.c_str()[0]) + 1), 0};
        i = std::string(tidx);
        for (auto &e: obj->getEntries()) {
            ret += generate_sysc_vcd_entries(e, prefix, i, loop);
        }
    } else {
        std::string r = "";
        std::string r_ = "";
        if (obj->getClockEdge() != CLK_ALWAYS) {
            r = obj->r_prefix() + ".";
            r_ = obj->r_prefix() + "_";
        }

        if (prefix.size()) {
            prefix += ".";
        }
        if (loop.size()) {
            ret += loop;
            loop = "";
        }
        if (obj->getObjDepth()) {
            ret += addspaces();
            ret += "RISCV_sprintf(tstr, sizeof(tstr), \"%s." + r_ + prefix + objname + "%d\", pn.c_str(), i);\n";
            ret += addspaces() + "sc_trace(o_vcd, " + r + prefix + objname + "[i], tstr);\n";
        } else if (obj->getParent()
            && obj->getParent()->getObjDepth()) {
            ret += addspaces();
            ret += "RISCV_sprintf(tstr, sizeof(tstr), \"%s." + r_ + obj->getParent()->getName() + "%d_" + objname + "\", pn.c_str(), i);\n";
            ret += addspaces() + "sc_trace(o_vcd, " + r + obj->getParent()->getName() + "[i]." + objname + ", tstr);\n";
        } else if (r.size()) {
            ret += addspaces() + "sc_trace(o_vcd, " + r + prefix + objname + ", pn + \"." + r_ + prefix + objname + "\");\n";
        } else {
            ret += addspaces() + "sc_trace(o_vcd, " + prefix + objname + ", " + prefix + objname + ".name());\n";
        }
    }

    if (obj->getObjDepth()) {
        if (loop.size() == 0) {
            popspaces();
            ret += addspaces() + "}\n";
        } else {
            popspaces();
            loop = "";
        }
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
    if (isAsyncResetParam() && getAsyncResetParam() == 0) {
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
    // Sub-item constructors:
    tcnt = 0;
    for (auto &p: getEntries()) {
        if (p->isInput()
            || p->isOutput()
            || p->isClock()
            || p->isVector()) {
            ret += ",\n    " + p->getName() + "(\"" + p->getName() + "\"";
            if (p->isVector()) {
                ret += ", " + p->getStrDepth();
            } else if (p->isClock()) {
                ret += ", " + p->getStrValue();
                ret += ", SC_SEC";
            }
            ret += ")";
        }
    }
    ret += " {\n";
    ret += "\n";
    pushspaces();
    // local copy of the generic parameters:
    if (isAsyncResetParam() && getAsyncResetParam() == 0) {
        ret += addspaces() + "async_reset_ = async_reset;\n";
    }
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
    std::string prefix1 = "";
    std::string loop = "";
    std::string r;
    std::map<std::string, std::list<GenObject *>> regmap;
    std::map<std::string, bool> is2dm;
    std::list<std::string> regproclist;
    GenObject *rstport;
    GenObject *clkport;

    getSortedRegsMap(regmap, is2dm);
    for (std::map<std::string, std::list<GenObject *>>::iterator it = regmap.begin();
        it != regmap.end(); ++it) {
        regproclist.push_back(it->first + "egisters");
    }

    for (auto &p: getEntries()) {
        if (!p->isProcess()) {
            continue;
        }
        // Exclude process with name equals to <r>egisters process,
        // because content of such process should be added to trigger action
        if (std::find(regproclist.begin(), regproclist.end(), p->getName())
            != regproclist.end()) {
            continue;
        }
        ret += "\n";
        ret += addspaces() + "SC_METHOD(" + p->getName() + ");\n";

        ln = std::string("i");
        for (auto &s: getEntries()) {
            ret += generate_sysc_sensitivity(s, prefix1, ln, loop);
        }
    }
    // Generate registers processes depending of clock source and reset polarity
    for (std::map<std::string, std::list<GenObject *>>::iterator it = regmap.begin();
        it != regmap.end(); ++it) {
        ret += "\n";
        r = (*it->second.begin())->r_prefix();
        rstport = (*it->second.begin())->getResetPort();
        clkport = (*it->second.begin())->getClockPort();

        ret += addspaces() + "SC_METHOD(" + r + "egisters);\n";
        if (rstport) {
            ret += addspaces() + "sensitive << " + rstport->getName() + ";\n";
        }
        if (clkport->isInput()) {
            ret += addspaces() + "sensitive << " + clkport->getName() + ".pos();\n";
        } else {
            ret += addspaces() + "sensitive << " + clkport->getName() + ".posedge_event();\n";
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
    std::string ln = "";
    std::string ln2 = "";
    std::string loop = "";
    std::list<GenObject *> comblist;
    std::map<std::string, std::list<GenObject *>> regmap;
    std::map<std::string, bool> is2dm;

    getCombProcess(comblist);
    getSortedRegsMap(regmap,is2dm);

    ret += generate_sysc_template_f_name();
    ret += "::generateVCD(sc_trace_file *i_vcd, sc_trace_file *o_vcd) {\n";
    pushspaces();
    if (regmap.size() && comblist.size()) {
        ret += addspaces() + "std::string pn(name());\n";
    }
    ret += addspaces() + "if (o_vcd) {\n";
    pushspaces();
    for (auto &p: getEntries()) {
        ln2 = "i";
        ret += generate_sysc_vcd_entries(p, ln, ln2, loop);
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
        out += generate_sysc_func(p);
        out += "\n";
    }

    // Process
    getCombProcess(proclist);
    for (auto &p: proclist) {
        out += generate_sysc_proc(p);
    }

    out += generate_sysc_proc_registers();
    return out;
}

std::string ModuleObject::generate_sysc_func(GenObject *func) {
    std::string ret = "";
    ret += generate_sysc_template_f_name(func->getType().c_str()) + "::";
    ret += static_cast<FunctionObject *>(func)->generate();
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
        if (!e->isValue() && !e->isStruct()) {  // no need to check typedef inside of proc
            continue;
        }
        ln += addspaces() + e->getType() + " " + e->getName();
        if (e->getObjDepth()) {
            ln += "[" + e->getStrDepth() + "]";
        }
        ln += ";";
        e->addComment(ln);
        ret += ln + "\n";
        tcnt++;
    }
    if (tcnt) {
        ret += "\n";
        tcnt = 0;
    }

    // nullify all local variables to avoid latches:
    size_t ret_sz = ret.size();
    for (auto &e: proc->getEntries()) {
        ret += generate_all_proc_nullify(e, "", "i");
    }
    if (ret.size() != ret_sz) {
        ret += "\n";
    }

    tcnt = static_cast<int>(ret.size());
    ret += generate_all_proc_r_to_v(false);

    if (tcnt != static_cast<int>(ret.size())) {
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
