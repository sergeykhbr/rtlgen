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

namespace sysvc {

class GenValue {
 public:
    GenValue(const char *op);

    virtual bool isNumber() {
        return sysc_.c_str()[0] >= '0' && sysc_.c_str()[0] <= '9';
    }
    virtual uint64_t getValue() { return val_; }

    virtual std::string generate_sysc(){ return sysc_; }

 protected:
    virtual void parse(const char *op);

 protected:
    uint64_t val_;

    std::string sysc_;  // systemc representation
    std::string sysv_;  // system verilog
    std::string vhdl_;  // vhdl
};

class BOOL : public GenValue {
 public:
    BOOL(const char *op) : GenValue(op) {}
    BOOL(bool v);
};


class I32D : public GenValue {
 public:
    I32D(const char *op) : GenValue(op) {}
    I32D(int v);
};

class UI64H : public GenValue {
 public:
    UI64H(const char *op) : GenValue(op) {}
    UI64H(uint64_t v);
};

}  // namespace sysvc
