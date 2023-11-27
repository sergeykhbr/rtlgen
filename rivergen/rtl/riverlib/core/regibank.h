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

class RegIntBank : public ModuleObject {
 public:
    RegIntBank(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) : ProcObject(parent, "comb"),
            int_daddr("0", "int_daddr", this),
            int_waddr("0", "int_waddr", this),
            int_radr1("0", "int_radr1", this),
            int_radr2("0", "int_radr2", this),
            v_inordered("1", "v_inordered", "0", this),
            next_tag("CFG_REG_TAG_WIDTH", "next_tag", "'0", this) {
            RegIntBank *p = static_cast<RegIntBank *>(parent);
            Operation::start(this);
            p->proc_comb();
        }
    public:
        I32D int_daddr;
        I32D int_waddr;
        I32D int_radr1;
        I32D int_radr2;
        Logic v_inordered;
        Logic next_tag;
    };

    void proc_comb();

 public:
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
    OutPort o_gp;
    OutPort o_tp;
    OutPort o_t0;
    OutPort o_t1;
    OutPort o_t2;
    OutPort o_fp;
    OutPort o_s1;
    OutPort o_a0;
    OutPort o_a1;
    OutPort o_a2;
    OutPort o_a3;
    OutPort o_a4;
    OutPort o_a5;
    OutPort o_a6;
    OutPort o_a7;
    OutPort o_s2;
    OutPort o_s3;
    OutPort o_s4;
    OutPort o_s5;
    OutPort o_s6;
    OutPort o_s7;
    OutPort o_s8;
    OutPort o_s9;
    OutPort o_s10;
    OutPort o_s11;
    OutPort o_t3;
    OutPort o_t4;
    OutPort o_t5;
    OutPort o_t6;

 protected:
    class RegValueType : public StructObject {
     public:
        RegValueType(GenObject *parent, const char *name, const char *comment="")
            : StructObject(parent, "RegValueType", name, comment),
            val(this, "val", "RISCV_ARCH"),
            tag(this, "tag", "CFG_REG_TAG_WIDTH") {}
     public:
        RegSignal val;
        RegSignal tag;
    } RegValueTypeDef_;

    class RegTableType : public TStructArray<RegValueType> {
     public:
        RegTableType(GenObject *parent, const char *name)
            : TStructArray<RegValueType>(parent, name, "REGS_TOTAL", true) {
        }
    };

    RegTableType arr;

    // process should be intialized last to make all signals available
    CombProcess comb;
};

class regibank_file : public FileObject {
 public:
    regibank_file(GenObject *parent) :
        FileObject(parent, "regibank"),
        mod_(this, "") { }

 private:
    RegIntBank mod_;
};

