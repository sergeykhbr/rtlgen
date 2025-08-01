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
#include <genconfig.h>
#include "../fadd_generic.h"
#include "../../../sim/pll/pll_generic.h"

using namespace sysvc;

class fadd_tb : public ModuleObject {
 public:
    fadd_tb(GenObject *parent, const char *name);

    virtual bool isTop() override { return true; }
    virtual bool isAsyncResetParam() override { return false; }
    virtual bool isTestBench() override { return true; }

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb") {
        }
     public:
    };

    // Clock1
    class TestClkProcess : public ProcObject {
     public:
        TestClkProcess(GenObject *parent, GenObject *clk) :
            ProcObject(parent, "test_clk", clk, CLK_POSEDGE, 0, ACTIVE_NONE, NO_COMMENT) {
        }
     public:
    };

    void proc_comb();
    void proc_test_clk();

public:
    ParamI32D latency;
    // Ports:
    Signal nrst;
    Signal clk;
    Signal w_ena;
    Signal wb_a;
    Signal wb_b;
    Signal wb_res;
    Signal w_valid;
    Signal w_ex;
    Signal w_compare_ena;
    Signal wb_compare_a;
    Signal w_show_result;

    // regs
    RegSignal clk_cnt;
    RegSignal compare_cnt;
    RegSignal err_cnt;
    WireArray<RegSignal> compare_a;

    // Sub-module instances:
    pll_generic pll0;
    fadd_generic tt;

    CombProcess comb;
    TestClkProcess test_clk;
};

class fadd_tb_file : public FileObject {
 public:
    fadd_tb_file(GenObject *parent) :
        FileObject(parent, "fadd_tb"),
        fadd_tb_(this, "fadd_tb") { }

 private:
    fadd_tb fadd_tb_;
};

