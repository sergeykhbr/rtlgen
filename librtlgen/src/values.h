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
    virtual std::string getStrWidth() override ;
    virtual bool isLocal();

 protected:
    virtual size_t parse(const char *ops, size_t pos,
                        uint64_t &num,
                        std::string &sysc,
                        std::string &sv,
                        std::string &sv_pkg,    // package name include
                        std::string &vhdl);

 protected:
    std::string sysc_;  // systemc representation
    std::string sv_;    // system verilog
    std::string sv_pkg_;// system verilog with the package name
    std::string vhdl_;  // vhdl

    std::string width_sysc_;  // systemc representation
    std::string width_sv_;    // system verilog
    std::string width_sv_pkg_;// system verilog with the package name
    std::string width_vhdl_;  // vhdl
};

class BOOL : public GenValue {
 public:
    BOOL(const char *val, const char *name="",
        GenObject *parent=0, const char *comment=""):
        GenValue("1", val, name, parent, comment) {}

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

}  // namespace sysvc
