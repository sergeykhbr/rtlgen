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

class rgb2ycbcr : public ModuleObject {
 public:
    rgb2ycbcr(GenObject *parent, const char *name, const char *comment);

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
    InPort i_rgb565;
    OutPort o_y;
    OutPort o_cr;
    OutPort o_cb;

    RegSignal y;
    RegSignal cr;
    RegSignal cb;

    CombProcess comb;
};

class rgb2ycbcr_file : public FileObject {
 public:
    rgb2ycbcr_file(GenObject *parent) :
        FileObject(parent, "rgb2ycbcr"),
        rgb2ycbcr_(this, "rgb2ycbcr", NO_COMMENT) {}

 private:
    rgb2ycbcr rgb2ycbcr_;
};

