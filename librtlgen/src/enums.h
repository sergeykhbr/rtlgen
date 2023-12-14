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

class EnumObject : public GenObject {
 public:
    EnumObject(GenObject *parent,
               const char *name);

    virtual std::string getName() override { return name_; }
    virtual void add_value(const char *name, const char *comment="");

    virtual std::string generate();
 protected:
    std::string generate_sysc();
    std::string generate_sysv();
    std::string generate_vhdl();

 protected:
    std::string name_;
};

}  // namespace sysvc
