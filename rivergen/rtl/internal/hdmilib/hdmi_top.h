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

using namespace sysvc;

class hdmi_top : public ModuleObject {
 public:
    hdmi_top(GenObject *parent, const char *name, const char *comment);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb") {
        }

     public:
    };

    void proc_comb();

 public:
    // io:
    InPort i_nrst;
    InPort i_clk;
    InPort i_hdmi_clk;
    OutPort o_hsync;
    OutPort o_vsync;
    OutPort o_de;
    OutPort o_data;
    OutPort o_spdif;
    InPort i_spdif_out;
    InPort i_irq;

    Signal w_sync_hsync;
    Signal w_sync_vsync;
    Signal w_sync_de;
    Signal wb_sync_x;
    Signal wb_sync_y;

    video_sync sync0;
    framebuf fb0;
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

