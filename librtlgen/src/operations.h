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

#pragma once

#include "genobjects.h"
#include "const.h"
#include "values.h"
#include "params.h"
#include "logic.h"
#include "modules.h"
#include "utils.h"
#include <iostream>
#include <list>
#include <map>

namespace sysvc {

class Operation : public GenObject {
 public:
    Operation(const char *comment = NO_COMMENT);
    Operation(GenObject *parent, const char *comment = NO_COMMENT);

    virtual bool isOperation() override { return true; }
    std::string nameInModule(EPorts portid, bool sc_read) override { return generate(); }

    static void start(GenObject *owner);
    static void push_obj(GenObject *obj);
    static void pop_obj();
    static GenObject *top_obj();
    static GenObject *push_for(GenObject *idx);
    static void pop_for();
    static std::string obj2varname(GenObject *obj, const char *prefix="r", bool read=false);
    static std::string addtext(GenObject *obj, size_t curpos);

    virtual std::string getName() override { return generate(); }
    virtual std::string getStrValue() override { return generate(); }
    virtual std::string generate() override {
        std::string ret;
        for (auto &e: getEntries()) {
            ret += e->generate();
        }
        return ret;
    }
};


class ConvertOperation : public Operation {
 public:
    ConvertOperation(GenObject *a, int sz, const char *comment)
        : Operation(NO_PARENT, comment), a_(a), objWidth_(sz) {
    }

    virtual uint64_t getValue() override { return a_->getValue(); }
    virtual std::string getStrValue() override { return generate(); }
    virtual uint64_t getWidth() override { return objWidth_.getValue(); }
    virtual std::string generate() = 0;

 protected:
    GenObject *a_;
    DecConst objWidth_;
};

class TwoStandardOperandsOperation : public Operation {
 public:
    TwoStandardOperandsOperation(GenObject *a, GenObject *b, const char *comment)
        : Operation(NO_PARENT, comment), a_(a), b_(b) {}

    virtual bool isGenericDep() override { return a_->isGenericDep() || b_->isGenericDep(); }
    virtual std::string getOperand() = 0;
    virtual uint64_t getValue() = 0;
    virtual std::string getStrValue() override;
    virtual uint64_t getWidth() override;
    virtual std::string generate() override { return getStrValue(); }

 protected:
    GenObject *a_;
    GenObject *b_;
};

class TwoStandardLogicOperation : public TwoStandardOperandsOperation {
 public:
    TwoStandardLogicOperation(GenObject *a, GenObject *b, const char *comment)
        : TwoStandardOperandsOperation(a, b, comment), objWidth_(1) {}

    virtual uint64_t getWidth() override { return objWidth_.getValue(); }

 protected:
    DecConst objWidth_;
};

class NStandardOperandsOperation : public Operation {
 public:
    NStandardOperandsOperation(bool oneline, const char *comment)
        : Operation(NO_PARENT, comment), oneline_(oneline) {}

    virtual std::string getOpeningBrace() { return std::string("("); }
    virtual std::string getClosingBrace() { return std::string(")"); }
    /** Format output (important for CCx):
            (A ||             (A
             B ||               || B
             ..                 || ..
             C)                 || C)
     */
    virtual bool isOperandNewLine() { return true; }
    virtual std::string getOperand() = 0;
    virtual uint64_t getValue() = 0;
    virtual std::string getStrValue() override;
    virtual uint64_t getWidth() override { return (*getEntries().begin())->getWidth(); }
    virtual std::string generate() override { return getStrValue(); }

 protected:
    bool oneline_;          // output in one line
};

class ReduceOperation : public Operation {
 public:
    ReduceOperation(GenObject *a, const char *comment)
        : Operation(NO_PARENT, comment), a_(a), objWidth_(1) {}

    virtual uint64_t getValue() = 0;
    virtual std::string getStrValue() = 0;
    virtual uint64_t getWidth() override { return objWidth_.getValue(); }
    virtual std::string generate() override { return getStrValue(); }

 protected:
    GenObject *a_;
    DecConst objWidth_;
};

/**
    Set array index. Applied only to next operation, then cleared
*/
class SetArrayIndexOperation : public Operation {
 public:
    SetArrayIndexOperation(GenObject *a, GenObject *idx, const char *comment)
        : Operation(top_obj(), comment), a_(a), idx_(idx) {}

    virtual std::string generate() override { 
        a_->setSelector(idx_);
        return std::string("");
    }

 protected:
    GenObject *a_;
    GenObject *idx_;
};

/**
    Get value. Do not have a parent:
        a
        a[h:l]
        a[arridx].item[h:l]
        a[arridx][h:l]
 */
class GetValueOperation : public Operation {
 public:
    GetValueOperation(GenObject *a,         // value to get
                      GenObject *idx,       // array index (depth should be non-zero)
                      GenObject *item,      // struct item
                      bool h_as_width,      // interpret h as width argument for sv [start +: width] operation
                      GenObject *h,         // MSB bit
                      GenObject *l,         // LSB bit
                      const char *comment);

    virtual std::string getStrValue() override;
    virtual std::string generate() override { return getStrValue(); }

 protected:
    virtual std::string getRegPrefix() { return std::string("r"); }     // prefix of a_ ("v" for set; "r" for get)
    virtual bool isForceRead() { return h_ ? true: false; }             // a_.read() forced

 protected:
    GenObject *a_;
    GenObject *idx_;
    GenObject *item_;
    GenObject *h_;
    GenObject *l_;
    bool h_as_width_;
};


/**
    Set value int variable:
        a = b
        #T a = b
        a[h:l] = b
        a[arridx].item[h:l] = b
        a[arridx][h:l] = b
 */
class SetValueOperation : public GetValueOperation {
 public:
    SetValueOperation(GenObject *a,         // value to set
                      GenObject *idx,       // array index (depth should be non-zero)
                      GenObject *item,      // struct item
                      bool h_as_width,      // interpret h as width argument for sv [start +: width] operation
                      GenObject *h,         // MSB bit
                      GenObject *l,         // LSB bit
                      bool non_blocking,    // false=blocking; true=non-blocking
                      GenObject *val,       // value to assign
                      GenObject *delay,     // delay
                      const char *comment);

    virtual std::string getStrValue() override { return generate(); }
    virtual std::string generate() override;

 protected:
    virtual std::string getRegPrefix() { return std::string("v"); }     // prefix of a_ ("v" for set; "r" for get)
    virtual bool isForceRead() { return false; }                        // a.read() forced
    virtual bool isAssign() { return false; }       // modificator to use out-of-process assign operator (<=)

 protected:
    GenObject *v_;
    GenObject *T_;
    bool non_blocking_;
};

/**
    Assign value is possible only for RTL (not in SystemC). SystemC should create process.
        assign a <= b;
 */
class AssignValueOperation : public SetValueOperation {
 public:
    // Array item operations
    AssignValueOperation(GenObject *a,         // value to set
                         GenObject *idx,       // array index (depth should be non-zero)
                         GenObject *item,      // struct item
                         bool h_as_width,      // interpret h as width argument for sv [start +: width] operation
                         GenObject *h,         // MSB bit
                         GenObject *l,         // LSB bit
                         GenObject *val,
                         const char *comment)
        : SetValueOperation(a, idx, item, h_as_width, h, l, false, val, 0, comment) {}

 protected:
    virtual bool isAssign() override { return true; }       // modificator to use out-of-process assign operator (<=)
};

/**
    Assigning value in the out-of-combination method:
        a = 0
        a = b
 */
class AssignOperation : public Operation {
 public:
    AssignOperation(GenObject &a, const char *comment)
        : Operation(comment), a_(&a), b_(0) {}
    AssignOperation(GenObject &a, GenObject &b, const char *comment)
        : Operation(comment), a_(&a), b_(&b) {}

    virtual bool isAssign() override { return true; }
    virtual std::string generate() override;
 protected:
    GenObject *a_;
    GenObject *b_;
};

/**
    Increment value (vhdl does not support += operation):
        a += b
        a[h:l] += b
        a[arridx].item[h:l] += b
        a[arridx][h:l] += b
 */
class IncrementValueOperation : public SetValueOperation {
 public:
    IncrementValueOperation(GenObject *a,   // value to set
                      GenObject *idx,       // array index (depth should be non-zero)
                      GenObject *item,      // struct item
                      bool h_as_width,      // interpret h as width argument for sv [start +: width] operation
                      GenObject *h,         // MSB bit
                      GenObject *l,         // LSB bit
                      GenObject *val,
                      const char *comment)
        : SetValueOperation(a, idx, item, h_as_width, h, l, false, val, 0, comment) {}

    virtual std::string generate() override;
};

/**
    Generate commenting string:
        // text
 */
class TextOperation : public Operation {
 public:
    TextOperation(const char *comment) : Operation(top_obj(), comment) {}
    virtual std::string generate() override;
};

/**
    Generate constant value to all bits. No parent.
 */
class AllConstOperation : public Operation {
 public:
    AllConstOperation(int v, const char *comment)
        : Operation(NO_PARENT, comment), v_(v) {}

    virtual std::string getStrValue() override;
    virtual std::string generate() override { return getStrValue(); }
 protected:
    int v_;
};

/**
    Bit selection a[idx].
 */
class BitOperation : public Operation {
 public:
    BitOperation(GenObject *a, GenObject *idx, const char *comment)
        : Operation(NO_PARENT, comment), a_(a), idx_(idx) {}
    virtual std::string getStrValue() override { return generate(); }
    virtual uint64_t getWidth() override { return 1; }
    virtual std::string generate() override;
 protected:
    GenObject *a_;
    GenObject *idx_;
};

/**
    Bits selection by range: a[h:l]
 */
class BitsOperation : public Operation {
 public:
    BitsOperation(GenObject *a, GenObject *h, GenObject *l, const char *comment)
        : Operation(NO_PARENT, comment), a_(a), h_(h), l_(l) {}
    virtual std::string getStrValue() override { return generate(); }
    virtual uint64_t getWidth() override { return h_->getValue() - l_->getValue() + 1; }
    virtual std::string generate() override;
 protected:
    GenObject *a_;
    GenObject *h_;
    GenObject *l_;
};

/**
    Bits selection by width: a[start +: width]
 */
class BitswOperation : public Operation {
 public:
    BitswOperation(GenObject *a, GenObject *start, GenObject *width, const char *comment)
        : Operation(NO_PARENT, comment), a_(a), start_(start), width_(width) {}
    virtual std::string getStrValue() override { return generate(); }
    virtual uint64_t getWidth() override { return width_->getValue(); }
    virtual std::string generate() override;
 protected:
    GenObject *a_;
    GenObject *start_;
    GenObject *width_;
};

/**
    Convert to signed int:
 */
class ToIntOperation : public ConvertOperation {
 public:
    ToIntOperation(GenObject *a, const char *comment)
        : ConvertOperation(a, 32, comment) {}
    virtual std::string generate() override;
};

/**
    Convert to unsigned int:
 */
class ToU32Operation : public ConvertOperation {
 public:
    ToU32Operation(GenObject *a, const char *comment)
        : ConvertOperation(a, 32, comment) {}
    virtual std::string generate() override;
};

/**
    Convert to uint64:
 */
class ToU64Operation : public ConvertOperation {
 public:
    ToU64Operation(GenObject *a, const char *comment)
        : ConvertOperation(a, 64, comment) {}
    virtual std::string generate() override;
};

/**
    Convert to string:
 */
class ToCStrOperation : public ConvertOperation {
 public:
    ToCStrOperation(GenObject *a, const char *comment)
        : ConvertOperation(a, 0, comment) {}
    virtual std::string generate() override;
};

/**
    Convert cs_biguint<> to uint64_ (for sysc only):
 */
class BigToU64Operation : public ConvertOperation {
 public:
    BigToU64Operation(GenObject *a, const char *comment)
        : ConvertOperation(a, 64, comment) {}
    virtual std::string generate() override;
};

/**
    Convert to biguint<> (effect in systemc only):
 */
class ToBigOperation : public ConvertOperation {
 public:
    ToBigOperation(GenObject *a, int sz, const char *comment)
        : ConvertOperation(a, sz, comment) {}
    virtual std::string generate() override;
};

class SetZOperation : public Operation {
 public:
    SetZOperation(GenObject *a, const char *comment)
        : Operation(comment), a_(a) {
    }
    virtual std::string getName() override { return ""; }
    virtual std::string generate() override;
 protected:
    GenObject *a_;
};

Operation &SETZ(GenObject &a, const char *comment="");

class SetStrOperation : public Operation {
 public:
    SetStrOperation(GenObject *a, GenObject *str, const char *comment)
        : Operation(comment), a_(a), str_(str) {
    }
    virtual std::string getName() override { return ""; }
    virtual std::string generate() override;
 protected:
    GenObject *a_;
    GenObject *str_;
};

Operation &SETSTR(GenObject &a, const char *str, const char *comment="");


class SetStrFmtOperation : public Operation {
 public:
    SetStrFmtOperation(GenObject *a,
                       GenObject *fmt,
                       bool add,
                       const char *comment)
        : Operation(comment), a_(a), fmt_(fmt), add_(add), cnt_(0) {
    }
    virtual std::string getName() override { return ""; }
    virtual std::string generate() override;
    virtual void addArgument(GenObject *arg) { 
        args_[cnt_++] = arg;
    }
 protected:
    GenObject *a_;
    GenObject *fmt_;
    bool add_;          // false: str = val, true : str += val
    int cnt_;
    GenObject *args_[256];
};
Operation &SETSTRF(GenObject &a, const char *fmt, size_t cnt, ...);
Operation &ADDSTRF(GenObject &a, const char *fmt, size_t cnt, ...);

class AddU8toStrOperation : public Operation {
 public:
    AddU8toStrOperation(GenObject *out,
                        GenObject *in,
                        GenObject *val,
                        const char *comment)
        : Operation(comment), out_(out), in_(in), val_(val) {
    }
    virtual std::string getName() override { return ""; }
    virtual std::string generate() override;
 protected:
    GenObject *out_;
    GenObject *in_;
    GenObject *val_;
};

Operation &ADDSTRU8(GenObject &strout, GenObject &strin, GenObject &val);

/**
    a == b ? 1 : 0
 */
class EqOperation : public TwoStandardLogicOperation {
 public:
    EqOperation(GenObject *a, GenObject *b, const char *comment)
        : TwoStandardLogicOperation(a, b, comment) {}

    virtual std::string getOperand() override;
    virtual uint64_t getValue() override { return a_->getValue() == b_->getValue() ? 1: 0; }
};

/**
    a != b ? 1 : 0
 */
class NeOperation : public TwoStandardLogicOperation {
 public:
    NeOperation(GenObject *a, GenObject *b, const char *comment)
        : TwoStandardLogicOperation(a, b, comment) {}

    virtual std::string getOperand() override;
    virtual uint64_t getValue() override { return a_->getValue() != b_->getValue() ? 1: 0; }
};

/**
    a > b ? 1 : 0
 */
class GtOperation : public TwoStandardLogicOperation {
 public:
    GtOperation(GenObject *a, GenObject *b, const char *comment)
        : TwoStandardLogicOperation(a, b, comment) {}

    virtual std::string getOperand() override { return std::string(" > "); }
    virtual uint64_t getValue() override { return a_->getValue() > b_->getValue() ? 1: 0; }
};

/**
    a >= b ? 1 : 0
 */
class GeOperation : public TwoStandardLogicOperation {
 public:
    GeOperation(GenObject *a, GenObject *b, const char *comment)
        : TwoStandardLogicOperation(a, b, comment) {}

    virtual std::string getOperand() override { return std::string(" >= "); }
    virtual uint64_t getValue() override { return a_->getValue() >= b_->getValue() ? 1: 0; }
};

/**
    a < b ? 1 : 0
 */
class LsOperation : public TwoStandardLogicOperation {
 public:
    LsOperation(GenObject *a, GenObject *b, const char *comment)
        : TwoStandardLogicOperation(a, b, comment) {}

    virtual std::string getOperand() override { return std::string(" < "); }
    virtual uint64_t getValue() override { return a_->getValue() < b_->getValue() ? 1: 0; }
};

/**
    a <= b ? 1 : 0
 */
class LeOperation : public TwoStandardLogicOperation {
 public:
    LeOperation(GenObject *a, GenObject *b, const char *comment)
        : TwoStandardLogicOperation(a, b, comment) {}

    virtual std::string getOperand() override { return std::string(" <= "); }
    virtual uint64_t getValue() override { return a_->getValue() <= b_->getValue() ? 1: 0; }
};

/**
    a == 0 ? 1 : 0
 */
class EzOperation : public ReduceOperation {
 public:
    EzOperation(GenObject *a, const char *comment)
        : ReduceOperation(a, comment) {}

    virtual uint64_t getValue() override { return a_->getValue() == 0 ? 1: 0; }
    virtual std::string getStrValue() override;
};

/**
    a != 0 ? 1 : 0
 */
class NzOperation : public ReduceOperation {
 public:
    NzOperation(GenObject *a, const char *comment)
        : ReduceOperation(a, comment) {}

    virtual uint64_t getValue() override { return a_->getValue() ? 1: 0; }
    virtual std::string getStrValue() override;
};

/**
    Inversion logical or arithemical:
        ~a (!a)
 */
class InvOperation : public Operation {
 public:
    InvOperation(GenObject *a, bool logical, const char *comment)
        : Operation(NO_PARENT, comment), a_(a), logical_(logical) {}

    virtual std::string getStrValue() override { return generate(); }
    virtual uint64_t getWidth() override { return a_->getWidth(); }
    virtual std::string generate() override;

 protected:
    GenObject *a_;
    bool logical_;    // affect only systemc
};

/**
    a + b
*/
class Add2Operation : public TwoStandardOperandsOperation {
 public:
    Add2Operation(GenObject *a, GenObject *b, const char *comment)
        : TwoStandardOperandsOperation(a, b, comment) {}

    virtual std::string getOperand() override { return std::string(" + "); }
    virtual uint64_t getValue() { return a_->getValue() + b_->getValue(); }
};

/**
    a - b
*/
class Sub2Operation : public TwoStandardOperandsOperation {
 public:
    Sub2Operation(GenObject *a, GenObject *b, const char *comment)
        : TwoStandardOperandsOperation(a, b, comment) {}

    virtual std::string getOperand() override { return std::string(" - "); }
    virtual uint64_t getValue() { return a_->getValue() - b_->getValue(); }
};

/**
    a * b
*/
class Mul2Operation : public TwoStandardOperandsOperation {
 public:
    Mul2Operation(GenObject *a, GenObject *b, const char *comment)
        : TwoStandardOperandsOperation(a, b, comment) {}

    virtual std::string getOperand() override { return std::string(" * "); }
    virtual uint64_t getValue() { return a_->getValue() * b_->getValue(); }
};

/**
    a / b
*/
class Div2Operation : public TwoStandardOperandsOperation {
 public:
    Div2Operation(GenObject *a, GenObject *b, const char *comment)
        : TwoStandardOperandsOperation(a, b, comment) {}

    virtual std::string getOperand() override { return std::string(" / "); }
    virtual uint64_t getValue() { return a_->getValue() / b_->getValue(); }
};

/**
    Logical AND: (a & b), (a && b)
*/
class And2Operation : public TwoStandardOperandsOperation {
 public:
    And2Operation(GenObject *a, GenObject *b, bool logical, const char *comment)
        : TwoStandardOperandsOperation(a, b, comment), logical_(logical) {}

    virtual std::string getOperand() override;
    virtual uint64_t getValue() { return a_->getValue() & b_->getValue(); }
 protected:
    bool logical_;
};

/**
    Logical OR2: (a | b), (a || b)
*/
class Or2Operation : public TwoStandardOperandsOperation {
 public:
    Or2Operation(GenObject *a, GenObject *b, bool logical, const char *comment)
        : TwoStandardOperandsOperation(a, b, comment), logical_(logical) {}

    virtual std::string getOperand() override;
    virtual uint64_t getValue() { return a_->getValue() | b_->getValue(); }
 protected:
    bool logical_;
};

/**
    Logical XOR2: (a ^ b)
*/
class Xor2Operation : public TwoStandardOperandsOperation {
 public:
    Xor2Operation(GenObject *a, GenObject *b, const char *comment)
        : TwoStandardOperandsOperation(a, b, comment) {}

    virtual std::string getOperand() override;
    virtual uint64_t getValue() { return a_->getValue() ^ b_->getValue(); }
};

/**
    (&a)
 */
class AndReduceOperation : public ReduceOperation {
 public:
    AndReduceOperation(GenObject *a, const char *comment)
        : ReduceOperation(a, comment) {}

    virtual uint64_t getValue() { return 0; }  // todo: a == (mask-1)
    virtual std::string getStrValue() override;
};

/**
    (|a)
 */
class OrReduceOperation : public ReduceOperation {
 public:
    OrReduceOperation(GenObject *a, const char *comment)
        : ReduceOperation(a, comment) {}

    virtual uint64_t getValue() { return a_->getValue() ? 1: 0; }
    virtual std::string getStrValue() override;
};

/**
    (a & b & c); (a && b && c)
 */
class And3Operation : public NStandardOperandsOperation {
 public:
    And3Operation(GenObject *a, GenObject *b, GenObject *c, bool logical, const char *comment)
        : NStandardOperandsOperation(true, comment), logical_(logical) {
        add_entry(a);
        add_entry(b);
        add_entry(c);
    }

    virtual std::string getOperand() override;
    virtual uint64_t getValue() override { return 0; }
 protected:
    bool logical_;
};

/**
    (a | b | c); (a || b || c)
 */
class Or3Operation : public NStandardOperandsOperation {
 public:
    Or3Operation(GenObject *a, GenObject *b, GenObject *c, bool logical, const char *comment)
        : NStandardOperandsOperation(true, comment), logical_(logical) {
        add_entry(a);
        add_entry(b);
        add_entry(c);
    }

    virtual std::string getOperand() override;
    virtual uint64_t getValue() override { return 0; }
 protected:
    bool logical_;
};

/**
    (a && b && c && d)
 */
class And4Operation : public NStandardOperandsOperation {
 public:
    And4Operation(GenObject *a, GenObject *b, GenObject *c, GenObject *d, const char *comment)
        : NStandardOperandsOperation(true, comment) {
        add_entry(a);
        add_entry(b);
        add_entry(c);
        add_entry(d);
    }

    virtual std::string getOperand() override;
    virtual uint64_t getValue() override { return 0; }
};

/**
    (a || b || c || d)
 */
class Or4Operation : public NStandardOperandsOperation {
 public:
    Or4Operation(GenObject *a, GenObject *b, GenObject *c, GenObject *d, const char *comment)
        : NStandardOperandsOperation(true, comment) {
        add_entry(a);
        add_entry(b);
        add_entry(c);
        add_entry(d);
    }

    virtual std::string getOperand() override;
    virtual uint64_t getValue() override { return 0; }
};

/**
    Sum of elements
 */
class ADDxOperation : public NStandardOperandsOperation {
 public:
    ADDxOperation(const char *comment)
        : NStandardOperandsOperation(false, comment) {}

    virtual std::string getOperand() override { return std::string("+"); }
    virtual uint64_t getValue() { return 0; }
};

/**
    AND of elements
 */
class ANDxOperation : public NStandardOperandsOperation {
 public:
    ANDxOperation(bool logical, const char *comment)
        : NStandardOperandsOperation(false, comment), logical_(logical) {}

    virtual std::string getOperand() override;
    virtual uint64_t getValue() { return 0; }
 protected:
    bool logical_;
};

/**
    OR of elements
 */
class ORxOperation : public NStandardOperandsOperation {
 public:
    ORxOperation(bool logical, const char *comment)
        : NStandardOperandsOperation(false, comment), logical_(logical) {}

    virtual std::string getOperand() override;
    virtual uint64_t getValue() { return 0; }
 protected:
    bool logical_;
};

/**
    XOR of elements
 */
class XORxOperation : public NStandardOperandsOperation {
 public:
    XORxOperation(const char *comment)
        : NStandardOperandsOperation(false, comment) {}

    virtual std::string getOperand() override;
    virtual uint64_t getValue() { return 0; }
};

/**
    Concatation operation
 */
class CCxOperation : public NStandardOperandsOperation {
 public:
    CCxOperation(bool oneline, const char *comment)
        : NStandardOperandsOperation(oneline, comment) {}

    virtual std::string getOpeningBrace() override;
    virtual std::string getClosingBrace() override;
    virtual bool isOperandNewLine() override { return false; }
    virtual std::string getOperand() override;
    virtual uint64_t getValue() { return 0; }
    virtual std::string getStrValue() override;
    virtual uint64_t getWidth() override;
};

/**
    Split bus on signals
 */
class SplitOperation : public Operation {
 public:
    SplitOperation(GenObject *a, const char *comment)
        : Operation(top_obj(), comment), a_(a) {}

    virtual std::string getStrValue() override { return std::string(""); }
    virtual std::string generate() override;

 protected:
    GenObject *a_;
};

/**
    Calculate width of the bus using buswidth of arguments
 */
class CalcWidthOperation : public NStandardOperandsOperation {
 public:
    CalcWidthOperation(const char *comment)
        : NStandardOperandsOperation(false, comment), objWdith_(32) {}

    virtual std::string getOperand() { return std::string(" +"); }
    virtual std::string getType() override { return objWdith_.getType(); }
    virtual uint64_t getValue() override;
    virtual std::string getStrValue() override;
    virtual uint64_t getWidth() override { return objWdith_.getWidth(); }
    virtual std::string getStrWidth() override { return objWdith_.getStrWidth(); }
    virtual std::string generate() override { return getStrValue(); }

 protected:
    DecConst objWdith_;
};

/**
    Left shift operation
 */
class LshOperation : public Operation {
 public:
    LshOperation(GenObject *a, GenObject *sz, const char *comment)
        : Operation(NO_PARENT, comment), a_(a), sz_(sz) {}

    virtual uint64_t getValue() override { return a_->getValue() << sz_->getValue(); }
    virtual std::string getStrValue() override;
    virtual uint64_t getWidth() override { return a_->getWidth(); }
    virtual std::string generate() override { return getStrValue(); }

 protected:
    GenObject *a_;
    GenObject *sz_;
};

/**
    Power of 2, special case of LSH
 */
class Pow2Operation : public LshOperation {
 public:
    Pow2Operation(GenObject *sz, const char *comment)
        : LshOperation(&a1_, sz, comment), a1_(1) {}

    virtual std::string getStrValue() override;
 protected:
    DecConst a1_;
};


/**
    Right shift operation
 */
class RshOperation : public Operation {
 public:
    RshOperation(GenObject *a, GenObject *sz, const char *comment)
        : Operation(NO_PARENT, comment), a_(a), sz_(sz) {}

    virtual uint64_t getValue() override { return a_->getValue() >> sz_->getValue(); }
    virtual std::string getStrValue() override { return generate(); }
    virtual uint64_t getWidth() override { return a_->getWidth(); }
    virtual std::string generate() override;

 protected:
    GenObject *a_;
    GenObject *sz_;
};

class ArrItemOperation : public Operation {
 public:
    ArrItemOperation(GenObject *arr, GenObject *idx, GenObject *item,
        bool force_read, const char *comment)
        : Operation(NO_PARENT, comment), arr_(arr), idx_(idx),
        item_(item), force_read_(force_read) {
        if (arr_ == 0) {
            SHOW_ERROR("%s", "Array not defined");
        } else if (item_ == 0) {
            SHOW_ERROR("%s", "Array item not defined");
        }
    }

    virtual std::string getName() override { return ""; }
    virtual std::string generate() override;
 protected:
    GenObject *arr_;
    GenObject *idx_;
    GenObject *item_;
    bool force_read_;
};

Operation &ARRITEM(GenObject &arr, GenObject &idx, GenObject &item, const char *comment="");
Operation &ARRITEM(GenObject &arr, int idx, GenObject &item, const char *comment="");
Operation &ARRITEM(GenObject &arr, int idx);
Operation &ARRITEM_B(GenObject &arr, GenObject &idx, GenObject &item, const char *comment="");  // .read() for signals and ports in bits operations

class IfOtherwiseOperation : public Operation {
 public:
    IfOtherwiseOperation(GenObject *cond, GenObject *a, GenObject *b, const char *comment)
        : Operation(NO_PARENT, comment), cond_(cond), a_(a), b_(b) {
    }

    virtual std::string getName() override { return ""; }
    virtual std::string generate() override;
 protected:
    GenObject *cond_;
    GenObject *a_;
    GenObject *b_;
};

Operation &IF_OTHERWISE(GenObject &cond, GenObject &a, GenObject &b, const char *comment="");

class IfOperation : public Operation {
 public:
    IfOperation(GenObject *a, StringConst *genname, const char *comment)
        : Operation(comment), a_(a), genname_(genname) {
        push_obj(this);
    }

    virtual std::string getName() override { return ""; }
    virtual std::string generate() override;
 protected:
    GenObject *a_;
    StringConst *genname_;
};

class ElseIfOperation : public Operation {
 public:
    ElseIfOperation(GenObject *a, StringConst *genname, const char *comment)
        : Operation(comment), a_(a), genname_(genname) {}

    virtual std::string generate() override;
 protected:
    GenObject *a_;
    StringConst *genname_;
};

class ElseOperation : public Operation {
 public:
    ElseOperation(StringConst *genname, const char *comment)
        : Operation(comment), genname_(genname) {}

    virtual std::string generate() override;
 protected:
    StringConst *genname_;
};

class EndIfOperation : public Operation {
 public:
    EndIfOperation(StringConst *genname, const char *comment)
        : Operation(comment), genname_(genname) {
        pop_obj();
    }

    virtual std::string generate() override;
 protected:
    StringConst *genname_;
};

Operation &IF(GenObject &a, const char *comment = NO_COMMENT);
void ELSIF(GenObject &a, const char *comment = NO_COMMENT);
void ELSE(const char *comment = NO_COMMENT);
void ENDIF(const char *comment = NO_COMMENT);
// RTL specific if condition with names
Operation &IFGEN(GenObject &a, StringConst *name, const char *comment = NO_COMMENT);
void ELSEGEN(StringConst *name, const char *comment = NO_COMMENT);
void ENDIFGEN(StringConst *name, const char *comment = NO_COMMENT);

class SwitchOperation : public Operation {
 public:
    SwitchOperation(GenObject *a, const char *comment)
        : Operation(comment), a_(a) {
        push_obj(this);
    }

    virtual std::string getName() override { return ""; }
    virtual std::string generate() override;
 protected:
    GenObject *a_;
};

class CaseOperation : public Operation {
 public:
    CaseOperation(GenObject *a, const char *comment)
        : Operation(comment), a_(a) {}
    virtual std::string generate() override;
 protected:
    GenObject *a_;
};

class EndCaseOperation : public Operation {
 public:
    EndCaseOperation(const char *comment) : Operation(comment) {
    }
    virtual std::string generate() override;
};

class EndSwitchOperation : public Operation {
 public:
    EndSwitchOperation(const char *comment) : Operation(comment) {
        pop_obj();
    }
    virtual std::string generate() override;
};

Operation &SWITCH(GenObject &a, const char *comment="");
void CASE(GenObject &a, const char *comment="");
void CASEDEF(const char *comment="");
void ENDCASE(const char *comment="");
void ENDSWITCH(const char *comment="");

/**
    for () operation
 */
class ForOperation : public Operation {
 public:
    ForOperation(GenObject *parent,
                 GenObject *i,
                 GenObject *start,
                 GenObject *end,
                 StringConst *dir,
                 StringConst *genname, // if not zero then 'generate'
                 const char *comment)
        : Operation(parent, comment),
        i_(i), start_(start), end_(end), dir_(dir), genname_(genname) {
            Operation::push_obj(this);  // for becomes top_obj.
            i_ = Operation::push_for(i);
        }

    virtual std::string getName() override { return ""; }
    virtual std::string generate() override;

 protected:
    GenObject *i_;
    GenObject *start_;
    GenObject *end_;
    StringConst *dir_;
    StringConst *genname_;
};

/**
    end of for() cycle operation
 */
class EndForOperation : public Operation {
 public:
    EndForOperation(StringConst *genname, // if not zero then 'generate'
                    const char *comment)
        : Operation(comment), genname_(genname) {
            Operation::pop_obj();
            Operation::pop_for();
        }

    virtual std::string generate() override;

 protected:
    StringConst *genname_;
};

GenObject &FOR_INC(GenObject &end,
                   const char *comment=NO_COMMENT);

GenObject &FOR(const char *i,
               GenObject &start,
               GenObject &end,
               const char *dir,
               const char *comment=NO_COMMENT);

GenObject &FORGEN(const char *i,
                  GenObject &start,
                  GenObject &end,
                  const char *dir,
                  StringConst *name,
                  const char *comment=NO_COMMENT);

void ENDFOR(const char *comment="");
void ENDFORGEN(StringConst *name, const char *comment="");

class WhileOperation : public Operation {
 public:
    WhileOperation(GenObject *a, const char *comment)
        : Operation(comment), a_(a) {
        push_obj(this);
    }

    virtual std::string getName() override { return ""; }
    virtual std::string generate() override;
 protected:
    GenObject *a_;
};

class EndWhileOperation : public Operation {
 public:
    EndWhileOperation(const char *comment) : Operation(comment) {
        pop_obj();
    }
    virtual std::string generate() override;
};

Operation &WHILE(GenObject &a, const char *comment = NO_COMMENT);
void ENDWHILE(const char *comment = NO_COMMENT);

// xrst is an additional reset signal
/**
    Sync reset operation (only inside of process)
*/
class SyncResetOperation : public Operation {
 public:
    SyncResetOperation(GenObject *proc, GenObject *xrst)
        : Operation(NO_COMMENT), proc_(proc), xrst_(xrst) {
    }
    virtual std::string getName() override { return ""; }
    virtual std::string generate() override;
 protected:
    GenObject *proc_;
    GenObject *xrst_;
};

void SYNC_RESET(GenObject *xrst = 0);

// call function
class CallFuncOperation : public Operation {
 public:
    CallFuncOperation(GenObject *ret, GenObject *a)
        : Operation(NO_COMMENT), ret_(ret), a_(a), cnt_(0) {
    }
    virtual std::string getName() override { return ""; }
    virtual std::string generate() override;
    virtual void addArgument(GenObject *arg) {
        args_[cnt_++] = arg;
    }
 protected:
    GenObject *ret_;
    GenObject *a_;
    int cnt_;
    GenObject *args_[256];
};

Operation &CALLF(GenObject *ret, GenObject &a, size_t argcnt, ...);

// write string into file
class FileOpenOperation : public Operation {
 public:
    FileOpenOperation(GenObject *fname, GenObject *str, const char *comment)
        : Operation(comment), fname_(fname), str_(str) {
    }
    virtual std::string getName() override { return ""; }
    virtual std::string generate() override;
 protected:
    GenObject *fname_;
    GenObject *str_;
};

Operation &FOPEN(GenObject &f, GenObject &str);

class FileWriteOperation : public Operation {
 public:
    FileWriteOperation(GenObject *f, GenObject *str, bool byte, const char *comment)
        : Operation(comment), f_(f), str_(str), byte_(byte) {
    }
    virtual std::string getName() override { return ""; }
    virtual std::string generate() override;
 protected:
    GenObject *f_;
    GenObject *str_;
    bool byte_;
};

Operation &FWRITE(GenObject &f, GenObject &str);
Operation &FWRITECHAR(GenObject &f, GenObject &str);

class FileFlushOperation : public Operation {
 public:
    FileFlushOperation(GenObject *f, const char *comment)
        : Operation(comment), f_(f) {
    }
    virtual std::string getName() override { return ""; }
    virtual std::string generate() override;
 protected:
    GenObject *f_;
};

Operation &FFLUSH(GenObject &f);

class ReadMemHexOperation : public Operation {
 public:
    ReadMemHexOperation(GenObject *f, GenObject *mem, const char *comment)
        : Operation(comment), f_(f), mem_(mem) {
    }
    virtual std::string getName() override { return ""; }
    virtual std::string generate() override;
 protected:
    GenObject *f_;
    GenObject *mem_;
};

Operation &READMEMH(GenObject &fname, GenObject &mem);

class DisplayStrOperation : public Operation {
 public:
    DisplayStrOperation(GenObject *str, const char *comment)
        : Operation(comment), str_(str) {
    }
    virtual std::string getName() override { return ""; }
    virtual std::string generate() override;
 protected:
    GenObject *str_;
};

Operation &DISPLAYSTR(GenObject &str);

/**
    Create new module instance
*/
class NewOperation : public Operation {
 public:
    NewOperation(GenObject &m, const char *name, GenObject *idx, const char *comment)
        : Operation(comment), m_(dynamic_cast<ModuleObject *>(&m)), idx_(idx) {
        instname_ = std::string(name);
        push_obj(this);
    }
    virtual std::string getName() override { return ""; }
    virtual std::string generate() override;

    virtual void add_connection(std::string port, GenObject *arg) {
        connection_[port] = arg;
    }
    virtual std::string gen_connection(std::string port) {
        std::string ret = "";
        if (connection_.find(port) != connection_.end()) {
            ret = connection_[port]->generate();
        } else {
            SHOW_ERROR("Port %s not found", port.c_str());
        }
        return ret;
    }

 private:
    std::string generate_sc();
    std::string generate_sv();
    std::string generate_vhdl();
 protected:
    ModuleObject *m_;       // Module class
    GenObject *idx_;        // Module index (optional)
    std::string instname_;  // instance name
    std::map<std::string, GenObject *> connection_;
};

/**
    Connect module instance port. Generated as a childs of NEW operation:
*/
class ConnectOperation : public Operation {
 public:
    ConnectOperation(GenObject &m, GenObject *idx, GenObject &port, GenObject &s, const char *comment)
        : Operation(comment), m_(dynamic_cast<ModuleObject *>(&m)), idx_(idx), port_(&port), s_(&s) {
        dynamic_cast<NewOperation *>(getParent())->add_connection(port.getName(), this);
    }
    virtual std::string getName() override { return ""; }
    virtual std::string generate() override;

 protected:
    ModuleObject *m_;       // Module class
    GenObject *idx_;        // Module index (optional)
    GenObject *port_;       // Port to connect
    GenObject *s_;          // Connection signal
};

/**
    End of module connection process. Parent assigned after poping:
*/
class EndNewOperation : public Operation {
 public:
    EndNewOperation(const char *comment) : Operation(comment) {
        pop_obj();
    }
    virtual std::string getName() override { return ""; }
    virtual std::string generate() override { return std::string(""); }
};

// RTL specific not used in SystemC
class DeclareTStrOperation : public Operation {
 public:
    DeclareTStrOperation(const char *comment) : Operation(comment) {
    }
    virtual std::string getName() override { return ""; }
    virtual std::string generate() override;
};

void DECLARE_TSTR();    // declare temporary string buffer

class InitialOperation : public Operation {
 public:
    InitialOperation(const char *comment) : Operation(comment) {
        push_obj(this);
    }
    virtual std::string getName() override { return ""; }
    virtual std::string generate() override;
};

Operation &INITIAL();

class EndInitialOperation : public Operation {
 public:
    EndInitialOperation(const char *comment) : Operation(comment) {
        pop_obj();
    }
    virtual std::string getName() override { return ""; }
    virtual std::string generate() override;
};

void ENDINITIAL();

/**
    Generate block:
*/
class GenerateOperation : public Operation {
 public:
    GenerateOperation(GenObject *name, const char *comment)
        : Operation(comment), name_(name) {
        push_obj(this);
    }
    virtual std::string getName() override { return ""; }
    virtual std::string generate() override;
 protected:
    GenObject *name_;
};

/**
    End of generation block
 */
class EndGenerateOperation : public Operation {
 public:
    EndGenerateOperation(GenObject *name, const char *comment)
        : Operation(comment), name_(name) {
        pop_obj();
    }
    virtual std::string getName() override { return ""; }
    virtual std::string generate() override;
 protected:
    GenObject *name_;
};

GenObject &GENERATE(const char *name, const char *comment="");
void ENDGENERATE(const char *name, const char *comment="");

/**
    posedge <>/ negedge <> events
 */
class EdgeOperation : public Operation {
 public:
    EdgeOperation(GenObject *obj, EClockEdge edge)
        : Operation(NO_PARENT, NO_COMMENT), obj_(obj), edge_(edge) {
    }
    virtual std::string getName() override { return ""; }
    virtual std::string generate() override;
 protected:
    GenObject *obj_;
    EClockEdge edge_;
};

Operation &EDGE(GenObject &obj, EClockEdge edge);
Operation &EDGE(GenObject &obj, EResetActive edge);

/**
    always, always_comb and always_ff blocks:
*/
class AlwaysOperation : public Operation {
 public:
    AlwaysOperation(GenObject *cond, GenObject *clk, GenObject *rst, const char *comment)
        : Operation(comment), cond_(cond), clk_(clk), rst_(rst) {
        push_obj(this);
    }
    virtual std::string getName() override { return ""; }
    virtual std::string generate() override;
 protected:
    GenObject *cond_;
    GenObject *clk_;
    GenObject *rst_;
};

/**
    End of always block
 */
class EndAlwaysOperation : public Operation {
 public:
    EndAlwaysOperation(const char *comment)
        : Operation(comment) {
        pop_obj();
    }
    virtual std::string getName() override { return ""; }
    virtual std::string generate() override;
};

GenObject &ALWAYS(GenObject *cond, const char *comment=NO_COMMENT);
GenObject &ALWAYS_FF(GenObject &clk, const char *comment=NO_COMMENT);
GenObject &ALWAYS_FF(GenObject &clk, GenObject &rst, const char *comment=NO_COMMENT);
void ENDALWAYS(const char *comment=NO_COMMENT);

}  // namespace sysvc
