// 
//  Copyright 2023 Sergey Khabarov, sergeykhbr@gmail.com
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

#include <api.h>

using namespace sysvc;

class sdctrl_cfg : public FileObject {
 public:
    sdctrl_cfg(GenObject *parent);

 public:
    TextLine _cmd0_;
    TextLine _cmd1_;
    ParamLogic CMD0;
    TextLine _cmd2_;
    ParamLogic R1;
    ParamLogic R2;
    TextLine _cmd3_;
    ParamLogic DIR_OUTPUT;
    ParamLogic DIR_INPUT;
    TextLine _err0_;
    ParamLogic CMDERR_NONE;
    ParamLogic CMDERR_NO_RESPONSE;
    ParamLogic CMDERR_WRONG_RESPONSE;
    TextLine _n_;
};

extern sdctrl_cfg *sdctrl_cfg_;

