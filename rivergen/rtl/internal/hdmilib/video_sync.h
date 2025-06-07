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

class video_sync : public ModuleObject {
 public:
    video_sync(GenObject *parent, const char *name, const char *comment);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb") {
        }

     public:
    };

    void proc_comb();

 public:
    DefParamI32D H_ACTIVE;
    DefParamI32D H_FRONT;
    DefParamI32D H_SYNC;
    DefParamI32D H_BACK;
    DefParamI32D V_ACTIVE;
    DefParamI32D V_FRONT;
    DefParamI32D V_SYNC;
    DefParamI32D V_BACK;
    ParamI32D H_TOTAL;
    ParamI32D V_TOTAL;
    // io:
    InPort i_nrst;
    InPort i_clk;
    OutPort o_hsync;
    OutPort o_vsync;
    OutPort o_de;
    OutPort o_x;
    OutPort o_y;
    OutPort o_xy_total;

    RegSignal h_count;
    RegSignal v_count;
    RegSignal h_sync;
    RegSignal v_sync;
    RegSignal de;
    RegSignal x_pix;
    RegSignal y_pix;

    CombProcess comb;
};

class video_sync_file : public FileObject {
 public:
    video_sync_file(GenObject *parent) :
        FileObject(parent, "video_sync"),
        video_sync_(this, "video_sync", NO_COMMENT) {}

 private:
    video_sync video_sync_;
};

