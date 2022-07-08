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

using namespace sysvc;

class BpPreDecoder : public ModuleObject {
 public:
    BpPreDecoder(GenObject *parent, river_cfg *cfg);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) : ProcObject(parent, "comb"),
            vb_tmp(this, "vb_tmp", "32"),
            vb_npc(this, "vb_npc", "CFG_CPU_ADDR_BITS"),
            vb_pc(this, "vb_pc", "CFG_CPU_ADDR_BITS"),
            vb_jal_off(this, "vb_jal_off", "CFG_CPU_ADDR_BITS"),
            vb_jal_addr(this, "vb_jal_addr", "CFG_CPU_ADDR_BITS"),
            vb_branch_off(this, "vb_branch_off", "CFG_CPU_ADDR_BITS"),
            vb_branch_addr(this, "vb_branch_addr", "CFG_CPU_ADDR_BITS"),
            vb_c_j_off(this, "vb_c_j_off", "CFG_CPU_ADDR_BITS"),
            vb_c_j_addr(this, "vb_c_j_addr", "CFG_CPU_ADDR_BITS") {
            BpPreDecoder *p = static_cast<BpPreDecoder *>(parent);
            p->proc_comb();
        }
     public:
        Logic vb_tmp;
        Logic vb_npc;
        Logic vb_pc;
        Logic vb_jal_off;
        Logic vb_jal_addr;
        Logic vb_branch_off;
        Logic vb_branch_addr;
        Logic vb_c_j_off;
        Logic vb_c_j_addr;
    };

    void proc_comb();

 protected:
    river_cfg *cfg_;

    InPort i_c_valid;
    InPort i_addr;
    InPort i_data;
    InPort i_ra;
    OutPort o_jmp;
    OutPort o_pc;
    OutPort o_npc;

    Signal vb_npc;
    Signal v_jal;
    Signal v_branch;
    Signal v_c_j;
    Signal v_c_ret;

    // process should be intialized last to make all signals available
    CombProcess comb;
};

class bp_predec_file : public FileObject {
 public:
    bp_predec_file(GenObject *parent, river_cfg *cfg) : FileObject(parent, "bp_predec"),
    bp_predec_(this, cfg) {}

 private:
    BpPreDecoder bp_predec_;
};

