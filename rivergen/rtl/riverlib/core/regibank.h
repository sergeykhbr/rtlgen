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

class RegIntBank : public ModuleObject {
 public:
    RegIntBank(GenObject *parent);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) : ProcObject(parent, "comb") {
            RegIntBank *p = static_cast<RegIntBank *>(parent);
            p->proc_comb();
        }
    };

    void proc_comb();

 protected:
    InPort i_clk;
    InPort i_nrst;
    InPort i_radr1;
    OutPort o_rdata1;
    OutPort o_rtag1;
    InPort i_radr2;
    OutPort o_rdata2;
    OutPort o_rtag2;
    InPort i_waddr;
    InPort i_wena;
    InPort i_wtag;
    InPort i_wdata;
    InPort i_inorder;
    OutPort o_ignored;
    InPort i_dport_addr;
    InPort i_dport_ena;
    InPort i_dport_write;
    InPort i_dport_wdata;
    OutPort o_dport_rdata;
    OutPort o_ra;
    OutPort o_sp;

    class RegArrayType : public ArrayObject {
     public:
        RegArrayType(GenObject *parent) : ArrayObject(parent, "RegArrayType", "arr", 32),
        val(this, "val", "RISCV_ARCH"),
        tag(this, "tag", "CFG_REG_TAG_WIDTH") {}
     public:
        Signal val;
        Signal tag;
    };
    
    Reg reg;

    // process should be intialized last to make all signals available
    CombProcess comb;
};

class regibank_file : public FileObject {
 public:
    regibank_file(GenObject *parent);

 private:
    RegIntBank mod_;
};

