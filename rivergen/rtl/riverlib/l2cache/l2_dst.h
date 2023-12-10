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

class L2Destination : public ModuleObject {
 public:
    L2Destination(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            vcoreo(this, "vcoreo", "ADD(CFG_SLOT_L1_TOTAL,1)", "axi4_l1_out_none", NO_COMMENT),
            vlxi(this, "vlxi", "CFG_SLOT_L1_TOTAL", "axi4_l1_in_none", NO_COMMENT),
            vb_src_aw(this, "vb_src_aw", "CFG_SLOT_L1_TOTAL", "'0", NO_COMMENT),
            vb_src_ar(this, "vb_src_ar", "CFG_SLOT_L1_TOTAL", "'0", NO_COMMENT),
            vb_broadband_mask_full(this, "vb_broadband_mask_full", "ADD(CFG_SLOT_L1_TOTAL,1)", "'0", NO_COMMENT),
            vb_broadband_mask(this, "vb_broadband_mask", "ADD(CFG_SLOT_L1_TOTAL,1)", "'0", NO_COMMENT),
            vb_ac_valid(this, "vb_ac_valid", "ADD(CFG_SLOT_L1_TOTAL,1)", "'0", NO_COMMENT),
            vb_cr_ready(this, "vb_cr_ready", "ADD(CFG_SLOT_L1_TOTAL,1)", "'0", NO_COMMENT),
            vb_cd_ready(this, "vb_cd_ready", "ADD(CFG_SLOT_L1_TOTAL,1)", "'0", NO_COMMENT),
            vb_srcid(this, "vb_srcid", "3", "'0", NO_COMMENT),
            v_req_valid(this, "v_req_valid", "1"),
            vb_req_type(this, "vb_req_type", "L2_REQ_TYPE_BITS", "'0", NO_COMMENT) {
        }

     public:
        StructVarArray<types_river::axi4_l1_out_type> vcoreo;
        StructVarArray<types_river::axi4_l1_in_type> vlxi;
        Logic vb_src_aw;
        Logic vb_src_ar;
        Logic vb_broadband_mask_full;
        Logic vb_broadband_mask;
        Logic vb_ac_valid;
        Logic vb_cr_ready;
        Logic vb_cd_ready;
        Logic vb_srcid;
        Logic v_req_valid;
        Logic vb_req_type;
    };

    void proc_comb();

 public:
    // io:
    InPort i_clk;
    InPort i_nrst;
    InPort i_resp_valid;
    InPort i_resp_rdata;
    InPort i_resp_status;
    InStruct<types_river::axi4_l1_out_vector> i_l1o;
    OutStruct<types_river::axi4_l1_in_vector> o_l1i;
    TextLine _0_;
    InPort i_req_ready;
    OutPort o_req_valid;
    OutPort o_req_type;
    OutPort o_req_addr;
    OutPort o_req_size;
    OutPort o_req_prot;
    OutPort o_req_wdata;
    OutPort o_req_wstrb;

    ParamLogic Idle;
    ParamLogic CacheReadReq;
    ParamLogic CacheWriteReq;
    ParamLogic ReadMem;
    ParamLogic WriteMem;
    ParamLogic snoop_ac;
    ParamLogic snoop_cr;
    ParamLogic snoop_cd;

    RegSignal state;
    RegSignal srcid;
    RegSignal req_addr;
    RegSignal req_size;
    RegSignal req_prot;
    RegSignal req_src;
    RegSignal req_type;
    RegSignal req_wdata;
    RegSignal req_wstrb;
    RegSignal ac_valid;
    RegSignal cr_ready;
    RegSignal cd_ready;

    CombProcess comb;
};

class l2_dst_file : public FileObject {
 public:
    l2_dst_file(GenObject *parent) :
        FileObject(parent, "l2_dst"),
        l2_dst_(this, "L2Destination") {}

 private:
    L2Destination l2_dst_;
};

