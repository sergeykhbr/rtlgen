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
#include "logic.h"
#include "utils.h"

namespace sysvc {

template<class T>
class ParamType : public T {
 public:
    ParamType(GenObject *parent,
              const char *name,
              const char *val,
              const char *comment) : T(parent, name, val, comment) {
        SCV_set_cfg_type(this);
    }

    ParamType(GenObject *parent,
              const char *name,
              GenObject *val,
              const char *comment) : T(parent, name, val, comment) {
        SCV_set_cfg_type(this);
    }

    virtual bool isParam() override { return true; }
    virtual bool isGenericDep() override { return T::objValue_->isGenericDep(); }
    virtual std::string generate() override { return T::getStrValue(); }
};

class ParamBOOL : public ParamType<BOOL> {
 public:
    ParamBOOL(GenObject *parent,
              const char *name,
              const char *val,
              const char *comment=NO_COMMENT)
        : ParamType<BOOL>(parent, name, val, comment) {}
};

class ParamString : public ParamType<STRING> {
 public:
    ParamString(GenObject *parent,
                const char *name,
                const char *val,
                const char *comment=NO_COMMENT)
        : ParamType<STRING>(parent, name, val, comment) {}

    virtual std::string generate() override;
};

class ParamUI16D : public ParamType<UI16D> {
 public:
    ParamUI16D(GenObject *parent,
               const char *name,
               const char *val,
               const char *comment=NO_COMMENT)
        : ParamType<UI16D>(parent, name, val, comment) {}
};

class ParamI32D : public ParamType<I32D> {
 public:
    ParamI32D(GenObject *parent,
              const char *name,
              const char *val,
              const char *comment=NO_COMMENT)
        : ParamType<I32D>(parent, name, val, comment) {}

    ParamI32D(GenObject *parent,
              const char *name,
              GenObject *val,
              const char *comment=NO_COMMENT)
        : ParamType<I32D>(parent, name, val, comment) {}

    ParamI32D(GenObject *parent,
              const char *name,
              int val,
              const char *comment=NO_COMMENT)
        : ParamI32D(parent, name, new DecConst(val), comment) {}
};


class ParamUI32D : public ParamType<UI32D> {
 public:
    ParamUI32D(GenObject *parent,
               const char *name,
               const char *val,
               const char *comment=NO_COMMENT)
        : ParamType<UI32D>(parent, name, val, comment) {}
};

class ParamUI64H : public ParamType<UI64H> {
 public:
    ParamUI64H(GenObject *parent,
               const char *name,
               const char *val,
               const char *comment=NO_COMMENT)
        : ParamType<UI64H>(parent, name, val, comment) {}
};

class ParamTIMESEC : public ParamType<TIMESEC> {
 public:
    ParamTIMESEC(GenObject *parent,
                 const char *name,
                 const char *val,
                 const char *comment=NO_COMMENT)
        : ParamType<TIMESEC>(parent, name, val, comment) {}
};

class ParamLogic : public Logic {
 public:
    ParamLogic(GenObject *parent,
               const char *name,
               const char *width,
               const char *val,
               const char *comment)
        : Logic(parent, name, width, val, comment) {
        SCV_set_cfg_type(this);
    }

    virtual bool isParam() override { return true; }
    virtual bool isGenericDep() override { return objValue_->isGenericDep(); }
    virtual std::string generate() override { return Logic::getStrValue(); }
};

template<class T>
class ParamStruct : public T {
    public:
    ParamStruct(GenObject *parent,
                const char *name,
                const char *comment)
        : T(parent, name, comment) {
        SCV_set_cfg_type(this);
    }
    virtual bool isParam() override { return true; }
};

}  // namespace sysvc
