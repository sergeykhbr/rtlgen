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

#include <api.h>
#include "../ambalib/types_amba.h"
#include "../ambalib/types_pnp.h"
#include "../ambalib/axi_slv.h"
#include "sdctrl_cfg.h"
#include "sdctrl_regs.h"
#include "sdctrl_crc7.h"
#include "sdctrl_crc16.h"
#include "sdctrl_cmd_transmitter.h"

using namespace sysvc;

class sdctrl : public ModuleObject {
 public:
    sdctrl(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            v_crc16_next(this, "v_crc16_next", "1"),
            vb_cmd_req_arg(this, "vb_cmd_req_arg", "32"),
            v_cmd_resp_ready(this, "v_cmd_resp_ready", "1"),
            v_clear_cmderr(this, "v_clear_cmderr", "1") {
        }

     public:
        Logic v_crc16_next;
        Logic vb_cmd_req_arg;
        Logic v_cmd_resp_ready;
        Logic v_clear_cmderr;
    };

    void proc_comb();

 public:
    ParamI32D log2_fifosz;
    ParamI32D fifo_dbits;
    // io:
    InPort i_clk;
    InPort i_nrst;
    InStruct<types_amba::mapinfo_type> i_xmapinfo;
    OutStruct<types_pnp::dev_config_type> o_xcfg;
    InStruct<types_amba::axi4_slave_in_type> i_xslvi;
    OutStruct<types_amba::axi4_slave_out_type> o_xslvo;
    InStruct<types_amba::mapinfo_type> i_pmapinfo;
    OutStruct<types_pnp::dev_config_type> o_pcfg;
    InStruct<types_amba::apb_in_type> i_apbi;
    OutStruct<types_amba::apb_out_type> o_apbo;
    OutPort o_sclk;
    InPort i_cmd;           // CMD IO Command/Resonse; Data output in SPI mode
    OutPort o_cmd;
    OutPort o_cmd_dir;
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
    OutPort o_cd_dat3;
    OutPort o_cd_dat3_dir;
    InPort i_detected;
    InPort i_protect;
    
    TextLine _sdstate0_;
    ParamLogic SDSTATE_PRE_INIT;
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

    Signal w_regs_sck_posedge;
    Signal w_regs_sck_negedge;
    Signal w_regs_clear_cmderr;
    Signal wb_regs_watchdog;
    Signal w_regs_pcie_12V_support;
    Signal w_regs_pcie_available;
    Signal wb_regs_voltage_supply;
    Signal wb_regs_check_pattern;
    Signal w_mem_req_valid;
    Signal wb_mem_req_addr;
    Signal wb_mem_req_size;
    Signal w_mem_req_write;
    Signal wb_mem_req_wdata;
    Signal wb_mem_req_wstrb;
    Signal w_mem_req_last;
    Signal w_mem_req_ready;
    Signal w_mem_resp_valid;
    Signal wb_mem_resp_rdata;
    Signal wb_mem_resp_err;

    Signal w_cmd_req_ready;
    Signal w_cmd_resp_valid;
    Signal wb_cmd_resp_cmd;
    Signal wb_cmd_resp_reg;
    Signal wb_cmd_resp_crc7_rx;
    Signal wb_cmd_resp_crc7_calc;
    Signal w_cmd_resp_ready;
    Signal wb_trx_cmdstate;
    Signal wb_trx_cmderr;
    Signal w_clear_cmderr;
    Signal w_400kHz_ena;
    
    Signal w_crc7_clear;
    Signal w_crc7_next;
    Signal w_crc7_dat;
    Signal wb_crc7;
    Signal w_crc16_next;
    Signal wb_crc16_dat;
    Signal wb_crc16;

    RegSignal clkcnt;
    RegSignal cmd_set_low;
    RegSignal cmd_req_valid;
    RegSignal cmd_req_cmd;
    RegSignal cmd_req_arg;
    RegSignal cmd_req_rn;
    RegSignal cmd_resp_cmd;
    RegSignal cmd_resp_reg;

    RegSignal crc16_clear;
    RegSignal dat;
    RegSignal dat_dir;

    RegSignal sdstate;
    RegSignal idlestate;
    RegSignal readystate;
    RegSignal identstate;
    RegSignal wait_cmd_resp;
    RegSignal sdtype;
    RegSignal HCS;
    RegSignal S18;
    RegSignal RCA;
    RegSignal OCR_VoltageWindow;

    CombProcess comb;

    axi_slv xslv0;
    sdctrl_regs regs0;
    sdctrl_crc7 crccmd0;
    sdctrl_crc16 crcdat0;
    sdctrl_cmd_transmitter cmdtrx0;
};

class sdctrl_file : public FileObject {
 public:
    sdctrl_file(GenObject *parent) :
        FileObject(parent, "sdctrl"),
        sdctrl_(this, "") {}

 private:
    sdctrl sdctrl_;
};

