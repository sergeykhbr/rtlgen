// 
//  Copyright 2023 Sergey Khabarov, sergeykhbr@gmail.com
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

class SysPLL_tech : public ModuleObject {
 public:
    SysPLL_tech(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    virtual bool isAsyncResetParam() override { return false; }     // do not generate async_reset generic parameter

 protected:
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
    InPort i_reset;
    InPort i_clk_tcxo;
    OutPort o_clk_sys;
    OutPort o_clk_ddr;
    OutPort o_clk_pcie;
    OutPort o_locked;

 private:
    CombProcess comb;
};

class SysPLL_tech_file : public FileObject {
 public:
    SysPLL_tech_file(GenObject *parent) :
        FileObject(parent, "SysPLL_tech"),
        SysPLL_tech_(this, "SysPLL_tech") {}

 private:
    SysPLL_tech SysPLL_tech_;
};

