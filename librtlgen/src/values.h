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
    GenValue(GenObject *parent, const char *name, const char *val,
            const char *comment=NO_COMMENT);
    GenValue(GenObject *parent, const char *name, GenObject *val,
            const char *comment=NO_COMMENT);

    virtual bool isValue() override { return true; }
    virtual bool isConst() override { return getName() == ""; }
    virtual std::string getName() override;
    virtual std::string getStrValue() override;

    /** Signal could be a register when it inside of register struct */
    virtual bool isReg() override;
    virtual bool isNReg() override;

    virtual std::string v_name(std::string v) override;
    virtual std::string r_name(std::string v) override;
};

class BOOL : public GenValue {
 public:
    BOOL(const char *val, const char *name="",
        GenObject *parent=0, const char *comment=""):
        GenValue(parent, name, val, comment) {}

    virtual std::string getType() override;
    virtual std::string getStrValue() override;
    virtual uint64_t getValue() override;
    virtual int getWidth() override { return 1; }
    virtual std::string generate() override;
};

class STRING : public GenValue {
 public:
    STRING(const char *val, const char *name="",
        GenObject *parent=0, const char *comment="")
        : GenValue(parent, name, val, comment) {}

    virtual bool isString() override { return true; }
    virtual std::string getType();
    virtual std::string generate() override;
};

class FileValue : public GenValue {
 public:
    FileValue(const char *val, const char *name="",
        GenObject *parent=0, const char *comment=""):
        GenValue(parent, name, val, comment) {
        id_ = ID_FILEVALUE;
    }

    virtual std::string getType();
};


class UI16D : public GenValue {
 public:
    UI16D(const char *val, const char *name="",
        GenObject *parent=0, const char *comment=""):
        GenValue(parent, name, val, comment) {}

    virtual std::string getType();
    virtual int getWidth() override { return 16; }
};

class I32D : public GenValue {
 public:
    I32D(const char *val, const char *name="",
        GenObject *parent=0, const char *comment=""):
        GenValue(parent, name, val, comment) {}
    I32D(GenObject *val, const char *name,
        GenObject *parent, const char *comment="") :
        GenValue(parent, name, val, comment) {}

    virtual std::string getType();
    virtual int getWidth() override { return 32; }
};

class UI32D : public GenValue {
 public:
    UI32D(const char *val, const char *name="",
        GenObject *parent=0, const char *comment=""):
        GenValue(parent, name, val, comment) {}

    virtual std::string getType();
    virtual int getWidth() override { return 32; }
};

class UI64H : public GenValue {
 public:
    UI64H(const char *val, const char *name="",
        GenObject *parent=0, const char *comment=""):
        GenValue(parent, name, val, comment) {}

    virtual bool isHex() override { return true; }
    virtual std::string getType();
    virtual int getWidth() override { return 64; }
};

class TIMESEC : public GenValue {
 public:
    TIMESEC(const char *val, const char *name="",
        GenObject *parent=0, const char *comment=""):
        GenValue(parent, name, val, comment) {}

    virtual std::string getType();
    virtual bool isFloat() { return true; }
};

/** Variable used in generate cycle */
class GenVar : public I32D {
 public:
    GenVar(const char *val, const char *name,
        GenObject *parent, const char *comment="") :
        I32D(val, name, parent, comment) {}

    virtual bool isGenVar() override { return true; }
    virtual std::string getStrValue() override { return getName(); }
};

}  // namespace sysvc
