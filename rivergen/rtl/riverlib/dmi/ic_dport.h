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
#include "../river_cfg.h"
#include "../types_river.h"

using namespace sysvc;

class ic_dport : public ModuleObject {
 public:
    ic_dport(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject* parent)
            : ProcObject(parent, "comb"),
            vb_hartsel(this, "vb_hartsel", "CFG_LOG2_CPU_MAX"),
            vb_cpu_mask(this, "vb_cpu_mask", "CFG_CPU_MAX"),
            vb_haltreq(this, "vb_haltreq", "CFG_CPU_MAX"),
            vb_resumereq(this, "vb_resumereq", "CFG_CPU_MAX"),
            vb_resethaltreq(this, "vb_resethaltreq", "CFG_CPU_MAX"),
            vb_hartreset(this, "vb_hartreset", "CFG_CPU_MAX"),
            vb_req_valid(this, "vb_req_valid", "CFG_CPU_MAX"),
            vb_req_ready(this, "vb_req_ready", "CFG_CPU_MAX"),
            vb_dporti(this, "vb_dporti"),
            vb_dporto(this, "vb_dporto"),
            v_req_accepted(this, "v_req_accepted", "1") {
        }
     public:
        Logic vb_hartsel;
        Logic vb_cpu_mask;
        Logic vb_haltreq;
        Logic vb_resumereq;
        Logic vb_resethaltreq;
        Logic vb_hartreset;
        Logic vb_req_valid;
        Logic vb_req_ready;
        types_river::dport_in_vector vb_dporti;
        types_river::dport_out_vector vb_dporto;
        Logic v_req_accepted;
    };

    void proc_comb();

public:
    // Ports:
    InPort i_clk;
    InPort i_nrst;
    TextLine _dmi0_;
    InPort i_hartsel;
    InPort i_haltreq;
    InPort i_resumereq;
    InPort i_resethaltreq;
    InPort i_hartreset;
    InPort i_dport_req_valid;
    InPort i_dport_req_type;
    InPort i_dport_addr;
    InPort i_dport_wdata;
    InPort i_dport_size;
    OutPort o_dport_req_ready;
    InPort i_dport_resp_ready;
    OutPort o_dport_resp_valid;
    OutPort o_dport_resp_error;
    OutPort o_dport_rdata;
    TextLine _core0_;
    OutStruct<types_river::dport_in_vector> o_dporti;
    InStruct<types_river::dport_out_vector> i_dporto;

    // param
    ParamLogic ALL_CPU_MASK;

    // regs
    RegSignal hartsel;

    // process
    CombProcess comb;
};

class ic_dport_file : public FileObject {
 public:
    ic_dport_file(GenObject *parent) :
        FileObject(parent, "ic_dport"),
        ic_dport_(this, "") { }

 private:
    ic_dport ic_dport_;
};

