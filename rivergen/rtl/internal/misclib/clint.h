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

class clint : public ModuleObject {
 public:
    clint(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

 protected:
    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            vrdata(this, "vrdata", "CFG_SYSBUS_DATA_BITS", "'0", NO_COMMENT),
            vb_msip(this, "vb_msip", "cpu_total", "'0", NO_COMMENT),
            vb_mtip(this, "vb_mtip", "cpu_total", "'0", NO_COMMENT),
            regidx(this, "regidx", "0", NO_COMMENT) {
        }

     public:
        Logic vrdata;
        Logic vb_msip;
        Logic vb_mtip;
        I32D regidx;
    };

    void proc_comb();

 public:
    TmplParamI32D cpu_total;
    // io:
    InPort i_clk;
    InPort i_nrst;
    InStruct<types_amba::mapinfo_type> i_mapinfo;
    OutStruct<types_pnp::dev_config_type> o_cfg;
    InStruct<types_amba::axi4_slave_in_type> i_xslvi;
    OutStruct<types_amba::axi4_slave_out_type> o_xslvo;
    OutPort o_mtimer;
    OutPort o_msip;
    OutPort o_mtip;

 protected:
    class clint_cpu_type : public StructObject {
     public:
        // Structure definition
        clint_cpu_type(GenObject *parent,
                       GenObject *clk,
                       EClockEdge edge,
                       GenObject *nrst,
                       EResetActive active,
                       const char *name,
                       const char *rstval,
                       const char *comment)
            : StructObject(parent, clk, edge, nrst, active, "clint_cpu_type", name, rstval, comment),
            msip(this, clk, edge, nrst, active, "msip", "1", RSTVAL_ZERO, NO_COMMENT),
            mtip(this, clk, edge, nrst, active, "mtip", "1", RSTVAL_ZERO, NO_COMMENT),
            mtimecmp(this, clk, edge, nrst, active, "mtimecmp", "64", RSTVAL_ZERO, NO_COMMENT) {}

        clint_cpu_type(GenObject *parent,
                       const char *name,
                       const char *comment)
            : clint_cpu_type(parent, 0, CLK_ALWAYS, 0, ACTIVE_NONE, 
                             name, RSTVAL_NONE, comment) {}
     public:
        RegSignal msip;
        RegSignal mtip;
        RegSignal mtimecmp;
    };


    class ClintCpuTableType : public ValueArray<clint_cpu_type> {
     public:
        ClintCpuTableType(GenObject *parent,
                          Logic *clk,
                          Logic *rstn,
                          const char *name,
                          const char *comment)
            : ValueArray<clint_cpu_type>(parent, clk, CLK_POSEDGE,
                        rstn, ACTIVE_LOW, name, "cpu_total", RSTVAL_NONE, comment) {}
    };

    clint_cpu_type clint_cpu_type_def_;
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

    RegSignal mtime;
    ClintCpuTableType hart;
    RegSignal rdata;
    RegSignal resp_valid;

    CombProcess comb;

    axi_slv xslv0;
};

class clint_file : public FileObject {
 public:
    clint_file(GenObject *parent) :
        FileObject(parent, "clint"),
        clint_(this, "clint") {}

 private:
    clint clint_;
};

