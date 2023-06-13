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
                           int idx,
                           const char *comment)
    : GenObject(parent, type,
                idx != -1 ? ID_STRUCT_INST 
                          : name[0] ? ID_STRUCT_INST : ID_STRUCT_DEF, name, comment) {
    idx_ = idx;
    zeroval_ = "";
    if (idx != -1) {
        char tstr[256];
        RISCV_sprintf(tstr, sizeof(tstr), "%d", idx);
        name_ = std::string(tstr);
    }
}

bool StructObject::isVector() {
    if (getEntries().size() != 1) {
        return false;
    }
    if (getEntries().front()->getId() == ID_ARRAY_DEF) {
        return true;
    }
    return false;
}

std::string StructObject::getName() {
    std::string ret = GenObject::getName();
    if (idx_ == -1) {
        return ret;
    }
    GenObject *sel = getParent()->getSelector();
    if (!sel) {
        return ret;
    }
    if (sel->getId() == ID_CONST) {
        ret = sel->getStrValue();
    } else if (sel->getId() == ID_OPERATION) {
        ret = sel->generate();
    } else {
        ret = sel->getName();
    }
    return ret;
}

std::string StructObject::getStrValue() {
    std::string ret = zeroval_;
    //if (ret.size() == 0) {
        //SHOW_ERROR("todo: struct initialization", getName());
    //}
    return ret;
}

std::string StructObject::generate_interface_constructor() {
    std::string ret = "";

    ret += Operation::addspaces() + getType() + "() {\n";
    Operation::set_space(Operation::get_space() + 1);
    std::list<GenObject *> *pentries = &getEntries();
    std::string prefix = "";
    if (isVector()) {
        pentries = &getItem()->getEntries();
        prefix = getEntries().front()->getName() + "[i].";

        ret += Operation::addspaces();
        ret += "for (int i = 0; i < " + getEntries().front()->getStrDepth() + "; i++) {\n";
        Operation::set_space(Operation::get_space() + 1);
    }
    for (auto& p : *pentries) {
        if (p->getId() == ID_COMMENT) {
            ret += Operation::addspaces() + "// " + p->getName() + "\n";
        } else if (p->getId() == ID_STRUCT_INST) {
            // 1 level of sub structures without recursive implementation
            for (auto &p2 : p->getEntries()) {
                if (p2->getId() == ID_COMMENT) {
                    continue;
                }
                ret += Operation::addspaces();
                ret += prefix + p->getName() + "." + p2->getName();
                ret += " = " + p2->getStrValue() + ";\n";
            }
        } else {
            ret += Operation::addspaces() + prefix + p->getName();
            ret += " = " + p->getStrValue() + ";\n";
        }
    }
    if (isVector()) {
        Operation::set_space(Operation::get_space() - 1);
        ret += Operation::addspaces() + "}\n";
    }
    Operation::set_space(Operation::get_space() - 1);
    ret += Operation::addspaces() + "}\n";
    ret += "\n";

    return ret;
}

std::string StructObject::generate_interface_constructor_init() {
    std::string ret = "";
    int argcnt = 0;
    int aligncnt;

    ret += Operation::addspaces() + getType() + "(";
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
    Operation::set_space(Operation::get_space() + 1);
    for (auto& p : getEntries()) {
        if (p->getId() == ID_COMMENT) {
            continue;
        }
        ret += Operation::addspaces();
        ret += p->getName() + " = " + p->getName() + "_;\n";
    }
    Operation::set_space(Operation::get_space() - 1);
    ret += Operation::addspaces() + "}\n";
    ret += "\n";

    return ret;
}

std::string StructObject::generate_interface_op_equal() {
    std::string ret = "";
    int tcnt = 0;

    ret += Operation::addspaces() + "inline bool operator == (const " + getType() + " &rhs) const {\n";
    Operation::set_space(Operation::get_space() + 1);
    ret += Operation::addspaces() + "bool ret = true;\n";

    std::list<GenObject *> *pentries = &getEntries();
    std::string prefix = "";
    if (isVector()) {
        pentries = &getItem()->getEntries();
        prefix = getEntries().front()->getName() + "[i].";

        ret += Operation::addspaces();
        ret += "for (int i = 0; i < " + getEntries().front()->getStrDepth() + "; i++) {\n";
        Operation::set_space(Operation::get_space() + 1);
    }

    ret += Operation::addspaces() + "ret = ret";
    Operation::set_space(Operation::get_space() + 1);
    for (auto& p : *pentries) {
        if (p->getId() == ID_COMMENT) {
        } else if (p->getId() == ID_STRUCT_INST) {
            // 1 level of sub structures without recursive implementation
            for (auto &p2 : p->getEntries()) {
                if (p2->getId() == ID_COMMENT) {
                    continue;
                }
                ret += "\n" + Operation::addspaces() + "&& ";
                ret += "rhs." + prefix + p->getName() + "." + p2->getName() + " == ";
                ret += prefix + p->getName() + "." + p2->getName();
            }
        } else {
            ret += "\n" + Operation::addspaces() + "&& ";
            ret += "rhs." + prefix + p->getName() + " == " + prefix + p->getName();
        }
    }
    ret += ";\n";
    Operation::set_space(Operation::get_space() - 1);
    if (isVector()) {
        Operation::set_space(Operation::get_space() - 1);
        ret += Operation::addspaces() + "}\n";
    }
    ret += Operation::addspaces() + "return ret;\n";
    Operation::set_space(Operation::get_space() - 1);
    ret += Operation::addspaces() + "}\n";
    ret += "\n";

    return ret;
}

std::string StructObject::generate_interface_op_assign() {
    std::string ret = "";

    ret += Operation::addspaces() + "inline " + getType() + "& operator = (const " + getType() + " &rhs) {\n";
    Operation::set_space(Operation::get_space() + 1);

    std::list<GenObject *> *pentries = &getEntries();
    std::string prefix = "";
    if (isVector()) {
        pentries = &getItem()->getEntries();
        prefix = getEntries().front()->getName() + "[i].";

        ret += Operation::addspaces();
        ret += "for (int i = 0; i < " + getEntries().front()->getStrDepth() + "; i++) {\n";
        Operation::set_space(Operation::get_space() + 1);
    }
    for (auto& p : *pentries) {
        if (p->getId() == ID_COMMENT) {
        } else if (p->getId() == ID_STRUCT_INST) {
            // 1 level of sub structures without recursive implementation
            for (auto &p2 : p->getEntries()) {
                if (p2->getId() == ID_COMMENT) {
                    continue;
                }
                ret += Operation::addspaces();
                ret += prefix + p->getName() + "." + p2->getName();
                ret += " = rhs." + prefix + p->getName() + "." + p2->getName() + ";\n";
            }
        } else {
            ret += Operation::addspaces();
            ret += prefix + p->getName() + " = rhs." + prefix + p->getName() + ";\n";
        }
    }
    if (isVector()) {
        Operation::set_space(Operation::get_space() - 1);
        ret += Operation::addspaces() + "}\n";
    }
    ret += Operation::addspaces() + "return *this;\n";
    Operation::set_space(Operation::get_space() - 1);
    ret += Operation::addspaces() + "}\n";
    ret += "\n";

    return ret;
}

std::string StructObject::generate_interface_op_bracket() {
    std::string ret = "";
    ret += Operation::addspaces() + getEntries().front()->getType();
    ret += " operator [](int i) const { return ";
    ret += getEntries().front()->getName() + "[i]; ";
    ret += "}\n";

    ret += Operation::addspaces() + getEntries().front()->getType();
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

    ln = Operation::addspaces() + "inline friend ostream &operator << (";
    tspace = ln.size();
    ret = ln + "ostream &os,\n";
    ln = "";
    while (ln.size() < tspace) {
        ln += " ";
    }
    ret += ln + getType() + " const &v) {\n";
    Operation::set_space(Operation::get_space() + 1);

    std::list<GenObject *> *pentries = &getEntries();
    std::string prefix = "";
    if (isVector()) {
        pentries = &getItem()->getEntries();
        prefix = getEntries().front()->getName() + "[i].";

        ret += Operation::addspaces();
        ret += "for (int i = 0; i < " + getEntries().front()->getStrDepth() + "; i++) {\n";
        Operation::set_space(Operation::get_space() + 1);
    }
    ret += Operation::addspaces() + "os << \"(\"\n";
    for (auto& p : *pentries) {
        if (p->getId() == ID_COMMENT) {
            // do nothing
        } else if (p->getId() == ID_STRUCT_INST) {
            // 1 level of sub structures without recursive implementation
            for (auto &p2 : p->getEntries()) {
                if (p2->getId() == ID_COMMENT) {
                    continue;
                }
                if (tcnt++) {
                    ret += " << \",\"\n";
                }
                ret += Operation::addspaces();
                ret += "<< v." + prefix + p->getName() + "." + p2->getName();
            }
        } else {
            if (tcnt++) {
                ret += " << \",\"\n";
            }
            ret += Operation::addspaces();
            ret += "<< v." + prefix + p->getName();
        }
    }
    ret += " << \")\";\n";
    if (isVector()) {
        Operation::set_space(Operation::get_space() - 1);
        ret += Operation::addspaces() + "}\n";
    }
    ret += Operation::addspaces() + "return os;\n";
    Operation::set_space(Operation::get_space() - 1);
    ret += Operation::addspaces() + "}\n";
    ret += "\n";

    return ret;
}

std::string StructObject::generate_interface_sc_trace() {
    std::string ret = "";
    std::string ln;
    size_t tspace;

    ln = Operation::addspaces() + "inline friend void sc_trace(";
    tspace = ln.size();
    ret = ln + "sc_trace_file *tf,\n";
    ln = "";
    while (ln.size() < tspace) {
        ln += " ";
    }
    ret += ln + "const " + getType() + "&v,\n";
    ret += ln + "const std::string &NAME) {\n";
    Operation::set_space(Operation::get_space() + 1);

    std::list<GenObject *> *pentries = &getEntries();
    std::string prefix = "";
    std::string N_plus = "";
    if (isVector()) {
        pentries = &getItem()->getEntries();
        prefix = getEntries().front()->getName() + "[i].";
        N_plus = "N + ";

        ret += Operation::addspaces() + "char N[16];\n";
        ret += Operation::addspaces();
        ret += "for (int i = 0; i < " + getEntries().front()->getStrDepth() + "; i++) {\n";
        Operation::set_space(Operation::get_space() + 1);
        ret += Operation::addspaces() + "sprintf(N, \"(%d)\", i);\n";
    }
    for (auto& p : *pentries) {
        if (p->getId() == ID_COMMENT) {
            // do nothing
        } else if (p->getId() == ID_STRUCT_INST) {
            // 1 level of sub structures without recursive implementation
            for (auto &p2 : p->getEntries()) {
                if (p2->getId() == ID_COMMENT) {
                    continue;
                }
                ret += Operation::addspaces();
                ret += "sc_trace(tf, v." + prefix + p->getName() + "." + p2->getName() + ", NAME + ";
                ret += N_plus + "\"_" + p->getName() + "_" + p2->getName() + "\");\n";
            }
        } else {
            ret += Operation::addspaces();
            ret += "sc_trace(tf, v." + prefix + p->getName() + ", NAME + ";
            ret += N_plus + "\"_" + p->getName() + "\");\n";
        }
    }
    if (isVector()) {
        Operation::set_space(Operation::get_space() - 1);
        ret += Operation::addspaces() + "}\n";
    }
    Operation::set_space(Operation::get_space() - 1);
    ret += Operation::addspaces() + "}\n";
    ret += "\n";

    return ret;
}


std::string StructObject::generate_interface() {
    std::string ret = "";
    std::string ln;
    int space = Operation::get_space();
    Operation::set_space(0);

    if (getComment().size()) {
        ret += "// " + getComment() + "\n";
    }
    if (SCV_is_sysc()) {
        ret += "class " + getType() + " {\n";
        ret += " public:\n";

        Operation::set_space(Operation::get_space() + 1);
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
        Operation::set_space(Operation::get_space() - 1);
        ret += " public:\n";
    } else {
        ret += "typedef struct {\n";
    }
    Operation::set_space(Operation::get_space() + 1);
    for (auto& p : entries_) {
        ln = Operation::addspaces();
        if (p->getId() == ID_COMMENT) {
            ln += "//";
        }
        ln += p->getType() + " " + p->getName();
        if (p->getDepth()) {
            ln += "[";
            if (SCV_is_sysc()) {
                ln += p->getStrDepth();
            }
            else {
                ln += "0: " + p->getStrDepth() + " - 1";
            }
            ln += "]";
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
    Operation::set_space(Operation::get_space() - 1);
    ret += Operation::addspaces();
    if (SCV_is_sysc()) {
        ret += "};\n";
    }
    else {
        ret += "} " + getType() + ";\n";
    }
    Operation::set_space(space);
    return ret;
}

std::string StructObject::generate_const_none() {
    std::string ret = "";
    int space = Operation::get_space();
    Operation::set_space(0);

    if (SCV_is_sysc()) {
        ret += "static const " + getType() + " " + getName() + ";\n";
    } else {
        ret += "const " + getType() + " " + getName() + " = '{\n";
        Operation::set_space(Operation::get_space() + 1);
        for (auto& p : entries_) {
            if (p->getId() == ID_COMMENT) {
                continue;
            }
            ret += Operation::addspaces();
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
        Operation::set_space(Operation::get_space() - 1);
        ret += "};\n";
    }
    Operation::set_space(space);
    return ret;
}

std::string StructObject::generate() {
    std::string ret = "";
    std::string ln;

    if (getParent()->getId() == ID_FILE) {
        if (getId() == ID_STRUCT_INST) {
            ret = generate_const_none();
        } else {
            ret = generate_interface();
        }
        return ret;
    }

    if (getComment().size()) {
        ret += "    // " + getComment() + "\n";
    }
    ret += Operation::addspaces();
    if (SCV_is_sysc()) {
        ret += "struct " + getType() + " {\n";
    } else {
        ret += "typedef struct {\n";
    }
    Operation::set_space(Operation::get_space() + 1);
    for (auto &p: entries_) {
        ln = Operation::addspaces();
        if (p->getId() == ID_COMMENT) {
            ln += "//";
        }
        if (SCV_is_sysc()) {
            if (p->isVector()) {
                ln += "sc_vector<";
            }
            if (p->isSignal()) {
                ln += "sc_signal<";
            }
        }
        ln += p->getType();
        if (SCV_is_sysc()) {
            if (p->isSignal()) {
                ln += ">";
            }
            if (p->isVector()) {
                ln += ">";
            }
        }
        ln += " " + p->getName();
        if (p->getDepth()) {
            ln += "[";
            if (SCV_is_sysc()) {
                ln += p->getStrDepth();
            } else {
                ln += "0: " + p->getStrDepth() + " - 1";
            }
            ln += "]";
        }
        if (p->getId() != ID_COMMENT) {
            ln += ";";
        }
        if (p->getComment().size()) {
            while (ln.size() < 60) {
                ln += " ";
            }
            ln += "// " + p->getComment();
        }
        ret += ln + "\n";
    }
    Operation::set_space(Operation::get_space() - 1);
    ret += Operation::addspaces();
    if (SCV_is_sysc()) {
        ret += "};\n";
    } else  {
        ret += "} " + getType() +";\n";
    }
    ret += "\n";
    return ret;
}

}
