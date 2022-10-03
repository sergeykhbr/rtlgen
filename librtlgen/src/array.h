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
#include "modules.h"
#include <iostream>
#include <list>

namespace sysvc {

template<class T>
class ArrayObject : public GenObject {
 public:
    ArrayObject(GenObject *parent,
                const char *type,
                const char *name,
                const char *depth,
                const char *comment="")
    : GenObject(parent, type, ID_ARRAY_DEF, name, comment) {
        setStrDepth(depth);
    }

    virtual std::string getType() override {
        if (type_.size()) {
            return type_;
        }
        return getItem(0)->getType();
    }

    virtual std::string generate() override {
        std::string ret = "";
        if (SCV_is_sv()) {
            ret += "typedef " + getItem()->getType() + " " + getType();
            ret += "[0: " + getStrDepth() + "-1];\n";
        } else if (SCV_is_sysc()) {
            ret += "typedef sc_vector<sc_signal<" + getItem()->getType() + ">> ";
            ret += getType() + ";\n";
        }
        return ret;
    }

    // No need to redfine operator '->' because we use this object directly
    virtual GenObject *getItem() override { return arr_[0]; }
    virtual GenObject *getItem(int idx) override { return arr_[idx]; }
    virtual GenObject *getItem(const char *name) override { return getItem()->getItem(name); }
    T *operator->() const { return arr_[0]; }
    T *operator->() { return arr_[0]; }
    T *Item() { return arr_[0]; }
 protected:
    T **arr_;
};

// T = signal or logic
template<class T>
class WireArray : public ArrayObject<T> {
 public:
    WireArray(GenObject *parent, const char *name, const char *width,
        const char *depth, bool reg=false, const char *comment="")
        : ArrayObject(parent, "", name, depth, comment) {
        char tstr[64];
        reg_ = reg;
        arr_ = new T *[getDepth()];
        for (int i = 0; i < getDepth(); i++) {
            RISCV_sprintf(tstr, sizeof(tstr), "%d", i);
            arr_[i] = new T(width, tstr, "0", this);
        }
    }
};

template<class T>
class TStructArray : public ArrayObject<T> {
    public:
    TStructArray(GenObject *parent, const char *type, const char *name,
        const char *depth, const char *comment="")
        : ArrayObject(parent, type, name, depth, comment) {
        //reg_ = reg;
        arr_ = new T *[getDepth()];
        char tstr[64];
        for (int i = 0; i < getDepth(); i++) {
            RISCV_sprintf(tstr, sizeof(tstr), "%d", i);
            arr_[i] = new T(this, tstr);
        }
    }
};

template<class T>
class ModuleArray : public ArrayObject<T> {
 public:
    ModuleArray(GenObject *parent, const char *name, const char *depth, const char *comment="")
        : ArrayObject(parent, "", name, depth, comment) {
        arr_ = new T *[getDepth()];
        char tstr[64];
        for (int i = 0; i < getDepth(); i++) {
            RISCV_sprintf(tstr, sizeof(tstr), "%s%d", name, i);
            arr_[i] = new T(this, tstr);
        }
    }
    virtual void changeTmplParameter(const char *name, const char *val) {
        getItem(name)->setStrValue(val);
    }
};

class StringArray : public ArrayObject<ParamString> {
 public:
    StringArray(GenObject *parent, const char *name, const char *depth, const char *comment="")
        : ArrayObject(parent, "", name, depth, comment) {
        id_ = ID_ARRAY_STRING;
        arr_ = new ParamString *[getDepth()];
        char tstr[64];
        for (int i = 0; i < getDepth(); i++) {
            RISCV_sprintf(tstr, sizeof(tstr), "%s%d", name, i);
            arr_[i] = new ParamString(this, tstr, "");
        }
    }
};

}  // namespace sysvc
