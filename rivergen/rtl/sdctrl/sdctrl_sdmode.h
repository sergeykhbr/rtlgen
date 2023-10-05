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
#include "sdctrl_cfg.h"

using namespace sysvc;

class sdctrl_sdmode : public ModuleObject {
 public:
    sdctrl_sdmode(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            v_crc16_next(this, "v_crc16_next", "1"),
            vb_cmd_req_arg(this, "vb_cmd_req_arg", "32"),
            v_cmd_resp_ready(this, "v_cmd_resp_ready", "1"),
            v_cmd_dir(this, "v_cmd_dir", "1"),
            v_cmd_in(this, "v_cmd_in", "1"),
            v_dat0_dir(this, "v_dat0_dir", "1"),
            v_dat3_dir(this, "v_dat3_dir", "1"),
            v_dat3_out(this, "v_dat3_out", "1"),
            v_clear_cmderr(this, "v_clear_cmderr", "1"),
            v_mem_req_ready(this, "v_mem_req_ready", "1"),
            v_req_sdmem_ready(this, "v_req_sdmem_ready", "1"),
            v_cache_resp_ready(this, "v_cache_resp_ready", "1") {
        }

     public:
        Logic v_crc16_next;
        Logic vb_cmd_req_arg;
        Logic v_cmd_resp_ready;
        Logic v_cmd_dir;
        Logic v_cmd_in;
        Logic v_dat0_dir;
        Logic v_dat3_dir;
        Logic v_dat3_out;
        Logic v_clear_cmderr;
        Logic v_mem_req_ready;
        Logic v_req_sdmem_ready;
        Logic v_cache_resp_ready;
    };

    void proc_comb();

 public:
    // io:
    InPort i_clk;
    InPort i_nrst;
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
    ParamLogic SDSTATE_SPI_DATA;
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
    ParamLogic IDLESTATE_CMD58;
    ParamLogic IDLESTATE_CARD_IDENTIFICATION;
    TextLine _readystate0_;
    ParamLogic READYSTATE_CMD11;
    ParamLogic READYSTATE_CMD2;
    ParamLogic READYSTATE_CHECK_CID;
    TextLine _identstate0_;
    ParamLogic IDENTSTATE_CMD3;
    ParamLogic IDENTSTATE_CHECK_RCA;
    TextLine _spidatastate0_;
    ParamLogic SPIDATASTATE_WAIT_MEM_REQ;
    ParamLogic SPIDATASTATE_CACHE_REQ;
    ParamLogic SPIDATASTATE_CACHE_WAIT_RESP;
    ParamLogic SPIDATASTATE_CMD17_READ_SINGLE_BLOCK;
    ParamLogic SPIDATASTATE_CMD24_WRITE_SINGLE_BLOCK;
    ParamLogic SPIDATASTATE_WAIT_DATA_START;
    ParamLogic SPIDATASTATE_READING_DATA;
    ParamLogic SPIDATASTATE_READING_CRC15;
    ParamLogic SPIDATASTATE_READING_END;

    Signal w_regs_sck_posedge;
    Signal w_regs_sck_negedge;
    Signal w_regs_clear_cmderr;
    Signal wb_regs_watchdog;
    Signal w_regs_spi_mode;
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
    Signal w_cache_req_ready;
    Signal w_cache_resp_valid;
    Signal wb_cache_resp_rdata;
    Signal w_cache_resp_err;
    Signal w_cache_resp_ready;
    Signal w_req_sdmem_ready;
    Signal w_req_sdmem_valid;
    Signal w_req_sdmem_write;
    Signal wb_req_sdmem_addr;
    Signal wb_req_sdmem_wdata;
    Signal w_regs_flush_valid;
    Signal w_cache_flush_end;

    Signal w_trx_cmd_dir;
    Signal w_trx_cmd_cs;
    Signal w_cmd_in;
    Signal w_cmd_req_ready;
    Signal w_cmd_resp_valid;
    Signal wb_cmd_resp_cmd;
    Signal wb_cmd_resp_reg;
    Signal wb_cmd_resp_crc7_rx;
    Signal wb_cmd_resp_crc7_calc;
    Signal wb_cmd_resp_spistatus;
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
    Signal wb_crc16_0;
    Signal wb_crc16_1;
    Signal wb_crc16_2;
    Signal wb_crc16_3;

    RegSignal clkcnt;
    RegSignal cmd_set_low;
    RegSignal cmd_req_valid;
    RegSignal cmd_req_cmd;
    RegSignal cmd_req_arg;
    RegSignal cmd_req_rn;
    RegSignal cmd_resp_cmd;
    RegSignal cmd_resp_reg;
    RegSignal cmd_resp_spistatus;
    RegSignal cache_req_valid;
    RegSignal cache_req_addr;
    RegSignal cache_req_write;
    RegSignal cache_req_wdata;
    RegSignal cache_req_wstrb;
    RegSignal sdmem_addr;
    RegSignal sdmem_data;
    RegSignal sdmem_valid;
    RegSignal sdmem_err;

    RegSignal crc16_clear;
    RegSignal crc16_calc0;
    RegSignal crc16_rx0;
    RegSignal dat;
    RegSignal dat_dir;
    RegSignal dat3_dir;
    RegSignal dat_tran;

    RegSignal sdstate;
    RegSignal idlestate;
    RegSignal readystate;
    RegSignal identstate;
    RegSignal spidatastate;
    RegSignal wait_cmd_resp;
    RegSignal sdtype;
    RegSignal HCS;
    RegSignal S18;
    RegSignal RCA;
    RegSignal OCR_VoltageWindow;
    RegSignal bitcnt;

    CombProcess comb;
};

class sdctrl_sdmode_file : public FileObject {
 public:
    sdctrl_sdmode_file(GenObject *parent) :
        FileObject(parent, "sdctrl_sdmode"),
        sdctrl_sdmode_(this, "") {}

 private:
    sdctrl_sdmode sdctrl_sdmode_;
};

