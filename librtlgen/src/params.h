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
#include <iostream>

namespace sysvc {

class ParamGeneric {
 public:
    // if local = false, then register parameter using method SCV_..
    ParamGeneric(GenValue *parent);

    virtual std::string genparam(GenValue *p);
};

class ParamBOOL : public BOOL,
                  public ParamGeneric {
 public:
    ParamBOOL(GenObject *parent, const char *name,
                const char *val, const char *comment="")
        : BOOL(val, name, parent, comment),
        ParamGeneric(static_cast<GenValue *>(this)) {
        id_ = ID_PARAM;
    }
    virtual std::string generate() override {
        return genparam(static_cast<GenValue *>(this));
    }
};

class ParamString : public STRING,
                    public ParamGeneric {
 public:
    ParamString(GenObject *parent, const char *name,
                const char *val, const char *comment="")
        : STRING(val, name, parent, comment),
        ParamGeneric(static_cast<GenValue *>(this)) {
        id_ = ID_PARAM;
    }
    virtual std::string generate() override {
        return genparam(static_cast<GenValue *>(this));
    }
};

class TmplParamI32D : public I32D,
                      public ParamGeneric {
 public:
    TmplParamI32D(GenObject *parent, const char *name, const char *val,
                  const char *comment="")
        : I32D(val, name, parent, comment),
        ParamGeneric(static_cast<GenValue *>(this)) {
        id_ = ID_TMPL_PARAM;
    }
    virtual std::string generate() override {
        return genparam(static_cast<GenValue *>(this));
    }
};

class ParamUI16D : public UI16D,
                   public ParamGeneric {
 public:
    ParamUI16D(GenObject *parent, const char *name, const char *val,
        const char *comment="")
        : UI16D(val, name, parent, comment),
        ParamGeneric(static_cast<GenValue *>(this)) {
            id_ = ID_PARAM;
        }
    virtual std::string generate() override {
        return genparam(static_cast<GenValue *>(this));
    }
};

class ParamI32D : public I32D,
                  public ParamGeneric {
 public:
    ParamI32D(GenObject *parent, const char *name, const char *val,
        const char *comment="")
        : I32D(val, name, parent, comment),
        ParamGeneric(static_cast<GenValue *>(this)) {
            id_ = ID_PARAM;
        }
    ParamI32D(GenObject *parent, const char *name, GenObject *val,
        const char *comment="")
        : I32D(val, name, parent, comment),
        ParamGeneric(static_cast<GenValue *>(this)) {
            id_ = ID_PARAM;
        }
    virtual std::string generate() override {
        return genparam(static_cast<GenValue *>(this));
    }
};


class ParamUI32D : public UI32D,
                   public ParamGeneric {
 public:
    ParamUI32D(GenObject *parent, const char *name, const char *val,
        const char *comment="")
        : UI32D(val, name, parent, comment),
        ParamGeneric(static_cast<GenValue *>(this)) {
            id_ = ID_PARAM;
        }
    virtual std::string generate() override {
        return genparam(static_cast<GenValue *>(this));
    }
};

class ParamUI64H : public UI64H,
                   public ParamGeneric {
 public:
    ParamUI64H(GenObject *parent, const char *name, const char *val,
        const char *comment="")
        : UI64H(val, name, parent, comment),
        ParamGeneric(static_cast<GenValue *>(this)) {
            id_ = ID_PARAM;
        }
    virtual std::string generate() override {
        return genparam(static_cast<GenValue *>(this));
    }
};

class ParamLogic : public Logic,
                   public ParamGeneric {
 public:
    ParamLogic(GenObject *parent, const char *width, const char *name,
                const char *val, const char *comment="")
        : Logic(width, name, val, parent, comment),
        ParamGeneric(static_cast<GenValue *>(this)) {
        id_ = ID_PARAM;
    }
    ParamLogic(GenObject *parent, GenValue *width, const char *name,
               const char *val, const char *comment="")
        : Logic(width, name, val, parent, comment),
        ParamGeneric(static_cast<GenValue *>(this)) {
        id_ = ID_PARAM;
    }
    virtual std::string generate() override {
        return genparam(static_cast<GenValue *>(this));
    }
};

class TmplParamLogic : public ParamLogic {
 public:
    TmplParamLogic(GenObject *parent, const char *width, const char *name,
                   const char *val, const char *comment="")
        : ParamLogic(parent, width, name, val, comment) {
        id_ = ID_TMPL_PARAM;
    }
    virtual std::string generate() override {
        return genparam(static_cast<GenValue *>(this));
    }
};


}  // namespace sysvc
