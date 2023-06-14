// 
//  Copyright 2023 Sergey Khabarov, sergeykhbr@gmail.com
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

#include <api.h>

using namespace sysvc;

class vip_sdcard_top : public ModuleObject {
 public:
    vip_sdcard_top(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb") {
        }

     public:
    };

    void proc_comb();

 public:
    DefParamI32D half_period;
    // io:
    InPort i_rstn;
    InPort i_rx;

    Signal w_clk;
    Signal wb_rdata;

    CombProcess comb;
};

class vip_sdcard_top_file : public FileObject {
 public:
    vip_sdcard_top_file(GenObject *parent) :
        FileObject(parent, "vip_sdcard_top"),
        vip_sdcard_top_(this, "") {}

 private:
    vip_sdcard_top vip_sdcard_top_;
};

