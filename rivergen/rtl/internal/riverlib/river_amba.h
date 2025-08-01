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
#include "l1_dma_snoop.h"

using namespace sysvc;

class RiverAmba : public ModuleObject {
 public:
    RiverAmba(GenObject *parent, const char *name, const char *depth);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject* parent)
            : CombinationalProcess(parent, "comb"),
            vdporto(this, "vdporto", "dport_out_none", NO_COMMENT) {
        }
     public:
        StructVar<types_river::dport_out_type> vdporto;
    };

    void proc_comb();

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
    Signal resp_mem_path_i;
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


    // functions
    // Sub-module instances:
    RiverTop river0;
    l1_dma_snoop l1dma0;
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

