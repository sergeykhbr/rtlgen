// 
//  Copyright 2025 Sergey Khabarov, sergeykhbr@gmail.com
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
#include "river_cfg.h"
#include "types_river.h"

using namespace sysvc;

class l1_dma_snoop : public ModuleObject {
 public:
    l1_dma_snoop(GenObject *parent, const char *name, const char *comment);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject* parent)
            : CombinationalProcess(parent, "comb"),
            v_resp_mem_valid(this, "v_resp_mem_valid", "1"),
            v_mem_er_load_fault(this, "v_mem_er_load_fault", "1"),
            v_mem_er_store_fault(this, "v_mem_er_store_fault", "1"),
            v_next_ready(this, "v_next_ready", "1"),
            vmsto(this, "vmsto", "axi4_master_out_none", NO_COMMENT),
            v_snoop_next_ready(this, "v_snoop_next_ready", "1"),
            req_snoop_valid(this, "req_snoop_valid", "1"),
            vb_req_snoop_addr(this, "vb_req_snoop_addr", "CFG_CPU_ADDR_BITS", "'0", NO_COMMENT),
            vb_req_snoop_type(this, "vb_req_snoop_type", "SNOOP_REQ_TYPE_BITS", "'0", NO_COMMENT),
            v_cr_valid(this, "v_cr_valid", "1"),
            vb_cr_resp(this, "vb_cr_resp", "5", "'0", NO_COMMENT),
            v_cd_valid(this, "v_cd_valid", "1"),
            vb_cd_data(this, "vb_cd_data", "L1CACHE_LINE_BITS", "'0", NO_COMMENT) {
        }
     public:
        Logic v_resp_mem_valid;
        Logic v_mem_er_load_fault;
        Logic v_mem_er_store_fault;
        Logic v_next_ready;
        StructVar<types_river::axi4_l1_out_type> vmsto;
        Logic v_snoop_next_ready;
        Logic req_snoop_valid;
        Logic vb_req_snoop_addr;
        Logic vb_req_snoop_type;
        Logic v_cr_valid;
        Logic vb_cr_resp;
        Logic v_cd_valid;
        Logic vb_cd_data;
    };

    void proc_comb();

    class reqtype2arsnoop_func : public FunctionObject {
     public:
        reqtype2arsnoop_func(GenObject *parent);
        virtual std::string getType() override { return ret.getType(); }
        virtual void getArgsList(std::list<GenObject *> &args) {
            args.push_back(&reqtype);
        }
        virtual GenObject *getpReturn() { return &ret; }
     protected:
        Logic ret;
        Logic reqtype;
    };

    class reqtype2awsnoop_func : public FunctionObject {
     public:
        reqtype2awsnoop_func(GenObject *parent);
        virtual std::string getType() override { return ret.getType(); }
        virtual void getArgsList(std::list<GenObject *> &args) {
            args.push_back(&reqtype);
        }
        virtual GenObject *getpReturn() { return &ret; }
     protected:
        Logic ret;
        Logic reqtype;
    };

public:
    TmplParamI32D abits;
    DefParamI32D userbits;
    DefParamLogic base_offset;
    DefParamBOOL coherence_ena;
    // Ports:
    InPort i_nrst;
    InPort i_clk;
    OutPort o_req_mem_ready;
    InPort i_req_mem_path;
    InPort i_req_mem_valid;
    InPort i_req_mem_type;
    InPort i_req_mem_size;
    InPort i_req_mem_addr;
    InPort i_req_mem_strob;
    InPort i_req_mem_data;
    OutPort o_resp_mem_path;
    OutPort o_resp_mem_valid;
    OutPort o_resp_mem_load_fault;
    OutPort o_resp_mem_store_fault;
    OutPort o_resp_mem_data;
    TextLine _dsnoop0_;
    OutPort o_req_snoop_valid;
    OutPort o_req_snoop_type;
    InPort i_req_snoop_ready;
    OutPort o_req_snoop_addr;
    OutPort o_resp_snoop_ready;
    InPort i_resp_snoop_valid;
    InPort i_resp_snoop_data;
    InPort i_resp_snoop_flags;
    InStruct<types_river::axi4_l1_in_type> i_msti;
    OutStruct<types_river::axi4_l1_out_type> o_msto;

    ParamLogic state_idle;
    ParamLogic state_ar;
    ParamLogic state_r;
    ParamLogic state_aw;
    ParamLogic state_w;
    ParamLogic state_b;

    ParamLogic snoop_idle;
    ParamLogic snoop_ac_wait_accept;
    ParamLogic snoop_cr;
    ParamLogic snoop_cr_wait_accept;
    ParamLogic snoop_cd;
    ParamLogic snoop_cd_wait_accept;

    // Signals:
    RegSignal state;
    RegSignal req_addr;
    RegSignal req_path;
    RegSignal req_cached;
    RegSignal req_wdata;
    RegSignal req_wstrb;
    RegSignal req_size;
    RegSignal req_prot;
    RegSignal req_ar_snoop;
    RegSignal req_aw_snoop;
    RegSignal snoop_state;
    RegSignal ac_addr;
    RegSignal ac_snoop;
    RegSignal cr_resp;
    RegSignal req_snoop_type;
    RegSignal resp_snoop_data;
    RegSignal cache_access;
    RegSignal watchdog;

    // functions
    reqtype2arsnoop_func reqtype2arsnoop;
    reqtype2awsnoop_func reqtype2awsnoop;
    // Sub-module instances:
    // process
    CombProcess comb;
};

class l1_dma_snoop_file : public FileObject {
 public:
    l1_dma_snoop_file(GenObject *parent) :
        FileObject(parent, "l1_dma_snoop"),
        l1_dma_snoop_(this, "l1_dma_snoop", NO_COMMENT) { }

 private:
    l1_dma_snoop l1_dma_snoop_;
};

