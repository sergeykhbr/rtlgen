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

class sdctrl_spimode : public ModuleObject {
 public:
    sdctrl_spimode(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            v_dat(this, "v_dat", "1"),
            vb_cmd_req_arg(this, "vb_cmd_req_arg", "32"),
            v_data_req_ready(this, "v_data_req_ready", "1"),
            v_crc16_next(this, "v_crc16_next", "1") {
            }

     public:
        Logic v_dat;
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
    InPort i_miso;
    OutPort o_mosi;
    OutPort o_csn;
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
    InPort i_cmd_resp_r1r2;
    InPort i_cmd_resp_arg32;
    OutPort o_data_req_ready;
    InPort i_data_req_valid;
    InPort i_data_req_write;
    InPort i_data_req_addr;
    InPort i_data_req_wdata;
    OutPort o_data_resp_valid;
    OutPort o_data_resp_rdata;
    InPort i_crc16_0;
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
    
    TextLine _state0_;
    ParamLogic STATE_CMD0;
    ParamLogic STATE_CMD8;
    ParamLogic STATE_CMD55;
    ParamLogic STATE_ACMD41;
    ParamLogic STATE_CMD58;
    ParamLogic STATE_WAIT_DATA_REQ;
    ParamLogic STATE_CMD17_READ_SINGLE_BLOCK;
    ParamLogic STATE_CMD24_WRITE_SINGLE_BLOCK;
    ParamLogic STATE_WAIT_DATA_START;
    ParamLogic STATE_READING_DATA;
    ParamLogic STATE_READING_CRC15;
    ParamLogic STATE_READING_END;
    
    RegSignal clkcnt;
    RegSignal cmd_req_valid;
    RegSignal cmd_req_cmd;
    RegSignal cmd_req_arg;
    RegSignal cmd_req_rn;
    RegSignal cmd_resp_cmd;
    RegSignal cmd_resp_arg32;
    RegSignal cmd_resp_r1;
    RegSignal cmd_resp_r2;
    RegSignal data_addr;
    RegSignal data_data;
    RegSignal data_resp_valid;
    RegSignal wdog_ena;
    RegSignal crc16_clear;
    RegSignal crc16_calc0;
    RegSignal crc16_rx0;
    RegSignal dat_csn;
    RegSignal dat_reading;
    RegSignal err_clear;
    RegSignal err_valid;
    RegSignal err_code;
    RegSignal sck_400khz_ena;

    RegSignal state;
    RegSignal wait_cmd_resp;
    RegSignal sdtype;
    RegSignal HCS;
    RegSignal S18;
    RegSignal OCR_VoltageWindow;
    RegSignal bitcnt;

    CombProcess comb;
};

class sdctrl_spimode_file : public FileObject {
 public:
    sdctrl_spimode_file(GenObject *parent) :
        FileObject(parent, "sdctrl_spimode"),
        sdctrl_spimode_(this, "") {}

 private:
    sdctrl_spimode sdctrl_spimode_;
};

