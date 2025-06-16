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
#include "../ambalib/types_amba.h"
#include "cdc_afifo.h"

using namespace sysvc;

class afifo_xmst : public ModuleObject {
 public:
    afifo_xmst(GenObject *parent, const char *name, const char *comment);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject* parent)
            : CombinationalProcess(parent, "comb"),
            vb_xmsti(this, "vb_xmsti", NO_COMMENT),
            vb_xslvi(this, "vb_xslvi", NO_COMMENT) {
        }
     public:
        types_amba::axi4_master_in_type vb_xmsti;
        types_amba::axi4_master_out_type vb_xslvi;
    };

    void proc_comb();

public:
    TmplParamI32D abits_depth;
    TmplParamI32D dbits_depth;
    // Ports:
    InPort i_xmst_nrst;
    InPort i_xmst_clk;
    InStruct<types_amba::axi4_master_out_type> i_xmsto;
    OutStruct<types_amba::axi4_master_in_type> o_xmsti;
    InPort i_xslv_nrst;
    InPort i_xslv_clk;
    OutStruct<types_amba::axi4_master_out_type> o_xslvi;
    InStruct<types_amba::axi4_master_in_type> i_xslvo;
    
    // Parameters
    ParamI32D AR_REQ_WIDTH;
    ParamI32D AW_REQ_WIDTH;
    ParamI32D W_REQ_WIDTH;
    ParamI32D R_RESP_WIDTH;
    ParamI32D B_RESP_WIDTH;

    // Signals:
    Signal w_req_ar_valid_i;
    Signal wb_req_ar_payload_i;
    Signal w_req_ar_wready_o;
    Signal w_req_ar_rd_i;
    Signal wb_req_ar_payload_o;
    Signal w_req_ar_rvalid_o;
    Signal w_req_aw_valid_i;
    Signal wb_req_aw_payload_i;
    Signal w_req_aw_wready_o;
    Signal w_req_aw_rd_i;
    Signal wb_req_aw_payload_o;
    Signal w_req_aw_rvalid_o;
    Signal w_req_w_valid_i;
    Signal wb_req_w_payload_i;
    Signal w_req_w_wready_o;
    Signal w_req_w_rd_i;
    Signal wb_req_w_payload_o;
    Signal w_req_w_rvalid_o;
    Signal w_resp_r_valid_i;
    Signal wb_resp_r_payload_i;
    Signal w_resp_r_wready_o;
    Signal w_resp_r_rd_i;
    Signal wb_resp_r_payload_o;
    Signal w_resp_r_rvalid_o;
    Signal w_resp_b_valid_i;
    SignalBig wb_resp_b_payload_i;
    Signal w_resp_b_wready_o;
    Signal w_resp_b_rd_i;
    SignalBig wb_resp_b_payload_o;
    Signal w_resp_b_rvalid_o;

    // functions
    // Sub-module instances:
    cdc_afifo req_ar;
    cdc_afifo req_aw;
    cdc_afifo req_w;
    cdc_afifo resp_r;
    cdc_afifo resp_b;
    // process
    CombProcess comb;
};

class afifo_xmst_file : public FileObject {
 public:
    afifo_xmst_file(GenObject *parent) :
        FileObject(parent, "afifo_xmst"),
        afifo_xmst_(this, "afifo_xmst", NO_COMMENT) { }

 private:
    afifo_xmst afifo_xmst_;
};

