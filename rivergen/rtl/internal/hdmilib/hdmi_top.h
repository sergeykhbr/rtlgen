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
    InPort i_clk;
    InPort i_nrst;
    InPort i_we;
    InPort i_wdata;
    InPort i_re;
    OutPort o_rdata;
    OutPort o_count;


    RegSignal wr_cnt;
    RegSignal rd_cnt;
    RegSignal total_cnt;

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

