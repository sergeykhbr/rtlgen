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

class Operation : public GenObject {
 public:
    Operation(const char *comment="");
    Operation(GenObject *parent, const char *comment="");

    static void start(GenObject *owner);
    static void push_obj(GenObject *obj);
    static void pop_obj();
    static void set_space(int n);

    virtual std::string getType(EGenerateType v) { return std::string(""); }
    virtual std::string generate(EGenerateType v) = 0;
 protected:
    virtual std::string addspaces();
    virtual std::string obj2varname(EGenerateType v, GenObject *obj);
 protected:
    union ArgsType {
        GenObject *obj;
        int i32;
    };

    ArgsType args[16];
    int argcnt_;
};

class ZEROS : public Operation {
 public:
    ZEROS(GenObject *a, const char *comment="")
        : Operation(comment) {
        args[argcnt_++].obj = a;
    }

    virtual std::string generate(EGenerateType v);
};

class ONE : public Operation {
 public:
    ONE(GenObject *a, const char *comment="")
        : Operation(comment) {
        args[argcnt_++].obj = a;    // output signal
    }

    virtual std::string generate(EGenerateType v);
};


class EQ : public Operation {
 public:
    EQ(GenObject *a, GenObject *b, const char *comment="")
        : Operation(comment) {
        args[argcnt_++].obj = a;
        args[argcnt_++].obj = b;
    }

    virtual std::string generate(EGenerateType v);
};


class SETBIT : public Operation {
 public:
    SETBIT(GenObject *a, GenObject *b, const char *comment="")
        : Operation(comment) {
        args[argcnt_++].obj = a;
        args[argcnt_++].obj = b;
    }

    virtual std::string generate(EGenerateType v);
};

class NOT : public Operation {
 public:
    NOT(GenObject *a, const char *comment="")
        : Operation(0, comment) {
        args[argcnt_++].obj = a;
    }

    virtual std::string generate(EGenerateType v);
};

class OR2 : public Operation {
 public:
    OR2(GenObject *a, GenObject *b, const char *comment="")
        : Operation(0, comment) {
        args[argcnt_++].obj = a;
        args[argcnt_++].obj = b;
    }

    virtual std::string generate(EGenerateType v);
};

class AND2 : public Operation {
 public:
    AND2(GenObject *a, GenObject *b, const char *comment="")
        : Operation(0, comment) {
        args[argcnt_++].obj = a;
        args[argcnt_++].obj = b;
    }

    virtual std::string generate(EGenerateType v);
};

class IF : public Operation {
 public:
    IF(GenObject *a, const char *comment="") : Operation(comment) {
        args[argcnt_++].obj = a;
        push_obj(this);
    }
    virtual std::string generate(EGenerateType v);
};

class ELSE : public Operation {
 public:
    ELSE(const char *comment="") : Operation(comment) {
        pop_obj();
        push_obj(this);
    }
    virtual std::string generate(EGenerateType v);
};

class ENDIF : public Operation {
 public:
    ENDIF(const char *comment="") : Operation(comment) {
        pop_obj();
    }
    virtual std::string generate(EGenerateType v) { return std::string(""); }
};

}  // namespace sysvc
