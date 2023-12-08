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

class ParamBOOL : public BOOL {
 public:
    ParamBOOL(GenObject *parent, const char *name,
                const char *val, const char *comment="")
        : BOOL(val, name, parent, comment) {
        SCV_set_cfg_type(this);
    }
    virtual bool isParam() override { return true; }
    virtual bool isGenericDep() override { return objValue_->isGenericDep(); }
    virtual std::string generate() override { return BOOL::getStrValue(); }
};

class ParamString : public STRING {
 public:
    ParamString(GenObject *parent, const char *name,
                const char *val, const char *comment="")
        : STRING(val, name, parent, comment) {
        SCV_set_cfg_type(this);
    }
    virtual bool isParam() override { return true; }
    virtual bool isGenericDep() override { return objValue_->isGenericDep(); }
    virtual std::string generate() override;
};

class ParamUI16D : public UI16D {
 public:
    ParamUI16D(GenObject *parent, const char *name, const char *val,
        const char *comment="")
        : UI16D(val, name, parent, comment) {
            SCV_set_cfg_type(this);
    }
    virtual bool isParam() override { return true; }
    virtual bool isGenericDep() override { return objValue_->isGenericDep(); }
    virtual std::string generate() override { return UI16D::getStrValue(); }
};

class ParamI32D : public I32D {
 public:
    ParamI32D(GenObject *parent, const char *name, const char *val,
        const char *comment="") : I32D(val, name, parent, comment) {
            SCV_set_cfg_type(this);
    }
    ParamI32D(GenObject *parent, const char *name, GenObject *val,
        const char *comment="") : I32D(val, name, parent, comment) {
            SCV_set_cfg_type(this);
    }
    virtual bool isParam() override { return true; }
    virtual bool isGenericDep() override { return objValue_->isGenericDep(); }
    virtual std::string generate() override { return I32D::getStrValue(); }
};


class ParamUI32D : public UI32D {
 public:
    ParamUI32D(GenObject *parent, const char *name, const char *val,
        const char *comment="") : UI32D(val, name, parent, comment) {
            SCV_set_cfg_type(this);
    }
    virtual bool isParam() override { return true; }
    virtual bool isGenericDep() override { return objValue_->isGenericDep(); }
    virtual std::string generate() override { return UI32D::getStrValue(); }
};

class ParamUI64H : public UI64H {
 public:
    ParamUI64H(GenObject *parent, const char *name, const char *val,
        const char *comment="") : UI64H(val, name, parent, comment) {
            SCV_set_cfg_type(this);
    }
    virtual bool isParam() override { return true; }
    virtual bool isGenericDep() override { return objValue_->isGenericDep(); }
    virtual std::string generate() override { return UI64H::getStrValue(); }
};

class ParamLogic : public Logic {
 public:
    ParamLogic(GenObject *parent, const char *width, const char *name,
                const char *val, const char *comment="")
        : Logic(width, name, val, parent, comment) {
        SCV_set_cfg_type(this);
    }
    virtual bool isParam() override { return true; }
    virtual bool isGenericDep() override { return objValue_->isGenericDep(); }
    virtual std::string generate() override { return Logic::getStrValue(); }
};

class ParamTIMESEC : public TIMESEC {
 public:
    ParamTIMESEC(GenObject *parent, const char *name,
                   const char *val, const char *comment="")
        : TIMESEC(val, name, parent, comment) {
            SCV_set_cfg_type(this);
    }
    virtual bool isParam() override { return true; }
    virtual bool isGenericDep() override { return objValue_->isGenericDep(); }
    virtual std::string generate() override { return TIMESEC::getStrValue(); }
};

template<class T>
class ParamStruct : public T {
    public:
    ParamStruct(GenObject *parent, const char *name, const char *comment)
        : T(parent, name, comment) {
        SCV_set_cfg_type(this);
    }
    virtual bool isParam() override { return true; }
};

}  // namespace sysvc
