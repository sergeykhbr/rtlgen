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
#include "../../../../rtl/techmap/bufg/iobuf_tech.h"

using namespace sysvc;

class vip_sdcard_top : public ModuleObject {
 public:
    vip_sdcard_top(GenObject *parent, const char *name);

    virtual GenObject *getClockPort() override { return &i_sclk; }

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            vb_cmd_txshift(this, "vb_cmd_txshift", "48") {
        }

     public:
        Logic vb_cmd_txshift;
    };

    void proc_comb();

 public:
    // io:
    InPort i_nrst;           // to avoid xxx after start
    InPort i_sclk;
    IoPort io_cmd;           // CMD IO Command/Resonse; Data output in SPI mode
    IoPort io_dat0;          // Data0 IO; Data input in SPI mode
    IoPort io_dat1;
    IoPort io_dat2;
    IoPort io_cd_dat3;       // CD/DAT3 IO CardDetect/Data Line 3; CS output in SPI mode

    // param
    TextLine _cmdstate0_;
    ParamLogic CMDSTATE_IDLE;
    ParamLogic CMDSTATE_REQ_ARG;
    ParamLogic CMDSTATE_REQ_CRC7;
    ParamLogic CMDSTATE_REQ_STOPBIT;
    ParamLogic CMDSTATE_WAIT_RESP;
    ParamLogic CMDSTATE_RESP;
    // signals
    Signal w_clk;
    Signal wb_rdata;
    Signal w_cmd_in;
    Signal w_cmd_out;

    RegSignal cmd_dir;
    RegSignal cmd_rxshift;
    RegSignal cmd_txshift;
    RegSignal cmd_state;
    RegSignal bitcnt;

    CombProcess comb;

    iobuf_tech iobufcmd0;
};

class vip_sdcard_top_file : public FileObject {
 public:
    vip_sdcard_top_file(GenObject *parent) :
        FileObject(parent, "vip_sdcard_top"),
        vip_sdcard_top_(this, "") {}

 private:
    vip_sdcard_top vip_sdcard_top_;
};

