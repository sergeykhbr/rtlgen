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
#include "bp_predec.h"
#include "bp_btb.h"

using namespace sysvc;

class BranchPredictor : public ModuleObject {
 public:
    BranchPredictor(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            vb_addr(this, "vb_addr", "RISCV_ARCH", "CFG_BP_DEPTH"),
            vb_piped(this, "vb_piped", "SUB(RISCV_ARCH,2)", "4"),
            vb_fetch_npc(this, "vb_fetch_npc", "RISCV_ARCH"),
            v_btb_we(this, "v_btb_we", "1"),
            vb_btb_we_pc(this, "vb_btb_we_pc", "RISCV_ARCH"),
            vb_btb_we_npc(this, "vb_btb_we_npc", "RISCV_ARCH"),
            vb_hit(this, "vb_hit", "4"),
            vb_ignore_pd(this, "vb_ignore_pd", "2") {
            proc_comb();
        }

        void proc_comb();

     protected:
        WireArray<Logic> vb_addr;
        WireArray<Logic> vb_piped;
        Logic vb_fetch_npc;
        Logic v_btb_we;
        Logic vb_btb_we_pc;
        Logic vb_btb_we_npc;
        Logic vb_hit;
        Logic vb_ignore_pd;
    };

 public:
    InPort i_clk;
    InPort i_nrst;
    InPort i_flush_pipeline;
    InPort i_resp_mem_valid;
    InPort i_resp_mem_addr;
    InPort i_resp_mem_data;
    InPort i_e_jmp;
    InPort i_e_pc;
    InPort i_e_npc;
    InPort i_ra;
    OutPort o_f_valid;
    OutPort o_f_pc;
    InPort i_f_requested_pc;
    InPort i_f_fetching_pc;
    InPort i_f_fetched_pc;
    InPort i_d_pc;

 protected:
    
    class PreDecType : public StructObject {
     public:
        PreDecType(GenObject *parent, const char *name="", int idx=-1, const char *comment="")
            : StructObject(parent, "PreDecType", name, idx, comment),
            c_valid(this, "c_valid", "1"),
            addr(this, "addr", "RISCV_ARCH"),
            data(this, "data", "32"),
            jmp(this, "jmp", "1"),
            pc(this, "pc", "RISCV_ARCH"),
            npc(this, "npc", "RISCV_ARCH") {}
     public:
        Signal c_valid;
        Signal addr;
        Signal data;
        Signal jmp;
        Signal pc;
        Signal npc;
    } PreDecTypeDef_;

    TStructArray<PreDecType> wb_pd;

    Signal w_btb_e;
    Signal w_btb_we;
    Signal wb_btb_we_pc;
    Signal wb_btb_we_npc;
    Signal wb_start_pc;
    Signal wb_npc;
    Signal wb_bp_exec;


    // process should be intialized last to make all signals available
    CombProcess comb;

    // Sub-module instances:
    BpBTB btb;
    ModuleArray<BpPreDecoder> predec;
};

class bp_file : public FileObject {
 public:
    bp_file(GenObject *parent) : FileObject(parent, "bp"),
    bp_(this, "") {}

 private:
    BranchPredictor bp_;
};

