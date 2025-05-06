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
#include "logic.h"
#include "params.h"
#include "values.h"
#include <iostream>

namespace sysvc {
/**
    Variable or structure without signal flag won't get into sensitivity
    list of a process.
    Signal can be implicitly transformed into RegSignal if CLK_..EDGE is set
*/

class Signal : public Logic {
 public:
    Signal(GenObject *parent,
           GenObject *clk,
           EClockEdge edge,
           GenObject *nrst,
           EResetActive active,
           const char *name,
           const char *width,
           const char *rstval,
           const char *comment);

    Signal(GenObject *parent,
           const char *name,
           const char *width,
           const char *val="'0",
           const char *comment="")
        : Signal(parent, 0, CLK_ALWAYS, 0, ACTIVE_NONE,
                name, width, val, comment) {}

    virtual bool isSignal() override { return true; }
    virtual std::string nameInModule(EPorts port, bool no_sc_read) override {
        std::string ret = Logic::nameInModule(port, no_sc_read);
        if (SCV_is_sysc() && !no_sc_read) {
            // registers grouped into r-structures
            ret += ".read()";
        }
        return ret;
    }
};

/**
    Force to use sc_uint<1> instead of bool in SystemC even when width=1
 */
class Signal1 : public Logic1 {
 public:
     Signal1(GenObject *parent,
             GenObject *clk,
             EClockEdge edge,
             GenObject *nrst,
             EResetActive active,
             const char *name,
             const char *width,
             const char *rstval,
             const char *comment);

    Signal1(GenObject *parent,
           const char *name,
           const char *width,
           const char *val,
           const char *comment)
        : Signal1(parent, 0, CLK_ALWAYS, 0, ACTIVE_NONE,
                name, width, val, comment) {}


    virtual bool isSignal() override { return true; }
    virtual std::string nameInModule(EPorts port, bool no_sc_read) override {
        std::string ret = Logic::nameInModule(port, no_sc_read);
        if (SCV_is_sysc() && !no_sc_read) {
            ret += ".read()";
        }
        return ret;
    }
};

// Always use sc_biguint in SystemC
class SignalBig : public Signal {
 public:
    SignalBig(GenObject *parent,
           const char *name,
           const char *width,
           const char *val="'0",
           const char *comment="")
        : Signal(parent, name, width, val, comment) {}

    virtual bool isBigSC() override { return true; }
    virtual std::string nameInModule(EPorts port, bool no_sc_read) override {
        std::string ret = Logic::nameInModule(port, no_sc_read);
        if (SCV_is_sysc() && !no_sc_read) {
            ret += ".read()";
        }
        return ret;
    }
};

template<class T>
class SignalStruct : public T {
 public:
    SignalStruct(GenObject *parent, const char *name, const char *comment)
        : T(parent, name, comment) {
    }
    virtual bool isSignal() override { return true; }
    virtual std::string nameInModule(EPorts port, bool no_sc_read) override {
        std::string ret = T::nameInModule(port, no_sc_read);
        if (SCV_is_sysc() && !no_sc_read) {
            ret += ".read()";
        }
        return ret;
    }
};


}  // namespace sysvc
