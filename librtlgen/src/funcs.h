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
#include <iostream>
#include <vector>

namespace sysvc {

class FunctionObject : public GenObject {
 public:
    FunctionObject(GenObject *parent,
                   const char *name,
                   const char *comment="");

    virtual bool isFunction() override { return true; }
    virtual std::string getType() { return std::string(""); }
    virtual std::string generate();
    virtual void getArgsList(std::list<GenObject *> &args) {}
    virtual GenObject *getpReturn() { return 0; }

 protected:
    std::string generate_sysc();
    std::string generate_sysv();
    std::string generate_vhdl();
};

}  // namespace sysvc
