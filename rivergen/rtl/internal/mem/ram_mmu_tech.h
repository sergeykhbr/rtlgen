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

using namespace sysvc;

class ram_mmu_tech : public ModuleObject {
 public:
    ram_mmu_tech(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    virtual bool isVcd() override { return false; }     // disable tracing
    virtual bool isMemory() override { return true; }
    virtual int  getMemoryAddrWidth() override { return static_cast<int>(abits.getValue()); }
    virtual int  getMemoryDataWidth() override { return static_cast<int>(dbits.getValue()); }

    class RegistersProcess : public ProcObject {
     public:
        RegistersProcess(GenObject *parent, GenObject *clk) :
            ProcObject(parent, "registers", clk, CLK_POSEDGE, 0, ACTIVE_NONE, NO_COMMENT) {
        }
    };

    void registers();

 public:
    TmplParamI32D abits;
    TmplParamI32D dbits;

    InPort i_clk;
    InPort i_addr;
    InPort i_wena;
    InPort i_wdata;
    OutPort o_rdata;

    ParamI32D DEPTH;

    Logic rdata;
    LogicMemory mem;

    // process should be intialized last to make all signals available
    RegistersProcess rproc;
};

class ram_mmu_tech_file : public FileObject {
 public:
    ram_mmu_tech_file(GenObject *parent) :
        FileObject(parent, "ram_mmu_tech"),
        ram_mmu_tech_(this, "ram_mmu_tech") {}

 private:
    ram_mmu_tech ram_mmu_tech_;
};

