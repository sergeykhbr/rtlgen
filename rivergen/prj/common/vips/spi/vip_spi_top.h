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
#include "../clk/vip_clk.h"
#include "vip_spi_transmitter.h"

using namespace sysvc;

class vip_spi_top : public ModuleObject {
 public:
    vip_spi_top(GenObject *parent, const char *name, const char *comment);

    virtual GenObject *getClockPort() override { return &w_clk; }

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            rdata(this, "rdata" ,"32", "'0", NO_COMMENT),
            vb_gpio_in(this, "vb_gpio_in", "1") {
        }

     public:
        Logic rdata;
        Logic vb_gpio_in;
    };

    void proc_comb();

 public:
    DefParamI32D instnum;
    DefParamI32D baudrate;
    DefParamI32D scaler;
    ParamTIMESEC pll_period;
    // io:
    InPort i_nrst;
    InPort i_csn;
    InPort i_sclk;
    InPort i_mosi;
    OutPort o_miso;
    OutPort o_vip_uart_loopback_ena;
    IoPort io_vip_gpio;

    Signal w_clk;
    Signal w_req_valid;
    Signal w_req_write;
    Signal wb_req_addr;
    Signal wb_req_wdata;
    Signal w_req_ready;
    Signal w_resp_valid;
    Signal wb_resp_rdata;
    Signal w_resp_ready;
    Signal wb_gpio_in;

    RegSignal resp_valid;
    RegSignal resp_rdata;
    RegSignal scratch0;
    RegSignal scratch1;
    RegSignal scratch2;
    RegSignal uart_loopback;
    RegSignal gpio_out;
    RegSignal gpio_dir;

    vip_clk clk0;
    vip_spi_transmitter tx0;

    CombProcess comb;
};

class vip_spi_top_file : public FileObject {
 public:
    vip_spi_top_file(GenObject *parent) :
        FileObject(parent, "vip_spi_top"),
        vip_spi_top_(this, "vip_spi_top", NO_COMMENT) {}

 private:
    vip_spi_top vip_spi_top_;
};

