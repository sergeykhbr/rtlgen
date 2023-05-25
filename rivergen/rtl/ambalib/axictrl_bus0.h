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
#include "types_amba.h"
#include "axi_slv.h"
#include "types_bus0.h"

using namespace sysvc;

class axictrl_bus0 : public ModuleObject {
 public:
    axictrl_bus0(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            vmsti(this, "", "vmsti", "ADD(CFG_BUS0_XMST_TOTAL,1)"),
            vmsto(this, "", "vmsto", "ADD(CFG_BUS0_XMST_TOTAL,1)"),
            vslvi(this, "", "vmsti", "ADD(CFG_BUS0_XSLV_TOTAL,1)"),
            vslvo(this, "", "vmsto", "ADD(CFG_BUS0_XSLV_TOTAL,1)"),
            vb_ar_midx(this, "vb_ar_midx", "CFG_BUS0_XMST_LOG2_TOTAL"),
            vb_aw_midx(this, "vb_aw_midx", "CFG_BUS0_XMST_LOG2_TOTAL"),
            vb_ar_sidx(this, "vb_ar_sidx", "CFG_BUS0_XSLV_LOG2_TOTAL"),
            vb_aw_sidx(this, "vb_aw_sidx", "CFG_BUS0_XSLV_LOG2_TOTAL"),
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
        TStructArray<types_amba::axi4_master_in_type> vmsti;
        TStructArray<types_amba::axi4_master_out_type> vmsto;
        TStructArray<types_amba::axi4_slave_in_type> vslvi;
        TStructArray<types_amba::axi4_slave_out_type> vslvo;
        Logic vb_ar_midx;
        Logic vb_aw_midx;
        Logic vb_ar_sidx;
        Logic vb_aw_sidx;
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

    class bus0_xmst_in_vector_miss : public types_amba::axi4_master_in_type {
     public:
        bus0_xmst_in_vector_miss(GenObject *parent, const char *name, const char *descr="")
            : axi4_master_in_type(parent, name, descr) {
            type_ = std::string("bus0_xmst_in_vector_miss");
            setStrDepth("ADD(CFG_BUS0_XMST_TOTAL,1)");
            
            registerCfgType(name);                  // will be registered if name == ""
            if (name[0]) {
                std::string strtype = getType();
                SCV_get_cfg_parameter(strtype);   // to trigger dependecy array
            }
        }
        virtual bool isTypedef() override { return true; }
        virtual bool isVector() override { return true; }
        virtual bool isSignal() override { return true; }
        virtual std::string generate() override { return std::string("axi4_master_in_type"); }
    };

    class bus0_xmst_out_vector_miss : public types_amba::axi4_master_out_type {
     public:
        bus0_xmst_out_vector_miss(GenObject *parent, const char *name, const char *descr="")
            : axi4_master_out_type(parent, name, descr) {
            type_ = std::string("bus0_xmst_out_vector_miss");
            setStrDepth("ADD(CFG_BUS0_XMST_TOTAL,1)");
            
            registerCfgType(name);                  // will be registered if name == ""
            if (name[0]) {
                std::string strtype = getType();
                SCV_get_cfg_parameter(strtype);   // to trigger dependecy array
            }
        }
        virtual bool isTypedef() override { return true; }
        virtual bool isVector() override { return true; }
        virtual bool isSignal() override { return true; }
        virtual std::string generate() override { return std::string("axi4_master_out_type"); }
    };

    class bus0_xslv_in_vector_miss : public types_amba::axi4_slave_in_type {
     public:
        bus0_xslv_in_vector_miss(GenObject *parent, const char *name, const char *descr="")
            : axi4_slave_in_type(parent, name, descr) {
            type_ = std::string("bus0_xslv_in_vector_miss");
            setStrDepth("ADD(CFG_BUS0_XSLV_TOTAL,1)");
            
            registerCfgType(name);                  // will be registered if name == ""
            if (name[0]) {
                std::string strtype = getType();
                SCV_get_cfg_parameter(strtype);   // to trigger dependecy array
            }
        }
        virtual bool isTypedef() override { return true; }
        virtual bool isVector() override { return true; }
        virtual bool isSignal() override { return true; }
        virtual std::string generate() override { return std::string("axi4_slave_in_type"); }
    };

    class bus0_xslv_out_vector_miss : public types_amba::axi4_slave_out_type {
     public:
        bus0_xslv_out_vector_miss(GenObject *parent, const char *name, const char *descr="")
            : axi4_slave_out_type(parent, name, descr) {
            type_ = std::string("bus0_xslv_out_vector_miss");
            setStrDepth("ADD(CFG_BUS0_XSLV_TOTAL,1)");
            
            registerCfgType(name);                  // will be registered if name == ""
            if (name[0]) {
                std::string strtype = getType();
                SCV_get_cfg_parameter(strtype);   // to trigger dependecy array
            }
        }
        virtual bool isTypedef() override { return true; }
        virtual bool isVector() override { return true; }
        virtual bool isSignal() override { return true; }
        virtual std::string generate() override { return std::string("axi4_slave_out_type"); }
    };

 public:
    // io:
    InPort i_clk;
    InPort i_nrst;
    OutStruct<types_amba::dev_config_type> o_cfg;
    InStruct<types_bus0::bus0_xmst_out_vector> i_xmsto;
    OutStruct<types_bus0::bus0_xmst_in_vector> o_xmsti;
    InStruct<types_bus0::bus0_xslv_out_vector> i_xslvo;
    OutStruct<types_bus0::bus0_xslv_in_vector> o_xslvi;
    OutStruct<types_bus0::bus0_mapinfo_vector> o_mapinfo;

    TextLine _miss0_;
    bus0_xmst_in_vector_miss bus0_xmst_in_vector_miss_def_;
    bus0_xmst_out_vector_miss bus0_xmst_out_vector_miss_def_;
    bus0_xslv_in_vector_miss bus0_xslv_in_vector_miss_def_;
    bus0_xslv_out_vector_miss bus0_xslv_out_vector_miss_def_;

    types_amba::mapinfo_type wb_def_mapinfo;
    types_amba::axi4_slave_in_type wb_def_xslvi;
    types_amba::axi4_slave_out_type wb_def_xslvo;
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
        axictrl_bus0_(this, "") {}

 private:
    axictrl_bus0 axictrl_bus0_;
};

