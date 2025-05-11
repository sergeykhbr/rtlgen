// 
//  Copyright 2024 Sergey Khabarov, sergeykhbr@gmail.com
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
#include "cdc_dp_mem.h"
#include "cdc_afifo_gray.h"

using namespace sysvc;

class cdc_afifo : public ModuleObject {
 public:
    cdc_afifo(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    virtual bool isAsyncResetParam() override { return false; }

 protected:
    void proc_comb();
    void proc_wff();
    void proc_rff();

 public:
     TmplParamI32D abits;
     TmplParamI32D dbits;
     InPort i_nrst;
     InPort i_wclk;
     InPort i_wr;
     InPort i_wdata;
     OutPort o_wready;
     InPort i_rclk;
     InPort i_rd;
     OutPort o_rdata;
     OutPort o_rvalid;

 private:
    Signal w_wr_ena;
    Signal wb_wgray_addr;
    Signal wb_wgray;
    Logic q1_wgray;
    Signal q2_wgray;
    Signal w_wgray_full;
    Signal w_wgray_empty_unused;

    Signal w_rd_ena;
    Signal wb_rgray_addr;
    Signal wb_rgray;
    Logic q1_rgray;
    Signal q2_rgray;
    Signal w_rgray_full_unused;
    Signal w_rgray_empty;

    cdc_dp_mem mem0;
    cdc_afifo_gray wgray0;
    cdc_afifo_gray rgray0;

    ProcObject comb;
    ProcObject wff;
    ProcObject rff;
};

class cdc_afifo_file : public FileObject {
 public:
    cdc_afifo_file(GenObject *parent) :
        FileObject(parent, "cdc_afifo"),
        cdc_afifo_(this, "cdc_afifo") {}

 private:
    cdc_afifo cdc_afifo_;
};

