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

using namespace sysvc;

class vip_sdcard_ctrl : public ModuleObject {
 public:
    vip_sdcard_ctrl(GenObject *parent, const char *name, const char *comment);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            v_resp_valid(this, "v_resp_valid", "1"),
            vb_resp_data32(this, "vb_resp_data32", "32") {
        }

     public:
        Logic v_resp_valid;
        Logic vb_resp_data32;
    };

    void proc_comb();

 public:
    // generic param
    DefParamI32D CFG_SDCARD_POWERUP_DONE_DELAY;
    DefParamLogic CFG_SDCARD_HCS;
    DefParamLogic CFG_SDCARD_VHS;
    DefParamLogic CFG_SDCARD_PCIE_1_2V;
    DefParamLogic CFG_SDCARD_PCIE_AVAIL;
    DefParamLogic CFG_SDCARD_VDD_VOLTAGE_WINDOW;
    // io:
    InPort i_nrst;
    InPort i_clk;
    InPort i_spi_mode;
    InPort i_cs;
    InPort i_cmd_req_valid;
    InPort i_cmd_req_cmd;
    InPort i_cmd_req_data;
    OutPort o_cmd_req_ready;
    OutPort o_cmd_resp_valid;
    OutPort o_cmd_resp_data32;
    InPort i_cmd_resp_ready;
    OutPort o_cmd_resp_r1b;
    OutPort o_cmd_resp_r2;
    OutPort o_cmd_resp_r3;
    OutPort o_cmd_resp_r7;
    OutPort o_stat_idle_state;
    OutPort o_stat_illegal_cmd;
    OutPort o_mem_addr;
    InPort i_mem_rdata;
    OutPort o_crc16_clear;
    OutPort o_crc16_next;
    InPort i_crc16;
    OutPort o_dat_trans;
    OutPort o_dat;
    InPort i_cmdio_busy;

    // param
    TextLine _sdstate0_;
    TextLine _sdstate1_;
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
    TextLine _sdstate2_;
    TextLine _sdstate3_;
    ParamLogic DATASTATE_IDLE;
    ParamLogic DATASTATE_START;
    ParamLogic DATASTATE_CRC15;
    ParamLogic DATASTATE_STOP;
    // signals

    // registers
    RegSignal sdstate;
    RegSignal datastate;
    RegSignal powerup_cnt;
    RegSignal preinit_cnt;
    RegSignal delay_cnt;
    RegSignal powerup_done;
    RegSignal cmd_req_ready;
    RegSignal cmd_resp_valid;
    RegSignal cmd_resp_valid_delayed;
    RegSignal cmd_resp_data32;
    RegSignal cmd_resp_r1b;
    RegSignal cmd_resp_r2;
    RegSignal cmd_resp_r3;
    RegSignal cmd_resp_r7;
    RegSignal illegal_cmd;
    RegSignal ocr_hcs;
    RegSignal ocr_vdd_window;
    RegSignal req_mem_valid;
    RegSignal req_mem_addr;
    RegSignal shiftdat;
    RegSignal bitcnt;
    RegSignal crc16_clear;
    RegSignal crc16_next;
    RegSignal dat_trans;

    CombProcess comb;
};

class vip_sdcard_ctrl_file : public FileObject {
 public:
    vip_sdcard_ctrl_file(GenObject *parent) :
        FileObject(parent, "vip_sdcard_ctrl"),
        vip_sdcard_ctrl_(this, "", NO_COMMENT) {}

 private:
    vip_sdcard_ctrl vip_sdcard_ctrl_;
};

