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
#include "sdctrl_cfg.h"

using namespace sysvc;

class sdctrl_sdmode : public ModuleObject {
 public:
    sdctrl_sdmode(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            v_dat0(this, "v_dat0", "1"),
            v_dat1(this, "v_dat1", "1"),
            v_dat2(this, "v_dat2", "1"),
            v_dat3(this, "v_dat3", "1"),
            vb_cmd_req_arg(this, "vb_cmd_req_arg", "32"),
            v_data_req_ready(this, "v_data_req_ready", "1"),
            v_crc16_next(this, "v_crc16_next", "1") {
        }

     public:
        Logic v_dat0;
        Logic v_dat1;
        Logic v_dat2;
        Logic v_dat3;
        Logic vb_cmd_req_arg;
        Logic v_data_req_ready;
        Logic v_crc16_next;
    };

    void proc_comb();

 public:
    // io:
    InPort i_clk;
    InPort i_nrst;
    InPort i_posedge;
    InPort i_dat0;          // Data0 IO; Data input in SPI mode
    OutPort o_dat0;
    OutPort o_dat0_dir;
    InPort i_dat1;
    OutPort o_dat1;
    OutPort o_dat1_dir;
    InPort i_dat2;
    OutPort o_dat2;
    OutPort o_dat2_dir;
    InPort i_cd_dat3;      // CD/DAT3 IO CardDetect/Data Line 3; CS output in SPI mode
    OutPort o_dat3;
    OutPort o_dat3_dir;
    InPort i_detected;
    InPort i_protect;
    InPort i_cfg_pcie_12V_support;
    InPort i_cfg_pcie_available;
    InPort i_cfg_voltage_supply;
    InPort i_cfg_check_pattern;
    InPort i_cmd_req_ready;
    OutPort o_cmd_req_valid;
    OutPort o_cmd_req_cmd;
    OutPort o_cmd_req_arg;
    OutPort o_cmd_req_rn;
    InPort i_cmd_resp_valid;
    InPort i_cmd_resp_cmd;
    InPort i_cmd_resp_arg32;
    OutPort o_data_req_ready;
    InPort i_data_req_valid;
    InPort i_data_req_write;
    InPort i_data_req_addr;
    InPort i_data_req_wdata;
    OutPort o_data_resp_valid;
    OutPort o_data_resp_rdata;
    InPort i_crc16_0;
    InPort i_crc16_1;
    InPort i_crc16_2;
    InPort i_crc16_3;
    OutPort o_crc16_clear;
    OutPort o_crc16_next;
    OutPort o_wdog_ena;
    InPort i_wdog_trigger;
    InPort i_err_code;
    OutPort o_err_valid;
    OutPort o_err_clear;
    OutPort o_err_code;
    OutPort o_400khz_ena;
    OutPort o_sdtype;

    TextLine _sdstate0_;
    ParamLogic SDSTATE_IDLE;
    ParamLogic SDSTATE_READY;
    ParamLogic SDSTATE_IDENT;
    ParamLogic SDSTATE_STBY;
    ParamLogic SDSTATE_TRAN;
    ParamLogic SDSTATE_DATA;
    ParamLogic SDSTATE_RCV;
    ParamLogic SDSTATE_PRG;
    ParamLogic SDSTATE_DIS;
    ParamLogic SDSTATE_INA;
    TextLine _idlestate0_;
    ParamLogic IDLESTATE_CMD0;
    ParamLogic IDLESTATE_CMD8;
    ParamLogic IDLESTATE_CMD55;
    ParamLogic IDLESTATE_ACMD41;
    ParamLogic IDLESTATE_CARD_IDENTIFICATION;
    TextLine _readystate0_;
    ParamLogic READYSTATE_CMD11;
    ParamLogic READYSTATE_CMD2;
    ParamLogic READYSTATE_CHECK_CID;
    TextLine _identstate0_;
    ParamLogic IDENTSTATE_CMD3;
    ParamLogic IDENTSTATE_CHECK_RCA;

    RegSignal clkcnt;
    RegSignal cmd_req_valid;
    RegSignal cmd_req_cmd;
    RegSignal cmd_req_arg;
    RegSignal cmd_req_rn;
    RegSignal cmd_resp_cmd;
    RegSignal cmd_resp_arg32;
    RegSignal data_addr;
    RegSignal data_data;
    RegSignal data_resp_valid;
    RegSignal wdog_ena;
    RegSignal crc16_clear;
    RegSignal crc16_calc0;
    RegSignal crc16_calc1;
    RegSignal crc16_calc2;
    RegSignal crc16_calc3;
    RegSignal crc16_rx0;
    RegSignal crc16_rx1;
    RegSignal crc16_rx2;
    RegSignal crc16_rx3;
    RegSignal dat_full_ena;
    RegSignal dat_csn;
    RegSignal err_clear;
    RegSignal err_valid;
    RegSignal err_code;
    RegSignal sck_400khz_ena;

    RegSignal sdstate;
    RegSignal idlestate;
    RegSignal readystate;
    RegSignal identstate;
    RegSignal wait_cmd_resp;
    RegSignal sdtype;
    RegSignal HCS;
    RegSignal S18;
    RegSignal OCR_VoltageWindow;
    RegSignal bitcnt;

    CombProcess comb;
};

class sdctrl_sdmode_file : public FileObject {
 public:
    sdctrl_sdmode_file(GenObject *parent) :
        FileObject(parent, "sdctrl_sdmode"),
        sdctrl_sdmode_(this, "sdctrl_sdmode") {}

 private:
    sdctrl_sdmode sdctrl_sdmode_;
};

