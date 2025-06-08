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
#include "../clk/vip_clk.h"
#include "vip_jtag_tap.h"

using namespace sysvc;

class jtag_app : public ModuleObject {
 public:
    jtag_app(GenObject *parent, const char *name);

    virtual bool isTop() override { return true; }
    virtual bool isAsyncResetParam() override { return false; }

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb") {
        }

     public:
    };

    // Clock1
    class TestClk1Process : public ProcObject {
     public:
        TestClk1Process(GenObject *parent, GenObject *clk) :
            ProcObject(parent, "test_clk1", clk, CLK_POSEDGE, 0, ACTIVE_NONE, NO_COMMENT) {
        }
     public:
    };

    void proc_comb();
    void proc_test_clk1();

public:
    // Ports:
    OutPort o_trst;
    OutPort o_tck;
    OutPort o_tms;
    OutPort o_tdo;
    InPort i_tdi;

    Signal w_nrst;
    Signal w_tck;
    Logic wb_clk1_cnt;
    Signal w_req_valid;
    Signal wb_req_irlen;
    Signal wb_req_ir;
    Signal wb_req_drlen;
    Signal wb_req_dr;
    Signal w_resp_valid;
    Signal wb_resp_data;


    // Sub-module instances:
    vip_clk clk1;

    CombProcess comb;
    TestClk1Process test_clk1;
    vip_jtag_tap tap;
};

class jtag_app_file : public FileObject {
 public:
    jtag_app_file(GenObject *parent) :
        FileObject(parent, "jtag_app"),
        jtag_app_(this, "jtag_app") { }

 private:
    jtag_app jtag_app_;
};

