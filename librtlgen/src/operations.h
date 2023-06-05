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

    static void start(GenObject *owner);
    static void push_obj(GenObject *obj);
    static void pop_obj();
    static void set_space(int n);
    static int get_space();
    static std::string addspaces();
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
    virtual bool isGen(generate_type t) { return t == igen_; }
    virtual GenObject *getArg(int cnt) { return args[cnt]; }
    virtual std::string getType() { return std::string(""); }
    virtual std::string generate() override {
        std::string ret = igen_(args);
        for (auto &e: entries_) {
            if (e->getId() != ID_OPERATION) {
                continue;
            }
            ret += e->generate();
        }
        return ret;
    }

    generate_type igen_;
 protected:
    GenObject *args[256];
    int argcnt_;
    std::map<std::string, GenObject *> connection_;
};

void TEXT(const char *comment="");
Operation &ALLZEROS(const char *comment="");
Operation &ALLONES(const char *comment="");
Operation &SETZERO(GenObject &a, const char *comment="");
Operation &SETONE(GenObject &a, const char *comment="");

Operation &BIT(GenObject &a, GenObject &b, const char *comment="");
Operation &BIT(GenObject &a, const char *b, const char *comment="");
Operation &BIT(GenObject &a, int b, const char *comment="");
Operation &BITS(GenObject &a, GenObject &h, GenObject &l, const char *comment="");
Operation &BITS(GenObject &a, int h, int l, const char *comment="");
Operation &BITSW(GenObject &a, GenObject &start, GenObject &width, const char *comment="");
GenObject &CONST(const char *val);
GenObject &CONST(const char *val, const char *width);
GenObject &CONST(const char *val, int width);
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
Operation &SETSTR(GenObject &a, const char *str, const char *comment="");
Operation &SETSTRF(GenObject &a, const char *fmt, size_t cnt, ...);
Operation &ADDSTRF(GenObject &a, const char *fmt, size_t cnt, ...);
Operation &TO_INT(GenObject &a, const char *comment="");
Operation &TO_U32(GenObject &a, const char *comment="");
Operation &TO_U64(GenObject &a, const char *comment="");
Operation &TO_CSTR(GenObject &a, const char *comment="");
Operation &BIG_TO_U64(GenObject &a, const char *comment="");        // explicit conersion of biguint to uint64 (sysc only)
Operation &TO_BIG(size_t sz, GenObject &a); // convert to biguint
Operation &EQ(GenObject &a, GenObject &b, const char *comment="");  // ==
Operation &NE(GenObject &a, GenObject &b, const char *comment="");  // !=
Operation &EZ(GenObject &a, const char *comment="");        // equal-zero
Operation &NZ(GenObject &a, const char *comment="");        // Non-zero
Operation &GT(GenObject &a, GenObject &b, const char *comment="");        // Greater (>)
Operation &GE(GenObject &a, GenObject &b, const char *comment="");        // Greater-Equal (>=)
Operation &LS(GenObject &a, GenObject &b, const char *comment="");        // Less (<)
Operation &LE(GenObject &a, GenObject &b, const char *comment="");        // Less-Equal (<=)
Operation &INV_L(GenObject &a, const char *comment="");        // bits inversion
Operation &INV(GenObject &a, const char *comment="");        // logical inversion
Operation &OR2_L(GenObject &a, GenObject &b, const char *comment="");   // bitwise OR
Operation &OR2(GenObject &a, GenObject &b, const char *comment="");
Operation &OR3(GenObject &a, GenObject &b, GenObject &c, const char *comment="");
Operation &OR4(GenObject &a, GenObject &b, GenObject &c, GenObject &d, const char *comment="");
Operation &ORx(size_t cnt, ...);
Operation &ORx_L(size_t cnt, ...);
Operation &OR_REDUCE(GenObject &a, const char *comment="");
Operation &XOR2(GenObject &a, GenObject &b, const char *comment="");
Operation &XORx(size_t cnt, ...);
Operation &AND2_L(GenObject &a, GenObject &b, const char *comment="");
Operation &AND3_L(GenObject &a, GenObject &b, GenObject &c, const char *comment="");
Operation &ADD2(GenObject &a, GenObject &b, const char *comment="");
Operation &ADDx(size_t cnt, ...);
Operation &CALCWIDTHx(size_t cnt, ...);
Operation &SUB2(GenObject &a, GenObject &b, const char *comment="");
Operation &AND_REDUCE(GenObject &a, const char *comment="");
Operation &AND2(GenObject &a, GenObject &b, const char *comment="");
Operation &AND3(GenObject &a, GenObject &b, GenObject &c, const char *comment="");
Operation &AND4(GenObject &a, GenObject &b, GenObject &c, GenObject &d, const char *comment="");
Operation &ANDx(size_t cnt, ...);
Operation &ANDx_L(size_t cnt, ...);
Operation &DEC(GenObject &a, const char *comment="");
Operation &INC(GenObject &a, const char *comment="");
Operation &INCVAL(GenObject &res, GenObject &inc, const char *comment="");
Operation &MUL2(GenObject &a, GenObject &b, const char *comment="");
Operation &DIV2(GenObject &a, GenObject &b, const char *comment="");
Operation &CC2(GenObject &a, GenObject &b, const char *comment="");     // concatation
Operation &CC3(GenObject &a, GenObject &b, GenObject &c, const char *comment="");     // concatation
Operation &CC4(GenObject &a, GenObject &b, GenObject &c, GenObject &d, const char *comment="");     // concatation
Operation &CCx(size_t cnt, ...);                    // concatation
Operation &SPLx(GenObject &a, size_t cnt, ...);     // cplit concatated bus
Operation &LSH(GenObject &a, GenObject &sz, const char *comment="");    // left shift
Operation &LSH(GenObject &a, int sz, const char *comment="");           // left shift
Operation &RSH(GenObject &a, GenObject &sz, const char *comment="");    // right shift
Operation &RSH(GenObject &a, int sz, const char *comment="");           // right shift

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
void READMEMH(GenObject &fname, GenObject &mem);

// Create new module
void NEW(GenObject &m, const char *name, GenObject *idx=0, const char *comment="");
void CONNECT(GenObject &inst, GenObject *idx, GenObject &port, GenObject &s, const char *comment="");
void ENDNEW(const char *comment="");

// RTL specific not used in SystemC
void DECLARE_TSTR();    // declare temporary string buffer
void INITIAL();
void ENDINITIAL();
void GENERATE(const char *name, const char *comment="");
void ENDGENERATE(const char *name, const char *comment="");
Operation &ASSIGNZERO(GenObject &a, const char *comment="");
Operation &ASSIGN(GenObject &a, GenObject &b, const char *comment="");

}  // namespace sysvc
