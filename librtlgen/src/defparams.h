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
#include "utils.h"

namespace sysvc {

template<class T>
class DefParamType : public T {
 public:
    DefParamType(GenObject *parent,
              const char *name,
              const char *val,
              const char *comment) : T(parent, name, val, comment) {}

    DefParamType(GenObject *parent,
              const char *name,
              GenObject *val,
              const char *comment) : T(parent, name, val, comment) {}

    virtual bool isParamGeneric() override { return true; }
    virtual bool isGenericDep() override { return true; }
    virtual void setObjValue(GenObject *v) override { T::objValue_ = v; }
    std::string nameInModule(EPorts portid, bool sc_read) override {
        std::string ret = T::nameInModule(portid, sc_read);
        if (SCV_is_sysc() && !this->isParamTemplate()) {
            ret += "_";
        }
        return ret;
    }
};


class DefParamBOOL : public DefParamType<ParamBOOL> {
 public:
    DefParamBOOL(GenObject *parent,
                 const char *name,
                 const char *val,
                 const char *comment=NO_COMMENT)
        : DefParamType<ParamBOOL>(parent, name, val, comment) {}
};

class DefParamString : public DefParamType<ParamString> {
 public:
    DefParamString(GenObject *parent,
                   const char *name,
                   const char *val,
                   const char *comment=NO_COMMENT)
        : DefParamType<ParamString>(parent, name, val, comment) {}
};

class DefParamUI16D : public DefParamType<ParamUI16D> {
 public:
    DefParamUI16D(GenObject *parent,
                  const char *name,
                  const char *val,
                  const char *comment=NO_COMMENT)
        : DefParamType<ParamUI16D>(parent, name, val, comment) {}
};

class DefParamI32D : public DefParamType<ParamI32D> {
 public:
    DefParamI32D(GenObject *parent,
                 const char *name,
                 const char *val,
                 const char *comment=NO_COMMENT)
        : DefParamType<ParamI32D>(parent, name, val, comment) {}
};

class DefParamUI32D : public DefParamType<ParamUI32D> {
 public:
    DefParamUI32D(GenObject *parent,
                  const char *name,
                  const char *val,
                  const char *comment=NO_COMMENT)
                : DefParamType<ParamUI32D>(parent, name, val, comment) {}
};

class DefParamUI64H : public DefParamType<ParamUI64H> {
 public:
    DefParamUI64H(GenObject *parent,
                  const char *name,
                  const char *val,
                  const char *comment=NO_COMMENT)
        : DefParamType<ParamUI64H>(parent, name, val, comment) {}
};

class DefParamTIMESEC : public DefParamType<ParamTIMESEC> {
 public:
    DefParamTIMESEC(GenObject *parent,
                    const char *name,
                    const char *val,
                    const char *comment=NO_COMMENT)
        : DefParamType<ParamTIMESEC>(parent, name, val, comment) {}
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
    std::string nameInModule(EPorts portid, bool sc_read) override {
        std::string ret = ParamLogic::nameInModule(portid, sc_read);
        if (SCV_is_sysc() && !isParamTemplate()) {
            ret += "_";
        }
        return ret;
    }
};

/** Template paramaters is a special keys in sytemc. For SV and VHDL it is
  the same generic parameter DefParam */
class TmplParamI32D : public DefParamI32D {
 public:
    TmplParamI32D(GenObject *parent,
                  const char *name,
                  const char *val,
                  const char *comment)
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
