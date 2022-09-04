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

    virtual std::string getType() override;
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

    virtual std::string getType() override;
};


template<class T>
class IoStruct : public GenObject {
public:
    IoStruct(GenObject* parent, const char* name, const char* comment = "")
        : GenObject(parent, ID_IOPORT, name, comment), s_(0, name) {
    }
    virtual std::string getType() override {
        std::string out = "";
        if (SCV_is_sysc()) {
            out += getDirection() + "<" + s_.getType() + ">";
        } else if (SCV_is_sv()) {
            out += getDirection() + " ";
            if (SCV_get_cfg_file(s_.getType()).size()) {
                out += SCV_get_cfg_file(s_.getType()) + "_pkg::";
            }
            out += s_.getType();
        } else {
        }
        return out;
    }
    T* operator->() const { return &s_; }
    T* operator->() { return &s_; }

 protected:
    virtual std::string getDirection() = 0;
    T s_;
};

template<class T>
class InStruct : public IoStruct<T> {
 public:
    InStruct(GenObject* parent, const char* name, const char* comment = "")
        : IoStruct<T>(parent, name, comment) { }
 protected:
    virtual std::string getDirection() override {
        std::string ret = "";
        if (SCV_is_sysc()) {
            ret = "sc_in";
        } else {
            ret = "input";
        }
        return ret;
    }
};

template<class T>
class OutStruct : public IoStruct<T> {
 public:
    OutStruct(GenObject* parent, const char* name, const char* comment = "")
        : IoStruct<T>(parent, name, comment) { }
 protected:
    virtual std::string getDirection() override {
        std::string ret = "";
        if (SCV_is_sysc()) {
            ret = "sc_out";
        } else {
            ret = "output";
        }
        return ret;
    }
};


}  // namespace sysvc
