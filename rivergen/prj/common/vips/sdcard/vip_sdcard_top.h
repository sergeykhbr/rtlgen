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
#include "vip_sdcard_cmdio.h"
#include "vip_sdcard_ctrl.h"

using namespace sysvc;

class vip_sdcard_top : public ModuleObject {
 public:
    vip_sdcard_top(GenObject *parent, const char *name, const char *comment);

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
    ParamLogic CFG_SDCARD_HCS;
    ParamLogic CFG_SDCARD_VHS;
    ParamLogic CFG_SDCARD_PCIE_1_2V;
    ParamLogic CFG_SDCARD_PCIE_AVAIL;
    ParamLogic CFG_SDCARD_VDD_VOLTAGE_WINDOW;
    // signals
    Signal w_clk;
    Signal wb_rdata;
    Signal w_spi_mode;
    Signal w_cmd_in;
    Signal w_cmd_out;
    Signal w_cmd_dir;
    Signal w_dat0_in;
    Signal w_dat1_in;
    Signal w_dat2_in;
    Signal w_dat3_in;
    Signal w_dat0_out;
    Signal w_dat1_out;
    Signal w_dat2_out;
    Signal w_dat3_out;
    Signal w_dat0_dir;
    Signal w_dat1_dir;
    Signal w_dat2_dir;
    Signal w_dat3_dir;
    Signal w_cmd_req_valid;
    Signal wb_cmd_req_cmd;
    Signal wb_cmd_req_data;
    Signal w_cmd_req_ready;
    Signal w_cmd_resp_valid;
    Signal wb_cmd_resp_data32;
    Signal w_cmd_resp_ready;
    Signal w_cmd_resp_r1b;
    Signal w_cmd_resp_r2;
    Signal w_cmd_resp_r3;
    Signal w_cmd_resp_r7;
    Signal w_cmdio_cmd_dir;
    Signal w_cmdio_cmd_out;
    TextLine _status0_;
    Signal w_stat_idle_state;
    Signal w_stat_erase_reset;
    Signal w_stat_illegal_cmd;
    Signal w_stat_err_erase_sequence;
    Signal w_stat_err_address;
    Signal w_stat_err_parameter;
    Signal w_stat_locked;
    Signal w_stat_wp_erase_skip;
    Signal w_stat_err;
    Signal w_stat_err_cc;
    Signal w_stat_ecc_failed;
    Signal w_stat_wp_violation;
    Signal w_stat_erase_param;
    Signal w_stat_out_of_range;
    Signal wb_mem_addr;
    Signal wb_mem_rdata;
    Signal w_crc16_clear;
    Signal w_crc16_next;
    Signal wb_crc16;
    Signal w_dat_trans;
    Signal wb_dat;
    Signal w_cmdio_busy;

    CombProcess comb;
    iobuf_tech iobufcmd0;
    iobuf_tech iobufdat0;
    iobuf_tech iobufdat1;
    iobuf_tech iobufdat2;
    iobuf_tech iobufdat3;
    vip_sdcard_cmdio cmdio0;
    vip_sdcard_ctrl ctrl0;
};

class vip_sdcard_top_file : public FileObject {
 public:
    vip_sdcard_top_file(GenObject *parent) :
        FileObject(parent, "vip_sdcard_top"),
        vip_sdcard_top_(this, "vip_sdcard_top", NO_COMMENT) {}

 private:
    vip_sdcard_top vip_sdcard_top_;
};

