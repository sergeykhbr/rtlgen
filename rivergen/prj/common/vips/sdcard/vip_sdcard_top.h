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
#include "vip_sdcard_crc7.h"

using namespace sysvc;

class vip_sdcard_top : public ModuleObject {
 public:
    vip_sdcard_top(GenObject *parent, const char *name);

    virtual GenObject *getClockPort() override { return &i_sclk; }

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            vb_cmd_txshift(this, "vb_cmd_txshift", "48"),
            v_crc7_clear(this, "v_crc7_clear", "1"),
            v_crc7_next(this, "v_crc7_next", "1"),
            v_crc7_in(this, "v_crc7_in", "1") {
        }

     public:
        Logic vb_cmd_txshift;
        Logic v_crc7_clear;
        Logic v_crc7_next;
        Logic v_crc7_in;
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
    TextLine _cfg0_;
    ParamI32D CFG_SDCARD_POWERUP_DONE_DELAY;
    ParamLogic CFG_SDCARD_VHS;
    ParamLogic CFG_SDCARD_PCIE_1_2V;
    ParamLogic CFG_SDCARD_PCIE_AVAIL;
    ParamLogic CFG_SDCARD_VDD_VOLTAGE_WINDOW;
    TextLine _cmdstate0_;
    TextLine _cmdstate1_;
    ParamLogic CMDSTATE_IDLE;
    ParamLogic CMDSTATE_REQ_STARTBIT;
    ParamLogic CMDSTATE_REQ_CMD;
    ParamLogic CMDSTATE_REQ_ARG;
    ParamLogic CMDSTATE_REQ_CRC7;
    ParamLogic CMDSTATE_REQ_STOPBIT;
    ParamLogic CMDSTATE_WAIT_RESP;
    ParamLogic CMDSTATE_RESP;
    ParamLogic CMDSTATE_RESP_CRC7;
    // signals
    Signal w_clk;
    Signal wb_rdata;
    Signal w_cmd_in;
    Signal w_cmd_out;
    Signal w_crc7_clear;
    Signal w_crc7_next;
    Signal w_crc7_dat;
    Signal wb_crc7;

    RegSignal cmd_dir;
    RegSignal cmd_rxshift;
    RegSignal cmd_txshift;
    RegSignal cmd_state;
    RegSignal bitcnt;
    RegSignal powerup_cnt;
    RegSignal powerup_done;

    CombProcess comb;

    iobuf_tech iobufcmd0;
    vip_sdcard_crc7 crccmd0;
};

class vip_sdcard_top_file : public FileObject {
 public:
    vip_sdcard_top_file(GenObject *parent) :
        FileObject(parent, "vip_sdcard_top"),
        vip_sdcard_top_(this, "") {}

 private:
    vip_sdcard_top vip_sdcard_top_;
};

