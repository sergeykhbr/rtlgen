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
#include "../../internal/ambalib/types_amba.h"
#include "../../internal/ambalib/types_pnp.h"
#include "../../internal/misclib/apb_ddr.h"
#include "../../internal/misclib/axi_sram.h"
#include "../../sim/pll/pll_generic.h"

using namespace sysvc;

class ddr3_tech : public ModuleObject {
 public:
    ddr3_tech(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    virtual bool isAsyncResetParam() override { return false; }     // do not generate async_reset generic parameter

 protected:
    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb") {
        }

     public:
    };

    void proc_comb();

 public:
    // io:
    InPort i_apb_nrst;
    InPort i_apb_clk;
    InPort i_xslv_nrst;
    InPort i_xslv_clk;
    TextLine _t0_;
    InStruct<types_amba::mapinfo_type> i_xmapinfo;
    OutStruct<types_pnp::dev_config_type> o_xcfg;
    InStruct<types_amba::axi4_slave_in_type> i_xslvi;
    OutStruct<types_amba::axi4_slave_out_type> o_xslvo;
    TextLine _t1_;
    InStruct<types_amba::mapinfo_type> i_pmapinfo;
    OutStruct<types_pnp::dev_config_type> o_pcfg;
    InStruct<types_amba::apb_in_type> i_apbi;
    OutStruct<types_amba::apb_out_type> o_apbo;
    TextLine _t3_;
    OutPort o_ui_nrst;
    OutPort o_ui_clk;
    OutPort o_init_calib_done;

    // signals:
    Signal w_ui_nrst;
    Signal w_ui_clk;
    Signal w_init_calib_done;
    Signal wb_device_temp;
    Signal w_sr_active;
    Signal w_ref_ack;
    Signal w_zq_ack;
    SignalStruct<types_pnp::dev_config_type> wb_xcfg_unused;

    // Registers:
    RegSignal ddr_calib;

    pll_generic clk0;
    apb_ddr pctrl0;
    axi_sram sram0;
 private:
    CombProcess comb;
};

class ddr3_tech_file : public FileObject {
 public:
    ddr3_tech_file(GenObject *parent) :
        FileObject(parent, "ddr3_tech"),
        ddr3_tech_(this, "ddr3_tech") {}

 private:
    ddr3_tech ddr3_tech_;
};

