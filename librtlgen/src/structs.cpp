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

namespace sysvc {

StructObject::StructObject(GenObject *parent,
                           const char *type,
                           const char *name,
                           const char *val,
                           const char *comment)
    : GenObject(parent, type, ID_STRUCT, name, comment) {
    strValue_ = std::string(val);   // do not calculate getValue()
    //zeroval_ = std::string(val);
    if (getName() == "") {
        SHOW_ERROR("Unnamed structure of type %s", type_.c_str());
    } else if (getName() == type_) {
        SCV_set_cfg_type(this);
    } else if (!isLocal()) {
        SCV_set_cfg_parameter(this);   // global constant definition
    } else {
        SCV_get_cfg_parameter(type_);   // to trigger dependecy array
    }
}

/** it is better to remove empty name */
bool StructObject::isTypedef() {
    return getName() == "" || getName() == getType();
}

std::string StructObject::getStrValue() {
    std::string ret = "";
    if (!isParam()) {
        return strValue_;
    }
    int d = getDepth();
    if (SCV_is_sysc()) {
        ret += "{";
    } else if (SCV_is_sv()) {
        ret += "'{";
    } else if (SCV_is_vhdl()) {
        ret += "(";
    }

    std::string checktype = getType();
    if (isVector()) {
        checktype = getTypedef();
    }
    if (d > 1) {
        if (getEntries().back()->getType() != checktype
            || getEntries().size() < d) {
            SHOW_ERROR("Wrong struct param definition %d < %d",
                        d, getEntries().size());
        }
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
        for (auto &p: getEntries()) {
            if (!p->isValue() && !p->isStruct()) {
                continue;
            }
            ret += p->getStrValue();
            if (p != getEntries().back()) {
                ret += ", ";
            }
        }
    }


    if (SCV_is_sysc()) {
        ret += "}";
    } else if (SCV_is_sv()) {
        ret += "}";
    } else if (SCV_is_vhdl()) {
        ret += ")";
    }
    return ret;
}


std::string StructObject::generate_interface_constructor() {
    std::string ret = "";

    ret += addspaces() + getType() + "() {\n";
    pushspaces();
    std::list<GenObject *> *pentries = &getEntries();
    std::string prefix = "";
    if (isVector()) {
        pentries = &getItem()->getEntries();
        prefix = getEntries().front()->getName() + "[i].";

        ret += addspaces();
        ret += "for (int i = 0; i < " + getEntries().front()->getStrDepth() + "; i++) {\n";
        pushspaces();
    }
    for (auto& p : *pentries) {
        if (p->getId() == ID_COMMENT) {
            ret += p->generate();
        } else if (p->isStruct() && !p->isTypedef()) {
            // 1 level of sub structures without recursive implementation
            for (auto &p2 : p->getEntries()) {
                if (p2->getId() == ID_COMMENT) {
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
        if (p->getId() == ID_COMMENT) {
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
        if (p->getId() == ID_COMMENT) {
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

std::string StructObject::generate_interface_op_equal() {
    std::string ret = "";
    int tcnt = 0;

    ret += addspaces() + "inline bool operator == (const " + getType() + " &rhs) const {\n";
    pushspaces();
    ret += addspaces() + "bool ret = true;\n";

    std::list<GenObject *> *pentries = &getEntries();
    std::string prefix = "";
    if (isVector()) {
        pentries = &getItem()->getEntries();
        prefix = getEntries().front()->getName() + "[i].";

        ret += addspaces();
        ret += "for (int i = 0; i < " + getEntries().front()->getStrDepth() + "; i++) {\n";
        pushspaces();
    }

    ret += addspaces() + "ret = ret";
    pushspaces();
    for (auto& p : *pentries) {
        if (p->getId() == ID_COMMENT) {
        } else if (p->isStruct() && !p->isTypedef()) {
            // 1 level of sub structures without recursive implementation
            for (auto &p2 : p->getEntries()) {
                if (p2->getId() == ID_COMMENT) {
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

std::string StructObject::generate_interface_op_assign() {
    std::string ret = "";

    ret += addspaces() + "inline " + getType() + "& operator = (const " + getType() + " &rhs) {\n";
    pushspaces();

    std::list<GenObject *> *pentries = &getEntries();
    std::string prefix = "";
    if (isVector()) {
        pentries = &getItem()->getEntries();
        prefix = getEntries().front()->getName() + "[i].";

        ret += addspaces();
        ret += "for (int i = 0; i < " + getEntries().front()->getStrDepth() + "; i++) {\n";
        pushspaces();
    }
    for (auto& p : *pentries) {
        if (p->getId() == ID_COMMENT) {
        } else if (p->isStruct() && !p->isTypedef()) {
            // 1 level of sub structures without recursive implementation
            for (auto &p2 : p->getEntries()) {
                if (p2->getId() == ID_COMMENT) {
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

    std::list<GenObject *> *pentries = &getEntries();
    std::string prefix = "";
    if (isVector()) {
        pentries = &getItem()->getEntries();
        prefix = getEntries().front()->getName() + "[i].";

        ret += addspaces();
        ret += "for (int i = 0; i < " + getEntries().front()->getStrDepth() + "; i++) {\n";
        pushspaces();
    }
    ret += addspaces() + "os << \"(\"\n";
    for (auto& p : *pentries) {
        if (p->getId() == ID_COMMENT) {
            // do nothing
        } else if (p->isStruct() && !p->isTypedef()) {
            // 1 level of sub structures without recursive implementation
            for (auto &p2 : p->getEntries()) {
                if (p2->getId() == ID_COMMENT) {
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

    std::list<GenObject *> *pentries = &getEntries();
    std::string prefix = "";
    std::string N_plus = "";
    if (isVector()) {
        pentries = &getItem()->getEntries();
        prefix = getEntries().front()->getName() + "[i].";
        N_plus = "N + ";

        ret += addspaces() + "char N[16];\n";
        ret += addspaces();
        ret += "for (int i = 0; i < " + getEntries().front()->getStrDepth() + "; i++) {\n";
        pushspaces();
        ret += addspaces() + "sprintf(N, \"(%d)\", i);\n";
    }
    for (auto& p : *pentries) {
        if (p->getId() == ID_COMMENT) {
            // do nothing
        } else if (p->isStruct() && !p->isTypedef()) {
            // 1 level of sub structures without recursive implementation
            for (auto &p2 : p->getEntries()) {
                if (p2->getId() == ID_COMMENT) {
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
        if (isInitable()) {
            ret += generate_interface_constructor_init();
        }
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
        if (p->getId() == ID_COMMENT) {
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

std::string StructObject::generate_const_none() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret += addspaces();
        ret += "static const " + getType() + " " + getName() + ";\n";
    } else if (SCV_is_sv()) {
        ret += addspaces();
        ret += "const " + getType() + " " + getName() + " = '{\n";
        pushspaces();
        for (auto& p : entries_) {
            if (p->getId() == ID_COMMENT) {
                continue;
            }
            ret += addspaces();
            std::string strvalue = p->getStrValue();
            if (p->isNumber(strvalue)
                && p->getWidth() > 1 && p->getValue() == 0) {
                ret += "'0";
            } else {
                ret += strvalue;
            }
            if (p != entries_.back()) {
                ret += ",";
            }
            ret += "  // " + p->getName() + "\n";
        }
        popspaces();
        ret += addspaces() + "};\n";
    } else if (SCV_is_vhdl()) {
        ret += addspaces();
        ret += "constant " + getName() + " : " + getType() + " := (\n";
        pushspaces();
        for (auto& p : entries_) {
            if (p->getId() == ID_COMMENT) {
                continue;
            }
            ret += addspaces();
            std::string strvalue = p->getStrValue();
            if (p->isNumber(strvalue)
                && p->getWidth() > 1 && p->getValue() == 0) {
                ret += "(others => '0')";
            } else {
                ret += strvalue;
            }
            if (p != entries_.back()) {
                ret += ",";
            }
            ret += "  -- " + p->getName() + "\n";
        }
        popspaces();
        ret += addspaces() + ");\n";
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
        if (getDepth() > 1) {
            ret += "[" + getStrDepth() + "]";
        }
        ret += " = ";
    } else if (SCV_is_sv()) {
        ret += "const " + getType() + " " + getName();
        if (!isVector() && getDepth() > 1) {
            ret += "[" + getStrDepth() + "]";
        }
        ret += " = ";
    } else if (SCV_is_vhdl()) {
        ret += "constant " + getName() + "of " + getType() + " := ";
    }

    ret += getStrValue() + ";\n";
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
    if (getParent()->getId() == ID_FILE) {
        if (!isTypedef()) {
            ret = generate_const_none();
        } else {
            ret = generate_interface();
        }
        return ret;
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
        if (p->getId() == ID_COMMENT) {
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
        if (p->getDepth() > 1) {
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

}
