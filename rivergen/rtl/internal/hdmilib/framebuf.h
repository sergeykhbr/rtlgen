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

class framebuf : public ModuleObject {
 public:
    framebuf(GenObject *parent, const char *name, const char *comment);

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
    InPort i_hsync;
    InPort i_vsync;
    InPort i_de;
    InPort i_x;
    InPort i_y;
    OutPort o_hsync;
    OutPort o_vsync;
    OutPort o_de;
    OutPort o_YCbCr;

    RegSignal pix_x0;;
    RegSignal h_sync;
    RegSignal v_sync;
    RegSignal de;
    RegSignal Y0;
    RegSignal Y1;
    RegSignal Cb;
    RegSignal Cr;
    RegSignal YCbCr;

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

