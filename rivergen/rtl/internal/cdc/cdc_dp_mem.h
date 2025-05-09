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

class cdc_dp_mem : public ModuleObject {
 public:
    cdc_dp_mem(GenObject *parent, const char *name, const char *comment);

    virtual bool isAsyncResetParam() override { return false; }
    virtual bool isVcd() override { return false; }     // disable tracing

    void proc_wproc();
    void proc_rproc();

 public:
    TmplParamI32D abits;
    TmplParamI32D dbits;

    InPort i_wclk;
    InPort i_wena;
    InPort i_waddr;
    InPort i_wdata;
    InPort i_rclk;
    InPort i_raddr;
    OutPort o_rdata;

    ParamI32D DEPTH;

    LogicMemory mem;

    // process should be intialized last to make all signals available
    ProcObject wproc;
    ProcObject rproc;
};

class cdc_dp_mem_file : public FileObject {
 public:
    cdc_dp_mem_file(GenObject *parent) :
        FileObject(parent, "cdc_dp_mem"),
        cdc_dp_mem_(this, "cdc_dp_mem", NO_COMMENT) {}

 private:
    cdc_dp_mem cdc_dp_mem_;
};

