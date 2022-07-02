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

class ArrayObject : public GenObject {
 public:
    ArrayObject(GenObject *parent,
                GenObject *type,
                const char *name,
                const char *depth,
                const char *comment="");

    virtual std::string getType(EGenerateType v) { return type_; }
    virtual int getDepth() override { return static_cast<int>(depth_.getValue()); }    // two-dimensional object
    virtual std::string getDepth(EGenerateType v) override { return depth_.getValue(v); }
    virtual std::string generate(EGenerateType v);
 protected:
    std::string generate_sysc();
    std::string generate_sysv();
    std::string generate_vhdl();
 protected:
    std::string type_;
    I32D depth_;
    GenObject **arr_;
};

class ArrayItem : public GenObject {
 public:
    ArrayItem(GenObject *parent, GenObject *type, int idx)
              : GenObject(parent, ID_ARRAY_ITEM, ""), type_(type), idx_(idx) {}

    virtual std::string getType(EGenerateType v) { return type_->getName(); }
    virtual std::string getName() override;
 protected:
    GenObject *type_;
    int idx_;
};

}  // namespace sysvc
