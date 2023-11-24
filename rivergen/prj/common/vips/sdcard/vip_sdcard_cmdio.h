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

#include <api_rtlgen.h>
#include "../../../../rtl/techmap/bufg/iobuf_tech.h"
#include "vip_sdcard_crc7.h"

using namespace sysvc;

class vip_sdcard_cmdio : public ModuleObject {
 public:
    vip_sdcard_cmdio(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            vb_cmd_txshift(this, "vb_cmd_txshift", "48"),
            v_crc7_clear(this, "v_crc7_clear", "1"),
            v_crc7_next(this, "v_crc7_next", "1"),
            v_crc7_in(this, "v_crc7_in", "1"),
            v_busy(this, "v_busy", "1", "1") {
        }

     public:
        Logic vb_cmd_txshift;
        Logic v_crc7_clear;
        Logic v_crc7_next;
        Logic v_crc7_in;
        Logic v_busy;
    };

    void proc_comb();

 public:
    // io:
    InPort i_nrst;           // to avoid xxx after start
    InPort i_clk;
    InPort i_cs;
    OutPort o_spi_mode;
    InPort i_cmd;
    OutPort o_cmd;
    OutPort o_cmd_dir;
    OutPort o_cmd_req_valid;
    OutPort o_cmd_req_cmd;
    OutPort o_cmd_req_data;
    InPort i_cmd_req_ready;
    InPort i_cmd_resp_valid;
    InPort i_cmd_resp_data32;
    OutPort o_cmd_resp_ready;
    InPort i_cmd_resp_r1b;
    InPort i_cmd_resp_r2;
    InPort i_cmd_resp_r3;
    InPort i_cmd_resp_r7;
    InPort i_stat_idle_state;
    InPort i_stat_erase_reset;
    InPort i_stat_illegal_cmd;
    InPort i_stat_err_erase_sequence;
    InPort i_stat_err_address;
    InPort i_stat_err_parameter;
    InPort i_stat_locked;
    InPort i_stat_wp_erase_skip;
    InPort i_stat_err;
    InPort i_stat_err_cc;
    InPort i_stat_ecc_failed;
    InPort i_stat_wp_violation;
    InPort i_stat_erase_param;
    InPort i_stat_out_of_range;
    OutPort o_busy;

    // param
    TextLine _cmdstate0_;
    TextLine _cmdstate1_;
    ParamLogic CMDSTATE_REQ_STARTBIT;
    ParamLogic CMDSTATE_REQ_TXBIT;
    ParamLogic CMDSTATE_REQ_CMD;
    ParamLogic CMDSTATE_REQ_ARG;
    ParamLogic CMDSTATE_REQ_CRC7;
    ParamLogic CMDSTATE_REQ_STOPBIT;
    ParamLogic CMDSTATE_REQ_VALID;
    ParamLogic CMDSTATE_WAIT_RESP;
    ParamLogic CMDSTATE_RESP;
    ParamLogic CMDSTATE_RESP_CRC7;
    ParamLogic CMDSTATE_RESP_STOPBIT;
    ParamLogic CMDSTATE_INIT;
    // signals
    Signal w_cmd_out;
    Signal w_crc7_clear;
    Signal w_crc7_next;
    Signal w_crc7_dat;
    Signal wb_crc7;

    RegSignal clkcnt;
    RegSignal cs;
    RegSignal spi_mode;
    RegSignal cmdz;
    RegSignal cmd_dir;
    RegSignal cmd_rxshift;
    RegSignal cmd_txshift;
    RegSignal cmd_state;
    RegSignal cmd_req_crc_err;
    RegSignal bitcnt;
    RegSignal txbit;
    RegSignal crc_calc;
    RegSignal crc_rx;
    RegSignal cmd_req_valid;
    RegSignal cmd_req_cmd;
    RegSignal cmd_req_data;
    RegSignal cmd_resp_ready;

    CombProcess comb;
    vip_sdcard_crc7 crccmd0;
};

class vip_sdcard_cmdio_file : public FileObject {
 public:
    vip_sdcard_cmdio_file(GenObject *parent) :
        FileObject(parent, "vip_sdcard_cmdio"),
        vip_sdcard_cmdio_(this, "") {}

 private:
    vip_sdcard_cmdio vip_sdcard_cmdio_;
};

