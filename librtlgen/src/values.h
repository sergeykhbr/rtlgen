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

    virtual uint64_t getValue() override { return val_; }
    virtual std::string getValue(EGenerateType) override;
    virtual int getWidth() override { return width_; }
    virtual std::string getWidth(EGenerateType) override ;

 protected:
    virtual size_t parse(const char *ops, size_t pos,
                        uint64_t &num,
                        std::string &sysc,
                        std::string &sv,
                        std::string &sv_pkg,    // package name include
                        std::string &vhdl);

 protected:
    uint64_t val_;
    int width_;

    std::string sysc_;  // systemc representation
    std::string sv_;    // system verilog
    std::string sv_pkg_;// system verilog with the package name
    std::string vhdl_;  // vhdl

    std::string width_sysc_;  // systemc representation
    std::string width_sv_;    // system verilog
    std::string width_sv_pkg_;// system verilog with the package name
    std::string width_vhdl_;  // vhdl
};

class GenValueWrapper : public GenObject {
 public:
    GenValueWrapper(GenObject *parent,
                    EIdType id,
                    const char *name,
                    GenValue *value,
                    const char *comment="")
    : GenObject(parent, id, name, comment), value_(value) {}

    virtual std::string getType(EGenerateType v) override { return value_->getType(v); }
    virtual uint64_t getValue() override { return value_->getValue(); }
    virtual std::string getValue(EGenerateType v) override { return value_->getValue(v); };
    virtual int getWidth() override { return value_->getWidth(); }
    virtual std::string getWidth(EGenerateType v) override { return value_->getWidth(v); }

    virtual GenValue *getpWrappedValue() { return value_; }
protected:
    GenValue *value_;
};

class BOOL : public GenValue {
 public:
    BOOL(const char *val, const char *name="",
        GenObject *parent=0, const char *comment=""):
        GenValue("1", val, name, parent, comment) {}

    virtual std::string getType(EGenerateType);
};

class I32D : public GenValue {
 public:
    I32D(const char *val, const char *name="",
        GenObject *parent=0, const char *comment=""):
        GenValue("32", val, name, parent, comment) {}

    virtual std::string getType(EGenerateType);
};

class UI32D : public GenValue {
 public:
    UI32D(const char *val, const char *name="",
        GenObject *parent=0, const char *comment=""):
        GenValue("32", val, name, parent, comment) {}

    virtual std::string getType(EGenerateType);
};

class UI64H : public GenValue {
 public:
    UI64H(const char *val, const char *name="",
        GenObject *parent=0, const char *comment=""):
        GenValue("64", val, name, parent, comment) {}

    virtual std::string getType(EGenerateType);
};

}  // namespace sysvc
