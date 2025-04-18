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

class apb_slv : public ModuleObject {
 public:
    apb_slv(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            vb_rdata(this, "vb_rdata", "32", "'0", NO_COMMENT),
            vcfg(this, "vcfg", "dev_config_none", NO_COMMENT),
            vapbo(this, "vapbo", "apb_out_none", NO_COMMENT) {
        }

     public:
        Logic vb_rdata;
        StructVar<types_pnp::dev_config_type> vcfg;
        StructVar<types_amba::apb_out_type> vapbo;
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
    InStruct<types_amba::apb_in_type> i_apbi;
    OutStruct<types_amba::apb_out_type> o_apbo;
    OutPort o_req_valid;
    OutPort o_req_addr;
    OutPort o_req_write;
    OutPort o_req_wdata;
    InPort i_resp_valid;
    InPort i_resp_rdata;
    InPort i_resp_err;

    ParamLogic State_Idle;
    ParamLogic State_Request;
    ParamLogic State_WaitResp;
    ParamLogic State_Resp;

    RegSignal state;
    RegSignal req_valid;
    RegSignal req_addr;
    RegSignal req_write;
    RegSignal req_wdata;
    RegSignal resp_valid;
    RegSignal resp_rdata;
    RegSignal resp_err;

    CombProcess comb;
};

class apb_slv_file : public FileObject {
 public:
    apb_slv_file(GenObject *parent) :
        FileObject(parent, "apb_slv"),
        apb_slv_(this, "apb_slv") {}

 private:
    apb_slv apb_slv_;
};

