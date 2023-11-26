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

using namespace sysvc;

class BpPreDecoder : public ModuleObject {
 public:
    BpPreDecoder(GenObject *parent, const char *name, const char *depth);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) : ProcObject(parent, "comb"),
            vb_tmp(this, "vb_tmp", "32"),
            vb_npc(this, "vb_npc", "RISCV_ARCH"),
            vb_pc(this, "vb_pc", "RISCV_ARCH"),
            vb_jal_off(this, "vb_jal_off", "RISCV_ARCH"),
            vb_jal_addr(this, "vb_jal_addr", "RISCV_ARCH"),
            vb_branch_off(this, "vb_branch_off", "RISCV_ARCH"),
            vb_branch_addr(this, "vb_branch_addr", "RISCV_ARCH"),
            vb_c_j_off(this, "vb_c_j_off", "RISCV_ARCH"),
            vb_c_j_addr(this, "vb_c_j_addr", "RISCV_ARCH") {
            BpPreDecoder *p = static_cast<BpPreDecoder *>(parent);
            Operation::start(this);
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

 public:
    InPort i_c_valid;
    InPort i_addr;
    InPort i_data;
    InPort i_ra;
    OutPort o_jmp;
    OutPort o_pc;
    OutPort o_npc;

 protected:
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
    bp_predec_file(GenObject *parent) : FileObject(parent, "bp_predec"),
    bp_predec_(this, "BpPreDecoder", "0") {}

 private:
    BpPreDecoder bp_predec_;
};

