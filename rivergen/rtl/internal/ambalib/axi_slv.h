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
#include "types_amba.h"
#include "types_pnp.h"

using namespace sysvc;

class axi_slv : public ModuleObject {
 public:
    axi_slv(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            vb_req_addr_next(this, "vb_req_addr_next", "12", "'0", NO_COMMENT),
            vb_req_len_next(this, "vb_req_len_next", "8", "'0", NO_COMMENT),
            vcfg(this, "vcfg", "dev_config_none", NO_COMMENT),
            vxslvo(this, "vxslvo", "axi4_slave_out_none", NO_COMMENT) {
        }

     public:
        Logic vb_req_addr_next;
        Logic vb_req_len_next;
        StructVar<types_pnp::dev_config_type> vcfg;
        StructVar<types_amba::axi4_slave_out_type> vxslvo;
    };

    void proc_comb();

 public:
    DefParamUI32D vid;
    DefParamUI32D did;
    // io:
    InPort i_clk;
    InPort i_nrst;
    InStruct<types_amba::mapinfo_type> i_mapinfo;
    OutStruct<types_pnp::dev_config_type> o_cfg;
    InStruct<types_amba::axi4_slave_in_type> i_xslvi;
    OutStruct<types_amba::axi4_slave_out_type> o_xslvo;
    OutPort o_req_valid;
    OutPort o_req_addr;
    OutPort o_req_size;
    OutPort o_req_write;
    OutPort o_req_wdata;
    OutPort o_req_wstrb;
    OutPort o_req_last;
    InPort i_req_ready;
    InPort i_resp_valid;
    InPort i_resp_rdata;
    InPort i_resp_err;

    ParamLogic State_Idle;
    ParamLogic State_w;
    ParamLogic State_burst_w;
    ParamLogic State_addr_r;
    ParamLogic State_data_r;
    ParamLogic State_out_r;
    ParamLogic State_b;

    RegSignal state;
    RegSignal req_valid;
    RegSignal req_addr;
    RegSignal req_write;
    RegSignal req_wdata;
    RegSignal req_wstrb;
    RegSignal req_xsize;
    RegSignal req_len;
    RegSignal1 req_user;
    RegSignal req_id;
    RegSignal req_burst;
    RegSignal req_last_a;
    RegSignal req_last_r;
    RegSignal req_done;
    RegSignal resp_valid;
    RegSignal resp_last;
    RegSignal resp_rdata;
    RegSignal resp_err;

    CombProcess comb;
};

class axi_slv_file : public FileObject {
 public:
    axi_slv_file(GenObject *parent) :
        FileObject(parent, "axi_slv"),
        axi_slv_(this, "axi_slv") {}

 private:
    axi_slv axi_slv_;
};

