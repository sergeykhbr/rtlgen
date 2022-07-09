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
    ID_MODULE_INST,
    ID_MINSTANCE,   // module instance
    ID_INPUT,
    ID_OUTPUT,
    ID_SIGNAL,
    ID_STRUCT_DEF,
    ID_STRUCT_INST,
    ID_ARRAY_DEF,
    ID_PROCESS,
    ID_COMMENT,
    ID_EMPTYLINE,
    ID_OPERATION
};

enum EGenerateType {
    GEN_UNDEFINED,
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
    virtual std::list<GenObject *> &getEntries() { return entries_; }

    virtual EIdType getId() { return id_; }
    virtual GenObject *getParent() { return parent_; }
    virtual std::string getName() { return name_; }
    virtual std::string getComment() { return comment_; }
    virtual std::string getType(EGenerateType) = 0;
    virtual uint64_t getValue() { return 0; }
    virtual std::string getValue(EGenerateType) { return std::string(""); }
    virtual int getWidth() { return 0; }
    virtual std::string getWidth(EGenerateType) { return std::string(""); }
    virtual int getDepth() { return 0; }    // two-dimensional object
    virtual std::string getDepth(EGenerateType) { return std::string(""); }
    virtual void setReg() { reg_ = true; };
    virtual bool isReg() { return reg_; }

    virtual std::string generate(EGenerateType) { return std::string(""); }

    virtual bool isNumber(std::string &s) {
        return s.c_str()[0] >= '0' && s.c_str()[0] <= '9';
    }

 protected:
    EIdType id_;
    GenObject *parent_;
    bool reg_;              // Mark object (signal, value, port, structure) as a Flip-flop
    std::string name_;
    std::string comment_;
    std::list<GenObject *> entries_;
};

}  // namespace sysvc
