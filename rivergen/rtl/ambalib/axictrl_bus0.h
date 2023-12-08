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
#include "types_amba.h"
#include "types_pnp.h"
#include "axi_slv.h"
#include "types_bus0.h"

using namespace sysvc;

class axictrl_bus0 : public ModuleObject {
 public:
    axictrl_bus0(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            vmsti(this, "vmsti", "ADD(CFG_BUS0_XMST_TOTAL,1)", "axi4_master_in_none", NO_COMMENT),
            vmsto(this, "vmsto", "ADD(CFG_BUS0_XMST_TOTAL,1)", "axi4_master_out_none", NO_COMMENT),
            vslvi(this, "vslvi", "ADD(CFG_BUS0_XSLV_TOTAL,1)", "axi4_slave_in_none", NO_COMMENT),
            vslvo(this, "vslvo", "ADD(CFG_BUS0_XSLV_TOTAL,1)", "axi4_slave_out_none", NO_COMMENT),
            vb_def_mapinfo(this, "vb_def_mapinfo", "mapinfo_none", NO_COMMENT),
            i_ar_midx("0", "i_ar_midx", this),
            i_aw_midx("0", "i_aw_midx", this),
            i_ar_sidx("0", "i_ar_sidx", this),
            i_aw_sidx("0", "i_aw_sidx", this),
            i_r_midx("0", "i_r_midx", this),
            i_r_sidx("0", "i_r_sidx", this),
            i_w_midx("0", "i_w_midx", this),
            i_w_sidx("0", "i_w_sidx", this),
            i_b_midx("0", "i_b_midx", this),
            i_b_sidx("0", "i_b_sidx", this),
            v_aw_fire(this, "v_aw_fire", "1"),
            v_ar_fire(this, "v_ar_fire", "1"),
            v_w_fire(this, "v_w_fire", "1"),
            v_w_busy(this, "v_w_busy", "1"),
            v_r_fire(this, "v_r_fire", "1"),
            v_r_busy(this, "v_r_busy", "1"),
            v_b_fire(this, "v_b_fire", "1"),
            v_b_busy(this, "v_b_busy", "1") {
        }

     public:
        StructVarArray<types_amba::axi4_master_in_type> vmsti;
        StructVarArray<types_amba::axi4_master_out_type> vmsto;
        StructVarArray<types_amba::axi4_slave_in_type> vslvi;
        StructVarArray<types_amba::axi4_slave_out_type> vslvo;
        StructVar<types_amba::mapinfo_type> vb_def_mapinfo;
        I32D i_ar_midx;
        I32D i_aw_midx;
        I32D i_ar_sidx;
        I32D i_aw_sidx;
        I32D i_r_midx;
        I32D i_r_sidx;
        I32D i_w_midx;
        I32D i_w_sidx;
        I32D i_b_midx;
        I32D i_b_sidx;
        Logic v_aw_fire;
        Logic v_ar_fire;
        Logic v_w_fire;
        Logic v_w_busy;
        Logic v_r_fire;
        Logic v_r_busy;
        Logic v_b_fire;
        Logic v_b_busy;
    };

    void proc_comb();


 public:
    // io:
    InPort i_clk;
    InPort i_nrst;
    OutStruct<types_pnp::dev_config_type> o_cfg;
    InStruct<types_bus0::bus0_xmst_out_vector> i_xmsto;
    OutStruct<types_bus0::bus0_xmst_in_vector> o_xmsti;
    InStruct<types_bus0::bus0_xslv_out_vector> i_xslvo;
    OutStruct<types_bus0::bus0_xslv_in_vector> o_xslvi;
    OutStruct<types_bus0::bus0_mapinfo_vector> o_mapinfo;

    SignalStruct<types_amba::mapinfo_type> wb_def_mapinfo;
    SignalStruct<types_amba::axi4_slave_in_type> wb_def_xslvi;
    SignalStruct<types_amba::axi4_slave_out_type> wb_def_xslvo;
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

    RegSignal r_midx;
    RegSignal r_sidx;
    RegSignal w_midx;
    RegSignal w_sidx;
    RegSignal b_midx;
    RegSignal b_sidx;

    CombProcess comb;

    axi_slv xdef0;
};

class axictrl_bus0_file : public FileObject {
 public:
    axictrl_bus0_file(GenObject *parent) :
        FileObject(parent, "axictrl_bus0"),
        axictrl_bus0_(this, "axictrl_bus0") {}

 private:
    axictrl_bus0 axictrl_bus0_;
};

