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
#include "target_accel_cfg.h"
#include "../../../rtl/internal/ambalib/types_amba.h"
#include "../../../rtl/internal/ambalib/types_pnp.h"
#include "../../../rtl/internal/misclib/apb_prci.h"
#include "../../../rtl/internal/pcie_dma/types_pcie_dma.h"
#include "../../../rtl/sim/ddr3_phy/ddr3_tech.h"
#include "../../../rtl/sim/io/ids_tech.h"
#include "../../../rtl/sim/io/iobuf_tech.h"
#include "../../../rtl/sim/io/ibuf_tech.h"
#include "../../../rtl/sim/io/obuf_tech.h"
#include "../../../rtl/sim/io/obuf_arr_tech.h"
#include "../../../rtl/sim/pll/SysPLL_tech.h"
#include "../../../rtl/internal/accel/accel_soc.h"


using namespace sysvc;

class asic_accel_top : public ModuleObject {
 public:
    asic_accel_top(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

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
    TextLine _i2c0_;
    OutPort o_i2c0_scl;
    IoPort io_i2c0_sda;
    OutPort o_i2c0_nreset;
    TextLine _i2c1_;
    OutPort o_hdmi_clk;
    OutPort o_hdmi_hsync;
    OutPort o_hdmi_vsync;
    OutPort o_hdmi_de;
    OutPort o_hdmi_d;
    OutPort o_hdmi_spdif;
    InPort i_hdmi_spdif_out;
    InPort i_hdmi_int;
    TextLine _ddr0_;
    OutPort o_ddr3_reset_n;
    OutPort o_ddr3_ck_n;
    OutPort o_ddr3_ck_p;
    OutPort o_ddr3_cke;
    OutPort o_ddr3_cs_n;
    OutPort o_ddr3_ras_n;
    OutPort o_ddr3_cas_n;
    OutPort o_ddr3_we_n;
    OutPort o_ddr3_dm;
    OutPort o_ddr3_ba;
    OutPort o_ddr3_addr;
    IoPort io_ddr3_dq;
    IoPort io_ddr3_dqs_n;
    IoPort io_ddr3_dqs_p;
    OutPort o_ddr3_odt;

    // Param
    // Signals:
    Signal ib_clk_tcxo;
    Signal ib_gpio_ipins;
    Signal ob_gpio_opins;
    Signal ob_gpio_direction;
    Signal ob_i2c0_scl;
    Signal ob_i2c0_sda;
    Signal ob_i2c0_sda_direction;
    Signal ib_i2c0_sda;
    Signal ob_i2c0_nreset;
    Signal ob_hdmi_hsync;
    Signal ob_hdmi_vsync;
    Signal ob_hdmi_de;
    Signal ob_hdmi_d;
    Signal ob_hdmi_spdif;
    Signal ib_hdmi_spdif_out;
    Signal ib_hdmi_int;

    Signal w_sys_rst;
    Signal w_sys_nrst;
    Signal w_dbg_nrst;
    Signal w_hdmi_nrst;
    Signal w_dmreset;
    Signal w_sys_clk;
    Signal w_ddr_clk;
    Signal w_ddr_phy_clk;
    Signal w_pcie_clk;
    Signal w_pll_lock;
    TextLine _t5_;
    Signal wb_ddr_aw_id;
    Signal wb_ddr_aw_addr;
    Signal wb_ddr_aw_len;
    Signal wb_ddr_aw_size;
    Signal wb_ddr_aw_burst;
    Signal w_ddr_aw_lock;
    Signal wb_ddr_aw_cache;
    Signal wb_ddr_aw_prot;
    Signal wb_ddr_aw_qos;
    Signal w_ddr_aw_valid;
    Signal w_ddr_aw_ready;
    Signal wb_ddr_w_data;
    Signal wb_ddr_w_strb;
    Signal w_ddr_w_last;
    Signal w_ddr_w_valid;
    Signal w_ddr_w_ready;
    Signal w_ddr_b_ready;
    Signal wb_ddr_b_id;
    Signal wb_ddr_b_resp;
    Signal w_ddr_b_valid;
    Signal wb_ddr_ar_id;
    Signal wb_ddr_ar_addr;
    Signal wb_ddr_ar_len;
    Signal wb_ddr_ar_size;
    Signal wb_ddr_ar_burst;
    Signal w_ddr_ar_lock;
    Signal wb_ddr_ar_cache;
    Signal wb_ddr_ar_prot;
    Signal wb_ddr_ar_qos;
    Signal w_ddr_ar_valid;
    Signal w_ddr_ar_ready;
    Signal w_ddr_r_ready;
    Signal wb_ddr_r_id;
    Signal wb_ddr_r_data;
    Signal wb_ddr_r_resp;
    Signal w_ddr_r_last;
    Signal w_ddr_r_valid;
    Signal w_ddr_app_sr_req;
    Signal w_ddr_app_ref_req;
    Signal w_ddr_app_zq_req;
    Signal w_ddr_app_sr_active;
    Signal w_ddr_app_ref_ack;
    Signal w_ddr_app_zq_ack;

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
    Signal w_ddr3_init_calib_done;
    Signal wb_ddr3_temperature;
    Signal w_pcie_phy_lnk_up;

    SignalStruct<types_amba::mapinfo_type> prci_pmapinfo;
    SignalStruct<types_pnp::dev_config_type> prci_dev_cfg;
    SignalStruct<types_amba::apb_in_type> prci_apbi;
    SignalStruct<types_amba::apb_out_type> prci_apbo;

    Signal w_pcie_user_clk;
    Signal w_pcie_user_rst;
    Signal w_pcie_nrst;
    Signal wb_pcie_completer_id;
    SignalStruct<types_pcie_dma::pcie_dma64_out_type> pcie_dmao;
    SignalStruct<types_pcie_dma::pcie_dma64_in_type> pcie_dmai;

    // Sub-module instances:
    ids_tech iclk0;
    obuf_tech oi2c0scl;
    obuf_tech oi2c0nreset;
    iobuf_tech ioi2c0sda;
    obuf_tech ohdmiclk;
    obuf_tech ohdmihsync;
    obuf_tech ohdmivsync;
    obuf_tech ohdmide;
    obuf_arr_tech ohdmid;
    obuf_tech ohdmispdif;
    ibuf_tech ihdmispdif;
    ibuf_tech ihdmiint;

    SysPLL_tech pll0;
    apb_prci prci0;
    ddr3_tech ddr3;
    accel_soc soc0;
    // process
};

class asic_accel_top_file : public FileObject {
 public:
    asic_accel_top_file(GenObject *parent) :
        FileObject(parent, "asic_accel_top"),
        asic_accel_top_(this, "asic_accel_top") { }

 private:
    asic_accel_top asic_accel_top_;
};

