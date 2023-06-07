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

#include <api.h>
#include "../ambalib/types_amba.h"
#include "../ambalib/axi_slv.h"

using namespace sysvc;

class clint : public ModuleObject {
 public:
    clint(GenObject *parent, const char *name);

 protected:
    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            vrdata(this, "vrdata", "CFG_SYSBUS_DATA_BITS"),
            regidx("0", "regidx", this) {
        }

     public:
        Logic vrdata;
        I32D regidx;
    };

    void proc_comb();

 public:
    TmplParamI32D cpu_total;
    // io:
    InPort i_clk;
    InPort i_nrst;
    InStruct<types_amba::mapinfo_type> i_mapinfo;
    OutStruct<types_amba::dev_config_type> o_cfg;
    InStruct<types_amba::axi4_slave_in_type> i_xslvi;
    OutStruct<types_amba::axi4_slave_out_type> o_xslvo;
    OutPort o_mtimer;
    OutPort o_msip;
    OutPort o_mtip;

 protected:

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

    RegSignal msip;
    RegSignal mtip;
    RegSignal mtime;
    RegSignal mtimecmp;
    RegSignal rdata;

    CombProcess comb;

    axi_slv xslv0;
};

class clint_file : public FileObject {
 public:
    clint_file(GenObject *parent) :
        FileObject(parent, "clint"),
        clint_(this, "") {}

 private:
    clint clint_;
};

