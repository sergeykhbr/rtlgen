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
           const char *width="1",
           const char *comment="");

    InPort(GenObject *parent,
           const char *name,
           GenValue *width,
           const char *comment="");

    virtual bool isInput() override { return true; }
    //virtual std::string getType() override;
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
           const char *width="1",
           const char *comment="");

    OutPort(GenObject *parent,
           const char *name,
           GenValue *width,
           const char *comment="");

    virtual bool isOutput() override { return true; }
    //virtual std::string getType() override;
};


template<class T>
class IoStruct : public T {
public:
    IoStruct(GenObject* parent, const char* name, const char* comment = "")
        : T(parent, name, comment) {
    }
    /*virtual std::string getType() override {
        std::string out = "";
        if (SCV_is_sysc()) {
            if (isVector()) {
                out += "sc_vector<";
            }
            if (isInput()) {
                out += "sc_in<";
            } else {
                out += "sc_out<";
            }
            out += T::getType() + ">";
            if (isVector()) {
                out += ">";
            }
        } else if (SCV_is_sv()) {
            if (isInput()) {
                out += "input  ";
            } else {
                out += "output  ";
            }
            if (SCV_get_cfg_file(T::getType()).size()) {
                out += SCV_get_cfg_file(T::getType()) + "_pkg::";
            }
            out += T::getType();
        } else {
        }
        return out;
    }*/
    T* operator->() const { return this; }
    T* operator->() { return this; }
    virtual GenObject *getItem() override { return this; }
    virtual T* read() { return this; }
};

template<class T>
class InStruct : public IoStruct<T> {
 public:
    InStruct(GenObject* parent, const char* name, const char* comment = "")
        : IoStruct<T>(parent, name, comment) { }
 protected:
    virtual bool isInput() override { return true; }
};

template<class T>
class OutStruct : public IoStruct<T> {
 public:
    OutStruct(GenObject* parent, const char* name, const char* comment = "")
        : IoStruct<T>(parent, name, comment) { }
 protected:
    virtual bool isOutput() override { return true; }
};


}  // namespace sysvc
