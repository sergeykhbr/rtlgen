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
#include <list>

namespace sysvc {

class StructObject : public GenObject {
 public:
    // Declare structure type and create on instance
    StructObject(GenObject *parent,
                 StructObject *type,
                 const char *name,
                 const char *comment="");

    virtual std::string getType(EGenerateType) { return type_; }
    virtual std::string generate(EGenerateType v);
 protected:
    std::string generate_sysc();
    std::string generate_sysv();
    std::string generate_vhdl();
 protected:
    std::string type_;
    std::list<std::string> instances_;   // instance list
};

}  // namespace sysvc
