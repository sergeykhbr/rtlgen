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
#include "video_sync.h"
#include "framebuf.h"
#include "rgb2ycbcr.h"
#include "../ambalib/types_amba.h"
#include "../ambalib/types_pnp.h"
#include "../ambalib/types_dma.h"
#include "../ambalib/axi_dma.h"

using namespace sysvc;

class hdmi_top : public ModuleObject {
 public:
    hdmi_top(GenObject *parent, const char *name, const char *comment);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            vb_xmst_cfg(this, "vb_xmst_cfg", NO_COMMENT),
            vb_xmsto(this, "vb_xmsto", NO_COMMENT) {
        }

     public:
        types_pnp::dev_config_type vb_xmst_cfg;
        types_amba::axi4_master_out_type vb_xmsto;
    };

    void proc_comb();

 public:
    DefParamLogic WIDTH;
    DefParamLogic HEIGHT;
    // io:
    InPort i_nrst;
    InPort i_clk;
    InPort i_hdmi_nrst;
    InPort i_hdmi_clk;
    OutPort o_hsync;
    OutPort o_vsync;
    OutPort o_de;
    OutPort o_data;
    OutPort o_spdif;
    InPort i_spdif_out;
    InPort i_irq;
    TextLine _text1_;
    OutStruct<types_pnp::dev_config_type> o_xmst_cfg;
    InStruct<types_amba::axi4_master_in_type> i_xmsti;
    OutStruct<types_amba::axi4_master_out_type> o_xmsto;

    Signal wb_width_m1;
    Signal wb_height_m1;
    Signal w_sync_hsync;
    Signal w_sync_vsync;
    Signal w_sync_de;
    Signal w_fb_hsync;
    Signal w_fb_vsync;
    Signal w_fb_de;
    Signal wb_fb_rgb565;
    Signal w_req_mem_ready;
    Signal w_req_mem_valid;
    Signal w_req_mem_write;
    Signal wb_req_mem_bytes;
    Signal wb_req_mem_addr;
    Signal wb_req_mem_strob;
    Signal wb_req_mem_data;
    Signal w_req_mem_last;
    Signal w_resp_mem_valid;
    Signal w_resp_mem_last;
    Signal w_resp_mem_fault_unused;
    Signal wb_resp_mem_addr;
    Signal wb_resp_mem_data;
    Signal w_resp_mem_ready;
    Signal w_dbg_valid_unused;
    Signal wb_dbg_payload_unused;

    video_sync sync0;
    framebuf fb0;
    rgb2ycbcr rgb2y0;
    axi_dma xdma0;

    CombProcess comb;
};

class hdmi_top_file : public FileObject {
 public:
    hdmi_top_file(GenObject *parent) :
        FileObject(parent, "hdmi_top"),
        hdmi_top_(this, "hdmi_top", NO_COMMENT) {}

 private:
    hdmi_top hdmi_top_;
};

