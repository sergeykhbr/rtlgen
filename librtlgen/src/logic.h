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
#include <iostream>

namespace sysvc {

class Param;

class Logic : public GenValue {
 public:
    Logic(const char *width="1",
          const char *name="",
          const char *val="",
          GenObject *parent=0,
          const char *comment="");

    Logic(GenValue *width,
          const char *name="",
          const char *val="",
          GenObject *parent=0,
          const char *comment="");

    Logic(Param *width,
          const char *name="",
          const char *val="",
          GenObject *parent=0,
          const char *comment="");

    virtual void allzero();
    virtual void eq(const char *val);
    virtual void setbit(const char *idx);
    virtual void clearbit(const char *idx);

    virtual std::string getType(EGenerateType);
    virtual uint64_t getValue() override { return GenValue::getValue(); }
    virtual std::string getValue(EGenerateType v) override;
 protected:
    GenValue *width_;
};

}  // namespace sysvc