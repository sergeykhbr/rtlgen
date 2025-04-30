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

class Logic : public GenValue {
 public:
    Logic(GenObject *parent,
          GenObject *clk,
          EClockEdge edge,
          GenObject *nrst,
          EResetActive active,
          const char *name,
          const char *width,
          const char *val,
          const char *comment);

    Logic(GenObject *parent,
          const char *name,
          const char *width,
          const char *val = RSTVAL_ZERO,
          const char *comment = NO_COMMENT);

    virtual bool isLogic() override { return true; }
    virtual std::string getType() override;
    virtual GenObject *getObjWidth() { return objWidth_; }
    virtual uint64_t getWidth() override { return objWidth_->getValue(); }
    virtual std::string getStrWidth() override { return objWidth_->getName(); }
    virtual void setSelector(GenObject *sel) override { objBitidx_ = sel; }           // Set object as an array index
    virtual GenObject *getSelector() override { return objBitidx_; }                  // generate  Name[obj]
    virtual std::string generate() override;

 protected:
    virtual int getMinWidthOfArray() { return 2; }
 protected:
    GenObject *objWidth_;
    GenObject *objBitidx_;      // Bit selector used by Operations, cleared after each operation automatically
};

/**
    Logic class that instantiate sc_uint<1> instead of bool when width = 1,
    in other cases it has absoletly the same behaviour.
 */
class Logic1 : public Logic {
 public:
    Logic1(GenObject *parent,
          GenObject *clk,
          EClockEdge edge,
          GenObject *nrst,
          EResetActive active,
          const char *name,
          const char *width,
          const char *val,
          const char *comment)
        : Logic(parent, clk, edge, nrst, active, name, width, val, comment) {}
    Logic1(GenObject *parent, const char *name, const char *width,
        const char *val, const char *comment)
        : Logic(parent, name, width, val, comment) {}

 protected:
    virtual int getMinWidthOfArray() override { return 1; }
};

// SystemC only use sc_biguint always
class LogicBig : public Logic {
 public:
    LogicBig(GenObject *parent,
          GenObject *clk,
          EClockEdge edge,
          GenObject *nrst,
          EResetActive active,
          const char *name,
          const char *width,
          const char *val,
          const char *comment)
        : Logic(parent, clk, edge, nrst, active, name, width, val, comment) {}
    LogicBig(GenObject *parent,
             const char *name,
             const char *width,
             const char *val,
             const char *comment) :
        Logic(parent, name, width, val, comment) {}

    virtual bool isBigSC() override { return true; }
};

class LogicBv : public Logic {
 public:
    LogicBv(GenObject *parent,
          GenObject *clk,
          EClockEdge edge,
          GenObject *nrst,
          EResetActive active,
          const char *name,
          const char *width,
          const char *val,
          const char *comment)
        : Logic(parent, clk, edge, nrst, active, name, width, val, comment) {}
    LogicBv(GenObject *parent,
             const char *name,
             const char *width,
             const char *val,
             const char *comment) :
        Logic(parent, name, width, val, comment) {}

    virtual bool isBvSC() override { return true; }
};

}  // namespace sysvc
