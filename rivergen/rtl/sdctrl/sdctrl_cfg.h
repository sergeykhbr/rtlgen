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
    ParamLogic CMD2;
    ParamLogic CMD3;
    ParamLogic CMD8;
    ParamLogic CMD11;
    ParamLogic ACMD41;
    ParamLogic CMD55;
    ParamLogic CMD58;
    TextLine _cmd2_;
    ParamLogic R1;
    TextLine _r20_;
    TextLine _r21_;
    TextLine _r22_;
    TextLine _r23_;
    TextLine _r24_;
    TextLine _r25_;
    TextLine _r26_;
    TextLine _r27_;
    TextLine _r28_;
    TextLine _r29_;
    TextLine _r2a_;
    TextLine _r2b_;
    TextLine _r2c_;
    ParamLogic R2;
    TextLine _r30_;
    TextLine _r31_;
    TextLine _r32_;
    TextLine _r33_;
    TextLine _r34_;
    TextLine _r35_;
    TextLine _r36_;
    ParamLogic R3;
    TextLine _r60_;
    TextLine _r61_;
    TextLine _r62_;
    TextLine _r63_;
    TextLine _r64_;
    TextLine _r65_;
    TextLine _r66_;
    TextLine _r67_;
    ParamLogic R6;
    TextLine _r70_;
    TextLine _r71_;
    TextLine _r72_;
    TextLine _r73_;
    TextLine _r74_;
    TextLine _r75_;
    TextLine _r76_;
    TextLine _r77_;
    TextLine _r78_;
    TextLine _r79_;
    TextLine _r710_;
    ParamLogic R7;
    TextLine _cmd3_;
    ParamLogic DIR_OUTPUT;
    ParamLogic DIR_INPUT;
    TextLine _sd0_;
    TextLine _sd1_;
    ParamLogic SDCARD_UNKNOWN;
    ParamLogic SDCARD_VER1X;
    ParamLogic SDCARD_VER2X_SC;
    ParamLogic SDCARD_VER2X_HC;
    ParamLogic SDCARD_UNUSABLE;
    TextLine _err0_;
    ParamLogic CMDERR_NONE;
    ParamLogic CMDERR_NO_RESPONSE;
    ParamLogic CMDERR_WRONG_RESP_STARTBIT;
    ParamLogic CMDERR_WRONG_RESP_STOPBIT;
    TextLine _n_;
};

extern sdctrl_cfg *sdctrl_cfg_;

