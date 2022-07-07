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
#include "logic.h"
#include <iostream>
#include <list>

namespace sysvc {

typedef std::string (*generate_type)(EGenerateType v, GenObject **args);

class Operation : public GenObject {
 public:
    Operation(const char *comment="");
    Operation(GenObject *parent, const char *comment="");

    static void start(GenObject *owner);
    static void push_obj(GenObject *obj);
    static void pop_obj();
    static void set_space(int n);
    static std::string addspaces();
    static std::string obj2varname(const char *prefix, EGenerateType v, GenObject *obj);
    static std::string obj2varname(EGenerateType v, GenObject *obj);
    static std::string addtext(EGenerateType v, GenObject *obj, size_t curpos);

    virtual void add_arg(GenObject *arg) {
        args[argcnt_++] = arg;
    }
    virtual std::string getType(EGenerateType v) { return std::string(""); }
    virtual std::string generate(EGenerateType v) {
        std::string ret = igen_(v, args);
        for (auto &e: entries_) {
            if (e->getId() != ID_OPERATION) {
                continue;
            }
            ret += e->generate(v);
        }
        return ret;
    }

    generate_type igen_;
 protected:
    GenObject *args[16];
    int argcnt_;
};

void TEXT(const char *comment="");
Operation &SETZERO(GenObject &a, const char *comment="");
Operation &SETONE(GenObject &a, const char *comment="");
Operation &SETALLONE(GenObject &a, const char *comment="");
Operation &SETBIT(GenObject &a, GenObject &b, const char *comment="");
Operation &SETVAL(GenObject &a, GenObject &b, const char *comment="");
Operation &TO_INT(GenObject &a, const char *comment="");
Operation &EQ(GenObject &a, GenObject &b, const char *comment="");  // ==
Operation &EZ(GenObject &a, const char *comment="");        // equal-zero
Operation &NZ(GenObject &a, const char *comment="");        // Non-zero
Operation &INV(GenObject &a, const char *comment="");        // logical inversion
Operation &OR2(GenObject &a, GenObject &b, const char *comment="");
Operation &OR3(GenObject &a, GenObject &b, GenObject &c, const char *comment="");
Operation &OR4(GenObject &a, GenObject &b, GenObject &c, GenObject &d, const char *comment="");
Operation &AND2(GenObject &a, GenObject &b, const char *comment="");
Operation &AND3(GenObject &a, GenObject &b, GenObject &c, const char *comment="");
Operation &AND4(GenObject &a, GenObject &b, GenObject &c, GenObject &d, const char *comment="");
Operation &INC(GenObject &a, const char *comment="");

Operation &SELECTARRITEM(GenObject &arr, GenObject &mux, const char *comment="");

void IF(GenObject &a, const char *comment="");
void ELSIF(GenObject &a, const char *comment="");
void ELSE(const char *comment="");
void ENDIF(const char *comment="");

void SYNC_RESET(GenObject &a, const char *comment="");

}  // namespace sysvc
