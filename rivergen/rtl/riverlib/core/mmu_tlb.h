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
#include "../cache/mem/ram.h"

using namespace sysvc;

class MmuTlb : public ModuleObject {
 public:
    MmuTlb(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb") {
            Operation::start(this);
            MmuTlb *p = static_cast<MmuTlb *>(getParent());
            p->proc_comb();
        }
    };

    void proc_comb();

 public:
    InPort i_clk;
    InPort i_adr;
    InPort i_wena;
    InPort i_wdata;
    OutPort o_rdata;

 protected:
    class MemType : public StructObject {
     public:
        MemType(GenObject *parent, int idx, const char *comment="")
            : StructObject(parent, "MemType", "", idx, comment),
            rdata(this, "rdata", "8"),
            wdata(this, "wdata", "8") {}
     public:
        Signal rdata;
        Signal wdata;
    } MemTypeDef_;

    TStructArray<MemType> wb_mem_data;


    // Sub-modules:
    ModuleArray<ram> mem;

    // process should be intialized last to make all signals available
    CombProcess comb;
};

class mmu_tlb_file : public FileObject {
 public:
    mmu_tlb_file(GenObject *parent) :
        FileObject(parent, "mmu_tlb"),
        MmuTlb_(this, "") {}

 private:
    MmuTlb MmuTlb_;
};

