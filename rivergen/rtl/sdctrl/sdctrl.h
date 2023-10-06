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
#include "sdctrl_crc16.h"
#include "sdctrl_cmd_transmitter.h"
#include "sdctrl_err.h"
#include "sdctrl_wdog.h"
#include "sdctrl_spimode.h"
#include "sdctrl_sdmode.h"
#include "sdctrl_cache.h"

using namespace sysvc;

class sdctrl : public ModuleObject {
 public:
    sdctrl(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            v_cmd_dir(this, "v_cmd_dir", "1", "DIR_OUTPUT"),
            v_cmd_in(this, "v_cmd_in", "1"),
            v_cmd_out(this, "v_cmd_out", "1", "1"),
            v_dat0_dir(this, "v_dat0_dir", "1", "DIR_OUTPUT"),
            v_dat0_out(this, "v_dat0_out", "1", "1"),
            v_dat1_dir(this, "v_dat1_dir", "1", "DIR_OUTPUT"),
            v_dat1_out(this, "v_dat1_out", "1", "1"),
            v_dat2_dir(this, "v_dat2_dir", "1", "DIR_OUTPUT"),
            v_dat2_out(this, "v_dat2_out", "1", "1"),
            v_dat3_dir(this, "v_dat3_dir", "1", "DIR_OUTPUT"),
            v_dat3_out(this, "v_dat3_out", "1", "1"),
            v_cmd_req_valid(this, "v_cmd_req_valid", "1"),
            vb_cmd_req_cmd(this, "vb_cmd_req_cmd", "6"),
            vb_cmd_req_arg(this, "vb_cmd_req_arg", "32"),
            vb_cmd_req_rn(this, "vb_cmd_req_rn", "3"),
            v_req_sdmem_ready(this, "v_req_sdmem_ready", "1"),
            v_resp_sdmem_valid(this, "v_resp_sdmem_valid", "1"),
            vb_resp_sdmem_data(this, "vb_resp_sdmem_data", "512"),
            v_err_valid(this, "v_err_valid", "1"),
            v_err_clear(this, "v_err_clear", "1"),
            vb_err_setcode(this, "vb_err_setcode", "4"),
            v_400kHz_ena(this, "v_400kHz_ena", "1", "1"),
            vb_sdtype(this, "vb_sdtype", "3"),
            v_wdog_ena(this, "v_wdog_ena", "1"),
            v_crc16_clear(this, "v_crc16_clear", "1"),
            v_crc16_next(this, "v_crc16_next", "1") {
        }

     public:
        Logic v_cmd_dir;
        Logic v_cmd_in;
        Logic v_cmd_out;
        Logic v_dat0_dir;
        Logic v_dat0_out;
        Logic v_dat1_dir;
        Logic v_dat1_out;
        Logic v_dat2_dir;
        Logic v_dat2_out;
        Logic v_dat3_dir;
        Logic v_dat3_out;
        Logic v_cmd_req_valid;
        Logic vb_cmd_req_cmd;
        Logic vb_cmd_req_arg;
        Logic vb_cmd_req_rn;
        Logic v_req_sdmem_ready;
        Logic v_resp_sdmem_valid;
        Logic vb_resp_sdmem_data;
        Logic v_err_valid;
        Logic v_err_clear;
        Logic vb_err_setcode;
        Logic v_400kHz_ena;
        Logic vb_sdtype;
        Logic v_wdog_ena;
        Logic v_crc16_clear;
        Logic v_crc16_next;
    };

    void proc_comb();

 public:
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
    
    TextLine _mode0_;
    ParamLogic MODE_PRE_INIT;
    ParamLogic MODE_SPI;
    ParamLogic MODE_SD;

    Signal w_regs_sck_posedge;
    Signal w_regs_sck_negedge;
    Signal w_regs_err_clear;
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
    Signal w_req_sdmem_valid;
    Signal w_req_sdmem_write;
    Signal wb_req_sdmem_addr;
    Signal wb_req_sdmem_wdata;
    Signal w_regs_flush_valid;
    Signal w_cache_flush_end;

    Signal w_trx_cmd;
    Signal w_trx_cmd_dir;
    Signal w_trx_cmd_csn;
    Signal w_trx_wdog_ena;
    Signal w_trx_err_valid;
    Signal wb_trx_err_setcode;
    Signal w_cmd_in;
    Signal w_cmd_req_ready;
    Signal w_cmd_resp_valid;
    Signal wb_cmd_resp_cmd;
    Signal wb_cmd_resp_reg;
    Signal wb_cmd_resp_crc7_rx;
    Signal wb_cmd_resp_crc7_calc;
    Signal wb_cmd_resp_spistatus;
    Signal w_cmd_resp_ready;
    
    Signal wb_crc16_0;
    Signal wb_crc16_1;
    Signal wb_crc16_2;
    Signal wb_crc16_3;
    Signal w_wdog_trigger;
    Signal wb_err_code;
    Signal w_err_pending;

    TextLine _mux0_;
    TextLine _mux1_;
    Signal w_spi_dat;
    Signal w_spi_dat_csn;
    Signal w_spi_cmd_req_valid;
    Signal wb_spi_cmd_req_cmd;
    Signal wb_spi_cmd_req_arg;
    Signal wb_spi_cmd_req_rn;
    Signal w_spi_req_sdmem_ready;
    Signal w_spi_resp_sdmem_valid;
    Signal wb_spi_resp_sdmem_data;
    Signal w_spi_err_valid;
    Signal w_spi_err_clear;
    Signal wb_spi_err_setcode;
    Signal w_spi_400kHz_ena;
    Signal wb_spi_sdtype;
    Signal w_spi_wdog_ena;
    Signal w_spi_crc16_clear;
    Signal w_spi_crc16_next;
    TextLine _mux2_;
    TextLine _mux3_;
    Signal w_sd_dat0;
    Signal w_sd_dat0_dir;
    Signal w_sd_dat1;
    Signal w_sd_dat1_dir;
    Signal w_sd_dat2;
    Signal w_sd_dat2_dir;
    Signal w_sd_dat3;
    Signal w_sd_dat3_dir;
    Signal w_sd_cmd_req_valid;
    Signal wb_sd_cmd_req_cmd;
    Signal wb_sd_cmd_req_arg;
    Signal wb_sd_cmd_req_rn;
    Signal w_sd_req_sdmem_ready;
    Signal w_sd_resp_sdmem_valid;
    Signal wb_sd_resp_sdmem_data;
    Signal w_sd_err_valid;
    Signal w_sd_err_clear;
    Signal wb_sd_err_setcode;
    Signal w_sd_400kHz_ena;
    Signal wb_sd_sdtype;
    Signal w_sd_wdog_ena;
    Signal w_sd_crc16_clear;
    Signal w_sd_crc16_next;
    TextLine _mux4_;
    TextLine _mux5_;
    Signal w_cmd_req_valid;
    Signal wb_cmd_req_cmd;
    Signal wb_cmd_req_arg;
    Signal wb_cmd_req_rn;
    Signal w_req_sdmem_ready;
    Signal w_resp_sdmem_valid;
    Signal wb_resp_sdmem_data;
    Signal w_err_valid;
    Signal w_err_clear;
    Signal wb_err_setcode;
    Signal w_400kHz_ena;
    Signal wb_sdtype;
    Signal w_wdog_ena;
    Signal w_crc16_clear;
    Signal w_crc16_next;

    RegSignal nrst_spimode;
    RegSignal nrst_sdmode;
    RegSignal clkcnt;
    RegSignal cmd_set_low;
    RegSignal mode;

    CombProcess comb;

    axi_slv xslv0;
    sdctrl_regs regs0;
    sdctrl_err err0;
    sdctrl_wdog wdog0;
    sdctrl_crc16 crcdat0;
    sdctrl_crc16 crcdat1;
    sdctrl_crc16 crcdat2;
    sdctrl_crc16 crcdat3;
    sdctrl_spimode spimode0;
    sdctrl_sdmode sdmode0;
    sdctrl_cmd_transmitter cmdtrx0;
    sdctrl_cache cache0;
};

class sdctrl_file : public FileObject {
 public:
    sdctrl_file(GenObject *parent) :
        FileObject(parent, "sdctrl"),
        sdctrl_(this, "") {}

 private:
    sdctrl sdctrl_;
};

