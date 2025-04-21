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

#include "structs.h"
#include "files.h"
#include "utils.h"
#include "array.h"
#include "operations.h"
#include <cstring>

namespace sysvc {

//extern void detect_vr_prefixes(GenObject *obj, std::string &v, std::string &r);

StructObject::StructObject(GenObject *parent,
                           GenObject *clk,
                           EClockEdge edge,
                           GenObject *nrst,
                           EResetActive active,
                           const char *type,
                           const char *name,
                           const char *rstval,
                           const char *comment)
    : GenValue(parent, clk, edge, nrst, active, name, rstval, comment) {
    type_ = std::string(type);
    name_ = std::string(name);
    if (name_ == "") {
        SHOW_ERROR("Unnamed structure of type %s", type_.c_str());
    }
    if (name_ == type_) {
        SCV_set_cfg_type(this);
    } else {
        SCV_get_cfg_type(this, type_.c_str());
    }
    // See signals.cpp
    //detect_vr_prefixes(this, v_, r_);
}

StructObject::StructObject(GenObject *parent,
                           const char *type,
                           const char *name,
                           const char *comment)
    : StructObject(parent, 0, CLK_ALWAYS, 0, ACTIVE_NONE, type, name, "", comment) {
}

bool StructObject::isTypedef() {
    return getName() == getType();
}

/** No need in this method, all structures define in file automatically are interfaces */
bool StructObject::isInterface() {
    GenObject *p;
    if (!isTypedef()) {
        p = SCV_get_cfg_type(this, type_.c_str());
        if (p) {
            p = p->getParent();
        }
    } else {
        p = getParent();
    }
    if (p && p->isFile()) {
        return true;
    }
    return false;
}

std::string StructObject::getType() {
    std::string ret = type_;
    if (SCV_is_sv_pkg()) {
        GenObject *pfile = SCV_get_cfg_type(this, ret.c_str());
        while (pfile && !pfile->isFile()) {
            pfile = pfile->getParent();
        }
        if (pfile) {
            ret = pfile->getName() + "_pkg::" + ret;
        }
    }
    return ret;
}

std::string StructObject::getStrValue() {
    std::string ret = "";
    if (objValue_) {
        return objValue_->getName();
    }

    if (SCV_is_sysc()) {
        ret += "{\n";
    } else if (SCV_is_sv()) {
        ret += "'{\n";
    } else if (SCV_is_vhdl()) {
        ret += "(\n";
    }
    pushspaces();

    std::string checktype = getType();
    if (isVector()) {
        checktype = getTypedef();
    }
    if (getObjDepth()) {
        int d = static_cast<int>(getDepth());
        ret += "\n";
        pushspaces();
        int tcnt = 0;
        std::string ln;
        for (auto &p: getEntries()) {
            if (p->getType() != checktype) {
                // Only the same type entry should be generated
                continue;
            }
            ln = addspaces() + p->getStrValue();
            if (++tcnt < d) {
                ln += ",";
            }
            p->addComment(ln);
            ret += ln + "\n";
        }
        popspaces();
    } else {
            /*pushspaces();
            for (auto &p: it->second) {
                ln = addspaces() + p->getStrValue();
                if (p != it->second.back()) {
                    ln += ",";
                }
                while (ln.size() < 40) {
                    ln += " ";
                }
                ln += "// " + p->getName();
                ret += ln + "\n";
            }
            popspaces();*/

        std::string ln;
        for (auto &p: getEntries()) {
            if (!p->isValue()) {
                continue;
            }
            ln = addspaces();
            if (p->getObjDepth()) { 
                ln += "{}";
                SHOW_ERROR("2-dim constant %s:%s",
                    getName().c_str(), p->getName().c_str());
            } else {
                ln += p->getStrValue();
            }
            if (p != getEntries().back()) {
                ln += ", ";
            }
            while (ln.size() < 40) {
                ln += " ";
            }
            ln += "// " + p->getName();
            ret += ln + "\n";
        }
    }

    popspaces();
    ret += addspaces();
    if (SCV_is_sysc()) {
        ret += "}";
    } else if (SCV_is_sv()) {
        ret += "}";
    } else if (SCV_is_vhdl()) {
        ret += ")";
    }
    return ret;
}

bool StructObject::is2Dim() {
    if (getDepth()) {
        return true;
    }
    for (auto &p: getEntries()) {
        if (p->is2Dim()) {
            return true;
        }
    }
    return false;
}

std::string StructObject::getCopyValue(char *i,
                                       const char *dst_prefix,
                                       const char *optype,
                                       const char *src_prefix) {
    std::string ret;
    std::string dst = dst_prefix;
    std::string src = src_prefix;
    if (dst.size()) {
        dst += "." + getName();
    }
    if (src.size()) {
        src += "." + getName();
    }
    if (objValue_ && !is2Dim()) {
        // reset value request:
        ret = addspaces() + dst_prefix + " " + optype + " ";
        ret += objValue_->getName() + ";\n";
        return ret;
    }

    if (SCV_is_sysc() || SCV_is_sv()) {
        char i_idx[2] = {i[0]};
        if (getDepth()) {
            ret += addspaces() + "for (int " + i + " = 0; " + i + " < ";
            ret += getStrDepth() + "; " + i + "++) ";
            if (SCV_is_sysc()) {
                ret += "{\n";
            } else {
                ret += "begin\n";
            }
            dst += "[" + std::string(i) + "]";
            if (src.size()) {
                src += "[" + std::string(i) + "]";
            }
            pushspaces();
            i_idx[0]++;
        }

        for (auto &p: getEntries()) {
            ret += p->getCopyValue(i_idx, dst.c_str(), optype, src.c_str());
        }

        if (getDepth()) {
            popspaces();
            if (SCV_is_sysc()) {
                ret += addspaces() + "}\n";
            } else {
                ret += addspaces() + "end\n";
            }
        }

    }
    return ret;
}

std::string StructObject::generate_interface_constructor() {
    std::string ret = "";

    ret += addspaces() + getType() + "() {\n";
    pushspaces();
    std::string prefix = "";
    if (isVector()) {
        prefix = getEntries().front()->getName() + "[i].";

        ret += addspaces();
        ret += "for (int i = 0; i < " + getEntries().front()->getStrDepth() + "; i++) {\n";
        pushspaces();
    }
    for (auto& p : getEntries()) {
        if (p->isComment()) {
            ret += p->generate();
        } else if (p->isStruct() && !p->isTypedef()) {
            // 1 level of sub structures without recursive implementation
            for (auto &p2 : p->getEntries()) {
                if (p2->isComment()) {
                    continue;
                }
                ret += addspaces();
                ret += prefix + p->getName() + "." + p2->getName();
                ret += " = " + p2->getStrValue() + ";\n";
            }
        } else {
            ret += addspaces() + prefix + p->getName();
            ret += " = " + p->getStrValue() + ";\n";
        }
    }
    if (isVector()) {
        popspaces();
        ret += addspaces() + "}\n";
    }
    popspaces();
    ret += addspaces() + "}\n";
    ret += "\n";

    return ret;
}

std::string StructObject::generate_interface_constructor_init() {
    std::string ret = "";
    int argcnt = 0;
    int aligncnt;

    ret += addspaces() + getType() + "(";
    aligncnt = static_cast<int>(ret.size());
    for (auto& p : getEntries()) {
        if (p->isComment()) {
            continue;
        }
        if (argcnt++) {
            ret += ",\n";
            for (int i = 0; i < aligncnt; i++) {
                ret += " ";
            }
        }
        ret += p->getType() + " " + p->getName() + "_";
    }
    ret += ") {\n";
    pushspaces();
    for (auto& p : getEntries()) {
        if (p->isComment()) {
            continue;
        }
        ret += addspaces();
        ret += p->getName() + " = " + p->getName() + "_;\n";
    }
    popspaces();
    ret += addspaces() + "}\n";
    ret += "\n";

    return ret;
}

/**
    Redefine operator ==
*/
std::string StructObject::generate_interface_op_equal() {
    std::string ret = "";
    int tcnt = 0;

    ret += addspaces() + "inline bool operator == (const " + getType() + " &rhs) const {\n";
    pushspaces();
    ret += addspaces() + "bool ret = true;\n";

    std::string prefix = "";
    if (isVector()) {
        prefix = getEntries().front()->getName() + "[i].";

        ret += addspaces();
        ret += "for (int i = 0; i < " + getEntries().front()->getStrDepth() + "; i++) {\n";
        pushspaces();
    }

    ret += addspaces() + "ret = ret";
    pushspaces();
    for (auto& p : getEntries()) {
        if (p->isComment()) {
        } else if (p->isStruct() && !p->isTypedef()) {
            // 1 level of sub structures without recursive implementation
            for (auto &p2 : p->getEntries()) {
                if (p2->isComment()) {
                    continue;
                }
                ret += "\n" + addspaces() + "&& ";
                ret += "rhs." + prefix + p->getName() + "." + p2->getName() + " == ";
                ret += prefix + p->getName() + "." + p2->getName();
            }
        } else {
            ret += "\n" + addspaces() + "&& ";
            ret += "rhs." + prefix + p->getName() + " == " + prefix + p->getName();
        }
    }
    ret += ";\n";
    popspaces();
    if (isVector()) {
        popspaces();
        ret += addspaces() + "}\n";
    }
    ret += addspaces() + "return ret;\n";
    popspaces();
    ret += addspaces() + "}\n";
    ret += "\n";

    return ret;
}

/**
    Redefine operator =
 */
std::string StructObject::generate_interface_op_assign() {
    std::string ret = "";

    ret += addspaces() + "inline " + getType() + "& operator = (const " + getType() + " &rhs) {\n";
    pushspaces();

    std::string prefix = "";
    if (isVector()) {
        prefix = getEntries().front()->getName() + "[i].";

        ret += addspaces();
        ret += "for (int i = 0; i < " + getEntries().front()->getStrDepth() + "; i++) {\n";
        pushspaces();
    }
    for (auto& p : getEntries()) {
        if (p->isComment()) {
        } else if (p->isStruct() && !p->isTypedef()) {
            // 1 level of sub structures without recursive implementation
            for (auto &p2 : p->getEntries()) {
                if (p2->isComment()) {
                    continue;
                }
                ret += addspaces();
                ret += prefix + p->getName() + "." + p2->getName();
                ret += " = rhs." + prefix + p->getName() + "." + p2->getName() + ";\n";
            }
        } else {
            ret += addspaces();
            ret += prefix + p->getName() + " = rhs." + prefix + p->getName() + ";\n";
        }
    }
    if (isVector()) {
        popspaces();
        ret += addspaces() + "}\n";
    }
    ret += addspaces() + "return *this;\n";
    popspaces();
    ret += addspaces() + "}\n";
    ret += "\n";

    return ret;
}

std::string StructObject::generate_interface_op_bracket() {
    std::string ret = "";
    ret += addspaces() + getEntries().front()->getType();
    ret += " operator [](int i) const { return ";
    ret += getEntries().front()->getName() + "[i]; ";
    ret += "}\n";

    ret += addspaces() + getEntries().front()->getType();
    ret += " &operator [](int i) { return ";
    ret += getEntries().front()->getName() + "[i]; ";
    ret += "}\n";
    return ret;
}

/**
    Redefine operator <<
 */
std::string StructObject::generate_interface_op_stream() {
    std::string ret = "";
    int tcnt = 0;
    std::string ln;
    size_t tspace;

    ln = addspaces() + "inline friend ostream &operator << (";
    tspace = ln.size();
    ret = ln + "ostream &os,\n";
    ln = "";
    while (ln.size() < tspace) {
        ln += " ";
    }
    ret += ln + getType() + " const &v) {\n";
    pushspaces();

    std::string prefix = "";
    if (isVector()) {
        prefix = getEntries().front()->getName() + "[i].";

        ret += addspaces();
        ret += "for (int i = 0; i < " + getEntries().front()->getStrDepth() + "; i++) {\n";
        pushspaces();
    }
    ret += addspaces() + "os << \"(\"\n";
    for (auto& p : getEntries()) {
        if (p->isComment()) {
            // do nothing
        } else if (p->isStruct() && !p->isTypedef()) {
            // 1 level of sub structures without recursive implementation
            for (auto &p2 : p->getEntries()) {
                if (p2->isComment()) {
                    continue;
                }
                if (tcnt++) {
                    ret += " << \",\"\n";
                }
                ret += addspaces();
                ret += "<< v." + prefix + p->getName() + "." + p2->getName();
            }
        } else {
            if (tcnt++) {
                ret += " << \",\"\n";
            }
            ret += addspaces();
            ret += "<< v." + prefix + p->getName();
        }
    }
    ret += " << \")\";\n";
    if (isVector()) {
        popspaces();
        ret += addspaces() + "}\n";
    }
    ret += addspaces() + "return os;\n";
    popspaces();
    ret += addspaces() + "}\n";
    ret += "\n";

    return ret;
}

/**
    Redefine method sc_trace(..)
 */
std::string StructObject::generate_interface_sc_trace() {
    std::string ret = "";
    std::string ln;
    size_t tspace;

    ln = addspaces() + "inline friend void sc_trace(";
    tspace = ln.size();
    ret = ln + "sc_trace_file *tf,\n";
    ln = "";
    while (ln.size() < tspace) {
        ln += " ";
    }
    ret += ln + "const " + getType() + "&v,\n";
    ret += ln + "const std::string &NAME) {\n";
    pushspaces();

    std::string prefix = "";
    std::string N_plus = "";
    if (isVector()) {
        prefix = getEntries().front()->getName() + "[i].";
        N_plus = "N + ";

        ret += addspaces() + "char N[16];\n";
        ret += addspaces();
        ret += "for (int i = 0; i < " + getEntries().front()->getStrDepth() + "; i++) {\n";
        pushspaces();
        ret += addspaces() + "sprintf(N, \"(%d)\", i);\n";
    }
    for (auto& p : getEntries()) {
        if (p->isComment()) {
            // do nothing
        } else if (p->isStruct() && !p->isTypedef()) {
            // 1 level of sub structures without recursive implementation
            for (auto &p2 : p->getEntries()) {
                if (p2->isComment()) {
                    continue;
                }
                ret += addspaces();
                ret += "sc_trace(tf, v." + prefix + p->getName() + "." + p2->getName() + ", NAME + ";
                ret += N_plus + "\"_" + p->getName() + "_" + p2->getName() + "\");\n";
            }
        } else {
            ret += addspaces();
            ret += "sc_trace(tf, v." + prefix + p->getName() + ", NAME + ";
            ret += N_plus + "\"_" + p->getName() + "\");\n";
        }
    }
    if (isVector()) {
        popspaces();
        ret += addspaces() + "}\n";
    }
    popspaces();
    ret += addspaces() + "}\n";
    ret += "\n";

    return ret;
}


std::string StructObject::generate_interface() {
    std::string ret = "";
    std::string ln;

    if (getComment().size()) {
        ret += addspaces() + addComment() + "\n";
    }
    if (SCV_is_sysc()) {
        ret += addspaces() + "class " + getType() + " {\n";
        ret += addspaces() + " public:\n";

        pushspaces();
        ret += generate_interface_constructor();
        ret += generate_interface_constructor_init();
        ret += generate_interface_op_equal();
        ret += generate_interface_op_assign();
        ret += generate_interface_sc_trace();
        ret += generate_interface_op_stream();
        if (isVector()) {
            ret += generate_interface_op_bracket();
        }
        popspaces();
        ret += addspaces() + " public:\n";
    } else if (SCV_is_sv()) {
        ret += addspaces() + "typedef struct {\n";
    } else if (SCV_is_vhdl()) {
        ret += addspaces() + "type " + getType() + " is record\n";
    }
    pushspaces();
    for (auto& p : getEntries()) {
        ln = addspaces();
        if (p->isComment()) {
            ret += p->generate();
            continue;
        }
        if (SCV_is_vhdl()) {
            ln += p->getName() + " : " + p->getType();
        } else {
            ln += p->getType() + " " + p->getName();
        }
        if (p->getDepth()) {
            if (SCV_is_sysc()) {
                ln += "[" + p->getStrDepth() + "]";
            } else if (SCV_is_sv()) {
                ln += "[0: " + p->getStrDepth() + " - 1]";
            } else if (SCV_is_vhdl()) {
                ln += "(0 up " + p->getStrDepth() + " - 1)";
            }
        }
        ln += ";";
        p->addComment(ln);
        ret += ln + "\n";
    }
    popspaces();

    ret += addspaces();
    if (SCV_is_sysc()) {
        ret += "};\n";
    } else if (SCV_is_sv()) {
        ret += "} " + getType() + ";\n";
    } else if (SCV_is_vhdl()) {
        ret += "end record;\n";
    }
    return ret;
}


std::string StructObject::generate_vector_type() {
    std::string ret;
    if (SCV_is_sysc()) {
        ret += addspaces();
        ret += "typedef sc_vector<";
        if (isSignal()) {
            ret += "sc_signal<";
        }
        ret += getTypedef();
        if (isSignal()) {
            ret += ">";
        }
        ret += "> " + getType() + ";\n";
    } else if (SCV_is_sv()) {
        ret += addspaces();
        ret += "typedef " + getTypedef() + " " + getType();
        ret += "[0:" + getStrDepth() + " - 1];\n";
    } else if (SCV_is_vhdl()) {
    }
    return ret;
}

std::string StructObject::generate_param() {
    std::string ret = "";
    ret += addspaces();
    if (SCV_is_sysc()) {
        ret += "static const ";
        if (isVector()) {
            ret += getTypedef();
        } else {
            ret += getType();
        }
        ret += " " + getName();
        if (getObjDepth()) {
            ret += "[" + getStrDepth() + "]";
        }
        // do not call getStrValue() for sysc structure containing Logic
        // because we cannot directly assign value to template sc_uint<*>
        // But we should do that for BUSx_MAP constants definitions:
        bool islogic = false;
        for (auto &p: getEntries()) {
            if (p->isLogic()) {
                islogic = true;
                break;
            }
        }
        if (!islogic) {
            ret += " = " + getStrValue();
        }
        ret += ";\n";
    } else if (SCV_is_sv()) {
        ret += "const " + getType() + " " + getName();
        if (!isVector() && getObjDepth()) {
            ret += "[" + getStrDepth() + "]";
        }
        ret += " = ";
        ret += getStrValue() + ";\n";
    } else if (SCV_is_vhdl()) {
        ret += "constant " + getName() + "of " + getType() + " := ";
        ret += getStrValue() + ";\n";
    }
    return ret;
}

std::string StructObject::generate() {
    std::string ret = "";
    std::string ln;

    if (isParam()) {
        return generate_param();
    }
    if (isVector()) {
        return generate_vector_type();
    }
    if (getParent()->isFile()) {
        return generate_interface();
    }

    if (getComment().size()) {
        ret += addspaces();
        ret += addComment() + "\n";
    }
    ret += addspaces();
    if (SCV_is_sysc()) {
        ret += "struct " + getType() + " {\n";
    } else if (SCV_is_sv()) {
        ret += "typedef struct {\n";
    } else if (SCV_is_vhdl()) {
        ret += "type " + getType() + " is record\n";
    }
    pushspaces();
    for (auto &p: entries_) {
        ln = addspaces();
        if (p->isComment()) {
            ret += p->generate();
            continue;
        }
        if (SCV_is_sysc()) {
            if (p->isVector()) {
                ln += "sc_vector<";
            }
            if (p->isSignal() && !p->isIgnoreSignal()) {
                ln += "sc_signal<";
            }
            ln += p->getType();
            if (p->isSignal() && !p->isIgnoreSignal()) {
                ln += ">";
            }
            if (p->isVector()) {
                ln += ">";
            }
            ln += " " + p->getName();
        } else if (SCV_is_sv()) {
            ln += p->getType() + " " + p->getName();
        } else if (SCV_is_vhdl()) {
            ln += p->getName() + " :" +  p->getType();
        }
        if (p->getObjDepth()) {
            if (SCV_is_sysc()) {
                ln += "[" + p->getStrDepth() + "]";
            } else if (SCV_is_sv()) {
                ln += "[0: " + p->getStrDepth() + " - 1]";
            } else if (SCV_is_vhdl()) {
                ln += "(0 up " + p->getStrDepth() + " - 1)";
            }
        }
        ln += ";";
        p->addComment(ln);
        ret += ln + "\n";
    }
    popspaces();
    ret += addspaces();
    if (SCV_is_sysc()) {
        ret += "};\n";
    } else if (SCV_is_sv()) {
        ret += "} " + getType() +";\n";
    } else if (SCV_is_vhdl()) {
        ret += "end record;\n";
    }
    ret += "\n";
    return ret;
}

RegTypedefStruct::RegTypedefStruct(GenObject *parent,
                                   GenObject *clk,
                                   EClockEdge edge,
                                   GenObject *rst,
                                   EResetActive active,
                                   const char *type,
                                   const char *rstval)
    : StructObject(parent, clk, edge, rst, active,
                   type, type, rstval, NO_COMMENT), v_(0), rin_(0), r_(0) {
}

RegSignalInstance::RegSignalInstance(GenObject *parent,
                                 RegTypedefStruct *p,
                                 const char *name,
                                 const char *rstval)
    : StructObject(parent, 0, CLK_POSEDGE, 0, ACTIVE_NONE,
                   p->getType().c_str(), name, rstval, NO_COMMENT),
                   rstruct_(p) {
}

RegResetStruct::RegResetStruct(GenObject *parent,
                                RegTypedefStruct *p,
                                const char *name)
    : RegSignalInstance(parent, p, name, "") {
}


}
