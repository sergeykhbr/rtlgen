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
#include "../mathlib/mul_4x4_i8.h"

using namespace sysvc;

class rgb2ycbcr : public ModuleObject {
 public:
    rgb2ycbcr(GenObject *parent, const char *name, const char *comment);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            vb_m00(this, "vb_m00", "8", "0x21", "0.257 * 128"),
            vb_m01(this, "vb_m01", "8", "0x41", "0.504"),
            vb_m02(this, "vb_m02", "8", "0x0d", "0.0979"),
            vb_m03(this, "vb_m03", "8", "0x08", "0.0627"),
            vb_m10(this, "vb_m10", "8", "0xee", "-0.148"),
            vb_m11(this, "vb_m11", "8", "0xdc", "-0.291"),
            vb_m12(this, "vb_m12", "8", "0x38", "0.439"),
            vb_m13(this, "vb_m13", "8", "0x40", "0.502"),
            vb_m20(this, "vb_m20", "8", "0x38", "0.439"),
            vb_m21(this, "vb_m21", "8", "0xd2", "-0.368"),
            vb_m22(this, "vb_m22", "8", "0xf8", "-0.0714"),
            vb_m23(this, "vb_m23", "8", "0x40", "0.502"),
            vb_m30(this, "vb_m30", "8", "0", NO_COMMENT),
            vb_m31(this, "vb_m31", "8", "0", NO_COMMENT),
            vb_m32(this, "vb_m32", "8", "0", NO_COMMENT),
            vb_m33(this, "vb_m33", "8", "1", NO_COMMENT) {
        }
     public:
        Logic vb_m00;
        Logic vb_m01;
        Logic vb_m02;
        Logic vb_m03;
        Logic vb_m10;
        Logic vb_m11;
        Logic vb_m12;
        Logic vb_m13;
        Logic vb_m20;
        Logic vb_m21;
        Logic vb_m22;
        Logic vb_m23;
        Logic vb_m30;
        Logic vb_m31;
        Logic vb_m32;
        Logic vb_m33;
    };

    void proc_comb();

 public:
    // io:
    InPort i_nrst;
    InPort i_clk;
    InPort i_rgb565;
    InPort i_hsync;
    InPort i_vsync;
    InPort i_de;
    OutPort o_ycbcr422;
    OutPort o_hsync;
    OutPort o_vsync;
    OutPort o_de;

    Signal w_signed;
    Signal wb_v0;
    Signal wb_v1;
    Signal wb_v2;
    Signal wb_v3;
    Signal wb_m;
    Signal wb_res0;
    Signal wb_res1;
    Signal wb_res2;
    Signal wb_res3;

    RegSignal hsync;
    RegSignal vsync;
    RegSignal de;
    RegSignal even;         // HIGH, whn 0, 2, 4, ...
    RegSignal ycbcr422;

    mul_4x4_i8 im8;
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

