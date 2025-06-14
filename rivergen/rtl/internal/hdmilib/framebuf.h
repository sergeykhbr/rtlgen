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
#include "../../sim/mem/ram_tech.h"

using namespace sysvc;

class framebuf : public ModuleObject {
 public:
    framebuf(GenObject *parent, const char *name, const char *comment);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            v_rd_ena(this, "v_rd_ena", "1", RSTVAL_ZERO, NO_COMMENT),
            vb_ring_rdata(this, "vb_ring_rdata", "64", "'0", NO_COMMENT),
            vb_pix(this, "vb_pix", "16", "'0", NO_COMMENT) {
        }
     public:
        Logic v_rd_ena;
        Logic vb_ring_rdata;
        Logic vb_pix;
    };

    void proc_comb();

 public:
    // io:
    InPort i_nrst;
    InPort i_clk;
    InPort i_hsync;
    InPort i_vsync;
    InPort i_de;
    InPort i_width_m1;
    InPort i_height_m1;
    OutPort o_hsync;
    OutPort o_vsync;
    OutPort o_de;
    OutPort o_rgb565;
    TextLine _dma0_;
    InPort i_req_2d_ready;
    OutPort o_req_2d_valid;
    OutPort o_req_2d_bytes;
    OutPort o_req_2d_addr;
    InPort i_resp_2d_valid;
    InPort i_resp_2d_last;
    InPort i_resp_2d_addr;
    InPort i_resp_2d_data;
    OutPort o_resp_2d_ready;

    TextLine _state0_;
    ParamLogic STATE_Idle;
    ParamLogic STATE_Request;
    ParamLogic STATE_Writing;
    ParamLogic STATE_EndOfFrame;

    Signal wb_ring0_addr;
    Signal w_ring0_wena;
    Signal wb_ring0_rdata;
    Signal wb_ring1_addr;
    Signal w_ring1_wena;
    Signal wb_ring1_rdata;
    Signal wb_ring2_addr;
    Signal w_ring2_wena;
    Signal wb_ring2_rdata;
    Signal wb_ring3_addr;
    Signal w_ring3_wena;
    Signal wb_ring3_rdata;

    RegSignal wr_row;
    RegSignal wr_col;
    RegSignal wr_addr;
    RegSignal rd_row;
    RegSignal rd_col;
    RegSignal rd_addr;
    RegSignal mux_ena;
    RegSignal ring_sel;
    RegSignal pix_sel;
    RegSignal difcnt;

    RegSignal state;
    RegSignal req_addr;
    RegSignal req_valid;
    RegSignal resp_ready;
    RegSignal h_sync;
    RegSignal v_sync;
    RegSignal de;
    RegSignal rgb;

    ram_tech ring0;
    ram_tech ring1;
    ram_tech ring2;
    ram_tech ring3;
    CombProcess comb;
};

class framebuf_file : public FileObject {
 public:
    framebuf_file(GenObject *parent) :
        FileObject(parent, "framebuf"),
        framebuf_(this, "framebuf", NO_COMMENT) {}

 private:
    framebuf framebuf_;
};

