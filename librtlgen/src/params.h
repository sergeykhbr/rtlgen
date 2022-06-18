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
#include <iostream>

namespace sysvc {

class ParamObject : public GenObject {
 public:
    ParamObject(GenObject *parent,
                const char *name,
                GenValue *value,
                const char *comment);

    virtual int64_t getValue() { return value_->getValue(); }

    virtual std::string generate(EGenerateType v) { return std::string(""); }

 protected:
    GenValue *value_;
};

class ParamBOOL : public ParamObject {
 public:
    ParamBOOL::ParamBOOL(GenObject *parent, const char *name,
                         GenValue *value, const char *comment="")
        : ParamObject(parent, name, value, comment) {}

    virtual std::string generate(EGenerateType v) override;
};

class ParamI32 : public ParamObject {
 public:
    ParamI32::ParamI32(GenObject *parent, const char *name,
                       GenValue *value, const char *comment="")
        : ParamObject(parent, name, value, comment) {}

    virtual std::string generate(EGenerateType v) override;
};

class ParamUI64 : public ParamObject {
 public:
    ParamUI64::ParamUI64(GenObject *parent, const char *name,
                         GenValue *value, const char *comment="")
        : ParamObject(parent, name, value, comment) {}

    virtual std::string generate(EGenerateType v) override;
};


}  // namespace sysvc
