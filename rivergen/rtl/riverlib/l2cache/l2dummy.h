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
#include "../river_cfg.h"
#include "../types_river.h"

using namespace sysvc;

class L2Dummy : public ModuleObject {
 public:
    L2Dummy(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            vl1o(this, "vl1o", "CFG_SLOT_L1_TOTAL", NO_COMMENT),
            vlxi(this, "vlxi", "CFG_SLOT_L1_TOTAL", NO_COMMENT),
            vl2o(this, "vl2o", NO_COMMENT),
            vb_src_aw(this, "vb_src_aw", "CFG_SLOT_L1_TOTAL"),
            vb_src_ar(this, "vb_src_ar", "CFG_SLOT_L1_TOTAL"),
            vb_srcid("0", "vb_srcid", this),
            v_selected(this, "v_selected", "1") {
        }

     public:
        StructArray<types_river::axi4_l1_out_type> vl1o;
        StructArray<types_river::axi4_l1_in_type> vlxi;
        types_river::axi4_l2_out_type vl2o;
        Logic vb_src_aw;
        Logic vb_src_ar;
        I32D vb_srcid;
        Logic v_selected;
    };

    void proc_comb();


 public:
    // io:
    InPort i_clk;
    InPort i_nrst;
    InStruct<types_river::axi4_l1_out_vector> i_l1o;
    OutStruct<types_river::axi4_l1_in_vector> o_l1i;
    InStruct<types_river::axi4_l2_in_type> i_l2i;
    OutStruct<types_river::axi4_l2_out_type> o_l2o;
    InPort i_flush_valid;

    // params
    ParamLogic Idle;
    ParamLogic state_ar;
    ParamLogic state_r;
    ParamLogic l1_r_resp;
    ParamLogic state_aw;
    ParamLogic state_w;
    ParamLogic state_b;
    ParamLogic l1_w_resp;

    //regs
    RegSignal state;
    RegSignal srcid;
    RegSignal req_addr;
    RegSignal req_size;
    RegSignal req_prot;
    RegSignal req_lock;
    RegSignal req_id;
    RegSignal req_user;
    RegSignal req_wdata;
    RegSignal req_wstrb;
    RegSignal rdata;
    RegSignal resp;

    CombProcess comb;
};

class l2dummy_file : public FileObject {
 public:
    l2dummy_file(GenObject *parent) :
        FileObject(parent, "l2dummy"),
        l2dummy_(this, "") {}

 private:
    L2Dummy l2dummy_;
};

