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

#include "sdctrl_cfg.h"

sdctrl_cfg *sdctrl_cfg_ = 0;

sdctrl_cfg::sdctrl_cfg(GenObject *parent) :
    FileObject(parent, "sdctrl_cfg"),
    _cmd0_(this, ""),
    _cmd1_(this, ""),
    CMD0(this, "6", "CMD0", "0", "GO_IDLE_STATE: Reset card to idle state. Response - (4.7.4)"),
    CMD8(this, "6", "CMD8", "8", "SEND_IF_COND: Card interface condition. Response R7 (4.9.6)."),
    _cmd2_(this, ""),
    R1(this, "3", "R1", "1"),
    R2(this, "3", "R2", "2"),
    R3(this, "3", "R3", "3"),
    R6(this, "3", "R6", "6"),
    _cmd3_(this, ""),
    DIR_OUTPUT(this, "1", "DIR_OUTPUT", "0"),
    DIR_INPUT(this, "1", "DIR_INPUT", "1"),
    _err0_(this, ""),
    CMDERR_NONE(this, "4", "CMDERR_NONE", "0"),
    CMDERR_NO_RESPONSE(this, "4", "CMDERR_NO_RESPONSE", "1"),
    CMDERR_WRONG_RESP_STARTBIT(this, "4", "CMDERR_WRONG_RESP_STARTBIT", "2"),
    CMDERR_WRONG_RESP_STOPBIT(this, "4", "CMDERR_WRONG_RESP_STOPBIT", "3"),

    _n_(this)
{
    sdctrl_cfg_ = this;
}

