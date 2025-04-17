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

class ram_dp_fifo_tech : public ModuleObject {
 public:
    ram_dp_fifo_tech(GenObject *parent, const char *name, const char *comment);

    virtual bool isVcd() override { return false; }     // disable tracing

    class WrRegistersProcess : public ProcObject {
     public:
        WrRegistersProcess(GenObject *parent, GenObject *clk) :
            ProcObject(parent, "wr", clk, CLK_POSEDGE, 0, ACTIVE_NONE, NO_COMMENT) {
        }
    };

    class RdRegistersProcess : public ProcObject {
     public:
        RdRegistersProcess(GenObject *parent, GenObject *clk) :
            ProcObject(parent, "rd", clk, CLK_POSEDGE, 0, ACTIVE_NONE, NO_COMMENT) {
        }
    };

    void r1egisters();
    void r2egisters();

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
    WrRegistersProcess wproc;
    RdRegistersProcess rproc;
};

class ram_dp_fifo_tech_file : public FileObject {
 public:
    ram_dp_fifo_tech_file(GenObject *parent) :
        FileObject(parent, "ram_dp_fifo_tech"),
        ram_dp_fifo_tech_(this, "ram_dp_fifo_tech", NO_COMMENT) {}

 private:
    ram_dp_fifo_tech ram_dp_fifo_tech_;
};

