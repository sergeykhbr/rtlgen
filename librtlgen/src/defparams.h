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

#include "params.h"

namespace sysvc {

class DefParamBOOL : public ParamBOOL {
 public:
    DefParamBOOL(GenObject *parent, const char *name, const char *val,
                const char *comment="")
                : ParamBOOL(parent, name, val, comment) {}
    virtual bool isParamGeneric() override { return true; }
    virtual bool isGenericDep() override { return true; }
    virtual void setObjValue(GenObject *v) override { objValue_ = v; }
};

class DefParamString : public ParamString {
 public:
    DefParamString(GenObject *parent, const char *name, const char *val,
                const char *comment="")
                : ParamString(parent, name, val, comment) {}
    virtual bool isParamGeneric() override { return true; }
    virtual bool isGenericDep() override { return true; }
    virtual void setObjValue(GenObject *v) override { objValue_ = v; }
};

class DefParamUI16D : public ParamUI16D {
 public:
    DefParamUI16D(GenObject *parent, const char *name, const char *val,
                const char *comment="")
                : ParamUI16D(parent, name, val, comment) {}
    virtual bool isParamGeneric() override { return true; }
    virtual bool isGenericDep() override { return true; }
    virtual void setObjValue(GenObject *v) override { objValue_ = v; }
};

class DefParamI32D : public ParamI32D {
 public:
    DefParamI32D(GenObject *parent, const char *name, const char *val,
                const char *comment="")
                : ParamI32D(parent, name, val, comment) {}
    virtual bool isParamGeneric() override { return true; }
    virtual bool isGenericDep() override { return true; }
    virtual void setObjValue(GenObject *v) override { objValue_ = v; }
};

class DefParamUI32D : public ParamUI32D {
 public:
    DefParamUI32D(GenObject *parent, const char *name, const char *val,
                const char *comment="")
                : ParamUI32D(parent, name, val, comment) {}
    virtual bool isParamGeneric() override { return true; }
    virtual bool isGenericDep() override { return true; }
    virtual void setObjValue(GenObject *v) override { objValue_ = v; }
};

class DefParamUI64H : public ParamUI64H {
 public:
    DefParamUI64H(GenObject *parent, const char *name, const char *val,
                const char *comment="")
                : ParamUI64H(parent, name, val, comment) {}
    virtual bool isParamGeneric() override { return true; }
    virtual bool isGenericDep() override { return true; }
    virtual void setObjValue(GenObject *v) override { objValue_ = v; }
};

class DefParamLogic : public ParamLogic {
 public:
    DefParamLogic(GenObject *parent,
                  const char *name,
                  const char *width,
                  const char *val,
                  const char *comment)
        : ParamLogic(parent, name, width, val, comment) {}

    virtual bool isParamGeneric() override { return true; }
    virtual bool isGenericDep() override { return true; }
    virtual void setObjValue(GenObject *v) override { objValue_ = v; }
};

class DefParamTIMESEC : public ParamTIMESEC {
 public:
    DefParamTIMESEC(GenObject *parent, const char *name, const char *val,
                const char *comment="")
                : ParamTIMESEC(parent, name, val, comment) {}
    virtual bool isParamGeneric() override { return true; }
    virtual bool isGenericDep() override { return true; }
    virtual void setObjValue(GenObject *v) override { objValue_ = v; }
};

/** Template paramaters is special key in sytemc. For SV and VHDL it is
  the same generic parameter ID_DEF_PARAM */
class TmplParamI32D : public DefParamI32D {
 public:
    TmplParamI32D(GenObject *parent,
                  const char *name,
                  const char *val,
                  const char *comment="")
        : DefParamI32D(parent, name, val, comment) {}

    virtual bool isParamTemplate() override { return true; }
    virtual void setObjValue(GenObject *v) override { objValue_ = v; }
};

class TmplParamLogic : public DefParamLogic {
 public:
    TmplParamLogic(GenObject *parent,
                   const char *name,
                   const char *width,
                   const char *val,
                   const char *comment)
        : DefParamLogic(parent, name, width, val, comment) { }

    virtual bool isParamTemplate() override { return true; }
    virtual void setObjValue(GenObject *v) override { objValue_ = v; }
};

}  // namespace sysvc
