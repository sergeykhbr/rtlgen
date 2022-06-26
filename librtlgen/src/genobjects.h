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

#include <iostream>
#include <list>

namespace sysvc {

enum EIdType {
    ID_PROJECT,
    ID_FOLDER,
    ID_FILE,
    ID_CONST,
    ID_VALUE,
    ID_ENUM,
    ID_PARAM,
    ID_DEF_PARAM,   // Generic parameter used in ifdef/endif statements
    ID_FUNCTION,
    ID_MODULE,
    ID_MINSTANCE,   // module instance
    ID_INPUT,
    ID_OUTPUT,
    ID_SIGNAL,
    ID_REG,
    ID_STRUCT_DEF,
    ID_STRUCT_INST,
    ID_PROCESS,
    ID_COMMENT,
    ID_EMPTYLINE,
    ID_OPERATION
};

enum EGenerateType {
    SYSC_ALL,
    SYSC_H,
    SYSC_CPP,
    SV_ALL,
    SV_PKG,
    SV_MOD,
    VHDL_ALL,
    VHDL_PKG,
    VHDL_MOD
};

class GenObject {
 public:
    GenObject(GenObject *parent, EIdType id,
              const char *name, const char *comment="");

    virtual std::string getFullPath();
    virtual std::string getFile();
    virtual void add_entry(GenObject *p);

    unsigned getId() { return id_; }
    GenObject *getParent() { return parent_; }
    std::string getName() { return name_; }
    std::string getComment() { return comment_; }
    virtual std::string getType(EGenerateType) = 0;
    virtual std::string getValue(EGenerateType) { return std::string(""); }

    virtual std::string generate(EGenerateType) { return std::string(""); }

 protected:
    EIdType id_;
    GenObject *parent_;
    std::string name_;
    std::string comment_;
    std::list<GenObject *> entries_;
};

}  // namespace sysvc
