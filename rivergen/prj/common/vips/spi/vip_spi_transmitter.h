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

class vip_spi_transmitter : public ModuleObject {
 public:
    vip_spi_transmitter(GenObject *parent, const char *name, const char *comment);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            v_pos(this, "v_pos", "1"),
            v_neg(this, "v_neg", "1"),
            v_resp_ready(this, "v_resp_ready", "1") {
        }

     public:
        Logic v_pos;
        Logic v_neg;
        Logic v_resp_ready;
    };

    void proc_comb();

 public:
    DefParamI32D scaler;
    ParamI32D scaler_max;
    ParamI32D scaler_mid;
    // io:
    InPort i_nrst;
    InPort i_clk;
    InPort i_csn;
    InPort i_sclk;
    InPort i_mosi;
    OutPort o_miso;
    OutPort o_req_valid;
    OutPort o_req_write;
    OutPort o_req_addr;
    OutPort o_req_wdata;
    InPort i_req_ready;
    InPort i_resp_valid;
    InPort i_resp_rdata;
    OutPort o_resp_ready;

    ParamLogic state_cmd;
    ParamLogic state_addr;
    ParamLogic state_data;

    RegSignal state;
    RegSignal sclk;
    RegSignal rxshift;
    RegSignal txshift;
    RegSignal bitcnt;
    RegSignal bytecnt;
    RegSignal byterdy;
    RegSignal req_valid;
    RegSignal req_write;
    RegSignal req_addr;
    RegSignal req_wdata;

    CombProcess comb;
};

class vip_spi_transmitter_file : public FileObject {
 public:
    vip_spi_transmitter_file(GenObject *parent) :
        FileObject(parent, "vip_spi_transmitter"),
        vip_spi_transmitter_(this, "vip_spi_transmitter", NO_COMMENT) {}

 private:
    vip_spi_transmitter vip_spi_transmitter_;
};

