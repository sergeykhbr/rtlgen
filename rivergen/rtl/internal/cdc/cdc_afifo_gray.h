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

class cdc_afifo_gray : public ModuleObject {
 public:
    cdc_afifo_gray(GenObject *parent, const char *name, const char *comment);

    virtual bool isAsyncResetParam() override { return false; }

 protected:
    void proc_comb();
    void proc_ff();

 public:
    TmplParamI32D abits;

    InPort i_nrst;
    InPort i_clk;
    InPort i_ena;
    InPort i_q2_gray;
    OutPort o_addr;
    OutPort o_gray;
    OutPort o_empty;
    OutPort o_full;

    Logic wb_bin_next;
    Logic wb_gray_next;
    Logic bin;
    Logic gray;
    Logic empty;
    Logic full;

    ProcObject comb;
    ProcObject ff;
};

class cdc_afifo_gray_file : public FileObject {
 public:
    cdc_afifo_gray_file(GenObject *parent) :
        FileObject(parent, "cdc_afifo_gray"),
        cdc_afifo_gray_(this, "cdc_afifo_gray", NO_COMMENT) {}

 private:
    cdc_afifo_gray cdc_afifo_gray_;
};

