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
#include <iostream>

namespace sysvc {

class DefParamBOOL : public BOOL {
 public:
    DefParamBOOL(GenObject *parent, const char *name, const char *val,
                const char *comment="") : BOOL(val, name, parent, comment) {
        id_ = ID_DEF_PARAM;
        SCV_set_cfg_parameter(parent, this, getValue());
    }
    virtual std::string getStrValue() override {
        if (SCV_is_sysc()) {
            SCV_printf("FIXME: template parameter not working with generic parameters");
            return BOOL::getStrValue();
        }
        return getName();
    }
    virtual std::string generate() override { return BOOL::getStrValue(); }
};

class DefParamI32D : public I32D {
 public:
    DefParamI32D(GenObject *parent, const char *name, const char *val,
                const char *comment="") : I32D(val, name, parent, comment) {
        id_ = ID_DEF_PARAM;
        SCV_set_cfg_parameter(parent, this, getValue());
    }
    virtual std::string getStrValue() override {
        if (SCV_is_sysc()) {
            SCV_printf("FIXME: template parameter not working with generic parameters");
            return I32D::getStrValue();
        }
        return getName();
    }
    virtual std::string generate() override { return I32D::getStrValue(); }
};

class DefParamUI32D : public UI32D {
 public:
    DefParamUI32D(GenObject *parent, const char *name, const char *val,
                const char *comment="") : UI32D(val, name, parent, comment) {
        id_ = ID_DEF_PARAM;
        SCV_set_cfg_parameter(parent, this, getValue());
    }
    virtual std::string getStrValue() override {
        if (SCV_is_sysc()) {
            SCV_printf("FIXME: template parameter not working with generic parameters");
            return UI32D::getStrValue();
        }
        return getName();
    }
    virtual std::string generate() override { return UI32D::getStrValue(); }
};

class DefParamUI64H : public UI64H {
 public:
    DefParamUI64H(GenObject *parent, const char *name, const char *val,
                const char *comment="") : UI64H(val, name, parent, comment) {
        id_ = ID_DEF_PARAM;
        SCV_set_cfg_parameter(parent, this, getValue());
    }
    virtual std::string getStrValue() override {
        if (SCV_is_sysc()) {
            SCV_printf("FIXME: template parameter not working with generic parameters");
            return UI64H::getStrValue();
        }
        return getName();
    }
    virtual std::string generate() override { return UI64H::getStrValue(); }
};

class DefParamTIMESEC : public TIMESEC {
 public:
    DefParamTIMESEC(GenObject *parent, const char *name, const char *val,
                const char *comment="") : TIMESEC(val, name, parent, comment) {
        id_ = ID_DEF_PARAM;
        SCV_set_cfg_parameter(parent, this, getValue());
    }
    virtual std::string getStrValue() override {
        if (SCV_is_sysc()) {
            SCV_printf("FIXME: template parameter not working with generic parameters");
            return TIMESEC::getStrValue();
        }
        return getName();
    }
    virtual std::string generate() override { return TIMESEC::getStrValue(); }
};

class DefParamString : public STRING {
 public:
    DefParamString(GenObject *parent, const char *name, const char *val,
                const char *comment="") : STRING(val, name, parent, comment) {
        id_ = ID_DEF_PARAM;
    }
    virtual std::string getStrValue() override {
        if (SCV_is_sysc()) {
            SCV_printf("FIXME: template parameter not working with generic parameters");
            return STRING::getStrValue();
        }
        return getName();
    }
    virtual std::string generate() override { return STRING::getStrValue(); }
};

class DefParamLogic : public Logic {
 public:
    DefParamLogic(GenObject *parent, const char *name, const char *width, const char *val,
                const char *comment="") : Logic(parent, name, width, val, comment) {
        id_ = ID_DEF_PARAM;
    }
    virtual std::string getStrValue() override {
        if (SCV_is_sysc()) {
            SCV_printf("FIXME: template parameter not working with generic parameters");
            return Logic::getStrValue();
        }
        return getName();
    }
    virtual std::string generate() override { return Logic::getStrValue(); }
};

}  // namespace sysvc
