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

namespace sysvc {

class Operation : public GenObject {
 public:
    Operation(GenObject *parent,
              GenObject *a,
              GenObject *b,
              const char *comment="");

    virtual std::string generate(EGenerateType v) = 0;
 protected:
    GenObject *a_;
    GenObject *b_;
};

class ZEROS : public Operation {
 public:
    ZEROS(GenObject *parent, GenObject *a, const char *comment="")
        : Operation(parent, a, 0, comment) {}

    virtual std::string generate(EGenerateType v);
};


class EQ : public Operation {
 public:
    EQ(GenObject *parent, GenObject *a, GenObject *b, const char *comment="")
        : Operation(parent, a, b, comment) {}

    virtual std::string generate(EGenerateType v);
};


class SETBIT : public Operation {
 public:
    SETBIT(GenObject *parent, GenObject *a, GenObject *b, const char *comment="")
        : Operation(parent, a, b, comment) {}

    virtual std::string generate(EGenerateType v);
};


}  // namespace sysvc
