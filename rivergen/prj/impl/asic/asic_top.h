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
#include "../../../rtl/ambalib/types_amba.h"
#include "../../../rtl/ambalib/types_pnp.h"
#include "../../../rtl/riscv_soc.h"
#include "../../../rtl/misclib/apb_prci.h"

using namespace sysvc;

class asic_top : public ModuleObject {
 public:
    asic_top(GenObject *parent, const char *name);

    virtual GenObject *getResetPort() override { return &i_rst; }
    virtual bool getResetActive() override { return true; }
    virtual bool isTop() override { return true; }

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject* parent)
            : ProcObject(parent, "comb") {
        }
     public:
    };

    void proc_comb();

public:
    DefParamString bootfile;
    DefParamI32D sim_uart_speedup_rate;
    ParamBOOL async_reset;
    // Ports:
    InPort i_rst;
    TextLine _clk0_;
    InPort i_sclk_p;
    InPort i_sclk_n;
    TextLine _gpio0_;
    IoPort io_gpio;
    TextLine _jtag0_;
    InPort i_jtag_trst;
    InPort i_jtag_tck;
    InPort i_jtag_tms;
    InPort i_jtag_tdi;
    OutPort o_jtag_tdo;
    OutPort o_jtag_vref;
    TextLine _uart1_;
    InPort i_uart1_rd;
    OutPort o_uart1_td;
    TextLine _spi0_;
    OutPort o_spi_cs;
    OutPort o_spi_sclk;
    OutPort o_spi_mosi;
    InPort i_spi_miso;
    InPort i_sd_detected;
    InPort i_sd_protect;

    // Param

    // Signals:
    Signal ib_clk_tcxo;
    Signal ib_gpio_ipins;
    Signal ob_gpio_opins;
    Signal ob_gpio_direction;

    Signal w_sys_rst;
    Signal w_sys_nrst;
    Signal w_dbg_nrst;
    Signal w_dmreset;
    Signal w_sys_clk;
    Signal w_ddr_clk;
    Signal w_pll_lock;

    types_amba::mapinfo_type_signal ddr_xmapinfo;
    types_pnp::dev_config_type_signal ddr_xdev_cfg;
    types_amba::axi4_slave_out_type_signal ddr_xslvo;
    types_amba::axi4_slave_in_type_signal ddr_xslvi;

    types_amba::mapinfo_type_signal ddr_pmapinfo;
    types_pnp::dev_config_type_signal ddr_pdev_cfg;
    types_amba::apb_in_type_signal ddr_apbi;
    types_amba::apb_out_type_signal ddr_apbo;

    Signal w_ddr_ui_nrst;
    Signal w_ddr_ui_clk;
    Signal w_ddr3_init_calib_complete;

    types_amba::mapinfo_type_signal prci_pmapinfo;
    types_pnp::dev_config_type_signal prci_dev_cfg;
    types_amba::apb_in_type_signal prci_apbi;
    types_amba::apb_out_type_signal prci_apbo;

    // Sub-module instances:
    apb_prci prci0;
    riscv_soc soc0;
    // process
    CombProcess comb;
};

class asic_top_file : public FileObject {
 public:
    asic_top_file(GenObject *parent) :
        FileObject(parent, "asic_top"),
        asic_top_(this, "") { }

 private:
    asic_top asic_top_;
};

