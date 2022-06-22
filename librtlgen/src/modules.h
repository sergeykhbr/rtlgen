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
#include "minstance.h"
#include <iostream>
#include <list>

namespace sysvc {

/**
 * Parent module class definition
 */
class ModuleObject : public GenObject {
 public:
    ModuleObject(GenObject *parent, const char *name);

    virtual std::string getType(EGenerateType v);
    virtual std::string generate(EGenerateType v) override;
    virtual MInstanceObject *createInstance(GenObject *parent, const char *name);

    // Check registers in current module and all sub-modules to create
    // async_reset logic and always (ff) process
    virtual bool isAsyncReset();
    virtual bool isRegProcess();
 protected:
    std::string generate_sysc_h();
    std::string generate_sysc_cpp();
    std::string generate_sv_pkg();
    std::string generate_sv_mod();

 protected:
    std::list<MInstanceObject *> instances_;
};

}  // namespace sysvc
