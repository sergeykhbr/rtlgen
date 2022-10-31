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
#include "ambalib/types_amba.h"
#include "riverlib/river_cfg.h"
#include "riverlib/types_river.h"
#include "riverlib/river_amba.h"
#include "riverlib/workgroup.h"

using namespace sysvc;

class riscv_soc : public ModuleObject {
 public:
    riscv_soc(GenObject *parent, const char *name);

    virtual GenObject *getResetPort() override { return &i_rst; }
    virtual bool getResetActive() override { return true; }
    virtual bool isTop() override { return true; }

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject* parent)
            : ProcObject(parent, "comb"),
            v_flush_l2(this, "v_flush_l2", "1"),
            vb_halted(this, "vb_halted", "CFG_CPU_MAX"),
            vb_available(this, "vb_available", "CFG_CPU_MAX"),
            vb_irq(this, "vb_irq", "IRQ_TOTAL", "CFG_CPU_MAX") {
        }
     public:
        Logic v_flush_l2;
        Logic vb_halted;
        Logic vb_available;
        WireArray<Logic> vb_irq;
    };

    void proc_comb();


public:
    // Ports:
    InPort i_rst;
    InPort i_clk;
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

    // Param

    // Signals:
    Signal w_sys_nrst;
    Signal w_dbg_nrst;
    Signal w_dmreset;
    types_amba::axi4_master_out_type acpo;
    types_amba::axi4_master_in_type acpi;
    types_amba::bus0_xmst_in_vector   aximi;
    types_amba::bus0_xmst_out_vector  aximo;
    types_amba::bus0_xslv_in_vector   axisi;
    types_amba::bus0_xslv_out_vector  axiso;
    types_amba::bus0_xslv_cfg_vector  slv_cfg;
    types_amba::bus0_xmst_cfg_vector  mst_cfg;
    Signal wb_clint_mtimer;
    Signal wb_clint_msip;
    Signal wb_clint_mtip;
    Signal wb_plic_meip;
    Signal wb_plic_seip;

    // Sub-module instances:
    Workgroup group0;
    // process
    CombProcess comb;
};

class riscv_soc_file : public FileObject {
 public:
    riscv_soc_file(GenObject *parent) :
        FileObject(parent, "riscv_soc"),
        riscv_soc_(this, "") { }

 private:
    riscv_soc riscv_soc_;
};

