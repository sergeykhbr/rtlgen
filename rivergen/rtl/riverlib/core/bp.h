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

class BranchPredictor : public ModuleObject {
 public:
    BranchPredictor(GenObject *parent, river_cfg *cfg);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) : ProcObject(parent, "comb") {
            BranchPredictor *p = static_cast<BranchPredictor *>(parent);
            p->proc_comb();
        }
    };

    void proc_comb();

 protected:
    river_cfg *cfg_;

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

    class PreDecSignals {
     public:
        PreDecSignals(GenObject *parent) :
            c_valid(parent, "c_valid", "1"),
            addr(parent, "addr", "CFG_CPU_ADDR_BITS"),
            data(parent, "data", "32"),
            jmp(parent, "jmp", "1"),
            pc(parent, "pc", "CFG_CPU_ADDR_BITS"),
            npc(parent, "npc", "CFG_CPU_ADDR_BITS") {}
     public:
        Signal c_valid;
        Signal addr;
        Signal data;
        Signal jmp;
        Signal pc;
        Signal npc;
    };
    
    class PreDecStructDefinition : public StructObject,
                                   public PreDecSignals {
     public:
        PreDecStructDefinition(GenObject *parent, int idx, const char *comment="")
            : StructObject(parent, "PreDecStruct", "", idx, comment), PreDecSignals(this) {}
    } PreDecTypeDef_;

    class PreDecStructArray : public ArrayObject {
     public:
       PreDecStructArray(GenObject *parent, const char *name, const char *comment="")
            : ArrayObject(parent, name, "2", comment) {
            arr_ = new PreDecStructDefinition *[depth_.getValue()];
            for (int i = 0; i < static_cast<int>(depth_.getValue()); i++) {
                arr_[i] = new PreDecStructDefinition(this, i);
            }
        }
        virtual GenObject *getItem() { return arr_[0]; }
        PreDecStructDefinition **arr_;
    } wb_pd;

    Signal w_btb_e;
    Signal w_btb_we;
    Signal wb_btb_we_pc;
    Signal wb_btb_we_npc;
    Signal wb_start_pc;
    Signal wb_npc;
    Signal wb_bp_exec;


    // process should be intialized last to make all signals available
    CombProcess comb;
};

class bp_file : public FileObject {
 public:
    bp_file(GenObject *parent, river_cfg *cfg) : FileObject(parent, "bp"),
    bp_(this, cfg) {}

 private:
    BranchPredictor bp_;
};

