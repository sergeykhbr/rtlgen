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
    ID_FUNCTION,
    ID_MODULE,
    ID_IO_START,    // input/output signal start marker
    ID_INPUT,
    ID_OUTPUT,
    ID_IO_END,      // input/output signal end marker
    ID_COMMENT,
    ID_EMPTYLINE,
    ID_OPERATION
};

enum EGenerateType {
    SYSC_ALL,
    SYSC_DECLRATION,
    SYSC_DEFINITION,
    SYSVERILOG_ALL
};

class GenObject {
 public:
    GenObject(GenObject *parent, EIdType id,
              const char *name, const char *comment="");

    virtual std::string getFullPath();
    virtual void add_entry(GenObject *p);

    unsigned getId() { return id_; }
    std::string getName() { return name_; }
    std::string getComment() { return comment_; }
    GenObject *getEntryById(EIdType id);

    virtual std::string generate(EGenerateType) { return std::string(""); }

 protected:
    EIdType id_;
    GenObject *parent_;
    std::string name_;
    std::string comment_;
    std::list<GenObject *> entries_;
};

}  // namespace sysvc
