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
#include "operations.h"
#include "array.h"
#include "utils.h"
#include "modules.h"
#include "proc.h"
#include "comments.h"
#include <cstring>

namespace sysvc {

int stackcnt_ = 0;
GenObject *stackobj_[256] = {0};
int forcnt_ = 0;
GenObject *stackfor_[8] = {0};

Operation::Operation(GenObject *parent, const char *comment)
    : GenObject(parent, comment) {
}

Operation::Operation(const char *comment)
    : GenObject(top_obj(), comment) {
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

GenObject *Operation::push_for(GenObject *idx) {
    GenObject *ret = idx;
    if (idx == 0) {
        const char tname[2] = {'i' + static_cast<char>(forcnt_), 0};
        ret = new I32D(NO_PARENT, tname, "0", NO_COMMENT);
    }
    stackfor_[++forcnt_] = ret;
    return ret;
}

void Operation::pop_for() {
    if (--forcnt_ < 0) {
        SHOW_ERROR("Wrong FOR cycle stack index", forcnt_);
        forcnt_ = 0;
    }
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

std::string Operation::obj2varname(GenObject *obj, const char *prefix, bool read) {
    std::string ret = "";
    if (!obj) {
        return ret;
    }
    EPorts portid = PORT_IN;
    if (prefix[0] == 'r') {
        portid = PORT_OUT;
    }
    ret = obj->nameInModule(portid, read);

//    if (read && SCV_is_sysc()) {
//        if (obj->isInput() || obj->isSignal()) {
//            ret += ".read()";
//        }
//    }
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
    a_->setSelector(idx_);
    std::string ret = obj2varname(a_, getRegPrefix().c_str(), isForceRead());
    if (!a_->getDepth() && idx_) {
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
                                     bool non_blocking,
                                     GenObject *val,       // value to get
                                     GenObject *delay,
                                     const char *comment)
    : GetValueOperation(a, idx, item, h_as_width, h, l, comment),
        non_blocking_(non_blocking), v_(val), T_(delay) {
    if (top_obj()) {
        top_obj()->add_entry(this);
    }
}


std::string SetValueOperation::generate() {
    std::string ret = addspaces();
    //if (SCV_is_sysc()) {
        //ret += "// ";   // do not clear clock module
    //}
    if (isAssign() && SCV_is_sv()) {
        ret += "assign ";
    }

    if (SCV_is_sv() && T_) {
        ret += "#(" + T_->generate() + ") ";
    }
    ret += GetValueOperation::getStrValue();

    if (SCV_is_sysc() && T_) {
        ret += ".write(";
    } else if (SCV_is_vhdl()) {
        if (non_blocking_) {
            ret += " <= ";
        } else {
            ret += " := ";
        }
    } else if (SCV_is_sv()) {
        if (non_blocking_) {
            ret += " <= ";
        } else {
            ret += " = ";
        }
    } else {
        ret += " = ";
    }
    ret += v_->nameInModule(PORT_OUT, true);

    if (T_) {
        if (SCV_is_sysc()) {
            ret += ", " + T_->generate(), + " * SC_NS)";
        } else if (SCV_is_vhdl()) {
            ret += "delay " + T_->generate(), + " ns";
        }
    }
    a_->setSelector(0);
    ret += ";";
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


// SETZ
std::string SetZOperation::generate() {
    std::string ret = addspaces();

    if (SCV_is_sysc()) {
        // No systemc assigning Z    
        ret += "// ";
    }
    ret += Operation::obj2varname(a_, "v");
    ret += " = 1'bz;";
    ret += Operation::addtext(this, ret.size());
    ret += "\n";
    return ret;
}

Operation &SETZ(GenObject &a, const char *comment) {
    Operation *p = new SetZOperation(&a, comment);
    return *p;
}


// SETSTR
std::string SetStrOperation::generate() {
    std::string ret = addspaces();
    ret += Operation::obj2varname(a_, "v") + " = ";
    if (SCV_is_sv()) {
        ret += "{";
    }
    ret += str_->getStrValue();
    if (SCV_is_sv()) {
        ret += "}";
    }
    ret += ";";
    ret += Operation::addtext(this, ret.size());
    ret += "\n";
    return ret;
}

Operation &SETSTR(GenObject &a, const char *str, const char *comment) {
    Operation *p = new SetStrOperation(&a, new StringConst(str), comment);
    return *p;
}

// SETSTRF
std::string SetStrFmtOperation::generate() {
    std::string ret = addspaces();
    std::string fmt;
    if (SCV_is_sysc()) {
        ret += "RISCV_sprintf(tstr, sizeof(tstr), ";
    } else {
        ret += Operation::obj2varname(a_);
        if (add_) {
            ret += " += ";
        } else {
            ret += " = ";
        }
        ret += "$sformatf(";
    }
    fmt = fmt_->getStrValue();
    if (SCV_is_sv()) {
        // Remove RV_PRI64 occurences:
        const char *pos;
        while (pos = strstr(fmt.c_str(), "\" RV_PRI64 \"")) {
            fmt.erase(pos - fmt.c_str(), 12);
        }
    }
    ret += fmt;
    pushspaces();
    pushspaces();
    if (cnt_ > 1) {
        ret += ",\n" + addspaces();
    } else {
        ret += ", ";
    }
    for (size_t i = 0; i < cnt_; i++) {
        if (i > 0) {
            ret += ",\n" + addspaces();
        }
        ret += Operation::obj2varname(args_[i]);
        if (SCV_is_sysc()
            && args_[i]->isParamGeneric()
            && strstr(args_[i]->getType().c_str(), "string")) {
            ret += ".c_str()";
        }
    }
    popspaces();
    popspaces();
    ret += ");\n";
    if (SCV_is_sysc()) {
        ret += addspaces() + Operation::obj2varname(a_) + " ";
        if (add_) {
            ret += "+";
        }
        ret += "= std::string(tstr);\n";
    }
    return ret;
}

Operation &SETSTRF(GenObject &a, const char *fmt, size_t cnt, ...) {
    SetStrFmtOperation *p = new SetStrFmtOperation(&a,
                                          new StringConst(fmt),
                                          false,
                                          NO_COMMENT);
    GenObject *obj;
    va_list arg;
    va_start(arg, cnt);
    for (int i = 0; i < cnt; i++) {
        obj = va_arg(arg, GenObject *);
        p->addArgument(obj);
    }
    va_end(arg);
    return *p;
}

Operation &ADDSTRF(GenObject &a, const char *fmt, size_t cnt, ...) {
    SetStrFmtOperation *p = new SetStrFmtOperation(&a,
                                          new StringConst(fmt),
                                          true,
                                          NO_COMMENT);
    GenObject *obj;
    va_list arg;
    va_start(arg, cnt);
    for (int i = 0; i < cnt; i++) {
        obj = va_arg(arg, GenObject *);
        p->addArgument(obj);
    }
    va_end(arg);
    return *p;
}

// ADDSTRU8
std::string AddU8toStrOperation::generate() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret += addspaces() + "int tsz = RISCV_sprintf(tstr, sizeof(tstr), \"%s\", ";
        ret += Operation::obj2varname(in_) + ".c_str());\n";
        ret += addspaces() + "tstr[tsz++] = static_cast<char>(";
        ret += Operation::obj2varname(val_, "r", true) + ".to_uint());\n";
        ret += addspaces() + "tstr[tsz] = 0;\n";
        ret += addspaces() + Operation::obj2varname(out_);
        ret += " = tstr;\n";
    } else {
        ret += addspaces();
        ret += Operation::obj2varname(out_) + " = {";
        ret += Operation::obj2varname(in_) + ", ";
        ret += Operation::obj2varname(val_);
        ret += "};\n";
    }
    return ret;
}

Operation &ADDSTRU8(GenObject &strout, GenObject &strin, GenObject &val) {
    Operation *p = new AddU8toStrOperation(&strout, &strin, &val, NO_COMMENT);
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
std::string ArrItemOperation::generate() {
    std::string ret = "";
    arr_->setSelector(idx_);
    if (force_read_) {
        ret = Operation::obj2varname(item_, "r", true);
    } else {
        ret = Operation::obj2varname(item_);
    }
    arr_->setSelector(0);
    return ret;
}

Operation &ARRITEM(GenObject &arr, GenObject &idx, GenObject &item, const char *comment) {
    Operation *p = new ArrItemOperation(&arr, &idx, &item, false, comment);
    return *p;
}

Operation &ARRITEM(GenObject &arr, int idx, GenObject &item, const char *comment) {
    Operation *p = new ArrItemOperation(&arr, new DecConst(idx), &item, false, comment);
    return *p;
}

Operation &ARRITEM(GenObject &arr, int idx) {
    Operation *p = new ArrItemOperation(&arr, new DecConst(idx), &arr, false, NO_COMMENT);
    return *p;
}

Operation &ARRITEM_B(GenObject &arr, GenObject &idx, GenObject &item, const char *comment) {
    Operation *p = new ArrItemOperation(&arr, &idx, &item, true, comment);
    return *p;
}


//IF_OTHERWISE
std::string IfOtherwiseOperation::generate() {
    std::string ret = "";
    ret += Operation::obj2varname(cond_);
    ret += " ? ";
    ret += Operation::obj2varname(a_);
    ret += " : ";
    ret += Operation::obj2varname(b_);
    return ret;
}

Operation &IF_OTHERWISE(GenObject &cond, GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new IfOtherwiseOperation(&cond, &a, &b, comment);
    return *p;
}


// IF
std::string IfOperation::generate() {
    std::string ret = addspaces();
    std::string A = Operation::obj2varname(a_);
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
        if (genname_) {
            ret += ": " + genname_->getStrValue()
                    .substr(1, genname_->getStrValue().size() - 2);
        }
    }
    ret += Operation::addtext(this, ret.size());
    ret += "\n";
    for (auto &p : getEntries()) {
        ret += p->generate();
    }
    return ret;
}

Operation &IF(GenObject &a, const char *comment) {
    Operation *p = new IfOperation(&a, 0, comment);
    return *p;
}

Operation &IFGEN(GenObject &a, StringConst *name, const char *comment) {
    Operation *p = new IfOperation(&a, name, comment);
    return *p;
}

// ELSE IF
std::string ElseIfOperation::generate() {
    std::string ret = "";
    std::string A = Operation::obj2varname(a_);

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
    ret += Operation::addtext(this, ret.size());
    ret += "\n";
    pushspaces();
    return ret;
}

void ELSIF(GenObject &a, const char *comment) {
    new ElseIfOperation(&a, 0, comment);
}

// ELSE
std::string ElseOperation::generate() {
    std::string ret = "";
    popspaces();
    if (SCV_is_sysc()) {
        ret += addspaces() + "} else {\n";
    } else {
        if (genname_) {
            ret += addspaces();
            ret += "end: " + genname_->getStrValue()
                    .substr(1, genname_->getStrValue().size() - 2) + "_en\n";
            ret += addspaces();
            ret += "else begin: " + genname_->getStrValue()
                    .substr(1, genname_->getStrValue().size() - 2) + "_dis";
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
    new ElseOperation(0, comment);
}

void ELSEGEN(StringConst *name, const char *comment) {
    new ElseOperation(name, comment);
}


// ENDIF
std::string EndIfOperation::generate() {
    std::string ret = "";
    popspaces();
    if (SCV_is_sysc()) {
        ret = addspaces() + "}\n";
    } else {
        ret = addspaces() + "end";
        if (genname_) {
            ret += ": " + genname_->getStrValue()
                    .substr(1, genname_->getStrValue().size() - 2);
        }
        ret += "\n";
    }
    return ret;
}

void ENDIF(const char *comment) {
    new EndIfOperation(0, comment);
}

void ENDIFGEN(StringConst *name, const char *comment) {
    new EndIfOperation(name, comment);
}


// SWITCH
std::string SwitchOperation::generate() {
    std::string ret = addspaces();
    std::string A = a_->nameInModule(PORT_OUT, true);
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
    ret += Operation::addtext(this, ret.size());
    ret += "\n";
    for (auto &p: getEntries()) {
        ret += p->generate();
    }
    return ret;
}

Operation &SWITCH(GenObject &a, const char *comment) {
    Operation *p = new SwitchOperation(&a, comment);
    return *p;
}

// CASE
std::string CaseOperation::generate() {
    std::string ret = "";
    popspaces();
    ret += addspaces();
    if (SCV_is_sysc()) {
        if (a_) {
            std::string A = Operation::obj2varname(a_);
            ret += "case " + A + ":";
        } else {
            ret += "default:";
        }
    } else {
        if (a_) {
            std::string A = Operation::obj2varname(a_);
            ret += A + ": begin";
        } else {
            ret += "default: begin";
        }
    }
    ret += Operation::addtext(this, ret.size());
    ret += "\n";
    pushspaces();
    return ret;
}

void CASE(GenObject &a, const char *comment) {
    Operation *p = new CaseOperation(&a, comment);
}

void CASEDEF(const char *comment) {
    Operation *p = new CaseOperation(0, comment);
}

// ENDCASE
std::string EndCaseOperation::generate() {
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
    Operation *p = new EndCaseOperation(comment);
}

// ENDSWITCH
std::string EndSwitchOperation::generate() {
    std::string ret;
    popspaces();
    ret = addspaces();
    if (SCV_is_sysc()) {
        ret += "}\n";
    } else {
        ret += "endcase\n";
    }
    return ret;
}

void ENDSWITCH(const char *comment) {
    Operation *p = new EndSwitchOperation(comment);
}

// FOR
std::string ForOperation::generate() {
    std::string ret = addspaces();
    std::string i = Operation::obj2varname(i_);
    pushspaces();
    std::string start = Operation::obj2varname(start_);
    std::string end = Operation::obj2varname(end_);
    std::string dir = Operation::obj2varname(dir_);

    if (SCV_is_sysc()) {
        ret += "for (int " + i + " = ";
    } else {
        if (genname_) {
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
        if (genname_) {
            ret += ": " + genname_->getStrValue().substr(1,
                                genname_->getStrValue().size()-2);
        }
        ret += "\n";
    }
    for (auto &p: getEntries()) {
        ret += p->generate();
    }
    return ret;
}

GenObject &FOR_INC(GenObject &end, const char *comment) {
    new ForOperation(stackobj_[stackcnt_],
                    0,
                    new DecConst(0),
                    &end,
                    new StringConst("++"),
                    0,
                    comment);
    return *stackfor_[forcnt_];
}


GenObject &FOR(const char *i, GenObject &start, GenObject &end, const char *dir, const char *comment) {
    I32D *ret = new I32D(NO_PARENT, i, &start, NO_COMMENT);
    Operation *p = new ForOperation(stackobj_[stackcnt_],
                                    ret,
                                    &start,
                                    &end,
                                    new StringConst(dir),
                                    0,
                                    comment);
    return *ret;
}

// 'generate' for cycle used in rtl, it is the same for in systemc
GenObject &FORGEN(const char *i,
                  GenObject &start,
                  GenObject &end,
                  const char *dir,
                  StringConst *name,
                  const char *comment) {
    I32D *ret = new GenVar(NO_PARENT, i, "0", NO_COMMENT);
    Operation *p = new ForOperation(Operation::top_obj(),
                                    ret,
                                    &start,
                                    &end,
                                    new StringConst(dir),
                                    name,
                                    comment);
    return *ret;
}

// ENDFOR
std::string EndForOperation::generate() {
    std::string ret = "";
    popspaces();
    if (SCV_is_sysc()) {
        ret = addspaces() + "}\n";
    } else {
        ret = addspaces() + "end";
        if (genname_) {
            ret += ": " + genname_->getStrValue().substr(1,
                            genname_->getStrValue().size()-2);
        }
        ret += "\n";
    }
    return ret;
}

void ENDFOR(const char *comment) {
    Operation *p = new EndForOperation(0, comment);
}

void ENDFORGEN(StringConst *name, const char *comment) {
    Operation *p = new EndForOperation(name, comment);
}



// WHILE
std::string WhileOperation::generate() {
    std::string ret = addspaces();
    std::string A = Operation::obj2varname(a_, "r", true);
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
    for (auto &p: getEntries()) {
        ret += p->generate();
    }
    return ret;
}

Operation &WHILE(GenObject &a, const char *comment) {
    Operation *p = new WhileOperation(&a, comment);
    return *p;
}

// ENDWHILE
std::string EndWhileOperation::generate() {
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
    new EndWhileOperation(comment);
}

// Sync reset
std::string SyncResetOperation::generate() {
    std::string ret;
    GenObject *m;
    if (!proc_->isProcess()) {
        SHOW_ERROR("SYNC_RESET should be inside proc",
                    proc_->getName().c_str());
        return ret;
    }
    m = proc_->getParent();
    if (!m || !m->isModule()) {
        SHOW_ERROR("SYNC_RESET proc not in a module",
                    proc_->getName().c_str());
        return ret;
    }
    
    std::list<RegTypedefStruct *> *reglist =
        dynamic_cast<ProcObject *>(proc_)->getpSortedRegs();

    if (reglist == 0) {
        SHOW_ERROR("No registers in process %s", proc_->getName().c_str());
        return ret;
    }

    // "async_reset" could be forcibly disabled in module (cdc_afifo):
    GenObject *async_reset = m->getChildByName("async_reset");
    GenObject *op = 0;
    Operation::push_obj(NO_PARENT);
    for (auto &r : *reglist) {
        if (r->getResetActive() == ACTIVE_NONE) {
            continue;
        } else if (r->getResetActive() == ACTIVE_LOW) {
            if (async_reset) {
                op = &AND2(INV_L(*async_reset), EZ(*r->getResetPort()));
            } else {
                op = &EZ(*r->getResetPort());
            }
        } else if (r->getResetActive() == ACTIVE_HIGH) {
            if (async_reset) {
                op = &AND2(INV_L(*async_reset), NZ(*r->getResetPort()));
            } else {
                op = &NZ(*r->getResetPort());
            }
        }
        if (xrst_) {
            if (op) {
                op = &OR2(*op, *xrst_);
            } else {
                op = xrst_;
            }
        }
        if (op) {
            op = &IF (*op);
                if (SCV_is_sysc()) {
                    CALLF(0, *r->rst_func_instance(), 1, r->v_instance());
                } else {
                    SETVAL(*r->v_instance(), *r->rst_instance());
                }
            ENDIF();
            ret += op->generate();
        }
    }
    Operation::pop_obj();
    return ret;
}

void SYNC_RESET(GenObject *xrst) {
    new SyncResetOperation(Operation::top_obj(), xrst);
}

// CALLF
std::string CallFuncOperation::generate() {
    std::string ret = addspaces();
    if (ret_) {
        ret += ret_->nameInModule(PORT_IN, false) + " = ";
    }
    ret += a_->getName();
    ret += "(";
    pushspaces();
    pushspaces();
    for (size_t i = 0; i < cnt_; i++) {
        if (i > 0) {
            ret += ",\n" + addspaces();
        }
        ret += args_[i]->nameInModule(PORT_OUT, true);
    }
    popspaces();
    popspaces();

    ret += ");\n";
    return ret;
}

Operation &CALLF(GenObject *ret, GenObject &a, size_t argcnt, ...) {
    CallFuncOperation *p = new CallFuncOperation(ret, &a);
    GenObject *obj;
    va_list arg;
    va_start(arg, argcnt);
    for (int i = 0; i < argcnt; i++) {
        obj = va_arg(arg, GenObject *);
        p->addArgument(obj);
    }
    va_end(arg);
    return *p;
}

// FOPEN
std::string FileOpenOperation::generate() {
    std::string ret = addspaces();
    ret += Operation::obj2varname(fname_);
    if (SCV_is_sysc()) {
        ret += " = fopen(";
        ret += Operation::obj2varname(str_);
        ret += ".c_str()";
        ret += ", \"wb\");\n";
    } else {
        ret += " = $fopen(" + Operation::obj2varname(str_) + ", \"wb\");\n";
        ret += addspaces() + "assert (" + Operation::obj2varname(fname_) + ")\n";
        ret += addspaces() + "else begin\n";
        ret += addspaces() + "    $warning(\"Cannot open log-file\");\n";
        ret += addspaces() + "end\n";
    }
    return ret;
}

Operation &FOPEN(GenObject &f, GenObject &str) {
    Operation *p = new FileOpenOperation(&f, &str, NO_COMMENT);
    return *p;
}


// FWRITE
std::string FileWriteOperation::generate() {
    std::string ret = addspaces();
    if (SCV_is_sysc()) {
        ret += "fwrite(";
        if (byte_) {
            ret += Operation::obj2varname(str_) + ".to_uint(), 1, 1, ";
        } else {
            ret += Operation::obj2varname(str_) + ".c_str(), 1, ";
            ret += Operation::obj2varname(str_) + ".size(), ";
        }
        ret += Operation::obj2varname(f_);
    } else {
        ret += "$fwrite(";
        ret += Operation::obj2varname(f_);
        if (byte_) {
            ret += ", \"%c\", ";
        } else {
            ret += ", \"%s\", ";
        }
        ret += Operation::obj2varname(str_);
    }
    ret += ");\n";
    return ret;
}

Operation &FWRITE(GenObject &f, GenObject &str) {
    Operation *p = new FileWriteOperation(&f, &str, false, NO_COMMENT);
    return *p;
}

Operation &FWRITECHAR(GenObject &f, GenObject &str) {
    Operation *p = new FileWriteOperation(&f, &str, true, NO_COMMENT);
    return *p;
}

// FFLUSH
std::string FileFlushOperation::generate() {
    std::string ret = addspaces();
    if (SCV_is_sv()) {
        ret += "$";
    }
    ret += "fflush(";
    ret += Operation::obj2varname(f_);
    ret += ");\n";
    return ret;
}

Operation &FFLUSH(GenObject &f) {
    Operation *p = new FileFlushOperation(&f, NO_COMMENT);
    return *p;
}

// READMEMH
std::string ReadMemHexOperation::generate() {
    std::string ret = addspaces();
    if (SCV_is_sysc()) {
        ret += "SV_readmemh(";
        ret += Operation::obj2varname(f_) + ".c_str(), ";
        ret += Operation::obj2varname(mem_);
        ret += ");\n";
    } else if (SCV_is_sv()) {
        ret += "$readmemh(";
        ret += Operation::obj2varname(f_) + ", ";
        ret += Operation::obj2varname(mem_);
        ret += ");\n";
    } else if (SCV_is_vhdl()) {
        ret += Operation::obj2varname(mem_);
        ret += " = init_rom(";
        ret += Operation::obj2varname(f_) + ");\n";
    }
    return ret;
}

Operation &READMEMH(GenObject &fname, GenObject &mem) {
    Operation *p = new ReadMemHexOperation(&fname, &mem, NO_COMMENT);
    mem.getParentFile()->setSvApiUsed();
    return *p;
}

// DISPLAYSTR
std::string DisplayStrOperation::generate() {
    std::string ret = addspaces();
    if (SCV_is_sysc()) {
        ret += "SV_display(";
        ret += Operation::obj2varname(str_) + ".c_str()";
    } else {
        ret += "$display(\"%s\", ";
        ret += Operation::obj2varname(str_);
    }
    ret += ");\n";
    return ret;
}

Operation &DISPLAYSTR(GenObject &str) {
    Operation *p = new DisplayStrOperation(&str, NO_COMMENT);
    str.getParentFile()->setSvApiUsed();
    return *p;
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
    /*if (m_->isAsyncResetParam() && m_->getAsyncResetParam() == 0) {
        ret += ", async_reset";
    }*/
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
    if (genlist.size()) {
        ret += "#(\n";
        pushspaces();
        for (auto &e : genlist) {
            ret += addspaces() + "." + e->getName();
            ret += "(" + e->getStrValue() + ")";
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

    ln = addspaces();
    ln += instname_ + " : " + m_->getType() + " ";
    ret += ln;
    
    std::list<GenObject *>tmpllist;
    m_->getTmplParamList(tmpllist);
    m_->getParamList(tmpllist);    
    if (tmpllist.size()) {
        ret += "generic map (\n";
        pushspaces();
        for (auto &e : tmpllist) {
            ret += addspaces();
            ret += e->getName() + " => " + e->getStrValue();
            if (e != tmpllist.back()) {
                ret += ",";
            }
            ret += "\n";
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
std::string DeclareTStrOperation::generate() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret += addspaces() + "char tstr[256];\n";
    }
    return ret;
}

void DECLARE_TSTR() {
    new DeclareTStrOperation(NO_COMMENT);
}

// INITIAL
std::string InitialOperation::generate() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret += addspaces() + "// initial\n";
    } else {
        ret += addspaces() + "initial begin\n";
        pushspaces();
    }
    for (auto &p: getEntries()) {
        ret += p->generate();
    }
    return ret;
}

Operation &INITIAL() {
    Operation *p = new InitialOperation(NO_COMMENT);
    return *p;
}

// ENDINITIAL
std::string EndInitialOperation::generate() {
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
    Operation *p = new EndInitialOperation(NO_COMMENT);
}


// GENERATE
std::string GenerateOperation::generate() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret += addspaces() + "// generate\n";
    } else {
        ret += addspaces() + "generate\n";
        pushspaces();
    }
    for (auto &p : getEntries()) {
        ret += p->generate();
    }
    return ret;
}

GenObject &GENERATE(const char *name, const char *comment) {
    Operation *p = new GenerateOperation(new StringConst(name), comment);
    return *p;
}

// ENDGENERATE
std::string EndGenerateOperation::generate() {
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
    new EndGenerateOperation(new StringConst(name), comment);
}

// posedge/negedge of signal
std::string EdgeOperation::generate() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        SHOW_ERROR("Not implemented %s", "edge");
    } else if (SCV_is_sv()) {
        if (edge_ == CLK_POSEDGE) {
            ret += "posedge ";
        } else if (edge_ == CLK_NEGEDGE) {
            ret += "negedge ";
        }
        ret += obj_->getName();
    }
    return ret;
};

Operation &EDGE(GenObject &obj, EClockEdge edge) {
    Operation *p = new EdgeOperation(&obj, edge);
    return *p;
}

Operation &EDGE(GenObject &obj, EResetActive edge) {
    EClockEdge clkedge = CLK_ALWAYS;
    if (edge == ACTIVE_LOW) {
        clkedge = CLK_NEGEDGE;
    } else if (edge == ACTIVE_HIGH) {
        clkedge = CLK_POSEDGE;
    }
    Operation *p = new EdgeOperation(&obj, clkedge);
    return *p;
}

// always_ff
std::string AlwaysOperation::generate() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        SHOW_ERROR("Not implemented %s", "always_ff");
    } else if (SCV_is_sv()) {
        ret += addspaces() + "always";
        if (clk_) {
            ret += "_ff @(";
        }
        if (cond_) {
            ret += cond_->generate();
        } else if (clk_) {
            ret += clk_->generate();
        }
        if (rst_) {
            ret += ", " + rst_->generate();
        }
        if (cond_ || clk_) {
            ret += ")";
        }
        ret += " begin\n";
        pushspaces();
    }
    for (auto &p : getEntries()) {
        ret += p->generate();
    }
    return ret;
}

GenObject &ALWAYS(GenObject *cond, const char *comment) {
    Operation *p = new AlwaysOperation(cond, 0, 0, comment);
    return *p;
}

GenObject &ALWAYS_FF(GenObject &clk, const char *comment) {
    Operation *p = new AlwaysOperation(0, &clk, 0, comment);
    return *p;
}

GenObject &ALWAYS_FF(GenObject &clk, GenObject &rst, const char *comment) {
    Operation *p = new AlwaysOperation(0, &clk, &rst, comment);
    return *p;
}

// end of always_ff
std::string EndAlwaysOperation::generate() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        SHOW_ERROR("Not implemented %s", "end");
    } else if (SCV_is_sv()) {
        popspaces();
        ret += addspaces() + "end\n";
    }
    return ret;
}

void ENDALWAYS(const char *comment) {
    new EndAlwaysOperation(comment);
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
