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
                 const char *type,
                 const char *name,
                 const char *comment);

    /** GenObject generic methods */
    virtual bool isStruct() override { return true; }
    virtual bool isTypedef() override;
    virtual bool isInterface() override;
    virtual std::string getStrValue() override;

    virtual std::string generate() override;

 protected:
    virtual std::string generate_interface();
    virtual std::string generate_vector_type();
    virtual std::string generate_param();
    virtual std::string generate_interface_constructor();
    virtual std::string generate_interface_constructor_init();
    virtual std::string generate_interface_op_equal();      // operator ==
    virtual std::string generate_interface_op_assign();     // operator =
    virtual std::string generate_interface_op_stream();     // operator <<
    virtual std::string generate_interface_op_bracket();    // operator [] for vector only
    virtual std::string generate_interface_sc_trace();      // sc_trace
};

class RegStructObject : public StructObject {
 public:
    RegStructObject(GenObject *parent, const char *type, const char *name,
                 const char *comment)
        : StructObject(parent, type, name, comment) {}

    virtual bool isSignal() override { return true; }
    virtual bool isReg() override { return true; }
};

}  // namespace sysvc
