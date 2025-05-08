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
#include "target_cfg.h"
#include "../../../rtl/internal/ambalib/types_amba.h"
#include "../../../rtl/internal/ambalib/types_dma.h"
#include "../../../rtl/internal/ambalib/types_pnp.h"
#include "../../../rtl/internal/misclib/apb_prci.h"
#include "../../../rtl/sim/io/ids_tech.h"
#include "../../../rtl/sim/io/iobuf_tech.h"
#include "../../../rtl/sim/pll/SysPLL_tech.h"
#if CONFIG_RISCV_VHDL
    #include "../../../rtl/internal/gencpu64/gencpu64_soc.h"
#elif CONFIG_GPU3D
    #include "../../../rtl/internal/accel/accel_soc.h"
#endif


using namespace sysvc;

class asic_top : public ModuleObject {
 public:
    asic_top(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

public:
    DefParamLogic async_reset;
    DefParamI32D sim_uart_speedup_rate;

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
#if GENCFG_SD_CTRL_ENABLE
    TextLine _sdctrl0_;
    OutPort o_sd_sclk;
    IoPort io_sd_cmd;           // CMD IO Command/Resonse; Data output in SPI mode
    IoPort io_sd_dat0;          // Data0 IO; Data input in SPI mode
    IoPort io_sd_dat1;
    IoPort io_sd_dat2;
    IoPort io_sd_cd_dat3;       // CD/DAT3 IO CardDetect/Data Line 3; CS output in SPI mode
    InPort i_sd_detected;
    InPort i_sd_protect;
#endif

    // Param
    //gpio_signal_vector gpio_signal_vector_def_;

    // Signals:
    Signal ib_clk_tcxo;
    Signal ib_gpio_ipins;
    Signal ob_gpio_opins;
    Signal ob_gpio_direction;
#if GENCFG_SD_CTRL_ENABLE
    Signal ib_sd_cmd;
    Signal ob_sd_cmd;
    Signal ob_sd_cmd_direction;
    Signal ib_sd_dat0;
    Signal ob_sd_dat0;
    Signal ob_sd_dat0_direction;
    Signal ib_sd_dat1;
    Signal ob_sd_dat1;
    Signal ob_sd_dat1_direction;
    Signal ib_sd_dat2;
    Signal ob_sd_dat2;
    Signal ob_sd_dat2_direction;
    Signal ib_sd_cd_dat3;
    Signal ob_sd_cd_dat3;
    Signal ob_sd_cd_dat3_direction;
#endif

    Signal w_sys_rst;
    Signal w_sys_nrst;
    Signal w_dbg_nrst;
    Signal w_dmreset;
    Signal w_sys_clk;
    Signal w_ddr_clk;
    Signal w_pcie_clk;
    Signal w_pll_lock;

    SignalStruct<types_amba::mapinfo_type> ddr_xmapinfo;
    SignalStruct<types_pnp::dev_config_type> ddr_xdev_cfg;
    SignalStruct<types_amba::axi4_slave_out_type> ddr_xslvo;
    SignalStruct<types_amba::axi4_slave_in_type> ddr_xslvi;

    SignalStruct<types_amba::mapinfo_type> ddr_pmapinfo;
    SignalStruct<types_pnp::dev_config_type> ddr_pdev_cfg;
    SignalStruct<types_amba::apb_in_type> ddr_apbi;
    SignalStruct<types_amba::apb_out_type> ddr_apbo;

    Signal w_ddr_ui_nrst;
    Signal w_ddr_ui_clk;
    Signal w_ddr3_init_calib_complete;
    Signal w_pcie_phy_lnk_up;

    SignalStruct<types_amba::mapinfo_type> prci_pmapinfo;
    SignalStruct<types_pnp::dev_config_type> prci_dev_cfg;
    SignalStruct<types_amba::apb_in_type> prci_apbi;
    SignalStruct<types_amba::apb_out_type> prci_apbo;

#if GENCFG_PCIE_ENABLE
    Signal w_pcie_user_clk;
    Signal w_pcie_user_rst;
    Signal w_pcie_nrst;
    Signal wb_pcie_completer_id;
    SignalStruct<types_dma::pcie_dma64_out_type> pcie_dmao;
    SignalStruct<types_dma::pcie_dma64_in_type> pcie_dmai;
#endif

    // Sub-module instances:
    ids_tech iclk0;
#if GENCFG_SD_CTRL_ENABLE
    iobuf_tech iosdcmd0;
    iobuf_tech iosddat0;
    iobuf_tech iosddat1;
    iobuf_tech iosddat2;
    iobuf_tech iosddat3;
#endif
    SysPLL_tech pll0;
    apb_prci prci0;
#if CONFIG_RISCV_VHDL
    gencpu64_soc soc0;
#elif CONFIG_GPU3D
    accel_soc soc0;
#endif
    // process
};

class asic_top_file : public FileObject {
 public:
    asic_top_file(GenObject *parent) :
        FileObject(parent, "asic_top"),
        asic_top_(this, "asic_top") { }

 private:
    asic_top asic_top_;
};

