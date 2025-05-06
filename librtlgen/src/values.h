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

#include <inttypes.h>
#include <iostream>
#include <vector>
#include "genobjects.h"
#include "const.h"
#include "utils.h"

namespace sysvc {

class GenValue : public GenObject {
 public:
    GenValue(GenObject *parent,
             GenObject *clk,
             EClockEdge edge,
             GenObject *nrst,
             EResetActive active,
             const char *name,
             const char *val,
             const char *comment);

    GenValue(GenObject *parent,
             const char *name,
             const char *val,
             const char *comment);

    GenValue(GenObject *parent,
             const char *name,
             GenObject *val,
             const char *comment);

    virtual bool isValue() override { return true; }
    virtual std::string generate() override;

    virtual std::string getName() override;
    virtual uint64_t getValue() override {
        return objValue_->getValue();
    }
    virtual double getFloatValue() override { return objValue_->getFloatValue(); }
    virtual std::string getStrValue() override { return objValue_->getName(); }
    virtual GenObject *getObjValue() override { return objValue_; }
    virtual void setObjValue(GenObject *v) override { objValue_ = v; }   // used in BUSx_MAP assignment
    virtual void disableVcd() override { vcd_enabled_ = false; }
    virtual bool isVcd() override { return vcd_enabled_; }

    /** Signal could be a register when it inside of register struct */
    virtual bool is2Dim() override { return getDepth() > 0; }
    virtual GenObject *getClockPort() override { return objClock_; }
    virtual GenObject *getResetPort() { return objReset_; }
    virtual EClockEdge getClockEdge() { return edge_; }
    virtual EResetActive getResetActive() override { return active_; }

    virtual std::string nameInModule(EPorts portid, bool no_sc_read) override;
    virtual std::string v_prefix() override;
    virtual std::string r_prefix() override;

 protected:
    std::string name_;
    GenObject *objValue_;
    GenObject *objClock_;
    GenObject *objReset_;
    EClockEdge edge_;
    EResetActive active_;
    bool vcd_enabled_;          // add/remove variable from the trace (enabled by default)
};

class BOOL : public GenValue {
 public:
    BOOL(GenObject *parent,
         const char *name,
         const char *val,
         const char *comment) : GenValue(parent, name, val, comment) {}

    virtual std::string getType() override;
};

class STRING : public GenValue {
 public:
    STRING(GenObject *parent, 
           const char *name,
           const char *val,
           const char *comment) : GenValue(parent, name, "", comment) {
        objValue_ = new StringConst(val);
    }

    virtual bool isString() override { return true; }
    virtual std::string getType();
};

class FileValue : public GenValue {
 public:
    FileValue(GenObject *parent,
              const char *name,
              const char *val, 
              const char *comment) : GenValue(parent, name, val, comment) {}

    virtual bool isFileValue() override{ return true; }
    virtual std::string getType() override;
};


class UI16D : public GenValue {
 public:
    UI16D(GenObject *parent,
          const char *name,
          const char *val,
          const char *comment) : GenValue(parent, name, val, comment) {}

    virtual std::string getType();
    virtual uint64_t getWidth() override { return 16; }
};

class I32D : public GenValue {
 public:
    I32D(GenObject *parent,
         const char *name,
         const char *val, 
         const char *comment) : GenValue(parent, name, val, comment) {}

    I32D(GenObject *parent,
         const char *name,
         GenObject *val,
         const char *comment) : GenValue(parent, name, val, comment) {}

    virtual std::string getType();
    virtual uint64_t getWidth() override { return 32; }
};

class UI32D : public GenValue {
 public:
    UI32D(GenObject *parent,
          const char *name,
          const char *val,
          const char *comment) : GenValue(parent, name, val, comment) {}

    virtual std::string getType();
    virtual uint64_t getWidth() override { return 32; }
};

class UI64H : public GenValue {
 public:
    UI64H(GenObject *parent,
          const char *name,
          const char *val, 
          const char *comment) : GenValue(parent, name, val, comment) {}

    virtual std::string getType();
    virtual uint64_t getWidth() override { return 64; }
};

class TIMESEC : public GenValue {
 public:
    TIMESEC(GenObject *parent,
            const char *name,
            const char *val,
            const char *comment) : GenValue(parent, name, val, comment) {}

    virtual std::string getType();
    virtual bool isFloat() { return true; }
};

/** Variable used in generate cycle */
class GenVar : public I32D {
 public:
    GenVar(GenObject *parent,
           const char *name,
           const char *val,
           const char *comment) : I32D(parent, name, val, comment) {}

    virtual bool isGenVar() override { return true; }
    virtual std::string getStrValue() override { return getName(); }
};

/** Use this template if you want to assign a constant value to structure META_NONE or similar*/
template<class T>
class StructVar : public T {
 public:
    StructVar(GenObject *parent, const char *name, const char *val, const char *comment)
        : T(parent, name, comment) {
        objValue_ = SCV_parse_to_obj(this, val);
    }
    virtual bool isValue() override { return true; }    // TODO: move to struct: name != type

    virtual std::string getName() override { return this->name_; }
    virtual uint64_t getValue() override { return objValue_->getValue(); }
    virtual double getFloatValue() override { return objValue_->getFloatValue(); }
    virtual std::string getStrValue() override {
        if (objValue_) {
            return objValue_->getName();
        } else {
            return T::getStrValue();
        }
    }
    virtual GenObject *getObjValue() override { return objValue_; }

 protected:
    GenObject *objValue_;
};

}  // namespace sysvc
