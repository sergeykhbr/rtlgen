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
#include <genconfig.h>
#include "../asic/asic_top.h"
#include "../../common/vips/clk/vip_clk.h"
#include "../../common/vips/uart/vip_uart_top.h"

using namespace sysvc;

class asic_top_tb : public ModuleObject {
 public:
    asic_top_tb(GenObject *parent, const char *name);

    virtual bool isTop() override { return true; }
    virtual bool isAsyncResetParam() override { return false; }

    class TestProcess : public ProcObject {
     public:
        TestProcess(GenObject *parent, GenObject *clk) :
            ProcObject(parent, "test", clk, CLK_POSEDGE, 0, ACTIVE_NONE, NO_COMMENT) {
        }

     public:
    };

    void proc_test();


public:
    // Parameters
    ParamI32D sim_uart_speedup_rate;
    ParamI32D sim_uart_baudrate;

    // Ports:
    Signal w_rst;
    Signal w_nrst;
    Signal w_sclk_p;
    Signal w_sclk_n;
    Signal wb_gpio;
    Signal w_jtag_trst;
    Signal w_jtag_tck;
    Signal w_jtag_tms;
    Signal w_jtag_tdi;
    Signal w_jtag_tdo;
    Signal w_jtag_vref;
    Signal w_uart1_rd;
    Signal w_uart1_td;
    Signal w_uart1_loopback_ena;
#if GENCFG_HDMI_ENABLE
    Signal w_i2c_scl;
    Signal w_i2c_sda;
#endif
    Logic wb_clk_cnt;

#if GENCFG_SD_CTRL_ENABLE
#endif
#if GENCFG_SD_CTRL_ENABLE
#endif

    // Sub-module instances:
    vip_clk clk0;
    vip_uart_top uart1;
    asic_top tt;

    TestProcess proc;
};

class asic_top_tb_file : public FileObject {
 public:
    asic_top_tb_file(GenObject *parent) :
        FileObject(parent, "asic_top_tb"),
        asic_top_tb_(this, "asic_top_tb") { }

 private:
    asic_top_tb asic_top_tb_;
};

