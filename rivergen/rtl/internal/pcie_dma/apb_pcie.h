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
#include "../ambalib/types_amba.h"
#include "../ambalib/types_dma.h"
#include "../ambalib/types_pnp.h"
#include "../ambalib/apb_slv.h"

using namespace sysvc;

class apb_pcie : public ModuleObject {
 public:
    apb_pcie(GenObject *parent, const char *name, const char *comment);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            vb_rdata(this, "vb_rdata", "32", "'0") {
        }

     public:
        Logic vb_rdata;
    };

    void proc_comb();
    void proc_reqff();

 public:
    // io:
    InPort i_clk;
    InPort i_nrst;
    InStruct<types_amba::mapinfo_type> i_mapinfo;
    OutStruct<types_pnp::dev_config_type> o_cfg;
    InStruct<types_amba::apb_in_type> i_apbi;
    OutStruct<types_amba::apb_out_type> o_apbo;
    InPort i_pcie_completer_id;
    InPort i_dma_state;
    InPort i_dbg_mem_valid;
    InPort i_dbg_mem_wren;
    InPort i_dbg_mem_wstrb;
    InPort i_dbg_mem_addr;
    InPort i_dbg_mem_data;

    Signal w_req_valid;
    Signal wb_req_addr;
    Signal w_req_write;
    Signal wb_req_wdata;

    RegSignal resp_valid;
    RegSignal resp_rdata;
    RegSignal resp_err;
    Logic req_cnt;
    LogicArray req_data_arr;

    CombProcess comb;
    ProcObject reqff;

    apb_slv pslv0;
};

class apb_pcie_file : public FileObject {
 public:
    apb_pcie_file(GenObject *parent) :
        FileObject(parent, "apb_pcie"),
        apb_pcie_(this, "apb_pcie", NO_COMMENT) {}

 private:
    apb_pcie apb_pcie_;
};

