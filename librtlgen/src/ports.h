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
#include "params.h"
#include "logic.h"
#include "values.h"
#include <iostream>

namespace sysvc {

/*
 * Input/Output ports of the modules
 */

class InPort : public Logic {
 public:
    InPort(GenObject *parent,
           const char *name,
           const char *width="1",
           const char *comment="");

    InPort(GenObject *parent,
           const char *name,
           GenValue *width,
           const char *comment="");

    virtual std::string getType() override;
};

class OutPort : public Logic {
 public:
    OutPort(GenObject *parent,
           const char *name,
           const char *width="1",
           const char *comment="");

    OutPort(GenObject *parent,
           const char *name,
           GenValue *width,
           const char *comment="");

    virtual std::string getType() override;
};

}  // namespace sysvc
