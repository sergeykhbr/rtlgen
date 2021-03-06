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
#include "utils.h"
#include "params.h"
#include <iostream>
#include <list>

namespace sysvc {

class ArrayObject : public GenObject {
 public:
    ArrayObject(GenObject *parent,
                const char *name,
                const char *depth,
                const char *comment="");

    virtual std::string getType();
    virtual int getDepth() override { return static_cast<int>(depth_.getValue()); }    // two-dimensional object
    virtual std::string getStrDepth() override { return depth_.getStrValue(); }
    virtual void setSelector(GenObject *sel) { sel_ = sel; }
    virtual GenObject *getSelector() { return sel_; }
    virtual GenObject *getItem() = 0;
 protected:
    I32D depth_;
    GenObject *sel_;
};

// T = signal or logic
template<class T>
class WireArray : public ArrayObject {
 public:
    WireArray(GenObject *parent, const char *name, const char *width, const char *depth, bool reg=false, const char *comment="")
        : ArrayObject(parent, name, depth, comment) {
        char tstr[64];
        reg_ = reg;
        arr_ = new T *[depth_.getValue()];
        for (int i = 0; i < static_cast<int>(depth_.getValue()); i++) {
            RISCV_sprintf(tstr, sizeof(tstr), "%d", i);
            arr_[i] = new T(this, tstr, width);
        }
    }
    // No need to redfine operator '->' because we use this object directly
    virtual GenObject *getItem() { return arr_[0]; }

    T **arr_;
};

template<class T>
class TStructArray : public ArrayObject {
    public:
    TStructArray(GenObject *parent, const char *name, const char *depth, bool reg=false, const char *comment="")
        : ArrayObject(parent, name, depth, comment) {
        reg_ = reg;
        arr_ = new T *[depth_.getValue()];
        for (int i = 0; i < static_cast<int>(depth_.getValue()); i++) {
            arr_[i] = new T(this, i);
        }
    }
    T *operator->() const { return arr_[0]; }
    virtual GenObject *getItem() { return arr_[0]; }

    T **arr_;
};

template<class T>
class ModuleArray : public ArrayObject {
 public:
    ModuleArray(GenObject *parent, const char *name, const char *depth, const char *comment="")
        : ArrayObject(parent, name, depth, comment) {
        arr_ = new T *[depth_.getValue()];
        char tstr[64];
        for (int i = 0; i < static_cast<int>(depth_.getValue()); i++) {
            RISCV_sprintf(tstr, sizeof(tstr), "%s%d", name, i);
            arr_[i] = new T(this, tstr);
        }
    }
    T *operator->() const { return arr_[0]; }
    virtual GenObject *getItem() { return arr_[0]; }
    virtual void changeTmplParameter(const char *name, const char *val) {
        static_cast<ModuleObject *>(getItem())->changeTmplParameter(name, val);
    }
        
    T **arr_;
};

class StringArray : public ArrayObject {
 public:
    StringArray(GenObject *parent, const char *name, const char *depth, const char *comment="")
        : ArrayObject(parent, name, depth, comment) {
        id_ = ID_ARRAY_STRING;
        arr_ = new ParamString *[depth_.getValue()];
        char tstr[64];
        for (int i = 0; i < static_cast<int>(depth_.getValue()); i++) {
            RISCV_sprintf(tstr, sizeof(tstr), "%s%d", name, i);
            arr_[i] = new ParamString(this, tstr, "");
        }
    }
    ParamString *operator->() const { return arr_[0]; }
    virtual GenObject *getItem() { return arr_[0]; }
    virtual void setValue(int idx, const char *v) { arr_[idx]->setName(v); }
        
    ParamString **arr_;
};

}  // namespace sysvc
