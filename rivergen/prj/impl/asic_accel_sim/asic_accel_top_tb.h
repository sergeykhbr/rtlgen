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
#include "../asic_accel/asic_accel_top.h"
#include "../../../rtl/sim/pll/pll_generic.h"
#include "../../common/vips/uart/vip_uart_top.h"
#include "../../common/vips/i2c/vip_i2c_s.h"
#include "../../common/vips/jtag/jtag_app.h"
#include "../../../rtl/sim/io/iobuf_tech.h"

using namespace sysvc;

class asic_accel_top_tb : public ModuleObject {
 public:
    asic_accel_top_tb(GenObject *parent, const char *name);

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
    Signal w_i2c_scl;
    Signal w_i2c_sda;
    Signal w_i2c_nreset;
    Signal w_hdmi_clk;
    Signal w_hdmi_hsync;
    Signal w_hdmi_vsync;
    Signal w_hdmi_de;
    Signal wb_hdmi_d;
    Signal w_hdmi_spdif;
    Signal w_hdmi_spdif_out;
    Signal w_hdmi_int;
    TextLine _ddrphy0_;
    Signal w_ddr3_reset_n;
    Signal w_ddr3_ck_n;
    Signal w_ddr3_ck_p;
    Signal w_ddr3_cke;
    Signal w_ddr3_cs_n;
    Signal w_ddr3_ras_n;
    Signal w_ddr3_cas_n;
    Signal w_ddr3_we_n;
    Signal wb_ddr3_dm;
    Signal wb_ddr3_ba;
    Signal wb_ddr3_addr;
    Signal wb_ddr3_dq;
    Signal wb_ddr3_dqs_n;
    Signal wb_ddr3_dqs_p;
    Signal w_ddr3_odt;
    Signal w_bufo_i2c0_sda;
    Signal w_vipo_i2c0_sda;
    Signal w_vipo_i2c0_sda_dir;
    Logic wb_clk_cnt;

    // Sub-module instances:
    pll_generic clk0;
    vip_uart_top uart1;
    jtag_app jtag0;
    iobuf_tech iosda0;
    vip_i2c_s i2c0;
    asic_accel_top tt;

    TestProcess proc;
};

class asic_accel_top_tb_file : public FileObject {
 public:
    asic_accel_top_tb_file(GenObject *parent) :
        FileObject(parent, "asic_accel_top_tb"),
        asic_accel_top_tb_(this, "asic_accel_top_tb") { }

 private:
    asic_accel_top_tb asic_accel_top_tb_;
};

