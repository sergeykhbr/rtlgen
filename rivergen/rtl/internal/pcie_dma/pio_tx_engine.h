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

using namespace sysvc;

class pio_tx_engine : public ModuleObject {
 public:
    pio_tx_engine(GenObject *parent, const char *name, const char *comment);

    virtual bool isAsyncResetParam() override { return false; }

 protected:
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
    TmplParamI32D C_DATA_WIDTH;
    TmplParamI32D KEEP_WIDTH;
    // io:
    InPort i_nrst;
    InPort i_clk;
    TextLine _t0_;
    InPort i_s_axis_tx_tready;
    OutPort o_s_axis_tx_tdata;
    OutPort o_s_axis_tx_tkeep;
    OutPort o_s_axis_tx_tlast;
    OutPort o_s_axis_tx_tvalid;
    OutPort o_tx_src_dsc;
    TextLine _t1_;
    InPort i_req_compl;
    InPort i_req_compl_wd;
    OutPort o_compl_done;
    TextLine _t2_;
    InPort i_req_tc;
    InPort i_req_td;
    InPort i_req_ep;
    InPort i_req_attr;
    InPort i_req_len;
    InPort i_req_rid;
    InPort i_req_tag;
    InPort i_req_be;
    InPort i_req_addr;
    TextLine _t3_;
    OutPort o_rd_addr;
    OutPort o_rd_be;
    InPort i_rd_data;
    InPort i_completer_id;

    CombProcess comb;
    ProcObject reqff;
};

class pio_tx_engine_file : public FileObject {
 public:
    pio_tx_engine_file(GenObject *parent) :
        FileObject(parent, "PIO_TX_ENGINE"),
        pio_tx_engine_(this, "PIO_TX_ENGINE", NO_COMMENT) {}

 private:
    pio_tx_engine pio_tx_engine_;
};

