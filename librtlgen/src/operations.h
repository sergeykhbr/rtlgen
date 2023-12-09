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

typedef std::string (*generate_type)(GenObject **args);

class Operation : public GenObject {
 public:
    Operation(const char *comment="");
    Operation(GenObject *parent, const char *comment="");

    virtual bool isOperation() override { return true; }

    static void start(GenObject *owner);
    static void push_obj(GenObject *obj);
    static void pop_obj();
    static GenObject *top_obj();
    std::string v_name(std::string v) override;
    std::string r_name(std::string v) override { return v_name(v); }
    static std::string obj2varname(GenObject *obj, const char *prefix="r", bool read=false);
    static std::string fullname(const char *prefix, std::string name, GenObject *obj);
    static std::string addtext(GenObject *obj, size_t curpos);
    // Copy signals marked as 'reg' from v to r or vise versa
    static std::string copyreg_entry(char *idx, std::string dst, std::string src, GenObject *obj);
    static std::string copyreg(const char *dst, const char *src, ModuleObject *m);
    // Reset signals marked as 'reg'
    static std::string reset(const char *dst, const char *src, ModuleObject *m, std::string xrst);

    virtual void add_arg(GenObject *arg) {
        args[argcnt_++] = arg;
    }
    virtual bool isGen(generate_type t) { return t == igen_; }
    virtual GenObject *getArg(int cnt) { return args[cnt]; }
    virtual std::string getName() override { return getStrValue(); }
    virtual std::string getStrValue() override { return generate(); }
    virtual std::string generate() override {
        std::string ret = igen_(args);
        for (auto &e: getEntries()) {
            if (e->isOperation()) {
                ret += e->generate();
            }
        }
        return ret;
    }

    generate_type igen_;
 protected:
    GenObject *args[256];
    int argcnt_;
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
    Generate commenting string:
        // text
 */
class TextOperation : public Operation {
 public:
    TextOperation(const char *comment) : Operation(top_obj(), comment) {}
    virtual std::string generate() override;
};

/**
    Assign constant value to all bits. No parent.
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
    Set constant value int variable:
        a = const
 */
class SetConstOperation : public Operation {
 public:
    SetConstOperation(GenObject &a, uint64_t v, const char *comment);

    virtual std::string generate() override;

 protected:
    GenObject *a_;
    GenObject *b_;
};

Operation &SETBIT(GenObject &a, GenObject &b, GenObject &val, const char *comment="");
Operation &SETBIT(GenObject &a, int b, GenObject &val, const char *comment="");
Operation &SETBITONE(GenObject &a, GenObject &b, const char *comment="");
Operation &SETBITONE(GenObject &a, const char *b, const char *comment="");
Operation &SETBITONE(GenObject &a, int b, const char *comment="");
Operation &SETBITZERO(GenObject &a, GenObject &b, const char *comment="");
Operation &SETBITZERO(GenObject &a, const char *b, const char *comment="");
Operation &SETBITZERO(GenObject &a, int b, const char *comment="");
Operation &SETBITS(GenObject &a, GenObject &h, GenObject &l, GenObject &val, const char *comment="");
Operation &SETBITS(GenObject &a, int h, int l, GenObject &val, const char *comment="");
Operation &SETBITSW(GenObject &a, GenObject &start, GenObject &width, GenObject &val, const char *comment="");
Operation &SETVAL(GenObject &a, GenObject &b, const char *comment="");
Operation &SETZ(GenObject &a, const char *comment="");
Operation &SETSTR(GenObject &a, const char *str, const char *comment="");
Operation &SETSTRF(GenObject &a, const char *fmt, size_t cnt, ...);
Operation &ADDSTRF(GenObject &a, const char *fmt, size_t cnt, ...);
Operation &ADDSTRU8(GenObject &strout, GenObject &strin, GenObject &val);
Operation &TO_INT(GenObject &a, const char *comment="");
Operation &TO_U32(GenObject &a, const char *comment="");
Operation &TO_U64(GenObject &a, const char *comment="");
Operation &TO_CSTR(GenObject &a, const char *comment="");
Operation &BIG_TO_U64(GenObject &a, const char *comment="");        // explicit conersion of biguint to uint64 (sysc only)

/**
    Convet to biguint<> (effect in systemc only):
 */
class ToBigOperation : public Operation {
 public:
    ToBigOperation(GenObject *a, size_t sz, const char *comment)
        : Operation(NO_PARENT, comment), a_(a) {
        objWidth_ = new DecConst(static_cast<int>(sz));
    }

    virtual std::string getStrValue() override { return generate(); }
    virtual uint64_t getWidth() override { return objWidth_->getValue(); }
    virtual std::string generate() override;

 protected:
    GenObject *a_;
    GenObject *objWidth_;
};

/**
    a > b ? 1 : 0
 */
class GtOperation : public TwoStandardLogicOperation {
 public:
    GtOperation(GenObject *a, GenObject *b, const char *comment)
        : TwoStandardLogicOperation(a, b, comment) {}

    virtual std::string getOperand() { return std::string(" > "); }
    virtual uint64_t getValue() { return a_->getValue() > b_->getValue() ? 1: 0; }
};

Operation &EQ(GenObject &a, GenObject &b, const char *comment="");  // ==
Operation &NE(GenObject &a, GenObject &b, const char *comment="");  // !=
Operation &EZ(GenObject &a, const char *comment="");        // equal-zero
Operation &NZ(GenObject &a, const char *comment="");        // Non-zero
//Operation &GT(GenObject &a, GenObject &b, const char *comment="");        // Greater (>)
Operation &GE(GenObject &a, GenObject &b, const char *comment="");        // Greater-Equal (>=)
Operation &LS(GenObject &a, GenObject &b, const char *comment="");        // Less (<)
Operation &LE(GenObject &a, GenObject &b, const char *comment="");        // Less-Equal (<=)

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


Operation &OR2_L(GenObject &a, GenObject &b, const char *comment="");   // bitwise OR
Operation &OR2(GenObject &a, GenObject &b, const char *comment="");
Operation &OR3(GenObject &a, GenObject &b, GenObject &c, const char *comment="");
Operation &OR4(GenObject &a, GenObject &b, GenObject &c, GenObject &d, const char *comment="");
Operation &ORx(size_t cnt, ...);
Operation &ORx_L(size_t cnt, ...);

class AndReduceOperation : public ReduceOperation {
 public:
    AndReduceOperation(GenObject *a, const char *comment)
        : ReduceOperation(a, comment) {}

    virtual uint64_t getValue() { return 0; }  // todo: a == (mask-1)
    virtual std::string getStrValue() override;
};

class OrReduceOperation : public ReduceOperation {
 public:
    OrReduceOperation(GenObject *a, const char *comment)
        : ReduceOperation(a, comment) {}

    virtual uint64_t getValue() { return a_->getValue() ? 1: 0; }
    virtual std::string getStrValue() override;
};

Operation &XOR2(GenObject &a, GenObject &b, const char *comment="");
Operation &XORx(size_t cnt, ...);
Operation &AND3_L(GenObject &a, GenObject &b, GenObject &c, const char *comment="");
Operation &ADDx(size_t cnt, ...);
Operation &AND3(GenObject &a, GenObject &b, GenObject &c, const char *comment="");
Operation &AND4(GenObject &a, GenObject &b, GenObject &c, GenObject &d, const char *comment="");
Operation &ANDx(size_t cnt, ...);
Operation &ANDx_L(size_t cnt, ...);
Operation &INCVAL(GenObject &res, GenObject &inc, const char *comment="");

/**
    Concatation operation
 */
class CCxOperation : public NStandardOperandsOperation {
 public:
    CCxOperation(bool oneline, const char *comment)
        : NStandardOperandsOperation(oneline, comment) {}

    virtual std::string getOpeningBrace() override;
    virtual std::string getClosingBrace() override;
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
    virtual std::string getStrValue() override { return generate(); }
    virtual uint64_t getWidth() override { return a_->getWidth(); }
    virtual std::string generate() override;

 protected:
    GenObject *a_;
    GenObject *sz_;
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

Operation &ARRITEM(GenObject &arr, GenObject &idx, GenObject &item, const char *comment="");
Operation &ARRITEM(GenObject &arr, int idx, GenObject &item, const char *comment="");
Operation &ARRITEM(GenObject &arr, int idx);
Operation &ARRITEM_B(GenObject &arr, GenObject &idx, GenObject &item, const char *comment="");  // .read() for signals and ports in bits operations
Operation &SETARRITEM(GenObject &arr, GenObject &idx, GenObject &item, GenObject &val, const char *comment="");
Operation &ASSIGNARRITEM(GenObject &arr, GenObject &idx, GenObject &item, GenObject &val, const char *comment="");
Operation &SETARRITEM(GenObject &arr, int idx, GenObject &val);
Operation &SETARRITEMBIT(GenObject &arr, GenObject &idx, GenObject &item, 
                           GenObject &bitidx, GenObject &val, const char *comment="");
Operation &SETARRITEMBITSW(GenObject &arr, GenObject &idx, GenObject &item,
                           GenObject &start, GenObject &width, GenObject &val, const char *comment="");
Operation &ASSIGNARRITEM(GenObject &arr, int idx, GenObject &val);
Operation &SETARRIDX(GenObject &arr, GenObject &idx);
Operation &IF_OTHERWISE(GenObject &cond, GenObject &a, GenObject &b, const char *comment="");

void IF(GenObject &a, const char *comment="");
void ELSIF(GenObject &a, const char *comment="");
void ELSE(const char *comment="");
void ENDIF(const char *comment="");
// RTL specific if condition with names
void IFGEN(GenObject &a, STRING *name, const char *comment="");
void ELSEGEN(STRING *name, const char *comment="");
void ENDIFGEN(STRING *name, const char *comment="");

void SWITCH(GenObject &a, const char *comment="");
void CASE(GenObject &a, const char *comment="");
void CASEDEF(const char *comment="");
void ENDCASE(const char *comment="");
void ENDSWITCH(const char *comment="");

//GenObject &GENVAR(const char *name);
GenObject &FOR(const char *i, GenObject &start, GenObject &end, const char *dir, const char *comment="");
GenObject &FORGEN(const char *i, GenObject &start, GenObject &end, const char *dir, STRING *name, const char *comment="");
void ENDFOR(const char *comment="");
void ENDFORGEN(STRING *name, const char *comment="");

void WHILE(GenObject &a, const char *comment="");
void ENDWHILE(const char *comment="");

// xrst is an additional reset signal
void SYNC_RESET(GenObject &a, GenObject *xrst=0, const char *comment="");
// call function
void CALLF(GenObject *ret, GenObject &a, size_t argcnt, ...);
// write string into file
void FOPEN(GenObject &f, GenObject &str);
void FWRITE(GenObject &f, GenObject &str);
void FWRITECHAR(GenObject &f, GenObject &str);
void FFLUSH(GenObject &f);
void READMEMH(GenObject &fname, GenObject &mem);
void DISPLAYSTR(GenObject &str);

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
    EndNewOperation(const char *comment) : Operation(0, comment) {
        pop_obj();
        parent_ = top_obj();
        if (parent_) {
            parent_->add_entry(this);
        }
    }
    virtual std::string generate() override { return std::string(""); }
};

// RTL specific not used in SystemC
void DECLARE_TSTR();    // declare temporary string buffer
void INITIAL();
void ENDINITIAL();
void GENERATE(const char *name, const char *comment="");
void ENDGENERATE(const char *name, const char *comment="");

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
    virtual std::string generate() override;
 protected:
    GenObject *a_;
    GenObject *b_;
};

}  // namespace sysvc
