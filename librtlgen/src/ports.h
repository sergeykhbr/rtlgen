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
#include "params.h"
#include "logic.h"
#include "values.h"
#include "structs.h"
#include "utils.h"
#include <iostream>

namespace sysvc {

/*
 * Input/Output ports of the modules
 */

class InPort : public Logic {
 public:
    InPort(GenObject *parent,
           const char *name,
           const char *width,
           const char *comment="");

    virtual bool isSignal() override { return true; }
    virtual bool isInput() override { return true; }
    virtual std::string nameInModule(EPorts port, bool no_sc_read) override {
        std::string ret = Logic::nameInModule(port, no_sc_read);
        if (SCV_is_sysc() && !no_sc_read) {
            ret += ".read()";
        }
        return ret;
    }

    // ports should not have objValue. think how to fix that
    virtual std::string getStrValue() override {
        std::string t1 = getName();     // gcc compatible t1 variable
        return t1;
    }
};

class OutPort : public Logic {
 public:
    OutPort(const char *width,
          const char *name,
          const char *val,
          GenObject *parent,
          const char *comment="");

    OutPort(GenObject *parent,
           const char *name,
           const char *width,
           const char *comment="");

    virtual bool isSignal() override { return false; }
    virtual bool isOutput() override { return true; }
};

class IoPort : public Logic {
 public:
    IoPort(GenObject *parent,
           const char *name,
           const char *width,
           const char *comment="");

    virtual bool isSignal() override { return true; }
    virtual bool isInput() override { return true; }
    virtual bool isOutput() override { return true; }
    virtual std::string nameInModule(EPorts port, bool no_sc_read) override {
        std::string ret = Logic::nameInModule(port, no_sc_read);
        if (SCV_is_sysc() && !no_sc_read) {
            ret += ".read()";
        }
        return ret;
    }

    // ports should not have objValue. think how to fix that
    virtual std::string getStrValue() override {
        std::string t1 = getName();     // gcc compatible t1 variable
        return t1;
    }
};

template<class T>
class InStruct : public T {
 public:
    InStruct(GenObject *parent, const char *name, const char *comment = NO_COMMENT)
        : T(parent, name, comment) {}
 protected:
    virtual bool isSignal() override { return true; }
    virtual bool isInput() override { return true; }
    virtual bool isOutput() override { return false; }
    virtual std::string nameInModule(EPorts port, bool no_sc_read) override {
        std::string ret = T::nameInModule(port, no_sc_read);
        if (SCV_is_sysc() && !no_sc_read) {
            ret += ".read()";
        }
        return ret;
    }
    
    // ports should not have objValue. think how to fix that
    virtual std::string getStrValue() override {
        std::string t1 = T::getName();     // gcc compatible t1 variable
        return t1;
    }
};

template<class T>
class OutStruct : public T {
 public:
    OutStruct(GenObject *parent, const char *name, const char *comment = NO_COMMENT)
        : T(parent, name, comment) {}
 protected:
    virtual bool isSignal() override { return false; }
    virtual bool isInput() override { return false; }
    virtual bool isOutput() override { return true; }
};


}  // namespace sysvc
