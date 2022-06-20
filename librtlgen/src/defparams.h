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

class DefParam : public GenObject {
 public:
    DefParam(GenObject *parent,
             const char *name,
             GenValue *value,
             const char *comment="");

    virtual GenValue *getValue() { return value_; }
    virtual std::string getType(EGenerateType v) { return value_->getType(v); }
    virtual std::string generate(EGenerateType v);

 protected:
    std::string generate_sysc();
    std::string generate_sysv();
    std::string generate_vhdl();

 protected:
    GenValue *value_;
};

}  // namespace sysvc
