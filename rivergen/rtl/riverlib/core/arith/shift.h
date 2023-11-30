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

class Shifter : public ModuleObject {
 public:
    Shifter(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            wb_sll(this, "wb_sll", "64"),
            wb_sllw(this, "wb_sllw", "64"),
            wb_srl(this, "wb_srl", "64"),
            wb_sra(this, "wb_sra", "64"),
            wb_srlw(this, "wb_srlw", "64"),
            wb_sraw(this, "wb_sraw", "64"),
            v64(this, "v64", "64"),
            v32(this, "v32", "32"),
            msk64(this, "msk64", "64"),
            msk32(this, "msk32", "64")
        {
        }

     public:
        Logic wb_sll;
        Logic wb_sllw;
        Logic wb_srl;
        Logic wb_sra;
        Logic wb_srlw;
        Logic wb_sraw;
        Logic v64;
        Logic v32;
        Logic msk64;
        Logic msk32;
    };

    void proc_comb();

 public:
    InPort i_clk;
    InPort i_nrst;
    InPort i_mode;
    InPort i_a1;
    InPort i_a2;
    OutPort o_res;

 protected:
    RegSignal res;

    // process should be intialized last to make all signals available
    CombProcess comb;
};

class shift_file : public FileObject {
 public:
    shift_file(GenObject *parent) : FileObject(parent, "shift"),
    m_(this, "") {}

 private:
    Shifter m_;
};

