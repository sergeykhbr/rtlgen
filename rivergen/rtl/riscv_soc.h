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

#include <api_rtlgen.h>
#include "ambalib/types_amba.h"
#include "ambalib/types_bus0.h"
#include "ambalib/types_bus1.h"
#include "ambalib/types_pnp.h"
#include "ambalib/axictrl_bus0.h"
#include "ambalib/axi2apb_bus1.h"
#include "misclib/apb_uart.h"
#include "misclib/apb_gpio.h"
#include "misclib/apb_pnp.h"
#include "misclib/axi_sram.h"
#include "misclib/axi_rom.h"
#include "misclib/plic.h"
#include "misclib/clint.h"
#include "sdctrl/sdctrl.h"
#include "riverlib/river_cfg.h"
#include "riverlib/types_river.h"
#include "riverlib/river_amba.h"
#include "riverlib/workgroup.h"
#include "techmap/cdc_axi_sync/cdc_axi_sync_tech.h"

using namespace sysvc;

class riscv_soc : public ModuleObject {
 public:
    riscv_soc(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    virtual GenObject *getResetPort() override { return &i_sys_nrst; }

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject* parent)
            : ProcObject(parent, "comb"),
            v_gnd1(this, "v_gnd1", "1", "0", NO_COMMENT),
            vb_ext_irqs(this, "vb_ext_irqs", "SOC_PLIC_IRQ_TOTAL") {
        }
     public:
        Logic1 v_gnd1;
        Logic vb_ext_irqs;
    };

    void proc_comb();

public:
    DefParamI32D sim_uart_speedup_rate;
    //ParamBOOL async_reset;
    // Ports:
    InPort i_sys_nrst;
    InPort i_sys_clk;
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
    TextLine _sdctrl0_;
    OutPort o_sd_sclk;
    InPort i_sd_cmd;
    OutPort o_sd_cmd;
    OutPort o_sd_cmd_dir;
    InPort i_sd_dat0;
    OutPort o_sd_dat0;
    OutPort o_sd_dat0_dir;
    InPort i_sd_dat1;
    OutPort o_sd_dat1;
    OutPort o_sd_dat1_dir;
    InPort i_sd_dat2;
    OutPort o_sd_dat2;
    OutPort o_sd_dat2_dir;
    InPort i_sd_cd_dat3;
    OutPort o_sd_cd_dat3;
    OutPort o_sd_cd_dat3_dir;
    InPort i_sd_detected;
    InPort i_sd_protect;
    TextLine _prci0_;
    OutPort o_dmreset;
    OutStruct<types_amba::mapinfo_type> o_prci_pmapinfo;
    InStruct<types_pnp::dev_config_type> i_prci_pdevcfg;
    OutStruct<types_amba::apb_in_type> o_prci_apbi;
    InStruct<types_amba::apb_out_type> i_prci_apbo;
    TextLine _ddr0_;
    OutStruct<types_amba::mapinfo_type> o_ddr_pmapinfo;
    InStruct<types_pnp::dev_config_type> i_ddr_pdevcfg;
    OutStruct<types_amba::apb_in_type> o_ddr_apbi;
    InStruct<types_amba::apb_out_type> i_ddr_apbo;
    OutStruct<types_amba::mapinfo_type> o_ddr_xmapinfo;
    InStruct<types_pnp::dev_config_type> i_ddr_xdevcfg;
    OutStruct<types_amba::axi4_slave_in_type> o_ddr_xslvi;
    InStruct<types_amba::axi4_slave_out_type> i_ddr_xslvo;

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
    TextLine _cfg4_;
    TextLine _cfg5_;
    ParamI32D SOC_SPI0_LOG2_FIFOSZ;
    TextLine _plic0_;
    TextLine _plic1_;
    TextLine _plic2_;
    ParamI32D SOC_PLIC_CONTEXT_TOTAL;
    TextLine _plic3_;
    ParamI32D SOC_PLIC_IRQ_TOTAL;

    SignalStruct<types_amba::axi4_master_out_type> acpo;
    SignalStruct<types_amba::axi4_master_in_type> acpi;
    types_bus0::bus0_mapinfo_vector   bus0_mapinfo;
    types_bus0::bus0_xmst_in_vector   aximi;
    types_bus0::bus0_xmst_out_vector  aximo;
    types_bus0::bus0_xslv_in_vector   axisi;
    types_bus0::bus0_xslv_out_vector  axiso;
    types_bus1::bus1_mapinfo_vector   bus1_mapinfo;
    types_bus1::bus1_apb_in_vector   apbi;
    types_bus1::bus1_apb_out_vector  apbo;
    types_pnp::soc_pnp_vector dev_pnp;
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

    // Sub-module instances:
    axictrl_bus0 bus0;
    axi2apb_bus1 bus1;
    axi_rom rom0;
    axi_sram sram0;
    clint clint0;
    plic plic0;
    apb_uart uart1;
    apb_gpio gpio0;
    sdctrl sdctrl0;
    apb_pnp pnp0;
    Workgroup group0;
    cdc_axi_sync_tech u_cdc_ddr0;
    // process
    CombProcess comb;
};

class riscv_soc_file : public FileObject {
 public:
    riscv_soc_file(GenObject *parent) :
        FileObject(parent, "riscv_soc"),
        riscv_soc_(this, "riscv_soc") { }

 private:
    riscv_soc riscv_soc_;
};

