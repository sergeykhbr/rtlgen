// 
//  Copyright 2022 Sergey Khabarov, sergeykhbr@gmail.com
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
#include "../../river_cfg.h"

using namespace sysvc;

class divstage64 : public ModuleObject {
 public:
    divstage64(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            wb_bits(this, "wb_bits", "4"),
            wb_dif(this, "wb_dif", "64"),
            wb_divx1(this, "wb_divx1", "65"),
            wb_divx2(this, "wb_divx2", "65"),
            wb_divx3(this, "wb_divx3", "65"),
            wb_divx4(this, "wb_divx4", "65"),
            wb_divx5(this, "wb_divx5", "65"),
            wb_divx6(this, "wb_divx6", "65"),
            wb_divx7(this, "wb_divx7", "65"),
            wb_divx8(this, "wb_divx8", "65"),
            wb_divx9(this, "wb_divx9", "65"),
            wb_divx10(this, "wb_divx10", "65"),
            wb_divx11(this, "wb_divx11", "65"),
            wb_divx12(this, "wb_divx12", "65"),
            wb_divx13(this, "wb_divx13", "65"),
            wb_divx14(this, "wb_divx14", "65"),
            wb_divx15(this, "wb_divx15", "65"),
            wb_divx16(this, "wb_divx16", "65"),
            wb_divident(this, "wb_divident", "65"),
            wb_divisor(this, "wb_divisor", "124"),
            wb_thresh(this, "wb_thresh", "66", "16") {
        }

     public:
        Logic wb_bits;
        Logic wb_dif;
        Logic wb_divx1;
        Logic wb_divx2;
        Logic wb_divx3;
        Logic wb_divx4;
        Logic wb_divx5;
        Logic wb_divx6;
        Logic wb_divx7;
        Logic wb_divx8;
        Logic wb_divx9;
        Logic wb_divx10;
        Logic wb_divx11;
        Logic wb_divx12;
        Logic wb_divx13;
        Logic wb_divx14;
        Logic wb_divx15;
        Logic wb_divx16;
        Logic wb_divident;
        Logic wb_divisor;
        WireArray<Logic> wb_thresh;
    };

    void proc_comb();

 public:
    InPort i_divident;
    InPort i_divisor;
    OutPort o_resid;
    OutPort o_bits;

 protected:
    // process should be intialized last to make all signals available
    CombProcess comb;
};

class divstage64_file : public FileObject {
 public:
    divstage64_file(GenObject *parent) : FileObject(parent, "divstage64"),
    divstage64_(this, "divstage64") {}

 private:
    divstage64 divstage64_;
};

