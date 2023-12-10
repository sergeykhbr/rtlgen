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
#include <iostream>

namespace sysvc {

class Param;

class Logic : public GenValue {
 public:
    Logic(const char *width,
          const char *name,
          const char *val="0",
          GenObject *parent = NO_PARENT,
          const char *comment = NO_COMMENT);

    Logic(GenObject *parent,
          const char *name,
          const char *width,
          const char *val="0",
          const char *comment = NO_COMMENT) :
        Logic(width, name, val, parent, comment) {}

//    Logic(GenObject *parent,
//          const char *name,
//          GenObject *width,
//          GenObject *val,
//          const char *comment);

    virtual bool isLogic() override { return true; }
    virtual std::string getType() override;
    virtual GenObject *getObjWidth() { return objWidth_; }
    virtual uint64_t getWidth() override { return objWidth_->getValue(); }
    virtual std::string getStrWidth() override { return objWidth_->getName(); }
    virtual std::string generate() override;

 protected:
    GenObject *objWidth_;
};

class Logic1 : public Logic {
 public:
    Logic1(GenObject *parent, const char *name, const char *width,
        const char *val, const char *comment)
        : Logic(width, name, val, parent, comment) {}

    virtual std::string getType() override;
};

// SystemC only use sc_biguint always
class LogicBig : public Logic {
 public:
    LogicBig(GenObject *parent,
             const char *name,
             const char *width,
             const char *val="'0",
             const char *comment = NO_COMMENT) :
        Logic(width, name, val, parent, comment) {}

    virtual bool isBigSC() override { return true; }
};

class LogicBv : public Logic {
 public:
    LogicBv(GenObject *parent,
             const char *name,
             const char *width,
             const char *val="'0",
             const char *comment = NO_COMMENT) :
        Logic(width, name, val, parent, comment) {}

    virtual bool isBvSC() override { return true; }
};

}  // namespace sysvc
