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
    Logic(const char *width="1",
          const char *name="",
          const char *val="",
          GenObject *parent=0,
          const char *comment="");

    Logic(GenValue *width,
          const char *name="",
          const char *val="",
          GenObject *parent=0,
          const char *comment="");

    Logic(GenObject *parent,
          const char *name,
          const char *width,
          const char *comment="") :
        Logic(width, name, "", parent, comment) {}

    virtual std::string getType();
    virtual std::string getStrValue() override;
};

class Logic1 : public Logic {
 public:
    Logic1(const char *name="",
          const char *val="",
          GenObject *parent=0,
          const char *comment="") :
        Logic("1", name, val, parent, comment) {}

    Logic1(GenObject *parent,
          const char *name,
          const char *comment="") :
        Logic("1", name, "", parent, comment) {}

    std::string getType() override;
};

}  // namespace sysvc
