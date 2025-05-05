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

#include "api_rtlgen.h"
#include "structs.h"
#include "files.h"
#include "utils.h"
#include "array.h"
#include "operations.h"
#include "proc.h"
#include <cstring>

namespace sysvc {

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

    // TODO: change implementation of vector
    //       
    std::string checktype = getType();
    if (isVector()) {
        checktype = getTypedef();
    }
    if (getObjDepth()) {
        int d = static_cast<int>(getDepth());
        pushspaces();
        int tcnt = 0;
        std::string ln;
        for (auto &p: getEntries()) {
            if (isVector() && p->getType() != checktype) {
                // FIXME: vector implemented very badly. Only the same type entry should be generated
                continue;
            }
            ln = addspaces();
            if (p->getObjDepth()) { 
                
                ret += ln + "{\n";
                pushspaces();
                for (int i = 0; i < p->getDepth(); i++) {
                    ret += addspaces() + p->getStrValue();
                    if (i < p->getDepth() - 1) {
                        ret += ",";
                    }
                    ret += "\n";
                }

                popspaces();
                ln = addspaces() + "}";
            } else {
                ln += p->getStrValue();
            }
            if (p != getEntries().back()) {
                ln += ",";
            }
            p->addComment(ln);
            ret += ln + "\n";
        }
        popspaces();
    } else {
        std::string ln;
        for (auto &p: getEntries()) {
            if (p->isComment()) {
                continue;
            }
            ln = addspaces();
            if (p->getObjDepth()) { 
                
                ret += ln + "{\n";
                pushspaces();
                for (int i = 0; i < p->getDepth(); i++) {
                    ret += addspaces() + p->getStrValue();
                    if (i < p->getDepth() - 1) {
                        ret += ",";
                    }
                    ret += "\n";
                }

                popspaces();
                ln = addspaces() + "}";
            } else {
                ln += p->getStrValue();
            }
            if (p != getEntries().back()) {
                ln += ", ";
            }
            // Add variable name as a comment:
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
        ret += generate_interface_sc_constructor();
        ret += generate_interface_sc_constructor_init();
        ret += generate_interface_sc_op_equal();
        ret += generate_interface_sc_op_assign();
        ret += generate_interface_sc_trace();
        ret += generate_interface_sc_op_stream();
        if (isVector()) {
            ret += generate_interface_sc_op_bracket();
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

    if (isParam() || isConst()) {
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
                                   const char *suffix,
                                   const char *type,
                                   const char *rstval)
    : StructObject(parent, clk, edge, rst, active,
                   type, type, rstval, NO_COMMENT), rst_(0), v_(0), rin_(0), r_(0) {

    std::string r_name = "r" + std::string(suffix);
    std::string procname = r_name + "egisters";

    // "*_r_reset" value:
    if (active != ACTIVE_NONE) {
        // module is owner:
        rst_ = new RegResetStruct(parent,
                                  this,
                                  rstval);
        func_rst_ = new RegResetFunction(parent,
                                         this,
                                         rstval);
    }

    // "r" value (have input 'v' and output 'r' ports):
    r_ = new RegSignalInstance(parent,
                               this,
                               r_name.c_str(),
                               rstval);

    
    // "v" value:
    std::string v_name = "v" + std::string(suffix);
    v_ = new RegVariableInstance(NO_PARENT,
                                 this,
                                 v_name.c_str(),
                                 r_name.c_str());

    // "rin" value:
    std::string rin_name = r_name + "in";
    rin_ = new RegSignalInstance(parent,
                                this,
                                rin_name.c_str(),
                                rstval);

    // Register flip-flop process
    new RegisterCopyProcess(parent,
                            procname.c_str(),
                            this);
}

void RegTypedefStruct::configureGenerator(ECfgGenType cfg) {
    if (getResetActive() == ACTIVE_NONE) {
        return;
    }
    getParent()->getEntries().remove(rst_);
    getParent()->getEntries().remove(func_rst_);

    auto lt = getParent()->getEntries().begin();
    for (auto &p: getParent()->getEntries()) {
        lt++;
        if (p == this) {
            if (cfg == CFG_GEN_SYSC) {
                getParent()->getEntries().insert(lt, func_rst_);
            } else {
                getParent()->getEntries().insert(lt, rst_);
            }
            break;
        }
    }
    StructObject::configureGenerator(cfg);
}

void RegTypedefStruct::add_entry(GenObject *obj) {
    StructObject::add_entry(obj);
    if (rst_) {
        add_refreset_entry(rst_, obj);
        add_refreset_entry(func_rst_->getIV(), obj);
    }
    add_ref_entry(v_, obj);
    add_ref_entry(rin_, obj);
    add_ref_entry(r_, obj);
}

void RegTypedefStruct::add_ref_entry(GenObject *parent, GenObject *obj) {
    RefObject *ref = new RefObject(parent, obj, NO_COMMENT);
    for (auto &p: obj->getEntries()) {
        add_ref_entry(ref, p);
    }
}

void RegTypedefStruct::add_refreset_entry(GenObject *parent, GenObject *obj) {
    RefObject *ref = new RefResetObject(parent, obj, NO_COMMENT);
    for (auto &p: obj->getEntries()) {
        add_refreset_entry(ref, p);
    }
}


std::string RegTypedefStruct::reg_suffix(GenObject *p, int unique_idx) {
    std::string ret = "";
    if (unique_idx > 1) {
        char tstr[16];
        RISCV_sprintf(tstr, sizeof(tstr), "%d", unique_idx);
        ret += std::string(tstr);
    }
    if (p->getClockEdge() == CLK_NEGEDGE) {
        ret += "n";
    }
    if (p->getResetActive() == ACTIVE_NONE) {
        ret += "x";
    }
    if (p->getResetActive() == ACTIVE_HIGH) {
        ret += "h";
    }
    return ret;
}

// v
RegTypedefStruct::RegVariableInstance::RegVariableInstance(GenObject *parent,
                                 RegTypedefStruct *p,
                                 const char *name,
                                 const char *rstval)
    : StructObject(parent, 0, CLK_POSEDGE, 0, ACTIVE_NONE,
                   p->getType().c_str(), name, rstval, NO_COMMENT),
                   rstruct_(p) {
}

// r, rin
RegTypedefStruct::RegSignalInstance::RegSignalInstance(GenObject *parent,
                                 RegTypedefStruct *p,
                                 const char *name,
                                 const char *rstval)
    : RegVariableInstance(parent, p, name, rstval) {}

// r_reset
RegTypedefStruct::RegResetStruct::RegResetStruct(GenObject *parent,
                                RegTypedefStruct *p,
                                const char *name)
    : RegVariableInstance(parent, p, name, "") {
}

RegTypedefStruct::RegResetFunction::RegResetFunction(GenObject *parent,
                                RegTypedefStruct *p,
                                const char *name)
    : FunctionObject(parent, name), rstruct_(p), iv_(NO_PARENT, p, "iv") {
}

void RegTypedefStruct::RegResetFunction::postInit() {
    std::list<GenObject *>::iterator it1, it2;
    GenObject &rst = *rstruct_->rst_instance();
    Operation::push_obj(this);
    for (it1 = iv_.getEntries().begin(), it2 = rst.getEntries().begin();
        it1 != iv_.getEntries().end() && it2 != rst.getEntries().end();
        ++it1, ++it2) {
        if ((*it1)->isComment()) {
            continue;
        }
        SETVAL(*(*it1), *(*it2));
    }
    Operation::pop_obj();
}


std::string RegTypedefStruct::RegResetStruct::generate() {
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
            ret += " = " + RegVariableInstance::getStrValue();
        }
        ret += ";\n";
    } else if (SCV_is_sv()) {
        ret += "const " + getType() + " " + getName();
        if (!isVector() && getObjDepth()) {
            ret += "[" + getStrDepth() + "]";
        }
        ret += " = ";
        ret += RegVariableInstance::getStrValue() + ";\n";
    } else if (SCV_is_vhdl()) {
        ret += "constant " + getName() + "of " + getType() + " := ";
        ret += RegVariableInstance::getStrValue() + ";\n";
    }
    return ret;
}

}
