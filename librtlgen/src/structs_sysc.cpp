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
#include "proc.h"
#include <cstring>

namespace sysvc {

std::string StructObject::generate_interface_sc_constructor() {
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

std::string StructObject::generate_interface_sc_constructor_init() {
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
std::string StructObject::generate_interface_sc_op_equal() {
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
std::string StructObject::generate_interface_sc_op_assign() {
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

std::string StructObject::generate_interface_sc_op_bracket() {
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
std::string StructObject::generate_interface_sc_op_stream() {
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

}
