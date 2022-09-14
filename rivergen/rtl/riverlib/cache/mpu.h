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

class MPU : public ModuleObject {
 public:
    MPU(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            v_iflags(this, "v_iflags", "CFG_MPU_FL_TOTAL"),
            v_dflags(this, "v_dflags", "CFG_MPU_FL_TOTAL"),
            vb_addr(this, "vb_addr", "CFG_CPU_ADDR_BITS"),
            vb_mask(this, "vb_mask", "CFG_CPU_ADDR_BITS"),
            vb_flags(this, "vb_flags", "CFG_MPU_FL_TOTAL") {
        }

     public:
        Logic v_iflags;
        Logic v_dflags;
        Logic vb_addr;
        Logic vb_mask;
        Logic vb_flags;
    };

    void proc_comb();

 public:
    InPort i_clk;
    InPort i_nrst;
    InPort i_iaddr;
    InPort i_daddr;
    InPort i_region_we;
    InPort i_region_idx;
    InPort i_region_addr;
    InPort i_region_mask;
    InPort i_region_flags;
    OutPort o_iflags;
    OutPort o_dflags;

    class MpuTableItemType : public StructObject {
     public:
        MpuTableItemType(GenObject *parent, const char *name="", int idx=-1, const char *comment="")
            : StructObject(parent, "MpuTableItemType", name, idx, comment),
            addr(this, "addr", "CFG_CPU_ADDR_BITS"),
            mask(this, "mask", "CFG_CPU_ADDR_BITS"),
            flags(this, "flags", "CFG_MPU_FL_TOTAL", "-1", "Enable all regions by default") {
        }
     public:
        Signal addr;
        Signal mask;
        Signal flags;
    } MpuTableItemTypeDef_;

    class MpuTableType : public TStructArray<MpuTableItemType> {
     public:
        MpuTableType(GenObject *parent, const char *name) :
            TStructArray<MpuTableItemType>(parent, "", name, "CFG_MPU_TBL_SIZE") {
            setReg();
        }
    };
    MpuTableType tbl;
    CombProcess comb;
};

class mpu_file : public FileObject {
 public:
    mpu_file(GenObject *parent) :
        FileObject(parent, "mpu"),
        mpu_(this, "") {}

 private:
    MPU mpu_;
};

