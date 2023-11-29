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

class Signal : public Logic {
 public:
    Signal(const char *width,
           const char *name,
           const char *val,
           GenObject *parent,
           const char *comment);

    Signal(GenObject *parent,
           const char *name,
           const char *width,
           const char *val="'0",
           const char *comment="");

    virtual bool isSignal() override { return true; }
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
};

template<class T>
class SignalStruct : public T {
 public:
    SignalStruct(GenObject *parent, const char *name, const char *comment)
        : T(parent, name, comment) {
    }
    virtual bool isSignal() override { return true; }
};


}  // namespace sysvc
