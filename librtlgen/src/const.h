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

namespace sysvc {

class ConstObject : public GenObject {
 public:
    ConstObject(GenObject *parent, const char *comment)
        : GenObject(NO_PARENT, comment) {}

    virtual bool isConst() override { return true; }
    virtual std::string getName() override { return getStrValue(); }
    std::string nameInModule(EPorts portid, bool no_sc_read) override;
};


class DecConst : public ConstObject {
 public:
    DecConst(uint64_t v, const char *comment=NO_COMMENT)
        : ConstObject(NO_PARENT, comment), ui64_(v) {}
    DecConst(int v, const char *comment=NO_COMMENT)
        : ConstObject(NO_PARENT, comment), ui64_(static_cast<uint64_t>(v)) {}

    virtual uint64_t getValue() override { return ui64_; }
    virtual std::string getStrValue() override;
    virtual uint64_t getWidth() override { return 32; }

 protected:
    uint64_t ui64_;
};

class HexConst : public ConstObject {
 public:
    HexConst(uint64_t v) : ConstObject(NO_PARENT, NO_COMMENT), ui64_(v) {}

    virtual uint64_t getValue() override { return ui64_; }
    virtual std::string getStrValue() override;
    virtual uint64_t getWidth() override { return 64; }

 protected:
    uint64_t ui64_;
};

class FloatConst : public ConstObject {
 public:
    FloatConst(double v)
        : ConstObject(NO_PARENT, NO_COMMENT), f64_(v) {}

    virtual bool isFloat() override { return true; }
    virtual double getFloatValue() override { return f64_; }
    virtual std::string getStrValue() override;
    virtual uint64_t getWidth() override { return 64; }
    virtual std::string generate() override { return getStrValue(); }

 protected:
    double f64_;
};

// TODO: StringConst and StringVariable
class StringConst : public ConstObject {
 public:
    StringConst(const char *val, const char *comment=NO_COMMENT);

    virtual bool isString() override { return true; }
    virtual std::string getStrValue() override { return strval_; }

 protected:
    std::string strval_;
};

class DecLogicConst : public DecConst {
 public:
    DecLogicConst(GenObject *width, uint64_t val);

    virtual bool isLogic() override { return true; }
    virtual std::string getStrValue() override;
    virtual uint64_t getWidth() override { return objWidth_->getValue(); }
    virtual std::string getStrWidth() override { return objWidth_->getStrValue(); }

 protected:
    GenObject *objWidth_;
};

/**
    Special case of DecLogicConst to use in INC()/DEC() operation,
    in this case no need to output buswidth in SV, just 1 instead of 32'd1
 */
class DecLogicConstOne : public DecLogicConst {
 public:
    DecLogicConstOne(GenObject *width, uint64_t val)
        : DecLogicConst(width, val) {}

    virtual std::string getStrValue() override;
};

class HexLogicConst : public HexConst {
 public:
    HexLogicConst(GenObject *width, uint64_t val);

    virtual bool isLogic() override { return true; }
    virtual std::string getStrValue() override;
    virtual uint64_t getWidth() override { return objWidth_->getValue(); }
    virtual std::string getStrWidth() override { return objWidth_->getStrValue(); }

 protected:
    GenObject *objWidth_;
};


}  // namespace sysvc
