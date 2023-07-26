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
#include "../ambalib/apb_slv.h"
#include "../misclib/sfifo.h"

using namespace sysvc;

class sdctrl : public ModuleObject {
 public:
    sdctrl(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            v_posedge(this, "v_posedge", "1"),
            v_negedge(this, "v_negedge", "1"),
            v_txfifo_re(this, "v_txfifo_re", "1"),
            v_txfifo_we(this, "v_txfifo_we", "1"),
            vb_txfifo_wdata(this, "vb_txfifo_wdata", "8"),
            v_rxfifo_re(this, "v_rxfifo_re", "1"),
            v_inv7(this, "v_inv7", "1"),
            vb_crc7(this, "vb_crc7", "7"),
            v_inv16(this, "v_inv16", "1"),
            vb_crc16(this, "vb_crc16", "16"),
            vb_rdata(this, "vb_rdata", "32"),
            vb_shiftreg_next(this, "vb_shiftreg_next", "8") {
        }

     public:
        Logic v_posedge;
        Logic v_negedge;
        Logic v_txfifo_re;
        Logic v_txfifo_we;
        Logic vb_txfifo_wdata;
        Logic v_rxfifo_re;
        Logic v_inv7;
        Logic vb_crc7;
        Logic v_inv16;
        Logic vb_crc16;
        Logic vb_rdata;
        Logic vb_shiftreg_next;
    };

    void proc_comb();

 public:
    TmplParamI32D log2_fifosz;
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
    
    TextLine _state0_;
    ParamLogic idle;
    ParamLogic wait_edge;
    ParamLogic send_data;
    ParamLogic recv_data;
    ParamLogic recv_sync;
    ParamLogic ending;

    Signal w_preq_valid;
    Signal wb_preq_addr;
    Signal w_preq_write;
    Signal wb_preq_wdata;
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

    TextLine _rx0_;
    Signal w_rxfifo_we;
    Signal wb_rxfifo_wdata;
    Signal w_rxfifo_re;
    Signal wb_rxfifo_rdata;
    Signal wb_rxfifo_count;

    TextLine _tx0_;
    Signal w_txfifo_we;
    Signal wb_txfifo_wdata;
    Signal w_txfifo_re;
    Signal wb_txfifo_rdata;
    Signal wb_txfifo_count;

    RegSignal scaler;
    RegSignal scaler_cnt;
    RegSignal wdog;
    RegSignal wdog_cnt;
    RegSignal generate_crc;
    RegSignal rx_ena;
    RegSignal rx_synced;
    RegSignal rx_data_block;    // wait 0xFE marker
    RegSignal level;
    RegSignal cs;


    RegSignal state;
    RegSignal shiftreg;
    RegSignal ena_byte_cnt;
    RegSignal bit_cnt;
    RegSignal tx_val;
    RegSignal rx_val;
    RegSignal rx_ready;
    RegSignal crc7;
    RegSignal crc16;
    RegSignal spi_resp;
    RegSignal txmark;
    RegSignal rxmark;
    RegSignal presp_valid;
    RegSignal presp_rdata;
    RegSignal presp_err;

    CombProcess comb;

    apb_slv pslv0;
    axi_slv xslv0;
    sfifo rxfifo;
    sfifo txfifo;
};

class sdctrl_file : public FileObject {
 public:
    sdctrl_file(GenObject *parent) :
        FileObject(parent, "sdctrl"),
        sdctrl_(this, "") {}

 private:
    sdctrl sdctrl_;
};

