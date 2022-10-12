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

#include <inttypes.h>
#include <iostream>
#include <vector>
#include "genobjects.h"

namespace sysvc {

class GenValue : public GenObject {
 public:
    GenValue(const char *width, const char *val, const char *name,
             GenObject *parent, const char *comment="");
    GenValue(GenValue *width, const char *val, const char *name,
             GenObject *parent, const char *comment="");
    GenValue(const char *width, GenObject *val, const char *name,
             GenObject *parent, const char *comment="");
    virtual std::string getStrValue() override;
};

class BOOL : public GenValue {
 public:
    BOOL(const char *val, const char *name="",
        GenObject *parent=0, const char *comment=""):
        GenValue("1", val, name, parent, comment) {}

    virtual std::string getType();
};

class STRING : public GenValue {
 public:
    STRING(const char *val, const char *name="",
        GenObject *parent=0, const char *comment=""):
        GenValue("0", "", name, parent, comment) {
        strValue_ = "\"" + std::string(val) + "\"";
    }

    virtual std::string getType();
    virtual std::string getStrValue() override { return GenObject::getStrValue(); }
};

class FileValue : public GenValue {
 public:
    FileValue(const char *val, const char *name="",
        GenObject *parent=0, const char *comment=""):
        GenValue("0", "0", name, parent, comment) {
        id_ = ID_FILEVALUE;
    }

    virtual std::string getType();
};


class UI16D : public GenValue {
 public:
    UI16D(const char *val, const char *name="",
        GenObject *parent=0, const char *comment=""):
        GenValue("16", val, name, parent, comment) {}

    virtual std::string getType();
};

class I32D : public GenValue {
 public:
    I32D(const char *val, const char *name="",
        GenObject *parent=0, const char *comment=""):
        GenValue("32", val, name, parent, comment) {}
    I32D(GenObject *val, const char *name,
        GenObject *parent, const char *comment="") :
        GenValue("32", val, name, parent, comment) {}

    virtual std::string getType();
};

class UI32D : public GenValue {
 public:
    UI32D(const char *val, const char *name="",
        GenObject *parent=0, const char *comment=""):
        GenValue("32", val, name, parent, comment) {}

    virtual std::string getType();
};

class UI64H : public GenValue {
 public:
    UI64H(const char *val, const char *name="",
        GenObject *parent=0, const char *comment=""):
        GenValue("64", val, name, parent, comment) {}

    virtual std::string getType();
};

class GenVar : public I32D {
 public:
    GenVar(const char *val, const char *name,
        GenObject *parent, const char *comment="") :
        I32D(val, name, parent, comment) {}

    virtual bool isGenVar() override { return true; }
};

}  // namespace sysvc
