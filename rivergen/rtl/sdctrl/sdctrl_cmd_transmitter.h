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

#include <api_rtlgen.h>
#include "sdctrl_crc7.h"

using namespace sysvc;

class sdctrl_cmd_transmitter : public ModuleObject {
 public:
    sdctrl_cmd_transmitter(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            v_req_ready(this, "v_req_ready", "1"),
            vb_cmdshift(this, "vb_cmdshift", "48"),
            vb_resp_spistatus(this, "vb_resp_spistatus", "15"),
            v_crc7_dat(this, "v_crc7_dat", "1"),
            v_crc7_next(this, "v_crc7_next", "1") {
        }

     public:
        Logic v_req_ready;
        Logic vb_cmdshift;
        Logic vb_resp_spistatus;
        Logic v_crc7_dat;
        Logic v_crc7_next;
    };

    void proc_comb();

 public:
    // io:
    InPort i_clk;
    InPort i_nrst;
    InPort i_sclk_posedge;
    InPort i_sclk_negedge;
    InPort i_cmd;           // CMD IO Command/Resonse; Data output in SPI mode
    OutPort o_cmd;
    OutPort o_cmd_dir;
    OutPort o_cmd_cs;
    InPort i_spi_mode;
    InPort i_err_code;
    InPort i_wdog_trigger;
    InPort i_cmd_set_low;
    InPort i_req_valid;
    InPort i_req_cmd;
    InPort i_req_arg;
    InPort i_req_rn;
    OutPort o_req_ready;
    OutPort o_resp_valid;
    OutPort o_resp_cmd;
    OutPort o_resp_reg;
    OutPort o_resp_crc7_rx;
    OutPort o_resp_crc7_calc;
    OutPort o_resp_spistatus;
    InPort i_resp_ready;
    OutPort o_wdog_ena;
    OutPort o_err_valid;
    OutPort o_err_setcode;
    
    TextLine _cmdstate0_;
    ParamLogic CMDSTATE_IDLE;
    ParamLogic CMDSTATE_REQ_CONTENT;
    ParamLogic CMDSTATE_REQ_CRC7;
    ParamLogic CMDSTATE_REQ_STOPBIT;
    ParamLogic CMDSTATE_RESP_WAIT;
    ParamLogic CMDSTATE_RESP_TRANSBIT;
    ParamLogic CMDSTATE_RESP_CMD_MIRROR;
    ParamLogic CMDSTATE_RESP_REG;
    ParamLogic CMDSTATE_RESP_CID_CSD;
    ParamLogic CMDSTATE_RESP_CRC7;
    ParamLogic CMDSTATE_RESP_STOPBIT;
    ParamLogic CMDSTATE_RESP_SPI_R1;
    ParamLogic CMDSTATE_RESP_SPI_R2;
    ParamLogic CMDSTATE_RESP_SPI_DATA;
    ParamLogic CMDSTATE_PAUSE;

    Signal wb_crc7;
    Signal w_crc7_next;
    Signal w_crc7_dat;

    RegSignal req_cmd;
    RegSignal req_rn;
    RegSignal resp_valid;
    RegSignal resp_cmd;
    RegSignal resp_reg;
    RegSignal resp_spistatus;
    RegSignal cmdshift;
    RegSignal cmdmirror;
    RegSignal regshift;
    RegSignal cidshift;
    RegSignal crc_calc;
    RegSignal crc_rx;
    RegSignal cmdbitcnt;
    RegSignal crc7_clear;
    RegSignal cmdstate;
    RegSignal err_valid;
    RegSignal err_setcode;
    RegSignal cmd_cs;
    RegSignal cmd_dir;
    RegSignal wdog_ena;

    CombProcess comb;

    sdctrl_crc7 crc0;
};

class sdctrl_cmd_transmitter_file : public FileObject {
 public:
    sdctrl_cmd_transmitter_file(GenObject *parent) :
        FileObject(parent, "sdctrl_cmd_transmitter"),
        sdctrl_cmd_transmitter_(this, "") {}

 private:
    sdctrl_cmd_transmitter sdctrl_cmd_transmitter_;
};

