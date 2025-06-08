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
#include "../mem/ram_tech.h"

using namespace sysvc;

class framebuf : public ModuleObject {
 public:
    framebuf(GenObject *parent, const char *name, const char *comment);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            vb_raddr_next(this, "vb_raddr_next", "11", RSTVAL_ZERO, NO_COMMENT) {
        }

     public:
        Logic vb_raddr_next;
    };

    void proc_comb();

 public:
    // io:
    InPort i_nrst;
    InPort i_clk;
    InPort i_hsync;
    InPort i_vsync;
    InPort i_de;
    InPort i_x;
    InPort i_y;
    InPort i_xy_total;
    OutPort o_hsync;
    OutPort o_vsync;
    OutPort o_de;
    OutPort o_YCbCr;
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
    ParamLogic STATE_Request;
    ParamLogic STATE_Writing;
    ParamLogic STATE_Idle;

    Signal wb_ping_addr;
    Signal w_ping_wena;
    Signal wb_ping_rdata;
    Signal wb_pong_addr;
    Signal w_pong_wena;
    Signal wb_pong_rdata;

    RegSignal state;
    RegSignal pingpong;
    RegSignal req_addr;
    RegSignal req_valid;
    RegSignal resp_ready;
    RegSignal raddr;
    RegSignal raddr_z;
    RegSignal pix_x0;
    RegSignal h_sync;
    RegSignal v_sync;
    RegSignal de;
    RegSignal Y0;
    RegSignal Y1;
    RegSignal Cb;
    RegSignal Cr;
    RegSignal YCbCr;

    ram_tech ping;
    ram_tech pong;
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

