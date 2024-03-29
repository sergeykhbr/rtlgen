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

/** T Object contains 3 arguments without additional modificators:
        T could be: module, struct, vector or any other object
*/
template<class T>
class ObjectArray : public T {
 public:
    ObjectArray(GenObject *parent,
                const char *name,
                const char *depth,
                const char *comment)
        : T(parent, name, comment) {
        objArridx_ = 0;
        objDepth_ = SCV_parse_to_obj(this, depth);
    }
    virtual uint64_t getDepth() override { return objDepth_->getValue(); }
    virtual GenObject *getObjDepth() override { return objDepth_; }
    virtual std::string getStrDepth() override { return objDepth_->getName(); }
    virtual void setSelector(GenObject *sel) override { objArridx_ = sel; }           // Set object as an array index
    virtual GenObject *getSelector() override { return objArridx_; }                  // generate  Name[obj]

 protected:
    GenObject *objDepth_;
    GenObject *objArridx_;                                    // Used in ARRAYITEM operation
};


/** T Value array contains 4 arguments constructor with "val".
        T could be: struct, string, parameters or other variable with value
*/
template<class T>
class ValueArray : public T {
 public:
    ValueArray(GenObject *parent,
               const char *name,
               const char *depth,
               const char *val,
               const char *comment)
        : T(parent, name, val, comment) {
        objArridx_ = 0;
        objDepth_ = SCV_parse_to_obj(this, depth);
    }
    virtual uint64_t getDepth() override { return objDepth_->getValue(); }
    virtual GenObject *getObjDepth() override { return objDepth_; }
    virtual std::string getStrDepth() override { return objDepth_->getName(); }
    virtual void setSelector(GenObject *sel) override { objArridx_ = sel; }           // Set object as an array index
    virtual GenObject *getSelector() override { return objArridx_; }                  // generate  Name[obj]

 protected:
    GenObject *objDepth_;
    GenObject *objArridx_;                                    // Used in ARRAYITEM operation
};

// T = signal or logic, contains Width argument
template<class T>
class WireArray : public T {
 public:
    WireArray(GenObject *parent,
              const char *name,
              const char *width,
              const char *depth,
              const char *comment)
        : T(parent, name, width, "'0", comment) {
        objArridx_ = 0;
        objDepth_ = SCV_parse_to_obj(this, depth);
    }
    virtual uint64_t getDepth() override { return objDepth_->getValue(); }
    virtual GenObject *getObjDepth() override { return objDepth_; }
    virtual std::string getStrDepth() override { return objDepth_->getName(); }
    virtual void setSelector(GenObject *sel) override { objArridx_ = sel; }           // Set object as an array index
    virtual GenObject *getSelector() override { return objArridx_; }                  // generate  Name[obj]

 protected:
    GenObject *objDepth_;
    GenObject *objArridx_;                                    // Used in ARRAYITEM operation
};

class LogicArray : public WireArray<Logic> {
 public:
    LogicArray(GenObject *parent, const char *name, const char *width,
        const char *depth, const char *comment)
        : WireArray<Logic>(parent, name, width, depth, comment) {}
};

class RegArray : public WireArray<RegSignal> {
 public:
    RegArray(GenObject *parent, const char *name, const char *width,
        const char *depth, const char *comment)
        : WireArray<RegSignal>(parent, name, width, depth, comment) {}
};


/**
    Vector allows to connect its element to input/output ports in systemc,
    simple array or logic cannot do that.
    Vector has it own typedef name.
 */
template<class T>
class WireVector : public WireArray<T> {
 public:
    WireVector(GenObject *parent,
               const char *tpdef,
               const char *name,
               const char *width,
               const char *depth,
               const char *comment) :
        WireArray<T>(parent, name, width, depth, comment) {
        // Logic and signal types are empty. No need to trigger dependency
        type_ = std::string(tpdef);
        if (T::name_ == type_) {
            SCV_set_cfg_type(this);
        }
    }
    virtual bool isVector() override { return true; }
    virtual bool isTypedef() override { return T::name_ == type_; }  // type is empty for logic now
    virtual std::string getType() override { return type_; }         // otherwise it will depends of bitwidth and system

 protected:
    std::string type_;
};


/**
    LogicMemory and RegMemory are almost the same except that LogicMemory
    is used for pure memory bank without other logic that allows to avoid creating
    "comb" process. Only "registers" process is created.
 */
class LogicMemory : public LogicArray {
 public:
    LogicMemory(GenObject *parent, const char *name, const char *width, const char *depth,
        const char *comment) : LogicArray(parent, name, width, depth, comment) {}

    virtual bool isResetDisabled() override { return true; }        // no registers reset
};

class RegMemory : public RegArray {
 public:
    RegMemory(GenObject *parent, const char *name, const char *width, const char *depth,
        const char *comment) : RegArray(parent, name, width, depth, comment) {}

    virtual bool isResetDisabled() override { return true; }        // no registers reset
};

/**
    Array of structures with element access by integer index without "val" argument
 */
template<class T>
class StructArray : public ObjectArray<T> {
 public:
    StructArray(GenObject *parent, const char *name, const char *depth,
        const char *comment) : ObjectArray<T>(parent, name, depth, comment) {}
};

/**
    Struct array with the new redefined type as typedef.
    In SystemC it is sc_vector<T> not just an array [].
*/
template<class T>
class StructVector : public StructArray<T> {
 public:
    StructVector(GenObject *parent, const char *tpdef, const char *name,
                const char *depth, const char *comment)
        : StructArray<T>(parent, name, depth, comment) {
        SCV_get_cfg_type(this, T::type_.c_str());   // to trigger dependecy array
        typedef_ = T::type_;
        T::type_ = std::string(tpdef);
        if (T::name_ == T::type_) {
            SCV_set_cfg_type(this);
        }
    }
    virtual bool isVector() override { return true; }
    virtual bool isSignal() override { return true; }
    virtual std::string getTypedef() override { return typedef_; }

 protected:
    std::string typedef_;
};


template<class T>
class RegStructArray : public ObjectArray<SignalStruct<T>> {
 public:
    RegStructArray(GenObject *parent, const char *name, const char *depth,
        const char *comment) : ObjectArray<SignalStruct<T>>(parent, name, depth, comment) {}

    virtual bool isReg() override { return true; }
};


template<class T>
class ModuleArray : public ObjectArray<T> {
 public:
    ModuleArray(GenObject *parent, const char *name, const char *depth,
        const char *comment) : ObjectArray<T>(parent, name, depth, comment) {}
};

}  // namespace sysvc
