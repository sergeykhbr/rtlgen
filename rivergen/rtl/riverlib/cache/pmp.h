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

class PMP : public ModuleObject {
 public:
    PMP(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            v_r(this, "v_r", "1"),
            v_w(this, "v_w", "1"),
            v_x(this, "v_x", "1"),
            vb_start_addr(this, "vb_start_addr", "RISCV_ARCH"),
            vb_end_addr(this, "vb_end_addr", "RISCV_ARCH"),
            vb_flags(this, "vb_flags", "CFG_PMP_FL_TOTAL") {
        }

     public:
        Logic v_r;
        Logic v_w;
        Logic v_x;
        Logic vb_start_addr;
        Logic vb_end_addr;
        Logic vb_flags;
    };

    void proc_comb();

 public:
    InPort i_clk;
    InPort i_nrst;
    InPort i_ena;
    InPort i_iaddr;
    InPort i_daddr;
    InPort i_we;
    InPort i_region;
    InPort i_start_addr;
    InPort i_end_addr;
    InPort i_flags;
    OutPort o_r;
    OutPort o_w;
    OutPort o_x;

    class PmpTableItemType : public StructObject {
     public:
        PmpTableItemType(GenObject *parent, const char *name, const char *comment)
            : StructObject(parent, "PmpTableItemType", name, "", comment),
            start_addr(this, "start_addr", "RISCV_ARCH"),
            end_addr(this, "end_addr", "RISCV_ARCH"),
            flags(this, "flags", "CFG_PMP_FL_TOTAL", "0") {
        }
     public:
        Signal start_addr;
        Signal end_addr;
        Signal flags;
    } PmpTableItemTypeDef_;

    class PmpTableType : public RegStructArray<PmpTableItemType> {
     public:
        PmpTableType(GenObject *parent, const char *name) :
            RegStructArray<PmpTableItemType>(parent, name, "CFG_PMP_TBL_SIZE", NO_COMMENT) {
        }
    };
    PmpTableType tbl;
    CombProcess comb;
};

class pmp_file : public FileObject {
 public:
    pmp_file(GenObject *parent) :
        FileObject(parent, "pmp"),
        pmp_(this, "PMP") {}

 private:
    PMP pmp_;
};

