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
#include "river_cfg.h"
#include "types_river.h"
#include "river_top.h"

using namespace sysvc;

class RiverAmba : public ModuleObject {
 public:
    RiverAmba(GenObject *parent, const char *name, const char *depth);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject* parent)
            : ProcObject(parent, "comb"),
            v_resp_mem_valid(this, "v_resp_mem_valid", "1"),
            v_mem_er_load_fault(this, "v_mem_er_load_fault", "1"),
            v_mem_er_store_fault(this, "v_mem_er_store_fault", "1"),
            v_next_ready(this, "v_next_ready", "1"),
            vmsto(this, "vmsto", "axi4_l1_out_none", NO_COMMENT),
            vdporto(this, "vdporto", "dport_out_none", NO_COMMENT),
            _snoop0_(this, "snoop processing"),
            v_snoop_next_ready(this, "v_snoop_next_ready", "1"),
            req_snoop_valid(this, "req_snoop_valid", "1"),
            vb_req_snoop_addr(this, "vb_req_snoop_addr", "CFG_CPU_ADDR_BITS"),
            vb_req_snoop_type(this, "vb_req_snoop_type", "SNOOP_REQ_TYPE_BITS"),
            v_cr_valid(this, "v_cr_valid", "1"),
            vb_cr_resp(this, "vb_cr_resp", "5"),
            v_cd_valid(this, "v_cd_valid", "1"),
            vb_cd_data(this, "vb_cd_data", "L1CACHE_LINE_BITS") {
        }
     public:
        Logic v_resp_mem_valid;
        Logic v_mem_er_load_fault;
        Logic v_mem_er_store_fault;
        Logic v_next_ready;
        StructVar<types_river::axi4_l1_out_type> vmsto;
        StructVar<types_river::dport_out_type> vdporto;
        TextLine _snoop0_;
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
    DefParamUI32D hartid;
    DefParamBOOL fpu_ena;
    DefParamBOOL coherence_ena;
    DefParamBOOL tracer_ena;
    // Ports:
    InPort i_clk;
    InPort i_nrst;
    InPort i_mtimer;
    InStruct<types_river::axi4_l1_in_type> i_msti;
    OutStruct<types_river::axi4_l1_out_type> o_msto;
    InStruct<types_river::dport_in_type> i_dport;
    OutStruct<types_river::dport_out_type> o_dport;
    InPort i_irq_pending;
    OutPort o_flush_l2;                                // Flush L2 after D$ has been finished
    OutPort o_halted;
    OutPort o_available;
    InPort i_progbuf;

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
    Signal req_mem_ready_i;
    Signal req_mem_path_o;
    Signal req_mem_valid_o;
    Signal req_mem_type_o;
    Signal req_mem_size_o;
    Signal req_mem_addr_o;
    Signal req_mem_strob_o;
    Signal req_mem_data_o;
    Signal resp_mem_data_i;
    Signal resp_mem_valid_i;
    Signal resp_mem_load_fault_i;
    Signal resp_mem_store_fault_i;
    TextLine _dsnoop0_;
    Signal req_snoop_valid_i;
    Signal req_snoop_type_i;
    Signal req_snoop_ready_o;
    Signal req_snoop_addr_i;
    Signal resp_snoop_ready_i;
    Signal resp_snoop_valid_o;
    Signal resp_snoop_data_o;
    Signal resp_snoop_flags_o;
    Signal w_dporti_haltreq;
    Signal w_dporti_resumereq;
    Signal w_dporti_resethaltreq;
    Signal w_dporti_hartreset;
    Signal w_dporti_req_valid;
    Signal wb_dporti_dtype;
    Signal wb_dporti_addr;
    Signal wb_dporti_wdata;
    Signal wb_dporti_size;
    Signal w_dporti_resp_ready;
    Signal w_dporto_req_ready;
    Signal w_dporto_resp_valid;
    Signal w_dporto_resp_error;
    Signal wb_dporto_rdata;

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

    // functions
    reqtype2arsnoop_func reqtype2arsnoop;
    reqtype2awsnoop_func reqtype2awsnoop;
    // Sub-module instances:
    RiverTop river0;
    // process
    CombProcess comb;
};

class river_amba_file : public FileObject {
 public:
    river_amba_file(GenObject *parent) :
        FileObject(parent, "river_amba"),
        RiverAmba_(this, "RiverAmba", "0") { }

 private:
    RiverAmba RiverAmba_;
};

