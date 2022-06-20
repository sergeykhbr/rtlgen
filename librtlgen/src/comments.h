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
#include <iostream>

namespace sysvc {

/**
 * Commentary insertion
 */

class TextLine : public GenObject {
 public:
    TextLine(GenObject *parent, const char *comment);
    TextLine(GenObject *parent);

    virtual std::string getType(EGenerateType v) { return std::string(""); }
    virtual std::string generate(EGenerateType) override;
 protected:
    bool empty_;
};

class CommentLicense : public GenObject {
 public:
    CommentLicense();

    virtual std::string getType(EGenerateType v) { return std::string(""); }
    virtual std::string generate(EGenerateType) override;
 protected:
    TextLine str01;
    TextLine str02;
    TextLine str03;
    TextLine str04;
    TextLine str05;
    TextLine str06;
    TextLine str07;
    TextLine str08;
    TextLine str09;
    TextLine str10;
    TextLine str11;
    TextLine str12;
    TextLine str13;
    TextLine str14;
    TextLine str15;
};

}  // namespace sysvc
