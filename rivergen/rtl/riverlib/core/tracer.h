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

class Tracer : public ModuleObject {
 public:
    Tracer(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) : ProcObject(parent, "comb") {
        }
    public:
    };

    void proc_comb();

 public:
    DefParamString trace_file;
    InPort i_clk;
    InPort i_nrst;
    InPort i_dbg_executed_cnt;
    InPort i_e_valid;
    InPort i_e_pc;
    InPort i_e_instr;
    InPort i_e_wena;
    InPort i_e_waddr;
    InPort i_e_wdata;
    InPort i_e_memop_valid;
    InPort i_e_memop_type;
    InPort i_e_memop_size;
    InPort i_e_memop_addr;
    InPort i_e_memop_wdata;
    InPort i_e_flushd;
    InPort i_m_pc;
    InPort i_m_valid;
    InPort i_m_memop_ready;
    InPort i_m_wena;
    InPort i_m_waddr;
    InPort i_m_wdata;
    InPort i_reg_ignored;

 protected:
    ParamI32D TRACE_TBL_SZ;

 protected:
    class MemopActionType : public StructObject {
     public:
        MemopActionType(GenObject *parent, int idx, const char *comment="")
            : StructObject(parent, "MemopActionType", "", idx, comment),
            store(this, "store", "1", "0=load;1=store"),
            size(this, "size", "2"),
            memaddr(this, "memaddr", "64"),
            data(this, "data", "64"),
            regaddr(this, "regaddr", "6", "writeback address"),
            complete(this, "complete", "1"),
            sc_release(this, "sc_release", "1"),
            ignored(this, "ignored", "1") {}
     public:
        Signal store;
        Signal size;
        Signal memaddr;
        Signal data;
        Signal regaddr;
        Signal complete;
        Signal sc_release;
        Signal ignored;
    } MemopActionTypeDef_;

    class RegActionType : public StructObject {
     public:
        RegActionType(GenObject *parent, int idx, const char *comment="")
            : StructObject(parent, "RegActionType", "", idx, comment),
        waddr(this, "waddr", "6"),
        wres(this, "wres", "64") {}
     public:
        Signal waddr;
        Signal wres;
    } RegActionTypeDef_;


    TStructArray<MemopActionType> memaction;

    // process should be intialized last to make all signals available
    CombProcess comb;
};

class tracer_file : public FileObject {
 public:
    tracer_file(GenObject *parent) :
        FileObject(parent, "tracer"),
        m_(this, "") { }

 private:
    Tracer m_;
};

