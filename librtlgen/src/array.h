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
#include "values.h"
#include "signals.h"
#include "regs.h"
#include "utils.h"
#include "params.h"
#include "modules.h"
#include <iostream>
#include <list>

namespace sysvc {

// T = signal or logic
template<class T>
class WireArray : public T {
 public:
    WireArray(GenObject *parent, const char *name, const char *width,
        const char *depth, const char *comment="")
        : T(width, name, "'0", parent, comment) {
        T::setStrDepth(depth);
    }
};

class RegArray : public RegSignal {
 public:
    RegArray(GenObject *parent, const char *name, const char *width,
        const char *depth, const char *comment="")
        : RegSignal(width, name, "'0", parent, comment) {
        setStrDepth(depth);
    }
};

template<class T>
class StructArray : public T {
    public:
    StructArray(GenObject *parent, const char *name, const char *depth,
        const char *comment) : T(parent, name, comment) {
        T::setStrDepth(depth);
    }
};

template<class T>
class RegStructArray : public T {
    public:
    RegStructArray(GenObject *parent, const char *name, const char *depth,
        const char *comment) : T(parent, name, comment) {
        T::setStrDepth(depth);
    }
    virtual bool isSignal() override { return true; }
    virtual bool isReg() override { return true; }
};

}  // namespace sysvc
