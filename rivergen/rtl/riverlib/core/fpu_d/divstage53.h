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

class divstage53 : public ModuleObject {
 public:
    divstage53(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            wb_bits(this, "wb_bits", "8", "'0", NO_COMMENT),
            wb_divx3(this, "wb_divx3", "55", "0", "width 53+2"),
            wb_divx2(this, "wb_divx2", "55", "0", "width 53+2"),
            wb_muxind(this, "wb_muxind", "7", "'0", NO_COMMENT),
            w_muxind_rdy(this, "w_muxind_rdy", "1") {
        }

     public:
        Logic wb_bits;
        Logic wb_divx3;
        Logic wb_divx2;
        Logic wb_muxind;
        Logic w_muxind_rdy;
    };

    void proc_comb();

 public:
    InPort i_mux_ena;
    InPort i_muxind;
    InPort i_divident;
    InPort i_divisor;
    OutPort o_dif;
    OutPort o_bits;
    OutPort o_muxind;
    OutPort o_muxind_rdy;

 protected:
    WireArray<Logic> wb_thresh;
    WireArray<Logic> wb_dif;

    // process should be intialized last to make all signals available
    CombProcess comb;
};

class divstage53_file : public FileObject {
 public:
    divstage53_file(GenObject *parent) : FileObject(parent, "divstage53"),
    divstage53_(this, "divstage53") {}

 private:
    divstage53 divstage53_;
};

