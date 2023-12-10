// 
//  Copyright 2023 Sergey Khabarov, sergeykhbr@gmail.com
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
#include "../ambalib/types_amba.h"
#include "../ambalib/types_pnp.h"
#include "../ambalib/axi_slv.h"

using namespace sysvc;

class plic : public ModuleObject {
 public:
    plic(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

 protected:
     class plic_context_type : public StructObject {
     public:
        // Structure definition
        plic_context_type(GenObject *parent, const char *name, const char *comment)
            : StructObject(parent, "plic_context_type", name, comment),
            priority_th(this, "priority_th", "4", "0"),
            ie(this, "ie", "1024", "0", "interrupt enable per context"),
            ip_prio(this, "ip_prio", "MUL(4,1024)", "0", "interrupt pending priority per context"),
            prio_mask(this, "prio_mask", "16", "0", "pending interrupts priorites"),
            sel_prio(this, "sel_prio", "4", "0", "the most available priority"),
            irq_idx(this, "irq_idx", "10", "0", "currently selected most prio irq"),
            irq_prio(this, "irq_prio", "10", "0", "currently selected prio level") {}
     public:
        Logic priority_th;
        Logic ie;
        Logic ip_prio;
        Logic prio_mask;
        Logic sel_prio;
        Logic irq_idx;
        Logic irq_prio;
    };

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            vrdata(this, "vrdata", "CFG_SYSBUS_DATA_BITS", "'0", NO_COMMENT),
            vb_irq_idx(this, "vb_irq_idx", "10", "ctxmax", "Currently selected most prio irq"),
            vb_irq_prio(this, "vb_irq_prio", "10", "ctxmax", "Currently selected prio level"),
            vb_ctx(this, "vb_ctx", "ctxmax", NO_COMMENT),
            vb_src_priority(this, "vb_src_priority", "MUL(4,1024)"),
            vb_pending(this, "vb_pending", "1024"),
            vb_ip(this, "vb_ip", "ctxmax", "'0", NO_COMMENT),
            rctx_idx("0", "rctx_idx", this) {
        }

     public:
        Logic vrdata;
        WireArray<Logic> vb_irq_idx;
        WireArray<Logic> vb_irq_prio;
        StructArray<plic_context_type> vb_ctx;
        Logic vb_src_priority;
        Logic vb_pending;
        Logic vb_ip;
        I32D rctx_idx;
    };

    void proc_comb();

 public:
    TmplParamI32D ctxmax;
    TmplParamI32D irqmax;
    // io:
    InPort i_clk;
    InPort i_nrst;
    InStruct<types_amba::mapinfo_type> i_mapinfo;
    OutStruct<types_pnp::dev_config_type> o_cfg;
    InStruct<types_amba::axi4_slave_in_type> i_xslvi;
    OutStruct<types_amba::axi4_slave_out_type> o_xslvo;
    InPort i_irq_request;
    OutPort o_ip;

 protected:

    class PlicContextTableType : public RegStructArray<plic_context_type> {
     public:
        PlicContextTableType(GenObject *parent, const char *name, const char *comment)
            : RegStructArray<plic_context_type>(parent, name, "ctxmax", comment) {
        }
    };

    plic_context_type plic_context_type_def_;
    Signal w_req_valid;
    Signal wb_req_addr;
    Signal wb_req_size;
    Signal w_req_write;
    Signal wb_req_wdata;
    Signal wb_req_wstrb;
    Signal w_req_last;
    Signal w_req_ready;
    Signal w_resp_valid;
    Signal wb_resp_rdata;
    Signal wb_resp_err;

    RegSignal src_priority;
    RegSignal pending;
    RegSignal ip;
    PlicContextTableType ctx;
    RegSignal rdata;

    CombProcess comb;

    axi_slv xslv0;
};

class plic_file : public FileObject {
 public:
    plic_file(GenObject *parent) :
        FileObject(parent, "plic"),
        plic_(this, "plic") {}

 private:
    plic plic_;
};

