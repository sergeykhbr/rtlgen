// 
//  Copyright 2025 Sergey Khabarov, sergeykhbr@gmail.com
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
#include "../ambalib/types_amba.h"
#include "../misclib/apb_uart.h"
#include "../misclib/apb_i2c.h"
#include "../misclib/apb_gpio.h"
#include "../misclib/apb_pnp.h"
#include "../misclib/axi_sram.h"
#include "../misclib/axi_rom.h"
#include "../misclib/plic.h"
#include "../misclib/clint.h"
#include "../misclib/apb_ddr.h"
#include "../sdctrl/sdctrl.h"
#include "../pcie_dma/pcie_dma.h"
#include "../pcie_dma/apb_pcie.h"
#include "../hdmilib/hdmi_top.h"
#include "../riverlib/river_cfg.h"
#include "../riverlib/types_river.h"
#include "../riverlib/river_amba.h"
#include "../riverlib/workgroup.h"
#include "../cdc/afifo_xslv.h"
#include "../cdc/afifo_xmst.h"
#include "types_accel_bus0.h"
#include "types_accel_bus1.h"
#include "accel_axictrl_bus0.h"
#include "accel_axi2apb_bus1.h"

using namespace sysvc;

class accel_soc : public ModuleObject {
 public:
    accel_soc(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    virtual GenObject *getResetPort() override { return &i_sys_nrst; }

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject* parent)
            : CombinationalProcess(parent, "comb"),
            v_gnd1(this, "v_gnd1", "1", "0", NO_COMMENT),
            vb_ext_irqs(this, "vb_ext_irqs", "SOC_PLIC_IRQ_TOTAL"),
            vb_ddr_xcfg(this, "vb_ddr_xcfg", NO_COMMENT),
            vb_ddr_mapinfo(this, "vb_ddr_mapinfo", NO_COMMENT),
            vb_ddr_xslvo(this, "vb_ddr_xslvo", "systemc compatibility") {
        }
     public:
        Logic1 v_gnd1;
        Logic vb_ext_irqs;
        types_pnp::dev_config_type vb_ddr_xcfg;
        types_amba::mapinfo_type vb_ddr_mapinfo;
        types_amba::axi4_slave_out_type vb_ddr_xslvo;
    };

    void proc_comb();

public:
    DefParamI32D sim_uart_speedup_rate;
    // Ports:
    InPort i_sys_nrst;
    InPort i_sys_clk;
    InPort i_cpu_nrst;
    InPort i_cpu_clk;
    InPort i_apb_nrst;
    InPort i_apb_clk;
    InPort i_dbg_nrst;
    InPort i_ddr_nrst;
    InPort i_ddr_clk;
    TextLine _gpio0_;
    InPort i_gpio;
    OutPort o_gpio;
    OutPort o_gpio_dir;
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
    OutPort o_i2c0_sda;
    OutPort o_i2c0_sda_dir;
    InPort i_i2c0_sda;
    OutPort o_i2c0_nreset;
    InPort i_hdmi_nrst;
    InPort i_hdmi_clk;
    OutPort o_hdmi_hsync;
    OutPort o_hdmi_vsync;
    OutPort o_hdmi_de;
    OutPort o_hdmi_d;
    OutPort o_hdmi_spdif;
    InPort i_hdmi_spdif_out;
    InPort i_hdmi_int;
    TextLine _prci0_;
    OutPort o_dmreset;
    OutStruct<types_amba::mapinfo_type> o_prci_pmapinfo;
    InStruct<types_pnp::dev_config_type> i_prci_pdevcfg;
    OutStruct<types_amba::apb_in_type> o_prci_apbi;
    InStruct<types_amba::apb_out_type> i_prci_apbo;
    TextLine _ddr0_;
    OutPort o_ddr_aw_id;
    OutPort o_ddr_aw_addr;
    OutPort o_ddr_aw_len;
    OutPort o_ddr_aw_size;
    OutPort o_ddr_aw_burst;
    OutPort o_ddr_aw_lock;
    OutPort o_ddr_aw_cache;
    OutPort o_ddr_aw_prot;
    OutPort o_ddr_aw_qos;
    OutPort o_ddr_aw_valid;
    InPort i_ddr_aw_ready;
    OutPort o_ddr_w_data;
    OutPort o_ddr_w_strb;
    OutPort o_ddr_w_last;
    OutPort o_ddr_w_valid;
    InPort i_ddr_w_ready;
    OutPort o_ddr_b_ready;
    InPort i_ddr_b_id;
    InPort i_ddr_b_resp;
    InPort i_ddr_b_valid;
    OutPort o_ddr_ar_id;
    OutPort o_ddr_ar_addr;
    OutPort o_ddr_ar_len;
    OutPort o_ddr_ar_size;
    OutPort o_ddr_ar_burst;
    OutPort o_ddr_ar_lock;
    OutPort o_ddr_ar_cache;
    OutPort o_ddr_ar_prot;
    OutPort o_ddr_ar_qos;
    OutPort o_ddr_ar_valid;
    InPort i_ddr_ar_ready;
    OutPort o_ddr_r_ready;
    InPort i_ddr_r_id;
    InPort i_ddr_r_data;
    InPort i_ddr_r_resp;
    InPort i_ddr_r_last;
    InPort i_ddr_r_valid;
    InPort i_ddr_app_init_calib_done;
    InPort i_ddr_app_temp;
    OutPort o_ddr_app_sr_req;
    OutPort o_ddr_app_ref_req;
    OutPort o_ddr_app_zq_req;
    InPort i_ddr_app_sr_active;
    InPort i_ddr_app_ref_ack;
    InPort i_ddr_app_zq_ack;
    TextLine _pcie0_;
    InPort i_pcie_clk;
    InPort i_pcie_nrst;
    InPort i_pcie_completer_id;
    OutStruct<types_pcie_dma::pcie_dma64_out_type> o_pcie_dmao;
    InStruct<types_pcie_dma::pcie_dma64_in_type> i_pcie_dmai;

    // Param
    TextLine _hwid0_;
    TextLine _hwid1_;
    TextLine _hwid2_;
    ParamLogic SOC_HW_ID;
    TextLine _cfg0_;
    TextLine _cfg1_;
    ParamI32D SOC_UART1_LOG2_FIFOSZ;
    TextLine _cfg2_;
    TextLine _cfg3_;
    ParamI32D SOC_GPIO0_WIDTH;
    TextLine _plic0_;
    TextLine _plic1_;
    TextLine _plic2_;
    ParamI32D SOC_PLIC_CONTEXT_TOTAL;
    TextLine _plic3_;
    ParamI32D SOC_PLIC_IRQ_TOTAL;

    SignalStruct<types_amba::axi4_master_out_type> acpo;
    SignalStruct<types_amba::axi4_master_in_type> acpi;
    types_accel_bus0::bus0_mapinfo_vector   bus0_mapinfo;
    types_accel_bus0::bus0_xmst_in_vector   aximi;
    types_accel_bus0::bus0_xmst_out_vector  aximo;
    types_accel_bus0::bus0_xslv_in_vector   axisi;
    types_accel_bus0::bus0_xslv_out_vector  axiso;
    types_accel_bus1::bus1_mapinfo_vector   bus1_mapinfo;
    types_accel_bus1::bus1_apb_in_vector   apbi;
    types_accel_bus1::bus1_apb_out_vector  apbo;
    types_pnp::soc_pnp_vector dev_pnp;
    SignalStruct<types_amba::axi4_master_out_type> wb_group0_xmsto;
    SignalStruct<types_amba::axi4_master_in_type> wb_group0_xmsti;
    SignalStruct<types_amba::axi4_slave_in_type> wb_ddr_xslvi;
    SignalStruct<types_amba::axi4_slave_out_type> wb_ddr_xslvo;
    SignalStruct<types_amba::axi4_slave_in_type> wb_pbridge_xslvi;
    SignalStruct<types_amba::axi4_slave_out_type> wb_pbridge_xslvo;

    Signal wb_clint_mtimer;
    Signal wb_clint_msip;
    Signal wb_clint_mtip;
    Signal wb_plic_xeip;
    Signal wb_plic_meip;
    Signal wb_plic_seip;
    Signal w_irq_uart1;
    Signal wb_irq_gpio;
    Signal w_irq_pnp;
    Signal wb_ext_irqs;
    Signal wb_pcie_dma_state;
    Signal w_dbg_valid;
    Signal w_dbg_payload;
    Signal w_irq_i2c0;

    // Sub-module instances:
    accel_axictrl_bus0 bus0;
    accel_axi2apb_bus1 bus1;
    axi_rom rom0;
    axi_sram sram0;
    clint clint0;
    plic plic0;
    apb_uart uart1;
    apb_gpio gpio0;
    apb_ddr pddr0;
    apb_i2c i2c0;
    hdmi_top hdmi0;
    pcie_dma pcidma0;
    apb_pcie ppcie0;
    apb_pnp pnp0;
    Workgroup group0;
    afifo_xslv afifo_ddr0;
    afifo_xslv afifo_apb0;
    afifo_xmst afifo_group0;
    // process
    CombProcess comb;
};

class accel_soc_file : public FileObject {
 public:
    accel_soc_file(GenObject *parent) :
        FileObject(parent, "accel_soc"),
        accel_soc_(this, "accel_soc") { }

 private:
    accel_soc accel_soc_;
};

