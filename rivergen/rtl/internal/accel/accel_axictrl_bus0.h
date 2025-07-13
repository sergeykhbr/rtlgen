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
#include "../ambalib/types_amba.h"
#include "../ambalib/axi_slv.h"
#include "../ambalib/types_pnp.h"
#include "types_accel_bus0.h"

using namespace sysvc;

class accel_axictrl_bus0 : public ModuleObject {
 public:
    accel_axictrl_bus0(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            vmsti(this, "vmsti", "CFG_BUS0_XMST_TOTAL", "axi4_master_in_none", NO_COMMENT),
            vmsto(this, "vmsto", "CFG_BUS0_XMST_TOTAL", "axi4_master_out_none", NO_COMMENT),
            vslvi(this, "vslvi", "CFG_BUS0_XSLV_TOTAL", "axi4_slave_in_none", NO_COMMENT),
            vslvo(this, "vslvo", "CFG_BUS0_XSLV_TOTAL", "axi4_slave_out_none", NO_COMMENT),
            vb_ar_select(this, "vb_ar_select", "MUL(CFG_BUS0_XMST_TOTAL, CFG_BUS0_XSLV_TOTAL)", "'0", NO_COMMENT),
            vb_ar_available(this, "vb_ar_available", "MUL(ADD(CFG_BUS0_XMST_TOTAL,1), CFG_BUS0_XSLV_TOTAL)", "'1", NO_COMMENT),
            vb_ar_hit(this, "vb_ar_hit", "CFG_BUS0_XMST_TOTAL", "'0", NO_COMMENT),
            vb_aw_select(this, "vb_aw_select", "MUL(CFG_BUS0_XMST_TOTAL, CFG_BUS0_XSLV_TOTAL)", "'0", NO_COMMENT),
            vb_aw_available(this, "vb_aw_available", "MUL(ADD(CFG_BUS0_XMST_TOTAL,1), CFG_BUS0_XSLV_TOTAL)", "'1", NO_COMMENT),
            vb_aw_hit(this, "vb_aw_hit", "CFG_BUS0_XMST_TOTAL", "'0", NO_COMMENT),
            vb_w_select(this, "vb_w_select", "MUL(CFG_BUS0_XMST_TOTAL, CFG_BUS0_XSLV_LOG2_TOTAL)", "'0", NO_COMMENT),
            vb_w_active(this, "vb_w_active", "CFG_BUS0_XMST_TOTAL", "'0", NO_COMMENT) {
        }

     public:
        ValueArray<StructVar<types_amba::axi4_master_in_type>> vmsti;
        ValueArray<StructVar<types_amba::axi4_master_out_type>> vmsto;
        ValueArray<StructVar<types_amba::axi4_slave_in_type>> vslvi;
        ValueArray<StructVar<types_amba::axi4_slave_out_type>> vslvo;
        Logic vb_ar_select;
        Logic vb_ar_available;
        Logic vb_ar_hit;
        Logic vb_aw_select;
        Logic vb_aw_available;
        Logic vb_aw_hit;
        Logic vb_w_select;
        Logic vb_w_active;
    };

    void proc_comb();


 public:
    // io:
    InPort i_clk;
    InPort i_nrst;
    OutStruct<types_pnp::dev_config_type> o_cfg;
    InStruct<types_accel_bus0::bus0_xmst_out_vector> i_xmsto;
    OutStruct<types_accel_bus0::bus0_xmst_in_vector> o_xmsti;
    InStruct<types_accel_bus0::bus0_xslv_out_vector> i_xslvo;
    OutStruct<types_accel_bus0::bus0_xslv_in_vector> o_xslvi;
    OutStruct<types_accel_bus0::bus0_mapinfo_vector> o_mapinfo;

    SignalStruct<types_amba::axi4_slave_in_type> wb_def_xslvi;
    SignalStruct<types_amba::axi4_slave_out_type> wb_def_xslvo;
    SignalStruct<types_amba::mapinfo_type> wb_def_mapinfo;
    Signal w_def_req_valid;
    Signal wb_def_req_addr;
    Signal wb_def_req_size;
    Signal w_def_req_write;
    Signal wb_def_req_wdata;
    Signal wb_def_req_wstrb;
    Signal w_def_req_last;
    Signal w_def_req_ready;
    Signal w_def_resp_valid;
    Signal wb_def_resp_rdata;
    Signal w_def_resp_err;

    RegSignal w_select;
    RegSignal w_active;
    RegSignal r_def_valid;

    CombProcess comb;

    axi_slv xdef0;
};

class accel_axictrl_bus0_file : public FileObject {
 public:
    accel_axictrl_bus0_file(GenObject *parent) :
        FileObject(parent, "accel_axictrl_bus0"),
        accel_axictrl_bus0_(this, "accel_axictrl_bus0") {}

 private:
    accel_axictrl_bus0 accel_axictrl_bus0_;
};

