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

using namespace sysvc;

class ic_axi4_to_l1 : public ModuleObject {
 public:
    ic_axi4_to_l1(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject* parent)
            : CombinationalProcess(parent, "comb"),
            vb_xmsti(this, "vb_xmsti", "axi4_master_in_none", NO_COMMENT),
            vb_l1o(this, "vb_l1o", "axi4_l1_out_none", NO_COMMENT),
            idx(this, "idx", "SUB(CFG_LOG2_L1CACHE_BYTES_PER_LINE,3)", "0", "request always 64 bits"),
            vb_req_xbytes(this, "vb_req_xbytes", "XSIZE_TOTAL", "'0", NO_COMMENT),
            vb_req_mask(this, "vb_req_mask", "64", "'0", NO_COMMENT),
            vb_r_data_modified(this, "vb_r_data_modified", "L1CACHE_LINE_BITS", "'0", NO_COMMENT),
            vb_line_wstrb(this, "vb_line_wstrb", "L1CACHE_BYTES_PER_LINE", "'0", NO_COMMENT),
            vb_resp_data(this, "vb_resp_data", "64", "'0", NO_COMMENT),
            t_req_addr(this, "t_req_addr", "CFG_SYSBUS_ADDR_BITS", "'0", NO_COMMENT) {
        }
     public:
        StructVar<types_amba::axi4_master_in_type> vb_xmsti;
        StructVar<types_river::axi4_l1_out_type> vb_l1o;
        Logic idx;
        Logic vb_req_xbytes;
        Logic vb_req_mask;
        Logic vb_r_data_modified;
        Logic vb_line_wstrb;
        Logic vb_resp_data;
        Logic t_req_addr;
    };

    void proc_comb();

public:
    // Ports:
    InPort i_clk;
    InPort i_nrst;
    TextLine _axi4_;
    InStruct<types_amba::axi4_master_out_type> i_xmsto;
    OutStruct<types_amba::axi4_master_in_type> o_xmsti;
    TextLine _l1_;
    InStruct<types_river::axi4_l1_in_type> i_l1i;
    OutStruct<types_river::axi4_l1_out_type> o_l1o;

    // params:
    ParamLogic Idle;
    ParamLogic ReadLineRequest;
    ParamLogic WaitReadLineResponse;
    ParamLogic WriteDataAccept;
    ParamLogic WriteLineRequest;
    ParamLogic WaitWriteConfirmResponse;
    ParamLogic WaitWriteAccept;
    ParamLogic WaitReadAccept;
    ParamLogic CheckBurst;

    RegSignal state;
    RegSignal req_addr;
    RegSignal req_id;
    RegSignal1 req_user;
    RegSignal req_wstrb;
    RegSignal req_wdata;
    RegSignal req_len;
    RegSignal req_size;
    RegSignal req_prot;
    RegSignal writing;
    RegSignal read_modify_write;
    RegSignal line_data;
    RegSignal line_wstrb;
    RegSignal resp_data;

    // process
    CombProcess comb;
};

class ic_axi4_to_l1_file : public FileObject {
 public:
    ic_axi4_to_l1_file(GenObject *parent) :
        FileObject(parent, "ic_axi4_to_l1"),
        ic_axi4_to_l1_(this, "ic_axi4_to_l1") { }

 private:
    ic_axi4_to_l1 ic_axi4_to_l1_;
};

