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

class ParamBOOL : public BOOL {
 public:
    ParamBOOL(GenObject *parent,
              const char *name,
              const char *val,
              const char *comment=NO_COMMENT)
        : BOOL(parent, name, val, comment) {
        SCV_set_cfg_type(this);
    }
    virtual bool isParam() override { return true; }
    virtual bool isGenericDep() override { return objValue_->isGenericDep(); }
    virtual std::string generate() override;
};

class ParamString : public STRING {
 public:
    ParamString(GenObject *parent,
                const char *name,
                const char *val,
                const char *comment=NO_COMMENT)
        : STRING(parent, name, val, comment) {
        SCV_set_cfg_type(this);
    }

    virtual bool isParam() override { return true; }
    virtual bool isGenericDep() override { return objValue_->isGenericDep(); }
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

class ParamI32D : public I32D {
 public:
    ParamI32D(GenObject *parent,
              const char *name,
              const char *val,
              const char *comment=NO_COMMENT)
        : I32D(parent, name, val, comment) {
        SCV_set_cfg_type(this);
    }

    ParamI32D(GenObject *parent,
              const char *name,
              GenObject *val,
              const char *comment=NO_COMMENT)
        : I32D(parent, name, val, comment) {
        SCV_set_cfg_type(this);
    }

    ParamI32D(GenObject *parent,
              const char *name,
              int val,
              const char *comment=NO_COMMENT)
        : ParamI32D(parent, name, new DecConst(val), comment) {
        SCV_set_cfg_type(this);
    }

    virtual bool isParam() override { return true; }
    virtual bool isGenericDep() override { return objValue_->isGenericDep(); }
    virtual std::string generate() override;
};


class ParamUI32D : public UI32D {
 public:
    ParamUI32D(GenObject *parent,
               const char *name,
               const char *val,
               const char *comment=NO_COMMENT)
        : UI32D(parent, name, val, comment) {
        SCV_set_cfg_type(this);
    }

    virtual bool isParam() override { return true; }
    virtual bool isGenericDep() override { return objValue_->isGenericDep(); }
    virtual std::string generate() override;
};

class ParamUI64H : public UI64H {
 public:
    ParamUI64H(GenObject *parent,
               const char *name,
               const char *val,
               const char *comment=NO_COMMENT)
        : UI64H(parent, name, val, comment) {
        SCV_set_cfg_type(this);
    }

    virtual bool isParam() override { return true; }
    virtual bool isGenericDep() override { return objValue_->isGenericDep(); }
    virtual std::string generate() override;
};

class ParamTIMESEC : public TIMESEC {
 public:
    ParamTIMESEC(GenObject *parent,
                 const char *name,
                 const char *val,
                 const char *comment=NO_COMMENT)
        : TIMESEC(parent, name, val, comment) {
        SCV_set_cfg_type(this);
    }

    virtual bool isParam() override { return true; }
    virtual bool isGenericDep() override { return objValue_->isGenericDep(); }
    virtual std::string generate() override;
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
    virtual std::string generate() override;
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
