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

class StructDefObject : public GenObject {
 public:
    // Declare structure type and create on instance
    StructDefObject(GenObject *parent,
                    const char *type,
                    const char *name,
                    const char *comment="");

    virtual std::string getType(EGenerateType) { return getName(); }
    virtual std::string generate(EGenerateType v);

    virtual void add_instance(GenObject *parent, const char *name);
 protected:
    std::string generate_sysc();
    std::string generate_sysv();
    std::string generate_vhdl();
 protected:
    std::list<std::string> instlist_;   // instance list
};

class StructInstObject : public GenObject {
 public:
    StructInstObject(GenObject *parent,
                     StructDefObject *structdef,
                     const char *name,
                     const char *comment="")
    : GenObject(parent, ID_STRUCT_INST, name, comment), structdef_(structdef) {}

    virtual std::string getType(EGenerateType v) { return structdef_->getType(v); }
    StructDefObject *getStructDef() { return structdef_; }
 protected:
    StructDefObject *structdef_;
};


}  // namespace sysvc
