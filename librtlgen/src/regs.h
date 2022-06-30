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
#include "logic.h"
#include "signals.h"
#include <iostream>

namespace sysvc {

class RegGeneric : public GenObject {
 public:
    RegGeneric(GenObject *parent,
                const char *name,
                const char *comment="");

    virtual std::string getReset(EGenerateType v) = 0;
    virtual std::string getType(EGenerateType v) = 0;
};

class Reg : public RegGeneric {
 public:
    Reg(GenObject *parent, const char *name, const char *width="1", const char *comment="")
        : RegGeneric(parent, name, comment),
        regvalue_(this, "regval", width),
        regreset_(width, "regreset", "0", this) {
    }
    virtual std::string getReset(EGenerateType v) { return regreset_.getValue(v); }
    virtual std::string getType(EGenerateType v);
 protected:
    Signal regvalue_;
    Logic regreset_;
};

}  // namespace sysvc
