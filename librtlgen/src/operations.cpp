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

#include "operations.h"
#include "array.h"
#include "utils.h"
#include "modules.h"
#include "comments.h"
#include <cstring>

namespace sysvc {

int stackcnt_ = 0;
GenObject *stackobj_[256] = {0};

Operation::Operation(const char *comment)
    : GenObject(top_obj(), comment), igen_(0), argcnt_(0) {
}

Operation::Operation(GenObject *parent, const char *comment)
    : GenObject(parent, comment), igen_(0), argcnt_(0) {
}

void Operation::start(GenObject *owner) {
    stackcnt_ = 0;
    stackobj_[stackcnt_] = owner;
    SCV_set_local_module(owner);
}

void Operation::push_obj(GenObject *obj) {
    stackobj_[++stackcnt_] = obj;
}

void Operation::pop_obj() {
    stackobj_[stackcnt_--] = 0;
}

GenObject *Operation::top_obj() {
    return stackobj_[stackcnt_];
}

std::string Operation::addtext(GenObject *obj, size_t curpos) {
    std::string ret = "";
    if (obj->getComment().size()) {
        while (curpos++ < 60) {
            ret += " ";
        }
        ret += obj->addComment();
    }
    return ret;
}

std::string Operation::v_name(std::string v) {
    std::string ret = generate();
    if (v.size()) {
        ret += "." + v;
    }
    return ret;
}


std::string Operation::fullname(const char *prefix, std::string name, GenObject *obj) {
    if (!obj) {
        return name;
    }
    std::string read = "";
    GenObject *p = obj->getParent();
    std::string curname = "";
    if (obj->isConst()) {
        curname = obj->getStrValue();
    } else if (obj->isOperation()) {
        curname = obj->generate();
    } else if (obj->getSelector()) {
        curname = "";
        curname = fullname("", curname, obj->getSelector());
        if (SCV_is_vhdl()) {
            curname = "(" + curname + ")";
        } else {
            curname = "[" + curname + "]";
        }
        if ((obj->isInput() && !obj->isVector()) && SCV_is_sysc()) {
            curname = obj->getName() + ".read()" + curname;
        } else {
            curname = obj->getName() + curname;
        }
        if (name.size()) {
            curname += ".";
        }
        curname += name;
        obj->setSelector(0);
    } else if (obj->isStruct()) {
        curname = obj->getName();
        if (name.size()) {
            curname += ".";
        }
        curname += name;
    } else if (SCV_is_sysc() && obj->isParamGeneric() && !obj->isParamTemplate()) {
        curname = obj->getName() + "_";
    } else if (SCV_is_sv_pkg() && obj->isParam() && !obj->isParamGeneric()) {
        GenObject *pfile = obj->getParent();
        while (pfile && !pfile->isFile()) {
            pfile = pfile->getParent();
        }
        if (pfile) {
            curname += pfile->getName() + "_pkg::";
        }
        curname += obj->getName();
    } else {
        curname = obj->getName();
    }

    if (p && p->isInput()) {
        if (SCV_is_sysc()) {
            read += "read().";
        }
    }

    if (p && p->isStruct()) {
        curname = fullname(prefix, read + curname, obj->getParent());
    } else if (obj->getClockEdge() != CLK_ALWAYS) {
        if (prefix[0] == 'r') {
            curname = obj->r_prefix() + "." + read + curname;
        } else if (prefix[0] == 'v') {
            curname = obj->v_prefix() + "." + read + curname;
        }
    }
    return curname;
}

std::string Operation::obj2varname(GenObject *obj, const char *prefix, bool read) {
    std::string ret = "";
    if (!obj) {
        return ret;
    }
    ret = fullname(prefix, ret, obj);

    if (read) {
        if (obj->isInput()
            || obj->isSignal()) {
            if (SCV_is_sysc()) {
                ret += ".read()";
            }
        }
    }

    // IT IS JUST A BEGINNING OF THE fullname() FUNCTION REMOVING. IN PROGRESS!!!
    std::string tr = obj->r_name("");
    std::string tv = obj->v_name("");
    return ret;
}


/** Standard class for standard 2-operands functions:
        (A + B), (A - B), (A & B), (A / B) etc
 */
uint64_t TwoStandardOperandsOperation::getWidth() {
    return a_->getWidth() >= b_->getWidth() ? a_->getWidth(): b_->getWidth();
}

std::string TwoStandardOperandsOperation::getStrValue() {
    std::string A = obj2varname(a_, "r", true);
    std::string B = obj2varname(b_, "r", true);
    A = "(" + A + getOperand() + B + ")";
    return A;
}

/** Operation with multiple arguments
 */
std::string NStandardOperandsOperation::getStrValue() {
    std::string ret = "";
    ret += getOpeningBrace();
    if (!oneline_) {
        pushspaces();
        pushspaces();
    }

    for (auto &p: getEntries()) {
        ret += obj2varname(p, "r", true);
        if (p != getEntries().back()) {
            if (!oneline_ && isOperandNewLine()) {
                ret += "\n" + addspaces();
            }

            ret += getOperand();

            if (!oneline_ && !isOperandNewLine()) {
                ret += "\n" + addspaces();
            } else {
                ret += " ";
            }
        }
    }

    if (!oneline_) {
        popspaces();
        popspaces();
    }
    ret += getClosingBrace();
    return ret;
}

GetValueOperation::GetValueOperation(GenObject *a,         // value to get
                                     GenObject *idx,       // array index (depth should be non-zero)
                                     GenObject *item,      // struct item
                                     bool h_as_width,      // interpret h as width argument for sv [start +: width] operation
                                     GenObject *h,         // MSB bit
                                     GenObject *l,         // LSB bit
                                     const char *comment)
    : Operation(NO_PARENT, comment), a_(a), idx_(idx), item_(item),
    h_(h), l_(l), h_as_width_(h_as_width) {
}

std::string GetValueOperation::getStrValue() {
    std::string ret = obj2varname(a_, getRegPrefix().c_str(), isForceRead());
    if (a_->getDepth() && idx_) {
        if (SCV_is_vhdl()) {
            ret += "(" + obj2varname(idx_, "r", true) + ")";
        } else {
            ret += "[" + obj2varname(idx_, "r", true) + "]";
        }
    } else if (idx_) {
        SHOW_ERROR("Array[%s] without depth", idx_->getStrValue().c_str());
    }

    if (item_ && item_ != a_) {
        ret += "." + item_->getName();
    }

    if (h_ && (a_->isLogic()
              || (item_ && item_->isLogic()))) {
        // Bits selection:
        std::string h_name = obj2varname(h_, "r", true);
        std::string l_name = obj2varname(l_, "r", true);
        if (SCV_is_sysc()) {
            if (h_as_width_) {
                ret += "(" + l_name + " + " + h_name + " - 1, " + l_name + ")";
            } else if (l_) {
                ret += "(" + h_name + ", " + l_name + ")";
            } else {
                ret += "[" + h_name + "]";
            }
        } else if (SCV_is_sv()) {
            ret += "[";
            if (h_as_width_) {
                ret += l_name + " +: " + h_name;
            } else {
                ret += h_name;
                if (l_) {
                    ret += ": " + l_name;
                }
            }
            ret += "]";
        } else if (SCV_is_vhdl()) {
            ret += "(";
            if (h_as_width_) {
                ret += l_name + " + " + h_name + " - 1 downto " + l_name;
            } else if (l_) {
                ret += h_name + " downto " + l_name;
            } else {
                ret += h_name;
            }
            ret += ")";
        }
    }
    return ret;
}


/**
    Set value into variable:
        a = b
        a[h:l] = b
        a[arridx][h:l] = b
 */
SetValueOperation::SetValueOperation(GenObject *a,         // value to set
                                     GenObject *idx,       // array index (depth should be non-zero)
                                     GenObject *item,      // struct item
                                     bool h_as_width,      // interpret h as width argument for sv [start +: width] operation
                                     GenObject *h,         // MSB bit
                                     GenObject *l,         // LSB bit
                                     GenObject *val,       // value to get
                                     const char *comment)
    : GetValueOperation(a, idx, item, h_as_width, h, l, comment), v_(val) {
    top_obj()->add_entry(this);
}


std::string SetValueOperation::generate() {
    std::string ret = addspaces();
    if (SCV_is_sysc() && a_->isClock()) {
        ret += "// ";   // do not clear clock module
    }
    if (isAssign() && SCV_is_sv()) {
        ret += "assign ";
    }

    ret += GetValueOperation::getStrValue();

    if (SCV_is_vhdl()) {
        if (isAssign()) {
            ret += " <= ";
        } else {
            ret += " := ";
        }
    } else {
        ret += " = ";
    }
    ret += obj2varname(v_, "r", false) + ";";
    ret += addtext(this, ret.size());
    ret += "\n";
    return ret;
}

std::string IncrementValueOperation::generate() {
    std::string ret = addspaces();
    std::string t1 = GetValueOperation::getStrValue();

    ret += t1;
    if (SCV_is_vhdl()) {
        ret += " := " + t1 + " + ";
    } else {
        ret += " += ";
    }
    ret += obj2varname(v_, "r", false) + ";";
    ret += addtext(this, ret.size());
    ret += "\n";
    return ret;
}


/**
    Generate commenting string:
        // text
 */
std::string TextOperation::generate() {
    std::string ret = "";
    if (getComment().size() == 0) {
        // Do nothing
    } else {
        ret += addspaces();
        if (SCV_is_vhdl()) {
            ret += "-- ";
        } else {
            ret += "// ";
        }
        ret += getComment();
    }
    ret += "\n";
    return ret;
}

/**
    Assign constant value to all bits. No parent.
 */
std::string AllConstOperation::getStrValue() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        if (v_) {
            ret += "~0ull";
        } else {
            ret += "0";
        }
    } else if (SCV_is_sv()) {
        if (v_) {
            ret += "'1";
        } else {
            ret += "'0";
        }
    } else {
        if (v_) {
            ret += "(others => '1')";
        } else {
            ret += "(others => '0')";
        }
    }
    return ret;
}

std::string BitOperation::generate() {
    std::string ret = "";
    ret += Operation::obj2varname(a_, "r", true);
    if (SCV_is_sysc()) {
         ret += "[";
         ret += Operation::obj2varname(idx_, "r", true);
         ret += "]";
    } else if (SCV_is_sv()) {
         ret += "[";
         ret += Operation::obj2varname(idx_);
         ret += "]";
    } else {
         ret += "(";
         ret += Operation::obj2varname(idx_);
         ret += ")";
    }
    return ret;
}

// BITS
std::string BitsOperation::generate() {
    std::string ret = "";
    bool use_shift = false;
    std::string t1 = a_->getType();
    if (SCV_is_sysc() && 
        (t1 == "uint64_t" || t1 == "int" || t1 == "uint32_t" || t1 == "uint16_t")) {
        use_shift = true;
        ret += "(";
    }
    ret += Operation::obj2varname(a_, "r", true);
    if (use_shift) {
        ret += " >> " + Operation::obj2varname(l_, "r", true);
        ret += ")";
    } else if (SCV_is_sysc()) {
        ret += "(";
        ret += Operation::obj2varname(h_, "r", true);
        ret += ", ";
        ret += Operation::obj2varname(l_, "r", true);
        ret += ")";
    } else if (SCV_is_sv()) {
        ret += "[";
        ret += Operation::obj2varname(h_, "r", true);
        ret += ": ";
        ret += Operation::obj2varname(l_, "r", true);
        ret += "]";
    } else {
        ret += "(";
        ret += Operation::obj2varname(h_, "r", true);
        ret += " downto ";
        ret += Operation::obj2varname(l_, "r", true);
        ret += ")";
    }
    return ret;
}

// BITSW
std::string BitswOperation::generate() {
    std::string ret = "";
    ret += Operation::obj2varname(a_, "r", true);
    if (SCV_is_sysc()) {
         ret += "(";
         ret += Operation::obj2varname(start_, "r", true);
         ret += " + ";
         ret += Operation::obj2varname(width_, "r", true);
         ret += " - 1, ";
         ret += Operation::obj2varname(start_, "r", true);
         ret += ")";
    } else if (SCV_is_sv()) {
         ret += "[";
         ret += Operation::obj2varname(start_, "r", true);
         ret += " +: ";
         ret += Operation::obj2varname(width_, "r", true);
         ret += "]";
    } else {
         ret += "(";
         ret += Operation::obj2varname(start_, "r", true);
         ret += " + ";
         ret += Operation::obj2varname(width_, "r", true);
         ret += " - 1 downto ";
         ret += Operation::obj2varname(start_, "r", true);
         ret += ")";
    }
    return ret;
}

// TO_INT
std::string ToIntOperation::generate() {
    std::string A = obj2varname(a_, "r", true);
    if (SCV_is_sysc()) {
        if (!a_->isParam()) {      // params aren't use sc_uint<> templates
            A = A + ".to_int()";
        }
    } else if (SCV_is_sv()) {
        A = "int'(" + A + ")";
    } else if (SCV_is_vhdl()) {
        if (a_->isLogic()) {
            A = "conv_std_logic_to_int(" + A + ")";
        }
    }
    return A;
}

// TO_U32
std::string ToU32Operation::generate() {
    std::string A = obj2varname(a_, "r", true);
    if (SCV_is_sysc()) {
        A = A + ".to_uint()";
    }
    return A;
}

// TO_U64
std::string ToU64Operation::generate() {
    std::string A = obj2varname(a_, "r", true);
    if (SCV_is_sysc()) {
        A = A + ".to_uint64()";
    }
    return A;
}

// TO_CSTR
std::string ToCStrOperation::generate() {
    std::string A = obj2varname(a_, "r", true);
    if (SCV_is_sysc()) {
        A = A + ".c_str()";
    }
    return A;
}

// BIG_TO_U64: explicit conersion of biguint to uint64 (sysc only)
std::string BigToU64Operation::generate() {
    std::string A = "";
    if (SCV_is_sysc()) {
        A = obj2varname(a_, "r", true) + ".to_uint64()";
    } else {
        A = obj2varname(a_, "r", true);
    }
    return A;
}

// TO_BIG
std::string ToBigOperation::generate() {
    std::string A = "";
    if (SCV_is_sysc()) {
        A = "sc_biguint<" + objWidth_.getStrValue() + ">(";
        A += obj2varname(a_, "r", true) + ")";
    } else {
        A += obj2varname(a_, "r", true);
    }
    return A;
}


// INV (arithemtic, logical)
std::string InvOperation::generate() {
    std::string A = obj2varname(a_, "r", true);
    if (SCV_is_sysc()) {
        if (logical_) {
            A = "(~" + A + ")";
        } else {
            A = "(!" + A + ")";
        }
    } else if (SCV_is_sv()) {
        A = "(~" + A + ")";
    } else if (SCV_is_vhdl()) {
        A = "(not " + A + ")";
    }

    return A;
}


// SETVAL
std::string SETVAL_gen(GenObject **args) {
    std::string ret = addspaces();
    ret += Operation::obj2varname(args[1], "v") + " = ";
    if (SCV_is_sv() && args[2]->isString()) {
        ret += "{";
    }
    if (args[2]->isConst()) {
        ret += args[2]->getStrValue();
    } else if (args[2]->isValue()
            || args[2]->isClock()
            || args[2]->isParam()
            || args[2]->isStruct()) {
        ret += Operation::obj2varname(args[2]);
    } else {
        ret += args[2]->generate();
    }
    if (SCV_is_sv() && args[2]->isString()) {
        ret += "}";
    }
    ret += ";";
    ret += Operation::addtext(args[0], ret.size());
    ret += "\n";
    return ret;
}

// SETZ
std::string SETZ_gen(GenObject **args) {
    std::string ret = addspaces();

    if (SCV_is_sysc()) {
        // No systemc assigning Z    
        ret += "// ";
    }
    ret += Operation::obj2varname(args[1], "v");
    ret += " = 1'bz;";
    ret += Operation::addtext(args[0], ret.size());
    ret += "\n";
    return ret;
}

Operation &SETZ(GenObject &a, const char *comment) {
    Operation *p = new Operation(comment);
    p->igen_ = SETZ_gen;
    p->add_arg(p);
    p->add_arg(&a);
    return *p;
}


// SETSTR
Operation &SETSTR(GenObject &a, const char *str, const char *comment) {
    Operation *p = new Operation(comment);
    p->igen_ = SETVAL_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(new StringConst(str));
    return *p;
}

// SETSTRF
std::string SETSTRF_gen(GenObject **args) {
    std::string ret = addspaces();
    std::string fmt;
    if (SCV_is_sysc()) {
        ret += "RISCV_sprintf(tstr, sizeof(tstr), ";
    } else {
        ret += Operation::obj2varname(args[1]);
        if (args[4]) {
            ret += " += ";
        } else {
            ret += " = ";
        }
        ret += "$sformatf(";
    }
    fmt = args[2]->getStrValue();
    if (SCV_is_sv()) {
        // Remove RV_PRI64 occurences:
        const char *pos;
        while (pos = strstr(fmt.c_str(), "\" RV_PRI64 \"")) {
            fmt.erase(pos - fmt.c_str(), 12);
        }
    }
    ret += fmt;
    size_t cnt = args[3]->getValue();
    pushspaces();
    pushspaces();
    if (cnt > 1) {
        ret += ",\n" + addspaces();
    } else {
        ret += ", ";
    }
    for (size_t i = 0; i < cnt; i++) {
        if (i > 0) {
            ret += ",\n" + addspaces();
        }
        ret += Operation::obj2varname(args[5 + i]);
        if (SCV_is_sysc()
            && args[5+i]->isParamGeneric()
            && strstr(args[5+i]->getType().c_str(), "string")) {
            ret += ".c_str()";
        }
    }
    popspaces();
    popspaces();
    ret += ");\n";
    if (SCV_is_sysc()) {
        ret += addspaces() + Operation::obj2varname(args[1]) + " ";
        if (args[4]) {
            ret += "+";
        }
        ret += "= std::string(tstr);\n";
    }
    return ret;
}

Operation &SETSTRF(GenObject &a, const char *fmt, size_t cnt, ...) {
    Operation *p = new Operation("");
    GenObject *obj;
    p->igen_ = SETSTRF_gen;
    p->add_arg(p);  // 0
    p->add_arg(&a); // 1
    p->add_arg(new StringConst(fmt)); // 2
    p->add_arg(new DecConst(cnt)); // 3
    p->add_arg(0); // 4
    va_list arg;
    va_start(arg, cnt);
    for (int i = 0; i < cnt; i++) {
        obj = va_arg(arg, GenObject *);
        p->add_arg(obj);    // 5+
    }
    va_end(arg);
    return *p;
}

Operation &ADDSTRF(GenObject &a, const char *fmt, size_t cnt, ...) {
    Operation *p = new Operation("");
    GenObject *obj;
    p->igen_ = SETSTRF_gen;
    p->add_arg(p);  // 0
    p->add_arg(&a); // 1
    p->add_arg(new StringConst(fmt));    // 2
    p->add_arg(new DecConst(cnt)); // 3
    p->add_arg(new DecConst(1));   // 4
    va_list arg;
    va_start(arg, cnt);
    for (int i = 0; i < cnt; i++) {
        obj = va_arg(arg, GenObject *);
        p->add_arg(obj);    // 5+
    }
    va_end(arg);
    return *p;
}

// ADDSTRU8
std::string ADDSTRU8_gen(GenObject **args) {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret += addspaces() + "int tsz = RISCV_sprintf(tstr, sizeof(tstr), \"%s\", ";
        ret += Operation::obj2varname(args[2]) + ".c_str());\n";
        ret += addspaces() + "tstr[tsz++] = static_cast<char>(";
        ret += Operation::obj2varname(args[3], "r", true) + ".to_uint());\n";
        ret += addspaces() + "tstr[tsz] = 0;\n";
        ret += addspaces() + Operation::obj2varname(args[1]);
        ret += " = tstr;\n";
    } else {
        ret += addspaces();
        ret += Operation::obj2varname(args[1]) + " = {";
        ret += Operation::obj2varname(args[2]) + ", ";
        ret += Operation::obj2varname(args[3]);
        ret += "};\n";
    }
    return ret;
}

Operation &ADDSTRU8(GenObject &strout, GenObject &strin, GenObject &val) {
    Operation *p = new Operation("");
    p->igen_ = ADDSTRU8_gen;
    p->add_arg(p);  // 0
    p->add_arg(&strout); // 1
    p->add_arg(&strin); // 2
    p->add_arg(&val); // 3
    return *p;
}

// CALCWIDTHx
uint64_t CalcWidthOperation::getValue() {
    uint64_t ret = 0;
    for (auto &p: getEntries()) {
        ret += p->getWidth();
    }
    return ret;
}

std::string CalcWidthOperation::getStrValue() {
    std::string ret = "(";
    pushspaces();
    pushspaces();
    for (auto &p: getEntries()) {
        ret += p->getStrWidth() + "  // " + p->getName();
        if (p != getEntries().back()) {
            ret += "\n";
            ret += addspaces();
            ret += "+ ";
        }
    }
    popspaces();
    popspaces();
    ret += "\n" + addspaces() + ")";
    return ret;
}

// AND_REDUCE
std::string AndReduceOperation::getStrValue() {
    std::string A = obj2varname(a_, "r", true);
    if (SCV_is_sysc()) {
        A += ".and_reduce()";
    } else if (SCV_is_sv()) {
        A = "(&" + A + ")";
    } else if (SCV_is_vhdl()) {
        A = "and_reduce(" + A + ")";
    }
    return A;
}

// OR_REDUCE
std::string OrReduceOperation::getStrValue() {
    std::string A = obj2varname(a_, "r", true);
    if (SCV_is_sysc()) {
        A += ".or_reduce()";
    } else if (SCV_is_sv()) {
        A = "(|" + A + ")";
    } else if (SCV_is_vhdl()) {
        A = "or_reduce(" + A + ")";
    }
    return A;
}


// AND2/AND2_L
std::string And2Operation::getOperand() {
    if (SCV_is_vhdl()) {
        return std::string(" and ");
    }
    if (logical_) {
        return std::string(" & ");
    }
    return std::string(" && ");
}

// OR2/OR2_L
std::string Or2Operation::getOperand() {
    if (SCV_is_vhdl()) {
        return std::string(" or ");
    }
    if (logical_) {
        return std::string(" | ");
    }
    return std::string(" || ");
}

// XOR2
std::string Xor2Operation::getOperand() {
    if (SCV_is_vhdl()) {
        return std::string(" xor ");
    }
    return std::string(" ^ ");
}

// AND3
std::string And3Operation::getOperand() {
    if (SCV_is_vhdl()) {
        return std::string(" and");
    }
    if (logical_) {
        return std::string(" &");
    }
    return std::string(" &&");
}

// OR3
std::string Or3Operation::getOperand() {
    if (SCV_is_vhdl()) {
        return std::string(" or");
    }
    if (logical_) {
        return std::string(" |");
    }
    return std::string(" ||");
}

// AND4
std::string And4Operation::getOperand() {
    if (SCV_is_vhdl()) {
        return std::string(" and");
    }
    return std::string(" &&");
}

// OR4
std::string Or4Operation::getOperand() {
    if (SCV_is_vhdl()) {
        return std::string(" or");
    }
    return std::string(" ||");
}

// ANDx/ANDx_L
std::string ANDxOperation::getOperand() {
    if (SCV_is_vhdl()) {
        return std::string("and");
    }
    if (logical_) {
        return std::string("&");
    }
    return std::string("&&");
}

// ORx/ORx_L
std::string ORxOperation::getOperand() {
    if (SCV_is_vhdl()) {
        return std::string("or");
    }
    if (logical_) {
        return std::string("|");
    }
    return std::string("||");
}

// XORx
std::string XORxOperation::getOperand() {
    if (SCV_is_vhdl()) {
        return std::string("xor");
    }
    return std::string("^");
}

// EQ
std::string EqOperation::getOperand() {
    if (SCV_is_vhdl()) {
        return std::string(" = ");
    }
    return std::string(" == ");
}

// NE
std::string NeOperation::getOperand() {
    if (SCV_is_vhdl()) {
        return std::string(" /= ");
    }
    return std::string(" != ");
}

// EZ
std::string EzOperation::getStrValue() {
    std::string A = obj2varname(a_, "r", true);
    if (a_->getWidth() > 1) {
        if (SCV_is_sysc()) {
            A += ".or_reduce()";
        } else if (SCV_is_sv()) {
            A = "(|" + A + ")";
        }
    }
    if (SCV_is_sysc()) {
        A = "(" + A + " == 0)";
    } else {
        A = "(" + A + " == 1'b0)";
    }
    return A;
}

// NZ
std::string NzOperation::getStrValue() {
    std::string A = obj2varname(a_, "r", true);
    if (a_->getWidth() > 1) {
        if (SCV_is_sysc()) {
            A += ".or_reduce()";
        } else if (SCV_is_sv()) {
            A = "(|" + A + ")";
        }
    }
    if (SCV_is_sysc()) {
        A = "(" + A + " == 1)";
    } else {
        A = "(" + A + " == 1'b1)";
    }
    return A;
}



// SPLx
std::string SplitOperation::generate() {
    std::string ret = "";
    char tstr[64];
    int w = static_cast<int>(a_->getWidth());
    std::string busname = Operation::obj2varname(a_, "r", true);

    for (auto &p: getEntries()) {
        ret += addspaces();
        ret += obj2varname(p);
        ret += " = ";
        ret += busname;
        if (p->getWidth() > 1) {
            if (SCV_is_sysc()) {
                RISCV_sprintf(tstr, sizeof(tstr), "(%d, %d)",
                                w - 1, w - static_cast<int>(p->getWidth()));
            } else if (SCV_is_sv()) {
                RISCV_sprintf(tstr, sizeof(tstr), "[%d: %d]",
                                w - 1, w - static_cast<int>(p->getWidth()));
            } else {
                RISCV_sprintf(tstr, sizeof(tstr), "(%d downto %d)",
                                w - 1, w - static_cast<int>(p->getWidth()));
            }
            ret += std::string(tstr);
        } else {
            RISCV_sprintf(tstr, sizeof(tstr), "[%d]", w - 1);
            ret += std::string(tstr);
        }
        ret += ";\n";
        w -= static_cast<int>(p->getWidth());
    }
    return ret;
}

// CCx
std::string CCxOperation::getOpeningBrace() {
    if (SCV_is_sv()) {
        return std::string("{");
    }
    return NStandardOperandsOperation::getOpeningBrace();
}
std::string CCxOperation::getClosingBrace() {
    if (SCV_is_sv()) {
        return std::string("}");
    }
    return NStandardOperandsOperation::getClosingBrace();
}

std::string CCxOperation::getOperand() {
    if (SCV_is_vhdl()) {
        return std::string(" &");
    }
    return std::string(",");
}

uint64_t CCxOperation::getWidth() {
    uint64_t ret = 0;
    for (auto &p: getEntries()) {
        ret += p->getWidth();
    }
    return ret;
}

std::string CCxOperation::getStrValue() {
    std::string ret = "";
    std::list<GenObject *>::iterator it = getEntries().begin();
    GenObject *A = *it;
    it++;
    GenObject *B = *it;
    it++;
    if (SCV_is_sysc() && getEntries().size() == 2 && B->isConst()) {
        ret = getOpeningBrace();
        if (B->getValue() != 0) {
            ret += "(";
        }
        ret += obj2varname(A, "r", true) + " << " + B->getStrWidth();
        if (B->getValue() != 0) {
            ret += ") | " + B->getStrValue();
        }
        ret += getClosingBrace();
    } else if (SCV_is_sysc() && getEntries().size() == 3
        && A->isConst() && A->getValue() == 0 && (*it)->isConst()) {
        GenObject *C = (*it);
        ret = getOpeningBrace();
        if (C->getValue() != 0) {
            ret += "(";
        }
        ret += obj2varname(B, "r", true) + " << " + C->getStrWidth();
        if (C->getValue() != 0) {
            ret += ") | " + C->getStrValue();
        } else {
            ret += ")";
        }
        getClosingBrace();
    } else {
        return NStandardOperandsOperation::getStrValue();
    }

    return ret;
}


// LSH: left shift
std::string LshOperation::getStrValue() {
    std::string A = obj2varname(a_, "r", true);
    std::string B = obj2varname(sz_);
    if (SCV_is_sysc()) {
        A = "(" + A + " << " + B + ")";
    } else if (SCV_is_sv()) {
        if (sz_->isParam() || sz_->isConst()) {
            A = "{" + A + ", {" + B + "{1'b0}}}";
        } else {
            A = "(" + A + " << " + B + ")";
        }
    } else if (SCV_is_vhdl()) {
        A = "(" + A + " sll " + B + ")";
    }
    return A;
}

// POW2: 1 << sz
std::string Pow2Operation::getStrValue() {
    std::string A = "";
    std::string sz = obj2varname(sz_);
    if (SCV_is_sysc()) {
        A = "(1 << " + sz + ")";
    } else if (SCV_is_sv()) {
        A = "(2**" + sz + ")";
    } else if (SCV_is_vhdl()) {
        A = "(2**" + sz + ")";
    }
    return A;
}

// RSH: right shift
std::string RshOperation::generate() {
    std::string A = obj2varname(a_, "r", true);
    std::string B = obj2varname(sz_);
    if (SCV_is_sysc()) {
        A = "(" + A + " >> " + B + ")";
    } else if (SCV_is_sv()) {
        if (sz_->isConst()) {
            A = "{'0, " + A + "[" + a_->getStrWidth() + " - 1: " + B + "]}";
        } else {
            A = "(" + A + " >> " + B + ")";
        }
    } else if (SCV_is_vhdl()) {
        A = "(" + A + " srl " + B + ")";
    }
    return A;
}


// ARRITEM
std::string ARRITEM_gen(GenObject **args) {
    std::string ret = "";
    args[1]->setSelector(args[2]);
    if (args[4]) {
        ret = Operation::obj2varname(args[3], "r", true);
    } else {
        ret = Operation::obj2varname(args[3]);
    }
    return ret;
}

Operation &ARRITEM(GenObject &arr, GenObject &idx, GenObject &item, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = ARRITEM_gen;
    p->add_arg(p);      // 0
    p->add_arg(&arr);   // 1
    p->add_arg(&idx);   // 2
    p->add_arg(&item);  // 3
    p->add_arg(0);      // [4] do not use .read()
    return *p;
}

Operation &ARRITEM(GenObject &arr, int idx, GenObject &item, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = ARRITEM_gen;
    p->add_arg(p);      // 0
    p->add_arg(&arr);   // 1
    p->add_arg(new DecConst(idx));   // 2
    p->add_arg(&item);  // 3
    p->add_arg(0);      // [4] do not use .read()
    return *p;
}

Operation &ARRITEM(GenObject &arr, int idx) {
    Operation *p = new Operation(0, "");
    p->igen_ = ARRITEM_gen;
    p->add_arg(p);      // 0
    p->add_arg(&arr);   // 1
    p->add_arg(new DecConst(idx));   // 2
    p->add_arg(&arr);  // 3
    p->add_arg(0);      // [4] do not use .read()
    return *p;
}

Operation &ARRITEM_B(GenObject &arr, GenObject &idx, GenObject &item, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = ARRITEM_gen;
    p->add_arg(p);      // 0
    p->add_arg(&arr);   // 1
    p->add_arg(&idx);   // 2
    p->add_arg(&item);  // 3
    p->add_arg(p);      // [4] use .read()
    return *p;
}


//IF_OTHERWISE
std::string IF_OTHERWISE_gen(GenObject **args) {
    std::string ret = "";
    ret += Operation::obj2varname(args[1]);
    ret += " ? ";
    ret += Operation::obj2varname(args[2]);
    ret += " : ";
    ret += Operation::obj2varname(args[3]);
    return ret;
}

Operation &IF_OTHERWISE(GenObject &cond, GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Operation(0, comment);
    p->igen_ = IF_OTHERWISE_gen;
    p->add_arg(p);
    p->add_arg(&cond);
    p->add_arg(&a);
    p->add_arg(&b);
    return *p;
}


// IF
std::string IF_gen(GenObject **args) {
    std::string ret = addspaces();
    std::string A = Operation::obj2varname(args[1]);
    pushspaces();

    if (A.c_str()[0] == '(') {
    } else {
        A = "(" + A + ")";
    }
    ret += "if " + A;
    if (SCV_is_sysc()) {
        ret += " {";
    } else {
        ret += " begin";
        if (args[2]) {
            ret += ": " + args[2]->getStrValue().substr(1, args[2]->getStrValue().size()-2);
        }
    }
    ret += Operation::addtext(args[0], ret.size());
    ret += "\n";
    return ret;
}

void IF(GenObject &a, const char *comment) {
    Operation *p = new Operation(comment);
    Operation::push_obj(p);
    p->igen_ = IF_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(0);
}

void IFGEN(GenObject &a, StringConst *name, const char *comment) {
    Operation *p = new Operation(comment);
    Operation::push_obj(p);
    p->igen_ = IF_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(name);
}

// ELSE IF
std::string ELSIF_gen(GenObject **args) {
    std::string ret = "";
    std::string A = Operation::obj2varname(args[1]);

    if (A.c_str()[0] == '(') {
    } else {
        A = "(" + A + ")";
    }
    popspaces();
    if (SCV_is_sysc()) {
        ret += addspaces() + "} else if " + A + " {";
    } else {
        ret += addspaces() + "end else if " + A + " begin";
    }
    ret += Operation::addtext(args[0], ret.size());
    ret += "\n";
    pushspaces();
    return ret;
}

void ELSIF(GenObject &a, const char *comment) {
    Operation *p = new Operation(comment);
    Operation::pop_obj();
    Operation::push_obj(p);
    p->igen_ = ELSIF_gen;
    p->add_arg(p);
    p->add_arg(&a);
}

// ELSE
std::string ELSE_gen(GenObject **args) {
    std::string ret = "";
    popspaces();
    if (SCV_is_sysc()) {
        ret += addspaces() + "} else {\n";
    } else {
        if (args[1]) {
            ret += addspaces();
            ret += "end: " + args[1]->getStrValue().substr(1, args[1]->getStrValue().size()-2) + "_en\n";
            ret += addspaces();
            ret += "else begin: " + args[1]->getStrValue().substr(1, args[1]->getStrValue().size()-2) + "_dis";
        } else {
            ret += addspaces();
            ret += "end else begin";
        }
        ret += "\n";

    }
    pushspaces();
    return ret;
}

void ELSE(const char *comment) {
    Operation *p = new Operation(comment);
    Operation::pop_obj();
    Operation::push_obj(p);
    p->igen_ = ELSE_gen;
    p->add_arg(p);
    p->add_arg(0);
}

void ELSEGEN(StringConst *name, const char *comment) {
    Operation *p = new Operation(comment);
    Operation::pop_obj();
    Operation::push_obj(p);
    p->igen_ = ELSE_gen;
    p->add_arg(p);
    p->add_arg(name);
}


// ENDIF
std::string ENDIF_gen(GenObject **args) {
    std::string ret = "";
    popspaces();
    if (SCV_is_sysc()) {
        ret = addspaces() + "}\n";
    } else {
        ret = addspaces() + "end";
        if (args[1]) {
            ret += ": " + args[1]->getStrValue().substr(1, args[1]->getStrValue().size()-2);
        }
        ret += "\n";
    }
    return ret;
}

void ENDIF(const char *comment) {
    Operation::pop_obj();
    Operation *p = new Operation(comment);
    p->igen_ = ENDIF_gen;
    p->add_arg(p);
    p->add_arg(0);
}

void ENDIFGEN(StringConst *name, const char *comment) {
    Operation::pop_obj();
    Operation *p = new Operation(comment);
    p->igen_ = ENDIF_gen;
    p->add_arg(p);
    p->add_arg(name);
}


// SWITCH
std::string SWITCH_gen(GenObject **args) {
    std::string ret = addspaces();
    std::string A = Operation::obj2varname(args[1], "r", true);
    pushspaces();

    if (A.c_str()[0] == '(') {
    } else {
        A = "(" + A + ")";
    }
    if (SCV_is_sysc()) {
        ret += "switch " + A + " {";
    } else {
        ret += "case " + A;
    }
    ret += Operation::addtext(args[0], ret.size());
    ret += "\n";
    return ret;
}

void SWITCH(GenObject &a, const char *comment) {
    Operation *p = new Operation(comment);
    Operation::push_obj(p);
    p->igen_ = SWITCH_gen;
    p->add_arg(p);
    p->add_arg(&a);
}

// CASE
std::string CASE_gen(GenObject **args) {
    std::string ret = "";
    std::string A = Operation::obj2varname(args[1]);
    popspaces();
    if (SCV_is_sysc()) {
        ret += addspaces() + "case " + A + ":";
    } else {
        ret += addspaces() + A + ": begin";
    }
    ret += Operation::addtext(args[0], ret.size());
    ret += "\n";
    pushspaces();
    return ret;
}

void CASE(GenObject &a, const char *comment) {
    Operation *p = new Operation(comment);
    Operation::pop_obj();
    Operation::push_obj(p);
    p->igen_ = CASE_gen;
    p->add_arg(p);
    p->add_arg(&a);
}

// CASEDEF
std::string CASEDEF_gen(GenObject **args) {
    std::string ret = "";
    popspaces();
    ret += addspaces();
    if (SCV_is_sysc()) {
        ret += "default:";
    } else {
        ret += "default: begin";
    }
    ret += "\n";
    pushspaces();
    return ret;
}

void CASEDEF(const char *comment) {
    Operation *p = new Operation(comment);
    Operation::pop_obj();
    Operation::push_obj(p);
    p->igen_ = CASEDEF_gen;
    p->add_arg(p);
}

// ENDCASE
std::string ENDCASE_gen(GenObject **args) {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret += addspaces() + "break;\n";
    } else {
        popspaces();
        ret += addspaces() + "end\n";
        pushspaces();
    }
    return ret;
}

void ENDCASE(const char *comment) {
    Operation *p = new Operation(comment);
    p->igen_ = ENDCASE_gen;
    p->add_arg(p);
}

// ENDSWITCH
std::string ENDSWITCH_gen(GenObject **args) {
    popspaces();
    std::string ret = addspaces();
    if (SCV_is_sysc()) {
        ret += "}\n";
    } else {
        ret += "endcase\n";
    }
    return ret;
}

void ENDSWITCH(const char *comment) {
    Operation::pop_obj();
    Operation *p = new Operation(comment);
    p->igen_ = ENDSWITCH_gen;
    p->add_arg(p);
}

// GENVAR

// FOR
std::string FOR_gen(GenObject **args) {
    std::string ret = addspaces();
    GenObject *op = args[0];
    std::string i = Operation::obj2varname(args[1]);
    pushspaces();
    std::string start = Operation::obj2varname(args[2]);
    std::string end = Operation::obj2varname(args[3]);
    std::string dir = Operation::obj2varname(args[4]);
    GenObject *gename = args[5];


    if (SCV_is_sysc()) {
        ret += "for (int " + i + " = ";
    } else {
        if (args[1]->isGenVar()) {
            ret += "for (genvar " + i + " = ";
        } else {
            ret += "for (int " + i + " = ";
        }
    }
    ret += start + "; ";
    ret += i;
    if (dir == "\"++\"") {
        ret += " < ";
        dir = std::string("++");
    } else {
        ret += " >= ";
        dir = std::string("--");
    }
    ret += end + "; ";
    ret += i + dir;
    ret += ")";
    if (SCV_is_sysc()) {
        ret += " {\n";
    } else {
        ret += " begin";
        if (gename) {
            ret += ": " + gename->getStrValue().substr(1, gename->getStrValue().size()-2);
        }
        ret += "\n";
    }
    return ret;
}

GenObject &FOR(const char *i, GenObject &start, GenObject &end, const char *dir, const char *comment) {
    Operation *p = new Operation(comment);
    I32D *ret = new I32D(NO_PARENT, i, &start, NO_COMMENT);
    Operation::push_obj(p);
    p->igen_ = FOR_gen;
    p->add_arg(p);
    p->add_arg(ret);
    p->add_arg(&start);
    p->add_arg(&end);
    p->add_arg(new StringConst(dir));
    p->add_arg(0);
    return *ret;
}

// 'generate' for cycle used in rtl, it is the same for in systemc
GenObject &FORGEN(const char *i,
                  GenObject &start,
                  GenObject &end,
                  const char *dir,
                  StringConst *name,
                  const char *comment) {
    Operation *p = new Operation(comment);
    I32D *ret = new GenVar(NO_PARENT, i, "0", NO_COMMENT);
    Operation::push_obj(p);
    p->igen_ = FOR_gen;
    p->add_arg(p);
    p->add_arg(ret);
    p->add_arg(&start);
    p->add_arg(&end);
    p->add_arg(new StringConst(dir));
    p->add_arg(name);
    return *ret;
}

// ENDFOR
std::string ENDFOR_gen(GenObject **args) {
    std::string ret = "";
    popspaces();
    if (SCV_is_sysc()) {
        ret = addspaces() + "}\n";
    } else {
        GenObject *gename = args[1];
        ret = addspaces() + "end";
        if (gename) {
            ret += ": " + gename->getStrValue().substr(1, gename->getStrValue().size()-2);
        }
        ret += "\n";
    }
    return ret;
}

void ENDFOR(const char *comment) {
    Operation::pop_obj();
    Operation *p = new Operation(comment);
    p->igen_ = ENDFOR_gen;
    p->add_arg(p);
    p->add_arg(0);
}

void ENDFORGEN(StringConst *name, const char *comment) {
    Operation::pop_obj();
    Operation *p = new Operation(comment);
    p->igen_ = ENDFOR_gen;
    p->add_arg(p);
    p->add_arg(name);
}



// WHILE
std::string WHILE_gen(GenObject **args) {
    std::string ret = addspaces();
    std::string A = Operation::obj2varname(args[1], "r", true);
    pushspaces();

    if (A.c_str()[0] == '(') {
    } else {
        A = "(" + A + ")";
    }
    ret += "while " + A;
    if (SCV_is_sysc()) {
        ret += " {\n";
    } else {
        ret += " begin\n";
    }
    return ret;
}

void WHILE(GenObject &a, const char *comment) {
    Operation *p = new Operation(comment);
    Operation::push_obj(p);
    p->igen_ = WHILE_gen;
    p->add_arg(p);
    p->add_arg(&a);
}


// ENDWHILE
std::string ENDWHILE_gen(GenObject **args) {
    popspaces();
    std::string ret = addspaces();
    if (SCV_is_sysc()) {
        ret += "}\n";
    } else {
        ret += "end\n";
    }
    return ret;
}

void ENDWHILE(const char *comment) {
    Operation::pop_obj();
    Operation *p = new Operation(comment);
    p->igen_ = ENDWHILE_gen;
    p->add_arg(p);
}


// Sync reset
std::string SYNC_RESET_gen(GenObject **args) {
    ModuleObject *m = static_cast<ModuleObject *>(args[1]);
    std::string xrst = Operation::obj2varname(args[2]);
    std::string ret;
    ret = m->generate_all_proc_v_reset(xrst);
    return ret;
}

void SYNC_RESET(GenObject &a, GenObject *xrst, const char *comment) {
    Operation *p = new Operation(comment);
    p->igen_ = SYNC_RESET_gen;
    p->add_arg(p);
    p->add_arg(&a);
    p->add_arg(xrst);
}

// CALLF
std::string CALLF_gen(GenObject **args) {
    std::string ret = addspaces();
    if (args[1]) {
        ret += Operation::obj2varname(args[1], "v") + " = ";
    }
    ret += args[2]->getName();
    ret += "(";
    size_t cnt = reinterpret_cast<size_t>(args[3]);
    pushspaces();
    pushspaces();
    for (size_t i = 0; i < cnt; i++) {
        if (i > 0) {
            ret += ",\n" + addspaces();
        }
        ret += Operation::obj2varname(args[4 + i]);
    }
    popspaces();
    popspaces();

    ret += ");\n";
    return ret;
}

void CALLF(GenObject *ret, GenObject &a, size_t argcnt, ...) {
    Operation *p = new Operation("");
    GenObject *obj;
    p->igen_ = CALLF_gen;
    p->add_arg(p);
    p->add_arg(ret);
    p->add_arg(&a);
    p->add_arg(reinterpret_cast<GenObject *>(argcnt));
    va_list arg;
    va_start(arg, argcnt);
    for (int i = 0; i < argcnt; i++) {
        obj = va_arg(arg, GenObject *);
        p->add_arg(obj);
    }
    va_end(arg);
}

// FOPEN
std::string FOPEN_gen(GenObject **args) {
    std::string ret = addspaces();
    ret += Operation::obj2varname(args[1]);
    if (SCV_is_sysc()) {
        ret += " = fopen(";
        ret += Operation::obj2varname(args[2]);
        if (args[2]->isString()) {
            ret += ".c_str()";
        }
        ret += ", \"wb\");\n";
    } else {
        ret += " = $fopen(" + Operation::obj2varname(args[2]) + ", \"wb\");\n";
        ret += addspaces() + "assert (" + Operation::obj2varname(args[1]) + ")\n";
        ret += addspaces() + "else begin\n";
        ret += addspaces() + "    $warning(\"Cannot open log-file\");\n";
        ret += addspaces() + "end\n";
    }
    return ret;
}

void FOPEN(GenObject &f, GenObject &str) {
    Operation *p = new Operation("");
    p->igen_ = FOPEN_gen;
    p->add_arg(p);
    p->add_arg(&f);
    p->add_arg(&str);
}


// FWRITE
std::string FWRITE_gen(GenObject **args) {
    std::string ret = addspaces();
    if (SCV_is_sysc()) {
        ret += "fwrite(";
        ret += Operation::obj2varname(args[2]) + ".c_str(), 1, ";
        ret += Operation::obj2varname(args[2]) + ".size(), ";
        ret += Operation::obj2varname(args[1]);
    } else {
        ret += "$fwrite(";
        ret += Operation::obj2varname(args[1]);
        ret += ", \"%s\", ";
        ret += Operation::obj2varname(args[2]);
    }
    ret += ");\n";
    return ret;
}

void FWRITE(GenObject &f, GenObject &str) {
    Operation *p = new Operation("");
    p->igen_ = FWRITE_gen;
    p->add_arg(p);
    p->add_arg(&f);
    p->add_arg(&str);
}

// FWRITE
std::string FWRITECHAR_gen(GenObject **args) {
    std::string ret = addspaces();
    if (SCV_is_sysc()) {
        ret += "fwrite(";
        ret += Operation::obj2varname(args[2]) + "to_uint(), 1, 1, ";
        ret += Operation::obj2varname(args[1]);
    } else {
        ret += "$fwrite(";
        ret += Operation::obj2varname(args[1]);
        ret += ", \"%c\", ";
        ret += Operation::obj2varname(args[2]);
    }
    ret += ");\n";
    return ret;
}

void FWRITECHAR(GenObject &f, GenObject &str) {
    Operation *p = new Operation("");
    p->igen_ = FWRITECHAR_gen;
    p->add_arg(p);
    p->add_arg(&f);
    p->add_arg(&str);
}

// FFLUSH
std::string FFLUSH_gen(GenObject **args) {
    std::string ret = addspaces();
    if (SCV_is_sv()) {
        ret += "$";
    }
    ret += "fflush(";
    ret += Operation::obj2varname(args[1]);
    ret += ");\n";
    return ret;
}

void FFLUSH(GenObject &f) {
    Operation *p = new Operation("");
    p->igen_ = FFLUSH_gen;
    p->add_arg(p);
    p->add_arg(&f);
}

// READMEMH
std::string READMEMH_gen(GenObject **args) {
    std::string ret = addspaces();
    if (SCV_is_sysc()) {
        ret += "SV_readmemh(";
        ret += Operation::obj2varname(args[1]) + ".c_str(), ";
        ret += Operation::obj2varname(args[2]);
        ret += ");\n";
    } else if (SCV_is_sv()) {
        ret += "$readmemh(";
        ret += Operation::obj2varname(args[1]) + ", ";
        ret += Operation::obj2varname(args[2]);
        ret += ");\n";
    } else if (SCV_is_vhdl()) {
        ret += Operation::obj2varname(args[2]);
        ret += " = init_rom(";
        ret += Operation::obj2varname(args[1]) + ");\n";
    }
    return ret;
}

void READMEMH(GenObject &fname, GenObject &mem) {
    Operation *p = new Operation("");
    p->igen_ = READMEMH_gen;
    p->add_arg(p);
    p->add_arg(&fname);
    p->add_arg(&mem);
    mem.getParentFile()->setSvApiUsed();
}

// DISPLAYSTR
std::string DISPLAYSTR_gen(GenObject **args) {
    std::string ret = addspaces();
    if (SCV_is_sysc()) {
        ret += "SV_display(";
        ret += Operation::obj2varname(args[1]) + ".c_str()";
    } else {
        ret += "$display(\"%s\", ";
        ret += Operation::obj2varname(args[1]);
    }
    ret += ");\n";
    return ret;
}

void DISPLAYSTR(GenObject &str) {
    Operation *p = new Operation("");
    p->igen_ = DISPLAYSTR_gen;
    p->add_arg(p);
    p->add_arg(&str);
    str.getParentFile()->setSvApiUsed();
}


// NEW module instance
std::string NewOperation::generate() {
    if (SCV_is_sysc()) {
        return generate_sc();
    } else if (SCV_is_sv()) {
        return generate_sv();
    }
    return generate_vhdl();
}

std::string NewOperation::generate_sc() {
    std::string ret = "";
    std::string ln = "";
    std::string lasttmpl = "";
    std::string idx = "";
    int tcnt = 0;

    ln = addspaces();
    if (idx_) {
        idx = obj2varname(idx_);
        ret += addspaces();
        ret += "char tstr[256];\n";
        ret += addspaces();
        ret += "RISCV_sprintf(tstr, sizeof(tstr), \"" + instname_ + "%d\", " + idx + ");\n";
    }
    ln += instname_;
    if (idx.size()) {
        ln += "[" + idx + "]";
    }
    ln += " = new " + m_->getType();
    ret += ln;
    
    std::list<GenObject *>tmpllist;
    m_->getTmplParamList(tmpllist);
    tcnt = 0;
    if (tmpllist.size()) {
        ret += "<";
        for (auto &e : tmpllist) {
            if (tcnt) {
                ret += ",\n";
                for (int i = 0; i <= ln.size(); i++) {
                    ret += " ";
                }
            }
            ret += e->generate();
            lasttmpl = e->generate(); // need to properly compute space value
            tcnt++;
        }
        ret += ">";
        ln += lasttmpl + ">";
    }

    ret += "(";
    ln += "(";
    if (idx.size()) {
        ret += "tstr";
    } else {
        ret += "\"" + instname_ + "\"";
    }
    if (m_->isAsyncResetParam() && m_->getAsyncResetParam() == 0) {
        ret += ", async_reset";
    }
    std::list<GenObject *>genlist;
    m_->getParamList(genlist);
    for (auto &g : genlist) {
        ret += ",\n";
        for (int i = 0; i <= ln.size(); i++) {
            ret += " ";
        }
        ret += g->getStrValue();
    }
    ret += ");\n";

    std::list<GenObject *>iolist;
    m_->getIoList(iolist);
    for (auto &io : iolist) {
        ret += addspaces();
        ret += instname_;
        ret += gen_connection(io->getName());
        ret += ";";
        ret += "\n";
    }
    return ret;
}

std::string NewOperation::generate_sv() {
    std::string ret = "";
    std::string ln = "";
    std::string idx = "";
    int tcnt = 0;

    ln = addspaces();
    ln += m_->getType() + " ";
    ret += ln;
    
    std::list<GenObject *>genlist;
    m_->getTmplParamList(genlist);
    m_->getParamList(genlist);    
    tcnt = 0;
    if ((m_->isAsyncResetParam() && m_->getAsyncResetParam() == 0) || genlist.size()) {
        ret += "#(\n";
        pushspaces();
        if (m_->isAsyncResetParam() && m_->getAsyncResetParam() == 0) {
            ret += addspaces() + ".async_reset(async_reset)";
            if (genlist.size()) {
                ret += ",";
            }
            ret += "\n";
            tcnt++;
        }
        for (auto &e : genlist) {
            ret += addspaces() + "." + e->getName();
#if 1
            ret += "(" + e->getStrValue() + ")";
#else
            if (e->getObjValue()) {
                if (e->getObjValue()->getName() == "") {
                    ret += "(" + e->getObjValue()->getStrValue() + ")";
                } else {
                    // parameter is connected to some config parameter:
                    ret += "(" + e->getObjValue()->getName() + ")";
                }
            } else if (e->isParamTemplate()) {
                ret += "(" + e->generate() + ")";
            } else {
                ret += "(" + e->getName() + ")";
            }
#endif
            if (e != genlist.back()) {
                ret += ",";
            }
            ret += "\n";
            tcnt++;
        }
        popspaces();
        ret += addspaces() + ") ";
    }
    
    ret += instname_ + " (";
    pushspaces();

    std::list<GenObject *>iolist;
    m_->getIoList(iolist);
    ret += "\n";
    for (auto &io : iolist) {
        ret += gen_connection(io->getName());
        if (io != iolist.back()) {
            ret += ",";
        }
        ret += "\n";
    }
    popspaces();
    ret += addspaces() + ");";
    ret += "\n";
    return ret;
}

std::string NewOperation::generate_vhdl() {
    std::string ret = "";
    std::string ln = "";
    std::string idx = "";
    int tcnt = 0;

    ln = addspaces();
    ln += instname_ + " : " + m_->getType() + " ";
    ret += ln;
    
    std::list<GenObject *>tmpllist;
    m_->getTmplParamList(tmpllist);
    m_->getParamList(tmpllist);    
    tcnt = 0;
    if ((m_->isAsyncResetParam() && m_->getAsyncResetParam() == 0) || tmpllist.size()) {
        ret += "generic map (\n";
        pushspaces();
        if (m_->isAsyncResetParam() && m_->getAsyncResetParam() == 0) {
            ret += addspaces() + "async_reset => async_reset";
            if (tmpllist.size()) {
                ret += ",";
            }
            ret += "\n";
            tcnt++;
        }
        for (auto &e : tmpllist) {
            ret += addspaces();
#if 1
            ret += e->getName() + " => " + e->getStrValue();
#else
            if (e->getObjValue()) {
                // generic parameter but with the defined string value
                ret += e->getName() + " => " + e->getObjValue()->getName();
            } else if (e->isParamTemplate()) {
                ret += e->getName() + " => " + e->generate();
            } else {
                ret += e->getName() + " => " + e->getName();
            }
#endif
            if (e != tmpllist.back()) {
                ret += ",";
            }
            ret += "\n";
            tcnt++;
        }
        popspaces();
        ret += addspaces() + ") ";
    }
    
    ret += "port map (";
    pushspaces();

    std::list<GenObject *>iolist;
    m_->getIoList(iolist);
    ret += "\n";
    for (auto &io : iolist) {
        ret += gen_connection(io->getName());
        if (io != iolist.back()) {
            ret += ",";
        }
        ret += "\n";
    }
    popspaces();
    ret += addspaces() + ");";
    ret += "\n";
    return ret;
}


// CONNECT
std::string ConnectOperation::generate() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        if (idx_) {
            ret += "[" + obj2varname(idx_) + "]";
        }
        ret += "->";
        ret += port_->getName() + "(" + obj2varname(s_) + ")";
    } else if (SCV_is_sv()) {
        ret += addspaces() + ".";
        ret += port_->getName() + "(" + obj2varname(s_) + ")";
    } else {
        ret += addspaces();
        ret += port_->getName() + " => " + obj2varname(s_);
    }
    return ret;
}

// DECLARE_TSTR
std::string DECLARE_TSTR_gen(GenObject **args) {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret += addspaces() + "char tstr[256];\n";
    }
    return ret;
}

void DECLARE_TSTR() {
    Operation *p = new Operation("");
    p->igen_ = DECLARE_TSTR_gen;
    p->add_arg(p);
}

// INITIAL
std::string INITIAL_gen(GenObject **args) {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret += addspaces() + "// initial\n";
    } else {
        ret += addspaces() + "initial begin\n";
        pushspaces();
    }
    return ret;
}

void INITIAL() {
    Operation *p = new Operation("");
    Operation::push_obj(p);
    p->igen_ = INITIAL_gen;
    p->add_arg(p);
}

// ENDINITIAL
std::string ENDINITIAL_gen(GenObject **args) {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret += addspaces() + "// end initial\n";
    } else {
        popspaces();
        ret += addspaces() + "end\n";
    }
    return ret;
}

void ENDINITIAL() {
    Operation::pop_obj();
    Operation *p = new Operation("");
    p->igen_ = ENDINITIAL_gen;
    p->add_arg(p);
}


// GENERATE
std::string GENERATE_gen(GenObject **args) {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret += addspaces() + "// generate\n";
    } else {
        ret += addspaces() + "generate\n";
        pushspaces();
    }
    return ret;
}

void GENERATE(const char *name, const char *comment) {
    Operation *p = new Operation(comment);
    Operation::push_obj(p);
    p->igen_ = GENERATE_gen;
    p->add_arg(p);
    p->add_arg(new StringConst(name));
}

// ENDGENERATE
std::string ENDGENERATE_gen(GenObject **args) {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret += addspaces() + "// endgenerate\n";
    } else {
        popspaces();
        ret += addspaces() + "endgenerate\n";
    }
    return ret;
}

void ENDGENERATE(const char *name, const char *comment) {
    Operation::pop_obj();
    Operation *p = new Operation(comment);
    p->igen_ = ENDGENERATE_gen;
    p->add_arg(p);
    p->add_arg(new StringConst(name));
}

/**
    Assigning value in the out-of-combination method:
        a = 0
        a = b
 */
std::string AssignOperation::generate() {
    std::string ret = addspaces();
    if (SCV_is_sv()) {
        if (getParent() && getParent()->isProcess()) {
            GenObject *proc = getParent();
            proc->addPostAssign(this);
            parent_ = 0;
            return "";
        }
        ret += "assign ";
    }
    ret += obj2varname(a_, "v");
    if (SCV_is_vhdl()) {
        ret += " <= ";
    } else {
        ret += " = ";
    }
    if (b_) {
        ret += obj2varname(b_, "r");;
    } else if (SCV_is_sysc()) {
        ret += "0";
    } else if (SCV_is_sv()) {
        if (a_->getWidth() == 1) {
            ret += "1'b0";
        } else {
            ret += "'0";
        }
    } else if (SCV_is_vhdl()) {
        if (a_->getWidth() == 1) {
            ret += "'0'";
        } else {
            ret += "(others => '0')";
        }
    }
    ret +=  ";";
    ret += addtext(this, ret.size());
    ret += "\n";
    return ret;
}

}
