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
        plic_context_type(GenObject *parent,
                          GenObject *clk,
                          EClockEdge edge,
                          GenObject *nrst,
                          EResetActive active,
                          const char *name,
                          const char *rstval,
                          const char *comment)
            : StructObject(parent, clk, edge, nrst, active, "plic_context_type", name, rstval, comment),
            priority_th(this, clk, edge, nrst, active, "priority_th", "4", RSTVAL_ZERO, NO_COMMENT),
            ie(this, clk, edge, nrst, active, "ie", "1024", RSTVAL_ZERO, "interrupt enable per context"),
            ip_prio(this, clk, edge, nrst, active, "ip_prio", "MUL(4,1024)", RSTVAL_ZERO, "interrupt pending priority per context"),
            prio_mask(this, clk, edge, nrst, active, "prio_mask", "16", RSTVAL_ZERO, "pending interrupts priorites"),
            sel_prio(this, clk, edge, nrst, active, "sel_prio", "4", RSTVAL_ZERO, "the most available priority"),
            irq_idx(this, clk, edge, nrst, active, "irq_idx", "10", RSTVAL_ZERO, "currently selected most prio irq"),
            irq_prio(this, clk, edge, nrst, active, "irq_prio", "10", RSTVAL_ZERO, "currently selected prio level") {}

        plic_context_type(GenObject *parent,
                          const char *name,
                          const char *comment)
            : plic_context_type(parent, 0, CLK_ALWAYS, 0, ACTIVE_NONE,
                                name, RSTVAL_NONE, comment) {}

     public:
        Signal priority_th;
        Signal ie;
        Signal ip_prio;
        Signal prio_mask;
        Signal sel_prio;
        Signal irq_idx;
        Signal irq_prio;
    };

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            vrdata(this, "vrdata", "CFG_SYSBUS_DATA_BITS", "'0", NO_COMMENT),
            vb_irq_idx(this, "vb_irq_idx", "10", "ctxmax", "Currently selected most prio irq"),
            vb_irq_prio(this, "vb_irq_prio", "10", "ctxmax", "Currently selected prio level"),
            t0(this, "SystemC workaround: duplicate context structure to be able the bit assignment"),
            vb_ctx_priority_th(this, 0, CLK_ALWAYS, 0, ACTIVE_NONE, "vb_ctx_priority_th", "4", "ctxmax", RSTVAL_ZERO, NO_COMMENT),
            vb_ctx_ie(this, 0, CLK_ALWAYS, 0, ACTIVE_NONE, "vb_ctx_ie", "1024", "ctxmax", RSTVAL_ZERO, NO_COMMENT),
            vb_ctx_ip_prio(this, 0, CLK_ALWAYS, 0, ACTIVE_NONE, "vb_ctx_ip_prio", "MUL(4,1024)", "ctxmax", RSTVAL_ZERO, NO_COMMENT),
            vb_ctx_prio_mask(this, 0, CLK_ALWAYS, 0, ACTIVE_NONE, "vb_ctx_prio_mask", "16", "ctxmax", RSTVAL_ZERO, NO_COMMENT),
            vb_ctx_sel_prio(this, 0, CLK_ALWAYS, 0, ACTIVE_NONE, "vb_ctx_sel_prio", "4", "ctxmax", RSTVAL_ZERO, NO_COMMENT),
            vb_ctx_irq_idx(this, 0, CLK_ALWAYS, 0, ACTIVE_NONE, "vb_ctx_irq_idx", "10", "ctxmax", RSTVAL_ZERO, NO_COMMENT),
            vb_ctx_irq_prio(this, 0, CLK_ALWAYS, 0, ACTIVE_NONE, "vb_ctx_irq_prio", "10", "ctxmax", RSTVAL_ZERO, NO_COMMENT),
            vb_src_priority(this, "vb_src_priority", "MUL(4,1024)"),
            vb_pending(this, "vb_pending", "1024"),
            vb_ip(this, "vb_ip", "ctxmax", "'0", NO_COMMENT),
            rctx_idx(this, "rctx_idx", "0", NO_COMMENT) {
        }

     public:
        Logic vrdata;
        WireArray<Logic> vb_irq_idx;
        WireArray<Logic> vb_irq_prio;
        TextLine t0;
        WireArray<Logic> vb_ctx_priority_th;
        WireArray<Logic> vb_ctx_ie;
        WireArray<Logic> vb_ctx_ip_prio;
        WireArray<Logic> vb_ctx_prio_mask;
        WireArray<Logic> vb_ctx_sel_prio;
        WireArray<Logic> vb_ctx_irq_idx;
        WireArray<Logic> vb_ctx_irq_prio;
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

    class PlicContextTableType : public ValueArray<plic_context_type> {
     public:
        PlicContextTableType(GenObject *parent,
                             Logic *clk,
                             Logic *rstn,
                             const char *name,
                             const char *comment)
            : ValueArray<plic_context_type>(parent, clk, CLK_POSEDGE,
                                            rstn, ACTIVE_LOW, name, "ctxmax", "", comment) {}
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

